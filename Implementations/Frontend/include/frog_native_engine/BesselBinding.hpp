#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct BesselTypeEvidence { std::string_view portId; binding::TypeRef valueType; };
struct BesselTypeResult { binding::TypeResolution resolution; bool complete = false; };
[[nodiscard]] bool isBesselFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view besselFunctionKey(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> besselInputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> besselOutputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] BesselTypeResult resolveBesselPort(std::string_view catalogId, std::string_view portId,
    std::span<const BesselTypeEvidence> inputs = {});
}
