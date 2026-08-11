#pragma once

#include <algorithm>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

namespace frog::sdk::runtime {

struct ProtocolVersion {
    std::uint16_t major = 1U;
    std::uint16_t minor = 0U;

    [[nodiscard]] bool operator==(const ProtocolVersion &) const noexcept = default;
};

inline constexpr ProtocolVersion kProtocolVersion{ 1U, 0U };
inline constexpr std::uint32_t kArtifactSchemaVersion = 1U;

template <typename Tag>
struct StableId {
    std::string value;

    [[nodiscard]] bool empty() const noexcept { return value.empty(); }
    [[nodiscard]] explicit operator bool() const noexcept { return !empty(); }
    [[nodiscard]] bool operator==(const StableId &) const noexcept = default;
};

struct ArtifactIdTag;
struct DocumentIdTag;
struct SessionIdTag;
struct NodeIdTag;
struct PortIdTag;
struct EdgeIdTag;
struct WidgetIdTag;
struct SourceIdentityIdTag;

using ArtifactId = StableId<ArtifactIdTag>;
using DocumentId = StableId<DocumentIdTag>;
using SessionId = StableId<SessionIdTag>;
using NodeId = StableId<NodeIdTag>;
using PortId = StableId<PortIdTag>;
using EdgeId = StableId<EdgeIdTag>;
using WidgetId = StableId<WidgetIdTag>;
using SourceIdentityId = StableId<SourceIdentityIdTag>;

enum class ValueCategory {
    Unknown,
    Boolean,
    SignedInteger,
    UnsignedInteger,
    FloatingPoint,
    Complex,
    FixedPoint,
    String,
    Path,
    Array
};

struct ValueType {
    std::string canonicalName;
    ValueCategory category = ValueCategory::Unknown;
    std::uint16_t bitWidth = 0U;
    std::uint8_t rank = 0U;
    std::string elementType;

    [[nodiscard]] bool known() const noexcept {
        return category != ValueCategory::Unknown && !canonicalName.empty();
    }

    [[nodiscard]] bool isNumeric() const noexcept {
        return category == ValueCategory::SignedInteger ||
            category == ValueCategory::UnsignedInteger ||
            category == ValueCategory::FloatingPoint ||
            category == ValueCategory::Complex ||
            category == ValueCategory::FixedPoint;
    }

