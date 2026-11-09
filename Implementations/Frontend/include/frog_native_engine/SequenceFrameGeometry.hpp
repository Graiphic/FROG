#pragma once
#include "frog_native_engine/SequenceStructure.hpp"
#include <algorithm>
namespace frog::native_engine {
// Persisted frame width uses the same neutral geometry rule as the editor.
// The final frame consumes the remaining authored structure width.
[[nodiscard]] inline Rect sequenceFrameLayout(const DiagramNode &n, std::size_t i) noexcept {
    double x=n.layout.x;
    for (std::size_t j=0; j<i && j<n.sequenceFrames.size(); ++j)
        x+=(std::max)(kSequenceMinimumFrameWidth,n.sequenceFrames[j].width);
    if (i>=n.sequenceFrames.size()) return {x,n.layout.y,0,n.layout.height};
    const double width=i+1==n.sequenceFrames.size() ? n.layout.right()-x : n.sequenceFrames[i].width;
    return {x,n.layout.y,(std::max)(kSequenceMinimumFrameWidth,width),n.layout.height};
}
}
