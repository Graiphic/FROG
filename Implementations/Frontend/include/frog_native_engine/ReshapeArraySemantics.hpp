#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include <algorithm>
namespace frog::native_engine {
inline bool isReshapeArray(const DiagramNode &node) noexcept {
    return node.kind=="primitive" && (node.type=="frog.studio.array.reshape" || node.type=="array.reshape");
}
inline unsigned reshapeArrayDimensionCount(const DiagramNode &node) noexcept {
    return std::clamp(node.reshapeArrayDimensions,1U,16U);
}


}
