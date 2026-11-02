#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct EllipticTypeEvidence { std::string_view portId; binding::TypeRef valueType; };
struct EllipticTypeResult { binding::TypeResolution resolution; bool complete = false; };
[[nodiscard]] bool isEllipticFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view ellipticFunctionKey(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> ellipticInputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> ellipticOutputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] std::optional<double> ellipticOptionalInputDefault(std::string_view catalogId, std::string_view portId) noexcept;
[[nodiscard]] EllipticTypeResult resolveEllipticPort(std::string_view catalogId, std::string_view portId,
    std::span<const EllipticTypeEvidence> inputs = {});
}
