#pragma once
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/BindingResult.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/FrogWidgetData.hpp"
#include "frog_native_engine/FunctionCatalog.hpp"
#include "frog_native_engine/EventStructureSchema.hpp"

#include <functional>
#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace frog::native_engine::binding {

// Bind legacy By Name selections to stable cluster field IDs and refresh their
// display paths after schema edits. Removed IDs remain selected but invalid.
[[nodiscard]] bool synchronizeClusterFieldSelections(std::vector<DiagramNode> &nodes,
    std::span<const DiagramEdge> edges, FrogWidgetDataView canvas);
[[nodiscard]] bool isCase(const DiagramNode &node);
[[nodiscard]] std::string tunnelInputId(std::string_view id);
[[nodiscard]] std::string tunnelOutputId(std::string_view id);



// One read-only snapshot per resolution pass. No dependency on active region,
// HWND, screen geometry, persisted inferred hints, or the editor port cache.
class TypeResolver {
public:
    using LeafResolver = std::function<TypeParseResult(const DiagramNode &, std::string_view)>;
    TypeResolver(std::span<const DiagramNode> nodes, std::span<const DiagramEdge> edges,
        FrogWidgetDataView canvas, LeafResolver leaf = {});
    [[nodiscard]] BindingResult resolve(const DiagramEdgeEndpoint &endpoint);
    // Event Data belongs to the semantic region of the actual wire. A visible
    // editor case is never an input to type resolution.
    [[nodiscard]] BindingResult resolveInRegion(const DiagramEdgeEndpoint &endpoint,
        std::string_view ownerStructureId, std::string_view regionId);
    [[nodiscard]] BindingResult resolveSource(const DiagramEdge &edge);
    [[nodiscard]] const EventDataAuthoringSchema &eventSchema(
        std::string_view structureId, std::string_view regionId);
    // Inspect literal data, including nested Cluster fields. This does not
    // expose an owned field as a wire terminal; resolve/direction keep that guard.
    [[nodiscard]] BindingResult resolveLiteral(std::string_view nodeId);
    [[nodiscard]] BindingResult tunnel(const DiagramNode &node, const DiagramCaseStructureTunnel &tunnel);
    [[nodiscard]] TypeResolution connection(const DiagramEdgeEndpoint &source,
        const DiagramEdgeEndpoint &target);
    [[nodiscard]] TypeResolution connection(const DiagramEdge &edge);
    [[nodiscard]] TypeResolution connectionInRegion(const DiagramEdgeEndpoint &source,
        const DiagramEdgeEndpoint &target, std::string_view ownerStructureId, std::string_view regionId);
    [[nodiscard]] std::optional<FunctionPortDirection> direction(const DiagramEdgeEndpoint &endpoint);
private:
    friend struct TypeResolverTestAccess;
    using IncomingEdgeIndex = std::unordered_map<std::string, std::vector<const DiagramEdge *>>;

    FrogWidgetDataView canvas_;
    std::span<const DiagramNode> nodes_;
    LeafResolver leaf_;
    std::optional<std::vector<EventSourceSpec>> eventSources_;
    std::unordered_map<std::string,EventDataAuthoringSchema> eventSchemas_;
    EventDataAuthoringSchema pendingEventSchema_;
    std::span<const DiagramEdge> edges_;
    std::unordered_map<std::string, const DiagramNode *> nodesById_;
    std::optional<IncomingEdgeIndex> incomingEdgesByNode_;
    std::unordered_set<std::string> visiting_;
    std::unordered_map<std::string, BindingResult> resolved_;
    // Feedback outputs are previous-state type constraints. A detached pass
    // uses candidates only for these temporal reads, then checks the real
    // producers again; no value, editor hint or default f64 seeds a cycle.
    bool feedbackProbe_ = false;
    bool feedbackSolved_ = false;
    std::unordered_map<std::string, TypeRef> feedbackCandidates_;
    std::unordered_map<std::string, BindingResult> feedbackResults_;
    // Dependency requests are evaluated on a heap-backed endpoint stack.
    // The active frame exists only during a pure node evaluation; unresolved
    // child evidence belongs to that frame, never the persistent memo.
    struct ResolutionFrame;
    ResolutionFrame *evaluationFrame_ = nullptr;
    [[nodiscard]] BindingResult resolveRequest(const DiagramEdgeEndpoint &endpoint, bool literalData);
    [[nodiscard]] BindingResult resolveImpl(const DiagramNode &node, std::string_view port);
    [[nodiscard]] BindingResult paletteFunction(const DiagramNode &node, std::string_view port);
    void solveFeedbackTypes();
    [[nodiscard]] BindingResult feedbackPort(const DiagramNode &node, std::string_view port);
    [[nodiscard]] BindingResult input(const DiagramNode &node, std::string_view port);
    [[nodiscard]] BindingResult tunnelPort(const DiagramNode &node,
        const DiagramCaseStructureTunnel &boundary, std::string_view port);
    [[nodiscard]] std::span<const DiagramEdge *const> incomingEdges(const std::string &nodeId);
};
} // namespace frog::native_engine::binding
