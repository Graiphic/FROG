#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include <optional>

namespace frog::native_engine {
inline constexpr std::string_view kSequenceStructureType = "frog.studio.structures.sequence_structure";
inline constexpr double kSequenceMinimumFrameWidth = 96.0;
[[nodiscard]] bool isSequenceStructure(const DiagramNode &) noexcept;
[[nodiscard]] std::optional<std::size_t> sequenceFrameIndex(const DiagramNode &, std::string_view) noexcept;
// Frame owning the input/output data face. Empty denotes the parent diagram.
[[nodiscard]] std::string_view sequenceTunnelInputFrameId(const DiagramCaseStructureTunnel &) noexcept;
[[nodiscard]] std::string_view sequenceTunnelOutputFrameId(const DiagramCaseStructureTunnel &) noexcept;
[[nodiscard]] std::optional<std::string_view> sequenceTunnelPortFrameId(
    const DiagramCaseStructureTunnel &, std::string_view portId);
struct SequenceDiagnostic {
    std::string code, objectId, message;
};
[[nodiscard]] std::vector<SequenceDiagnostic> validateSequenceStructure(
    const DiagramNode &, const std::vector<DiagramNode> &, const std::vector<DiagramEdge> &);
std::string sequenceFrameOwner(const std::vector<DiagramNode>&,std::string_view,std::string,std::string);
}
