// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "frog_native_engine/ItemSelection.hpp"
#include <optional>
#include <string>

namespace frog::native_engine::listbox_widget {
enum class Projection { SelectedValue,SelectedValues,SelectedIds,Selection };
struct State {
    item_selection::State selection;
    Projection projection=Projection::SelectedValue;
    bool doubleClickActivates=true,enterActivates=true;
    // A malformed authored selection remains inspectable and saveable. It
    // cannot be admitted as an empty or guessed executable value.
    std::string diagnostic;
    std::optional<std::string> preservedProperties;
    bool operator==(const State &) const=default;
};
inline std::string_view projectionPath(Projection p) noexcept {
    switch(p){case Projection::SelectedValue:return "value.selected_value";
        case Projection::SelectedValues:return "value.selected_values";
        case Projection::SelectedIds:return "value.selected_ids";case Projection::Selection:return "value";}
    return {};
}
inline std::optional<Projection> parseProjection(std::string_view p) noexcept {
    if(p=="value.selected_value")return Projection::SelectedValue;
    if(p=="value.selected_values")return Projection::SelectedValues;
    if(p=="value.selected_ids")return Projection::SelectedIds;
    if(p=="value")return Projection::Selection;
    return {};
}
} // namespace frog::native_engine::listbox_widget
