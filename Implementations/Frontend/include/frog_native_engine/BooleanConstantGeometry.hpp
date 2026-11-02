#pragma once

#include "frog_native_engine/Geometry.hpp"
#include <cmath>
#include <string_view>

namespace frog::native_engine {
// The authored T/F capsule occupies 36.3 x 24.8 including its stroke.
// Crop the unused 48 px SVG canvas, retaining less than two pixels per side.
inline constexpr double kBooleanConstantWidth = 40.0;
inline constexpr double kBooleanConstantHeight = 28.0;
inline constexpr double kBooleanConstantViewX = 4.0;
inline constexpr double kBooleanConstantViewY = 10.0;

inline bool isBooleanConstantType(std::string_view type) noexcept {
    if (type.starts_with("frog.studio.")) type.remove_prefix(12);
    return type == "boolean.true_constant" || type == "boolean.false_constant";
}

inline Rect booleanConstantBounds(Rect bounds) noexcept {
    bounds.width = kBooleanConstantWidth;
    bounds.height = kBooleanConstantHeight;
    return bounds;
}

// Replace shipped defaults, including the former miniature 24 x 18 preset.
// Preserve explicitly authored rectangles instead of forcing every Boolean
// to a fixed size during document loading.
inline bool compactLegacyBooleanConstantBounds(Rect &bounds) noexcept {
    const auto equal = [](double a, double b) { return std::abs(a - b) < 0.001; };
    if (!((equal(bounds.width, 48) && equal(bounds.height, 48)) ||
          (equal(bounds.width, 24) && equal(bounds.height, 24)) ||
          (equal(bounds.width, 24) && equal(bounds.height, 18)))) return false;
    bounds = booleanConstantBounds(bounds);
    return true;
}
}
