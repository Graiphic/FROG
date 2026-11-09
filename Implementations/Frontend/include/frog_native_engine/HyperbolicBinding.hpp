#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct HyperbolicTypeEvidence {
    std::string_view portId;
    binding::TypeRef valueType;
};
struct HyperbolicTypeResult {
    binding::TypeResolution resolution;
    // A DBL palette preview never supplies an unwired operand.
    bool complete = false;
};
[[nodiscard]] bool isHyperbolicFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> hyperbolicInputPortIds(
    std::string_view catalogId) noexcept;
[[nodiscard]] HyperbolicTypeResult resolveHyperbolicPort(
    std::string_view catalogId,
    std::string_view portId,
    std::span<const HyperbolicTypeEvidence> inputs = {});
}
