#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace frog::native_engine {
struct DiagramNode;
struct NotifierTypeEvidence { std::string_view portId; binding::TypeRef valueType; };
struct NotifierTypeResult { binding::TypeResolution resolution; bool complete=false; };
[[nodiscard]] bool isNotifierFunction(std::string_view catalogId) noexcept;
[[nodiscard]] bool isNotifierMultipleFunction(std::string_view catalogId) noexcept;
[[nodiscard]] bool isNotifierWaitFunction(std::string_view catalogId) noexcept;
// Neutral contract owner; nodeId is used only for diagnostic attribution.
[[nodiscard]] NotifierTypeResult resolveNotifierFunctionPort(
    std::string_view operationId,std::string_view nodeId,std::string_view portId,
    std::span<const NotifierTypeEvidence> evidence={});
[[nodiscard]] std::optional<std::string> notifierOptionalInputDefault(
    std::string_view operationId,std::string_view portId);
// Studio adaptation retains the same contract implementation.
[[nodiscard]] NotifierTypeResult resolveNotifierFunctionPort(const DiagramNode &,
    std::string_view portId,std::span<const NotifierTypeEvidence> evidence={});
// Scalar defaults only; error_in uses the common canonical no-error cluster.
[[nodiscard]] std::optional<std::string> notifierOptionalInputDefault(
    const DiagramNode &,std::string_view portId);
} // namespace frog::native_engine
