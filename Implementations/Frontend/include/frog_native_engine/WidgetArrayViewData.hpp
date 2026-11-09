#pragma once
#include "WidgetInstance.hpp"
#include "ArrayViewportResize.hpp"
#include <algorithm>
namespace frog::native_engine {
struct ArrayVisibleGrid {
    std::uint32_t rows = 1U;
    std::uint32_t columns = 1U;
};

[[nodiscard]] inline ArrayVisibleGrid arrayVisibleGrid(const ArrayContainerState &state) noexcept {
    ArrayVisibleGrid grid;
    if (state.rank <= 1U) {
        const auto count = state.visibleCounts.empty() ? 1U : (std::max)(1U, state.visibleCounts.front());
        if (state.orientation == "horizontal") {
            grid.columns = count;
        } else {
            grid.rows = count;
        }
    } else {
        const auto minimum = minimumArrayVisibleAxisCount(state.rank);
        grid.rows = state.visibleCounts.empty() ? minimum : (std::max)(minimum, state.visibleCounts[0]);
        grid.columns = state.visibleCounts.size() < 2U
            ? minimum
            : (std::max)(minimum, state.visibleCounts[1]);
    }
    const auto bounded=boundedArrayViewportGrid({grid.rows,grid.columns});
    return {bounded.rows,bounded.columns};
}




// These dimensions mirror the published Default Array SVG subobjects.
inline constexpr double kArrayPlacementAuraExtent = 8.0;
inline constexpr double kArrayScrollbarThickness = 18.0;
inline constexpr double kArraySubcontainerPadding = 4.0;

[[nodiscard]] inline bool arrayVerticalScrollbarIsVisible(const ArrayContainerState &state) noexcept {
    return state.verticalScrollbarVisible && (state.rank > 1U || state.orientation != "horizontal");
}

[[nodiscard]] inline bool arrayHorizontalScrollbarIsVisible(const ArrayContainerState &state) noexcept {
    return state.horizontalScrollbarVisible && (state.rank > 1U || state.orientation == "horizontal");
}

[[nodiscard]] inline double arrayIndexDisplayHeight(const ArrayContainerState &state) noexcept {
    if (!state.indexDisplayVisible || state.rank == 0U) {
        return 0.0;
    }
    return static_cast<double>(state.rank) * state.indexBoxHeight +
        static_cast<double>(state.rank - 1U) * state.indexBoxGap;
}







}
