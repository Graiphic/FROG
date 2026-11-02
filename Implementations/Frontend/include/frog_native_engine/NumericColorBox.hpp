#pragma once
#include "frog_native_engine/WidgetInstance.hpp"
#include "frog_native_engine/WidgetFamilyEngine.hpp"
#include <algorithm>
#include <cmath>
#include <cstdint>

namespace frog::native_engine {

inline constexpr double kColorBoxWidgetSide = 60.0;
inline constexpr double kColorBoxConstantSide = 24.0;
inline Rect fixedColorBoxBounds(Rect bounds, double side) noexcept {
    bounds.width = bounds.height = side;
    return bounds;
}

// A Color Box has the same scalar U32 data as a numeric. Presentation never
// turns that value into a string, RGB tuple or a new nominal binding type.
inline void normalizeColorBoxGeometry(WidgetInstance &widget, double side = kColorBoxWidgetSide,
    unsigned depth = 0) {
    if (depth > 32) return;
    if (isNumericColorBox(widget)) widget.bounds = fixedColorBoxBounds(widget.bounds, side);
    if (auto *array = arrayContainerState(widget); array && array->elementTemplate) {
        normalizeColorBoxGeometry(*array->elementTemplate, side, depth + 1);
        if (isNumericColorBox(*array->elementTemplate)) array->cellWidth = array->cellHeight = side;
    }
    if (auto *cluster = clusterContainerState(widget))
        for (auto &field : cluster->elements)
            if (field.widget) normalizeColorBoxGeometry(*field.widget, side, depth + 1);
}
inline bool isColorBoxPaletteEntry(const WidgetCatalogEntry &entry) noexcept {
    return entry.family == WidgetFamily::Numeric && entry.valueType == "u32" &&
        ((entry.displayName == "Color Box Control" && entry.role == "control") ||
         (entry.displayName == "Color Box Indicator" && entry.role == "indicator"));
}
inline std::uint32_t colorBoxU32Value(double value) noexcept {
    return std::isfinite(value) ? static_cast<std::uint32_t>(
        std::clamp(std::trunc(value), 0.0, 4294967295.0)) : 0U;
}

} // namespace frog::native_engine
