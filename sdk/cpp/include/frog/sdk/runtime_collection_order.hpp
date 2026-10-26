#pragma once
#include "frog/sdk/collection_order.hpp"
#include "frog/sdk/runtime_compound_value.hpp"
#include <memory>
#include <optional>
#include <stdexcept>

namespace frog::sdk::runtime {
// Runtime's view of the same std-only kernel used by authoring values. The
// shared schema is parsed once per operation, not once for every sort compare.
struct RuntimeCollectionSchema {
  ValueType type;
  collection_order::Kind kind = collection_order::Kind::Unsupported;
  std::vector<RuntimeCollectionSchema> children;
  std::vector<std::string> fieldIds;
};
inline RuntimeCollectionSchema runtimeCollectionSchema(const ValueType &type,
                                                       unsigned depth = 0) {
  RuntimeCollectionSchema s;
  s.type = type;
  if (depth > 32 || type != valueTypeFromCanonicalName(type.canonicalName))
    return s;
  using K = collection_order::Kind;
  switch (type.category) {
  case ValueCategory::Boolean:
    s.kind = K::Boolean;
    break;
  case ValueCategory::SignedInteger:
    s.kind = K::Signed;
    break;
  case ValueCategory::UnsignedInteger:
    s.kind = K::Unsigned;
    break;
  case ValueCategory::FloatingPoint:
    s.kind = K::Real;
    break;
  case ValueCategory::String:
  case ValueCategory::Path:
    s.kind = K::Text;
    break;
  case ValueCategory::Timestamp:
    s.kind = K::Timestamp;
    break;
  case ValueCategory::Array:
    s.kind = K::Array;
    s.children.push_back(runtimeCollectionSchema(
        valueTypeFromCanonicalName(type.elementType), depth + 1));
    break;
  case ValueCategory::Cluster: {
    const auto fields =
        structural_detail::TypeSyntax(type.canonicalName).recordFields();
    if (!fields)
      break;
    s.kind = K::Record;
    for (const auto &[id, name] : *fields) {
      s.fieldIds.push_back(id);
      s.children.push_back(
          runtimeCollectionSchema(valueTypeFromCanonicalName(name), depth + 1));
    }
    break;
  }
  default:
    break;
  }
  return s;
}
class RuntimeCollectionTypeView {
public:
  explicit RuntimeCollectionTypeView(const RuntimeCollectionSchema &s)
      : schema_(&s) {}
  collection_order::Kind kind() const noexcept { return schema_->kind; }
  std::size_t childCount() const noexcept { return schema_->children.size(); }
  RuntimeCollectionTypeView child(std::size_t i) const {
    return RuntimeCollectionTypeView(schema_->children.at(i));
  }
  std::size_t rank() const noexcept { return schema_->type.rank; }

private:
  const RuntimeCollectionSchema *schema_;
};
class RuntimeCollectionValueView {
public:
  RuntimeCollectionValueView(const RuntimeValue &v,
                             const RuntimeCollectionSchema &s,
                             std::string_view nominal = {})
      : value_(&v), schema_(&s), nominal_(nominal) {}
  collection_order::Kind kind() const noexcept { return schema_->kind; }
  RuntimeCollectionTypeView schema() const noexcept {
    return RuntimeCollectionTypeView(*schema_);
  }
  bool sameSchema(const RuntimeCollectionValueView &other) const noexcept {
    return schema_->type == other.schema_->type && nominal_ == other.nominal_;
  }
  bool boolean() const { return std::get<bool>(*value_); }
  std::int64_t signedInteger() const { return std::get<std::int64_t>(*value_); }
  std::uint64_t unsignedInteger() const {
    return std::get<std::uint64_t>(*value_);
  }
  double real() const {
    return packedScalar_ ? *packedScalar_ : std::get<double>(*value_);
  }
  std::string_view text() const { return std::get<std::string>(*value_); }
  std::size_t rank() const {
    if (packed())
      return 1;
    const auto *a = runtimeArray(*value_);
    if (!a)
      throw std::invalid_argument(
          "A typed collection Array view needs a real immutable array.");
    return a->shape().size();
  }
  std::uint32_t extent(std::size_t d) const {
    if (const auto *a = packed()) {
      if (d)
        throw std::out_of_range("Packed Array is rank one.");
      return static_cast<std::uint32_t>(a->elements().size());
    }
    return runtimeArray(*value_)->shape()[d];
  }
  std::int64_t seconds() const { return runtimeTimestamp(*value_)->seconds(); }
  std::uint64_t fraction() const {
    return runtimeTimestamp(*value_)->fraction();
  }
  std::size_t childCount() const {
    if (schema_->kind == collection_order::Kind::Array) {
      if (const auto *a = packed())
        return a->elements().size();
      return runtimeArray(*value_)->elements().size();
    }
    if (schema_->kind == collection_order::Kind::Record)
      return std::get<std::shared_ptr<const CompoundRuntimeValue>>(*value_)
          ->fields()
          .size();
    return 0;
  }
  RuntimeCollectionValueView child(std::size_t i) const {
    if (schema_->kind == collection_order::Kind::Array) {
      if (const auto *a = packed())
        return RuntimeCollectionValueView(a->elements()[i],
                                          schema_->children.at(0), nominal_);
      return RuntimeCollectionValueView(runtimeArray(*value_)->elements()[i],
                                        schema_->children.at(0), nominal_);
    }
    const auto fields =
        std::get<std::shared_ptr<const CompoundRuntimeValue>>(*value_)
            ->fields();
    if (fields[i].id != schema_->fieldIds.at(i))
      throw std::invalid_argument(
          "Record ordering lost a logical field identity.");
    return RuntimeCollectionValueView(fields[i].value, schema_->children.at(i),
                                      nominal_);
  }

private:
  RuntimeCollectionValueView(double value,
                             const RuntimeCollectionSchema &schema,
                             std::string_view nominal)
      : value_(nullptr), schema_(&schema), nominal_(nominal),
        packedScalar_(value) {}
  const ImmutableF64Array *packed() const noexcept {
    if (!value_)
      return nullptr;
    const auto *p = std::get_if<RuntimeF64Array>(value_);
    return p && *p ? p->get() : nullptr;
  }
  const RuntimeValue *value_;
  const RuntimeCollectionSchema *schema_;
  std::string_view nominal_;
  std::optional<double> packedScalar_;
};
inline collection_order::Error
runtimeCollectionComparableType(const ValueType &type,
                                collection_order::Limits limits = {}) noexcept {
  try {
    const auto schema = runtimeCollectionSchema(type);
    return collection_order::comparableSchema(RuntimeCollectionTypeView(schema),
                                              limits);
  } catch (const std::bad_alloc &) {
    return collection_order::Error::AllocationFailure;
  } catch (...) {
    return collection_order::Error::InvalidShape;
  }
}
inline collection_order::Result compareRuntimeCollectionValues(
    const RuntimeValue &a, const RuntimeValue &b, const ValueType &type,
    const RuntimeValueOwner &owner, collection_order::Limits limits = {},
    collection_order::Cancelled cancelled = nullptr,
    void *context = nullptr) noexcept {
  try {
    if (!runtimeValueValidForOwner(a, type, owner) ||
        !runtimeValueValidForOwner(b, type, owner))
      return {{}, collection_order::Error::TypeMismatch};
    const auto schema = runtimeCollectionSchema(type);
    return collection_order::compare(RuntimeCollectionValueView(a, schema),
                                     RuntimeCollectionValueView(b, schema),
                                     limits, cancelled, context);
  } catch (const std::bad_alloc &) {
    return {{}, collection_order::Error::AllocationFailure};
  } catch (...) {
    return {{}, collection_order::Error::InvalidShape};
  }
}
inline collection_order::NormalizationResult normalizeRuntimeCollectionValues(
    std::span<const RuntimeValue> values, const ValueType &type,
    const RuntimeValueOwner &owner,
    collection_order::DuplicatePolicy duplicates,
    collection_order::Limits limits = {},
    collection_order::Cancelled cancelled = nullptr,
    void *context = nullptr) noexcept {
  try {
    const auto schema = runtimeCollectionSchema(type);
    if (const auto error = collection_order::comparableSchema(
            RuntimeCollectionTypeView(schema), limits);
        error != collection_order::Error::None)
      return {{}, error};
    std::vector<RuntimeCollectionValueView> views;
    views.reserve(values.size());
    for (const auto &v : values) {
      if (!runtimeValueValidForOwner(v, type, owner))
        return {{}, collection_order::Error::TypeMismatch};
      views.emplace_back(v, schema);
    }
    return collection_order::normalize(
        std::span<const RuntimeCollectionValueView>(views), duplicates, limits,
        cancelled, context);
  } catch (const std::bad_alloc &) {
    return {{}, collection_order::Error::AllocationFailure};
  } catch (...) {
    return {{}, collection_order::Error::InvalidShape};
  }
}
inline std::optional<std::pair<ValueType, ValueType>>
runtimeMapTypeParameters(std::string_view spelling) {
  if (!spelling.starts_with("map<") || !spelling.ends_with('>'))
    return {};
  spelling.remove_prefix(4);
  spelling.remove_suffix(1);
  unsigned depth = 0;
  bool quoted = false, escape = false;
  for (std::size_t i = 0; i < spelling.size(); ++i) {
    const auto c = spelling[i];
    if (quoted) {
      if (escape)
        escape = false;
      else if (c == '\\')
        escape = true;
      else if (c == '"')
        quoted = false;
      continue;
    }
    if (c == '"')
      quoted = true;
    else if (c == '<')
      ++depth;
    else if (c == '>')
      --depth;
    else if (c == ',' && !depth) {
      auto key = valueTypeFromCanonicalName(spelling.substr(0, i)),
           value = valueTypeFromCanonicalName(spelling.substr(i + 1));
      if (key.known() && value.known())
        return std::pair{std::move(key), std::move(value)};
      return {};
    }
  }
  return {};
}
} // namespace frog::sdk::runtime
