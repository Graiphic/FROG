#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

namespace frog::native_engine {

// Display capacity only; independent from Array rank and stored element count.
inline constexpr std::uint32_t kMaximumArrayVisibleCells = 1024U;

struct ArrayViewportResizeGrid {
    std::uint32_t rows{1U};
    std::uint32_t columns{1U};
};

inline ArrayViewportResizeGrid boundedArrayViewportGrid(ArrayViewportResizeGrid grid,
    bool preserveRows = true, std::uint32_t maximum = kMaximumArrayVisibleCells) noexcept {
    maximum = std::clamp(maximum, 1U, kMaximumArrayVisibleCells);
    grid.rows = std::clamp(grid.rows, 1U, maximum);
    grid.columns = std::clamp(grid.columns, 1U, maximum);
    if (preserveRows) grid.columns = (std::min)(grid.columns, maximum / grid.rows);
    else grid.rows = (std::min)(grid.rows, maximum / grid.columns);
    return grid;
}

// Geometry only: resizing an array display must never allocate or reshape data.
// Coordinates and cell pitches use the same (unzoomed) coordinate system.
inline ArrayViewportResizeGrid resizedArrayViewportGrid(
    ArrayViewportResizeGrid original, bool oneDimensional, bool vertical,
    bool moveRight, bool moveBottom, double dx, double dy,
    double cellWidth, double cellHeight, std::uint32_t maximum = kMaximumArrayVisibleCells) noexcept {
    maximum = std::clamp(maximum, 1U, kMaximumArrayVisibleCells);
    const auto axis = [maximum](std::uint32_t count, double delta, double pitch) {
        const double units = std::isfinite(delta) && std::isfinite(pitch) && pitch > 0.0
            ? std::round(delta / pitch) : 0.0;
        return static_cast<std::uint32_t>(std::clamp(
            static_cast<double>(count) + units, 1.0,
            static_cast<double>((std::max)(1U, maximum))));
    };
    ArrayViewportResizeGrid result{
        axis(original.rows, moveBottom ? dy : 0.0, cellHeight),
        axis(original.columns, moveRight ? dx : 0.0, cellWidth)};
    if (oneDimensional) {
        // A single cell may start a horizontal or vertical strip; an existing
        // strip retains its axis until it has been reduced to one cell.
        if (original.rows == 1U && original.columns == 1U) {
            if (result.columns > result.rows) vertical = false;
            else if (result.rows > result.columns) vertical = true;
        }
        if (vertical) result.columns = 1U;
        else result.rows = 1U;
    }
    // A single-axis drag cannot spend the budget by shrinking the other axis.
    // For diagonal/imported grids, rows have deterministic priority.
    return boundedArrayViewportGrid(result, !moveBottom || moveRight, maximum);
}

} // namespace frog::native_engine
