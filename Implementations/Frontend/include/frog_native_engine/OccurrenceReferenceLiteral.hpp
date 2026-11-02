#pragma once
#include "frog_native_engine/OccurrenceReference.hpp"
#include "frog_native_engine/ApplicationControlTypes.hpp"

namespace frog::native_engine {
struct DiagramNode;
// The saved reference is explicitly null. Opening it allocates no signal object.
[[nodiscard]] inline bool isNullOccurrenceReferenceValue(const ApplicationControlNode &value) {
    return value.kind==ApplicationControlKind::Reference && value.expectedClass==kOccurrenceReferenceClass &&
        value.valueType.kind==binding::TypeKind::Unknown && value.target.kind==ApplicationControlTargetKind::Unbound &&
        value.target.ownerDocumentId.empty() && value.target.widgetId.empty();
}
[[nodiscard]] bool isNullOccurrenceReferenceLiteral(const DiagramNode &node);
} // namespace frog::native_engine
