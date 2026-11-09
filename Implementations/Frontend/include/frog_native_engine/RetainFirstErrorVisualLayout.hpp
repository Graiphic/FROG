#pragma once

#include "frog_native_engine/DiagramShapeCatalog.hpp"
#include <algorithm>
#include <cmath>

namespace frog::native_engine {

// Logical diagram units, shared by layout, ports and the repeated SVG artwork.
// The supplied artwork's X coordinates are authored in a 72-unit viewBox;
// only that horizontal coordinate system is converted, never the row pitch.
struct RetainFirstErrorVisualLayout {
    static constexpr double width = 48.0;
    static constexpr double artworkWidth = 72.0;
    static constexpr double artworkScaleX = width / artworkWidth;
    static constexpr double rowPitch = 18.0;
    static constexpr double legacyRowPitch = 24.0;
    static constexpr double inputX = 2.0;
    static constexpr double outputX = width - inputX;
    static constexpr double collectorX = 52.0 * artworkScaleX;
    unsigned inputCount;

    explicit constexpr RetainFirstErrorVisualLayout(unsigned count = 2) noexcept
        : inputCount(std::clamp(count, 2U, static_cast<unsigned>(kDiagramExtensionMaximumInputCount))) {}

    static unsigned countForHeight(double height) noexcept {
        return countForPitch(height,rowPitch);
    }
    // Documents without explicit arity used the original 24-unit rows. Never
    // decode their height with the compact pitch, including after normalization.
    static unsigned countForLegacyHeight(double height) noexcept {
        return countForPitch(height,legacyRowPitch);
    }
    static unsigned countForPitch(double height,double pitch) noexcept {
        if (!std::isfinite(height)) return 2;
        const auto bounded = std::clamp(height, 2 * pitch, kDiagramExtensionMaximumInputCount * pitch);
        return static_cast<unsigned>(std::llround(bounded / pitch));
    }
    constexpr double height() const noexcept { return inputCount * rowPitch; }
    constexpr double inputY(unsigned row) const noexcept { return (row + .5) * rowPitch; }
    constexpr double outputY() const noexcept { return height() / 2; }
    static constexpr double artworkX(double logicalX) noexcept { return logicalX / artworkScaleX; }
};

}
