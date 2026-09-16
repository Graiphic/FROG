#pragma once

#include "frog/sdk/runtime_contract.hpp"

#include <map>
#include <compare>

namespace frog::sdk::runtime {

// Opt-in backend contract for event dispatch. This is not a reinterpretation
// of editor metadata, nor an extension of ProgramArtifact schema version 1.
// A compiler must validate/lower every region into its own ProgramArtifact.
struct EventIdentity {
    std::string scope;
    std::string source;
    std::string kind;
    bool filter = false;
    auto operator<=>(const EventIdentity &) const = default;
};

using EventPayload = std::map<std::string, RuntimeValue>;
using EventPayloadSchema = std::map<std::string, ValueType>;

struct EventNotification {
    EventIdentity identity;
    std::uint64_t timeMilliseconds = 0;
    EventPayload fields;
};

struct EventCaseArtifact {
    std::string regionId;
    std::vector<EventIdentity> subscriptions;
    ProgramArtifact body;
    // Explicit lowered inputs, not guessed port names or widget labels.
    // source, type and time are supplied by the dispatcher; additional fields
    // must be declared in payloadSchema and supplied by the event producer.
    std::map<std::string, WidgetId> fieldInputs;
    EventPayloadSchema payloadSchema;
    std::optional<WidgetId> discardOutput;
    // Filter responses may change only these explicitly declared fields.
    std::map<std::string, WidgetId> responseOutputs;
};

struct EventStructureArtifact {
    std::uint32_t schemaVersion = 1;
    NodeId structureId;
    std::vector<EventCaseArtifact> cases;
    std::string timeoutRegionId;
};

struct EventRegistrationHandle {
    std::uint64_t value = 0;
    bool operator==(const EventRegistrationHandle &) const = default;
};

struct UserEventHandle {
    std::uint64_t value = 0;
    bool operator==(const UserEventHandle &) const = default;
};

enum class EventDispatchStatus { Executed, Stopped, Rejected };

struct EventDispatchResult {
    EventDispatchStatus status = EventDispatchStatus::Rejected;
    std::string regionId;
    bool discard = false;
    EventPayload response;
    std::vector<SessionEvent> events;
    std::string error;
};

} // namespace frog::sdk::runtime
