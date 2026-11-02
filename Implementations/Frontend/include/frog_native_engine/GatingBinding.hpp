#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct GatingTypeEvidence {
    std::string_view portId;
    binding::TypeRef valueType;
};
struct GatingTypeResult {
    binding::TypeResolution resolution;
    // A palette DBL preview never supplies an unwired calculation operand.
    bool complete = false;
};
[[nodiscard]] bool isGatingFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> gatingInputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] GatingTypeResult resolveGatingPort(std::string_view catalogId,
    std::string_view portId, std::span<const GatingTypeEvidence> inputs = {});
}
