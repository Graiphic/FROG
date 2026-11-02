#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/FrogWidgetData.hpp"
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/EventNotificationContract.hpp"
#include <span>
namespace frog::native_engine {
struct EventSourceSpec {
    std::string id, label;
    EventSourceScope scope = EventSourceScope::Control;
    EventControlCategory controlCategory = EventControlCategory::Any;
    bool available = true;
    // Display ancestry only: bindings always retain the widget instanceId.
    std::string parentId, path;
    bool cluster = false;
    // Authoring schema copied at the widget boundary, independent of its value,
    // geometry or presentation. Built-in non-control sources have no value type.
    binding::TypeRef valueType;
    std::string valueTypeError;
    // Class identity from the common object-reference/type system. No handle
    // or memory address is persisted in an event binding.
    std::string referenceClassId;
    // Dynamic registration keeps its declared event and original source class;
    // Dynamic is an editor grouping, never an object-reference class.
    std::string registeredKind;
    EventSourceScope registeredSourceScope = EventSourceScope::Dynamic;
};
enum class EventResponseUnwiredPolicy { PreserveIncoming, False, NativeFeedback, Undeclared };
struct EventDataAuthoringField {
    std::string id, label;
    binding::TypeRef type;
    EventResponseUnwiredPolicy unwiredPolicy = EventResponseUnwiredPolicy::PreserveIncoming;
};
struct EventDataAuthoringSchema {
    std::vector<EventDataAuthoringField> fields;
    // Inward inputs of the response node, separate from readable event data.
    std::vector<EventDataAuthoringField> responses;
    // A usable authoring subset does not imply a complete payload or runtime.
    std::vector<std::string> limitations;
    std::string error;
    bool complete = true;
    // Private Studio runtime capability is separate from public source/FIR
    // lowering and the SDK backend. UI/filter families may remain unavailable.
    bool runtimeAvailable = false;
    [[nodiscard]] bool valid() const noexcept { return error.empty(); }
};

// Shared read-only source/type contracts. These functions never choose the
// visible UI case, mutate a document or invoke an event/runtime service.
EventDataAuthoringSchema eventCaseDataSchema(const DiagramNode &,std::string_view regionId,std::span<const EventSourceSpec>);
bool isEventAuthoringDataField(std::string_view) noexcept;
bool isEventAuthoringResponseField(std::string_view) noexcept;
bool eventNotificationAllowsNewBinding(const EventNotificationSpec &) noexcept;
bool eventNotificationAllowsNewBinding(const EventNotificationSpec &,EventSourceScope) noexcept;
bool eventNotificationAllowsNewBinding(const EventNotificationSpec &,const EventSourceSpec &) noexcept;
bool eventNotificationSupportsSource(const EventNotificationSpec &,const EventSourceSpec &) noexcept;
std::vector<EventSourceSpec> eventSources(FrogWidgetDataView,bool resolveTypes=true);
std::vector<EventSourceSpec> eventStructureSources(const DiagramNode &,const binding::TypeRef &,FrogWidgetDataView,bool resolveTypes=true);
std::string validateEventBindingSources(const std::vector<DiagramEventBinding> &,std::span<const EventSourceSpec>);
bool isPublicEventDescriptorKind(std::string_view) noexcept;
std::string validateEventBindings(const std::vector<DiagramEventBinding> &);
std::string validateEventCaseConfiguration(const DiagramNode &,std::string_view replacedRegion,const DiagramEventCase &);
}
