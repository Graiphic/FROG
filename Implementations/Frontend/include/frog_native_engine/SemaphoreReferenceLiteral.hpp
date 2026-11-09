#pragma once
#include "frog_native_engine/SemaphoreReference.hpp"
#include "frog_native_engine/ApplicationControlTypes.hpp"

namespace frog::native_engine {
struct DiagramNode;
// Serializable null only. Authoring never obtains a runtime resource or permit.
[[nodiscard]] inline bool isNullSemaphoreReferenceValue(const ApplicationControlNode &value) {
    return value.kind==ApplicationControlKind::Reference && value.expectedClass==kSemaphoreReferenceClass &&
        value.valueType.kind==binding::TypeKind::Unknown && value.target.kind==ApplicationControlTargetKind::Unbound &&
        value.target.ownerDocumentId.empty() && value.target.widgetId.empty();
}
[[nodiscard]] bool isNullSemaphoreReferenceLiteral(const DiagramNode &node);
} // namespace frog::native_engine
