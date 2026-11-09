#include "frog_native_engine/CanonicalNumericFixedConstants.hpp"
#include "frog_native_engine/ContextPathIdentity.hpp"
#include "frog_native_engine/CanonicalScalarNative.hpp"
#include "frog_native_engine/BuildArrayBinding.hpp"
#include "frog_native_engine/CanonicalDraftPortResolution.hpp"
#include "frog_native_engine/ApplicationControlSchema.hpp"
#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "frog_native_engine/ClusterFunctionBinding.hpp"
#include "frog_native_engine/ClusterFunctionPortProjection.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include "frog_native_engine/DiagramLiteralSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/FittingNodeBinding.hpp"
#include "frog_native_engine/FunctionCatalog.hpp"
#include "frog_native_engine/ErrorPaletteBinding.hpp"
#include "frog_native_engine/StructureBindingDefinition.hpp"
#include "frog_native_engine/InPlaceStructure.hpp"
#include "frog_native_engine/NativeCall.hpp"
#include "frog_native_engine/CollectionBinding.hpp"
#include "frog_native_engine/EventFunctionBinding.hpp"
#include "frog_native_engine/StructureTunnelMode.hpp"
#include "frog_native_engine/DiagramStructureCatalog.hpp"
#include "frog_native_engine/WidgetTypedValueSource.hpp"
#include "frog_native_engine/WidgetDataOperations.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <iterator>
#include <map>
#include <set>
#include <tuple>

