#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace frog::native_engine {
struct DiagramNode;
struct RendezvousTypeEvidence {std::string_view portId;binding::TypeRef valueType;};
struct RendezvousTypeResult {binding::TypeResolution resolution;bool complete=false;};
[[nodiscard]] bool isRendezvousFunction(std::string_view catalogId) noexcept;
[[nodiscard]] bool isRendezvousWaitFunction(std::string_view catalogId) noexcept;
[[nodiscard]] bool rendezvousExecutesOnInputError(std::string_view catalogId) noexcept;
// Required connectivity and the fixed nominal schema are independent: an
// unwired input remains typed for Create Constant but is not execution-ready.
// Neutral contract owner; nodeId is used only for diagnostic attribution.
[[nodiscard]] RendezvousTypeResult resolveRendezvousFunctionPort(
    std::string_view operationId,std::string_view nodeId,std::string_view portId,
    std::span<const RendezvousTypeEvidence> evidence={});
[[nodiscard]] std::optional<std::string> rendezvousOptionalInputDefault(
    std::string_view operationId,std::string_view portId);
// Studio adaptation retains the same contract implementation.
[[nodiscard]] RendezvousTypeResult resolveRendezvousFunctionPort(const DiagramNode &,
    std::string_view portId,std::span<const RendezvousTypeEvidence> evidence={});
[[nodiscard]] std::optional<std::string> rendezvousOptionalInputDefault(const DiagramNode &,std::string_view portId);
} // namespace frog::native_engine
