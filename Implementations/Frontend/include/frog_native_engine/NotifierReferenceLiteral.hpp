#pragma once
#include "frog_native_engine/NotifierReference.hpp"
#include "frog_native_engine/ApplicationControlTypes.hpp"

namespace frog::native_engine {
struct DiagramNode;
// A serializable typed null: no acquisition, live handle or reader history.
[[nodiscard]] inline bool isNullNotifierReferenceValue(const ApplicationControlNode &value) {
    return value.kind==ApplicationControlKind::Reference && value.expectedClass==kNotifierReferenceClass &&
        value.valueType.known() && value.target.kind==ApplicationControlTargetKind::Unbound &&
        value.target.ownerDocumentId.empty() && value.target.widgetId.empty();
}
[[nodiscard]] bool isNullNotifierReferenceLiteral(const DiagramNode &node);
} // namespace frog::native_engine
