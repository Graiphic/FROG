// Studio node adaptation. Value predicates remain the single neutral owner.
#include "frog_native_engine/QueueReferenceLiteral.hpp"
#include "frog_native_engine/NotifierReferenceLiteral.hpp"
#include "frog_native_engine/SemaphoreReferenceLiteral.hpp"
#include "frog_native_engine/RendezvousReferenceLiteral.hpp"
#include "frog_native_engine/OccurrenceReferenceLiteral.hpp"
#include "frog_native_engine/EventReferenceLiteral.hpp"
#include "frog_native_engine/FrogDraftData.hpp"

namespace frog::native_engine {
bool isNullQueueReferenceLiteral(const DiagramNode &node) {
    return node.kind=="primitive" && node.applicationControl && isNullQueueReferenceValue(*node.applicationControl);
}
bool isNullNotifierReferenceLiteral(const DiagramNode &node) {
    return node.kind=="primitive" && node.applicationControl && isNullNotifierReferenceValue(*node.applicationControl);
}
bool isNullSemaphoreReferenceLiteral(const DiagramNode &node) {
    return node.kind=="primitive" && node.applicationControl && isNullSemaphoreReferenceValue(*node.applicationControl);
}
bool isNullRendezvousReferenceLiteral(const DiagramNode &node) {
    return node.kind=="primitive" && node.applicationControl && isNullRendezvousReferenceValue(*node.applicationControl);
}
bool isNullOccurrenceReferenceLiteral(const DiagramNode &node) {
    return node.kind=="primitive" && node.applicationControl && isNullOccurrenceReferenceValue(*node.applicationControl);
}
bool isNullEventReferenceLiteral(const DiagramNode &node) {
    return node.kind=="primitive" && node.applicationControl && isNullEventReferenceValue(*node.applicationControl);
}
} // namespace frog::native_engine