    [[nodiscard]] bool operator==(const ValueType &) const noexcept = default;
};

[[nodiscard]] inline ValueType valueTypeFromCanonicalName(std::string_view name) {
    const auto scalar = [name](ValueCategory category, std::uint16_t bits = 0U) {
        return ValueType{ std::string(name), category, bits, 0U, {} };
    };

    if (name == "bool") return scalar(ValueCategory::Boolean, 1U);
    if (name == "i8") return scalar(ValueCategory::SignedInteger, 8U);
    if (name == "i16") return scalar(ValueCategory::SignedInteger, 16U);
    if (name == "i32") return scalar(ValueCategory::SignedInteger, 32U);
    if (name == "i64") return scalar(ValueCategory::SignedInteger, 64U);
    if (name == "u8") return scalar(ValueCategory::UnsignedInteger, 8U);
    if (name == "u16") return scalar(ValueCategory::UnsignedInteger, 16U);
    if (name == "u32") return scalar(ValueCategory::UnsignedInteger, 32U);
    if (name == "u64") return scalar(ValueCategory::UnsignedInteger, 64U);
    if (name == "f32") return scalar(ValueCategory::FloatingPoint, 32U);
    if (name == "f64") return scalar(ValueCategory::FloatingPoint, 64U);
    if (name == "c32") return scalar(ValueCategory::Complex, 64U);
    if (name == "c64") return scalar(ValueCategory::Complex, 128U);
    if (name == "fxp" || name == "cfx") return scalar(ValueCategory::FixedPoint);
    if (name == "string") return scalar(ValueCategory::String);
    if (name == "path") return scalar(ValueCategory::Path);

    constexpr std::string_view arrayPrefix = "array:";
    if (name.starts_with(arrayPrefix) && name.size() > arrayPrefix.size()) {
        return ValueType{
            std::string(name),
            ValueCategory::Array,
            0U,
            1U,
            std::string(name.substr(arrayPrefix.size()))
        };
    }
    return ValueType{ std::string(name), ValueCategory::Unknown, 0U, 0U, {} };
}

using RuntimeValue = std::variant<
    std::monostate,
    bool,
    std::int64_t,
    std::uint64_t,
    double,
    std::string>;

[[nodiscard]] inline bool runtimeValueMatches(
    const RuntimeValue &value,
    const ValueType &type) noexcept {
    if (std::holds_alternative<std::monostate>(value)) {
        return true;
    }
    switch (type.category) {
    case ValueCategory::Boolean:
        return std::holds_alternative<bool>(value);
    case ValueCategory::SignedInteger:
        return std::holds_alternative<std::int64_t>(value);
    case ValueCategory::UnsignedInteger:
        return std::holds_alternative<std::uint64_t>(value);
    case ValueCategory::FloatingPoint:
        return std::holds_alternative<double>(value);
    case ValueCategory::String:
    case ValueCategory::Path:
        return std::holds_alternative<std::string>(value);
    default:
        return false;
    }
}

enum class PortDirection { Input, Output };
enum class PortRequirement { Required, Recommended, Optional };
enum class PortCardinality { Single, Variadic };
enum class ConnectionPolicy {
    Exact,
    NumericImplicit,
    NumericExplicit,
    IntegerExplicit,
    AnyExplicit
};

[[nodiscard]] inline bool valueTypesCompatible(
    const ValueType &source,
    const ValueType &sink,
    ConnectionPolicy policy) noexcept {
    if (!source.known() || !sink.known()) {
        return false;
    }
    if (source == sink) {
        return true;
    }

    const auto isInteger = [](const ValueType &type) noexcept {
        return type.category == ValueCategory::SignedInteger ||
            type.category == ValueCategory::UnsignedInteger;
    };
    switch (policy) {
    case ConnectionPolicy::Exact:
        return false;
    case ConnectionPolicy::NumericImplicit:
    case ConnectionPolicy::NumericExplicit:
        return source.isNumeric() && sink.isNumeric();
    case ConnectionPolicy::IntegerExplicit:
        return isInteger(source) && isInteger(sink);
    case ConnectionPolicy::AnyExplicit:
        return true;
    }
    return false;
}

struct RuntimePort {
    PortId id;
    std::string displayName;
    PortDirection direction = PortDirection::Input;
    ValueType valueType;
    PortRequirement requirement = PortRequirement::Required;
    PortCardinality cardinality = PortCardinality::Single;
    ConnectionPolicy connectionPolicy = ConnectionPolicy::Exact;

    [[nodiscard]] bool operator==(const RuntimePort &) const noexcept = default;
};

enum class NodeKind { Control, Indicator, Constant, Function, Call };

struct RuntimeNode {
    NodeId id;
    NodeKind kind = NodeKind::Function;
    std::string operationId;
    std::string implementationId;
    std::vector<RuntimePort> ports;
    std::optional<RuntimeValue> literalValue;

    [[nodiscard]] bool operator==(const RuntimeNode &) const noexcept = default;
};

struct RuntimeEndpoint {
    NodeId nodeId;
    PortId portId;

    [[nodiscard]] bool operator==(const RuntimeEndpoint &) const noexcept = default;
};

struct RuntimeEdge {
    EdgeId id;
    RuntimeEndpoint source;
    RuntimeEndpoint sink;

    [[nodiscard]] bool operator==(const RuntimeEdge &) const noexcept = default;
};

enum class UiBindingDirection { ControlToRuntime, RuntimeToIndicator };

struct UiBinding {
    WidgetId widgetId;
    RuntimeEndpoint endpoint;
    UiBindingDirection direction = UiBindingDirection::ControlToRuntime;
    ValueType valueType;
    RuntimeValue initialValue;

