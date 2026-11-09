#pragma once
#include "frog_native_engine/FunctionPortIdentities.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include <algorithm>
#include <charconv>
#include <span>

namespace frog::native_engine {
inline bool isBuildArray(const DiagramNode &node) noexcept {
    return node.kind=="primitive" && (node.type=="frog.studio.array.build" || node.type=="array.build");
}
inline unsigned buildArrayInputCount(const DiagramNode &node) noexcept {
    return std::clamp(node.buildArrayInputs,1U,32U);
}


// Intrinsic geometry of Build array_element.svg; each new input adds one band.









}
