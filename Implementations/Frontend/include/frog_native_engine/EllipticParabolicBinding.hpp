#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct EllipticParabolicTypeEvidence { std::string_view portId; binding::TypeRef valueType; };
struct EllipticParabolicTypeResult { binding::TypeResolution resolution; bool complete = false; };
[[nodiscard]] bool isEllipticParabolicFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view ellipticParabolicFunctionKey(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> ellipticParabolicInputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> ellipticParabolicOutputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] EllipticParabolicTypeResult resolveEllipticParabolicPort(std::string_view catalogId,
    std::string_view portId, std::span<const EllipticParabolicTypeEvidence> inputs = {});
}
