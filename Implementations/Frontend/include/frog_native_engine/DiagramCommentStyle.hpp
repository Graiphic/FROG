#pragma once

#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/WidgetInstance.hpp"

namespace frog::native_engine {

[[nodiscard]] inline bool diagramCommentUsesThemeTextColor(const DiagramNode &node) noexcept {
    if (node.commentUseThemeTextColor.has_value()) return *node.commentUseThemeTextColor;
    if (!node.textRgb.has_value()) return true;

    // A legacy authored background is evidence that its accompanying text
    // color was chosen for that surface, even when it matches an old default.
    if (node.textBackgroundRgb.has_value() && !isTransparentColorModel(*node.textBackgroundRgb))
        return false;

    // Compatibility values only. The renderer resolves the current theme's
    // text color; this lookup never mutates the stored RGB or layout.
    switch (*node.textRgb) {
    case 0xFFFFFFU:
    case 0xEEEEEEU:
    case 0xE6E6E6U:
    case 0xD4D4D4U:
    case 0x202020U:
        return true;
    default:
        return false;
    }
}

} // namespace frog::native_engine
