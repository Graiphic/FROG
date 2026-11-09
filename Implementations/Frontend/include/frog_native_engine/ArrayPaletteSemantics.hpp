#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/FunctionPortIdentities.hpp"
namespace frog::native_engine {
inline bool isExpandableArrayPalette(const DiagramNode &node) {
    return node.kind=="primitive" && arrayPaletteKind(node.type)!=ArrayPaletteKind::None;
}
inline unsigned arrayPaletteMaximum(const DiagramNode &node) { return arrayPaletteMaximum(node.type); }
inline unsigned arrayPaletteCount(const DiagramNode &node) { return arrayPaletteCount(node.type,node.arrayFunctionTerminals); }
inline std::vector<ArrayPalettePort> arrayPaletteSemanticPorts(const DiagramNode &node) {
    return arrayPalettePorts(node.type,node.arrayFunctionTerminals);
}
inline std::optional<ArrayPalettePort> arrayPaletteSemanticPort(const DiagramNode &node,std::string_view id) {
    auto port=arrayPaletteDeclaredPort(node.type,id);
    return port&&port->group<arrayPaletteCount(node)?port:std::nullopt;
}
inline bool arrayToClusterFunction(const DiagramNode &node) {
    const auto id=arrayPaletteId(node.type);
    return id=="array.conversion.array_to_cluster" || id=="cluster.conversion.array_to_cluster";
}
}
