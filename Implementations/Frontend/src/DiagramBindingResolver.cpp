#include "frog_native_engine/DiagramBindingResolver.hpp"
#include "frog_native_engine/DiagramTunnelIdentity.hpp"
#include "frog_native_engine/SequenceStructure.hpp"
#include "frog_native_engine/ApplicationControlSchema.hpp"
#include "frog_native_engine/ConnectivityCatalog.hpp"
#include "frog_native_engine/ExponentialBinding.hpp"
#include "frog_native_engine/HyperbolicBinding.hpp"
#include "frog_native_engine/TrigonometricBinding.hpp"
#include "frog_native_engine/GatingBinding.hpp"
#include "frog_native_engine/DiscreteMathBinding.hpp"
#include "frog_native_engine/BesselBinding.hpp"
#include "frog_native_engine/GammaBinding.hpp"
#include "frog_native_engine/HypergeometricBinding.hpp"
#include "frog_native_engine/EllipticBinding.hpp"
#include "frog_native_engine/ExponentialIntegralsBinding.hpp"
#include "frog_native_engine/ErrorFunctionsBinding.hpp"
#include "frog_native_engine/EllipticParabolicBinding.hpp"
#include "frog_native_engine/OrdinaryDifferentialBinding.hpp"
#include "frog_native_engine/PartialDifferentialBinding.hpp"
#include "frog_native_engine/ObjectReferenceSchema.hpp"
#include "frog_native_engine/InPlaceStructure.hpp"
#include "frog_native_engine/DiagramLiteralSchema.hpp"
#include "frog_native_engine/DiagramStructureCatalog.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/FunctionBindingContract.hpp"
#include "frog_native_engine/StructureBindingDefinition.hpp"
#include "frog_native_engine/StructureTunnelMode.hpp"
#include "frog_native_engine/StructureTunnelDefaults.hpp"
#include "FunctionPolymorphism.hpp"
#include "StructureShelfPort.hpp"
#include "frog_native_engine/ArrayEditSemantics.hpp"
#include "frog_native_engine/BuildArrayBinding.hpp"
#include "frog_native_engine/ReshapeArraySemantics.hpp"
#include "frog_native_engine/ArrayPaletteSemantics.hpp"
#include "frog_native_engine/StringPaletteSemantics.hpp"
#include "frog_native_engine/FileBinding.hpp"
#include "frog_native_engine/QueueFunctionBinding.hpp"
#include "frog_native_engine/NotifierFunctionBinding.hpp"
#include "frog_native_engine/SemaphoreFunctionBinding.hpp"
#include "frog_native_engine/RendezvousFunctionBinding.hpp"
#include "frog_native_engine/OccurrenceFunctionBinding.hpp"
#include "frog_native_engine/EventFunctionBinding.hpp"
#include "frog_native_engine/ClusterFunctionBinding.hpp"
#include "frog_native_engine/CollectionBinding.hpp"
#include "frog/math/formula_operation_contract.hpp"

#include <algorithm>
#include <charconv>
#include <map>
#include <utility>

