#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/FunctionPortIdentities.hpp"
#include <algorithm>
namespace frog::native_engine {
inline bool isArrayEdit(const DiagramNode &node) noexcept {
    return node.kind == "primitive" && arrayEditKind(node.type) != ArrayEditKind::None;
}
inline bool arrayEditExpandable(const DiagramNode &node) noexcept {
    const auto kind = arrayEditKind(node.type);
    return isArrayEdit(node) && (kind == ArrayEditKind::Replace || kind == ArrayEditKind::Insert);
}
inline unsigned arrayEditGroupCount(const DiagramNode &node) noexcept {
    return arrayEditExpandable(node) ? std::clamp(node.arrayEditGroups, 1U, 32U) : 1U;
}








inline std::vector<ArrayEditPortDescriptor> arrayEditPorts(const DiagramNode &node) {
    const auto kind = arrayEditKind(node.type);
    const auto rank = std::clamp(node.arrayEditRank, 1U, 16U), groups = arrayEditGroupCount(node);
    std::vector<ArrayEditPortDescriptor> ports{{"array", {ArrayEditPortKind::Array}}, {"result", {ArrayEditPortKind::Result}}};
    if (kind == ArrayEditKind::Delete) ports.push_back({"length", {ArrayEditPortKind::Length}});
    for (unsigned group = 0; group < groups; ++group) {
        if (kind != ArrayEditKind::Insert || group == 0)
            for (unsigned dim = 0; dim < rank; ++dim) {
                ports.push_back({arrayEditIndexId(group,dim), {ArrayEditPortKind::Index,group,dim}});
                if (kind == ArrayEditKind::Subset) {
                    ports.push_back({arrayEditLengthId(dim), {ArrayEditPortKind::Length,0,dim}});
                }
            }
        if (kind == ArrayEditKind::Replace || kind == ArrayEditKind::Insert) {
            ports.push_back({arrayEditValueId(group), {ArrayEditPortKind::Value,group}});
        }
    }
    if (kind == ArrayEditKind::Delete) ports.push_back({"deleted", {ArrayEditPortKind::Deleted}});
    return ports;
}

}
