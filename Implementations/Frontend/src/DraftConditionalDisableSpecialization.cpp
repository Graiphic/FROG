#include "frog_native_engine/DraftConditionalDisableSpecialization.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include "frog_native_engine/DisabledStructureData.hpp"
#include "frog_native_engine/StructureTunnelDefaults.hpp"
#include <algorithm>
#include <map>
#include <unordered_map>
#include <unordered_set>

namespace frog::native_engine {
namespace {
bool disabled(const DiagramNode &node) { return node.type == "frog.studio.structures.disabled_structure"; }
// Both families are compile-time containers, never runtime branch selectors.
bool conditional(const DiagramNode &node) {
    return disabled(node) || node.type == "frog.studio.structures.conditional_disable_structure";
}
std::string key(const DiagramEdgeEndpoint &endpoint) { return endpoint.nodeId + '\x1f' + endpoint.portId; }

// Materialize defaults in the immutable compilation graph, in the owning
// region. An existing edge always wins; defaults never write over source data.
// Keeping the generated constant inside its region preserves branch scheduling.
void materializeTunnelDefaults(FrogDraftData &document) {
    binding::TypeResolver types(document.diagramNodes, document.diagramEdges, document.widgets);
    std::vector<DiagramNode> literals;
    std::vector<DiagramEdge> edges;
    std::unordered_set<std::string> nodeIds, edgeIds;
    for (const auto &n : document.diagramNodes) nodeIds.insert(n.id);
    for (const auto &e : document.diagramEdges) edgeIds.insert(e.id);
    const auto unique = [](std::unordered_set<std::string> &ids, std::string candidate) {
        while (!ids.insert(candidate).second) candidate += '_';
        return candidate;
    };
    for (const auto &node : document.diagramNodes) {
        for (const auto &tunnel : node.caseStructureTunnels) {
            if (!diagramTunnelSupportsDefault(node, tunnel) || !tunnel.useDefaultIfUnwired) continue;
            const auto resolved = types.tunnel(node, tunnel);
            if (!resolved.resolution.accepted() || !resolved.resolution.type.known()) continue;
            for (const auto &region : resolved.missingRegions) {
                DiagramNode literal;
                literal.id = unique(nodeIds, "__tunnel_default_" + node.id + "_" + tunnel.id + "_" + region);
                // Existing compiler-default representation supports typed defaults;
                // targets still reject types/structures they cannot execute.
                literal.kind = "conditional_default";
                literal.type = "frog.compiler.default";
                literal.valueType = resolved.resolution.type.canonical();
                literal.diagramOwnerStructureId = node.id;
                literal.diagramOwnerRegionId = region;
                DiagramEdge edge;
                edge.id = unique(edgeIds, literal.id + "_wire");
                edge.from = {literal.id, "value"};
                edge.to = {node.id, binding::tunnelInputId(tunnel.id)};
                edge.diagramOwnerStructureId = node.id;
                edge.diagramOwnerRegionId = region;
                literals.push_back(std::move(literal));
                edges.push_back(std::move(edge));
            }
        }
    }
    for (auto &n : literals) document.diagramNodes.push_back(std::move(n));
    for (auto &e : edges) document.diagramEdges.push_back(std::move(e));
}

// One activity law for borrowed interface inspection and full specialization.
// Its inputs and diagnostics are independent of windows, caches and geometry.
class NodeActivity {
    const std::unordered_map<std::string, const DiagramNode *> &nodes_;
    const ConditionalDisableSymbolMap &symbols_;
    std::vector<ConditionalDisableSelectionTrace> &selections_;
    std::vector<ConditionalDisableDiagnostic> &diagnostics_;
    std::unordered_map<std::string, bool> active_;
    std::unordered_set<std::string> visiting_;
    std::unordered_map<std::string, std::string> selected_;

