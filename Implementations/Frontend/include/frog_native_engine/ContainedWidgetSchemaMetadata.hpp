#pragma once
#include "frog_native_engine/WidgetInstance.hpp"

namespace frog::native_engine {
// Editor-schema safety only; this is not public semantic validation.
[[nodiscard]] bool containedWidgetSchemaHasCycle(const WidgetInstance &widget);
// Refresh compatibility metadata for the shared composition. The authoritative
// TypeRef law remains binding::widgetType. No cell decoding, data/default edits,
// ID generation, dimension changes, visible-cell changes or autosizing occur.
// Returns false before mutation for an ancestry cycle; shared children are valid.
[[nodiscard]] bool refreshContainedWidgetSchemaMetadata(WidgetInstance &widget);
}
