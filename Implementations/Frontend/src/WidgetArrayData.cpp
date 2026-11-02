// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/WidgetArrayViewData.hpp"
#include "frog_native_engine/ContainedWidgetSchemaMetadata.hpp"
#include <algorithm>
#include <utility>
namespace frog::native_engine {
bool installPreparedArrayElementTemplate(WidgetInstance &array,
    const WidgetInstance &element, std::vector<ArrayElementValue> preparedElements) {
    // The pure API is independently callable. Reject ownership cycles before
    // recursively copying either graph, using the existing family schema walk.
    if (!isArrayWidget(array) || !isSupportedArrayElementWidget(element) ||
        containedWidgetSchemaHasCycle(array) || containedWidgetSchemaHasCycle(element))
        return false;
    auto draft=array;
    auto *state = arrayContainerState(draft);
    if (state == nullptr) {
        return false;
    }

    const bool elementHasPublicBinding =
        (element.publicInputId.has_value() && !element.publicInputId->empty()) ||
        (element.publicOutputId.has_value() && !element.publicOutputId->empty()) ||
        (element.interfaceMapSlotId.has_value() && !element.interfaceMapSlotId->empty());
    if (elementHasPublicBinding) {
        draft.publicInputId = element.publicInputId;
        draft.publicOutputId = element.publicOutputId;
        draft.interfaceMapSlotId = element.interfaceMapSlotId;
        draft.interfaceConnectionRequirement = element.interfaceConnectionRequirement;
    }

    auto contained = std::make_shared<WidgetInstance>(element);
    contained->instanceId = array.instanceId + ".element";
    contained->bounds = Rect{ 0.0, 0.0, element.bounds.width, element.bounds.height };
    contained->labelVisible = false;
    contained->publicInputId.reset();
    contained->publicOutputId.reset();
    contained->interfaceMapSlotId.reset();
    contained->interfaceConnectionRequirement.reset();

    state->cellWidth = (std::max)(24.0, element.bounds.width);
    state->cellHeight = (std::max)(20.0, element.bounds.height);
    state->widgetComposedElements = true;
    state->elementClassId = element.classId;
    state->elementValueType = element.valueType;
    state->elementRole = element.role;
    state->elementTemplate = std::move(contained);

    draft.role = element.role == "indicator" ? "indicator" : "control";
    draft.classId = draft.role == "indicator"
        ? "frog.widgets.array_indicator"
        : "frog.widgets.array_control";
    draft.valueType = "frog.array<" + element.valueType + ",rank=" +
        std::to_string(state->rank) + ">";
    state->elements = std::move(preparedElements);
    array=std::move(draft);
    return true;
}

bool setArrayRank(WidgetInstance &array, std::uint32_t rank) {
    auto *state = arrayContainerState(array);
    if (state == nullptr) {
        return false;
    }

    rank = std::clamp(rank, 1U, 16U);
    if (rank == state->rank) {
        return false;
    }

    const auto visible = arrayVisibleGrid(*state);
    if (rank == 1U) {
        // Preserve a displayed strip, including one chosen while in 2D/ND.
        // For a full grid, reuse the user's last 1D orientation.
        if (visible.rows == 1U && visible.columns > 1U) {
            state->orientation = "horizontal";
        } else if (visible.columns == 1U && visible.rows > 1U) {
            state->orientation = "vertical";
        }
    }
    state->shape.resize((std::max)(1U, state->rank), 1U);
    state->firstIndices.resize((std::max)(1U, state->rank), 0U);

    while (state->rank < rank) {
        // Keep the existing 1D display axis when it becomes a 2D slice.
        const std::size_t addedAxis = state->rank == 1U && state->orientation != "horizontal" ? 1U : 0U;
        state->shape.insert(state->shape.begin() + addedAxis, 1U);
        state->firstIndices.insert(state->firstIndices.begin() + addedAxis, 0U);
        for (auto &element : state->elements) {
            element.indices.resize(state->rank, 0U);
            element.indices.insert(element.indices.begin() + addedAxis, 0U);
        }
        ++state->rank;
    }

    while (state->rank > rank) {
        // Show the selected slice, never flatten a matrix into a longer strip.
        const std::size_t removedAxis = state->rank == 2U && state->orientation != "horizontal" ? 1U : 0U;
        const std::uint32_t selectedSlice = state->firstIndices[removedAxis];
        state->shape.erase(state->shape.begin() + removedAxis);
        state->firstIndices.erase(state->firstIndices.begin() + removedAxis);
        state->elements.erase(
            std::remove_if(
                state->elements.begin(),
                state->elements.end(),
                [selectedSlice, removedAxis, previousRank = state->rank](ArrayElementValue &element) {
                    element.indices.resize(previousRank, 0U);
                    if (element.indices[removedAxis] != selectedSlice) {
                        return true;
                    }
                    element.indices.erase(element.indices.begin() + removedAxis);
                    return false;
                }),
            state->elements.end());
        --state->rank;
    }

    if (state->rank <= 1U) {
        state->visibleCounts = { state->orientation == "horizontal" ? visible.columns : visible.rows };
    } else {
        state->visibleCounts = { visible.rows, visible.columns };
    }

    state->shape.resize(state->rank, 1U);
    state->firstIndices.resize(state->rank, 0U);
    array.valueType = "frog.array<" +
        (state->elementValueType.empty() ? std::string{ "u16" } : state->elementValueType) +
        ",rank=" + std::to_string(state->rank) + ">";
    return true;
}

Size minimumArrayBoundsForVisibleCounts(const WidgetInstance &array) noexcept {
    const auto *state = arrayContainerState(array);
    if (state == nullptr) {
        return Size{};
    }

    const auto grid = arrayVisibleGrid(*state);
    const double indexWidth = state->indexDisplayVisible
        ? state->indexDisplayWidth +
            (2.0 * (state->indexContainerPadding + state->indexContainerBorderWidth)) +
            state->scrollbarGap
        : 0.0;
    const double verticalScrollbarWidth = arrayVerticalScrollbarIsVisible(*state)
        ? state->scrollbarGap + kArrayScrollbarThickness
        : 0.0;
    const double horizontalScrollbarHeight = arrayHorizontalScrollbarIsVisible(*state)
        ? state->scrollbarGap + kArrayScrollbarThickness
        : 0.0;
    const double viewportWidth =
        static_cast<double>(grid.columns) * state->cellWidth +
        static_cast<double>(grid.columns > 0U ? grid.columns - 1U : 0U) * state->elementGap;
    const double viewportHeight =
        static_cast<double>(grid.rows) * state->cellHeight +
        static_cast<double>(grid.rows > 0U ? grid.rows - 1U : 0U) * state->elementGap;
    const double indexHeight = state->indexDisplayVisible
        ? arrayIndexDisplayHeight(*state) +
            (2.0 * (state->indexContainerPadding + state->indexContainerBorderWidth))
        : 0.0;
    const double elementRegionWidth = viewportWidth + verticalScrollbarWidth +
        (2.0 * kArraySubcontainerPadding);
    const double elementRegionHeight = viewportHeight + horizontalScrollbarHeight +
        (2.0 * kArraySubcontainerPadding);

    return Size{
        kArrayPlacementAuraExtent + indexWidth + elementRegionWidth,
        kArrayPlacementAuraExtent +
            (std::max)(elementRegionHeight, indexHeight)
    };
}

}
