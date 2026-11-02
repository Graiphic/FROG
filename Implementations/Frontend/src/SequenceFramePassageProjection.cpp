#include "frog_native_engine/SequenceFramePassageProjection.hpp"
#include "frog_native_engine/SequenceStructure.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include <algorithm>
#include <unordered_set>
namespace frog::native_engine {
std::string sequenceFrameOfEndpoint(const DiagramNode &sequence,const std::vector<DiagramNode> &nodes,
    const DiagramEdgeEndpoint &endpoint) {
    if(endpoint.nodeId==sequence.id) {
        for(const auto &t:sequence.caseStructureTunnels)
            if(const auto frame=sequenceTunnelPortFrameId(t,endpoint.portId))return std::string(*frame);
        return {};
    }
    auto id=endpoint.nodeId;
    std::unordered_set<std::string> seen;
    while(!id.empty() && seen.insert(id).second) {
        const auto found=std::find_if(nodes.begin(),nodes.end(),[&](const auto &n){return n.id==id;});
        if(found==nodes.end())return {};
        if(found->diagramOwnerStructureId==sequence.id)return found->diagramOwnerRegionId;
        id=found->constantOwnerNodeId.empty()?found->diagramOwnerStructureId:found->constantOwnerNodeId;
    }
    return {};
}
SequenceFramePassageProjection projectSequenceFramePassagesForExecution(std::vector<DiagramNode> &documentNodes,std::vector<DiagramEdge> &documentEdges) {
    SequenceFramePassageProjection result;
    if(std::none_of(documentNodes.begin(),documentNodes.end(),[](const auto &node) {
        return std::any_of(node.caseStructureTunnels.begin(),node.caseStructureTunnels.end(),[](const auto &t) {
            return !t.sequenceLegacyFrameId.empty() || !t.sequenceLegacyEdgeId.empty();
        });
    }))return result;
    auto nodes=documentNodes;auto edges=documentEdges;
    for(auto &node:nodes) {
        std::unordered_set<std::string> groups;
        for(const auto &t:node.caseStructureTunnels) {
            if(t.sequenceLegacyFrameId.empty() && t.sequenceLegacyEdgeId.empty())continue;
            if(!isSequenceStructure(node) || t.mode!=DiagramTunnelMode::LastValue || t.conditional ||
                !t.shiftRegisterPairId.empty() || (!t.sequenceLegacyFrameId.empty() &&
                (t.sequenceLegacyEdgeId.empty() || !t.sequenceOuterFrameId.empty() ||
                 !sequenceFrameIndex(node,t.sequenceLegacyFrameId)))) {
                result.error="Invalid migrated Sequence corridor provenance: "+node.id+":"+t.id;return result;
            }
            groups.insert(t.sequenceLegacyEdgeId);
        }
        for(const auto &group:groups) {
            auto candidate=node;auto candidateEdges=edges;
            std::vector<std::string> passages;
            std::unordered_set<std::string> changedFaces,touchedEdges;
            bool exact=true;
            for(auto &t:candidate.caseStructureTunnels)if(t.sequenceLegacyEdgeId==group) {
                if(!t.sequenceLegacyFrameId.empty()) {
                    t.sequenceFrameId=t.sequenceLegacyFrameId;changedFaces.insert(t.id);
                } else if(!t.sequenceOuterFrameId.empty())passages.push_back(t.id);
                else exact=false;
            }
            // Deleting/reconnecting a migrated corridor is an ordinary edit,
            // not a permanent technical obligation to retain its old graph.
            if(changedFaces.empty() || passages.empty())exact=false;
            for(const auto &id:passages) {
                if(!exact)break;
                const auto input=binding::tunnelInputId(id),output=binding::tunnelOutputId(id);
                std::vector<std::size_t> incoming,outgoing;
                for(std::size_t i=0;i<candidateEdges.size();++i) {
                    const auto &e=candidateEdges[i];
                    if(e.to.nodeId==node.id && e.to.portId==input)incoming.push_back(i);
                    if(e.from.nodeId==node.id && e.from.portId==output)outgoing.push_back(i);
                }
                if(incoming.size()!=1 || outgoing.size()!=1 || incoming.front()==outgoing.front()) {exact=false;break;}
                const auto producer=candidateEdges[incoming.front()].from;
                auto &consumer=candidateEdges[outgoing.front()];
                consumer.from=producer;touchedEdges.insert(consumer.id);
                candidateEdges.erase(candidateEdges.begin()+static_cast<std::ptrdiff_t>(incoming.front()));
            }
            if(exact) {
                std::erase_if(candidate.caseStructureTunnels,[&](const auto &t) {
                    return t.sequenceLegacyEdgeId==group && t.sequenceLegacyFrameId.empty();
                });
                for(auto &edge:candidateEdges) {
                    bool affected=touchedEdges.contains(edge.id);
                    for(const auto &t:candidate.caseStructureTunnels)if(changedFaces.contains(t.id))
                        affected=affected || (edge.from.nodeId==node.id && edge.from.portId==binding::tunnelOutputId(t.id)) ||
                            (edge.to.nodeId==node.id && edge.to.portId==binding::tunnelInputId(t.id));
                    if(!affected)continue;
                    const auto from=sequenceFrameOfEndpoint(candidate,nodes,edge.from),to=sequenceFrameOfEndpoint(candidate,nodes,edge.to);
                    if(from!=to) {exact=false;break;}
                    edge.diagramOwnerStructureId=from.empty()?candidate.diagramOwnerStructureId:candidate.id;
                    edge.diagramOwnerRegionId=from.empty()?candidate.diagramOwnerRegionId:from;
                }
            }
            if(exact) {node=std::move(candidate);edges=std::move(candidateEdges);}
            for(auto &t:node.caseStructureTunnels)if(t.sequenceLegacyEdgeId==group) {
                t.sequenceLegacyFrameId.clear();t.sequenceLegacyEdgeId.clear();
            }
            result.changed=true;
        }
    }
    if(result.changed) {documentNodes=std::move(nodes);documentEdges=std::move(edges);}
    return result;
}
}
