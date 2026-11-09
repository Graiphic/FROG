#pragma once
#include "frog_native_engine/QueueReference.hpp"
#include "frog_native_engine/ApplicationControlTypes.hpp"

namespace frog::native_engine {
struct DiagramNode;
namespace source { struct JsonValue; }
// Authoring data only: the resource's typed null value carries no live lease,
// widget target or document target. It never registers Queue as an object class.
[[nodiscard]] inline bool isNullQueueReferenceValue(const ApplicationControlNode &value) {
    return value.kind==ApplicationControlKind::Reference && value.expectedClass==kQueueReferenceClass &&
        value.valueType.known() && value.target.kind==ApplicationControlTargetKind::Unbound &&
        value.target.ownerDocumentId.empty() && value.target.widgetId.empty();
}
[[nodiscard]] bool isNullQueueReferenceLiteral(const DiagramNode &node);
// Exact Source configuration, including invariant element schema and no
// retained/unknown fields. Shared by admission and owned-value consumers.
[[nodiscard]] bool isNullQueueReferenceConfiguration(const source::JsonValue &,const binding::TypeRef &);
} // namespace frog::native_engine
