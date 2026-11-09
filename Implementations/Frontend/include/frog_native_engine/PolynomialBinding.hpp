#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <span>
#include <string_view>

namespace frog::native_engine {
// Semantic contracts only. The numerical algorithms and their providers remain
// target responsibilities. Coefficient arrays use ascending powers.
struct PolynomialFunctionDescriptor {
    std::string_view catalogId,operationId,displayName;
};
struct PolynomialTypeEvidence {std::string_view portId;binding::TypeRef valueType;};
struct PolynomialTypeResult {binding::TypeResolution resolution;bool complete=false;};
[[nodiscard]] std::span<const PolynomialFunctionDescriptor> polynomialFunctions() noexcept;
[[nodiscard]] const PolynomialFunctionDescriptor *findPolynomialFunction(std::string_view) noexcept;
[[nodiscard]] bool isPolynomialFunction(std::string_view) noexcept;
[[nodiscard]] std::span<const std::string_view> polynomialInputPortIds(std::string_view) noexcept;
[[nodiscard]] std::span<const std::string_view> polynomialOutputPortIds(std::string_view) noexcept;
[[nodiscard]] PolynomialTypeResult resolvePolynomialPort(std::string_view,std::string_view,
    std::span<const PolynomialTypeEvidence> = {});
}
