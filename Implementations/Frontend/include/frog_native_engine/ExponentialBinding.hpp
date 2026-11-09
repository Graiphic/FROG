#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct ExponentialTypeEvidence {
    std::string_view portId;
    binding::TypeRef valueType;
};
struct ExponentialTypeResult {
    binding::TypeResolution resolution;
    // A DBL palette preview does not supply a missing calculation operand.
    bool complete = false;
};
[[nodiscard]] bool isExponentialFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> exponentialInputPortIds(
    std::string_view catalogId) noexcept;
[[nodiscard]] ExponentialTypeResult resolveExponentialPort(
    std::string_view catalogId,
    std::string_view portId,
    std::span<const ExponentialTypeEvidence> inputs = {});
}
