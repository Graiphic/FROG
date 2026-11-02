#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <span>

namespace frog::native_engine {

inline constexpr std::uint64_t kMaximumArrayElements = UINT32_MAX;
inline constexpr std::uint32_t kMaximumArrayIndex = UINT32_MAX - 1U;

// A dimension is U32; the complete array, including multidimensional arrays,
// has at most UINT32_MAX elements. Check zero extents before multiplying.
inline std::optional<std::size_t> checkedArrayElementCount(
    std::span<const std::uint32_t> shape) noexcept {
    if (shape.empty()) return std::nullopt;
    for (const auto extent : shape) if (!extent) return 0U;
    std::uint64_t count = 1;
    for (const auto extent : shape) {
        if (count > kMaximumArrayElements / extent) return std::nullopt;
        count *= extent;
    }
    if (count > (std::numeric_limits<std::size_t>::max)()) return std::nullopt;
    return static_cast<std::size_t>(count);
}

inline bool arrayAllocationFitsBudget(std::size_t count,
    std::size_t bytesPerElement, std::size_t extraBytes,
    std::uint64_t availableBytes) noexcept {
    if (count > kMaximumArrayElements || extraBytes > availableBytes) return false;
    if (bytesPerElement && count >
        ((std::numeric_limits<std::size_t>::max)() - extraBytes) / bytesPerElement)
        return false;
    return !bytesPerElement || count <= (availableBytes - extraBytes) / bytesPerElement;
}

// Admission uses the current OS commit/address-space availability, not a
// fixed element quota. Callers must still catch allocation failures: memory
// can change between this check and the allocator's request.
bool arrayAllocationFits(std::size_t count, std::size_t bytesPerElement,
    std::size_t extraBytes = 0) noexcept;

} // namespace frog::native_engine
