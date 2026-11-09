// SPDX-License-Identifier: Apache-2.0
// Extracted from the existing Studio data owner; no editor or runtime.
#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/ApplicationControlSchema.hpp"
#include <stdexcept>
namespace frog::native_engine {

WidgetInstance referenceWidget(std::string id,const binding::TypeRef &type,bool indicator,
    std::optional<ApplicationControlNode> symbolicValue) {
    if(type.kind!=binding::TypeKind::Reference || !type.known())
        throw std::invalid_argument("A reference widget requires a complete reference type.");
    if(symbolicValue && (symbolicValue->kind!=ApplicationControlKind::Reference ||
        applicationControlReferenceType(*symbolicValue)!=type))
        throw std::invalid_argument("A reference widget value must retain its exact reference type and target.");
    WidgetInstance widget;
    widget.instanceId=std::move(id);widget.classId=std::string(kReferenceWidgetClass);
    widget.role=indicator?"indicator":"control";widget.valueType=type.canonical();
    if(!symbolicValue) {
        ApplicationControlNode nullValue;
        nullValue.kind=ApplicationControlKind::Reference;nullValue.expectedClass=type.name;
        nullValue.valueType=type.children.empty()?binding::TypeRef{}:type.children.front();
        nullValue.target={ApplicationControlTargetKind::Unbound,{},{}};
        symbolicValue=std::move(nullValue);
    }
    widget.referenceValue=std::move(symbolicValue);
    widget.label=indicator?"Reference Indicator":"Reference Control";
    widget.bounds={0,0,110,26};widget.labelVisible=true;
    return widget;
}


}
