#include "frog_native_engine/Geometry.hpp"
namespace frog::native_engine {
double Rect::right() const noexcept {
    return x + width;
}
double Rect::bottom() const noexcept {
    return y + height;
}
}