namespace frog::native_engine::binding {
namespace {
bool literalPrototypeMatchesDeclaration(const TypeRef &declared,const TypeRef &actual) {
    // The textual type carries structural identities, ranks and domains. The
    // actual prototype additionally carries labels and choice tables which
    // that spelling cannot encode. Preserve that evidence without allowing a
    // prototype to replace any explicitly declared constraint.
    std::vector<std::pair<const TypeRef *,const TypeRef *>> pending{{&declared,&actual}};
    while(!pending.empty()) {
        const auto [expected,observed]=pending.back();pending.pop_back();
        if(expected->kind!=observed->kind || expected->name!=observed->name || expected->rank!=observed->rank ||
           expected->extent!=observed->extent || expected->fieldIds!=observed->fieldIds ||
           expected->children.size()!=observed->children.size() ||
           (!expected->fieldNames.empty() && expected->fieldNames!=observed->fieldNames) ||
           (expected->choiceKind!=ChoiceKind::None && expected->choiceKind!=observed->choiceKind) ||
           (!expected->choices.empty() && expected->choices!=observed->choices))return false;
        for(std::size_t i=0;i<expected->children.size();++i)
            pending.emplace_back(&expected->children[i],&observed->children[i]);
    }
    return true;
}
bool containsEmptyCluster(const TypeRef &type) {
    std::vector<const TypeRef *> pending{&type};
    while (!pending.empty()) {
        const auto *current = pending.back();
        pending.pop_back();
        // Collection models describe complete value schemas, independently of
        // their cardinality. Their validated zero-field Records are values,
        // not the unconfigured ordinary Cluster widget's empty prototype.
        if (current->kind == TypeKind::Map || current->kind == TypeKind::Set) continue;
        if (current->kind == TypeKind::Cluster && current->children.empty()) return true;
        for (const auto &child : current->children) pending.push_back(&child);
    }
    return false;
}
BindingResult fromParsed(const TypeParseResult &parsed) {
    if (!parsed.valid()) return {{ResolutionStatus::Incompatible, {}, "binding.invalid_type", parsed.error}, false, {}};
    if (!parsed.type.known()) return {{ResolutionStatus::Unresolved, parsed.type,
        "binding.type_unresolved", "The binding has no resolved type."}, false, {}};
    if (containsEmptyCluster(parsed.type)) return {{ResolutionStatus::Unresolved, parsed.type,
        "binding.container_incomplete", "A Cluster value requires a non-empty field schema."}, false, {}};
    return {{ResolutionStatus::Compatible, parsed.type, {}, {}}, true, {}};
}
TypeResolution connectShiftRegister(const TypeRef &source, const TypeRef &target) {
    // Scalar numeric writes may adapt to the initializer's representation.
    // Array elements, cluster fields and enum domains retain their exact schema.
    return connect(source, target,
        source.kind == TypeKind::Primitive && target.kind == TypeKind::Primitive
            ? ConnectionPolicy::NumericImplicit : ConnectionPolicy::Exact);
}
const DiagramCaseStructureTunnel *findTunnel(const DiagramNode &node, std::string_view port) {
    if (!isCase(node) && !isDiagramStructureNodeType(node.type)) return nullptr;
    for (const auto &tunnel : node.caseStructureTunnels) {
        if (port == tunnelInputId(tunnel.id) || port == tunnelOutputId(tunnel.id) ||
            (tunnel.conditional && port == diagramTunnelConditionPortId(tunnel.id))) return &tunnel;
    }
    return nullptr;
}
const FunctionPortBinding *functionPort(const FunctionBindingDefinition &definition, std::string_view id) {
    if (const auto *port = findFunctionBindingPort(definition, id)) return port;
    const auto ordinal = functionBindingInputPortOrdinal(definition, id);
    if (!ordinal || (definition.maximumInputCount && *ordinal >= definition.maximumInputCount)) return nullptr;
    for (const auto &port : definition.ports)
        if (port.direction == FunctionPortDirection::Input) return &port;
    return nullptr;
}
// String data/default terminals stay polymorphic. Only the published scalar
// navigation and formatting parameters have a fixed receiver representation.
// Keep that target separate from the actual producer's complete type; FIR then
// derives the existing NumericImplicit conversion from their real authored edge.
std::optional<TypeRef> fixedStringNumericParameterType(std::string_view catalog,
    const FunctionPortBinding &port) {
    if (!catalog.starts_with("string.") || port.direction != FunctionPortDirection::Input ||
        port.connectionPolicy != FunctionPortConnectionPolicy::NumericImplicit) return std::nullopt;
    const auto id = std::string_view(port.portId);
    if (id != "offset" && id != "length" && id != "location" && id != "start_index" &&
        id != "line_index" && id != "width" && id != "precision" &&
        id != "initial_scan_location") return std::nullopt;
    const auto target = parseType(port.defaultValueType);
    if (!target.valid() || target.type.kind != TypeKind::Primitive || !target.type.numeric())
        return std::nullopt;
    return target.type;
}
const FunctionPortBinding *unaryNumericFeedbackInput(const DiagramNode &node,
    const FunctionBindingDefinition &definition, const FunctionPortBinding &output,
    const TypeRef &candidate) {
    if (definition.resolverId != kStudioPolymorphicResolver ||
        !definition.variadicInputPrefix.empty()) return nullptr;
    const FunctionPortBinding *input = nullptr;
    for (const auto &port : definition.ports) {
        if (port.direction != FunctionPortDirection::Input) continue;
        if (input || port.cardinality != FunctionPortCardinality::Single) return nullptr;
        input = &port;
    }
    if (!input) return nullptr;
    const auto expression = parseTypeExpr(input->typeExpression);
    if (!expression.valid()) return nullptr;
    const auto constraint = expression.expression.kind;
    if (constraint != ConstraintKind::Numeric && constraint != ConstraintKind::Integer &&
        constraint != ConstraintKind::NumericTree &&
        constraint != ConstraintKind::NumericConvertibleTree) return nullptr;
    if (!constrain(candidate, expression.expression).accepted()) return nullptr;
    // A declared representation is a type candidate only. Its actual unary
    // contract must preserve that representation around the temporal cycle;
    // an operation that changes shape or representation cannot seed itself.
    const BindingResult seed{{ResolutionStatus::Compatible, candidate, {}, {}}, false, {}};
    const std::vector<std::string> wiredInputs{input->portId};
    const auto inferred = resolvePolymorphicFunction(node, definition, output.portId,
        [&](std::string_view port) {
            return port == input->portId ? seed : BindingResult{
                {ResolutionStatus::Unresolved, {}, {}, {}}, false, {}};
        }, wiredInputs);
    return inferred.resolution.accepted() && inferred.resolution.type == candidate
        ? input : nullptr;
}
TypeResolution promote(std::span<const TypeRef> types) {
    std::vector<std::string> storage;
    storage.reserve(types.size());
    for (const auto &type : types) if (type.known()) storage.push_back(type.canonical());
    std::vector<std::string_view> operands;
    operands.reserve(storage.size());
    for (const auto &text : storage) operands.push_back(text);
    const auto result = resolveNumericAddValueTypes(operands);
    if (result.resolved()) return {ResolutionStatus::Compatible, parseType(result.resolvedValueType).type, {}, {}};
    if (result.status == FunctionTypeResolutionStatus::NoOperands)
        return {ResolutionStatus::Unresolved, {}, "binding.type_unresolved", "No numeric operand determines this binding."};
    return {ResolutionStatus::Incompatible, {}, "binding.numeric_operands",
        "Numeric operand " + std::to_string(result.operandIndex + 1U) + " violates the numeric type/shape contract."};
}
}
bool isCase(const DiagramNode &node) {
    return node.type == diagramStructureNodeType(DiagramStructureFamily::Conditional) || node.type == "case";
}
std::string tunnelInputId(std::string_view id) { return diagram_identity::tunnelInputId(id); }
std::string tunnelOutputId(std::string_view id) { return diagram_identity::tunnelOutputId(id); }

TypeResolver::TypeResolver(std::span<const DiagramNode> nodes, std::span<const DiagramEdge> edges,
    FrogWidgetDataView canvas, LeafResolver leaf)
    : canvas_(canvas), nodes_(nodes), leaf_(std::move(leaf)), edges_(edges) {
    nodesById_.reserve(nodes.size());
    for (const auto &node : nodes) nodesById_.emplace(node.id, &node);
}
std::span<const DiagramEdge *const> TypeResolver::incomingEdges(const std::string &nodeId) {
    if (!incomingEdgesByNode_) {
        // Terminal-only resolvers never construct a map or inspect the edges.
        // Stage the index so an allocation failure cannot publish a partial map.
        IncomingEdgeIndex index;
        index.reserve((std::min)(nodesById_.size(), edges_.size()));
        // Preserve document order, duplicate sinks and region-specific sources.
        for (const auto &edge : edges_) index[edge.to.nodeId].push_back(&edge);
        incomingEdgesByNode_.emplace(std::move(index));
    }
    const auto found = incomingEdgesByNode_->find(nodeId);
    return found == incomingEdgesByNode_->end() ? std::span<const DiagramEdge *const>{} : found->second;
}
struct TypeResolver::ResolutionFrame {
    DiagramEdgeEndpoint endpoint;
    std::string key;
    std::string literalAncestor;
    std::unordered_map<std::string, BindingResult> dependencies;
    std::optional<std::pair<DiagramEdgeEndpoint, bool>> pending;
};
BindingResult TypeResolver::resolve(const DiagramEdgeEndpoint &endpoint) {
    if(const auto node=nodesById_.find(endpoint.nodeId);node!=nodesById_.end() &&
        node->second->type=="frog.studio.structures.event_structure" && endpoint.portId.starts_with("event_response."))
        return {{ResolutionStatus::Unresolved,{},"binding.event_region_required","Event response typing requires its actual case region."},false,{}};
    const auto node=nodesById_.find(endpoint.nodeId);
    if(node!=nodesById_.end() && node->second->type=="frog.studio.structures.event_structure" &&
        endpoint.portId.starts_with("event_data.")) {
        auto field=std::string_view(endpoint.portId).substr(11);field=field.substr(0,field.find('#'));
        if(field!="source" && field!="type" && field!="time")
            return {{ResolutionStatus::Unresolved,{},"binding.event_region_required",
                "Event-specific data requires the wire's explicit semantic region."},false,{}};
    }
    return resolveRequest(endpoint, false);
}
const EventDataAuthoringSchema &TypeResolver::eventSchema(
    std::string_view structureId,std::string_view regionId) {
    const auto key=std::string(structureId)+'\x1f'+std::string(regionId);
    if(const auto cached=eventSchemas_.find(key);cached!=eventSchemas_.end())return cached->second;
    EventDataAuthoringSchema schema;
    const auto node=nodesById_.find(std::string(structureId));
    if(node==nodesById_.end())schema.error="The Event Structure does not exist.";
    else {
        const auto registration=resolve({node->second->id,"event_registration"});
        if(registration.resolution.code=="binding.dependency_pending") {
            pendingEventSchema_={};pendingEventSchema_.error="The dynamic registration schema is pending.";
            return pendingEventSchema_;
        }
        const auto sources=eventStructureSources(*node->second,registration.resolution.type,canvas_);
        schema=eventCaseDataSchema(*node->second,regionId,sources);
    }
    return eventSchemas_.emplace(key,std::move(schema)).first->second;
}
BindingResult TypeResolver::resolveSource(const DiagramEdge &edge) {
    return resolveInRegion(edge.from,edge.diagramOwnerStructureId,edge.diagramOwnerRegionId);
}
BindingResult TypeResolver::resolveInRegion(const DiagramEdgeEndpoint &endpoint,
    std::string_view ownerStructureId, std::string_view regionId) {
    const auto found=nodesById_.find(endpoint.nodeId);
    if(found!=nodesById_.end() && found->second->type=="frog.studio.structures.event_structure" &&
        endpoint.portId.starts_with("event_response.")) {
        const auto fail=[](std::string code,std::string message) {
            return BindingResult{{ResolutionStatus::Incompatible,{},std::move(code),std::move(message)},false,{}};
        };
        if(ownerStructureId!=endpoint.nodeId || regionId.empty())
            return fail("binding.event_scope","Event responses must be written inside their actual Event case.");
        const auto &schema=eventSchema(endpoint.nodeId,regionId);
        if(!schema.valid())return fail("binding.event_schema",schema.error);
        auto field=std::string_view(endpoint.portId).substr(15);
        std::size_t occurrence=0;
        if (const auto hash=field.find('#');hash!=std::string_view::npos) {
            const auto suffix=field.substr(hash+1);
            const auto parsed=std::from_chars(suffix.data(),suffix.data()+suffix.size(),occurrence);
            if (suffix.empty() || parsed.ec!=std::errc{} || parsed.ptr!=suffix.data()+suffix.size() || occurrence==0)
                return fail("binding.event_response_occurrence","The response row occurrence is invalid.");
            field=field.substr(0,hash);
        }
        const auto &node = *found->second;
        const auto region = std::find_if(node.eventStructureCases.begin(),node.eventStructureCases.end(),
            [&](const auto &candidate) { return candidate.id == regionId; });
        if (region == node.eventStructureCases.end())
            return fail("binding.unknown_region","The Event case does not exist.");
        if (region->eventResponseFields) {
            const auto count = std::count(region->eventResponseFields->begin(),region->eventResponseFields->end(),field);
            if (static_cast<std::size_t>(count)<=occurrence)
                return fail("binding.event_response_missing","This response input is not exposed in the wire's Event case.");
        } else if (occurrence!=0)
            return fail("binding.event_response_missing","Automatic response selection exposes only the first row occurrence.");
        const auto available=std::find_if(schema.responses.begin(),schema.responses.end(),
            [&](const auto &item){return item.id==field;});
        if(available==schema.responses.end())
            return fail("binding.event_response_unavailable","This Event case does not declare that response input.");
        return fromParsed({available->type,{},false});
    }
    if(found==nodesById_.end() || found->second->type!="frog.studio.structures.event_structure" ||
        !endpoint.portId.starts_with("event_data.")) return resolve(endpoint);
    const auto fail=[](std::string code,std::string message) {
        return BindingResult{{ResolutionStatus::Incompatible,{},std::move(code),std::move(message)},false,{}};
    };
    const auto &node=*found->second;
    if(ownerStructureId!=node.id || regionId.empty())
        return fail("binding.event_scope","Event data must be read inside its actual Event case.");
    const auto region=std::find_if(node.eventStructureCases.begin(),node.eventStructureCases.end(),
        [&](const auto &item){return item.id==regionId;});
    if(region==node.eventStructureCases.end())return fail("binding.unknown_region","The Event case does not exist.");
    auto field=std::string_view(endpoint.portId).substr(11);std::size_t occurrence=0;
    if(const auto hash=field.find('#');hash!=std::string_view::npos) {
        const auto suffix=field.substr(hash+1);
        const auto parsed=std::from_chars(suffix.data(),suffix.data()+suffix.size(),occurrence);
        if(suffix.empty() || parsed.ec!=std::errc{} || parsed.ptr!=suffix.data()+suffix.size())
            return fail("binding.event_field","The Event field occurrence is invalid.");
        field=field.substr(0,hash);
    }
    if(static_cast<std::size_t>(std::count(region->eventDataFields.begin(),region->eventDataFields.end(),field))<=occurrence)
        return fail("binding.event_field_missing","This field occurrence is not exposed in the wire's Event case.");
    const auto &schema=eventSchema(node.id,regionId);
    if(!schema.valid())return fail("binding.event_schema",schema.error);
    const auto available=std::find_if(schema.fields.begin(),schema.fields.end(),[&](const auto &item){return item.id==field;});
    if(available==schema.fields.end())return fail("binding.event_field_unavailable","The configured events do not share this data field.");
    // Direct source schema, not a cached editor type or a regionless memo.
    return fromParsed({available->type,{},false});
}
BindingResult TypeResolver::resolveRequest(const DiagramEdgeEndpoint &endpoint, bool literalData) {
    const auto node = nodesById_.find(endpoint.nodeId);
    if (!literalData && node != nodesById_.end() && !node->second->constantOwnerNodeId.empty())
        return {{ResolutionStatus::Incompatible, {}, "binding.literal_field_not_terminal", "A cluster constant field is data, not a wire terminal."}, false, {}};
    const auto cycle = [] {
        return BindingResult{{ResolutionStatus::Unresolved, {}, "binding.cycle_unresolved",
            "A cyclic binding has no resolved source."}, false, {}};
    };
    const auto literalCycle = [] {
        return BindingResult{{ResolutionStatus::Incompatible, {}, "binding.literal_cycle",
            "Invalid cluster constant nesting."}, false, {}};
    };
    const auto pendingDependency = [] {
        return BindingResult{{ResolutionStatus::Unresolved, {}, "binding.dependency_pending",
            ""}, false, {}};
    };
    const auto keyFor = [](const DiagramEdgeEndpoint &value, bool data) {
        return data ? "literal-data\x1f" + value.nodeId : value.nodeId + '\x1f' + value.portId;
    };
    const auto key = keyFor(endpoint, literalData);
    if (const auto found = resolved_.find(key); found != resolved_.end()) return found->second;
    if (visiting_.contains(key)) return literalData ? literalCycle() : cycle();
    if (node != nodesById_.end() && endpoint.portId == "value" &&
        isDiagramClusterConstant(*node->second) && visiting_.contains("literal:" + endpoint.nodeId))
        return literalCycle();
    if (evaluationFrame_) {
        if (const auto found = evaluationFrame_->dependencies.find(key);
            found != evaluationFrame_->dependencies.end()) return found->second;
        // Stop at the first unevaluated dependency. The provisional result is
        // discarded by the driver, including any fallback reached after it.
        if (!evaluationFrame_->pending) evaluationFrame_->pending.emplace(endpoint, literalData);
        return pendingDependency();
    }

    std::vector<ResolutionFrame> work;
    const auto push = [&](const DiagramEdgeEndpoint &value, bool data) {
        ResolutionFrame frame;
        frame.endpoint = value;
        frame.key = keyFor(value, data);
        const auto found = nodesById_.find(value.nodeId);
        if (found != nodesById_.end() && value.portId == "value" && isDiagramClusterConstant(*found->second))
            frame.literalAncestor = "literal:" + value.nodeId;
        work.push_back(std::move(frame));
        visiting_.insert(work.back().key);
        if (!work.back().literalAncestor.empty()) visiting_.insert(work.back().literalAncestor);
    };
    struct Guard {
        ResolutionFrame *&active;
        std::unordered_set<std::string> &visiting;
        std::vector<ResolutionFrame> &work;
        ~Guard() {
            active = nullptr;
            for (const auto &frame : work) {
                visiting.erase(frame.key);
                if (!frame.literalAncestor.empty()) visiting.erase(frame.literalAncestor);
            }
        }
    } guard{evaluationFrame_, visiting_, work};
    push(endpoint, literalData);
    while (!work.empty()) {
        auto &frame = work.back();
        evaluationFrame_ = &frame;
        const auto current = nodesById_.find(frame.endpoint.nodeId);
        auto result = current == nodesById_.end()
            ? BindingResult{{ResolutionStatus::Incompatible, {}, "binding.missing_node", "Binding node does not exist."}, false, {}}
            : resolveImpl(*current->second, frame.endpoint.portId);
        evaluationFrame_ = nullptr;
        if (frame.pending) {
            auto dependency = std::move(*frame.pending);
            frame.pending.reset();
            push(dependency.first, dependency.second);
            continue;
        }
        // Unknown evidence may change when another ancestry is explored. Only
        // this caller retains it; known/incompatible results keep the memo.
        if (result.resolution.status != ResolutionStatus::Unresolved) resolved_.emplace(frame.key, result);
        const auto completedKey = std::move(frame.key);
        visiting_.erase(completedKey);
        if (!frame.literalAncestor.empty()) visiting_.erase(frame.literalAncestor);
        work.pop_back();
        if (work.empty()) return result;
        work.back().dependencies.insert_or_assign(completedKey, std::move(result));
    }
    return cycle(); // The first frame always returns above.
}
BindingResult TypeResolver::resolveLiteral(std::string_view nodeId) {
    const auto node = nodesById_.find(std::string(nodeId));
    if (node == nodesById_.end() || !isDiagramLiteralConstant(*node->second))
        return {{ResolutionStatus::Incompatible, {}, "binding.not_literal", "The node is not a literal constant."}, false, {}};
    return resolveRequest({node->second->id, std::string(diagramLiteralValuePortId(*node->second))}, true);
}
BindingResult TypeResolver::input(const DiagramNode &node, std::string_view port) {
    const DiagramEdge *source = nullptr;
    const auto *entry = findFunctionCatalogEntryByDiagramNodeType(node.type);
    const auto canonical = [&](std::string_view id) {
        return entry ? canonicalFunctionBindingPortId(entry->catalogId, id) : id;
    };
    for (const auto *edge : incomingEdges(node.id)) {
        if (canonical(edge->to.portId) != canonical(port)) continue;
        if (source != nullptr) return {{ResolutionStatus::Incompatible, {}, "binding.multiple_sources",
            "An input has more than one source."}, false, {}};
        source = edge;
    }
    if (source != nullptr && direction(source->from) != FunctionPortDirection::Output)
        return {{ResolutionStatus::Incompatible, {}, "binding.direction", "An input must be driven by an output."}, false, {}};
    return source == nullptr ? fromParsed({{}, {}, false}) : resolveSource(*source);
}
void TypeResolver::solveFeedbackTypes() {
    std::vector<const DiagramNode *> feedback;
    for(const auto &node:nodes_)if(node.type=="frog.studio.structures.feedback_node")feedback.push_back(&node);
    std::unordered_map<std::string,TypeRef> candidates;
    const auto wired=[&](const DiagramNode &node,std::string_view port){const auto edges=incomingEdges(node.id);return std::any_of(edges.begin(),edges.end(),[&](const auto *edge){return edge->to.portId==port;});};
    const auto initialType=[](const DiagramNode &node,const BindingResult &initial)->std::optional<TypeRef>{
        if(!initial.resolution.accepted()||!initial.resolution.type.known())return {};
        const auto &type=initial.resolution.type;
        if(!node.feedbackHistory)return type;
        if(type.kind!=TypeKind::Array||type.rank!=1||type.children.size()!=1)return {};
        return type.children.front(); // The outer history never flattens T.
    };
    // Reuse the ordinary operator/type contracts in detached read-only passes.
    // Only previous-state outputs receive candidate types; data, initializer
    // and Enable producers are evaluated through their actual connections.
    const auto maximumPasses=(std::min)(feedback.size()+32U,std::size_t{4096});
    const auto settle=[&]{
        for(std::size_t pass=0;pass<maximumPasses;++pass){
            TypeResolver probe(nodes_,edges_,canvas_,leaf_);probe.feedbackProbe_=true;probe.feedbackCandidates_=candidates;
            auto next=candidates;bool changed=false;
            for(const auto *node:feedback){
                const auto initial=wired(*node,"initial_value")?probe.input(*node,"initial_value"):BindingResult{};
                auto type=initialType(*node,initial);
                if(!type){const auto data=probe.input(*node,"value");if(data.resolution.accepted()&&data.resolution.type.known())type=data.resolution.type;}
                if(type){const auto old=next.find(node->id);if(old==next.end()||!fullTypeEqual(old->second,*type)){next.insert_or_assign(node->id,std::move(*type));changed=true;}}
            }
            candidates.swap(next);if(!changed)return true;
        }
        return false;
    };
    bool stable=settle();
    // A closed unary numeric recurrence has a declared operator representation
    // even without an initializer (for example previous -> Increment -> next).
    // Consult that contract only after all real type evidence has settled. A
    // bare Feedback, another Feedback, an open path or an unresolved explicit
    // initializer must not acquire a default state type.
    const auto closedUnaryDefault=[&](const DiagramNode &memory)->std::optional<TypeRef>{
        if(wired(memory,"initial_value"))return {};
        const DiagramNode *consumer=&memory;
        std::string_view inputPort="value";
        std::optional<TypeRef> candidate;
        std::unordered_set<std::string> visited;
        while(true){
            const auto *consumerEntry=findFunctionCatalogEntryByDiagramNodeType(consumer->type);
            const auto canonical=[&](std::string_view id){return consumerEntry?canonicalFunctionBindingPortId(consumerEntry->catalogId,id):id;};
            const DiagramEdge *driver=nullptr;
            for(const auto *edge:incomingEdges(consumer->id)){
                if(canonical(edge->to.portId)!=inputPort){if(consumer!=&memory)return {};continue;}
                if(driver)return {};
                driver=edge;
            }
            if(!driver)return {};
            if(driver->from.nodeId==memory.id){
                if(driver->from.portId!="result")return {};
                return candidate;
            }
            const auto producer=nodesById_.find(driver->from.nodeId);
            if(producer==nodesById_.end()||producer->second->kind!="primitive"||!visited.insert(producer->first).second)return {};
            const auto *entry=findFunctionCatalogEntryByDiagramNodeType(producer->second->type);
            const auto *definition=entry?findFunctionBindingDefinition(entry->catalogId):nullptr;
            const auto *output=definition?functionPort(*definition,canonicalFunctionBindingPortId(entry->catalogId,driver->from.portId)):nullptr;
            if(!output||output->direction!=FunctionPortDirection::Output)return {};
            if(!candidate){
                const auto declared=parseType(output->defaultValueType);
                if(!declared.valid()||declared.type.kind!=TypeKind::Primitive||!declared.type.numeric())return {};
                candidate=declared.type;
            }
            const auto *input=unaryNumericFeedbackInput(*producer->second,*definition,*output,*candidate);
            if(!input)return {};
            consumer=producer->second;inputPort=input->portId;
        }
    };
    if(stable){
        bool seeded=false;
        for(const auto *node:feedback)if(!candidates.contains(node->id))
            if(auto type=closedUnaryDefault(*node)){candidates.emplace(node->id,std::move(*type));seeded=true;}
        if(seeded)stable=settle();
    }
    std::unordered_map<std::string,BindingResult> results;
    TypeResolver probe(nodes_,edges_,canvas_,leaf_);probe.feedbackProbe_=true;probe.feedbackCandidates_=candidates;
    const auto invalid=[](std::string code,std::string message){return BindingResult{{ResolutionStatus::Incompatible,{},std::move(code),std::move(message)},false,{}};};
    // An initializer cannot depend on the state it must create. For another
    // Feedback output only its initialization dependency is relevant here;
    // its current data input is a separate temporal write, not such a path.
    const auto initializerCycle=[&](const DiagramNode &owner){
        std::vector<DiagramEdgeEndpoint> work;std::unordered_set<std::string> seen;
        for(const auto *edge:incomingEdges(owner.id))if(edge->to.portId=="initial_value")work.push_back(edge->from);
        while(!work.empty()){
            auto current=std::move(work.back());work.pop_back();
            if(current.nodeId==owner.id&&current.portId=="result")return true;
            if(!seen.insert(current.nodeId+'\x1f'+current.portId).second)continue;
            const auto found=nodesById_.find(current.nodeId);if(found==nodesById_.end())continue;
            const bool temporal=found->second->type=="frog.studio.structures.feedback_node"&&current.portId=="result";
            for(const auto *edge:incomingEdges(current.nodeId))if(!temporal||edge->to.portId=="initial_value")work.push_back(edge->from);
        }
        return false;
    };
    for(const auto *node:feedback){
        BindingResult result;
        const auto candidate=candidates.find(node->id);
        if(!stable)result={{ResolutionStatus::Unresolved,{},"binding.feedback_type_budget","Feedback type constraints did not reach a stable type within the bounded pass budget."},false,{}};
        else if(initializerCycle(*node))result=invalid("binding.feedback_initializer_cycle","A Feedback initializer depends on the previous-state output it must initialize.");
        else if(candidate==candidates.end())result={{ResolutionStatus::Unresolved,{},"binding.feedback_type_unresolved","Connect an initializer or a type-defining data source; no default numeric type is inferred for Feedback."},false,{}};
        else {
            result={{ResolutionStatus::Compatible,candidate->second,{},{}},true,{}};
            const auto data=probe.input(*node,"value");
            if(data.resolution.status==ResolutionStatus::Incompatible)result=data;
            else if(data.resolution.accepted()&&data.resolution.type.known()&&!fullTypeEqual(data.resolution.type,candidate->second))result=invalid("binding.feedback_type_conflict","The Feedback input must retain the exact previous-state type and metadata.");
            else result.complete=data.complete&&data.resolution.accepted();
            if(result.resolution.accepted()&&wired(*node,"initial_value")){
                const auto initial=probe.input(*node,"initial_value");const auto type=initialType(*node,initial);
                if(initial.resolution.status==ResolutionStatus::Incompatible)result=initial;
                else if(initial.resolution.accepted()&&(!type||!fullTypeEqual(*type,candidate->second)))result=invalid(node->feedbackHistory?"binding.feedback_history_type":"binding.feedback_initializer_type",
                    node->feedbackHistory?"History requires a rank-one Array of complete T samples; T itself may be an Array.":"The Feedback initializer must have the exact stored type and metadata.");
                else result.complete=result.complete&&initial.complete&&initial.resolution.accepted();
            }
            if(result.resolution.accepted()&&wired(*node,"enable")){
                const auto enable=probe.input(*node,"enable");
                if(enable.resolution.status==ResolutionStatus::Incompatible)result=enable;
                else if(enable.resolution.accepted()&&enable.resolution.type!=TypeRef::primitive("bool"))result=invalid("binding.feedback_enable_type","Feedback Enable requires Boolean without a numeric conversion.");
                else result.complete=result.complete&&enable.complete&&enable.resolution.accepted();
            }
        }
        results.emplace(node->id,std::move(result));
    }
    feedbackCandidates_.swap(candidates);feedbackResults_.swap(results);feedbackSolved_=true;
}
BindingResult TypeResolver::feedbackPort(const DiagramNode &node,std::string_view port){
    if(port!="value"&&port!="result"&&port!="initial_value"&&port!="enable")return {{ResolutionStatus::Incompatible,{},"binding.missing_port","Feedback has no such port."},false,{}};
    if(port=="enable"){
        const auto incoming=incomingEdges(node.id);const bool wired=std::any_of(incoming.begin(),incoming.end(),[](const auto *edge){return edge->to.portId=="enable";});
        if(!wired)return fromParsed(parseType("bool"));
        auto value=input(node,"enable");if(value.resolution.accepted()&&value.resolution.type!=TypeRef::primitive("bool"))return {{ResolutionStatus::Incompatible,{},"binding.feedback_enable_type","Feedback Enable requires Boolean."},false,{}};
        return value;
    }
    BindingResult result;
    if(feedbackProbe_){
        const auto candidate=feedbackCandidates_.find(node.id);
        result=candidate==feedbackCandidates_.end()?BindingResult{{ResolutionStatus::Unresolved,{},"binding.feedback_type_unresolved","The previous-state type is not yet constrained."},false,{}}
            :BindingResult{{ResolutionStatus::Compatible,candidate->second,{},{}},true,{}};
    }else{
        if(!feedbackSolved_)solveFeedbackTypes();const auto found=feedbackResults_.find(node.id);
        if(found==feedbackResults_.end())return {{ResolutionStatus::Unresolved,{},"binding.feedback_type_unresolved","The Feedback node has no type evidence."},false,{}};
        result=found->second;
    }
    if(port=="initial_value"&&node.feedbackHistory&&result.resolution.type.known())result.resolution.type=TypeRef::array(std::move(result.resolution.type));
    if(!feedbackProbe_&&port=="initial_value"&&result.resolution.accepted()){
        const auto incoming=incomingEdges(node.id);const bool wired=std::any_of(incoming.begin(),incoming.end(),[](const auto *edge){return edge->to.portId=="initial_value";});
        if(!wired)result.complete=true; // Optional typed default, not missing data.
        else {const auto source=input(node,"initial_value");if(source.resolution.code=="binding.dependency_pending")return source;result.complete=source.complete&&source.resolution.accepted()&&fullTypeEqual(source.resolution.type,result.resolution.type);}
    }
    return result;
}
BindingResult TypeResolver::tunnel(const DiagramNode &node, const DiagramCaseStructureTunnel &boundary) {
    std::vector<TypeRef> types;
    const DiagramCaseStructureTunnel *shiftInitializer=nullptr;
    if(!boundary.shiftRegisterPairId.empty()) for(const auto *edge:incomingEdges(node.id)) {
        const auto *member=findTunnel(node,edge->to.portId);
        if(!member || member->outputFromStructure || member->shiftRegisterPairId!=boundary.shiftRegisterPairId ||
            edge->to.portId!=tunnelInputId(member->id)) continue;
        // The topmost initialized history determines the common representation.
        // Other slots have independent initial values and convert to this type.
        if(!shiftInitializer || member->shiftRegisterHistoryIndex<shiftInitializer->shiftRegisterHistoryIndex ||
            (member->shiftRegisterHistoryIndex==shiftInitializer->shiftRegisterHistoryIndex && member->id<shiftInitializer->id))
            shiftInitializer=member;
    }
    const bool hasShiftInitializer=shiftInitializer!=nullptr;
    std::unordered_map<std::string, std::size_t> sourcesByRegion;
    bool allSourcesComplete = true;
    const auto port = tunnelInputId(boundary.id);
    for (const auto *incoming : incomingEdges(node.id)) {
        const auto &edge = *incoming;
        const auto *member = &boundary;
        if (edge.to.portId != port) {
            member = boundary.shiftRegisterPairId.empty() ? nullptr : findTunnel(node, edge.to.portId);
            if (member == nullptr || member->shiftRegisterPairId != boundary.shiftRegisterPairId ||
                edge.to.portId != tunnelInputId(member->id)) continue;
        }
        // Choose the authoritative face before resolving any producer. A
        // connected initializer keeps priority even while its type is unknown.
        // Inspecting the inner writer first can pull a feedback cycle (or an
        // invalid calculation) into an otherwise known external type.
        if (!boundary.shiftRegisterPairId.empty() &&
            member->outputFromStructure == hasShiftInitializer) continue;
        // The public resolver also accepts a copied tunnel descriptor. Stable
        // tunnel identity, rather than its C++ address, selects the initializer.
        if(shiftInitializer && member->id!=shiftInitializer->id) continue;
        if (member->outputFromStructure && edge.diagramOwnerStructureId != node.id)
            return {{ResolutionStatus::Incompatible, {}, "binding.boundary_scope", "A structure output must be driven inside its region."}, false, {}};
        if(isSequenceStructure(node)) {
            const auto frame=sequenceTunnelInputFrameId(*member);
            const bool scoped=frame.empty()
                ?edge.diagramOwnerStructureId==node.diagramOwnerStructureId && edge.diagramOwnerRegionId==node.diagramOwnerRegionId
                :edge.diagramOwnerStructureId==node.id && edge.diagramOwnerRegionId==frame;
            if(!scoped)return {{ResolutionStatus::Incompatible,{},"binding.boundary_scope",
                "A Sequence tunnel must be driven in its declared producing frame or parent diagram."},false,{}};
        }
        if (direction(edge.from) != FunctionPortDirection::Output)
            return {{ResolutionStatus::Incompatible, {}, "binding.direction", "A tunnel must be driven by an output."}, false, {}};
        auto source = resolveSource(edge);
        if (!hasShiftInitializer && !boundary.shiftRegisterPairId.empty() &&
            source.resolution.status == ResolutionStatus::Unresolved) {
            const auto producer = nodesById_.find(edge.from.nodeId);
            const auto *entry = producer == nodesById_.end() ? nullptr :
                findFunctionCatalogEntryByDiagramNodeType(producer->second->type);
            const auto *definition = entry
                ? findFunctionBindingDefinition(entry->catalogId) : nullptr;
            const auto *output = definition ? functionPort(*definition, edge.from.portId) : nullptr;
            // Direct temporal feedback can use a declared numeric scalar type
            // from an explicit conversion or a unary contract with that type
            // as a fixed point. The real input/output are still validated;
            // consumers, persisted caches and arbitrary unknown inputs do not
            // provide evidence or replace a missing runtime value.
            if (output && output->direction == FunctionPortDirection::Output &&
                producer->second->diagramOwnerStructureId == node.id) {
                const auto declared = parseType(output->defaultValueType);
                if (declared.valid() && declared.type.kind == TypeKind::Primitive && declared.type.numeric()) {
                    const bool conversion = entry->catalogId.starts_with("numeric.conversion.to_");
                    const auto family = diagramStructureFamilyForNodeType(node.type);
                    const auto producerInputs = incomingEdges(producer->second->id);
                    const auto *unaryInput = !conversion && entry->family == FunctionFamily::Numeric &&
                        (family == DiagramStructureFamily::ForLoop || family == DiagramStructureFamily::WhileLoop) &&
                        producerInputs.size() == 1U && producer->second->diagramOwnerRegionId == "body" &&
                        edge.diagramOwnerRegionId == "body"
                        ? unaryNumericFeedbackInput(*producer->second, *definition, *output, declared.type)
                        : nullptr;
                    const std::string_view feedbackPort = conversion ? std::string_view{"value"}
                        : unaryInput ? std::string_view{unaryInput->portId} : std::string_view{};
                    for (const auto *feedback : incomingEdges(producer->second->id)) {
                        const auto *read = feedback->from.nodeId == node.id ? findTunnel(node, feedback->from.portId) : nullptr;
                        if (feedbackPort.empty() ||
                            canonicalFunctionBindingPortId(entry->catalogId, feedback->to.portId) != feedbackPort ||
                            feedback->diagramOwnerStructureId != node.id || !read || read->outputFromStructure ||
                            read->shiftRegisterPairId != boundary.shiftRegisterPairId ||
                            feedback->from.portId != tunnelOutputId(read->id)) continue;
                        if (!conversion && feedback->diagramOwnerRegionId != "body") continue;
                        source = fromParsed(declared);
                        source.complete = false;
                        break;
                    }
                }
            }
        }
        if (source.resolution.status == ResolutionStatus::Incompatible) return source;
        types.push_back(source.resolution.type);
        allSourcesComplete = allSourcesComplete && source.complete && source.resolution.accepted();
        const auto region = (boundary.shiftRegisterPairId.empty() ? std::string{} : member->id + '\x1f') +
            (member->outputFromStructure
                ? edge.diagramOwnerRegionId : std::string{});
        if (++sourcesByRegion[region] > 1U)
            return {{ResolutionStatus::Incompatible, {}, "binding.multiple_sources", "A region supplies more than one source to the same tunnel."}, false, {}};
    }
    const bool defaultAllowed = diagramTunnelSupportsDefault(node, boundary) && boundary.useDefaultIfUnwired;
    // With no inner producer a declared indicator type can constrain the tunnel.
    // Polymorphic consumer hints and the persisted inferred cache cannot do so.
    if (boundary.shiftRegisterPairId.empty() && types.empty() && defaultAllowed) {
        for (const auto &edge : edges_) {
            if (edge.from.nodeId != node.id || edge.from.portId != tunnelOutputId(boundary.id) ||
                direction(edge.to) != FunctionPortDirection::Input) continue;
            const auto sink = nodesById_.find(edge.to.nodeId);
            if (sink == nodesById_.end() || sink->second->kind != "widget_value") continue;
            auto declared = resolveImpl(*sink->second, edge.to.portId);
            if (!declared.complete || !declared.resolution.accepted()) continue;
            auto type = declared.resolution.type;
            if (boundary.mode == DiagramTunnelMode::Indexing) {
                if (type.kind != TypeKind::Array) continue;
                if (type.rank == 1U) { auto element = type.children.front(); type = std::move(element); }
                else { --type.rank; type.extent.reset(); }
            }
            types.push_back(std::move(type));
        }
    }
    BindingResult result{unify(types), allSourcesComplete, {}};
    if (diagramTunnelSupportsDefault(node, boundary)) {
        const auto regions = diagramOutputTunnelRegionIds(node, boundary);
        for (const auto &[region, count] : sourcesByRegion) {
            static_cast<void>(count);
            if (std::find(regions.begin(), regions.end(), region) == regions.end())
                return {{ResolutionStatus::Incompatible, {}, "binding.unknown_region", "A tunnel source references a missing or different subdiagram."}, false, {}};
        }
        for (const auto &region : regions) {
            if (!sourcesByRegion.contains(region)) result.missingRegions.push_back(region);
        }
        const bool requireEveryRegion = !isCase(node) || caseBindingDefinition().requireEveryRegion;
        const bool allowDefault = defaultAllowed && (!isCase(node) || caseBindingDefinition().allowDefaultIfUnwired);
        result.complete = result.complete && (!requireEveryRegion || result.missingRegions.empty() || allowDefault);
    } else {
        result.complete = result.complete && !sourcesByRegion.empty();
    }
    // Stored boundary type is an inferred cache, never evidence after rewiring.
    result.complete = result.complete && result.resolution.accepted();
    return result;
}
BindingResult TypeResolver::tunnelPort(const DiagramNode &node,
    const DiagramCaseStructureTunnel &boundary, std::string_view port) {
    if (port == diagramTunnelConditionPortId(boundary.id))
        return fromParsed(parseType("bool"));
    auto result = tunnel(node, boundary);
    if (result.resolution.status == ResolutionStatus::Incompatible) return result;
    const auto transformed = diagramTunnelOutputType(node, boundary, result.resolution.type);
    if (transformed.status == ResolutionStatus::Incompatible || port == tunnelOutputId(boundary.id))
        result.resolution = transformed;
    if (!boundary.outputFromStructure && !boundary.shiftRegisterPairId.empty() &&
        port == tunnelOutputId(boundary.id) && result.resolution.accepted() &&
        result.resolution.type.known()) {
        // The first inner read depends on the outer initializer, or on the
        // resolved type's default when that input is unwired. It does not
        // depend on the inner write, which takes effect next iteration.
        const DiagramEdge *initial=nullptr;
        bool multiple=false;
        for(const auto *edge:incomingEdges(node.id)) if(edge->to.portId==tunnelInputId(boundary.id)) {
            if(initial) multiple=true;
            initial=edge;
        }
        if(multiple) result.complete=false;
        else if(!initial) result.complete=true;
        else {
            const auto source=resolveSource(*initial);
            result.complete=source.complete && source.resolution.accepted();
        }
    }
    if (boundary.outputFromStructure && !boundary.shiftRegisterPairId.empty() &&
        port == tunnelOutputId(boundary.id) && result.resolution.accepted()) {
        // The final value still requires a complete inner write. Its failures
        // affect execution readiness, never the type supplied by the initializer.
        const auto writer = input(node, tunnelInputId(boundary.id));
        result.complete = result.complete && writer.complete && writer.resolution.accepted() &&
            connectShiftRegister(writer.resolution.type, result.resolution.type).accepted();
    }
    if (boundary.conditional && port == tunnelOutputId(boundary.id)) {
        const auto condition = input(node, diagramTunnelConditionPortId(boundary.id));
        result.complete = result.complete && condition.complete &&
            condition.resolution.type == TypeRef::primitive("bool");
    }
    result.complete = result.complete && result.resolution.accepted();
    return result;
}

BindingResult TypeResolver::paletteFunction(const DiagramNode &node, std::string_view port) {
    const auto *entry = findFunctionCatalogEntryByDiagramNodeType(node.type);
    if(entry) port=canonicalFunctionBindingPortId(entry->catalogId,port);
    const auto *definition = entry ? findFunctionBindingDefinition(entry->catalogId) : nullptr;
    const auto *binding = definition ? functionPort(*definition, port) : nullptr;
    if (!binding) return fromParsed({{}, "Unknown palette port.", false});
    const auto unresolved = []() { return fromParsed(parseType("unknown")); };
    const auto invalid = [](std::string message) {
        return BindingResult{{ResolutionStatus::Incompatible, {}, "binding.palette_operands", std::move(message)}, false, {}};
    };
    const bool fixedFormula = definition->typeResolution == FunctionTypeResolutionPolicy::Fixed &&
        definition->implementationKind == FunctionImplementationKind::BuiltInProvider &&
        definition->providerRequirement == frog::math::formula::profile &&
        definition->implementationId == "frog.studio." + entry->catalogId &&
        frog::math::formula::parseOperationId(definition->implementationId).has_value();
    const auto source = [&](std::string_view id) {
        auto value = input(node, id);
        const auto *declared = functionPort(*definition, id);
        const auto incoming = incomingEdges(node.id);
        const bool wired = std::any_of(incoming.begin(), incoming.end(),
            [&](const auto *edge) { return canonicalFunctionBindingPortId(entry->catalogId, edge->to.portId) ==
                canonicalFunctionBindingPortId(entry->catalogId, id); });
        if (!wired && declared && (declared->requirement == FunctionPortRequirement::Optional ||
                entry->family==FunctionFamily::Path || entry->family==FunctionFamily::Error ||
                entry->family==FunctionFamily::Timestamp ||
                entry->catalogId.starts_with("string.") ||
                (fixedFormula && declared->connectionPolicy==FunctionPortConnectionPolicy::Exact))) {
            auto defaultType=fromParsed(parseType(declared->defaultValueType));
            // String/Path/Error/Timestamp panes have declared editor representations, including
            // unwired polymorphic conversions. This is type evidence only:
            // a required input is still incomplete, never a runtime value.
            // The closed Formula signatures also expose exact authoring type
            // evidence before their first wire, including both Prepared Records.
            defaultType.complete=declared->requirement==FunctionPortRequirement::Optional;
            return defaultType;
        }
        if (!value.resolution.accepted()) return value;
        if (declared) {
            const auto expr = parseTypeExpr(declared->typeExpression);
            if (!expr.valid()) return invalid(expr.error);
            value.resolution = constrain(value.resolution.type, expr.expression);
            if (value.resolution.accepted())
                if (const auto target = fixedStringNumericParameterType(entry->catalogId, *declared))
                    value.resolution = connect(value.resolution.type, *target, ConnectionPolicy::NumericImplicit);
            value.complete = value.complete && value.resolution.accepted();
        }
        if (value.resolution.accepted()) {
            const auto &t = value.resolution.type;
            if (entry->catalogId.starts_with("array.matrix.") && (id == "row_indices" || id == "column_indices")) {
                const auto &leaf = t.kind == TypeKind::Array ? t.children.front() : t;
                if (!leaf.numeric() || leaf.name.starts_with("c") ||
                    (t.kind == TypeKind::Array && t.rank != 1))
                    return invalid("Matrix indices must be real scalars or one-dimensional real arrays.");
            }
            if (entry->catalogId.starts_with("array.conversion.") &&
                (id == "array_type" || (id == "array" && entry->catalogId.ends_with("array_to_spreadsheet_string")))) {
                if (t.kind != TypeKind::Array || !(t.children.front().numeric() || t.children.front().name == "string"))
                    return invalid("Spreadsheet arrays contain numeric or String elements.");
            }
        }
        return value;
    };
    if (definition->resolverId == kStudioPolymorphicResolver) {
        std::vector<std::string> wired;
        std::unordered_map<std::string, BindingResult> boundInputs;
        for (const auto *edge : incomingEdges(node.id)) {
            wired.emplace_back(canonicalFunctionBindingPortId(entry->catalogId,edge->to.portId));
            // Gather through the normal input contract. Unevaluated producers
            // suspend this endpoint in the iterative driver; the polymorphic
            // dispatcher is never retained across graph dependencies.
            if (!boundInputs.contains(wired.back()))
                boundInputs.emplace(wired.back(), source(wired.back()));
        }
        const FunctionInputResolver preparedInput = [&](std::string_view id) {
            const auto canonical = canonicalFunctionBindingPortId(entry->catalogId, id);
            if (const auto found = boundInputs.find(std::string(canonical)); found != boundInputs.end())
                return found->second;
            return source(canonical); // An unwired declared/default input.
        };
        std::optional<std::string_view> formatLiteral;
        unsigned formatSourceCount=0;
        const bool commonFormat=entry->catalogId.ends_with("array_to_spreadsheet_string") || entry->catalogId.ends_with("spreadsheet_string_to_array");
        const std::string_view formatPort=commonFormat?"format":"format_string";
        if(isStringScan(node.type) || isStringFormat(node.type) || entry->catalogId=="string.conversion.format_value" || commonFormat) for(const auto *edge:incomingEdges(node.id))
            if(canonicalFunctionBindingPortId(entry->catalogId,edge->to.portId)==formatPort) {
                if(++formatSourceCount>1) return invalid("A format input has more than one source.");
                if(direction(edge->from)!=FunctionPortDirection::Output)
                    return invalid("A format input requires an existing output.");
                const auto it=nodesById_.find(edge->from.nodeId);
                if(it!=nodesById_.end()) formatLiteral=stringConstantLiteral(it->second);
            }
        return resolvePolymorphicFunction(node, *definition, port, preparedInput, wired, formatLiteral);
    }
    if (binding->direction == FunctionPortDirection::Input) return source(port);
    const auto expr = parseTypeExpr(binding->typeExpression);
    if (expr.valid() && expr.expression.kind == ConstraintKind::SameAs)
        return source(expr.expression.variable);
    if (definition->resolverId.empty()) return fromParsed(parseType(binding->defaultValueType));
    std::string_view id = entry->catalogId;
    if (id == "array.conversion.array_to_cluster") id = "cluster.conversion.array_to_cluster";
    if (id == "array.conversion.cluster_to_array") id = "cluster.conversion.cluster_to_array";
    if (id.starts_with("variant."))
        return fromParsed(parseType(binding->defaultValueType));
    if (id.starts_with("array.matrix.") && id != "array.matrix.build") {
        auto value = source("matrix");
        if (!value.resolution.accepted()) return value;
        if (value.resolution.type.kind != TypeKind::Array || value.resolution.type.rank != 2U)
            return invalid("Matrix operations require rank two.");
        if (id == "array.matrix.get_elements") {
            auto rows = source("row_indices"), columns = source("column_indices");
            if (rows.resolution.status == ResolutionStatus::Incompatible) return rows;
            if (columns.resolution.status == ResolutionStatus::Incompatible) return columns;
            if (!rows.resolution.accepted() || !columns.resolution.accepted()) return unresolved();
            if (rows.resolution.type.kind != TypeKind::Array && columns.resolution.type.kind != TypeKind::Array) {
                auto scalar = value.resolution.type.children.front();
                value.resolution.type = std::move(scalar);
            }
        }
        if (id.starts_with("array.matrix.set_")) {
            for (const auto *edge : incomingEdges(node.id)) {
                auto candidate = source(edge->to.portId);
                if (candidate.resolution.status == ResolutionStatus::Incompatible) return candidate;
                if (edge->to.portId == "elements" && candidate.resolution.accepted()) {
                    const auto &t = candidate.resolution.type;
                    if (!t.numeric() && !(t.kind == TypeKind::Array && t.rank == 2 && t.children.front().numeric()))
                        return invalid("Matrix replacement elements must be numeric scalars or matrices.");
                }
            }
        }
        return value;
    }
    if (id == "cluster.properties") {
        auto value = source("cluster");
        if (!value.resolution.accepted()) return value;
        const auto &fields = value.resolution.type.fieldIds;
        const auto found = std::find(fields.begin(), fields.end(), node.text);
        if (found == fields.end()) return unresolved();
        return fromParsed({value.resolution.type.children[static_cast<std::size_t>(found-fields.begin())], {}, false});
    }
    if (id == "cluster.conversion.cluster_to_array") {
        auto value = source("cluster");
        if (!value.resolution.accepted()) return value;
        if (std::any_of(value.resolution.type.children.begin(), value.resolution.type.children.end(),
                [](const auto &t) { return t.kind == TypeKind::Array; }))
            return invalid("Cluster To Array does not accept array fields.");
        auto common = unify(value.resolution.type.children);
        if (!common.accepted()) return BindingResult{common, false, {}};
        common.type = TypeRef::array(common.type);
        return {common, value.complete, {}};
    }
    if (id == "cluster.conversion.array_to_cluster") {
        // A cluster has fixed authoring arity, independent of runtime array size.
        // Nine is the documented default; explicit configuration is bounded.
        auto value = source("array");
        if (!value.resolution.accepted()) return value;
        const auto &array = value.resolution.type;
        if (array.rank != 1U) return invalid("Array To Cluster requires a one-dimensional array.");
        std::size_t arity = 9U;
        if (!node.text.empty()) {
            const auto parsed = std::from_chars(node.text.data(), node.text.data()+node.text.size(), arity);
            if (parsed.ec != std::errc{} || parsed.ptr != node.text.data()+node.text.size() ||
                arity == 0U || arity > 256U) return invalid("Cluster size must be between 1 and 256.");
        }
        std::vector<std::string> names;
        std::vector<TypeRef> fields;
        for (std::size_t i=0; i<arity; ++i) {
            names.push_back("element"+std::to_string(i+1));
            fields.push_back(array.children.front());
        }
        return fromParsed({TypeRef::cluster(std::move(names), std::move(fields)), {}, false});
    }
    std::map<std::size_t, TypeRef> operands;
    bool complete = true;
    for (const auto *edge : incomingEdges(node.id)) {
        const auto ordinal = functionBindingInputPortOrdinal(*definition, edge->to.portId);
        if (!ordinal) continue;
        auto value = source(edge->to.portId);
        if (!value.resolution.accepted()) return value;
        if (!operands.emplace(*ordinal, value.resolution.type).second) return invalid("Duplicate input source.");
        complete = complete && value.complete;
    }
    // Missing fixed inputs cannot silently shrink a cluster schema.
    for (std::size_t i=0; i<definition->minimumInputCount; ++i)
        if (!operands.contains(i)) return unresolved();
    if (operands.empty()) return unresolved();
    std::vector<TypeRef> fields;
    std::vector<std::string> names;
    std::size_t expected = 0U;
    for (const auto &[ordinal, operand] : operands) {
        if (ordinal != expected++) return unresolved();
        names.push_back("element"+std::to_string(ordinal+1));
        if (id == "cluster.index_build_array") {
            if (operand.kind != TypeKind::Array || operand.rank != 1U)
                return invalid("Index & Build Cluster Array requires one-dimensional arrays.");
            fields.push_back(operand.children.front());
        } else fields.push_back(operand);
    }
    TypeRef result;
    if (id == "array.matrix.build") {
        std::vector<TypeRef> leaves;
        for (const auto &field : fields) {
            if (field.numeric()) leaves.push_back(field);
            else if (field.kind == TypeKind::Array && field.rank <= 2 && field.children.front().numeric())
                leaves.push_back(field.children.front());
            else return invalid("Build Matrix requires numeric scalars, rows or matrices.");
        }
        const auto common = promote(leaves);
        if (!common.accepted()) return {common, false, {}};
        result = TypeRef::array(common.type, 2U);
    } else if (id == "cluster.build_array") {
        const auto common = unify(fields);
        if (!common.accepted()) return {common, false, {}};
        result = TypeRef::array(TypeRef::cluster({"element1"}, {common.type}));
    } else {
        result = TypeRef::cluster(std::move(names), std::move(fields));
        if (id == "cluster.index_build_array") result = TypeRef::array(std::move(result));
    }
    return {{ResolutionStatus::Compatible, std::move(result), {}, {}}, complete, {}};
}

BindingResult TypeResolver::resolveImpl(const DiagramNode &node, std::string_view port) {
    if(node.type=="frog.studio.structures.feedback_node")return feedbackPort(node,port);
    if(isCollectionConstant(node)||isCollectionFunction(node.type))return resolveCollectionPort(node,port,[&](std::string_view inputPort){return input(node,inputPort);});
    if(isEventFunction(node.type)) {
        std::vector<EventTypeEvidence> evidence;bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete && source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveEventFunctionPort(node,port,evidence);
        return {std::move(result.resolution),result.complete && upstreamComplete,{}};
    }
    if (isNotifierFunction(node.type)) {
        std::vector<NotifierTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveNotifierFunctionPort(node,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if (isSemaphoreFunction(node.type)) {
        std::vector<SemaphoreTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveSemaphoreFunctionPort(node,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if (isRendezvousFunction(node.type)) {
        std::vector<RendezvousTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveRendezvousFunctionPort(node,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if (isOccurrenceFunction(node.type)) {
        std::vector<OccurrenceTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveOccurrenceFunctionPort(node,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if (isQueueFunction(node.type)) {
        std::vector<QueueTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete && source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveQueueFunctionPort(node,port,evidence);
        return {std::move(result.resolution),result.complete && upstreamComplete,{}};
    }
    if (isFileFunction(node.type)) {
        std::vector<FileTypeEvidence> evidence;
        bool upstreamComplete = true;
        for (const auto *edge : incomingEdges(node.id)) {
            const auto source = resolveSource(*edge);
            if (!source.resolution.accepted()) return source;
            upstreamComplete = upstreamComplete && source.complete;
            evidence.push_back({edge->to.portId, source.resolution.type});
        }
        auto result = resolveFilePort(node, port, evidence);
        return {std::move(result.resolution), result.complete && upstreamComplete, {}};
    }
    if(isPartialDifferentialFunction(node.type)) {
        std::vector<PartialDifferentialTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolvePartialDifferentialPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if(isOrdinaryDifferentialFunction(node.type)) {
        const auto *entry=findFunctionCatalogEntryByDiagramNodeType(node.type);
        if(entry)port=canonicalFunctionBindingPortId(entry->catalogId,port);
        const auto *definition=entry?findFunctionBindingDefinition(entry->catalogId):nullptr;
        const auto *binding=definition?functionPort(*definition,port):nullptr;
        std::vector<OrdinaryDifferentialTypeEvidence> evidence;
        bool upstreamComplete=true;
        const bool individualInput=binding&&binding->direction==FunctionPortDirection::Input;
        for(const auto *edge:incomingEdges(node.id)) {
            // Fixed ODE input schemas do not depend on the other inputs. Keep
            // their diagnostics local; resolving an output still checks all inputs.
            if(individualInput&&canonicalFunctionBindingPortId(entry->catalogId,edge->to.portId)!=port)continue;
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveOrdinaryDifferentialPort(node.type,port,evidence);
        const bool complete=individualInput
            ? result.resolution.accepted()&&upstreamComplete&&
                (binding->requirement!=FunctionPortRequirement::Required||!evidence.empty())
            : result.complete&&upstreamComplete;
        return {std::move(result.resolution),complete,{}};
    }
    if(isEllipticParabolicFunction(node.type)) {
        std::vector<EllipticParabolicTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveEllipticParabolicPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if(isErrorFunctionsFunction(node.type)) {
        std::vector<ErrorFunctionsTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveErrorFunctionsPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if(isExponentialIntegralsFunction(node.type)) {
        std::vector<ExponentialIntegralsTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveExponentialIntegralsPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if(isEllipticFunction(node.type)) {
        std::vector<EllipticTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveEllipticPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if(isHypergeometricFunction(node.type)) {
        std::vector<HypergeometricTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveHypergeometricPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if(isGammaFunction(node.type)) {
        std::vector<GammaTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveGammaPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if(isBesselFunction(node.type)) {
        std::vector<BesselTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveBesselPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if(isDiscreteMathFunction(node.type)) {
        std::vector<DiscreteMathTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete&&source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveDiscreteMathPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete&&upstreamComplete,{}};
    }
    if(isGatingFunction(node.type)) {
        std::vector<GatingTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete && source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveGatingPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete && upstreamComplete,{}};
    }
    if(isTrigonometricFunction(node.type)) {
        std::vector<TrigonometricTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete && source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveTrigonometricPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete && upstreamComplete,{}};
    }
    if(isHyperbolicFunction(node.type)) {
        std::vector<HyperbolicTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete && source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveHyperbolicPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete && upstreamComplete,{}};
    }
    if(isExponentialFunction(node.type)) {
        std::vector<ExponentialTypeEvidence> evidence;
        bool upstreamComplete=true;
        for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            if(!source.resolution.accepted())return source;
            upstreamComplete=upstreamComplete && source.complete;
            evidence.push_back({edge->to.portId,source.resolution.type});
        }
        auto result=resolveExponentialPort(node.type,port,evidence);
        return {std::move(result.resolution),result.complete && upstreamComplete,{}};
    }
    if (findConnectivityFunction(node.type)) {
        std::vector<ConnectivityTypeEvidence> evidence;
        bool upstreamComplete=true;
        if(node.connectivity || node.nativeCall)for(const auto *edge:incomingEdges(node.id)) {
            const auto source=resolveSource(*edge);
            upstreamComplete=upstreamComplete && source.complete;
            // A wired but unresolved source is not the unwired connector's
            // default schema (notably the scalar Database Variant instance).
            evidence.push_back({edge->to.portId,source.resolution.accepted()?source.resolution.type:TypeRef{}});
        }
        for(const auto &p:connectivityNodePorts(node,evidence))if(p.id==port) {
            if(!p.valueType.known())return {{ResolutionStatus::Unresolved,{},"binding.connectivity_unresolved",p.unresolvedReason},false,{}};
            return {{ResolutionStatus::Compatible,p.valueType,{},{}},upstreamComplete,{}};
        }
        return {{ResolutionStatus::Incompatible,{},"binding.connectivity_missing_port","The configured Connectivity node has no terminal '"+std::string(port)+"'."},false,{}};
    }
    if (node.applicationControl) {
        auto configuration = *node.applicationControl;
        TypeRef wired;
        bool upstreamComplete=true;
        const bool attached=configuration.kind != ApplicationControlKind::Reference &&
            std::any_of(incomingEdges(node.id).begin(), incomingEdges(node.id).end(),
                [](const auto *edge) { return edge->to.portId=="reference_in"; });
        if (attached) {
            auto source=input(node,"reference_in");
            if (!source.resolution.accepted()) return source;
            upstreamComplete=source.complete;
            wired=std::move(source.resolution.type);
            if (wired.known() && wired.kind!=TypeKind::Reference)
                return {{ResolutionStatus::Incompatible, {}, "binding.reference_required", "This operation requires an object reference, not an ordinary value."}, false, {}};
        } else if (configuration.target.kind == ApplicationControlTargetKind::Widget &&
                configuration.target.ownerDocumentId.empty()) {
            // Both constants and implicit operations obtain the live schema
            // from source identity. An explicit reference wire has priority.
            const auto objects=buildFrogObjectReferenceSchema(canvas_);
            if (!objects.accepted())
                return {{ResolutionStatus::Incompatible, {}, "binding.reference_identity", "Reference targets have ambiguous identities."}, false, {}};
            const auto *record=objects.find(ObjectReferenceKind::Widget,configuration.target.widgetId);
            if (!record)
                return {{ResolutionStatus::Incompatible, {}, "binding.reference_target_missing", "The reference target was removed."}, false, {}};
            configuration.expectedClass=record->classId;
            configuration.valueType=record->valueType;
        }
        for (const auto &description:applicationControlPorts(configuration,wired,
            applicationControlReferencePresentation(node,nodes_,edges_,canvas_))) {
            if (description.id!=port) continue;
            auto result=isEventRegistrationReference(description.type) && isEventRegistrationCollection(description.type)
                ? BindingResult{{ResolutionStatus::Compatible,description.type,{},{}},true,{}}
                : fromParsed({description.type,{},false});
            result.complete=upstreamComplete;
            // Type validity is separate from supplying required runtime data.
            if (description.direction==ApplicationControlPortDirection::Input && description.required)
                result.complete=upstreamComplete && std::any_of(incomingEdges(node.id).begin(),incomingEdges(node.id).end(),
                    [&](const auto *edge) { return edge->to.portId==port; });
            return result;
        }
        return {{ResolutionStatus::Incompatible, {}, "binding.application_control_missing_port", "The configured operation has no such terminal."}, false, {}};
    }
    if(isInPlaceStructure(node) && port.starts_with("ipe:"))
        return resolveInPlacePort(node,port,[&](std::string_view p){return input(node,p);});
    if (isDiagramClusterConstant(node)) {
        if (port!="value") return {{ResolutionStatus::Incompatible, {}, "binding.missing_port", "Cluster constant has one value output."}, false, {}};
        std::vector<const DiagramNode *> members;
        for (const auto &[id,field]:nodesById_) if(field->constantOwnerNodeId==node.id) members.push_back(field);
        std::sort(members.begin(),members.end(),[](const auto *a,const auto *b){return a->constantFieldOrder<b->constantFieldOrder;});
        std::vector<std::string> ids, names; std::vector<TypeRef> types;
        bool complete=!members.empty();
        std::optional<std::uint32_t> previousOrder;
        for(const auto *field:members) {
            if(!isDiagramLiteralConstant(*field))
                return {{ResolutionStatus::Incompatible, {}, "binding.literal_field_not_constant", "Only constant data may belong to a cluster literal."},false,{}};
            if(field->constantFieldId.empty() || std::find(ids.begin(),ids.end(),field->constantFieldId)!=ids.end() ||
                (previousOrder && *previousOrder==field->constantFieldOrder))
                return {{ResolutionStatus::Incompatible, {}, "binding.literal_field_identity", "Cluster field IDs and order must be unique."},false,{}};
            previousOrder=field->constantFieldOrder;
            auto value=resolveRequest({field->id,std::string(diagramLiteralValuePortId(*field))},true);
            if(value.resolution.status==ResolutionStatus::Incompatible) return value;
            complete &= value.complete;
            ids.push_back(field->constantFieldId); types.push_back(std::move(value.resolution.type));
            names.push_back(field->label);
        }
        auto clusterType=TypeRef::cluster(std::move(ids),std::move(types));
        clusterType.fieldNames=std::move(names);
        return {{complete ? ResolutionStatus::Compatible : ResolutionStatus::Unresolved,
            std::move(clusterType),
            complete ? "" : "binding.cluster_constant_incomplete",
            complete ? "" : "Cluster constant contains an undefined or empty literal."},complete,{}};
    }
    if (node.kind == "conditional_default" && port == "value") return fromParsed(parseType(node.valueType));
    if (node.kind == "widget_value") {
        if (port != "value") return {{ResolutionStatus::Incompatible, {}, "binding.missing_port", "Widget has no such value port."}, false, {}};
        const auto *object = canvas_.objectById(node.widgetId);
        return object != nullptr && object->widget.has_value() ? fromParsed(widgetType(*object->widget)) :
            BindingResult{{ResolutionStatus::Incompatible, {}, "binding.missing_widget", "Widget instance does not exist."}, false, {}};
    }
    if (isCase(node)) {
        const auto &contract = caseBindingDefinition();
        if (port == contract.selectorOuterPort || port == contract.selectorInnerPort) {
            const auto incoming = incomingEdges(node.id);
            const bool connected = std::any_of(incoming.begin(), incoming.end(), [](const auto *edge) {
                return edge->to.portId == "selector";
            });
            auto result = connected ? input(node, "selector") : fromParsed(parseType(
                node.caseStructureSelectorValueType.empty() ? "bool" : node.caseStructureSelectorValueType));
            if (result.resolution.status == ResolutionStatus::Incompatible) return result;
            if (result.resolution.type.known())
                result.resolution = constrain(result.resolution.type, contract.selectorConstraint);
            result.complete = result.complete && result.resolution.accepted();
            return result;
        }
        if (const auto *boundary = findTunnel(node, port)) return tunnelPort(node, *boundary, port);
        return {{ResolutionStatus::Incompatible, {}, "binding.missing_port", "Case has no such port."}, false, {}};
    }
    if(node.type=="frog.studio.structures.event_structure" &&
        (port=="event_registration" || port=="event_registration_out")) {
        const auto incoming=input(node,"event_registration");
        if(incoming.resolution.status==ResolutionStatus::Incompatible)return incoming;
        if(!incoming.resolution.type.known())return {{ResolutionStatus::Compatible,
            eventRegistrationReferenceType(TypeRef::cluster({},{})),{},{}},true,{}};
        if(!isEventRegistrationCollection(incoming.resolution.type))return {{ResolutionStatus::Incompatible,{},
            "binding.event_registration_required","Event dynamic terminals require an EventRegistration or Cluster of registrations."},false,{}};
        return incoming;
    }
    if (const auto shelf=structureShelfPort(node,port)) return fromParsed(parseType(shelf->type));
    if (node.type == "frog.studio.structures.for_loop" && port == "count") {
        const auto incoming=incomingEdges(node.id);
        if (std::none_of(incoming.begin(),incoming.end(),[](const auto *edge){return edge->to.portId=="count";}))
            return fromParsed(parseType("i64")); // Existing unwired/indexed default.
        auto result=input(node,port);
        if (!result.resolution.accepted() || !result.resolution.type.known()) return result;
        const auto &type=result.resolution.type;
        if (!type.integer())
            return {{ResolutionStatus::Incompatible,{},"binding.for_count_type",
                "For count requires an exact primitive integer input; no floating conversion is inferred."},false,{}};
        // Only this intrinsic input preserves its producer's integer width.
        // Runtime admission controls supported widths; count_value/iteration remain i64.
        return result;
    }
    if (node.type == "frog.studio.structures.for_loop" && port == "condition") {
        return node.forLoopConditionalTerminal ? fromParsed(parseType("bool")) : fromParsed({{}, {}, false});
    }
    // Ordinary structure tunnels transport the full source schema, just like
    // Case tunnels. They are dynamic ports, not the structure's numeric/Boolean
    // intrinsic bindings (N, i, condition, timeout, ...).
    if (const auto *boundary = findTunnel(node, port)) return tunnelPort(node, *boundary, port);
    if (node.kind == "frog_call") {
        for (const auto &callPort : node.callPorts) if (callPort.id == port) return fromParsed(parseType(callPort.valueType));
    }
    // Constants carry a concrete instance type; legacy syntax is normalized here.
    if (port == "value" && (node.type.find("constant") != std::string::npos ||
        node.type == "bool" || node.type == "string" || node.type == "numeric")) {
        std::string valueType = node.valueType;
        const bool arrayPreset = node.type.starts_with("frog.studio.array.other_constants.");
        if (!arrayPreset && (node.type.find("boolean") != std::string::npos || node.type == "bool")) valueType = "bool";
        else if (!arrayPreset && (node.type.find("string") != std::string::npos || node.type == "string")) valueType = "string";
        else if (!arrayPreset && node.type.find("path") != std::string::npos) valueType = "path";
        if (valueType.empty()) {
            const auto *entry = findFunctionCatalogEntryByDiagramNodeType(node.type);
            const auto *definition = entry == nullptr ? nullptr : findFunctionBindingDefinition(entry->catalogId);
            const auto *declared = definition == nullptr ? nullptr : findFunctionBindingPort(*definition, port);
            valueType = declared == nullptr ? "unknown" : declared->defaultValueType;
        }
        auto parsed = parseType(valueType);
        if (node.type.find("enum") != std::string::npos && parsed.valid() && parsed.type.kind != TypeKind::Enum)
            parsed.type = TypeRef::enumeration(node.enumDomainId.empty() ? "frog.studio.constant." + node.id : node.enumDomainId, parsed.type);
        if (node.type.find("array") != std::string::npos && parsed.valid() && parsed.type.kind != TypeKind::Array)
            parsed.type = TypeRef::array(std::move(parsed.type), node.numericArrayRank);
        if(parsed.valid() && (!node.enumItems.empty() || node.type.find("enum")!=std::string::npos)) {
            auto *choiceType=&parsed.type;
            while(choiceType->kind==TypeKind::Array && !choiceType->children.empty())
                choiceType=&choiceType->children.front();
            choiceType->choiceKind = choiceType->kind==TypeKind::Enum ? ChoiceKind::Enum
                : choiceType->kind==TypeKind::Primitive && choiceType->name=="string"
                    ? ChoiceKind::Dropdown : ChoiceKind::Ring;
            for(const auto &item:node.enumItems)
                choiceType->choices.push_back({item.id,item.label,item.value,item.disabled,item.visible});
        }
        if(parsed.valid() && parsed.type.kind==TypeKind::Array && node.arrayElementTemplate) {
            const auto prototype=widgetType(*node.arrayElementTemplate);
            if(!prototype.valid() || !prototype.type.known())
                return {{ResolutionStatus::Incompatible,{},"binding.literal_prototype_invalid",
                    "The Array constant prototype must have a complete valid element schema."},false,{}};
            auto actual=TypeRef::array(prototype.type,node.numericArrayRank);
            if(!literalPrototypeMatchesDeclaration(parsed.type,actual))
                return {{ResolutionStatus::Incompatible,{},"binding.literal_prototype_mismatch",
                    "The Array constant prototype contradicts its declared field identities, rank or value domains."},false,{}};
            parsed.type=std::move(actual);
        }
        return fromParsed(parsed);
    }
    const auto *paletteEntry = findFunctionCatalogEntryByDiagramNodeType(node.type);
    const auto *paletteContract = paletteEntry ? findFunctionBindingContract(paletteEntry->catalogId) : nullptr;
    if (paletteContract && (paletteContract->profileId.starts_with("frog.studio.palette.") ||
        paletteContract->profileId.starts_with("frog.studio.polymorphic.")))
        return paletteFunction(node, port);
    if (leaf_) return fromParsed(leaf_(node, port));
    const auto *entry = findFunctionCatalogEntryByDiagramNodeType(node.type);
    const auto *definition = entry == nullptr ? nullptr : findFunctionBindingDefinition(entry->catalogId);
    const auto *binding = definition == nullptr ? nullptr : functionPort(*definition, port);
    if (binding == nullptr) return fromParsed({{}, {}, false});
    if (definition->typeResolution == FunctionTypeResolutionPolicy::PassThrough) {
        const auto first = std::find_if(definition->ports.begin(), definition->ports.end(),
            [](const auto &item) { return item.direction == FunctionPortDirection::Input; });
        if (first != definition->ports.end()) return input(node, first->portId);
    }
    if (definition->typeResolution == FunctionTypeResolutionPolicy::NumericPromotion ||
        definition->resolverId == "frog.core.add.type-resolution@1") {
        std::map<std::size_t, TypeRef> orderedOperands;
        std::vector<TypeRef> operands;
        bool complete = true;
        for (const auto *incoming : incomingEdges(node.id)) {
            const auto &edge = *incoming;
            const auto *operand = functionPort(*definition, edge.to.portId);
            if (operand == nullptr || operand->direction != FunctionPortDirection::Input) continue;
            if (direction(edge.from) != FunctionPortDirection::Output)
                return {{ResolutionStatus::Incompatible, {}, "binding.direction",
                    "An input must be driven by an output."}, false, {}};
            auto result = resolveSource(edge);
            if (result.resolution.status == ResolutionStatus::Incompatible) return result;
            if (result.resolution.accepted() && !functionPortAcceptsValueType(*operand, result.resolution.type.canonical()))
                return {{ResolutionStatus::Incompatible, {}, "binding.port_constraint", "The operand does not satisfy the numeric input contract."}, false, {}};
            const auto ordinal = functionBindingInputPortOrdinal(*definition, edge.to.portId);
            if (!ordinal || !orderedOperands.emplace(*ordinal, result.resolution.type).second)
                return {{ResolutionStatus::Incompatible, {}, "binding.multiple_sources", "A function operand has more than one source."}, false, {}};
            complete = complete && result.complete && result.resolution.accepted();
        }
        operands.reserve(orderedOperands.size());
        for (const auto &[ordinal, type] : orderedOperands) { static_cast<void>(ordinal); operands.push_back(type); }
        if (orderedOperands.empty() && incomingEdges(node.id).empty()) {
            // A declared authoring default is not a supplied runtime operand.
            auto value=fromParsed(parseType(binding->defaultValueType));
            value.complete=false;
            return value;
        }
        const auto promoted=promote(operands);
        if (promoted.status==ResolutionStatus::Incompatible) return {promoted,false,{}};
        if (binding->direction == FunctionPortDirection::Input) {
            const auto connected = input(node, port);
            if (connected.resolution.status != ResolutionStatus::Unresolved) return connected;
        }
        return {promoted, complete && promoted.accepted(), {}};
    }
    if (definition->typeResolution == FunctionTypeResolutionPolicy::CustomResolver)
        return {{ResolutionStatus::Unresolved, {}, "binding.resolver_unavailable", "This function needs its registered type resolver."}, false, {}};
    return fromParsed(parseType(binding->defaultValueType));
}
std::optional<FunctionPortDirection> TypeResolver::direction(const DiagramEdgeEndpoint &endpoint) {
    const auto found = nodesById_.find(endpoint.nodeId);
    if (found == nodesById_.end()) return std::nullopt;
    const auto &node = *found->second;
    if(!node.constantOwnerNodeId.empty())return std::nullopt;
    if(isCollectionConstant(node)) {
        const auto kind=collectionFunctionId(node.type)=="collection.map.constant"?"map":"set";
        if(endpoint.portId==kind||endpoint.portId=="value")return FunctionPortDirection::Output;return std::nullopt;
    }
    if(isCollectionFunction(node.type)) {
        const auto ports=collectionPorts(node,{});const auto p=std::find_if(ports.begin(),ports.end(),[&](const auto &p){return p.id==endpoint.portId;});
        return p==ports.end()?std::nullopt:std::optional{p->direction};
    }
    if(isEllipticParabolicFunction(node.type)) {
        const auto inputs=ellipticParabolicInputPortIds(node.type),outputs=ellipticParabolicOutputPortIds(node.type);
        if(std::find(inputs.begin(),inputs.end(),endpoint.portId)!=inputs.end())return FunctionPortDirection::Input;
        if(std::find(outputs.begin(),outputs.end(),endpoint.portId)!=outputs.end())return FunctionPortDirection::Output;
        return std::nullopt;
    }

    if(isErrorFunctionsFunction(node.type)) {
        const auto inputs=errorFunctionsInputPortIds(node.type),outputs=errorFunctionsOutputPortIds(node.type);
        if(std::find(inputs.begin(),inputs.end(),endpoint.portId)!=inputs.end())return FunctionPortDirection::Input;
        if(std::find(outputs.begin(),outputs.end(),endpoint.portId)!=outputs.end())return FunctionPortDirection::Output;
        return std::nullopt;
    }
    if(isExponentialIntegralsFunction(node.type)) {
        const auto inputs=exponentialIntegralsInputPortIds(node.type),outputs=exponentialIntegralsOutputPortIds(node.type);
        if(std::find(inputs.begin(),inputs.end(),endpoint.portId)!=inputs.end())return FunctionPortDirection::Input;
        if(std::find(outputs.begin(),outputs.end(),endpoint.portId)!=outputs.end())return FunctionPortDirection::Output;
        return std::nullopt;
    }
    if(isEllipticFunction(node.type)) {
        const auto inputs=ellipticInputPortIds(node.type),outputs=ellipticOutputPortIds(node.type);
        if(std::find(inputs.begin(),inputs.end(),endpoint.portId)!=inputs.end())return FunctionPortDirection::Input;
        if(std::find(outputs.begin(),outputs.end(),endpoint.portId)!=outputs.end())return FunctionPortDirection::Output;
        return std::nullopt;
    }
    if(isHypergeometricFunction(node.type)) {
        const auto inputs=hypergeometricInputPortIds(node.type),outputs=hypergeometricOutputPortIds(node.type);
        if(std::find(inputs.begin(),inputs.end(),endpoint.portId)!=inputs.end())return FunctionPortDirection::Input;
        if(std::find(outputs.begin(),outputs.end(),endpoint.portId)!=outputs.end())return FunctionPortDirection::Output;
        return std::nullopt;
    }
    if(isGammaFunction(node.type)) {
        const auto inputs=gammaInputPortIds(node.type),outputs=gammaOutputPortIds(node.type);
        if(std::find(inputs.begin(),inputs.end(),endpoint.portId)!=inputs.end())return FunctionPortDirection::Input;
        if(std::find(outputs.begin(),outputs.end(),endpoint.portId)!=outputs.end())return FunctionPortDirection::Output;
        return std::nullopt;
    }
    if(isBesselFunction(node.type)) {
        const auto inputs=besselInputPortIds(node.type),outputs=besselOutputPortIds(node.type);
        if(std::find(inputs.begin(),inputs.end(),endpoint.portId)!=inputs.end())return FunctionPortDirection::Input;
        if(std::find(outputs.begin(),outputs.end(),endpoint.portId)!=outputs.end())return FunctionPortDirection::Output;
        return std::nullopt;
    }
    if(isDiscreteMathFunction(node.type)) {
        const auto inputs=discreteMathInputPortIds(node.type),outputs=discreteMathOutputPortIds(node.type);
        if(std::find(inputs.begin(),inputs.end(),endpoint.portId)!=inputs.end())return FunctionPortDirection::Input;
        if(std::find(outputs.begin(),outputs.end(),endpoint.portId)!=outputs.end())return FunctionPortDirection::Output;
        return std::nullopt;
    }
    if(isGatingFunction(node.type)) {
        if(endpoint.portId=="result")return FunctionPortDirection::Output;
        if(endpoint.portId=="x")return FunctionPortDirection::Input;
        return std::nullopt;
    }
    if(isTrigonometricFunction(node.type)) {
        const auto inputs=trigonometricInputPortIds(node.type),outputs=trigonometricOutputPortIds(node.type);
        if(std::find(inputs.begin(),inputs.end(),endpoint.portId)!=inputs.end())return FunctionPortDirection::Input;
        if(std::find(outputs.begin(),outputs.end(),endpoint.portId)!=outputs.end())return FunctionPortDirection::Output;
        return std::nullopt;
    }
    if(isHyperbolicFunction(node.type)) {
        if(endpoint.portId=="result")return FunctionPortDirection::Output;
        if(endpoint.portId=="x")return FunctionPortDirection::Input;
        return std::nullopt;
    }
    if(isExponentialFunction(node.type)) {
        if(endpoint.portId=="result")return FunctionPortDirection::Output;
        const auto names=exponentialInputPortIds(node.type);
        if(std::find(names.begin(),names.end(),endpoint.portId)!=names.end())return FunctionPortDirection::Input;
        return std::nullopt;
    }
    if(findConnectivityFunction(node.type)) {
        for(const auto &p:connectivityNodePorts(node))if(p.id==endpoint.portId)
            return p.output?FunctionPortDirection::Output:FunctionPortDirection::Input;
        return std::nullopt;
    }
    if (node.applicationControl) {
        auto configuration=*node.applicationControl;
        // Fixed reference/error directions do not require resolving the
        // reference chain. This also keeps feedback traversal finite.
        if (endpoint.portId=="reference_in" && configuration.kind!=ApplicationControlKind::Reference)
            return FunctionPortDirection::Input;
        if (endpoint.portId=="reference_out" && configuration.kind!=ApplicationControlKind::Close)
            return FunctionPortDirection::Output;
        if (configuration.kind!=ApplicationControlKind::Reference) {
            if (endpoint.portId=="error_in") return FunctionPortDirection::Input;
            if (endpoint.portId=="error_out") return FunctionPortDirection::Output;
        }
        TypeRef wired;
        const bool attached=configuration.kind!=ApplicationControlKind::Reference &&
            std::any_of(incomingEdges(node.id).begin(),incomingEdges(node.id).end(),
                [](const auto *edge){return edge->to.portId=="reference_in";});
        if (attached) {
            const auto source=input(node,"reference_in");
            if (!source.resolution.accepted()) return std::nullopt;
            wired=source.resolution.type;
        } else if (configuration.target.kind==ApplicationControlTargetKind::Widget &&
                configuration.target.ownerDocumentId.empty()) {
            const auto objects=buildFrogObjectReferenceSchema(canvas_);
            if (!objects.accepted()) return std::nullopt;
            const auto *record=objects.find(ObjectReferenceKind::Widget,configuration.target.widgetId);
            if (!record) return std::nullopt;
            configuration.expectedClass=record->classId;
            configuration.valueType=record->valueType;
        }
        for (const auto &port:applicationControlPorts(configuration,wired,
            applicationControlReferencePresentation(node,nodes_,edges_,canvas_)))
            if (port.id==endpoint.portId)
                return port.direction==ApplicationControlPortDirection::Output
                    ? FunctionPortDirection::Output : FunctionPortDirection::Input;
        return std::nullopt;
    }
    if(isEventFunction(node.type)) {
        for(const auto &p:eventFunctionPorts(node))if(p.id==endpoint.portId)
            return p.output?FunctionPortDirection::Output:FunctionPortDirection::Input;
        return std::nullopt;
    }
    if(node.type=="frog.studio.structures.event_structure") {
        if(endpoint.portId=="event_registration")return FunctionPortDirection::Input;
        if(endpoint.portId=="event_registration_out")return FunctionPortDirection::Output;
    }
    if(isClusterFunction(node.type)) {
        for(const auto &p:clusterFunctionPorts(node)) if(p.id==endpoint.portId)
            return p.output?FunctionPortDirection::Output:FunctionPortDirection::Input;
        return std::nullopt;
    }
    if (const auto shelf=structureShelfPort(node,endpoint.portId)) return shelf->direction;
    if(isInPlaceStructure(node) && endpoint.portId.starts_with("ipe:")) return inPlacePortDirection(node,endpoint.portId);
    if (isDiagramClusterConstant(node)) return endpoint.portId=="value" ? std::optional{FunctionPortDirection::Output} : std::nullopt;
    if (isArrayEdit(node)) {
        const auto port = parseArrayEditPort(endpoint.portId);
        if (!port || !arrayEditPortAllowed(arrayEditKind(node.type), *port, 16, arrayEditGroupCount(node))) return {};
        return port->output() ? FunctionPortDirection::Output : FunctionPortDirection::Input;
    }
    if (isExpandableArrayPalette(node)) {
        const auto p=arrayPaletteSemanticPort(node,endpoint.portId);
        return p ? std::optional{p->output?FunctionPortDirection::Output:FunctionPortDirection::Input}:std::nullopt;
    }
    if (isReshapeArray(node)) {
        if (endpoint.portId == "result") return FunctionPortDirection::Output;
        if (endpoint.portId == "array") return FunctionPortDirection::Input;
        const auto dimension = reshapeArrayDimensionIndex(endpoint.portId);
        return dimension && *dimension < reshapeArrayDimensionCount(node) ?
            std::optional{FunctionPortDirection::Input} : std::nullopt;
    }
    if (isBuildArray(node)) {
        if (endpoint.portId == "result") return FunctionPortDirection::Output;
        const auto index = buildArrayInputIndex(endpoint.portId);
        return index && *index < buildArrayInputCount(node) ?
            std::optional{FunctionPortDirection::Input} : std::nullopt;
    }
    if (node.kind == "conditional_default" && endpoint.portId == "value") return FunctionPortDirection::Output;
    if (node.type == "frog.studio.structures.for_loop" && endpoint.portId == "condition")
        return node.forLoopConditionalTerminal ? std::optional{FunctionPortDirection::Input} : std::nullopt;
    if (node.kind == "widget_value" && endpoint.portId == "value") {
        const auto *object = canvas_.objectById(node.widgetId);
        if (object == nullptr || !object->widget) return std::nullopt;
        return object->widget->role == "indicator" ? FunctionPortDirection::Input : FunctionPortDirection::Output;
    }
    if (isCase(node)) {
        if (endpoint.portId == "selector") return FunctionPortDirection::Input;
        if (endpoint.portId == "selected_case") return FunctionPortDirection::Output;
        if (const auto *boundary = findTunnel(node, endpoint.portId))
            return endpoint.portId == tunnelOutputId(boundary->id) ? FunctionPortDirection::Output : FunctionPortDirection::Input;
    }
    if (const auto *boundary = findTunnel(node, endpoint.portId))
        return endpoint.portId == tunnelOutputId(boundary->id) ? FunctionPortDirection::Output : FunctionPortDirection::Input;
    if (node.kind == "frog_call") {
        for (const auto &port : node.callPorts) if (port.id == endpoint.portId)
            return port.input ? FunctionPortDirection::Input : FunctionPortDirection::Output;
    }
    const auto *entry = findFunctionCatalogEntryByDiagramNodeType(node.type);
    const auto *definition = entry == nullptr ? nullptr : findFunctionBindingDefinition(entry->catalogId);
    if (definition != nullptr) if (const auto *port = functionPort(*definition, endpoint.portId)) return port->direction;
    return endpoint.portId == "value" && node.type.find("constant") != std::string::npos ?
        std::optional{FunctionPortDirection::Output} : std::nullopt;
}
TypeResolution TypeResolver::connection(const DiagramEdgeEndpoint &source, const DiagramEdgeEndpoint &target) {
    const auto targetNode=nodesById_.find(target.nodeId);
    if(targetNode!=nodesById_.end() && targetNode->second->type=="frog.studio.structures.event_structure" &&
        target.portId.starts_with("event_response.")) {
        const auto sourceNode=nodesById_.find(source.nodeId);
        return connectionInRegion(source,target,
            sourceNode==nodesById_.end()?std::string_view{}:sourceNode->second->diagramOwnerStructureId,
            sourceNode==nodesById_.end()?std::string_view{}:sourceNode->second->diagramOwnerRegionId);
    }
    return connectionInRegion(source,target,
        targetNode==nodesById_.end()?std::string_view{}:targetNode->second->diagramOwnerStructureId,
        targetNode==nodesById_.end()?std::string_view{}:targetNode->second->diagramOwnerRegionId);
}
TypeResolution TypeResolver::connection(const DiagramEdge &edge) {
    return connectionInRegion(edge.from,edge.to,edge.diagramOwnerStructureId,edge.diagramOwnerRegionId);
}
TypeResolution TypeResolver::connectionInRegion(const DiagramEdgeEndpoint &source,
    const DiagramEdgeEndpoint &target,std::string_view ownerStructureId,std::string_view regionId) {
    if (direction(source) != FunctionPortDirection::Output || direction(target) != FunctionPortDirection::Input)
        return {ResolutionStatus::Incompatible, {}, "binding.direction", "A binding connects an existing output to an existing input."};
    const auto sourceResult = resolveInRegion(source,ownerStructureId,regionId);
    if (sourceResult.resolution.status == ResolutionStatus::Incompatible) return sourceResult.resolution;
    const auto found = nodesById_.find(target.nodeId);
    if(found!=nodesById_.end()&&isCollectionFunction(found->second->type))return checkCollectionConnection(*found->second,target.portId,sourceResult.resolution.type,[&](std::string_view p){return input(*found->second,p);});
    if(found!=nodesById_.end() && found->second->type=="frog.studio.structures.event_structure" &&
        target.portId.starts_with("event_response.")) {
        const auto response=resolveInRegion(target,ownerStructureId,regionId);
        if(!response.resolution.accepted())return response.resolution;
        return connect(sourceResult.resolution.type,response.resolution.type,ConnectionPolicy::Exact);
    }
    if(found!=nodesById_.end() && isInPlaceStructure(*found->second) && target.portId.starts_with("ipe:"))
        return checkInPlaceConnection(*found->second,target.portId,sourceResult.resolution.type,
            [&](std::string_view p){return input(*found->second,p);});
    if (found != nodesById_.end()) {
        if (const auto *boundary = findTunnel(*found->second, target.portId)) {
            if (target.portId == diagramTunnelConditionPortId(boundary->id))
                return connect(sourceResult.resolution.type, TypeRef::primitive("bool"));
            auto transformed = diagramTunnelOutputType(*found->second, *boundary, sourceResult.resolution.type);
            if (transformed.status == ResolutionStatus::Incompatible) return transformed;
            if (!boundary->shiftRegisterPairId.empty() &&
                target.portId == tunnelInputId(boundary->id)) {
                // The candidate replaces the source on this face. An outer
                // initializer may retype the pair; without one, the inner
                // writer may do the same. Other live edges are checked against
                // the adopted type by the document validator.
                if (!boundary->outputFromStructure) {
                    const DiagramCaseStructureTunnel *authority=boundary;
                    const DiagramEdge *initializer=nullptr;
                    for(const auto *edge:incomingEdges(found->second->id)) {
                        const auto *member=findTunnel(*found->second,edge->to.portId);
                        if(!member || member->outputFromStructure || member->shiftRegisterPairId!=boundary->shiftRegisterPairId ||
                            member->id==boundary->id || edge->to.portId!=tunnelInputId(member->id))continue;
                        if(member->shiftRegisterHistoryIndex<authority->shiftRegisterHistoryIndex ||
                            (member->shiftRegisterHistoryIndex==authority->shiftRegisterHistoryIndex && member->id<authority->id)) {
                            authority=member;initializer=edge;
                        }
                    }
                    if(!initializer)return sourceResult.resolution;
                    const auto authoritative=resolveSource(*initializer);
                    if(!authoritative.resolution.accepted())return authoritative.resolution;
                    return connectShiftRegister(sourceResult.resolution.type,authoritative.resolution.type);
                }
                bool hasInitializer = false;
                for (const auto *edge : incomingEdges(found->second->id)) {
                    const auto *member = findTunnel(*found->second, edge->to.portId);
                    if (member != nullptr && !member->outputFromStructure &&
                        member->shiftRegisterPairId == boundary->shiftRegisterPairId &&
                        edge->to.portId == tunnelInputId(member->id)) {
                        hasInitializer = true;
                        break;
                    }
                }
                if (!hasInitializer) return sourceResult.resolution;
                const auto targetResult = resolve(target);
                if (targetResult.resolution.status == ResolutionStatus::Incompatible)
                    return targetResult.resolution;
                return connectShiftRegister(sourceResult.resolution.type, targetResult.resolution.type);
            }
        }
    }
    if(found!=nodesById_.end() && found->second->type=="frog.studio.structures.event_structure" && target.portId=="event_registration") {
        if(!isEventRegistrationCollection(sourceResult.resolution.type))return {ResolutionStatus::Incompatible,{},
            "binding.event_registration_required","Event dynamic terminals require an EventRegistration or Cluster of registrations."};
        return sourceResult.resolution;
    }
    if(found!=nodesById_.end() && isEventFunction(found->second->type)) {
        std::vector<EventTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveEventFunctionPort(*found->second,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isNotifierFunction(found->second->type)) {
        std::vector<NotifierTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveNotifierFunctionPort(*found->second,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isSemaphoreFunction(found->second->type)) {
        std::vector<SemaphoreTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveSemaphoreFunctionPort(*found->second,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isRendezvousFunction(found->second->type)) {
        std::vector<RendezvousTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveRendezvousFunctionPort(*found->second,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isOccurrenceFunction(found->second->type)) {
        std::vector<OccurrenceTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveOccurrenceFunctionPort(*found->second,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isQueueFunction(found->second->type)) {
        std::vector<QueueTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveQueueFunctionPort(*found->second,target.portId,evidence).resolution;
    }
    if (found != nodesById_.end() && isFileFunction(found->second->type)) {
        std::vector<FileTypeEvidence> evidence{{target.portId, sourceResult.resolution.type}};
        for (const auto *edge : incomingEdges(found->second->id)) if (edge->to.portId != target.portId) {
            const auto previous = resolveSource(*edge);
            if (!previous.resolution.accepted()) return previous.resolution;
            evidence.push_back({edge->to.portId, previous.resolution.type});
        }
        return resolveFilePort(*found->second, target.portId, evidence).resolution;
    }
    if(found!=nodesById_.end() && isPartialDifferentialFunction(found->second->type)) {
        std::vector<PartialDifferentialTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolvePartialDifferentialPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isOrdinaryDifferentialFunction(found->second->type)) {
        std::vector<OrdinaryDifferentialTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        return resolveOrdinaryDifferentialPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isEllipticParabolicFunction(found->second->type)) {
        std::vector<EllipticParabolicTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveEllipticParabolicPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isErrorFunctionsFunction(found->second->type)) {
        std::vector<ErrorFunctionsTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveErrorFunctionsPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isExponentialIntegralsFunction(found->second->type)) {
        std::vector<ExponentialIntegralsTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveExponentialIntegralsPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isEllipticFunction(found->second->type)) {
        std::vector<EllipticTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveEllipticPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isHypergeometricFunction(found->second->type)) {
        std::vector<HypergeometricTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveHypergeometricPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isGammaFunction(found->second->type)) {
        std::vector<GammaTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveGammaPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isBesselFunction(found->second->type)) {
        std::vector<BesselTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveBesselPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isDiscreteMathFunction(found->second->type)) {
        std::vector<DiscreteMathTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveDiscreteMathPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isGatingFunction(found->second->type)) {
        std::vector<GatingTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveGatingPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isTrigonometricFunction(found->second->type)) {
        std::vector<TrigonometricTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveTrigonometricPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isHyperbolicFunction(found->second->type)) {
        std::vector<HyperbolicTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveHyperbolicPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && isExponentialFunction(found->second->type)) {
        std::vector<ExponentialTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(found->second->id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            if(!previous.resolution.accepted())return previous.resolution;
            evidence.push_back({edge->to.portId,previous.resolution.type});
        }
        return resolveExponentialPort(found->second->type,target.portId,evidence).resolution;
    }
    if(found!=nodesById_.end() && findConnectivityFunction(found->second->type)) {
        const auto &node=*found->second;
        std::vector<ConnectivityTypeEvidence> evidence{{target.portId,sourceResult.resolution.type}};
        for(const auto *edge:incomingEdges(node.id))if(edge->to.portId!=target.portId) {
            const auto previous=resolveSource(*edge);
            // A connected unresolved source is evidence of an unknown schema,
            // not permission to substitute the unwired connector's default.
            evidence.push_back({edge->to.portId,previous.resolution.accepted()?previous.resolution.type:TypeRef{}});
        }
        if(findConnectivityFunction(node.type)->catalogId=="connectivity.database.variant_to_data" &&
            (target.portId=="type" || target.portId=="database_variant")) {
            const auto conversion=connectivityDatabaseConversion(node,evidence);
            if(!conversion.accepted()) {
                // A valid source shape can be connected before the target
                // specimen is supplied. No result signature is claimed yet.
                if(target.portId=="database_variant" && conversion.code=="binding.database_target_unresolved")
                    return sourceResult.resolution;
                auto failure=conversion;
                failure.message="Node "+node.id+", terminal "+target.portId+": "+failure.message;
                return failure;
            }
        }
        for(const auto &p:connectivityNodePorts(node,evidence))if(p.id==target.portId && !p.output) {
            if(!p.valueType.known())return {ResolutionStatus::Unresolved,{},"binding.connectivity_unresolved",
                "Node "+node.id+", terminal "+p.name+": "+p.unresolvedReason};
            auto result=connect(sourceResult.resolution.type,p.valueType,ConnectionPolicy::Exact);
            if(!result.accepted())result.message="Node "+node.id+", terminal "+p.name+": expected "+
                p.valueType.canonical()+", received "+sourceResult.resolution.type.canonical()+". "+result.message;
            return result;
        }
        return {ResolutionStatus::Incompatible,{},"binding.connectivity_missing_port","Connectivity terminal does not exist or is not an input."};
    }
    if (found != nodesById_.end() && found->second->applicationControl && target.portId=="reference_in") {
        // Connecting the explicit source adapts the operation's effective
        // class. A stale unconnected palette default is not a runtime cast.
        if (sourceResult.resolution.type.kind!=TypeKind::Reference)
            return {ResolutionStatus::Incompatible,{},"binding.reference_required","An object operation requires a reference input."};
        if(!referenceClassAssignable(sourceResult.resolution.type.name,"frog.object") &&
            sourceResult.resolution.type.name!="file")
            return {ResolutionStatus::Incompatible,{},"binding.object_reference_class",
                "Property/Invoke nodes require a supported object or File reference; execution-resource Queue references are a distinct class."};
        return sourceResult.resolution;
    }
    if (found != nodesById_.end() && found->second->type=="frog.studio.structures.for_loop" && target.portId=="count") {
        // Candidate connection admission must use the same integer constraint
        // as count resolution, before the legacy NumericImplicit fallback.
        if (!sourceResult.resolution.type.known()) return sourceResult.resolution;
        if (!sourceResult.resolution.type.integer()) return {ResolutionStatus::Incompatible,{},
            "binding.for_count_type","For count requires an exact primitive integer input; no floating conversion is inferred."};
        return sourceResult.resolution;
    }
    if (found != nodesById_.end() && isCase(*found->second) && target.portId == "selector")
        return constrain(sourceResult.resolution.type, caseBindingDefinition().selectorConstraint);
    if(found!=nodesById_.end()&&found->second->type=="frog.studio.structures.feedback_node"){
        if(target.portId=="enable")return connect(sourceResult.resolution.type,TypeRef::primitive("bool"),ConnectionPolicy::Exact);
        if(target.portId!="value"&&target.portId!="initial_value")return {ResolutionStatus::Incompatible,{},"binding.missing_port","Feedback has no such input."};
        if(target.portId=="initial_value"&&found->second->feedbackHistory&&
            (sourceResult.resolution.type.kind!=TypeKind::Array||sourceResult.resolution.type.rank!=1||sourceResult.resolution.type.children.size()!=1))
            return {ResolutionStatus::Incompatible,{},"binding.feedback_history_type","History requires a rank-one Array of T samples."};
        const auto expected=resolve(target);
        if(expected.resolution.status==ResolutionStatus::Incompatible)return expected.resolution;
        if(expected.resolution.type.known()&&!fullTypeEqual(sourceResult.resolution.type,expected.resolution.type))
            return {ResolutionStatus::Incompatible,{},"binding.feedback_type_conflict","Feedback stores the exact type and metadata without coercion."};
        return sourceResult.resolution;
    }
    if (found != nodesById_.end() && !isCase(*found->second)) {
        const auto *entry = findFunctionCatalogEntryByDiagramNodeType(found->second->type);
        const auto *definition = entry == nullptr ? nullptr : findFunctionBindingDefinition(entry->catalogId);
        const auto *port = definition == nullptr ? nullptr : functionPort(*definition, target.portId);
        if (port != nullptr) {
            if (entry->catalogId.starts_with("array.matrix.") &&
                port->typeExpression == "array<T:numeric,rank>=2>" &&
                sourceResult.resolution.type.known() &&
                (sourceResult.resolution.type.kind != TypeKind::Array ||
                 sourceResult.resolution.type.rank != 2U))
                return {ResolutionStatus::Incompatible, {}, "binding.matrix_rank", "A matrix has exactly two dimensions."};
            if (!sourceResult.resolution.type.known()) return sourceResult.resolution;
            if (!functionPortAcceptsValueType(*port, sourceResult.resolution.type.canonical()))
                return {ResolutionStatus::Incompatible, {}, "binding.port_constraint", "The source does not satisfy the function input contract."};
            if (definition->resolverId == "frog.core.add.type-resolution@1") {
                const auto targetOrdinal=functionBindingInputPortOrdinal(*definition,target.portId);
                if(!targetOrdinal) return {ResolutionStatus::Incompatible,{},"binding.operand","Unknown numeric operand."};
                std::map<std::size_t,std::string> operands{{*targetOrdinal,sourceResult.resolution.type.canonical()}};
                for(const auto *edge:incomingEdges(target.nodeId)) {
                    const auto ordinal=functionBindingInputPortOrdinal(*definition,edge->to.portId);
                    if(!ordinal || *ordinal==*targetOrdinal) continue; // Candidate replaces this input.
                    const auto value=resolveSource(*edge);
                    if(!value.resolution.accepted()) continue;
                    if(!operands.emplace(*ordinal,value.resolution.type.canonical()).second)
                        return {ResolutionStatus::Incompatible,{},"binding.multiple_sources","A numeric operand has more than one source."};
                }
                std::vector<std::string_view> types;
                std::vector<std::size_t> ordinals;
                for(const auto &[ordinal,type]:operands) {ordinals.push_back(ordinal);types.push_back(type);}
                const auto numeric=resolveNumericAddValueTypes(types);
                // An already broken operand must remain repairable: report its
                // error on that operand, not on every other valid authored wire.
                if(numeric.resolved() || (numeric.operandIndex<ordinals.size() && ordinals[numeric.operandIndex]!=*targetOrdinal))
                    return sourceResult.resolution;
                return {ResolutionStatus::Incompatible,{},"binding.numeric_operands","Numeric operands require compatible array shapes and numeric leaves."};
            }
            if (definition->resolverId == kStudioPolymorphicResolver) {
                // Validate the candidate with the OTHER current inputs, without
                // mutating the graph or reusing an inferred output default.
                const auto targetPort=canonicalFunctionBindingPortId(entry->catalogId,target.portId);
                std::vector<std::string> wired{std::string(targetPort)};
                for (const auto *edge : incomingEdges(target.nodeId))
                    if (canonicalFunctionBindingPortId(entry->catalogId,edge->to.portId) != targetPort)
                        wired.emplace_back(canonicalFunctionBindingPortId(entry->catalogId,edge->to.portId));
                std::optional<std::string_view> formatLiteral;
                const bool commonFormat=entry->catalogId.ends_with("array_to_spreadsheet_string") || entry->catalogId.ends_with("spreadsheet_string_to_array");
                if(isStringScan(found->second->type) || isStringFormat(found->second->type) || entry->catalogId=="string.conversion.format_value" || commonFormat) {
                    const std::string_view formatPort=commonFormat?"format":"format_string";
                    const DiagramNode *literalNode=nullptr;
                    if(targetPort==formatPort) {
                        const auto it=nodesById_.find(source.nodeId); if(it!=nodesById_.end()) literalNode=it->second;
                    } else for(const auto *edge:incomingEdges(target.nodeId)) if(canonicalFunctionBindingPortId(entry->catalogId,edge->to.portId)==formatPort) {
                        const auto it=nodesById_.find(edge->from.nodeId); if(it!=nodesById_.end()) literalNode=it->second;
                    }
                    formatLiteral=stringConstantLiteral(literalNode);
                }
                auto candidate = resolvePolymorphicFunction(*found->second, *definition, targetPort,
                    [&](std::string_view id) {
                        if (id == targetPort) return BindingResult(sourceResult);
                          auto value = input(*found->second, id);
                          const auto *declared = functionPort(*definition, id);
                            if(declared && (declared->requirement==FunctionPortRequirement::Optional || entry->family==FunctionFamily::Path ||
                                    entry->catalogId.starts_with("string.")) &&
                                std::find(wired.begin(),wired.end(),id)==wired.end())
                            {
                                auto defaultType=fromParsed(parseType(declared->defaultValueType));
                                defaultType.complete=declared->requirement==FunctionPortRequirement::Optional;
                                return defaultType;
                            }
                        if (value.resolution.accepted() && declared) {
                            const auto constraint = parseTypeExpr(declared->typeExpression);
                            if (constraint.valid()) value.resolution = constrain(value.resolution.type, constraint.expression);
                        }
                        return value;
                    }, wired, formatLiteral);
                if (candidate.resolution.status == ResolutionStatus::Incompatible) return candidate.resolution;
            }
            if (const auto expected = fixedStringNumericParameterType(entry->catalogId, *port))
                return connect(sourceResult.resolution.type, *expected, ConnectionPolicy::NumericImplicit);
            return sourceResult.resolution;
        }
    }
    auto targetResult = resolve(target);
    if (targetResult.resolution.status == ResolutionStatus::Incompatible) return targetResult.resolution;
    // An unwired dynamic tunnel takes its first type from the candidate source.
    if (found != nodesById_.end() &&
        findTunnel(*found->second, target.portId) != nullptr && !targetResult.resolution.type.known())
        return sourceResult.resolution;
    return connect(sourceResult.resolution.type, targetResult.resolution.type,
        found != nodesById_.end() && (isCase(*found->second) || findTunnel(*found->second, target.portId))
            ? ConnectionPolicy::Exact : ConnectionPolicy::NumericImplicit);
}
bool synchronizeClusterFieldSelections(std::vector<DiagramNode> &nodes,
    std::span<const DiagramEdge> edges, FrogWidgetDataView canvas) {
    bool changed=false;
    // A downstream schema may use a port added by an upstream positional node in
    // this pass. Rebuild the resolver so no outside-port result stays cached.
    // One pass per node bounds even reverse-ordered chains and cyclic graphs.
    for(std::size_t pass=0;pass<nodes.size();++pass) {
        TypeResolver positionalResolver(nodes,edges,canvas);
        bool passChanged=false;
        for(auto &node:nodes) {
            const auto id=clusterFunctionId(node.type);
            if(id!="cluster.unbundle" && id!="cluster.bundle") continue;
            // Bundle's unconnected rail constructs a Cluster from its manual
            // rows. Once wired, the actual schema owns its count and types.
            if(id=="cluster.bundle" && std::none_of(edges.begin(),edges.end(),[&](const auto &edge) {
                return edge.to.nodeId==node.id && edge.to.portId=="cluster";
            }))continue;
            const auto schema=positionalResolver.resolve({node.id,"cluster"}).resolution;
            if(schema.accepted() && schema.type.kind==TypeKind::Cluster &&
                !schema.type.children.empty() && schema.type.children.size()<=32) {
                const auto count=static_cast<unsigned>(schema.type.children.size());
                if(node.clusterFunctionTerminals!=count) {node.clusterFunctionTerminals=count;passChanged=true;}
            }
        }
        if(!passChanged) break;
        changed=true;
    }
    TypeResolver resolver(nodes,edges,canvas);
    for(auto &node:nodes) {
        if(isInPlaceStructure(node)) {
            seedInPlaceIdentityCounters(node,edges);
            for(auto &pair:node.inPlacePairs)if(pair.kind=="cluster") {
                const auto schema=resolver.resolve({node.id,inPlacePortId(pair.id,"source")}).resolution;
                if(!schema.accepted() || schema.type.kind!=TypeKind::Cluster)continue;
                for(auto &access:pair.accesses) {
                    if(!access.fieldPath.empty())continue; // Removed IDs stay invalid.
                    for(const auto &field:schema.type.fieldIds) {
                        const bool used=std::any_of(pair.accesses.begin(),pair.accesses.end(),[&](const auto &other) {
                            return !other.fieldPath.empty() && other.fieldPath.front()==field;
                        });
                        if(used)continue;
                        access.fieldPath={field};changed=true;break;
                    }
                }
            }
            continue;
        }
        if(!isClusterByName(node.type)) continue;
        const auto schema=resolver.resolve({node.id,"cluster"}).resolution;
        if(!schema.accepted() || schema.type.kind!=TypeKind::Cluster) continue;
        const auto choices=clusterFieldChoices(schema.type);
        for(std::size_t row=0;row<clusterFunctionCount(node);++row) {
            if(row<node.clusterFieldIdPaths.size() && !node.clusterFieldIdPaths[row].empty()) {
                std::string currentName;
                if(!clusterFieldByIdPath(schema.type,node.clusterFieldIdPaths[row],&currentName))
                    continue; // A removed field stays invalid; never select its replacement.
                if(node.clusterFieldNames.size()<=row) node.clusterFieldNames.resize(row+1);
                if(node.clusterFieldNames[row]!=currentName) {
                    node.clusterFieldNames[row]=std::move(currentName);
                    changed=true;
                }
            } else if(row<node.clusterFieldNames.size() && !node.clusterFieldNames[row].empty()) {
                auto ids=clusterNamedFieldIdPath(schema.type,node.clusterFieldNames[row]);
                if(ids.empty()) continue;
                if(node.clusterFieldIdPaths.size()<=row) node.clusterFieldIdPaths.resize(row+1);
                node.clusterFieldIdPaths[row]=std::move(ids);
                changed=true;
            } else {
                // Only new/unselected rows get a default. Existing stable IDs
                // (including removed fields) and explicit choices remain intact.
                auto next=row==0 ? choices.begin() : choices.end();
                if(row>0 && row-1<node.clusterFieldIdPaths.size()) {
                    const auto previous=std::find_if(choices.begin(),choices.end(),[&](const auto &choice) {
                        return choice.ids==node.clusterFieldIdPaths[row-1];
                    });
                    if(previous!=choices.end()) next=previous+1;
                }
                if(next==choices.end()) continue;
                if(node.clusterFieldNames.size()<=row) node.clusterFieldNames.resize(row+1);
                if(node.clusterFieldIdPaths.size()<=row) node.clusterFieldIdPaths.resize(row+1);
                node.clusterFieldNames[row]=next->name;
                node.clusterFieldIdPaths[row]=next->ids;
                changed=true;
            }
        }
    }
    return changed;
}
} // namespace frog::native_engine::binding
