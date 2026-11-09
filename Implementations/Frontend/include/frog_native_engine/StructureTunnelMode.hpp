#pragma once

#include "frog_native_engine/DiagramExecutionContract.hpp"
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/IndexedTunnelType.hpp"

namespace frog::native_engine {
struct DiagramNode;
struct DiagramCaseStructureTunnel;
inline std::string_view diagramTunnelIndexingPolicyName(DiagramTunnelIndexingPolicy policy)noexcept {
    switch(policy){case DiagramTunnelIndexingPolicy::LeadingDimension:return "leading_dimension";case DiagramTunnelIndexingPolicy::AtomicElements:return "atomic_elements";default:return "invalid";}
}
inline DiagramTunnelIndexingPolicy parseDiagramTunnelIndexingPolicy(std::string_view text)noexcept {
    if(text=="leading_dimension")return DiagramTunnelIndexingPolicy::LeadingDimension;
    if(text=="atomic_elements")return DiagramTunnelIndexingPolicy::AtomicElements;
    return DiagramTunnelIndexingPolicy::Invalid;
}

inline std::string_view diagramTunnelModeName(DiagramTunnelMode mode) noexcept {
    switch (mode) {
    case DiagramTunnelMode::LastValue: return "last_value";
    case DiagramTunnelMode::Indexing: return "indexing";
    case DiagramTunnelMode::Concatenating: return "concatenating";
    default: return "invalid";
    }
}
inline DiagramTunnelMode parseDiagramTunnelMode(std::string_view mode) noexcept {
    if (mode == "last_value") return DiagramTunnelMode::LastValue;
    if (mode == "indexing") return DiagramTunnelMode::Indexing;
    if (mode == "concatenating") return DiagramTunnelMode::Concatenating;
    return DiagramTunnelMode::Invalid;
}
inline std::string diagramTunnelConditionPortId(std::string_view id) {
    return "tunnel:" + std::string(id) + ":condition";
}

// Studio adaptation uses the same indexing/type owner declared above. Keeping
// document-bearing overloads out of this header closes the neutral FIR graph.
[[nodiscard]] bool diagramTunnelSupportsModes(const DiagramNode &,
    const DiagramCaseStructureTunnel &) noexcept;
[[nodiscard]] DiagramTunnelMode diagramNewTunnelModeForSource(const DiagramNode &,
    const DiagramCaseStructureTunnel &,const binding::TypeRef &) noexcept;
[[nodiscard]] binding::TypeResolution diagramTunnelOutputType(const DiagramNode &,
    const DiagramCaseStructureTunnel &,binding::TypeRef);
} // namespace frog::native_engine
