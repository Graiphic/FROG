#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct HypergeometricTypeEvidence { std::string_view portId; binding::TypeRef valueType; };
struct HypergeometricTypeResult { binding::TypeResolution resolution; bool complete = false; };
[[nodiscard]] bool isHypergeometricFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view hypergeometricFunctionKey(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> hypergeometricInputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> hypergeometricOutputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] HypergeometricTypeResult resolveHypergeometricPort(std::string_view catalogId, std::string_view portId,
    std::span<const HypergeometricTypeEvidence> inputs = {});
}
