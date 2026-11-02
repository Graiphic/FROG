#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct OrdinaryDifferentialTypeEvidence {std::string_view portId;binding::TypeRef valueType;};
struct OrdinaryDifferentialTypeResult {binding::TypeResolution resolution;bool complete{false};};
[[nodiscard]] bool isOrdinaryDifferentialFunction(std::string_view id) noexcept;
[[nodiscard]] std::string_view ordinaryDifferentialFunctionKey(std::string_view id) noexcept;
[[nodiscard]] binding::TypeRef ordinaryDifferentialCallableType(bool dae);
[[nodiscard]] binding::TypeRef ordinaryDifferentialFormulaType(bool dae);
[[nodiscard]] binding::TypeRef ordinaryDifferentialParametersType(bool dae);
[[nodiscard]] OrdinaryDifferentialTypeResult resolveOrdinaryDifferentialPort(std::string_view id,std::string_view port,std::span<const OrdinaryDifferentialTypeEvidence> evidence={});
}
