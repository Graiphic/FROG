#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
namespace frog::native_engine {
struct SequenceFramePassageProjection { bool changed=false; std::string error; };
[[nodiscard]] std::string sequenceFrameOfEndpoint(const DiagramNode &,const std::vector<DiagramNode> &,const DiagramEdgeEndpoint &);
// Immutable-execution projection only; no authoring gesture or layout inference.
SequenceFramePassageProjection projectSequenceFramePassagesForExecution(std::vector<DiagramNode> &,std::vector<DiagramEdge> &);
inline SequenceFramePassageProjection projectSequenceFramePassagesForExecution(FrogDocumentState &document) {
    return projectSequenceFramePassagesForExecution(document.diagramNodes,document.diagramEdges);
}
}
