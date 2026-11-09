#pragma once

#include "frog_native_engine/BindingType.hpp"
#include <span>
#include <string_view>

namespace frog::native_engine {

// Public semantic laws, without an optimizer, provider, document or UI object.
struct InterpolationFunctionDescriptor {
    std::string_view catalogId;
    std::string_view operationId;
    std::string_view displayName;
};
struct InterpolationVariantDescriptor {
    std::string_view variantId;
    std::string_view operationId;
    std::string_view displayName;
};
struct InterpolationInputDefault {
    std::string_view portId;
    std::string_view valueType;
    std::string_view jsonValue;
};
struct InterpolationTypeEvidence {
    std::string_view portId;
    binding::TypeRef valueType;
};
struct InterpolationTypeResult {
    binding::TypeResolution resolution;
    bool complete = false;
};

[[nodiscard]] std::span<const InterpolationFunctionDescriptor> interpolationFunctions() noexcept;
[[nodiscard]] const InterpolationFunctionDescriptor *findInterpolationFunction(std::string_view catalogId) noexcept;
[[nodiscard]] bool isInterpolationFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const InterpolationVariantDescriptor> interpolationVariants(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view interpolationOperationId(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const std::string_view> interpolationInputPortIds(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const std::string_view> interpolationOutputPortIds(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const InterpolationInputDefault> interpolationInputDefaults(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] bool interpolationInputRequired(std::string_view catalogId,std::string_view portId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::string_view interpolationPortDisplayName(std::string_view catalogId,std::string_view portId,std::string_view variant = {}) noexcept;
[[nodiscard]] InterpolationTypeResult resolveInterpolationPort(
    std::string_view catalogId,std::string_view portId,
    std::span<const InterpolationTypeEvidence> inputs = {},std::string_view variant = {});

} // namespace frog::native_engine