    [[nodiscard]] bool operator==(const UiBinding &) const noexcept = default;
};

struct SourceSpan {
    std::uint32_t beginLine = 0U;
    std::uint32_t beginColumn = 0U;
    std::uint32_t endLine = 0U;
    std::uint32_t endColumn = 0U;

    [[nodiscard]] bool operator==(const SourceSpan &) const noexcept = default;
};

struct SourceIdentity {
    SourceIdentityId id;
    std::string sourceUri;
    SourceSpan span;
    NodeId nodeId;
    PortId portId;

    [[nodiscard]] bool operator==(const SourceIdentity &) const noexcept = default;
};

struct ArtifactRevision {
    std::uint64_t semantic = 0U;
    std::uint64_t interfaceContract = 0U;
    std::uint64_t topology = 0U;

    [[nodiscard]] bool operator==(const ArtifactRevision &) const noexcept = default;
};

struct ProgramArtifact {
    ProtocolVersion protocol = kProtocolVersion;
    std::uint32_t schemaVersion = kArtifactSchemaVersion;
    ArtifactId artifactId;
    DocumentId sourceDocumentId;
    std::string sourceDocumentUri;
    ArtifactRevision revision;
    std::vector<RuntimeNode> nodes;
    std::vector<RuntimeEdge> edges;
    std::vector<UiBinding> uiBindings;
    std::vector<SourceIdentity> sourceIdentities;
};

enum class DiagnosticSeverity { Information, Warning, Error };
enum class DiagnosticStage {
    ContractIntake,
    ArtifactResolution,
    CapabilityResolution,
    Scheduling,
    Execution,
    UiBinding,
    SessionLifecycle
};

struct Diagnostic {
    DiagnosticSeverity severity = DiagnosticSeverity::Error;
    DiagnosticStage stage = DiagnosticStage::ContractIntake;
    std::string code;
    std::string message;
    NodeId nodeId;
    PortId portId;
    EdgeId edgeId;
    bool blocksBuild = true;
    bool blocksRun = true;
};

struct RuntimeCapabilities {
    std::string targetId;
    ProtocolVersion protocol = kProtocolVersion;
    std::vector<std::string> operationIds;
    std::vector<std::string> valueTypes;
    bool supportsStop = true;
    bool supportsRestart = true;
    bool supportsIncrementalControlUpdates = true;

    [[nodiscard]] bool supportsOperation(std::string_view operationId) const {
        return std::find(operationIds.begin(), operationIds.end(), operationId) !=
            operationIds.end();
    }

