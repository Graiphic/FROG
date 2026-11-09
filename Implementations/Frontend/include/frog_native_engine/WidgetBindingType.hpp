#pragma once
#include "frog_native_engine/BindingType.hpp"

namespace frog::native_engine {
struct WidgetInstance;
namespace binding {
// Schema projection only. Neither document persistence nor diagram topology
// belongs to this interface. Shared children are valid; ancestry cycles are not.
[[nodiscard]] TypeParseResult widgetType(const WidgetInstance &widget);
} // namespace binding
} // namespace frog::native_engine
