#pragma once

#include "frog_native_engine/WidgetInstance.hpp"

#include <algorithm>

namespace frog::native_engine {

// placement_bounds is the canonical authored envelope.  The aura band is
// stored separately so selection and hover never change that envelope.
// Four pixels is retained only as the explicit migration value for documents
// authored before realization manifests persisted layout.aura_band_px.
inline constexpr double kDefaultPlacementAuraBandPx = 4.0;

inline double widgetPlacementAuraBandPx(const WidgetInstance &widget) noexcept {
    return (std::max)(0.0, widget.placementAuraBand.value_or(kDefaultPlacementAuraBandPx));
}

} // namespace frog::native_engine
