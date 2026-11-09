// SPDX-License-Identifier: Apache-2.0
// Read-only accessors moved unchanged from WidgetPropertyData.cpp.
#include "frog_native_engine/WidgetInstance.hpp"
#include "WidgetTypographyDetail.hpp"
namespace frog::native_engine {
using widget_typography_detail::normalizedFontScale;
using widget_typography_detail::kPlainTextStyle;
double widgetNumericValueFontScale(const WidgetInstance &widget) noexcept {
    return normalizedFontScale(widget.numericValueFontScale.value_or(widget.inheritedTextPointSize / kDocumentTextBasePointSize));
}

std::string_view widgetNumericValueTextStyle(const WidgetInstance &widget) noexcept {
    return widget.numericValueTextStyle.has_value() ? std::string_view{ widget.numericValueTextStyle.value() } : kPlainTextStyle;
}
}
