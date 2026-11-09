#pragma once
#include "WidgetInstance.hpp"
#include <utility>

namespace frog::native_engine {
[[nodiscard]] inline bool isReferenceWidget(const WidgetInstance &widget) noexcept {
    return widget.classId == kReferenceWidgetClass;
}
// The widget's data is a reference. It is independent of a reference TO this
// widget, whose Value type is in turn this complete reference schema.
[[nodiscard]] inline binding::TypeRef referenceWidgetType(const WidgetInstance &widget) {
    if (!isReferenceWidget(widget)) return {};
    if (widget.referenceValue) {
        const auto &value=*widget.referenceValue;
        if (value.kind!=ApplicationControlKind::Reference) return {};
        return binding::TypeRef::reference(value.expectedClass,
            value.valueType.known()?std::optional{value.valueType}:std::nullopt);
    }
    const auto parsed=binding::parseType(widget.valueType);
    return parsed.valid() && parsed.type.kind==binding::TypeKind::Reference ? parsed.type : binding::TypeRef{};
}
inline void clearReferenceWidgetValue(WidgetInstance &widget) {
    if (!isReferenceWidget(widget)) return;
    // Keep the complete schema (including Enum domains/choices and ranks)
    // while clearing the association. Canonical text alone is not that schema.
    if(widget.referenceValue)widget.referenceValue->target={ApplicationControlTargetKind::Unbound,{},{}};
}
[[nodiscard]] WidgetInstance referenceWidget(std::string id,const binding::TypeRef &completeReference,
    bool indicator=false,std::optional<ApplicationControlNode> symbolicValue={});
}