    void error(std::string code, std::string subject, std::string message,bool incomplete=false) {
        diagnostics_.push_back({std::move(code), std::move(subject), std::move(message),incomplete});
    }
public:
    NodeActivity(const std::unordered_map<std::string, const DiagramNode *> &nodes,
        const ConditionalDisableSymbolMap &symbols,
        std::vector<ConditionalDisableSelectionTrace> &selections,
        std::vector<ConditionalDisableDiagnostic> &diagnostics)
        : nodes_(nodes), symbols_(symbols), selections_(selections), diagnostics_(diagnostics) {}
    bool activeRegion(const std::string &owner, const std::string &region) {
        if (owner.empty()) return true;
        const auto found = nodes_.find(owner);
        if (found == nodes_.end() || !activeNode(*found->second)) return false;
        if (!conditional(*found->second)) return true;
        const auto selected = selected_.find(owner);
        return selected != selected_.end() && selected->second == region;
    }
    bool activeNode(const DiagramNode &node) {
        if (const auto cached = active_.find(node.id); cached != active_.end()) return cached->second;
        if (!visiting_.insert(node.id).second) {
            error("conditional_disable.ownership_cycle", node.id, "Structure ownership contains a cycle.");
            return false;
        }
        const bool active = activeRegion(node.diagramOwnerStructureId, node.diagramOwnerRegionId);
        if (active && disabled(node)) {
            for (const auto &region : disabledStructureRegions(node)) if (region.enabled) {
                selected_[node.id] = region.id; break;
            }
        } else if (active && conditional(node)) {
            const auto selection = resolveConditionalDisableSelection(node.conditionalDisableCases, symbols_);
            const auto region = selection.index ? node.conditionalDisableCases[*selection.index].id : std::string{};
            selections_.push_back({node.id, selection, region});
            if (selection.index) selected_[node.id] = region;
            else error(selection.code, node.id, selection.message,selection.status==ConditionalDisableSelectionStatus::Unresolved);
        }
        visiting_.erase(node.id);
        active_[node.id] = active;
        return active;
    }
    [[nodiscard]] bool isActive(std::string_view id) const {
        const auto found = active_.find(std::string(id));
        return found != active_.end() && found->second;
    }
    [[nodiscard]] bool excluded(std::string_view id) const {
        const auto found = active_.find(std::string(id));
        return found != active_.end() && !found->second;
    }
    [[nodiscard]] const auto &selectedRegions() const { return selected_; }
};

class Specializer {
    const FrogDraftData &source_;
    binding::TypeResolver sourceTypes_;
    DraftConditionalDisableSpecialization result_;
    std::unordered_map<std::string, const DiagramNode *> nodes_;
    NodeActivity activity_;
    std::unordered_map<std::string, DiagramEdgeEndpoint> resolved_;
    std::unordered_map<std::string, std::vector<DiagramEdgeEndpoint>> incoming_;
    std::unordered_set<std::string> resolving_;
    std::unordered_set<std::string> generatedIds_;

