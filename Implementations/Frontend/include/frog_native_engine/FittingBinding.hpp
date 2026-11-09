#pragma once

#include "frog_native_engine/BindingType.hpp"
#include <span>
#include <string_view>

namespace frog::native_engine {

// Public semantic laws, without an optimizer, provider, document or UI object.
struct FittingFunctionDescriptor {
    std::string_view catalogId;
    std::string_view operationId;
    std::string_view displayName;
};
struct FittingVariantDescriptor {
    std::string_view variantId;
    std::string_view operationId;
    std::string_view displayName;
};
struct FittingInputDefault {
    std::string_view portId;
    std::string_view valueType;
    std::string_view jsonValue;
};
struct FittingTypeEvidence {
    std::string_view portId;
    binding::TypeRef valueType;
};
struct FittingTypeResult {
    binding::TypeResolution resolution;
    bool complete = false;
};

[[nodiscard]] std::span<const FittingFunctionDescriptor> fittingFunctions() noexcept;
[[nodiscard]] const FittingFunctionDescriptor *findFittingFunction(std::string_view catalogId) noexcept;
[[nodiscard]] bool isFittingFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const FittingVariantDescriptor> fittingVariants(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view fittingOperationId(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const std::string_view> fittingInputPortIds(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const std::string_view> fittingOutputPortIds(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const FittingInputDefault> fittingInputDefaults(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] bool fittingInputRequired(std::string_view catalogId,std::string_view portId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::string_view fittingPortDisplayName(std::string_view catalogId,std::string_view portId,std::string_view variant = {}) noexcept;
[[nodiscard]] FittingTypeResult resolveFittingPort(
    std::string_view catalogId,std::string_view portId,
    std::span<const FittingTypeEvidence> inputs = {},std::string_view variant = {});

} // namespace frog::native_engine
