#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
struct ErrorFunctionsTypeEvidence { std::string_view portId; binding::TypeRef valueType; };
struct ErrorFunctionsTypeResult { binding::TypeResolution resolution; bool complete = false; };
[[nodiscard]] bool isErrorFunctionsFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view errorFunctionsFunctionKey(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> errorFunctionsInputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> errorFunctionsOutputPortIds(std::string_view catalogId) noexcept;
[[nodiscard]] ErrorFunctionsTypeResult resolveErrorFunctionsPort(std::string_view catalogId,
    std::string_view portId, std::span<const ErrorFunctionsTypeEvidence> inputs = {});
}
