#pragma once
#include <algorithm>
#include <cmath>
#include <concepts>
#include <limits>
#include <optional>
#include <string_view>

namespace frog::native_engine::horizontal_slider {
inline constexpr std::string_view asset = "asset:numeric_horizontal_slider_svg";

inline double fraction(double value, double low, double high) noexcept {
    if (!std::isfinite(value) || !std::isfinite(low) || !std::isfinite(high) || low >= high) return 0;
    value = std::clamp(value, low, high);
    if (low < 0 && high > 0 && !std::isfinite(high-low))
        return (value/2-low/2)/(high/2-low/2);
    return (value-low)/(high-low);
}

inline double valueAt(double position, double low, double high, double step) noexcept {
    if (!std::isfinite(low) || !std::isfinite(high) || high <= low) return low;
    if (position <= 0) return low;
    if (position >= 1) return high;
    const double value = std::lerp(low, high, position);
    const double steps = (value-low)/step;
    if (!(step > 0) || !std::isfinite(steps)) return value;
    return std::clamp(std::fma(std::round(steps), step, low), low, high);
}

// A binary64 rail rounds the native I64/U64 maximum up to the next power
// of two. Map that one endpoint to its exact typed value before converting.
template<std::integral Integer>
inline std::optional<Integer> integerAt(double value) noexcept {
    constexpr auto low = (std::numeric_limits<Integer>::lowest)();
    constexpr auto high = (std::numeric_limits<Integer>::max)();
    if (!std::isfinite(value) || std::trunc(value) != value ||
        value < static_cast<double>(low) || value > static_cast<double>(high)) return {};
    if (value == static_cast<double>(high)) return high;
    return static_cast<Integer>(value);
}
} // namespace frog::native_engine::horizontal_slider

namespace frog::native_engine {
struct WidgetInstance;
namespace numeric_instrument {
enum class Kind { None, HorizontalSlider, VerticalSlider, HorizontalRange, VerticalRange, Knob, Meter, Gauge, Tank };

constexpr Kind kind(std::string_view asset) noexcept {
    if (asset == horizontal_slider::asset) return Kind::HorizontalSlider;
    if (asset == "asset:numeric_vertical_slider_svg") return Kind::VerticalSlider;
    if (asset == "asset:numeric_horizontal_range_slider_svg") return Kind::HorizontalRange;
    if (asset == "asset:numeric_vertical_range_slider_svg") return Kind::VerticalRange;
    if (asset == "asset:numeric_knob_svg") return Kind::Knob;
    if (asset == "asset:numeric_meter_svg") return Kind::Meter;
    if (asset == "asset:numeric_gauge_svg") return Kind::Gauge;
    if (asset == "asset:numeric_tank_svg") return Kind::Tank;
    return Kind::None;
}

constexpr bool isSingleSlider(Kind instrument) noexcept {
    return instrument == Kind::HorizontalSlider || instrument == Kind::VerticalSlider;
}

constexpr bool isRange(Kind instrument) noexcept {
    return instrument == Kind::HorizontalRange || instrument == Kind::VerticalRange;
}

double value(const WidgetInstance &widget, bool upper = false) noexcept;
bool setValue(WidgetInstance &widget, double selected, bool upper = false);
bool setRepresentation(WidgetInstance &widget, std::string_view representation);
} // namespace numeric_instrument
} // namespace frog::native_engine
