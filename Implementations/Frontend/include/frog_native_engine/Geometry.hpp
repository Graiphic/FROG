#pragma once

namespace frog::native_engine {

struct Point {
    double x = 0.0;
    double y = 0.0;
};

struct Size {
    double width = 0.0;
    double height = 0.0;
};

struct Rect {
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;

    [[nodiscard]] double right() const noexcept;
    [[nodiscard]] double bottom() const noexcept;
    [[nodiscard]] bool contains(Point point) const noexcept;
};

enum class DragAxisConstraint {
    None,
    Horizontal,
    Vertical
};

[[nodiscard]] Point snapToGrid(Point point, double pitch) noexcept;
[[nodiscard]] Point applyDragAxisConstraint(
    Point delta,
    Point rawDelta,
    bool shiftPressed,
    DragAxisConstraint &constraint) noexcept;
[[nodiscard]] Rect boundsFromOppositeCorners(
    Point first,
    Point second) noexcept;

} // namespace frog::native_engine
