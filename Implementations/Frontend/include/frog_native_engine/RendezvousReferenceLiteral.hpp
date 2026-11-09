#pragma once
#include "frog_native_engine/RendezvousReference.hpp"
#include "frog_native_engine/ApplicationControlTypes.hpp"

namespace frog::native_engine {
struct DiagramNode;
// The saved constant is a typed null. Displaying it creates no runtime barrier.
[[nodiscard]] inline bool isNullRendezvousReferenceValue(const ApplicationControlNode &value) {
    return value.kind==ApplicationControlKind::Reference && value.expectedClass==kRendezvousReferenceClass &&
        value.valueType.kind==binding::TypeKind::Unknown && value.target.kind==ApplicationControlTargetKind::Unbound &&
        value.target.ownerDocumentId.empty() && value.target.widgetId.empty();
}
[[nodiscard]] bool isNullRendezvousReferenceLiteral(const DiagramNode &node);
} // namespace frog::native_engine
