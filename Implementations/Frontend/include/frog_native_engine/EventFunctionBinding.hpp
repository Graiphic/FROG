#pragma once
#include "EventReference.hpp"
#include "FrogDraftData.hpp"
#include "FunctionBindingCatalog.hpp"
#include <span>

namespace frog::native_engine {
struct EventTypeEvidence {std::string_view portId;binding::TypeRef valueType;};
struct EventTypeResult {binding::TypeResolution resolution;bool complete=false;};
struct EventFunctionPort {std::string id,label;bool output=false;};
[[nodiscard]] bool isEventFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::vector<DiagramEventRegistrationRow> eventRegistrationRowsForNode(const DiagramNode &);
[[nodiscard]] std::vector<EventFunctionPort> eventFunctionPorts(const DiagramNode &);
[[nodiscard]] EventTypeResult resolveEventFunctionPort(const DiagramNode &,std::string_view,
    std::span<const EventTypeEvidence> evidence={});
// Non-presence of Flush options is meaningful; only Generate's priority has a
// scalar default. error_in uses the common canonical no-error Cluster.
[[nodiscard]] std::optional<std::string> eventOptionalInputDefault(const DiagramNode &,std::string_view);
} // namespace frog::native_engine
