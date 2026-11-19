// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include "frog_native_engine/WidgetInstance.hpp"

namespace frog::native_engine::listbox_widget {
[[nodiscard]] std::vector<item_selection::Item> items(const WidgetInstance &);
[[nodiscard]] State state(const WidgetInstance &);
[[nodiscard]] std::string validate(const WidgetInstance &,const State &);
[[nodiscard]] binding::TypeRef bindingType(Projection);
[[nodiscard]] std::optional<source::JsonValue> bindingValue(const WidgetInstance &,std::string *error=nullptr);
bool setState(WidgetInstance &,State,std::string *error=nullptr);
bool assignBindingValue(WidgetInstance &,const source::JsonValue &,std::string *error=nullptr);
// The shared codec retains invalid authored fields and diagnoses them before
// admission. A read never changes a declared projection to guess a value.
void readState(WidgetInstance &,const source::JsonValue &widget);
[[nodiscard]] source::JsonValue properties(const WidgetInstance &);
[[nodiscard]] source::JsonValue currentState(const WidgetInstance &);
} // namespace frog::native_engine::listbox_widget
