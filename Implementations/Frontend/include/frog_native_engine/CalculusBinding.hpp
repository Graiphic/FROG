#pragma once

#include "frog_native_engine/BindingType.hpp"
#include <span>
#include <string_view>

namespace frog::native_engine {

// Public semantic laws only: no provider, editor, filesystem or runtime state.
struct CalculusFunctionDescriptor {
    std::string_view catalogId;
    std::string_view operationId;
    std::string_view displayName;
};
struct CalculusVariantDescriptor {
    std::string_view variantId;
    std::string_view operationId;
    std::string_view displayName;
};
struct CalculusInputDefault {
    std::string_view portId;
    std::string_view valueType;
    std::string_view jsonValue;
};
struct CalculusTypeEvidence {
    std::string_view portId;
    binding::TypeRef valueType;
};
struct CalculusTypeResult {
    binding::TypeResolution resolution;
    bool complete = false;
};

[[nodiscard]] std::span<const CalculusFunctionDescriptor> calculusFunctions() noexcept;
[[nodiscard]] const CalculusFunctionDescriptor *findCalculusFunction(std::string_view catalogId) noexcept;
[[nodiscard]] bool isCalculusFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const CalculusVariantDescriptor> calculusVariants(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view calculusOperationId(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const std::string_view> calculusInputPortIds(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const std::string_view> calculusOutputPortIds(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const CalculusInputDefault> calculusInputDefaults(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] bool calculusInputRequired(std::string_view catalogId,std::string_view portId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::string_view calculusPortDisplayName(std::string_view catalogId,std::string_view portId,std::string_view variant = {}) noexcept;
[[nodiscard]] CalculusTypeResult resolveCalculusPort(
    std::string_view catalogId,std::string_view portId,
    std::span<const CalculusTypeEvidence> inputs = {},std::string_view variant = {});

} // namespace frog::native_engine
