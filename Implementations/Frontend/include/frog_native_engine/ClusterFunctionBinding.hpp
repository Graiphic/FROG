#pragma once
#include "frog_native_engine/FunctionPortIdentities.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/BindingType.hpp"
#include <algorithm>
#include <charconv>

namespace frog::native_engine {









// Both authored Cluster Array SVGs share this intrinsic repeating strip.
// These design units also govern resize snapping and terminal hit positions.


inline double clusterFunctionTextPointSize(const DiagramNode &node) {
    return normalizedDocumentTextPointSize(node.fontPointSizeUsesDefault?
        .75*documentDefaultTextPointSize():node.fontPointSize);
}
inline double clusterFunctionRowPitch(const DiagramNode &node) {
    if(isClusterArrayStrip(node.type))return kClusterArrayStripRowHeight;
    if(isClusterByName(node.type)&&std::isfinite(node.clusterFunctionMeasuredRowPitch)&&node.clusterFunctionMeasuredRowPitch>0)
        return node.clusterFunctionMeasuredRowPitch;
    // Positional records have no text rows; share the authored Array strip pitch.
    return kClusterArrayStripRowHeight;
}

inline unsigned clusterFunctionCount(const DiagramNode &node) {
    return std::clamp(node.clusterFunctionTerminals ? node.clusterFunctionTerminals :
        (isClusterByName(node.type)?1U:2U),1U,32U);
}


inline double clusterFunctionRowsTop(const DiagramNode &node) {
    if(isClusterArrayStrip(node.type)) return 0;
    return isClusterByName(node.type)?0.4:2.0;
}
inline double clusterFunctionHeight(const DiagramNode &node) {
    if(isClusterArrayStrip(node.type)) return clusterFunctionRowPitch(node)*clusterFunctionCount(node);
    // The 0.8-unit border belongs to the authored artwork. Named rows share
    // measured text pitch with their ports, hit targets and resize snapping.
    return isClusterByName(node.type)?clusterFunctionRowPitch(node)*clusterFunctionCount(node)+0.8:
        clusterFunctionRowPitch(node)*clusterFunctionCount(node)+4.0;
}
inline std::string_view clusterFunctionDisplayName(const DiagramNode &node,unsigned row) {
    if(row>=node.clusterFieldNames.size()) return {};
    auto name=std::string_view(node.clusterFieldNames[row]);
    if(!node.clusterShowFullNames) {
        const auto separator=name.rfind('.');
        if(separator!=std::string_view::npos) name.remove_prefix(separator+1);
    }
    return name;
}
inline double clusterFunctionWidth(const DiagramNode &node) {
    if(isClusterArrayStrip(node.type)) return kClusterArrayStripWidth;
    if(isClusterByName(node.type)&&std::isfinite(node.clusterFunctionMeasuredWidth)&&node.clusterFunctionMeasuredWidth>0)
        return node.clusterFunctionMeasuredWidth;
    double width=isClusterByName(node.type)?67.42:32;
    if(isClusterByName(node.type)) for(unsigned i=0;i<clusterFunctionCount(node);++i)
        width=(std::max)(width,44.0+8.0*clusterFunctionDisplayName(node,i).size());
    return (std::min)(width,400.0);
}

inline double clusterFunctionPortX(const DiagramNode &node,const ClusterFunctionPort &port) {
    if(isClusterArrayStrip(node.type)) return port.output?kClusterArrayStripWidth-.665:.665;
    if(port.top) return isClusterByName(node.type)?clusterFunctionWidth(node)-12.4:clusterFunctionWidth(node)*2.0/3.0;
    return port.output?clusterFunctionWidth(node)-2:2;
}
inline double clusterFunctionRowPortY(const DiagramNode &node,unsigned row) {
    // Each field contact follows the centre of its displayed row. Field order
    // and identity are independent of this presentation coordinate.
    return clusterFunctionRowsTop(node)+clusterFunctionRowPitch(node)*row+
        clusterFunctionRowPitch(node)*.5;
}
inline std::vector<ClusterFunctionPort> clusterFunctionPorts(const DiagramNode &node) {
    std::vector<ClusterFunctionPort> result;
    const auto id=clusterFunctionId(node.type);
    if(!isClusterFunction(id)) return result;
    const bool record=isClusterRecordFunction(id), unbundle=isClusterUnbundleFunction(id);
    const double clusterY=clusterFunctionHeight(node)/2;
    if(record) result.push_back({"cluster",false,unbundle?clusterY:0.9,!unbundle});
    for(unsigned row=0;row<clusterFunctionCount(node);++row)
        result.push_back({clusterFunctionRowId(row),unbundle,
            clusterFunctionRowPortY(node,row)});
    if(!unbundle) result.push_back({record?"result":"array",true,clusterY});
    return result;
}

// A missing or ambiguous name is never replaced with a positional selection.

// A displayed field path is resolved by its full, nominal name. Duplicate
// names at any level are ambiguous, including a direct name containing '.'.





// Preorder follows each Cluster's authored order. Display names may repeat;
// every occurrence has its own stable identity and current ordinal path.


// Positional rows select the current authored ordinal. By Name rows retain
// their stored stable IDs across reordering and labels remain presentation.
inline std::vector<std::string> clusterRecordFieldIdPath(const DiagramNode &node,
    const binding::TypeRef &schema, std::size_t row) {
    if(schema.kind!=binding::TypeKind::Cluster) return {};
    if(isClusterRecordFunction(node.type) && !isClusterByName(node.type))
        return row<schema.fieldIds.size() && row<schema.children.size()
            ? std::vector<std::string>{schema.fieldIds[row]} : std::vector<std::string>{};
    if(row<node.clusterFieldIdPaths.size() && !node.clusterFieldIdPaths[row].empty())
        return clusterFieldByIdPath(schema,node.clusterFieldIdPaths[row])
            ? node.clusterFieldIdPaths[row] : std::vector<std::string>{};
    return row<node.clusterFieldNames.size()
        ? clusterNamedFieldIdPath(schema,node.clusterFieldNames[row]) : std::vector<std::string>{};
}
inline const binding::TypeRef *clusterSelectedField(const DiagramNode &node,
    const binding::TypeRef &schema, std::size_t row) {
    return clusterFieldByIdPath(schema,clusterRecordFieldIdPath(node,schema,row));
}
}
