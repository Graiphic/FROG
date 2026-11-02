#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct TrigonometricTypeEvidence {
    std::string_view portId;
    binding::TypeRef valueType;
};
struct TrigonometricTypeResult {
    binding::TypeResolution resolution;
    bool complete = false;
};
[[nodiscard]] bool isTrigonometricFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> trigonometricInputPortIds(
    std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> trigonometricOutputPortIds(
    std::string_view catalogId) noexcept;
[[nodiscard]] TrigonometricTypeResult resolveTrigonometricPort(
    std::string_view catalogId, std::string_view portId,
    std::span<const TrigonometricTypeEvidence> inputs = {});
}
