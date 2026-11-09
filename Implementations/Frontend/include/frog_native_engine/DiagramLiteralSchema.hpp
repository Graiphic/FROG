#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/ArrayViewportResize.hpp"
#include <algorithm>
#include <span>
#include <utility>

namespace frog::native_engine {
inline bool isDiagramClusterConstant(const DiagramNode &node) noexcept {
    return node.kind=="primitive"&&(node.type=="frog.studio.cluster.constant"||node.type=="frog.studio.error.constant");
}
inline bool isDiagramReferenceConstant(const DiagramNode &node) noexcept {
    return node.kind=="primitive"&&node.applicationControl&&node.applicationControl->kind==ApplicationControlKind::Reference;
}
inline std::string_view diagramLiteralValuePortId(const DiagramNode &node) noexcept {
    return isDiagramReferenceConstant(node)?"reference_out":"value";
}
inline bool isDiagramLiteralConstant(const DiagramNode &node) noexcept {
    if(node.applicationControl)return isDiagramReferenceConstant(node);
    return node.kind=="primitive"&&(node.type.find("constant")!=std::string::npos||
        node.type=="numeric"||node.type=="string"||node.type=="bool");
}
inline std::vector<const DiagramNode *> diagramClusterConstantFields(std::span<const DiagramNode> nodes, std::string_view id) {
    std::vector<const DiagramNode *> fields;
    for (const auto &node:nodes) if (node.constantOwnerNodeId==id) fields.push_back(&node);
    std::stable_sort(fields.begin(),fields.end(),[](const auto *a,const auto *b){return a->constantFieldOrder<b->constantFieldOrder;});
    return fields;
}
inline std::pair<unsigned,unsigned> draftArrayConstantVisibleGrid(const DiagramNode &node) {
    const auto count=[&](std::size_t axis) {
        return axis<node.numericArrayVisibleCounts.size()
            ? std::clamp(node.numericArrayVisibleCounts[axis],1U,kMaximumArrayVisibleCells) : 1U;
    };
    const bool vertical=node.numericArrayRank==1 && node.numericArrayOneDimensionalVertical;
    const auto grid=boundedArrayViewportGrid(
        {vertical?count(0):node.numericArrayRank>1?count(1):1U,vertical?1U:count(0)});
    return {grid.columns,grid.rows};
}
inline void normalizeDraftArrayConstantVisibleCounts(DiagramNode &node) {
    for(auto &count:node.numericArrayVisibleCounts)
        count=std::clamp(count,1U,kMaximumArrayVisibleCells);
    const auto [columns,rows]=draftArrayConstantVisibleGrid(node);
    if(!node.numericArrayVisibleCounts.empty())node.numericArrayVisibleCounts[0]=
        node.numericArrayRank==1 && node.numericArrayOneDimensionalVertical?rows:columns;
    if(node.numericArrayRank>1 && node.numericArrayVisibleCounts.size()>1)node.numericArrayVisibleCounts[1]=rows;
}
}
