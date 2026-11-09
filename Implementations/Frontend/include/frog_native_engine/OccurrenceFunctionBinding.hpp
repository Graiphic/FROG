#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace frog::native_engine {
struct DiagramNode;
struct OccurrenceTypeEvidence {std::string_view portId;binding::TypeRef valueType;};
struct OccurrenceTypeResult {binding::TypeResolution resolution;bool complete=false;};
[[nodiscard]] bool isOccurrenceFunction(std::string_view catalogId) noexcept;
[[nodiscard]] bool isOccurrenceWaitFunction(std::string_view catalogId) noexcept;
// A required reference remains typed for authoring even before it is wired.
// Neutral contract owner; nodeId is used only for diagnostic attribution.
[[nodiscard]] OccurrenceTypeResult resolveOccurrenceFunctionPort(
    std::string_view operationId,std::string_view nodeId,std::string_view portId,
    std::span<const OccurrenceTypeEvidence> evidence={});
[[nodiscard]] std::optional<std::string> occurrenceOptionalInputDefault(
    std::string_view operationId,std::string_view portId);
// Studio adaptation retains the same contract implementation.
[[nodiscard]] OccurrenceTypeResult resolveOccurrenceFunctionPort(const DiagramNode &,
    std::string_view portId,std::span<const OccurrenceTypeEvidence> evidence={});
[[nodiscard]] std::optional<std::string> occurrenceOptionalInputDefault(const DiagramNode &,std::string_view portId);
} // namespace frog::native_engine