    [[nodiscard]] bool supportsValueType(std::string_view valueType) const {
        return std::find(valueTypes.begin(), valueTypes.end(), valueType) !=
            valueTypes.end();
    }
};

enum class SessionCommandKind { Start, Stop, Restart, SetControlValue };

struct SessionCommand {
    SessionCommandKind kind = SessionCommandKind::Start;
    SessionId sessionId;
    ArtifactId artifactId;
    ArtifactRevision artifactRevision;
    WidgetId widgetId;
    RuntimeValue value;
};

enum class SessionEventKind {
    SessionStarted,
    SessionStopped,
    SessionRestarted,
    ControlValueAccepted,
    NodeEntered,
    NodeCompleted,
    WireValuePublished,
    IndicatorValueChanged,
    DiagnosticRaised,
    RuntimeFault,
    StaleArtifactRejected
};

struct SessionEvent {
    SessionEventKind kind = SessionEventKind::DiagnosticRaised;
    SessionId sessionId;
    ArtifactId artifactId;
    ArtifactRevision artifactRevision;
    std::uint64_t sequence = 0U;
    NodeId nodeId;
    PortId portId;
    EdgeId edgeId;
    WidgetId widgetId;
    RuntimeValue value;
    std::optional<Diagnostic> diagnostic;
};

[[nodiscard]] inline const RuntimeNode *findNode(
    const ProgramArtifact &artifact,
    std::string_view nodeId) noexcept {
    const auto found = std::find_if(
        artifact.nodes.begin(), artifact.nodes.end(),
        [nodeId](const RuntimeNode &node) { return node.id.value == nodeId; });
    return found == artifact.nodes.end() ? nullptr : &*found;
}

[[nodiscard]] inline const RuntimePort *findPort(
    const RuntimeNode &node,
    std::string_view portId) noexcept {
    const auto found = std::find_if(
        node.ports.begin(), node.ports.end(),
        [portId](const RuntimePort &port) { return port.id.value == portId; });
    return found == node.ports.end() ? nullptr : &*found;
}

[[nodiscard]] inline std::vector<Diagnostic> validateProgramArtifact(
    const ProgramArtifact &artifact) {
    std::vector<Diagnostic> diagnostics;
    const auto add = [&diagnostics](
                         std::string code,
                         std::string message,
                         NodeId nodeId = {},
                         PortId portId = {},
                         EdgeId edgeId = {}) {
        diagnostics.push_back(Diagnostic{
            DiagnosticSeverity::Error,
            DiagnosticStage::ContractIntake,
            std::move(code),
            std::move(message),
            std::move(nodeId),
            std::move(portId),
            std::move(edgeId),
            true,
            true
        });
    };

    if (artifact.protocol.major != kProtocolVersion.major) {
        add("contract.protocol.unsupported", "Unsupported Runtime protocol major version.");
    }
    if (artifact.schemaVersion != kArtifactSchemaVersion) {
        add("contract.artifact_schema.unsupported", "Unsupported Runtime artifact schema version.");
    }
    if (artifact.artifactId.empty()) {
        add("contract.artifact_id.missing", "The Runtime artifact requires a stable artifact ID.");
    }
    if (artifact.sourceDocumentId.empty()) {
        add("contract.document_id.missing", "The Runtime artifact requires a stable source document ID.");
    }

    std::unordered_set<std::string> nodeIds;
    for (const auto &node : artifact.nodes) {
        if (node.id.empty()) {
            add("contract.node_id.missing", "Every Runtime node requires a stable ID.");
            continue;
        }
        if (!nodeIds.insert(node.id.value).second) {
            add("contract.node_id.duplicate", "Runtime node IDs must be unique.", node.id);
        }
        if (node.operationId.empty()) {
            add("contract.operation_id.missing", "Every Runtime node requires an operation ID.", node.id);
        }
        std::unordered_set<std::string> portIds;
        for (const auto &port : node.ports) {
            if (port.id.empty()) {
                add("contract.port_id.missing", "Every Runtime port requires a stable ID.", node.id);
                continue;
            }
            if (!portIds.insert(port.id.value).second) {
                add("contract.port_id.duplicate", "Port IDs must be unique within a node.", node.id, port.id);
            }
            if (!port.valueType.known()) {
                add("contract.value_type.unknown", "Every Runtime port requires a known value type.", node.id, port.id);
            }
        }
        if (node.literalValue.has_value()) {
            const auto output = std::find_if(
                node.ports.begin(), node.ports.end(),
                [](const RuntimePort &port) { return port.direction == PortDirection::Output; });
            if (output == node.ports.end() ||
                !runtimeValueMatches(*node.literalValue, output->valueType)) {
                add("contract.literal.type_mismatch", "A literal value must match an output port type.", node.id);
            }
        }
    }

    std::unordered_set<std::string> edgeIds;
    std::unordered_set<std::string> occupiedSingleInputs;
    for (const auto &edge : artifact.edges) {
        if (edge.id.empty() || !edgeIds.insert(edge.id.value).second) {
            add("contract.edge_id.invalid", "Runtime edge IDs must be present and unique.", {}, {}, edge.id);
        }
        const auto *sourceNode = findNode(artifact, edge.source.nodeId.value);
        const auto *sinkNode = findNode(artifact, edge.sink.nodeId.value);
        const auto *sourcePort = sourceNode == nullptr
            ? nullptr : findPort(*sourceNode, edge.source.portId.value);
        const auto *sinkPort = sinkNode == nullptr
            ? nullptr : findPort(*sinkNode, edge.sink.portId.value);
        if (sourceNode == nullptr || sourcePort == nullptr ||
            sourcePort->direction != PortDirection::Output) {
            add("contract.edge.source_invalid", "An edge source must reference an output port.", edge.source.nodeId, edge.source.portId, edge.id);
        }
        if (sinkNode == nullptr || sinkPort == nullptr ||
            sinkPort->direction != PortDirection::Input) {
            add("contract.edge.sink_invalid", "An edge sink must reference an input port.", edge.sink.nodeId, edge.sink.portId, edge.id);
        } else {
            const std::string inputKey = edge.sink.nodeId.value + "\n" +
                edge.sink.portId.value;
            if (sinkPort->cardinality == PortCardinality::Single &&
                !occupiedSingleInputs.insert(inputKey).second) {
                add("contract.edge.sink_occupied", "A single-cardinality input may receive only one edge.", edge.sink.nodeId, edge.sink.portId, edge.id);
            }
        }
        if (sourcePort != nullptr && sinkPort != nullptr &&
            sourcePort->direction == PortDirection::Output &&
            sinkPort->direction == PortDirection::Input &&
            !valueTypesCompatible(
                sourcePort->valueType,
                sinkPort->valueType,
                sinkPort->connectionPolicy)) {
            add("contract.edge.type_mismatch", "The source value type is not compatible with the sink connection policy.", edge.sink.nodeId, edge.sink.portId, edge.id);
        }
    }

    for (const auto &binding : artifact.uiBindings) {
        const auto *node = findNode(artifact, binding.endpoint.nodeId.value);
        const auto *port = node == nullptr
            ? nullptr : findPort(*node, binding.endpoint.portId.value);
        const auto requiredDirection = binding.direction == UiBindingDirection::ControlToRuntime
            ? PortDirection::Output : PortDirection::Input;
        if (binding.widgetId.empty() || port == nullptr || port->direction != requiredDirection) {
            add("contract.ui_binding.invalid", "A UI binding must reference a compatible Runtime port.", binding.endpoint.nodeId, binding.endpoint.portId);
        } else if (!(port->valueType == binding.valueType) ||
                   !runtimeValueMatches(binding.initialValue, binding.valueType)) {
            add("contract.ui_binding.type_mismatch", "A UI binding value and port must use the same type.", binding.endpoint.nodeId, binding.endpoint.portId);
        }
    }

    return diagnostics;
}

[[nodiscard]] inline std::vector<Diagnostic> validateRuntimeCapabilities(
    const ProgramArtifact &artifact,
    const RuntimeCapabilities &capabilities) {
    std::vector<Diagnostic> diagnostics;
    const auto add = [&diagnostics](
                         std::string code,
                         std::string message,
                         NodeId nodeId = {},
                         PortId portId = {}) {
        diagnostics.push_back(Diagnostic{
            DiagnosticSeverity::Error,
            DiagnosticStage::CapabilityResolution,
            std::move(code),
            std::move(message),
            std::move(nodeId),
            std::move(portId),
            {},
            true,
            true
        });
    };

    if (capabilities.protocol.major != artifact.protocol.major) {
        add("capability.protocol.unsupported",
            "The Runtime and artifact protocol major versions are incompatible.");
    }
    for (const auto &node : artifact.nodes) {
        if (node.kind == NodeKind::Function || node.kind == NodeKind::Call) {
            const std::string_view operation = node.implementationId.empty()
                ? std::string_view(node.operationId)
                : std::string_view(node.implementationId);
            if (!capabilities.supportsOperation(operation)) {
                add("capability.operation.unavailable",
                    "The target Runtime does not provide the node implementation.",
                    node.id);
            }
        }
        for (const auto &port : node.ports) {
            if (!capabilities.supportsValueType(port.valueType.canonicalName)) {
                add("capability.value_type.unavailable",
                    "The target Runtime does not support the port value type.",
                    node.id,
                    port.id);
            }
        }
    }
    return diagnostics;
}

} // namespace frog::sdk::runtime
