#include "frog_native_engine/CalculusNodeBinding.hpp"
#include "frog_native_engine/ApplicationControlSchema.hpp"
#include "frog_native_engine/CanonicalNumericFixedConstants.hpp"
#include "frog_native_engine/DraftTypedConnections.hpp"
#include "frog_native_engine/ClusterFunctionPortProjection.hpp"
#include "frog_native_engine/ConnectivityCatalog.hpp"
#include "frog_native_engine/DiagramLiteralSchema.hpp"
#include "frog_native_engine/CollectionBinding.hpp"
#include "frog_native_engine/QueueFunctionBinding.hpp"
#include "frog_native_engine/EventFunctionBinding.hpp"
#include "frog_native_engine/NotifierFunctionBinding.hpp"
#include "frog_native_engine/SemaphoreFunctionBinding.hpp"
#include "frog_native_engine/RendezvousFunctionBinding.hpp"
#include "frog_native_engine/OccurrenceFunctionBinding.hpp"
#include "frog_native_engine/PolynomialBinding.hpp"
#include "frog_native_engine/WidgetCatalog.hpp"
#include "FunctionPolymorphism.hpp"
#include "frog_native_engine/DraftSemanticPreparation.hpp"
#include "frog_native_engine/DraftRequiredInputs.hpp"
#include "frog_native_engine/BuildArrayBinding.hpp"
#include "frog_native_engine/ArrayPaletteSemantics.hpp"
#include "frog_native_engine/ArrayEditSemantics.hpp"
#include "frog_native_engine/ReshapeArraySemantics.hpp"
#include "frog_native_engine/ClusterFunctionBinding.hpp"
#include "frog_native_engine/NativeCall.hpp"
#include "frog_native_engine/DiagramStructureCatalog.hpp"
#include "frog_native_engine/DraftFrontendPreparation.hpp"
#include "frog_native_engine/DraftInterfaceInspection.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include "frog_native_engine/CanonicalScalarNative.hpp"
#include "frog_native_engine/CanonicalScalarSelection.hpp"
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/FittingNodeBinding.hpp"
#include "frog_native_engine/CanonicalEventExecutionIr.hpp"
#include "CanonicalComposedDraftMigration.hpp"
#include "frog_native_engine/Sha256.hpp"
#include <algorithm>
#include <map>
#include <iterator>
#include <optional>
#include <unordered_map>
#include <utility>
namespace frog::native_engine {
namespace {
struct PortDescription {
    FunctionPortDirection direction = FunctionPortDirection::Input;
    std::string valueType;
    const FunctionPortBinding *binding = nullptr;
};

[[nodiscard]] std::string endpointKey(const DiagramEdgeEndpoint &endpoint) {
    return endpoint.nodeId+'\x1f'+endpoint.portId;
}
[[nodiscard]] const DiagramNode *findNode(
    const std::unordered_map<std::string, const DiagramNode *> &nodes,
    std::string_view nodeId) {
    const auto found = nodes.find(std::string(nodeId));
    return found == nodes.end() ? nullptr : found->second;
}

[[nodiscard]] std::optional<PortDescription> describePort(
    FrogDraftDataView document,
    const DiagramNode &node,
    std::string_view portId,
    binding::TypeResolver &resolver,
    std::string_view ownerStructureId, std::string_view regionId) {
    if (!node.constantOwnerNodeId.empty()) return std::nullopt;
    if(node.type == kNativeCallDiagramNodeType || findConnectivityFunction(node.type)) {
        const auto direction=resolver.direction({node.id,std::string(portId)});
        if(!direction)return std::nullopt;
        return PortDescription{*direction,resolver.resolve({node.id,std::string(portId)}).resolution.type.canonical()};
    }
    if (isClusterFunction(node.type)) {
        const DiagramEdgeEndpoint endpoint{node.id,std::string(portId)};
        const auto direction=resolver.direction(endpoint);
        if(!direction)return std::nullopt;
        return PortDescription{*direction,resolver.resolveInRegion(endpoint,ownerStructureId,regionId).resolution.type.canonical()};
    }
    if (isDiagramClusterConstant(node)) {
        if (portId!="value") return std::nullopt;
        return PortDescription{FunctionPortDirection::Output,resolver.resolve({node.id,"value"}).resolution.type.canonical()};
    }
    // Editor-managed literals may have no static catalog port contract.
    // Their real value terminal is still a source; target availability is
    // checked independently, never inferred from this authoring description.
    if (isDiagramLiteralConstant(node) && portId == "value" &&
        resolver.direction({node.id, "value"}) == FunctionPortDirection::Output) {
        return PortDescription{FunctionPortDirection::Output,
            resolver.resolve({node.id, "value"}).resolution.type.canonical()};
    }
    if (node.kind == "conditional_default" && portId == "value")
        return PortDescription{FunctionPortDirection::Output, node.valueType};
    if (isDiagramStructureNodeType(node.type)) {
        const DiagramEdgeEndpoint endpoint{node.id, std::string(portId)};
        const auto direction = resolver.direction(endpoint);
        if (!direction) return std::nullopt;
        return PortDescription{*direction, resolver.resolveInRegion(endpoint,ownerStructureId,regionId).resolution.type.canonical()};
    }
    if (node.kind == "widget_value") {
        if (portId != "value" || node.widgetId.empty()) {
            return std::nullopt;
        }
        const auto *object = document.canvas.objectById(node.widgetId);
        if (object == nullptr || !object->widget.has_value()) {
            return std::nullopt;
        }
        const auto &widget=*object->widget;
        auto role=parseWidgetRole(widget.role);
        if(!role)if(const auto *definition=findWidgetDefinitionByClassId(widget.classId))role=definition->canonicalRole;
        const auto direction = role.value_or(WidgetRole::Support) == WidgetRole::Indicator
            ? FunctionPortDirection::Input
            : FunctionPortDirection::Output;
        return PortDescription{
            direction,
            resolver.resolve({node.id, std::string(portId)}).resolution.type.canonical()
        };
    }

    if (node.kind == "frog_call") {
        const auto found = std::find_if(
            node.callPorts.begin(),
            node.callPorts.end(),
            [portId](const DiagramCallPort &port) {
                return port.id == portId;
            });
        if (found == node.callPorts.end()) {
            return std::nullopt;
        }
        return PortDescription{
            found->input ? FunctionPortDirection::Input : FunctionPortDirection::Output,
            found->valueType
        };
    }

    if(node.kind=="primitive"&&!canonicalNumericIeeeConstantMigrationIdentity(node.type).empty()) {
        if(portId!="value")return std::nullopt;
        return PortDescription{FunctionPortDirection::Output,"f64"};
    }
    if((node.kind=="fitting_model_reference" && nativeNumericalModelReference(node.type))||(node.kind=="calculus_function_reference"&&calculusFunctionReferenceDimensions(node.type))) {
        const auto *definition=functionBindingDefinitionForNode(node);
        const auto *port=definition?findFunctionBindingPort(*definition,portId):nullptr;
        if(!port)return std::nullopt;
        return PortDescription{port->direction,
            resolver.resolve({node.id,std::string(portId)}).resolution.type.canonical(),port};
    }

    if (node.applicationControl) {
        const auto direction=resolver.direction({node.id,std::string(portId)});
        if (!direction) return std::nullopt;
        const auto result=resolver.resolve({node.id,std::string(portId)});
        return PortDescription{*direction,result.resolution.type.canonical()};
    }

    if (node.kind != "primitive") {
        return std::nullopt;
    }
    if(isCollectionFunction(node.type)||isCollectionConstant(node)) {
        const auto direction=resolver.direction({node.id,std::string(portId)});
        if(!direction)return std::nullopt;
        return PortDescription{*direction,resolver.resolve({node.id,std::string(portId)}).resolution.type.canonical()};
    }
    const auto *entry = findFunctionCatalogEntryByDiagramNodeType(node.type);
    if (entry == nullptr) {
        return std::nullopt;
    }
    const auto *definition = functionBindingDefinitionForNode(node);
    if (definition == nullptr) {
        return std::nullopt;
    }
    const auto *port = findFunctionBindingPort(*definition, portId);
    if (port == nullptr && !definition->variadicInputPrefix.empty() &&
        portId.starts_with(definition->variadicInputPrefix)) {
        const auto ordinal = functionBindingInputPortOrdinal(*definition, portId);
        if (ordinal && (definition->maximumInputCount == 0U ||
                        *ordinal < definition->maximumInputCount)) {
            const auto input = std::find_if(definition->ports.begin(), definition->ports.end(),
                [](const auto &candidate) { return candidate.direction == FunctionPortDirection::Input; });
            if (input != definition->ports.end()) port = &*input;
        }
    }
    if (port == nullptr) {
        return std::nullopt;
    }
    return PortDescription{ port->direction,
        resolver.resolve({node.id, std::string(portId)}).resolution.type.canonical(), port };
}

} // namespace

// Source owner: DocumentReadiness.cpp describePort and actual edge-validation
// loop; this is language validation, independent of installed realizations.
DraftConnectionInspection inspectDraftTypedConnections(FrogDraftDataView source,
    binding::TypeResolver *existingResolver) {
    // Readiness supplies its already-specialized resolver. Standalone callers
    // first inspect schemas without copying nodes/literals; only a genuinely
    // different Unbundle port count needs the established semantic projection.
    std::vector<DiagramNode> projectedNodes;
    std::optional<binding::TypeResolver> owned;
    if(!existingResolver) {
        owned.emplace(source.diagramNodes,source.diagramEdges,source.canvas);
        existingResolver=&*owned;
        const bool changesPorts=std::any_of(source.diagramNodes.begin(),source.diagramNodes.end(),[&](const auto &node) {
            if(clusterFunctionId(node.type)!="cluster.unbundle")return false;
            const auto schema=existingResolver->resolve({node.id,"cluster"}).resolution;
            return schema.accepted()&&schema.type.kind==binding::TypeKind::Cluster&&!schema.type.children.empty()&&
                schema.type.children.size()<=32&&clusterFunctionCount(node)!=schema.type.children.size();
        });
        if(changesPorts) {
            projectedNodes=projectClusterFunctionPorts(source.diagramNodes,source.diagramEdges,source.canvas);
            owned.reset();owned.emplace(projectedNodes,source.diagramEdges,source.canvas);
            existingResolver=&*owned;
        }
    }
    const FrogDraftDataView document{projectedNodes.empty()?source.diagramNodes:projectedNodes,source.diagramEdges,source.canvas};
    auto &typeResolver=*existingResolver;
    DraftConnectionInspection result;
    std::unordered_map<std::string,const DiagramNode*> nodes;
    nodes.reserve(document.diagramNodes.size());
    for(const auto &node:document.diagramNodes)nodes.emplace(node.id,&node);
    std::unordered_set<std::string> edgeIds;
    const auto emit=[&](DocumentDiagnosticSeverity severity,DocumentDiagnosticDomain domain,
        std::string code,std::string subject,std::string message,bool affectsStructure,bool,
        bool blocksBuild,bool blocksRun) {
        DocumentDiagnostic diagnostic{severity,domain,std::move(code),std::move(subject),std::move(message),blocksBuild,blocksRun};
        diagnostic.stage=DocumentDiagnosticStage::Validation;
        result.diagnostics.push_back(std::move(diagnostic));
        if(affectsStructure)result.structurallyValid=false;
    };
    std::unordered_set<std::string> connectedSinks;
    auto &connectedPorts=result.connectedPorts;
    for (const auto &edge : document.diagramEdges) {
        if (edge.id.empty() || !edgeIds.insert(edge.id).second) {
            emit( DocumentDiagnosticSeverity::Error,
                DocumentDiagnosticDomain::Structure,
                edge.id.empty() ? "empty_diagram_edge_id" : "duplicate_diagram_edge_id",
                edge.id,
                "Every Diagram edge requires a unique, non-empty ID.",
                true, true, true, true);
        }
        const auto *sourceNode = findNode(nodes, edge.from.nodeId);
        const auto *sinkNode = findNode(nodes, edge.to.nodeId);
        if (sourceNode == nullptr || sinkNode == nullptr) {
            emit( DocumentDiagnosticSeverity::Error,
                DocumentDiagnosticDomain::Topology,
                "dangling_diagram_edge", edge.id,
                "A Diagram edge references a node that does not exist.",
                true, true, true, true);
            continue;
        }
        const auto sourcePort = describePort(document, *sourceNode, edge.from.portId, typeResolver, edge.diagramOwnerStructureId, edge.diagramOwnerRegionId);
        const auto sinkPort = describePort(document, *sinkNode, edge.to.portId, typeResolver, edge.diagramOwnerStructureId, edge.diagramOwnerRegionId);
        if (!sourcePort.has_value() || !sinkPort.has_value() ||
            sourcePort->direction != FunctionPortDirection::Output ||
            sinkPort->direction != FunctionPortDirection::Input) {
            emit( DocumentDiagnosticSeverity::Error,
                DocumentDiagnosticDomain::Topology,
                "invalid_diagram_edge_ports", edge.id,
                "A Diagram edge must connect an existing output port to an existing input port.",
                false, true, true, true);
            continue;
        }
        connectedPorts.insert(endpointKey(edge.from));
        connectedPorts.insert(endpointKey(edge.to));
        const auto *sinkEntry = findFunctionCatalogEntryByDiagramNodeType(sinkNode->type);
        if(sinkEntry) connectedPorts.insert(edge.to.nodeId+'\x1f'+std::string(canonicalFunctionBindingPortId(sinkEntry->catalogId,edge.to.portId)));
        const auto *sinkDefinition = functionBindingDefinitionForNode(*sinkNode);
        // A polymorphic port cannot be validated in isolation: two Boolean
        // arrays may each satisfy logical_tree but have incompatible ranks.
        // Use the same graph-aware candidate check as the visible wire editor.
        const bool pilotSink = (sinkDefinition && sinkDefinition->resolverId == binding::kStudioPolymorphicResolver) ||
            sinkNode->kind == "widget_value" || isPolynomialFunction(sinkNode->type) || isDiagramStructureNodeType(sinkNode->type) || isQueueFunction(sinkNode->type) || isEventFunction(sinkNode->type) || isNotifierFunction(sinkNode->type) || isSemaphoreFunction(sinkNode->type) || isRendezvousFunction(sinkNode->type) || isOccurrenceFunction(sinkNode->type);
        const auto typedConnection = pilotSink ? typeResolver.connection(edge) : binding::TypeResolution{};
        const bool compatibleTypes = pilotSink ? typedConnection.accepted() : sinkPort->binding != nullptr
            ? functionPortAcceptsValueType(
                *sinkPort->binding,
                sourcePort->valueType)
            : sourcePort->valueType == sinkPort->valueType ||
                (isNumericScalarValueType(sourcePort->valueType) &&
                 isNumericScalarValueType(sinkPort->valueType));
        if (!compatibleTypes) {
            emit( DocumentDiagnosticSeverity::Error,
                DocumentDiagnosticDomain::Semantics,
                "incompatible_wire_types", edge.id,
                pilotSink ? typedConnection.message : "The Diagram wire is retained for editing, but its source and target data types are incompatible.",
                false, true, true, true);
        }
        const bool regionSink = isDiagramStructureNodeType(sinkNode->type) && edge.diagramOwnerStructureId == sinkNode->id;
        const auto sinkKey = endpointKey(edge.to) + (regionSink ? '\x1f' + edge.diagramOwnerStructureId + '\x1f' + edge.diagramOwnerRegionId : std::string{});
        if (!connectedSinks.insert(sinkKey).second) {
            emit( DocumentDiagnosticSeverity::Error,
                DocumentDiagnosticDomain::Topology,
                "multiple_sources_for_sink", edge.id,
                "A dataflow input cannot receive more than one source.",
                false, true, true, true);
        }
    }

    return result;
}

// Source owner: Studio DocumentReadiness.cpp, former required-input block
// (NativeCall, Cluster, extensible Array families, fixed contracts and calls).
std::vector<DocumentDiagnostic> inspectDraftRequiredInputs(FrogDraftDataView document,
    const std::unordered_set<std::string> *existingConnected) {
    std::vector<DocumentDiagnostic> diagnostics;
    std::optional<binding::TypeResolver> resolver;
    if(!existingConnected || std::any_of(document.diagramNodes.begin(),document.diagramNodes.end(),[](const auto &node){return node.applicationControl.has_value();}))
        resolver.emplace(document.diagramNodes,document.diagramEdges,document.canvas);
    std::unordered_set<std::string> gathered;
    if(!existingConnected) {
        std::map<std::string,const DiagramNode*,std::less<>> nodes;
        for(const auto &node:document.diagramNodes)nodes.emplace(node.id,&node);
        for(const auto &edge:document.diagramEdges) {
            const auto from=resolver->direction(edge.from),to=resolver->direction(edge.to);
            if(!from||!to||*from!=FunctionPortDirection::Output||*to!=FunctionPortDirection::Input)continue;
            gathered.insert(edge.from.nodeId+'\x1f'+edge.from.portId);
            gathered.insert(edge.to.nodeId+'\x1f'+edge.to.portId);
            const auto node=nodes.find(edge.to.nodeId);
            const auto *entry=node==nodes.end()?nullptr:findFunctionCatalogEntryByDiagramNodeType(node->second->type);
            if(entry)gathered.insert(edge.to.nodeId+'\x1f'+std::string(canonicalFunctionBindingPortId(entry->catalogId,edge.to.portId)));
        }
    }
    const auto &connected=existingConnected?*existingConnected:gathered;
    const auto emit=[&](const DiagramNode &node,const std::string &port,const char *message,const char *code="required_input_not_connected") {
        DocumentDiagnostic d{DocumentDiagnosticSeverity::Error,DocumentDiagnosticDomain::Topology,code,node.id+":"+port,message,true,true};
        d.stage=DocumentDiagnosticStage::Validation;d.nodeId=node.id;d.portId=port;
        d.ownerStructureId=node.diagramOwnerStructureId;d.regionId=node.diagramOwnerRegionId;
        diagnostics.push_back(std::move(d));
    };
    for(const auto &node:document.diagramNodes) {
        const auto &id=node.id;
        const auto wired=[&](const std::string &port){return connected.contains(id+'\x1f'+port);};
        if(node.kind=="frog_call") {
            for(const auto &port:node.callPorts)if(port.input&&port.connectionRequirement=="required"&&!wired(port.id))
                emit(node,port.id,"A required FROG call input is not connected.","required_call_input_not_connected");
            continue;
        }
        if(node.kind!="primitive"||!node.constantOwnerNodeId.empty())continue;
        if(node.applicationControl) {
            binding::TypeRef reference;
            if(applicationControlHasReferenceInput(*node.applicationControl))
                for(const auto &edge:document.diagramEdges)if(edge.to.nodeId==id&&edge.to.portId=="reference_in") {
                    reference=resolver->resolve(edge.from).resolution.type;break;
                }
            const auto *presentation=applicationControlReferencePresentation(node,document.diagramNodes,document.diagramEdges,document.canvas);
            for(const auto &port:applicationControlPorts(*node.applicationControl,reference,presentation))
                if(port.direction==ApplicationControlPortDirection::Input&&port.required&&!wired(port.id))
                    emit(node,port.id,"A required Application Control input is not connected.");
            continue;
        }
        const auto *entry=findFunctionCatalogEntryByDiagramNodeType(node.type);
        if(entry&&entry->catalogId==kNativeCallCatalogId) {
            if(node.nativeCall)for(const auto &port:nativeCallPorts(*node.nativeCall))
                if(!port.output&&port.requirement==NativeCallInputRequirement::Required&&!port.defaultCanonicalValue&&!wired(port.id))
                    emit(node,port.id,"A required native-call input needs a producer or an explicit valid default.");
            continue;
        }
        const auto *definition=functionBindingDefinitionForNode(node);
        if(!definition)continue;
        if(isClusterFunction(node.type)) {
            const bool positional=clusterFunctionId(node.type)=="cluster.bundle",schema=wired("cluster");
            for(const auto &port:clusterFunctionPorts(node)) {
                const bool required=!positional||(port.id!="cluster"&&!schema);
                if(!port.output&&required&&!wired(port.id))emit(node,port.id,"A required Cluster function input is not connected.");
            }
        }
        if(entry->catalogId=="string.search_split"&&!wired("search_string")&&!wired("offset"))
            emit(node,"search_string|offset","Search/Split requires a search String or an offset.");
        if(isExpandableArrayPalette(node))for(const auto &port:arrayPaletteSemanticPorts(node)) {
            const auto *binding=findFunctionBindingPort(*definition,port.id);
            if(!port.output&&port.group!=0&&binding&&binding->requirement==FunctionPortRequirement::Required&&!wired(port.id))
                emit(node,port.id,"A required Array input is not connected.");
        }
        if(isBuildArray(node))for(unsigned i=1;i<buildArrayInputCount(node);++i) {
            const auto port=buildArrayInputId(i);
            if(!wired(port)&&!(i==1&&wired("input_2")))emit(node,port,"A Build Array input is not connected.");
        }
        if(isReshapeArray(node))for(unsigned i=1;i<reshapeArrayDimensionCount(node);++i) {
            const auto port=reshapeArrayDimensionId(i);
            if(!wired(port))emit(node,port,"A Reshape Array dimension size is not connected.");
        }
        if(arrayEditExpandable(node))for(unsigned group=1;group<arrayEditGroupCount(node);++group) {
            const auto port=arrayEditValueId(group);
            if(!wired(port))emit(node,port,"A required replacement/insertion value is not connected.");
        }
        for(const auto &port:definition->ports) {
            if(isDiagramStructureNodeType(node.type)||isClusterFunction(node.type))continue;
            if(isExpandableArrayPalette(node)&&!arrayPaletteSemanticPort(node,port.portId))continue;
            if(port.direction==FunctionPortDirection::Input&&port.requirement==FunctionPortRequirement::Required&&!wired(port.portId))
                emit(node,port.portId,"A required function input is not connected.");
        }
    }
    return diagnostics;
}

CanonicalDraftResolvedTypes resolveCanonicalDraftTypes(FrogDraftDataView document,std::string sourceSha256,
    bool &hasScalarNumericCoercion) {
    binding::TypeResolver resolver(document.diagramNodes,document.diagramEdges,document.canvas);
    CanonicalDraftResolvedTypes result;result.originalSha256=std::move(sourceSha256);
    for(const auto &node:document.diagramNodes) {
        if(node.kind!="primitive"||!node.constantOwnerNodeId.empty())continue;
        if(const auto *selection=canonicalScalarSelectionAuthoredContract(node.type)) {
            CanonicalDraftScalarResolution resolved;resolved.nodeId=node.id;resolved.authoredOperation=node.type;
            for(const auto port:selection->inputs) {
                CanonicalDraftScalarOperandType operand;operand.inputPortId=port;
                const DiagramEdge *driver=nullptr;bool unique=true;
                for(const auto &edge:document.diagramEdges)if(edge.to.nodeId==node.id&&edge.to.portId==port){if(driver)unique=false;else driver=&edge;}
                if(unique&&driver){operand.sourceNodeId=driver->from.nodeId;operand.sourcePortId=driver->from.portId;
                    const auto actual=resolver.resolve(driver->from);operand.accepted=actual.resolution.accepted()&&actual.resolution.type.known();operand.complete=actual.complete;
                    if(operand.accepted)operand.valueType=actual.resolution.type.canonical();}
                resolved.operands.push_back(std::move(operand));
            }
            result.scalars.push_back(std::move(resolved));continue;
        }
        const auto *contract=canonicalScalarNativeContract(canonicalScalarMigrationIdentity(node.type));if(!contract)continue;
        CanonicalDraftScalarResolution resolved;resolved.nodeId=node.id;resolved.authoredOperation=node.type;
        const auto *entry=findFunctionCatalogEntryByDiagramNodeType(node.type);
        const bool compare=contract->domain==CanonicalScalarDomain::ExactEquality||contract->domain==CanonicalScalarDomain::ExactOrder;
        std::vector<std::string> actualInputs;
        for(std::size_t input=0;input<contract->inputCount;++input)actualInputs.push_back(compare?(input?"y":"x"):std::string(input?contract->inputB:contract->inputA));
        const auto *definition=entry?findFunctionBindingDefinition(entry->catalogId):nullptr;
        if((contract->domain==CanonicalScalarDomain::Boolean||contract->catalogId=="numeric.add")&&contract->inputCount==2&&definition&&!definition->variadicInputPrefix.empty()) {
            std::map<std::size_t,std::string> optional;
            for(const auto &edge:document.diagramEdges)if(edge.to.nodeId==node.id)if(const auto ordinal=contract->catalogId=="numeric.add"?canonicalNumericAddInputOrdinal(edge.to.portId):canonicalBooleanInputOrdinal(edge.to.portId);ordinal&&*ordinal>=2)optional.emplace(*ordinal,edge.to.portId);
            for(const auto &[ordinal,port]:optional)actualInputs.push_back(port);
        }
        for(const auto &input:actualInputs) {
            CanonicalDraftScalarOperandType operand;operand.inputPortId=input;
            const bool realPort=entry&&((definition && findFunctionBindingPort(*definition,operand.inputPortId) && findFunctionBindingPort(*definition,operand.inputPortId)->direction==FunctionPortDirection::Input)||
                (definition&&functionBindingInputPortOrdinal(*definition,input).has_value()&&canonicalBooleanInputOrdinal(input).has_value()));
            const DiagramEdge *driver=nullptr;bool unique=true;
            for(const auto &edge:document.diagramEdges)if(edge.to.nodeId==node.id&&edge.to.portId==operand.inputPortId){if(driver)unique=false;else driver=&edge;}
            if(realPort&&unique&&driver) {
                operand.sourceNodeId=driver->from.nodeId;operand.sourcePortId=driver->from.portId;
                // Resolve the actual SOURCE connector. A target's promoted
                // input/output type is never evidence for either operand.
                const auto actual=resolver.resolve(driver->from);operand.accepted=actual.resolution.accepted()&&actual.resolution.type.known();operand.complete=actual.complete;
                if(operand.accepted)operand.valueType=actual.resolution.type.canonical();
            }
            resolved.operands.push_back(std::move(operand));
        }
        result.scalars.push_back(std::move(resolved));
    }
    // Routing evidence comes from the current common resolver, before the
    // complete composed-port pass. A direct numeric control/indicator wire
    // has no scalar operation and therefore cannot be found in scalars.
    hasScalarNumericCoercion=false;
    std::map<std::string,const DiagramNode*> routingNodes;
    for(const auto &node:document.diagramNodes)routingNodes.emplace(node.id,&node);
    for(const auto &edge:document.diagramEdges) {
        const auto position=routingNodes.find(edge.to.nodeId);
        if(position==routingNodes.end())continue;
        const auto *found=position->second;
        if(!found->constantOwnerNodeId.empty())continue;
        bool implicit=false;
        if(found->kind=="widget_value"&&edge.to.portId=="value") {
            const auto *widget=document.canvas.objectById(found->widgetId);
            implicit=widget&&widget->widget&&widget->widget->role=="indicator";
        } else if(found->kind=="primitive"&&found->type!="frog.studio.structures.feedback_node") {
            const auto *entry=findFunctionCatalogEntryByDiagramNodeType(found->type);
            if(!entry)entry=findFunctionCatalogEntryByPrimitiveType(found->type);
            const auto *definition=functionBindingDefinitionForNode(*found);
            const auto *port=definition?findFunctionBindingPort(*definition,
                entry?canonicalFunctionBindingPortId(entry->catalogId,edge.to.portId):std::string_view(edge.to.portId)):nullptr;
            implicit=port&&port->direction==FunctionPortDirection::Input&&
                (port->connectionPolicy==FunctionPortConnectionPolicy::NumericImplicit||
                 port->connectionPolicy==FunctionPortConnectionPolicy::NumericScalarOrArrayImplicit);
        }
        if(!implicit)continue; // No structure/tunnel/Feedback or explicit-conversion admission.
        const auto source=resolver.resolveSource(edge);
        const auto target=resolver.resolveInRegion(edge.to,edge.diagramOwnerStructureId,edge.diagramOwnerRegionId);
        if(!source.complete||!target.complete||!source.resolution.accepted()||!target.resolution.accepted()||
            !canonicalNumericScalarCoercionSupported(source.resolution.type,target.resolution.type))continue;
        const auto from=resolver.direction(edge.from),to=resolver.direction(edge.to);
        if(!from||!to||*from!=FunctionPortDirection::Output||*to!=FunctionPortDirection::Input)continue;
        if(!resolver.connection(edge).accepted()||binding::connect(source.resolution.type,target.resolution.type,
                binding::ConnectionPolicy::NumericImplicit).status!=binding::ResolutionStatus::Coercion)continue;
        hasScalarNumericCoercion=true;break;
    }
    return result;
}
CanonicalDraftMigrationResult prepareDraftSemanticSource(const FrogDraftData &document,std::string_view bytes,std::string_view uri) {
    bool coercion=false;auto types=resolveCanonicalDraftTypes(document,hashing::sha256Hex(bytes),coercion);
    const auto root=source::parseJson(bytes,nullptr,source::JsonNumberPolicy::PreserveLexeme);
    if(!root) {CanonicalDraftMigrationResult out;out.diagnostics.push_back({"frontend.draft_json_invalid","/","The draft JSON could not be read.",true,DocumentDiagnosticDomain::Integrity});return out;}
    bool legacyNumeric=false;
    if(const auto *panel=root->member("frontPanel"))if(const auto *widgets=panel->member("widgets");widgets&&widgets->type==source::JsonValue::Type::Array)
        legacyNumeric=std::any_of(widgets->array.begin(),widgets->array.end(),[](const auto &widget){const auto *klass=widget.member("class_ref");return klass&&klass->type==source::JsonValue::Type::String&&klass->string=="frog.widgets.numeric_control";});
    if(coercion||canonicalDraftNeedsComposition(*root,&types)||legacyNumeric) {
        auto diagnostics=appendCanonicalDraftComposedPorts(document,types);
        if(std::any_of(diagnostics.begin(),diagnostics.end(),[](const auto &d){return d.blocking;})) {CanonicalDraftMigrationResult out;out.diagnostics=std::move(diagnostics);return out;}
    }
    return prepareCanonicalDraftMigration(bytes,uri,types);
}
std::vector<DocumentDiagnostic> inspectDraftSubFrogSemantics(const FrogDraftData &document,const ConditionalDisableCompilationContext &context) {
    auto diagnostics=inspectFrogDraftInterfaceBindings(document.declaredInterface,document.widgets,document.diagramNodes,document.properties,context);
    auto connections=inspectDraftTypedConnections(document);
    auto required=inspectDraftRequiredInputs(document,&connections.connectedPorts);
    diagnostics.insert(diagnostics.end(),std::make_move_iterator(connections.diagnostics.begin()),std::make_move_iterator(connections.diagnostics.end()));
    diagnostics.insert(diagnostics.end(),std::make_move_iterator(required.begin()),std::make_move_iterator(required.end()));
    const auto bytes=serializeFrogDraftData(document);
    const auto raw=source::parseJson(bytes,nullptr,source::JsonNumberPolicy::PreserveLexeme);
    if(std::any_of(diagnostics.begin(),diagnostics.end(),[](const auto &d){return d.severity==DocumentDiagnosticSeverity::Error;})) {
        if(raw)for(auto &d:diagnostics)attributeDraftSourceDiagnostic(d,document,*raw);
        return diagnostics;
    }
    const auto append=[&](DocumentDiagnostic diagnostic) {
        if(raw)attributeDraftSourceDiagnostic(diagnostic,document,*raw);
        diagnostics.push_back(std::move(diagnostic));
    };
    auto prepared=prepareDraftSemanticSource(document,bytes,"frog-callee:/current");
    if(!prepared.accepted()) {
        for(const auto &d:prepared.diagnostics)if(d.blocking) {
            DocumentDiagnostic diagnostic{DocumentDiagnosticSeverity::Error,d.domain,d.code,{},d.message,true,true};
            diagnostic.sourcePath=d.path;append(std::move(diagnostic));
        }
        if(std::none_of(diagnostics.begin(),diagnostics.end(),[](const auto &d){return d.severity==DocumentDiagnosticSeverity::Error;}))
            append({DocumentDiagnosticSeverity::Error,DocumentDiagnosticDomain::Control,"frontend.callee_validation_indeterminate",{},
                "Callee preparation produced neither a validated source nor an attributable failure.",true,true});
        return diagnostics;
    }
    std::vector<CanonicalIrDiagnostic> why;CanonicalIrReadOptions options;options.sourceUri="frog-callee:/current";
    const bool event=std::any_of(document.diagramNodes.begin(),document.diagramNodes.end(),[](const auto &n){return n.type=="frog.studio.structures.event_structure";});
    const bool admitted=event?bool(CanonicalEventExecutionIr::derive(*prepared.publicSource,"ir:callee-check",options,&why)):bool(CanonicalExecutionIr::derive(*prepared.publicSource,"ir:callee-check",options,&why));
    if(!admitted) {
        for(const auto &d:why) {
            DocumentDiagnostic diagnostic{DocumentDiagnosticSeverity::Error,d.unsupported?DocumentDiagnosticDomain::FrontendCapability:DocumentDiagnosticDomain::Semantics,d.code,{},d.message,true,true};
            diagnostic.stage=DocumentDiagnosticStage::Fir;
            // FIR reports canonical Source locations. Use the existing exact
            // migration map to return to the callee's authored namespace.
            const std::string prefix=options.sourceUri+"#";
            const auto path=d.location.starts_with(prefix)?d.location.substr(prefix.size()):d.location;
            const CanonicalDraftMigrationIdentity *match=nullptr;
            for(const auto &mapping:prepared.identities)
                if(path==mapping.publicPointer||(path.starts_with(mapping.publicPointer)&&path.size()>mapping.publicPointer.size()&&path[mapping.publicPointer.size()]=='/'))
                    if(!match||mapping.publicPointer.size()>match->publicPointer.size())match=&mapping;
            if(match) {
                diagnostic.sourcePath=match->originalPointer;
                if(!match->generated)diagnostic.sourcePath+=path.substr(match->publicPointer.size());
            }
            append(std::move(diagnostic));
        }
        if(why.empty()) {
            DocumentDiagnostic diagnostic{DocumentDiagnosticSeverity::Error,DocumentDiagnosticDomain::Control,"frontend.callee_validation_indeterminate",{},
                "Callee FIR derivation did not complete and returned no attributable diagnostic.",true,true};
            diagnostic.stage=DocumentDiagnosticStage::Fir;append(std::move(diagnostic));
        }
    }
    return diagnostics;
}
}
