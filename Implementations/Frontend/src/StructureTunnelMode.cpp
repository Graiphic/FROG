#include "frog_native_engine/StructureTunnelMode.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include <utility>
namespace frog::native_engine {
bool diagramTunnelSupportsModes(const DiagramNode &node,
    const DiagramCaseStructureTunnel &tunnel) noexcept {
    return tunnel.shiftRegisterPairId.empty() &&
        (node.type == "frog.studio.structures.for_loop" ||
         node.type == "frog.studio.structures.while_loop");
}
binding::TypeResolution diagramTunnelOutputType(const DiagramNode &node,
    const DiagramCaseStructureTunnel &tunnel, binding::TypeRef source) {
    using namespace binding;
    const auto invalid = [](const char *message) {
        return TypeResolution{ResolutionStatus::Incompatible, {}, "binding.tunnel_mode", message};
    };
    if (tunnel.mode == DiagramTunnelMode::Invalid)
        return invalid("Unknown tunnel mode.");
    if(tunnel.indexingPolicy==DiagramTunnelIndexingPolicy::Invalid)return invalid("Unknown tunnel indexing policy.");
    if ((tunnel.mode != DiagramTunnelMode::LastValue || tunnel.conditional) &&
        !diagramTunnelSupportsModes(node, tunnel))
        return invalid("Tunnel modes require a For/While loop tunnel, not a shift register.");
    if (tunnel.conditional && !tunnel.outputFromStructure)
        return invalid("Only loop output tunnels can be conditional.");
    if (tunnel.mode == DiagramTunnelMode::Concatenating && !tunnel.outputFromStructure)
        return invalid("Only loop outputs can concatenate.");
    if (!source.known())
        return {ResolutionStatus::Unresolved, {}, "binding.type_unresolved", "Wire the tunnel data input."};
    if (tunnel.mode == DiagramTunnelMode::Concatenating && source.kind != TypeKind::Array &&
        source != TypeRef::primitive("string"))
        return invalid("Concatenating requires an array or a string.");
    if (tunnel.mode == DiagramTunnelMode::Concatenating && source.kind == TypeKind::Array)
        source.extent.reset(); // Accumulation does not retain the input's fixed length.
    if (tunnel.mode == DiagramTunnelMode::Indexing) {
        if(tunnel.indexingPolicy==DiagramTunnelIndexingPolicy::AtomicElements&&node.type!="frog.studio.structures.for_loop")return invalid("Atomic element indexing requires a For loop.");
        return diagramIndexedTunnelType(std::move(source),tunnel.outputFromStructure,tunnel.indexingPolicy);
    }
    return {ResolutionStatus::Compatible, std::move(source), {}, {}};
}
} // namespace frog::native_engine
