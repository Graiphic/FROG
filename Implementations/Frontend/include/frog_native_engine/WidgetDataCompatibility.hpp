#pragma once

#include "WidgetInstance.hpp"

namespace frog::native_engine {

// Existing PRIVATE authoring/transfer predicates. They inspect the model only:
// no document decoding, opaque-cell materialization, default validation or FIR.
// A positive result does not certify a serialized transfer or a public program.
[[nodiscard]] bool canMigrateContainedArrayValue(const WidgetInstance &target,
    const WidgetInstance &previous);

namespace data_operations {
bool supported(const WidgetInstance &);
bool compatible(const WidgetInstance &, const WidgetInstance &);
bool canCut(const WidgetInstance &);
} // namespace data_operations
} // namespace frog::native_engine