    void error(std::string code, std::string subject, std::string message) {
        result_.diagnostics.push_back({std::move(code), std::move(subject), std::move(message)});
    }
    void semantic(std::string_view value) {
        result_.semanticKey += std::to_string(value.size()) + ':' + std::string(value);
    }
    void validateOwner(const std::string &id, const std::string &owner, const std::string &region) {
        if (owner.empty()) {
            if (!region.empty()) error("conditional_disable.orphan_region", id, "Region ownership requires a structure ID.");
            return;
        }
        const auto found = nodes_.find(owner);
        if (found == nodes_.end() || region.empty()) {
            error("conditional_disable.owner_missing", id, "Owner structure/region does not exist."); return;
        }
        if (disabled(*found->second)) {
            const auto regions = disabledStructureRegions(*found->second);
            if (std::none_of(regions.begin(), regions.end(), [&](const auto &r) { return r.id == region; }))
                error("disabled_structure.region_missing", id, "Owner Diagram Disable subdiagram does not exist.");
        } else if (conditional(*found->second) && std::none_of(found->second->conditionalDisableCases.begin(),
            found->second->conditionalDisableCases.end(), [&](const auto &item) { return item.id == region; }))
            error("conditional_disable.region_missing", id, "Owner Conditional Disable subdiagram does not exist.");
    }
    template<class Items> void checkIds(const Items &items, std::string_view kind) {
        std::unordered_set<std::string> ids;
        for (const auto &item : items) {
            if (item.id.empty() || !ids.insert(item.id).second)
                error("conditional_disable.invalid_source_id", item.id, "Empty or duplicate " + std::string(kind) + " ID.");
            validateOwner(item.id, item.diagramOwnerStructureId, item.diagramOwnerRegionId);
        }
    }
    void indexSource() {
        nodes_.reserve(source_.diagramNodes.size());
        for (const auto &node : source_.diagramNodes) { nodes_.emplace(node.id, &node); generatedIds_.insert(node.id); }
        checkIds(source_.diagramNodes, "node"); checkIds(source_.diagramEdges, "wire"); checkIds(source_.diagramWireFragments, "fragment");
        for (const auto &edge : source_.diagramEdges)
            if (!nodes_.contains(edge.from.nodeId) || !nodes_.contains(edge.to.nodeId))
                error("conditional_disable.dangling_source_wire", edge.id, "A source wire references an absent node.");
        // Syntax remains checked even in an excluded parent. Symbol evaluation
        // below is demand-driven and never visits its excluded child structures.
        std::vector<const DiagramNode *> structures;
        for (const auto &node : source_.diagramNodes) if (conditional(node)) {
            structures.push_back(&node);
            if (disabled(node)) {
                const auto regions = disabledStructureRegions(node);
                std::unordered_set<std::string> ids; int enabled = 0;
                for (const auto &region : regions) {
                    enabled += region.enabled ? 1 : 0;
                    if (region.id.empty() || !ids.insert(region.id).second)
                        error("disabled_structure.invalid_region", node.id, "Empty or duplicate Diagram Disable region ID.");
                }
                if (enabled > 1) error("disabled_structure.multiple_enabled", node.id, "Only one subdiagram can be enabled.");
                continue;
            }
            if (const auto message = validateConditionalDisableCases(node.conditionalDisableCases, true); !message.empty())
                error("conditional_disable.invalid", node.id, message);
        }
        std::sort(structures.begin(), structures.end(), [](auto left, auto right) { return left->id < right->id; });
        for (const auto *node : structures) {
            semantic(node->id);
            if (disabled(*node)) {
                for (const auto &region : disabledStructureRegions(*node)) {
                    semantic(region.id); semantic(region.enabled ? "enabled" : "disabled");
                }
                continue;
            }
            for (const auto &region : node->conditionalDisableCases) {
                semantic(region.id); semantic(region.conditionDraft ? "draft" : region.isDefault ? "default" : "condition");
                for (const auto &term : region.anyOf) {
                    semantic("OR");
                    for (const auto &comparison : term.comparisons) {
                        semantic(comparison.symbol); semantic(comparison.comparison); semantic(comparison.value);
                    }
                }
                semantic("end-region");
            }
        }
    }
    template<class Item> void promoteOwner(Item &item) {
        for (std::size_t depth = 0; depth <= nodes_.size() && !item.diagramOwnerStructureId.empty(); ++depth) {
            const auto owner = nodes_.find(item.diagramOwnerStructureId);
            if (owner == nodes_.end() || !conditional(*owner->second)) break;
            item.diagramOwnerStructureId = owner->second->diagramOwnerStructureId;
            item.diagramOwnerRegionId = owner->second->diagramOwnerRegionId;
        }
    }
    DiagramEdgeEndpoint resolve(const DiagramEdgeEndpoint &endpoint) {
        const auto owner = nodes_.find(endpoint.nodeId);
        if (owner == nodes_.end() || !conditional(*owner->second)) return endpoint;
        const auto endpointKey = key(endpoint);
        if (const auto cached = resolved_.find(endpointKey); cached != resolved_.end()) return cached->second;
        if (!resolving_.insert(endpointKey).second) {
            error("conditional_disable.tunnel_cycle", endpoint.nodeId, "A boundary tunnel chain contains a cycle."); return {};
        }
        const auto &node = *owner->second;
        const auto tunnel = std::find_if(node.caseStructureTunnels.begin(), node.caseStructureTunnels.end(),
            [&](const auto &item) { return binding::tunnelOutputId(item.id) == endpoint.portId; });
        DiagramEdgeEndpoint result;
        if (tunnel == node.caseStructureTunnels.end()) {
            error("conditional_disable.port_invalid", node.id, "Conditional Disable has boundary tunnels, not a runtime selector.");
        } else {
            const auto inputKey = key({node.id, binding::tunnelInputId(tunnel->id)});
            const auto input = incoming_.find(inputKey);
            if (input != incoming_.end() && input->second.size() == 1) result = resolve(input->second.front());
            else if (input != incoming_.end() && input->second.size() > 1)
                error("conditional_disable.multiple_tunnel_sources", node.id + ":" + tunnel->id, "The selected subdiagram supplies multiple sources to one tunnel.");
            else if (diagramTunnelSupportsDefault(node, *tunnel) && tunnel->useDefaultIfUnwired) {
                const auto type = sourceTypes_.tunnel(node, *tunnel).resolution;
                if (!type.accepted() || !type.type.known())
                    error("conditional_disable.default_type_unresolved", node.id + ":" + tunnel->id, "An unwired default requires a concrete tunnel type.");
                else {
                    DiagramNode literal;
                    literal.id = "__conditional_default_" + node.id + "_" + tunnel->id;
                    while (!generatedIds_.insert(literal.id).second) literal.id += '_';
                    literal.kind = "conditional_default"; literal.type = "frog.compiler.default";
                    literal.valueType = type.type.canonical();
                    literal.diagramOwnerStructureId = node.diagramOwnerStructureId;
                    literal.diagramOwnerRegionId = node.diagramOwnerRegionId; promoteOwner(literal);
                    result = {literal.id, "value"}; result_.document.diagramNodes.push_back(std::move(literal));
                }
            } else error("conditional_disable.tunnel_unwired", node.id + ":" + tunnel->id,
                "The selected subdiagram requires a source for this tunnel (Use Default If Unwired is off).");
        }
        resolving_.erase(endpointKey); resolved_[endpointKey] = result; return result;
    }
public:
    Specializer(const FrogDraftData &source, const ConditionalDisableCompilationContext &target)
        : source_(source), sourceTypes_(source.diagramNodes, source.diagramEdges, source.widgets),
          activity_(nodes_, result_.symbols, result_.selections, result_.diagnostics) {
        result_.document = source;
        result_.symbols = resolveConditionalDisableSymbols(conditionalDisableContextForDocument(source.properties, target));
        const std::map<std::string, std::string> ordered(result_.symbols.begin(), result_.symbols.end());
        for (const auto &[name, value] : ordered) { semantic(name); semantic(value); }
    }
    DraftConditionalDisableSpecialization run() {
        indexSource();
        for (const auto &node : source_.diagramNodes) static_cast<void>(activity_.activeNode(node));
        auto &document = result_.document;
        std::erase_if(document.diagramNodes, [&](const auto &node) { return !activity_.isActive(node.id) || conditional(node); });
        std::erase_if(document.diagramWireFragments, [&](const auto &item) { return !activity_.activeRegion(item.diagramOwnerStructureId, item.diagramOwnerRegionId); });
        std::erase_if(document.diagramEdges, [&](const auto &edge) {
            if (!activity_.activeRegion(edge.diagramOwnerStructureId, edge.diagramOwnerRegionId)) return true;
            // An absent endpoint is invalid, not an excluded node. Preserve the
            // wire so the ordinary graph validator can report its exact error.
            const auto excluded = [&](const std::string &id) {
                return activity_.excluded(id);
            };
            return excluded(edge.from.nodeId) || excluded(edge.to.nodeId);
        });
        for (const auto &edge : document.diagramEdges) incoming_[key(edge.to)].push_back(edge.from);
        for (const auto &[id, region] : activity_.selectedRegions()) {
            (void)region;
            for (const auto &tunnel : nodes_.at(id)->caseStructureTunnels)
                if (tunnel.outputFromStructure) static_cast<void>(resolve({id, binding::tunnelOutputId(tunnel.id)}));
        }
        for (auto &edge : document.diagramEdges) {
            edge.from = resolve(edge.from);
            promoteOwner(edge);
        }
        std::erase_if(document.diagramEdges, [&](const auto &edge) {
            const auto node = nodes_.find(edge.to.nodeId);
            return node != nodes_.end() && conditional(*node->second);
        });
        for (auto &node : document.diagramNodes) promoteOwner(node);
        for (auto &fragment : document.diagramWireFragments) promoteOwner(fragment);
        materializeTunnelDefaults(document);
        return std::move(result_);
    }
};
}
std::vector<const DiagramNode *> activeConditionalDisableDiagramNodes(
    std::span<const DiagramNode> nodes, const FrogSourceProperties &properties,
    const ConditionalDisableCompilationContext &target) {
    std::unordered_map<std::string, const DiagramNode *> indexed;
    indexed.reserve(nodes.size());
    for (const auto &node : nodes) indexed.emplace(node.id, &node);
    const auto symbols = resolveConditionalDisableSymbols(conditionalDisableContextForDocument(properties, target));
    std::vector<ConditionalDisableSelectionTrace> selections;
    std::vector<ConditionalDisableDiagnostic> diagnostics;
    NodeActivity activity(indexed, symbols, selections, diagnostics);
    for (const auto &node : nodes) static_cast<void>(activity.activeNode(node));
    std::vector<const DiagramNode *> active;
    active.reserve(nodes.size());
    for (const auto &node : nodes)
        if (activity.isActive(node.id) && !conditional(node)) active.push_back(&node);
    return active;
}

DraftConditionalDisableSpecialization specializeConditionalDisableData(const FrogDraftData &source,
    const ConditionalDisableCompilationContext &target) {
    return Specializer(source, target).run();
}
}
