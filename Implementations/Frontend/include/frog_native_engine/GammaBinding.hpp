#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct GammaTypeEvidence { std::string_view portId; binding::TypeRef valueType; };
struct GammaTypeResult { binding::TypeResolution resolution; bool complete = false; };
[[nodiscard]] bool isGammaFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view gammaFunctionKey(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> gammaInputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> gammaOutputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] std::optional<double> gammaOptionalInputDefault(std::string_view catalogId,
    std::string_view portId) noexcept;
[[nodiscard]] GammaTypeResult resolveGammaPort(std::string_view catalogId, std::string_view portId,
    std::span<const GammaTypeEvidence> inputs = {});
}
