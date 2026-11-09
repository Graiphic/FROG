#pragma once

#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/DiagramStructureCatalog.hpp"

namespace frog::native_engine {

// Alternative subdiagrams share boundary tunnels. Sequence frames instead own
// their individual tunnels; linking one frame must not wire every other frame.
inline bool diagramStructureSupportsLinkedInputTunnels(const DiagramNode &node) noexcept {
    using Family = DiagramStructureFamily;
    const auto family = diagramStructureFamilyForNodeType(node.type);
    return family == Family::Conditional || family == Family::Event ||
        family == Family::Disabled || family == Family::ConditionalDisable ||
        node.type == "case";
}

// A default replaces an absent inner producer, never a shift-register update
// or an absent outer consumer. The flag alone supplies no type information.
inline bool diagramTunnelSupportsDefault(const DiagramNode &node,
    const DiagramCaseStructureTunnel &tunnel) noexcept {
    return tunnel.outputFromStructure && tunnel.shiftRegisterPairId.empty() &&
        (isDiagramStructureNodeType(node.type) || node.type == "case");
}

inline std::vector<std::string> diagramOutputTunnelRegionIds(const DiagramNode &node,
    const DiagramCaseStructureTunnel &tunnel) {
    using Family = DiagramStructureFamily;
    const auto family = diagramStructureFamilyForNodeType(node.type);
    std::vector<std::string> ids;
    if (family == Family::Conditional || node.type == "case") {
        for (const auto &r : node.caseStructureRegions) ids.push_back(r.id);
        if (ids.empty()) ids = {std::string(kCaseStructureTrueRegionId), std::string(kCaseStructureFalseRegionId)};
    } else if (family == Family::Event) {
        for (const auto &r : node.eventStructureCases) ids.push_back(r.id);
    } else if (family == Family::Sequence) {
        // A Sequence output belongs to one frame, not to every frame.
        ids.push_back(tunnel.sequenceFrameId);
    } else if (family == Family::Disabled) {
        for (const auto &r : node.disabledStructureRegions) ids.push_back(r.id);
        if (ids.empty()) ids = {std::string(kDisabledStructureEnabledRegionId), std::string(kDisabledStructureDisabledRegionId)};
    } else if (family == Family::ConditionalDisable) {
        for (const auto &r : node.conditionalDisableCases) ids.push_back(r.id);
    } else if (family) ids.push_back("body");
    return ids;
}
} // namespace frog::native_engine
