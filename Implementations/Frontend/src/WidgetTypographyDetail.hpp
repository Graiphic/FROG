#pragma once
// Pure typography normalization shared by the existing getters and setters.
#include "frog_native_engine/DocumentTextDefaults.hpp"
#include <algorithm>
#include <cmath>
#include <string_view>
namespace frog::native_engine::widget_typography_detail {
inline constexpr std::string_view kPlainTextStyle = "plain";
[[nodiscard]] inline double normalizedFontScale(double scale) noexcept {
    if (!std::isfinite(scale)) {
        return 1.0;
    }

    return std::clamp(scale, 6.0 / kDocumentTextBasePointSize, 144.0 / kDocumentTextBasePointSize);
}
}
