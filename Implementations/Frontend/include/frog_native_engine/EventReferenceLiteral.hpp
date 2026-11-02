#pragma once
#include "EventReference.hpp"
#include "ApplicationControlTypes.hpp"
namespace frog::native_engine {
struct DiagramNode;
[[nodiscard]] inline bool isNullEventReferenceValue(const ApplicationControlNode &value) {
    return value.kind==ApplicationControlKind::Reference &&
        (value.expectedClass==kUserEventReferenceClass || value.expectedClass==kEventRegistrationReferenceClass) &&
        value.valueType.known() &&
        (value.expectedClass!=kEventRegistrationReferenceClass || value.valueType.kind==binding::TypeKind::Cluster) &&
        value.target.kind==ApplicationControlTargetKind::Unbound && value.target.ownerDocumentId.empty() && value.target.widgetId.empty();
}
[[nodiscard]] bool isNullEventReferenceLiteral(const DiagramNode &node);
}
