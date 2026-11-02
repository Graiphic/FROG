#pragma once
#include "FrogSourceDocument.hpp"
#include "CanonicalNativeCallScalar.hpp"
#include "CanonicalObjectReferenceDescriptor.hpp"
#include <cstdint>
#include <memory>
#include <span>
#include <utility>

namespace frog::native_engine {
class CanonicalEventExecutionIr;
struct CanonicalFrontendMetrics;
inline constexpr std::string_view canonicalEventRegionBodyProfile="frog.private.event-region-body.v1";
struct CanonicalIrDiagnostic {
    std::string code;
    std::string message;
    std::string location;
    bool unsupported=false;
};
struct CanonicalIrLimits {
    std::size_t maximumIrBytes=16U*1024U*1024U;
    std::size_t maximumSourceBytes=8U*1024U*1024U;
    std::size_t maximumNodes=1024;
    std::size_t maximumEdges=4096;
    std::size_t maximumWidgets=1024;
    std::size_t maximumSourceIdBytes=1024;
    std::size_t maximumIrIdBytes=2112;
    std::size_t maximumMappingRecords=16384;
};
struct CanonicalIrReadOptions {
    std::string sourceUri;
    CanonicalIrLimits limits;
    // Opt-in observations only; never serialized or used for admission.
    CanonicalFrontendMetrics *metrics=nullptr;
};
struct CanonicalIrPort {
    std::string id;
    bool input=false;
    std::string typeRef;
    // Nominal source schema is retained even when a backend uses a declared
    // physical representation (for example an Enum's u16 representation).
    std::string fullTypeJson;
};
// Local Studio state contract, retained in open FIR rather than reconstructed
// from a node's drawing. Port identities remain the historical Studio ones.
struct CanonicalIrFeedback {
    std::uint32_t delay=1;
    std::string initialization="compile_or_load";
    std::string initializerLoopId;
    bool history=false;
};
struct CanonicalIrNode {
    std::string id;
    std::string sourceNodeId;
    std::string family;
    std::string kind;
    std::string role;
    std::vector<CanonicalIrPort> ports;
    std::string widgetId;
    std::string widgetClass;
    std::optional<double> defaultValue;
    // Exact typed source values and explicit region ownership. An empty
    // regionId denotes the root scope, never a region inferred from layout.
    std::string regionId;
    std::string valueType;
    std::optional<std::string> defaultValueJson;
    // Explicitly profiled scalar/immutable-Record literal semantics belong to
    // an executable constant, never a widget's editable default/current value.
    // Retain the exact admitted JSON lexeme separately from widget defaults.
    std::optional<std::string> literalValueJson;
    // Explicit local producer profile; scalar and immutable Record constants
    // remain distinct. Published Unbundle selections follow OUTPUT order.
    std::string literalProfile;
    std::vector<std::string> selectedFields;
    // Only the explicit Event-region FIR factory sets this field. This is a
    // real EventCase field identity, never a Front Panel widget declaration.
    std::string eventFieldId;
    // Complete local NativeCall codec/signature, never a Source-only pointer.
    std::optional<CanonicalNativeCallDescriptor> nativeCall;
    std::optional<CanonicalObjectReferenceDescriptor> objectReference;
    std::string configurationJson,fullTypeJson,widgetLabel,authoredKind;
    std::string collectionConfigurationJson;
    std::optional<CanonicalIrFeedback> feedback;
};
struct CanonicalIrEndpoint {std::string objectId;std::string portId;};
struct CanonicalIrConnection {
    std::string id;
    std::string sourceEdgeId;
    CanonicalIrEndpoint from,to;
};
struct CanonicalIrSourceMap {
    std::string id,sourceKind,sourceRef,targetKind,relation;
    std::vector<std::string> targetRefs;
};
struct CanonicalIrCorrespondence {
    std::string id,sourceRef,relationKind;
    std::vector<std::string> targetRefs;
};
struct CanonicalIrRegion {
    std::string id,sourceRegionId,ownerNodeId;
    std::vector<std::string> memberNodeIds;
    std::string role="body",sourcePointer;
};
struct CanonicalIrCaseArm {std::string regionId;std::optional<std::string> matchJson;bool defaultArm=false;};
struct CanonicalIrShiftRegisterStage {
    std::string tunnelId,initializerPortId,readPortId,typeRef;
    std::size_t historyIndex=0;
};
struct CanonicalIrShiftRegister {
    std::string pairId,writePortId,resultPortId;
    std::vector<CanonicalIrShiftRegisterStage> stages;
};
struct CanonicalIrTunnelWriter {std::string regionId,inputPortId;};
struct CanonicalIrTunnel {
    std::string id,typeRef,fullTypeJson,inputPortId,readPortId,resultPortId;
    std::vector<CanonicalIrTunnelWriter> writers;
    std::optional<std::string> unwiredDefaultJson;
    bool useDefaultIfUnwired=false;
    std::string mode="last_value",indexingPolicy="leading_dimension";
    std::string externalTypeRef,externalFullTypeJson;
    std::string conditionPortId;
};
// Explicit temporal transfer between Sequence frames. An empty region denotes
// the surrounding diagram; neither frame identity comes from paint geometry.
struct CanonicalIrSequenceTransfer {
    std::string id,typeRef,fullTypeJson,inputPortId,outputPortId;
    std::string sourceRegionId,targetRegionId;
    bool useDefaultIfUnwired=false;
};
struct CanonicalIrStructure {
    std::string nodeId,kind;
    std::vector<std::string> regionIds;
    std::string iterationPortId,conditionPortId,countPortId,selectorPortId;
    std::string countValuePortId,selectedCasePortId;
    std::vector<CanonicalIrCaseArm> caseArms;
    std::vector<CanonicalIrShiftRegister> shiftRegisters;
    std::vector<CanonicalIrTunnel> tunnels;
    std::string eventConfigurationJson,timeoutPortId;
    std::string inPlaceConfigurationJson;
    bool stopIfTrue=false;
    std::vector<std::string> caseSelectorFieldPath;
    std::string sequenceProfile;
    std::vector<CanonicalIrSequenceTransfer> sequenceTransfers;
};
struct CanonicalIrEventScope {
    std::string sourceStructureId,sourceRegionId,regionId;
};

// Bounded intake of the open canonical artifact. The sealed normalized view
// stays subordinate to retained immutable IR and exact authoritative source.
// The historical flat f64/Add corridor remains intact. The explicit typed
// corridor additionally admits scalar While/For bodies, Boolean Case regions,
// their exact intrinsic terminal projections, Boolean Not and Less.
// The explicit frog.studio.composed-program@1 local profile additionally
// retains full nominal schemas and owned structure/Queue/Event composition.
// Its availability for a target is determined by the real downstream lowerer
// and backend, never by this admission comment or by a palette declaration.
// It is neither a Canvas projection, a general schema interpreter, a lowerer,
// nor activation of the experimental Runtime POC.
class CanonicalExecutionIr {
public:
    [[nodiscard]] static std::optional<CanonicalExecutionIr> read(std::string_view irUtf8,
        const FrogSourceDocument &authoritativeSource,const CanonicalIrReadOptions &,
        std::vector<CanonicalIrDiagnostic> *diagnostics=nullptr);
    // Embedded producer for the same explicitly admitted corridor. It reads
    // the authoritative source directly, never invokes Python or consumes a
    // cached FIR/private authoring graph, and seals through the strict reader.
    [[nodiscard]] static std::optional<CanonicalExecutionIr> derive(
        const FrogSourceDocument &authoritativeSource,std::string_view documentId,
        const CanonicalIrReadOptions &,
        std::vector<CanonicalIrDiagnostic> *diagnostics=nullptr);
    // Defined in runtime-integration only. Retains the complete authoritative
    // Event Source and seals one actual region into this SAME FIR node/edge
    // representation. The ordinary reader never admits the local profile.
    [[nodiscard]] static std::optional<CanonicalExecutionIr> deriveEventRegion(
        const CanonicalEventExecutionIr &,std::string_view sourceRegionId,
        const CanonicalIrReadOptions &,std::vector<CanonicalIrDiagnostic> *diagnostics=nullptr);
    [[nodiscard]] static std::optional<CanonicalExecutionIr> readEventRegion(
        std::string_view irUtf8,const CanonicalEventExecutionIr &,std::string_view sourceRegionId,
        const CanonicalIrReadOptions &,std::vector<CanonicalIrDiagnostic> *diagnostics=nullptr);
    [[nodiscard]] std::string_view originalUtf8() const noexcept;
    [[nodiscard]] const source::JsonValue &root() const noexcept;
    [[nodiscard]] const FrogSourceDocument &authoritativeSource() const noexcept;
    [[nodiscard]] std::string_view documentId() const noexcept;
    [[nodiscard]] std::string_view unitId() const noexcept;
    [[nodiscard]] std::string_view sourceUri() const noexcept;
    [[nodiscard]] std::string_view sourceHash() const noexcept;
    [[nodiscard]] std::span<const CanonicalIrNode> nodes() const noexcept;
    [[nodiscard]] std::span<const CanonicalIrConnection> connections() const noexcept;
    [[nodiscard]] std::span<const CanonicalIrSourceMap> sourceMap() const noexcept;
    [[nodiscard]] std::span<const CanonicalIrCorrespondence> correspondence() const noexcept;
    [[nodiscard]] std::span<const CanonicalIrRegion> regions() const noexcept;
    [[nodiscard]] std::span<const CanonicalIrStructure> structures() const noexcept;
    [[nodiscard]] const CanonicalIrEventScope *eventScope() const noexcept;
private:
    struct Storage;
    explicit CanonicalExecutionIr(std::shared_ptr<const Storage> storage):storage_(std::move(storage)){}
    std::shared_ptr<const Storage> storage_;
};
}
