#include "frog_native_engine/SequenceStructure.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>
namespace frog::native_engine {
bool isSequenceStructure(const DiagramNode &n) noexcept {
    return n.kind == "primitive" && n.type == kSequenceStructureType;
}
std::optional<std::size_t> sequenceFrameIndex(const DiagramNode &n, std::string_view id) noexcept {
    for (std::size_t i=0; i<n.sequenceFrames.size(); ++i)
        if (n.sequenceFrames[i].id == id) return i;
    return std::nullopt;
}
std::string_view sequenceSelectedFrameId(const DiagramNode &n) noexcept {
    if (const auto i=sequenceFrameIndex(n,n.sequenceSelectedFrameId)) return n.sequenceFrames[*i].id;
    return n.sequenceFrames.empty() ? std::string_view{} : n.sequenceFrames.front().id;
}
std::string_view sequenceTunnelInputFrameId(const DiagramCaseStructureTunnel &t) noexcept {
    return t.outputFromStructure?t.sequenceFrameId:t.sequenceOuterFrameId;
}
std::string_view sequenceTunnelOutputFrameId(const DiagramCaseStructureTunnel &t) noexcept {
    return t.outputFromStructure?t.sequenceOuterFrameId:t.sequenceFrameId;
}
std::optional<std::string_view> sequenceTunnelPortFrameId(
    const DiagramCaseStructureTunnel &t,std::string_view portId) {
    if(portId==binding::tunnelInputId(t.id))return sequenceTunnelInputFrameId(t);
    if(portId==binding::tunnelOutputId(t.id))return sequenceTunnelOutputFrameId(t);
    return {};
}
std::string sequenceFrameOwner(const std::vector<DiagramNode> &nodes, std::string_view root,
    std::string owner, std::string region) {
    for (std::size_t depth=0; !owner.empty() && depth<128; ++depth) {
        if (owner==root) return region;
        const auto it=std::find_if(nodes.begin(),nodes.end(),[&](const auto &n){return n.id==owner;});
        if(it==nodes.end()) return {};
        owner=it->constantOwnerNodeId.empty()?it->diagramOwnerStructureId:it->constantOwnerNodeId;
        region=it->diagramOwnerRegionId;
    }
    return {};
}
std::vector<SequenceDiagnostic> validateSequenceStructure(const DiagramNode &s,
    const std::vector<DiagramNode> &nodes, const std::vector<DiagramEdge> &edges) {
    std::vector<SequenceDiagnostic> out;
    const auto report=[&](std::string code,std::string id,std::string message){
        out.push_back({std::move(code),std::move(id),std::move(message)});};
    if(!isSequenceStructure(s)) return out;
    if(s.sequenceFrames.empty()) report("sequence.no_frames",s.id,"A Sequence requires at least one frame.");
    if(s.sequenceSemanticProfile!="flat_sequence"&&s.sequenceSemanticProfile!="stacked_sequence")
        report("sequence.unsupported_profile",s.id,"The Sequence profile must be flat_sequence or stacked_sequence.");
    std::unordered_set<std::string> ids;
    for(const auto &f:s.sequenceFrames) {
        if(f.id.empty() || !ids.insert(f.id).second) report("sequence.frame_id",s.id,"Frame IDs must be non-empty and unique.");
        if(!std::isfinite(f.width) || f.width<kSequenceMinimumFrameWidth)
            report("sequence.frame_width",s.id,"Frame width is invalid.");
    }
    for(const auto &n:nodes) if(n.diagramOwnerStructureId==s.id && !ids.contains(n.diagramOwnerRegionId))
        report("sequence.unknown_frame",n.id,"The node refers to a missing frame.");
    const auto endpointFrame=[&](const DiagramEdgeEndpoint &p) -> std::string {
        if(p.nodeId==s.id) {
            for(const auto &t:s.caseStructureTunnels) {
                if(const auto frame=sequenceTunnelPortFrameId(t,p.portId))return std::string(*frame);
            }
            return {};
        }
        const auto n=std::find_if(nodes.begin(),nodes.end(),[&](const auto &n){return n.id==p.nodeId;});
        return n==nodes.end()?std::string{}:sequenceFrameOwner(nodes,s.id,n->diagramOwnerStructureId,n->diagramOwnerRegionId);
    };
    for(const auto &e:edges) {
        const auto a=sequenceFrameIndex(s,endpointFrame(e.from)), b=sequenceFrameIndex(s,endpointFrame(e.to));
        if(a && b && *a>*b) report("sequence.backward_read",e.id,"A frame cannot read a value produced by a later frame.");
    }
    for(const auto &t:s.caseStructureTunnels) {
        if(!ids.contains(t.sequenceFrameId)) report("sequence.tunnel_frame",s.id,"A tunnel refers to a missing frame.");
        if(!t.sequenceOuterFrameId.empty()) {
            if(!ids.contains(t.sequenceOuterFrameId))
                report("sequence.tunnel_outer_frame",s.id,"A Sequence passage refers to a missing other frame.");
            const auto from=sequenceFrameIndex(s,sequenceTunnelInputFrameId(t));
            const auto to=sequenceFrameIndex(s,sequenceTunnelOutputFrameId(t));
            if(from && to && *from>=*to)
                report("sequence.backward_read",s.id+":"+t.id,"A Sequence passage must carry a value to a strictly later frame.");
        }
        if(t.mode!=DiagramTunnelMode::LastValue || t.conditional || !t.shiftRegisterPairId.empty())
            report("sequence.tunnel_mode",s.id,"Sequence tunnels do not index, concatenate or hold state.");
        if(t.outputFromStructure || !t.sequenceOuterFrameId.empty()) {
            std::size_t sources=0;
            for(const auto &e:edges) if(e.to.nodeId==s.id && e.to.portId==binding::tunnelInputId(t.id)) ++sources;
            if(sources>1 || (sources==0 && !t.useDefaultIfUnwired))
                report("sequence.output_source",s.id,"Each Sequence output requires one producer or a resolved type default.");
        }
    }
    return out;
}
}
