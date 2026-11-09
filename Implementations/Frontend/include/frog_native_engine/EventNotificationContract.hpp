#pragma once

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

namespace frog::native_engine {
struct DiagramEventBinding;
struct DiagramEventCase;

// Shared read-only Studio contract. Catalogue membership does not establish
// a producer, a compiled Event body, or public Source/FIR runtime support.
enum class EventSourceScope { Application, Vi, Pane, Control, Dynamic };
enum class EventControlCategory { Any, WaveformGraph };
struct EventNotificationSpec {
    std::string_view kind, label;
    std::string_view group;
    std::uint32_t sourceScopes;
    EventControlCategory controlCategory = EventControlCategory::Any;
    bool filter = false;
    // One private catalogue owns readable fields and response inputs. These
    // space-separated semantic IDs are independent of labels and row order.
    std::string_view dataFields, responseFields;
    bool sourceReference = true;
    std::string_view contractDiagnostic;
    std::string_view referenceDocument;
};

std::span<const EventNotificationSpec> eventNotificationCatalog() noexcept;
const EventNotificationSpec *eventNotificationSpec(std::string_view kind) noexcept;
std::string_view eventSourceScopeName(EventSourceScope scope) noexcept;
EventSourceScope eventBindingSourceScope(const DiagramEventBinding &binding) noexcept;
// Preserve the historical kind/source fallback without materializing a case.
std::vector<DiagramEventBinding> eventCaseBindings(const DiagramEventCase &eventCase);
}
