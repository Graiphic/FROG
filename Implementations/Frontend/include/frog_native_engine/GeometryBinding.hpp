#pragma once

#include "frog_native_engine/BindingType.hpp"
#include <span>
#include <string_view>

namespace frog::native_engine {

// Public semantic laws, without an optimizer, provider, document or UI object.
struct GeometryFunctionDescriptor {
    std::string_view catalogId;
    std::string_view operationId;
    std::string_view displayName;
};
struct GeometryVariantDescriptor {
    std::string_view variantId;
    std::string_view operationId;
    std::string_view displayName;
};
struct GeometryInputDefault {
    std::string_view portId;
    std::string_view valueType;
    std::string_view jsonValue;
};
struct GeometryTypeEvidence {
    std::string_view portId;
    binding::TypeRef valueType;
};
struct GeometryTypeResult {
    binding::TypeResolution resolution;
    bool complete = false;
};

[[nodiscard]] std::span<const GeometryFunctionDescriptor> geometryFunctions() noexcept;
[[nodiscard]] const GeometryFunctionDescriptor *findGeometryFunction(std::string_view catalogId) noexcept;
[[nodiscard]] bool isGeometryFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const GeometryVariantDescriptor> geometryVariants(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view geometryOperationId(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const std::string_view> geometryInputPortIds(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const std::string_view> geometryOutputPortIds(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const GeometryInputDefault> geometryInputDefaults(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] bool geometryInputRequired(std::string_view catalogId,std::string_view portId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::string_view geometryPortDisplayName(std::string_view catalogId,std::string_view portId,std::string_view variant = {}) noexcept;
[[nodiscard]] GeometryTypeResult resolveGeometryPort(
    std::string_view catalogId,std::string_view portId,
    std::span<const GeometryTypeEvidence> inputs = {},std::string_view variant = {});

} // namespace frog::native_engine
