#pragma once
// Shared local ordering policy. This header depends only on the C++ standard
// library: neither RuntimeValue, authoring types nor a JSON parser lives here.
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <new>
#include <numeric>
#include <span>
#include <string_view>
#include <utility>
#include <vector>

namespace frog::sdk::collection_order {
inline constexpr std::string_view profile = "frog.studio.collection.order@1";
enum class Kind {
  Boolean,
  Signed,
  Unsigned,
  Real,
  Text,
  Record,
  Array,
  Timestamp,
  Unsupported
};
enum class Order { Less, Equal, Greater };
enum class Error {
  None,
  TypeMismatch,
  Unsupported,
  NonFinite,
  DepthLimit,
  ValueLimit,
  WorkLimit,
  InvalidShape,
  Cancelled,
  AllocationFailure
};
enum class DuplicatePolicy { KeepFirst, KeepLast };
struct Limits {
  unsigned maximumDepth = 32;
  std::size_t maximumValueNodes = 4096;
  std::size_t maximumElements = 4096;
  std::size_t maximumWork = 16'000'000;
};
using Cancelled = bool (*)(void *) noexcept;
struct Result {
  Order order = Order::Equal;
  Error error = Error::None;
  [[nodiscard]] constexpr bool accepted() const noexcept {
    return error == Error::None;
  }
};
struct NormalizationResult {
  // Indices into the original input. Preserve whole values and metadata;
  // do not reconstruct entries from their display text or a sorted JSON form.
  std::vector<std::size_t> indices;
  Error error = Error::None;
  [[nodiscard]] bool accepted() const noexcept { return error == Error::None; }
};
namespace detail {
struct Budget {
  const Limits &limits;
  Cancelled cancelled = nullptr;
  void *context = nullptr;
  std::size_t work = 0;
  Error visit(unsigned depth, std::size_t &nodes) {
    if (depth > limits.maximumDepth)
      return Error::DepthLimit;
    if (++nodes > limits.maximumValueNodes)
      return Error::ValueLimit;
    if (work >= limits.maximumWork)
      return Error::WorkLimit;
    ++work;
    if ((work == 1 || !(work & 255)) && cancelled && cancelled(context))
      return Error::Cancelled;
    return Error::None;
  }
};
template <class TypeView>
Error schema(TypeView value, Budget &budget, unsigned depth,
             std::size_t &nodes) {
  if (const auto error = budget.visit(depth, nodes); error != Error::None)
    return error;
  switch (value.kind()) {
  case Kind::Boolean:
  case Kind::Signed:
  case Kind::Unsigned:
  case Kind::Real:
  case Kind::Text:
  case Kind::Timestamp:
    return Error::None;
  case Kind::Record:
    if (value.childCount() > budget.limits.maximumValueNodes)
      return Error::ValueLimit;
    for (std::size_t i = 0; i < value.childCount(); ++i)
      if (const auto error = schema(value.child(i), budget, depth + 1, nodes);
          error != Error::None)
        return error;
    return Error::None;
  case Kind::Array:
    if (value.rank() < 1 || value.rank() > 16 || value.childCount() != 1)
      return Error::InvalidShape;
    return schema(value.child(0), budget, depth + 1, nodes);
  case Kind::Unsupported:
    return Error::Unsupported;
  }
  return Error::Unsupported;
}
template <class Scalar> Result scalar(const Scalar &a, const Scalar &b) {
  return {a < b   ? Order::Less
          : b < a ? Order::Greater
                  : Order::Equal,
          Error::None};
}
template <class View>
Result value(View a, View b, Budget &budget, unsigned depth,
             std::size_t &nodes) {
  if (const auto error = budget.visit(depth, nodes); error != Error::None)
    return {{}, error};
  if (a.kind() != b.kind() || !a.sameSchema(b))
    return {{}, Error::TypeMismatch};
  switch (a.kind()) {
  case Kind::Boolean:
    return scalar(a.boolean(), b.boolean());
  case Kind::Signed:
    return scalar(a.signedInteger(), b.signedInteger());
  case Kind::Unsigned:
    return scalar(a.unsignedInteger(), b.unsignedInteger());
  case Kind::Real: {
    const auto x = a.real(), y = b.real();
    if (!std::isfinite(x) || !std::isfinite(y))
      return {{}, Error::NonFinite};
    // The ordered ordinal policy treats both signed zeros as equivalent.
    // It never promotes exact integer keys through a floating point value.
    return scalar(x, y);
  }
  case Kind::Text:
    return scalar(std::string_view(a.text()), std::string_view(b.text()));
  case Kind::Timestamp: {
    const auto seconds = scalar(a.seconds(), b.seconds());
    return seconds.order == Order::Equal ? scalar(a.fraction(), b.fraction())
                                         : seconds;
  }
  case Kind::Array: {
    if (a.rank() < 1 || a.rank() > 16 || b.rank() < 1 || b.rank() > 16)
      return {{}, Error::InvalidShape};
    const auto ranks = scalar(a.rank(), b.rank());
    if (ranks.order != Order::Equal)
      return ranks;
    std::size_t countA = 1, countB = 1;
    for (std::size_t d = 0; d < a.rank(); ++d) {
      const auto x = std::size_t(a.extent(d)), y = std::size_t(b.extent(d));
      if ((x && countA > (std::numeric_limits<std::size_t>::max)() / x) ||
          (y && countB > (std::numeric_limits<std::size_t>::max)() / y))
        return {{}, Error::InvalidShape};
      countA *= x;
      countB *= y;
    }
    if (countA != a.childCount() || countB != b.childCount())
      return {{}, Error::InvalidShape};
    for (std::size_t d = 0; d < a.rank(); ++d) {
      const auto shape = scalar(a.extent(d), b.extent(d));
      if (shape.order != Order::Equal)
        return shape;
    }
    break;
  }
  case Kind::Record:
    break;
  case Kind::Unsupported:
    return {{}, Error::Unsupported};
  }
  const auto n = (std::min)(a.childCount(), b.childCount());
  for (std::size_t i = 0; i < n; ++i) {
    const auto child = value(a.child(i), b.child(i), budget, depth + 1, nodes);
    if (!child.accepted() || child.order != Order::Equal)
      return child;
  }
  return scalar(a.childCount(), b.childCount());
}
} // namespace detail
// Type validation is independent from data cardinality. In particular, callers
// must call this for an empty collection before accepting its key/element type.
template <class TypeView>
[[nodiscard]] Error comparableSchema(TypeView type,
                                     Limits limits = {}) noexcept {
  try {
    detail::Budget budget{limits};
    std::size_t nodes = 0;
    return detail::schema(type, budget, 0, nodes);
  } catch (const std::bad_alloc &) {
    return Error::AllocationFailure;
  } catch (...) {
    return Error::InvalidShape;
  }
}
template <class View>
[[nodiscard]] Result compare(View a, View b, Limits limits = {},
                             Cancelled cancelled = nullptr,
                             void *context = nullptr) noexcept {
  try {
    detail::Budget budget{limits, cancelled, context};
    std::size_t schemaNodes = 0;
    if (const auto error = detail::schema(a.schema(), budget, 0, schemaNodes);
        error != Error::None)
      return {{}, error};
    std::size_t nodes = 0;
    return detail::value(a, b, budget, 0, nodes);
  } catch (const std::bad_alloc &) {
    return {{}, Error::AllocationFailure};
  } catch (...) {
    return {{}, Error::InvalidShape};
  }
}
// One stable merge kernel is shared by authoring-data and Runtime carriers.
// maximumValueNodes is per key comparison; the cumulative N log N work has its
// separate maximumWork bound. On failure no partial ordering is returned.
template <class View>
[[nodiscard]] NormalizationResult
normalize(std::span<const View> values, DuplicatePolicy duplicates,
          Limits limits = {}, Cancelled cancelled = nullptr,
          void *context = nullptr) noexcept {
  try {
    if (values.size() > limits.maximumElements)
      return {{}, Error::ValueLimit};
    if (cancelled && cancelled(context))
      return {{}, Error::Cancelled};
    if (values.empty())
      return {};
    detail::Budget budget{limits, cancelled, context};
    std::size_t schemaNodes = 0;
    if (const auto error =
            detail::schema(values.front().schema(), budget, 0, schemaNodes);
        error != Error::None)
      return {{}, error};
    const auto cmp = [&](std::size_t x, std::size_t y) {
      std::size_t nodes = 0;
      return detail::value(values[x], values[y], budget, 0, nodes);
    };
    for (std::size_t i = 0; i < values.size(); ++i) {
      if (!values.front().sameSchema(values[i]))
        return {{}, Error::TypeMismatch};
      const auto checked = cmp(i, i);
      if (!checked.accepted())
        return {{}, checked.error};
    }
    std::vector<std::size_t> order(values.size()), work(values.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    for (std::size_t width = 1; width < order.size();) {
      for (std::size_t begin = 0; begin < order.size();) {
        const auto middle = begin + (std::min)(width, order.size() - begin);
        const auto end = middle + (std::min)(width, order.size() - middle);
        auto left = begin, right = middle, out = begin;
        while (left < middle && right < end) {
          const auto c = cmp(order[left], order[right]);
          if (!c.accepted())
            return {{}, c.error};
          work[out++] =
              c.order == Order::Greater ? order[right++] : order[left++];
        }
        while (left < middle)
          work[out++] = order[left++];
        while (right < end)
          work[out++] = order[right++];
        begin = end;
      }
      order.swap(work);
      if (width > order.size() / 2)
        break;
      width *= 2;
    }
    std::vector<std::size_t> result;
    result.reserve(order.size());
    for (std::size_t begin = 0; begin < order.size();) {
      auto end = begin + 1;
      while (end < order.size()) {
        const auto c = cmp(order[begin], order[end]);
        if (!c.accepted())
          return {{}, c.error};
        if (c.order != Order::Equal)
          break;
        ++end;
      }
      result.push_back(
          order[duplicates == DuplicatePolicy::KeepLast ? end - 1 : begin]);
      begin = end;
    }
    return {std::move(result), Error::None};
  } catch (const std::bad_alloc &) {
    return {{}, Error::AllocationFailure};
  } catch (...) {
    return {{}, Error::InvalidShape};
  }
}
} // namespace frog::sdk::collection_order
