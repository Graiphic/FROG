#pragma once

#include "WidgetInstance.hpp"

namespace frog::native_engine {

// Assignment boundary: callers supply a DETACHED destination after the
// existing compatibility/codec guards. Decode/encode stay here; the value
// calculation links only the prepared model. Failure must discard this draft.
[[nodiscard]] bool assignPreparedPrivateWidgetValues(WidgetInstance &detachedTarget,
    const WidgetInstance &source,bool useFieldIdentities);

// Compatibility operations for the PRIVATE document adapter. These are not
// widget-family operations, canonical source validation, or FIR preparation.
void materializeArrayElementValue(WidgetInstance &widget, const ArrayElementValue *value);
[[nodiscard]] std::optional<ArrayElementValue> prepareArrayElementValueFromWidget(
    const WidgetInstance &array, const std::vector<std::uint32_t> &indices,
    const WidgetInstance &elementWidget);
[[nodiscard]] bool setArrayElementValueFromWidget(WidgetInstance &array,
    const std::vector<std::uint32_t> &indices, const WidgetInstance &elementWidget);
// Migrates composed cells against the old prototype before any installation.
[[nodiscard]] bool setArrayElementTemplate(WidgetInstance &array, const WidgetInstance &element);

} // namespace frog::native_engine
