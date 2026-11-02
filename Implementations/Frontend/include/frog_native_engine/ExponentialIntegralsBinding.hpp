#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct ExponentialIntegralsTypeEvidence { std::string_view portId; binding::TypeRef valueType; };
struct ExponentialIntegralsTypeResult { binding::TypeResolution resolution; bool complete = false; };
[[nodiscard]] bool isExponentialIntegralsFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view exponentialIntegralsFunctionKey(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> exponentialIntegralsInputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> exponentialIntegralsOutputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] ExponentialIntegralsTypeResult resolveExponentialIntegralsPort(std::string_view catalogId,
    std::string_view portId, std::span<const ExponentialIntegralsTypeEvidence> inputs = {});
}
