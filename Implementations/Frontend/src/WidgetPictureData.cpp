// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/WidgetPictureData.hpp"
#include <algorithm>
#include <cmath>
namespace frog::native_engine::picture_data {
constexpr double kPictureDisplayFrameWidth=1.0;
constexpr std::uint32_t kMaximumPictureDisplayAxis=4096U;
std::uint32_t axisForBounds(double extent) noexcept {
    if (!std::isfinite(extent)) {
        return 1U;
    }
    const double interior = (std::max)(1.0, std::floor(extent - (2.0 * kPictureDisplayFrameWidth)));
    return static_cast<std::uint32_t>((std::min)(
        interior,
        static_cast<double>(kMaximumPictureDisplayAxis)));
}

}