namespace frog::native_engine {
std::vector<CanonicalDraftMigrationDiagnostic> appendCanonicalDraftWidgetResolutions(
    FrogDraftDataView document,CanonicalDraftResolvedTypes &result,std::span<const std::string_view> widgetIds) {
    std::vector<CanonicalDraftMigrationDiagnostic> diagnostics;
    const auto diagnostic=[&](const std::string &id,const std::string &port,const std::string &message) {
        diagnostics.push_back({"migration.composed_port_resolution_invalid",id+":"+port,message,true});
    };
    for(const auto &object:document.canvas.objects())if(object.widget) {
        if(!widgetIds.empty()&&std::find(widgetIds.begin(),widgetIds.end(),object.id)==widgetIds.end())continue;
        const auto &current=*object.widget;const auto type=binding::widgetType(current);
        if(!type.valid()||!type.type.known()) {diagnostic(object.id,"value","The current widget has no complete schema.");continue;}
        const auto defaults=data_operations::defaultValue(current);const auto defaultType=binding::widgetType(defaults);
        if(defaults.instanceId!=current.instanceId||!defaultType.valid()||!canonicalObjectReferenceFullTypeEqual(type.type,defaultType.type)) {
            diagnostic(object.id,"default","The existing default projection changed actual widget identity or its current complete schema.");continue;
        }
        const auto currentJson=data_operations::encode(current),defaultJson=data_operations::encode(defaults);
        if(currentJson.empty()||defaultJson.empty()) {diagnostic(object.id,"value","The common private value codec refused current/default records.");continue;}
        CanonicalDraftWidgetResolution evidence{object.id,canonical_json::encode(canonicalObjectReferenceWriteType(type.type)),currentJson,defaultJson};
        if(const auto value=typedWidgetSourceValue(current))evidence.currentValueJson=canonical_json::encode(*value);
        if(const auto value=typedWidgetSourceValue(defaults))evidence.defaultValueJson=canonical_json::encode(*value);
        result.widgets.push_back(std::move(evidence));
    }
    return diagnostics;
}
std::vector<CanonicalDraftMigrationDiagnostic> appendCanonicalDraftComposedPorts(
    FrogDraftDataView document,CanonicalDraftResolvedTypes &result) {
    std::vector<CanonicalDraftMigrationDiagnostic> diagnostics;
    const auto projected=projectClusterFunctionPorts(document.diagramNodes,document.diagramEdges,document.canvas);
    binding::TypeResolver resolver(projected,document.diagramEdges,document.canvas);
    using Key=std::tuple<std::string,std::string,bool>;
    std::map<Key,binding::TypeRef> schemas;
    std::set<std::tuple<std::string,std::string,bool,std::string,std::string>> emitted;
    std::map<std::string,const DiagramNode*> nodes;
    for(const auto &node:projected)nodes.emplace(node.id,&node);
    const auto diagnostic=[&](const std::string &id,const std::string &port,const std::string &message) {
        diagnostics.push_back({"migration.composed_port_resolution_invalid",id+":"+port,message,true});
    };
    auto widgetDiagnostics=appendCanonicalDraftWidgetResolutions(document,result);
    diagnostics.insert(diagnostics.end(),std::make_move_iterator(widgetDiagnostics.begin()),std::make_move_iterator(widgetDiagnostics.end()));
    const auto append=[&](const std::string &id,const std::string &port,bool input,const binding::TypeRef &schema,
        const std::string &sourceNode=std::string{},const std::string &sourcePort=std::string{}) {
        if(!schema.known())return;
        const Key key{id,port,input};const auto [found,inserted]=schemas.emplace(key,schema);
        if(!inserted&&!canonicalObjectReferenceFullTypeEqual(found->second,schema)) {
            diagnostic(id,port,"One connector has different complete types in distinct regions; this admission needs explicitly regional connector schemas.");return;
        }
        if(!emitted.emplace(id,port,input,sourceNode,sourcePort).second)return;
        result.ports.push_back({id,port,input,canonical_json::encode(canonicalObjectReferenceWriteType(schema)),sourceNode,sourcePort});
    };
    // Real wire sources are resolved first in the edge's ACTUAL semantic case.
    // This neither selects the visible case nor promotes a source to its sink.
    for(const auto &edge:document.diagramEdges) {
        const auto source=resolver.resolveSource(edge);
        const auto target=resolver.resolveInRegion(edge.to,edge.diagramOwnerStructureId,edge.diagramOwnerRegionId);
        if(!source.resolution.accepted()||!source.resolution.type.known()) {
            diagnostic(edge.from.nodeId,edge.from.portId,"The actual source connector is unresolved: "+source.resolution.message);continue;
        }
        if(!target.resolution.accepted()||!target.resolution.type.known()) {
            diagnostic(edge.to.nodeId,edge.to.portId,"The actual sink connector is unresolved: "+target.resolution.message);continue;
        }
        const auto sourceDirection=resolver.direction(edge.from),targetDirection=resolver.direction(edge.to);
        if(!sourceDirection||!targetDirection||*sourceDirection!=FunctionPortDirection::Output||*targetDirection!=FunctionPortDirection::Input) {
            diagnostic(edge.to.nodeId,edge.to.portId,"The real edge contradicts the common connector directions.");continue;
        }
        append(edge.from.nodeId,edge.from.portId,false,source.resolution.type);
        append(edge.to.nodeId,edge.to.portId,true,target.resolution.type,edge.from.nodeId,edge.from.portId);
    }
    const auto addEndpoint=[&](const DiagramNode &node,const std::string &port,bool input,
        const binding::TypeRef &declared=binding::TypeRef{}) {
        // A wired input already has every actual regional driver. Adding an
        // unbound duplicate would discard its source provenance.
        if(schemas.contains({node.id,port,input}))return;
        const auto resolved=resolver.resolveInRegion({node.id,port},node.diagramOwnerStructureId,node.diagramOwnerRegionId);
        if(resolved.resolution.accepted()&&resolved.resolution.type.known())append(node.id,port,input,resolved.resolution.type);
        else if(declared.known())append(node.id,port,input,declared);
    };
    for(const auto &node:projected) {
        if(!node.constantOwnerNodeId.empty()) {
            // An owned Array is literal data inside its Cluster, never a wire
            // endpoint. The composed literal decoder still needs its exact
            // element schema/rank, including the real stored prototype.
            if(isDiagramLiteralConstant(node)&&node.type.find("array")!=std::string::npos) {
                const auto actual=resolver.resolveLiteral(node.id);
                if(actual.resolution.accepted()&&actual.resolution.type.kind==binding::TypeKind::Array&&actual.resolution.type.known())
                    append(node.id,"value",false,actual.resolution.type);
                else diagnostic(node.id,"value","The owned Array literal has no complete exact schema: "+actual.resolution.message);
            }
            continue;
        }
        if(!frog::native_engine::canonicalNumericIeeeConstantMigrationIdentity(node.type).empty()) {
            addEndpoint(node,"value",false,binding::TypeRef::primitive("f64"));continue;
        }
        if(node.kind=="widget_value") {
            const auto *object=document.canvas.objectById(node.widgetId);
            if(!object||!object->widget){diagnostic(node.id,"value","A real widget declaration is required.");continue;}
            const auto schema=binding::widgetType(*object->widget);
            if(!schema.valid()||!schema.type.known()){diagnostic(node.id,"value",schema.error);continue;}
            addEndpoint(node,"value",object->widget->role=="indicator",schema.type);continue;
        }
        if(node.nativeCall) {
            for(const auto &port:nativeCallPorts(*node.nativeCall,64))
                addEndpoint(node,port.id,!port.output,port.valueType);
            continue;
        }
        if(node.applicationControl) {
            const auto effective=resolver.resolve({node.id,node.applicationControl->kind==ApplicationControlKind::Close?"reference_in":"reference_out"});
            for(const auto &port:applicationControlPorts(*node.applicationControl,effective.resolution.type))
                addEndpoint(node,port.id,port.direction==ApplicationControlPortDirection::Input,port.type);
            continue;
        }
        if(isInPlaceStructure(node)) {
            const auto input=[&](std::string_view id){return resolver.resolve({node.id,std::string(id)});};
            for(const auto &port:inPlacePorts(node,input))
                addEndpoint(node,port.id,port.direction==FunctionPortDirection::Input,port.type);
            // Ordinary tunnels keep their normal ownership and type rules.
            for(const auto &tunnel:node.caseStructureTunnels) {
                const auto actual=resolver.tunnel(node,tunnel);
                if(!actual.resolution.accepted()||!actual.resolution.type.known()){diagnostic(node.id,tunnel.id,"The IPE tunnel has no complete inferred type.");continue;}
                addEndpoint(node,binding::tunnelInputId(tunnel.id),true,actual.resolution.type);
                const auto projected=diagramTunnelOutputType(node,tunnel,actual.resolution.type);
                addEndpoint(node,binding::tunnelOutputId(tunnel.id),false,projected.accepted()?projected.type:binding::TypeRef{});
            }
            continue;
        }
        if(node.kind=="structure"||isDiagramStructureNodeType(node.type)) {
            for(const auto &tunnel:node.caseStructureTunnels) {
                const auto actual=resolver.tunnel(node,tunnel);
                if(!actual.resolution.accepted()||!actual.resolution.type.known()) {
                    diagnostic(node.id,tunnel.id,"The authored structure boundary has no complete inferred type.");continue;
                }
                addEndpoint(node,binding::tunnelInputId(tunnel.id),true,actual.resolution.type);
                const auto projected=diagramTunnelOutputType(node,tunnel,actual.resolution.type);
                addEndpoint(node,binding::tunnelOutputId(tunnel.id),false,projected.accepted()?projected.type:binding::TypeRef{});
            }
            if(binding::isCase(node)) {
                addEndpoint(node,std::string(binding::caseBindingDefinition().selectorOuterPort),true);
                addEndpoint(node,std::string(binding::caseBindingDefinition().selectorInnerPort),false);
            } else if(node.type=="frog.studio.structures.while_loop") {
                addEndpoint(node,"iteration",false,binding::TypeRef::primitive("i64"));
                addEndpoint(node,"condition",true,binding::TypeRef::primitive("bool"));
            } else if(node.type=="frog.studio.structures.for_loop") {
                addEndpoint(node,"count",true,binding::TypeRef::primitive("i64"));
                addEndpoint(node,"count_value",false,binding::TypeRef::primitive("i64"));
                addEndpoint(node,"iteration",false,binding::TypeRef::primitive("i64"));
                if(node.forLoopConditionalTerminal)addEndpoint(node,"condition",true,binding::TypeRef::primitive("bool"));
            } else if(node.type=="frog.studio.structures.event_structure") {
                addEndpoint(node,"timeout",true,binding::TypeRef::primitive("i64"));
                addEndpoint(node,"timeout_value",false,binding::TypeRef::primitive("i64"));
                for(const auto &region:node.eventStructureCases) {
                    const auto &schema=resolver.eventSchema(node.id,region.id);
                    if(!schema.valid()){diagnostic(node.id,region.id,schema.error);continue;}
                    for(const auto &field:schema.fields)if(field.type.known()&&
                        std::find(region.eventDataFields.begin(),region.eventDataFields.end(),field.id)!=region.eventDataFields.end())
                        append(node.id,"event_data."+field.id,false,field.type);
                    for(const auto &field:schema.responses)if(field.type.known())
                        addEndpoint(node,"event_response."+field.id,true,field.type);
                }
            }
            continue;
        }
        if(node.kind!="primitive")continue;
        if(node.type=="frog.studio.structures.feedback_node") {
            for(const auto id:{"value","initial_value","result"})addEndpoint(node,id,std::string_view(id)!="result");
            addEndpoint(node,"enable",true,binding::TypeRef::primitive("bool"));
            const auto data=resolver.resolve({node.id,"result"});
            if(!data.resolution.accepted()||!data.resolution.type.known())
                diagnostic(node.id,"result","Feedback state type must be anchored by a complete graph type; no f64 default is inferred.");
            continue;
        }
        if(isContextPathFunction(node.type)){addEndpoint(node,"result",false,binding::TypeRef::primitive("path"));continue;}
        if(isCollectionConstant(node)) {
            const auto actual=resolver.resolveLiteral(node.id);
            const auto *state=collectionConstantState(node);
            const std::string preferred=state&&state->kind==CollectionKind::Map?"map":"set";
            if(!actual.resolution.accepted()||!actual.resolution.type.known()) {
                diagnostic(node.id,preferred,"The Collection constant has no complete exact schema.");continue;
            }
            // Both the current map/set spelling and the historical value
            // spelling denote the same single producer. Keep the spelling
            // actually used by the document; do not invent its other alias.
            std::vector<std::string> outputs;
            for(const auto &[key,_]:schemas)if(std::get<0>(key)==node.id&&!std::get<2>(key))outputs.push_back(std::get<1>(key));
            if(outputs.empty())addEndpoint(node,preferred,false,actual.resolution.type);
            else if(outputs.size()!=1 || (outputs.front()!=preferred&&outputs.front()!="value"))
                diagnostic(node.id,preferred,"A Collection constant must use one effective producer identity; mixed output aliases are not separate ports.");
            continue;
        }
        if(isEventFunction(node.type)) {
            // Register's stable source:row connectors replace the catalog's
            // generic source specimen. Never emit both as runtime inputs.
            for(const auto &p:eventFunctionPorts(node))addEndpoint(node,p.id,!p.output);
            continue;
        }
        if(isCollectionFunction(node.type)) {
            const auto input=[&](std::string_view id)->binding::BindingResult {
                const auto edge=std::find_if(document.diagramEdges.begin(),document.diagramEdges.end(),[&](const auto &e){return e.to.nodeId==node.id&&e.to.portId==id;});
                return edge==document.diagramEdges.end()?binding::BindingResult{}:resolver.resolveSource(*edge);
            };
            for(const auto &p:collectionPorts(node,input))addEndpoint(node,p.id,p.direction==FunctionPortDirection::Input,p.type);
            continue;
        }
        const auto *fixedStringEntry=findFunctionCatalogEntryByDiagramNodeType(node.type);
        const bool fixedStringLiteral=fixedStringEntry&&functionFixedStringConstantValue(*fixedStringEntry).has_value();
        if(fixedStringLiteral||node.type=="frog.studio.cluster.constant"||node.type=="frog.studio.error.constant"||
            node.type=="frog.studio.numeric.numeric_constant"||node.type=="frog.studio.numeric.enum_constant"||
            node.type=="frog.studio.boolean.true_constant"||node.type=="frog.studio.boolean.false_constant"||
            node.type=="frog.studio.string.constant"||node.type=="frog.studio.path.constant"||
            node.type=="frog.studio.array.array_constant"||node.type=="frog.studio.numeric.numeric_array_constant"||
            node.type=="frog.studio.numeric.enum_array_constant"||node.type=="frog.studio.numeric.ring_array_constant"||
            node.type=="frog.studio.string.string_array_constant"||node.type=="frog.studio.path.path_array_constant"||node.type.starts_with("frog.studio.array.other_constants.")) {
            const auto actual=resolver.resolveLiteral(node.id);
            if(actual.resolution.accepted()&&actual.resolution.type.known())addEndpoint(node,"value",false,actual.resolution.type);
            else diagnostic(node.id,"value","Literal has no complete authored schema: "+actual.resolution.message);
            continue;
        }
        if(isClusterFunction(node.type)) {
            for(const auto &port:clusterFunctionPorts(node))addEndpoint(node,port.id,!port.output);
            continue;
        }
        if(isErrorPaletteFunction(node.type)) {
            for(const auto &port:errorPalettePorts(node,true))addEndpoint(node,port.id,!port.output);
            continue;
        }
        if(isBuildArray(node)) {
            // The catalogue supplies the initial slots; the authored arity
            // owns the actual signature, including its historical second alias.
            for(int index=0;index<buildArrayInputCount(node);++index) {
                auto id=buildArrayInputId(index);
                if(index==1&&schemas.contains({node.id,"input_2",true}))id="input_2";
                addEndpoint(node,id,true);
            }
            addEndpoint(node,"result",false);
            continue;
        }
        const auto *entry=findFunctionCatalogEntryByDiagramNodeType(node.type);
        const auto *definition=functionBindingDefinitionForNode(node);
        if(definition&&entry&&entry->catalogId=="numeric.add") {
            // Wired optional inputs were emitted with actual source provenance
            // above. Validate their identities without adding unwired slots.
            for(const auto &edge:document.diagramEdges)if(edge.to.nodeId==node.id&&!canonicalNumericAddInputOrdinal(edge.to.portId))
                diagnostic(node.id,edge.to.portId,"Add input identity is outside a,b,input_3..input_16.");
        }
        if(definition)for(const auto &port:definition->ports) {
            // This fallback is a DECLARED fixed optional/default type from the
            // common semantic catalogue. It never establishes a wired source.
            const auto declared=binding::parseType(port.defaultValueType);
            addEndpoint(node,port.portId,port.direction==FunctionPortDirection::Input,
                declared.valid()?declared.type:binding::TypeRef{});
        }
        // No presentation-only fallback supplies semantic ports. Connected
        // endpoints above and the canonical producer validate intrinsic ports;
        // an absent semantic definition stays unavailable, never a UI guess.
    }
    return diagnostics;
}
}
