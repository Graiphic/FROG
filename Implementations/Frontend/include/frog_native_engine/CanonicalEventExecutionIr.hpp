#pragma once
#include "CanonicalExecutionIr.hpp"
#include "frog/sdk/event_runtime_contract.hpp"

namespace frog::native_engine {
// Separate public profile: one Event Structure, an actual i64 timeout control,
// scalar Widget Value bodies, f64 Add, versioned cluster Unbundle and owned Pane notifications.
// No Canvas/studio_bindings intake or inferred native producer.
struct CanonicalEventNode {
    std::string id, sourceNodeId, sourceRef, family, kind, role, widgetId, fieldId, typeRef;
    std::vector<CanonicalIrPort> ports;
    std::optional<sdk::runtime::RuntimeValue> defaultValue;
    bool suppliedByEvent = false;
    // Direct canonical field identities, in resolved output-port order.
    std::vector<std::string> selectedFields;
};
struct CanonicalEventConnection {
    std::string id, sourceRef;
    CanonicalIrEndpoint from, to;
};
struct CanonicalEventRegion {
    std::string id, sourceRegionId, sourceRef;
    std::optional<sdk::runtime::EventIdentity> subscription;
    sdk::runtime::EventPayloadSchema payloadSchema;
    sdk::runtime::EventPayloadSchema responseSchema;
    std::vector<CanonicalEventNode> nodes;
    std::vector<CanonicalEventConnection> connections;
};
struct CanonicalEventWidget {
    std::string id, role, typeRef;
    std::optional<sdk::runtime::RuntimeValue> defaultValue;
};
class CanonicalEventExecutionIr {
public:
    [[nodiscard]] static std::optional<CanonicalEventExecutionIr> derive(
        const FrogSourceDocument &, std::string_view documentId, const CanonicalIrReadOptions &,
        std::vector<CanonicalIrDiagnostic> *diagnostics = nullptr);
    // The admitted open JSON is checked against the exact retained source and
    // the profile's complete canonical realization; cached/private input has
    // no authority. Whitespace and object-member order are nonsemantic.
    [[nodiscard]] static std::optional<CanonicalEventExecutionIr> read(std::string_view,
        const FrogSourceDocument &, const CanonicalIrReadOptions &,
        std::vector<CanonicalIrDiagnostic> *diagnostics = nullptr);
    [[nodiscard]] std::string_view originalUtf8() const noexcept;
    [[nodiscard]] const source::JsonValue &root() const noexcept;
    [[nodiscard]] const FrogSourceDocument &authoritativeSource() const noexcept;
    [[nodiscard]] std::string_view documentId() const noexcept;
    [[nodiscard]] std::string_view unitId() const noexcept;
    [[nodiscard]] std::string_view sourceUri() const noexcept;
    [[nodiscard]] std::string_view sourceHash() const noexcept;
    [[nodiscard]] bool requiresCapturedValues() const noexcept;
    // A privately constructed immutable owner has already admitted its entire
    // source and FIR. Reuse that proof only when no caller budget is stricter;
    // reduced budgets still go through the checked reader.
    [[nodiscard]] bool admittedWithin(const CanonicalIrLimits &) const noexcept;
    [[nodiscard]] std::string_view structureId() const noexcept;
    [[nodiscard]] std::string_view timeoutWidgetId() const noexcept;
    [[nodiscard]] std::int64_t timeoutDefault() const noexcept;
    [[nodiscard]] std::span<const CanonicalEventRegion> regions() const noexcept;
    [[nodiscard]] std::span<const CanonicalEventWidget> widgets() const noexcept;
private:
    struct Storage;
    explicit CanonicalEventExecutionIr(std::shared_ptr<const Storage> storage) : storage_(std::move(storage)) {}
    std::shared_ptr<const Storage> storage_;
};
} // namespace frog::native_engine
