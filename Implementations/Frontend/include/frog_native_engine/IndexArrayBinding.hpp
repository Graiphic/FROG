#pragma once
#include "frog_native_engine/FunctionPortIdentities.hpp"
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include <algorithm>
#include <charconv>

namespace frog::native_engine {




inline bool isIndexArray(const DiagramNode &node) noexcept {
    return node.kind == "primitive" &&
        (node.type == "frog.studio.array.index" || node.type == "array.index");
}

// Group zero keeps the historic identifiers. Dimension indices never change
// identity when the source rank changes; removed dimensions leave broken wires.



inline unsigned indexArrayOutputCount(const DiagramNode &node) noexcept {
    return std::clamp(node.indexArrayOutputs, 1U, kIndexArrayMaximumOutputs);
}





}
