#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace frog::native_engine {
struct DiagramNode;
struct QueueTypeEvidence { std::string_view portId; binding::TypeRef valueType; };
struct QueueTypeResult { binding::TypeResolution resolution; bool complete=false; };
[[nodiscard]] bool isQueueFunction(std::string_view catalogId) noexcept;
// Neutral contract owner; nodeId is used only for diagnostic attribution.
[[nodiscard]] QueueTypeResult resolveQueueFunctionPort(
    std::string_view operationId,std::string_view nodeId,std::string_view portId,
    std::span<const QueueTypeEvidence> evidence={});
[[nodiscard]] std::optional<std::string> queueOptionalInputDefault(
    std::string_view operationId,std::string_view portId);
// Studio adaptation retains the same contract implementation.
[[nodiscard]] QueueTypeResult resolveQueueFunctionPort(const DiagramNode &,
    std::string_view portId,std::span<const QueueTypeEvidence> evidence={});
// Scalar defaults only; error_in uses the shared canonical no-error cluster.
[[nodiscard]] std::optional<std::string> queueOptionalInputDefault(
    const DiagramNode &,std::string_view portId);
} // namespace frog::native_engine
