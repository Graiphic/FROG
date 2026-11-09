#pragma once

#include "frog_native_engine/BindingType.hpp"
#include <span>
#include <string_view>

namespace frog::native_engine {

// Public semantic description only. Algorithms and their target providers do
// not belong to this Frontend. Matrix keeps the existing numeric Array rank 2.
struct BlasFunctionDescriptor {
    std::string_view catalogId;
    std::string_view operationId;
    std::string_view displayName;
};
struct BlasInputDefault {
    std::string_view portId;
    std::string_view valueType;
    std::string_view jsonValue;
};
struct BlasTypeEvidence {
    std::string_view portId;
    binding::TypeRef valueType;
};
struct BlasTypeResult {
    binding::TypeResolution resolution;
    bool complete = false;
};

[[nodiscard]] std::span<const BlasFunctionDescriptor>
blasFunctions() noexcept;
[[nodiscard]] const BlasFunctionDescriptor *findBlasFunction(
    std::string_view catalogId) noexcept;
[[nodiscard]] bool isBlasFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> blasInputPortIds(
    std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> blasOutputPortIds(
    std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const BlasInputDefault> blasInputDefaults(
    std::string_view catalogId) noexcept;
[[nodiscard]] BlasTypeResult resolveBlasPort(
    std::string_view catalogId,
    std::string_view portId,
    std::span<const BlasTypeEvidence> inputs = {});

} // namespace frog::native_engine
