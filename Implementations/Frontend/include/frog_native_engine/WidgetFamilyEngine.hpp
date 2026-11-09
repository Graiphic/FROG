#pragma once

#include "frog_native_engine/WidgetCatalog.hpp"
#include "frog_native_engine/WidgetInstance.hpp"

#include <optional>

namespace frog::native_engine {

struct WidgetInstance;
// Color Box is an exact U32 Numeric presentation, not a distinct value type.
inline bool isNumericColorBox(const WidgetInstance &widget) noexcept {
    return isNumericWidget(widget)&&widget.valueType=="u32"&&widget.numericColorBox;
}

[[nodiscard]] const WidgetDefinition *widgetDefinition(
    const WidgetInstance &widget) noexcept;

[[nodiscard]] std::optional<WidgetFamily> widgetBehaviorFamily(
    const WidgetInstance &widget) noexcept;

[[nodiscard]] bool widgetIsFamily(
    const WidgetInstance &widget,
    WidgetFamily family) noexcept;

[[nodiscard]] bool widgetHasCapability(
    const WidgetInstance &widget,
    WidgetCapability capability) noexcept;

[[nodiscard]] std::optional<WidgetRole> widgetRole(
    const WidgetInstance &widget) noexcept;

[[nodiscard]] bool widgetHasRole(
    const WidgetInstance &widget,
    WidgetRole role) noexcept;

[[nodiscard]] bool widgetSupportsRole(
    const WidgetInstance &widget,
    WidgetRole role) noexcept;

[[nodiscard]] bool widgetCanConvertRole(
    const WidgetInstance &widget,
    WidgetRole role) noexcept;

// Central role conversion contract for every widget family. This is the one
// place that maps a semantic Control/Indicator role to concrete class IDs and
// keeps family-specific state (labels and Array element templates) coherent.
[[nodiscard]] bool setWidgetRole(
    WidgetInstance &widget,
    WidgetRole role);

[[nodiscard]] bool widgetIsNumericOrChoice(
    const WidgetInstance &widget) noexcept;

[[nodiscard]] WidgetTerminalDirection widgetTerminalDirection(
    const WidgetInstance &widget) noexcept;

[[nodiscard]] bool widgetSupportsDiagramTerminal(
    const WidgetInstance &widget) noexcept;

} // namespace frog::native_engine
