#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct PartialDifferentialTypeEvidence { std::string_view portId; binding::TypeRef valueType; };
struct PartialDifferentialTypeResult { binding::TypeResolution resolution; bool complete = false; };
[[nodiscard]] bool isPartialDifferentialFunction(std::string_view id) noexcept;
[[nodiscard]] binding::TypeRef partialDifferentialProblemType(std::string_view family);
[[nodiscard]] binding::TypeRef partialDifferentialCallableType(std::string_view family);
[[nodiscard]] PartialDifferentialTypeResult resolvePartialDifferentialPort(std::string_view id, std::string_view port,
    std::span<const PartialDifferentialTypeEvidence> evidence = {});
}
