#include "frog_native_engine/CanonicalFrontendMetrics.hpp"
#include "frog_native_engine/ContextPathIdentity.hpp"
#include "CanonicalComposedExecutionIr.hpp"
#include "frog_native_engine/CanonicalFileNative.hpp"
#include "CanonicalBooleanTree.hpp"
#include "CanonicalComparisonTree.hpp"
#include "frog_native_engine/CanonicalNumericScalarPalette.hpp"
#include "CanonicalJsonSupport.hpp"
#include "frog_native_engine/TimedLoopConfiguration.hpp"
#include "frog_native_engine/DiagramStructureShelfCatalog.hpp"
#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "frog_native_engine/CanonicalNativeCallScalar.hpp"
#include "frog_native_engine/CanonicalInPlaceDescriptor.hpp"
#include "frog_native_engine/CanonicalCollectionDescriptor.hpp"
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog_native_engine/CanonicalNumericFixedConstants.hpp"
#include "frog_native_engine/CanonicalNumericArrayReduction.hpp"
#include "frog_native_engine/CanonicalNumericRandom.hpp"
#include "frog_native_engine/CanonicalNumericAccumulate.hpp"
#include "frog_native_engine/CanonicalNumericBooleanConversion.hpp"
#include "frog_native_engine/CanonicalNumericCalibration.hpp"
#include "frog_native_engine/CanonicalNumericBitReinterpretation.hpp"
#include "frog_native_engine/CanonicalNumericText.hpp"
#include "frog_native_engine/CanonicalNumericComplex.hpp"
#include "frog_native_engine/CanonicalScalarNative.hpp"
#include "frog_native_engine/CollectionAuthoringState.hpp"
#include "frog_native_engine/IndexedTunnelType.hpp"
#include "frog_native_engine/StructureTunnelMode.hpp"
#include "frog_native_engine/NotifierFunctionBinding.hpp"
#include "frog_native_engine/QueueFunctionBinding.hpp"
#include "frog_native_engine/NotifierReference.hpp"
#include "frog_native_engine/SemaphoreFunctionBinding.hpp"
#include "frog_native_engine/RendezvousFunctionBinding.hpp"
#include "frog_native_engine/OccurrenceFunctionBinding.hpp"
#include "frog_native_engine/EventFunctionBinding.hpp"
#include "frog_native_engine/EventReference.hpp"
#include "frog_native_engine/OccurrenceReference.hpp"
#include "frog_native_engine/RendezvousReference.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/FittingNodeBinding.hpp"
#include "frog_native_engine/CalculusNodeBinding.hpp"
#include "frog_native_engine/StatisticsNodeBinding.hpp"
#include "frog_native_engine/InterpolationNodeBinding.hpp"
#include "frog_native_engine/GeometryNodeBinding.hpp"
#include "frog_native_engine/CanonicalScalePowerTwo.hpp"
#include "frog_native_engine/ExponentialFunctionIdentity.hpp"
#include "frog_native_engine/FunctionPortIdentities.hpp"
#include "frog_native_engine/CanonicalArrayNative.hpp"
#include "frog_native_engine/LinearAlgebraBinding.hpp"
#include "frog_native_engine/PolynomialBinding.hpp"
#include "frog_native_engine/BlasBinding.hpp"
#include "frog_native_engine/FittingBinding.hpp"
#include "frog_native_engine/CalculusBinding.hpp"
#include "frog_native_engine/CanonicalDifferentialNative.hpp"
#include "frog_native_engine/OptimizationBinding.hpp"
#include "frog_native_engine/CanonicalDiscreteArrayNative.hpp"
#include "frog_native_engine/StatisticsBinding.hpp"
#include "frog_native_engine/InterpolationBinding.hpp"
#include "frog_native_engine/GeometryBinding.hpp"
#include "frog_native_engine/DiagramTunnelIdentity.hpp"
#include "frog_native_engine/Sha256.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <deque>
#include <map>
#include <set>

namespace frog::native_engine::structured_ir {
namespace {
using namespace canonical_json;
struct Failure {CanonicalIrDiagnostic diagnostic;};
[[noreturn]] void fail(std::string code,std::string message,std::string path,bool unsupported=false) {
    throw Failure{{std::move(code),std::move(message),std::move(path),unsupported}};
}
const Json &objectAt(const Json *v,const std::string &p) {
    if(!v||v->type!=Json::Type::Object)fail("IR_COMPOSED_OBJECT","An explicit semantic object is required.",p);return *v;
}
const std::vector<Json> &arrayAt(const Json *v,const std::string &p) {
    if(!v||v->type!=Json::Type::Array)fail("IR_COMPOSED_ARRAY","An explicit semantic array is required.",p);return v->array;
}
std::string text(const Json *v,const std::string &p,bool empty=false) {
    if(!v||v->type!=Json::Type::String||(!empty&&v->string.empty()))fail("IR_COMPOSED_STRING","An explicit bounded string is required.",p);return v->string;
}
bool flag(const Json *v,bool fallback=false) {
    if(!v)return fallback;
    if(v->type!=Json::Type::Boolean)fail("IR_COMPOSED_BOOLEAN","A declared semantic flag must be Boolean.","/");return v->boolean;
}
void fields(const Json &v,std::initializer_list<std::string_view> allowed,const std::string &path) {
    objectAt(&v,path);
    for(const auto &[key,_]:v.object)if(std::find(allowed.begin(),allowed.end(),key)==allowed.end())
        fail("IR_COMPOSED_FIELD_UNSUPPORTED","No composed semantic contract is declared for field: "+key,path+"/"+key,true);
}
Json strings(const std::vector<std::string> &vs) {auto a=array();for(const auto &v:vs)a.array.push_back(string(v));return a;}
Json parse(std::string_view s) {auto v=source::parseJson(s,nullptr,source::JsonNumberPolicy::PreserveLexeme);if(!v)fail("IR_COMPOSED_JSON","An exact retained semantic value is invalid.","/");return std::move(*v);}
struct RawNode {const Json *raw;std::string path,region,owner;};
struct RawEdge {const Json *raw;std::string path,region,owner;};
struct Builder {
    const FrogSourceDocument &source;const CanonicalIrReadOptions &options;Draft out;
    std::map<std::string,RawNode> rawNodes;
    std::vector<RawEdge> rawEdges;
    std::map<std::string,std::size_t> nodeIndex;
    std::map<std::string,const Json*> widgets;
    std::map<std::string,std::string> widgetPaths;
    std::set<std::string> edgeIds,regionIds;
    std::map<std::string,std::string> edgeRegions;
    // Borrowed only during this Builder's lifetime, after rawEdges is sealed.
    // Keys are synthetic FIR identities; sourceNodeId remains the real receiver.
    std::map<std::string,const RawEdge*> coercionOrigins;
    struct ErrorBridgeOrigin {const RawEdge *edge;bool sourceEndpoint;};
    std::map<std::string,ErrorBridgeOrigin> errorBridgeOrigins;
    std::string ref(const std::string &p)const {return options.sourceUri+"#"+p;}
    void map(std::string id,std::string kind,const std::string &p,std::string target,std::string targetKind="object",std::string relation="direct_primary") {
        out.maps.push_back({std::move(id),std::move(kind),ref(p),std::move(targetKind),std::move(relation),{std::move(target)}});
    }
    void declarations() {
        fields(source.root(),{"spec_version","metadata","interface","diagram","front_panel","ide","icon","connector","cache","host","execution_policy"},"/");
        const auto &panel=objectAt(source.section("front_panel"),"/front_panel");
        const auto &ws=arrayAt(panel.member("widgets"),"/front_panel/widgets");
        if(ws.size()>options.limits.maximumWidgets)fail("IR_RESOURCE_LIMIT","Widget budget exceeded.","/front_panel",true);
        for(std::size_t i=0;i<ws.size();++i) {
            const auto p="/front_panel/widgets/"+std::to_string(i);const auto id=text(ws[i].member("id"),p);
            if(id.size()>options.limits.maximumSourceIdBytes||!widgets.emplace(id,&ws[i]).second)fail("IR_SOURCE_DUPLICATE_ID","Widget identities must remain distinct and bounded.",p);
            widgetPaths[id]=p;
        }
    }
    void index(const Json &scope,const std::string &path,std::string region={},std::string owner={},unsigned depth=0) {
        if(depth>32)fail("IR_RESOURCE_LIMIT","Explicit region nesting exceeds32.",path,true);
        fields(scope,{"profile","nodes","edges","wire_fragments","annotations"},path);
        if(text(scope.member("profile"),path)!=composedSourceProfile)fail("IR_COMPOSED_PROFILE","Every composed scope must declare the same semantic profile.",path);
        if(const auto *broken=scope.member("wire_fragments");broken&&!arrayAt(broken,path).empty())fail("IR_INCOMPLETE_SOURCE","Broken wires cannot become compiled connections.",path);
        const auto &ns=arrayAt(scope.member("nodes"),path+"/nodes");
        if(ns.size()>options.limits.maximumNodes-rawNodes.size())fail("IR_RESOURCE_LIMIT","Node budget exceeded.",path,true);
        for(std::size_t i=0;i<ns.size();++i) {
            const auto p=path+"/nodes/"+std::to_string(i),id=text(ns[i].member("id"),p);
            if(id.size()>options.limits.maximumSourceIdBytes||!rawNodes.emplace(id,RawNode{&ns[i],p,region,owner}).second)fail("IR_SOURCE_DUPLICATE_ID","Node identities must remain unique across explicit regions.",p);
            if(const auto *regions=ns[i].member("regions")) {
                const auto &rs=arrayAt(regions,p+"/regions");
                for(std::size_t j=0;j<rs.size();++j) {
                    const auto rp=p+"/regions/"+std::to_string(j),sid=text(rs[j].member("id"),rp);
                    fields(rs[j],{"id","match","matches","default","diagram","event","event_data_fields","event_payload_fields","event_response_fields","title","layout","doc","tags"},rp);
                    if(sid.size()>options.limits.maximumSourceIdBytes||out.regions.size()>=1024)fail("IR_RESOURCE_LIMIT","Region identities or count exceed their bounded contract.",rp,true);
                    const auto rid=encodedId("region",id)+":"+encodedId("",sid).substr(1);
                    if(!regionIds.insert(rid).second)fail("IR_REGION_ID","Regions must have distinct owner-qualified identities.",rp);
                    out.regions.push_back({rid,sid,encodedId("node",id),{},"body",rp});
                    index(objectAt(rs[j].member("diagram"),rp+"/diagram"),rp+"/diagram",rid,id,depth+1);
                }
            }
        }
        const auto &es=arrayAt(scope.member("edges"),path+"/edges");
        if(es.size()>options.limits.maximumEdges-rawEdges.size())fail("IR_RESOURCE_LIMIT","Edge budget exceeded.",path,true);
        for(std::size_t i=0;i<es.size();++i) {
            const auto p=path+"/edges/"+std::to_string(i),id=text(es[i].member("id"),p);
            fields(es[i],{"id","from","to","bends","doc","tags"},p);
            if(id.size()>options.limits.maximumSourceIdBytes||!edgeIds.insert(id).second)fail("IR_EDGE_ID","Edge IDs must remain distinct and bounded.",p);
            rawEdges.push_back({&es[i],p,region,owner});
            edgeRegions.emplace(id,region);
        }
    }
    CanonicalIrNode &node(const std::string &id) {const auto i=nodeIndex.find(id);if(i==nodeIndex.end())fail("IR_ENDPOINT_NODE","Endpoint refers to no actual node.",id);return out.nodes[i->second];}
    CanonicalIrPort &port(CanonicalIrNode &n,const std::string &id,bool input,const std::string &p) {
        const auto i=std::find_if(n.ports.begin(),n.ports.end(),[&](const auto &v){return v.id==id&&v.input==input;});
        if(i==n.ports.end())fail("IR_ENDPOINT_PORT","Endpoint refers to no admitted typed port: "+id,p);return *i;
    }
    std::string qualified(const RawEdge &edge,const std::string &sid,const std::string &pid,bool input)const {
        // Separate each Case/Event output writer at the actual region. This
        // avoids multiple SDK drivers while preserving the authored tunnel ID.
        const auto &raw=rawNodes.at(sid);
        if(input&&sid==edge.owner&&pid.starts_with("tunnel:")) {
            const auto *kind=raw.raw->member("structure_type");
            if(kind&&(kind->string=="case"||kind->string=="event_structure"))return pid+"@"+edge.region;
        }
        if(!input&&sid==edge.owner&&pid.starts_with("event_data."))return pid+"@"+edge.region;
        return pid;
    }
    bool numericImplicitReceiver(const CanonicalIrNode &target,const std::string &pid,
        bool *declaredNumericArray=nullptr)const {
        if(declaredNumericArray)*declaredNumericArray=false;
        if(target.family=="widget_value")return target.role=="indicator"&&pid=="value";
        // Structure boundary/state conversions require their own contract;
        // admitting a plain scalar must not widen a Case/tunnel/Feedback rule.
        if(target.family!="primitive"||target.feedback)return false;
        const FunctionCatalogEntry *catalog=findFunctionCatalogEntryByDiagramNodeType(target.authoredKind);
        if(!catalog)catalog=findFunctionCatalogEntry(target.authoredKind);
        if(!catalog)if(const auto *scalar=canonicalScalarNativeContract(target.kind))catalog=findFunctionCatalogEntry(scalar->catalogId);
        if(!catalog&&target.kind==canonicalNumericExactAbsId)catalog=findFunctionCatalogEntry("numeric.absolute_value");
        if(!catalog)return false;
        const auto *definition=isGeometryFunction(target.kind)?geometryBindingDefinition(target.kind):isInterpolationFunction(target.kind)?interpolationBindingDefinition(target.kind):isStatisticsFunction(target.kind)?statisticsBindingDefinition(target.kind):isCalculusFunction(target.kind)?calculusBindingDefinition(target.kind):isFittingFunction(target.kind)?fittingBindingDefinition(target.kind):findFunctionBindingDefinition(catalog->catalogId);
        const auto *declared=definition?findFunctionBindingPort(*definition,canonicalFunctionBindingPortId(catalog->catalogId,pid)):nullptr;
        if(!declared||declared->direction!=FunctionPortDirection::Input)return false;
        const bool numericArray=declared->connectionPolicy==FunctionPortConnectionPolicy::NumericArrayImplicit;
        if(declaredNumericArray)*declaredNumericArray=numericArray;
        // These polymorphic Array data receivers also admit Bool/String/Path/
        // nominal Records. Their AnyExplicit palette policy must stay broad;
        // only their resolved numeric receiver schemas request conversion.
        const auto cid=std::string_view(catalog->catalogId),port=std::string_view(declared->portId);
        const bool commonArrayData=(cid=="array.replace_subset"||cid=="array.insert")?(port=="array"||port=="subset"):
            cid=="array.interleave_1d"?port=="array_a":cid=="array.search_1d"?(port=="array"||port=="value"):
            cid.starts_with("array.matrix.set_")&&(port=="elements"||port=="diagonal_values"||port=="subset");
        return declared->connectionPolicy==FunctionPortConnectionPolicy::NumericImplicit||
             declared->connectionPolicy==FunctionPortConnectionPolicy::NumericScalarOrArrayImplicit||numericArray||commonArrayData;
    }
    static bool plainNumericArray(const binding::TypeRef &type) {
        if(type.kind!=binding::TypeKind::Array||type.rank<1||type.rank>16||type.children.size()!=1)return false;
        const auto &child=type.children.front();
        return child.kind==binding::TypeKind::Primitive&&child.numeric()&&
            binding::fullTypeEqual(child,binding::TypeRef::primitive(child.name))&&
            binding::fullTypeEqual(type,binding::TypeRef::array(child,type.rank));
    }
    static bool scalarErrorRoles(const binding::TypeRef &type) {
        if(!binding::isErrorCluster(type)||type.fieldIds.size()!=3)return false;
        // Known scalar roles do not make ordinary Records interchangeable.
        // Preserve every original ID/schema; reject ambiguous field identity.
        return std::none_of(type.fieldIds.begin(),type.fieldIds.end(),[](const auto &id){return id.empty();})&&
            std::set<std::string>(type.fieldIds.begin(),type.fieldIds.end()).size()==3;
    }
    void scalarErrorBridge(const RawEdge &edge,const RawNode &sourceRaw,const RawNode &sinkRaw,
        const std::string &id,const std::string &sourceNodeId,const std::string &sinkNodeId,
        const std::string &fromId,const std::string &qa,const std::string &toId,const std::string &qb,
        const CanonicalIrPort &from,const CanonicalIrPort &to,
        const binding::TypeRef &sourceType,const binding::TypeRef &targetType) {
        if(out.nodes.size()>options.limits.maximumNodes||options.limits.maximumNodes-out.nodes.size()<2||
            out.connections.size()>options.limits.maximumEdges||options.limits.maximumEdges-out.connections.size()<5)
            fail("IR_RESOURCE_LIMIT","Derived scalar Error role bridges exceed the existing FIR node/edge budget.",edge.path,true);
        const auto selectId=encodedId("node-error-select",id),constructId=encodedId("node-error-construct",id);
        const std::array<std::string,5> wires{encodedId("edge-error-input",id),encodedId("edge-error-role1",id),
            encodedId("edge-error-role2",id),encodedId("edge-error-role3",id),encodedId("edge",id)};
        // Prefix namespaces are disjoint from authored node:/edge: identities
        // and numeric node-coercion:/edge-coercion-input: identities. Raw edge
        // IDs are globally unique and encodedId is injective.
        if(selectId.size()>options.limits.maximumIrIdBytes||constructId.size()>options.limits.maximumIrIdBytes||
            std::any_of(wires.begin(),wires.end(),[&](const auto &wire){return wire.size()>options.limits.maximumIrIdBytes;}))
            fail("IR_RESOURCE_LIMIT","Derived scalar Error bridge identities exceed the existing FIR identity budget.",edge.path,true);
        const auto make=[&](CanonicalObjectReferenceKind kind,const binding::TypeRef &record,
                            const CanonicalIrPort &rail,const std::string &nodeId,bool select) {
            CanonicalIrNode result;result.id=nodeId;result.sourceNodeId=select?sourceNodeId:sinkNodeId;
            result.family="primitive";result.kind=std::string(canonicalObjectReferenceOperation(kind));result.role="data";
            result.regionId=edge.region;result.valueType=rail.typeRef;result.fullTypeJson=rail.fullTypeJson;
            std::vector<CanonicalObjectReferencePort> signature;std::vector<std::vector<std::string>> paths;
            if(select)signature.push_back({"cluster",rail.typeRef,rail.fullTypeJson,true,true,{}});
            for(std::size_t index=0;index<3;++index) {
                const auto portId="element"+std::to_string(index+1);
                signature.push_back({portId,canonicalObjectReferencePhysicalType(record.children[index]).canonical(),
                    encode(canonicalObjectReferenceWriteType(record.children[index])),!select,!select,{}});
                paths.push_back({record.fieldIds[index]});
            }
            if(!select)signature.push_back({"result",rail.typeRef,rail.fullTypeJson,false,false,{}});
            CanonicalObjectReferenceDiagnostic diagnostic;
            result.objectReference=canonicalObjectReferenceRecordDescriptor(kind,record,std::move(signature),std::move(paths),
                nullptr,edge.path+(select?"/from":"/to"),&diagnostic);
            if(!result.objectReference)fail("IR_ERROR_BRIDGE_SIGNATURE",diagnostic.message,edge.path);
            for(const auto &p:result.objectReference->ports)result.ports.push_back({p.id,p.input,p.type,p.fullTypeJson});
            return result;
        };
        auto select=make(CanonicalObjectReferenceKind::Select,sourceType,from,selectId,true);
        auto construct=make(CanonicalObjectReferenceKind::Construct,targetType,to,constructId,false);
        out.connections.push_back({wires[0],id,{fromId,qa},{selectId,"cluster"}});
        for(std::size_t index=0;index<3;++index) {
            const auto role="element"+std::to_string(index+1);
            out.connections.push_back({wires[index+1],id,{selectId,role},{constructId,role}});
        }
        out.connections.push_back({wires[4],id,{constructId,"result"},{toId,qb}});
        map(encodedId("map-error-input",id),"diagram_edge",edge.path,wires[0],"connection","support_only");
        for(std::size_t index=0;index<3;++index)
            map(encodedId("map-error-role"+std::to_string(index+1),id),"diagram_edge",edge.path,wires[index+1],"connection","support_only");
        map(encodedId("map-edge",id),"diagram_edge",edge.path,wires[4],"connection");
        map(encodedId("map-error-select-edge",id),"diagram_edge",edge.path,selectId,"support_object","support_only");
        map(encodedId("map-error-select-source",id),"diagram_node",sourceRaw.path,selectId,"support_object","support_only");
        map(encodedId("map-error-construct-edge",id),"diagram_edge",edge.path,constructId,"support_object","support_only");
        map(encodedId("map-error-construct-receiver",id),"diagram_node",sinkRaw.path,constructId,"support_object","support_only");
        if(!edge.region.empty()) {
            const auto region=std::find_if(out.regions.begin(),out.regions.end(),[&](const auto &r){return r.id==edge.region;});
            if(region==out.regions.end())fail("IR_ERROR_BRIDGE_SCOPE","The actual scalar Error wire region disappeared.",edge.path);
            region->memberNodeIds.push_back(selectId);region->memberNodeIds.push_back(constructId);
        }
        // Preserve endpoint-specific Source/port attribution through FIR.
        // Neither descriptor owns an authored substitute node.
        errorBridgeOrigins.emplace(selectId,ErrorBridgeOrigin{&edge,true});
        errorBridgeOrigins.emplace(constructId,ErrorBridgeOrigin{&edge,false});
        out.nodes.push_back(std::move(select));out.nodes.push_back(std::move(construct));
    }
    void nodes() {
        for(const auto &[sid,raw]:rawNodes) {
            const auto &v=*raw.raw;const auto &p=raw.path;CanonicalIrNode n;
            fields(v,{"id","kind","type","value","constant_profile","studio_type_schema","studio_ports","studio_authored_type","studio_configuration","studio_event","studio_in_place","studio_timed_loop","studio_sequence","studio_collection","studio_feedback","widget","value_type","layout","structure_type","boundary","structure_terminals","structure_tunnels","regions","case_profile","doc","tags","native_call_profile","native_call_configuration","selected_fields_version","selected_fields",canonicalObjectReferenceAttribute},p);
            n.sourceNodeId=sid;n.id=encodedId("node",sid);n.regionId=raw.region;n.family=text(v.member("kind"),p);
            if(v.member("studio_sequence")&&(n.family!="structure"||!v.member("structure_type")||v.member("structure_type")->string!="sequence_structure"))fail("IR_SEQUENCE_OWNER","Sequence semantics belong only to an actual Sequence structure.",p);
            if(v.member("studio_collection")&&n.family!="primitive")fail("IR_COLLECTION_OWNER","Collection operation configuration belongs only to primitive Collection operations.",p);
            if(v.member("studio_feedback")&&n.family!="primitive")fail("IR_FEEDBACK_OWNER","Feedback state configuration belongs only to a primitive Feedback Node.",p);
            if((v.member("native_call_profile")||v.member("native_call_configuration"))&&
                (n.family!="primitive"||!v.member("type")||v.member("type")->string!=kNativeCallScalarOperationId))
                fail("IR_NATIVE_CALL_OWNER","The complete NativeCall configuration belongs only to its declared scalar5 operator.",p);
            if((v.member("selected_fields")||v.member("selected_fields_version"))&&
                (n.family!="primitive"||!v.member("type")||v.member("type")->string!="frog.core.unbundle.by_name"))
                fail("IR_RECORD_SELECTION_OWNER","Selected fields belong only to their declared immutable Record selector.",p);
            if(const auto *type=v.member("studio_authored_type"))n.authoredKind=text(type,p);
            if(const auto *config=v.member("studio_configuration"))n.configurationJson=encode(*config);
            if(const auto *schema=v.member("studio_type_schema"))n.fullTypeJson=encode(*schema);
            std::set<std::pair<std::string,bool>> unique;
            for(const auto &entry:arrayAt(v.member("studio_ports"),p+"/studio_ports")) {
                fields(entry,{"id","direction","type","studio_type_schema"},p+"/studio_ports");
                const auto id=text(entry.member("id"),p),direction=text(entry.member("direction"),p);
                if(direction!="input"&&direction!="output")fail("IR_PORT_DIRECTION","Port direction is not declared.",p);
                const auto schema=canonicalObjectReferenceReadType(objectAt(entry.member("studio_type_schema"),p));
                if(!schema||!schema->known())fail("IR_PORT_TYPE","Port needs a complete admitted source type schema.",p);
                const auto physical=canonicalObjectReferencePhysicalType(*schema).canonical();
                if(physical!=text(entry.member("type"),p))fail("IR_PORT_TYPE","Physical port type differs from its sealed nominal representation.",p);
                if(id.size()>options.limits.maximumSourceIdBytes||id.find('@')!=std::string::npos||!unique.emplace(id,direction=="input").second)fail("IR_PORT_ID","Source connector identities must be distinct, bounded and cannot use the reserved physical-region separator.",p);
                n.ports.push_back({id,direction=="input",physical,encode(*entry.member("studio_type_schema"))});
            }
            if(n.family=="constant") {
                if(text(v.member("constant_profile"),p)!=composedSourceProfile)fail("IR_CONSTANT_PROFILE","A composed constant requires its explicit profile.",p);
                n.kind="frog.core.constant";n.literalProfile=std::string(composedSourceProfile);
                n.valueType=text(v.member("type"),p);const auto *literal=v.member("value");
                if(!literal)fail("IR_CONSTANT_VALUE","A constant requires its actual exact typed value.",p);n.literalValueJson=encode(*literal);
                if(n.ports.size()!=1||n.ports[0].input||n.ports[0].typeRef!=n.valueType)fail("IR_CONSTANT_SCHEMA","A constant must retain its exact single producer port.",p);
                if(n.fullTypeJson.empty())n.fullTypeJson=n.ports[0].fullTypeJson;
                if(!exact(parse(n.fullTypeJson),parse(n.ports[0].fullTypeJson)))fail("IR_CONSTANT_SCHEMA","The literal and its actual producer port must preserve the same complete nominal schema.",p);
            } else if(n.family=="widget_value") {
                n.kind=n.family;n.widgetId=text(v.member("widget"),p);
                const auto w=widgets.find(n.widgetId);if(w==widgets.end())fail("IR_WIDGET_MISMATCH","Terminal refers to no actual widget.",p);
                const auto &widget=*w->second;n.role=text(widget.member("role"),p);n.widgetClass=text(widget.member("class_ref"),p);
                n.valueType=text(widget.member("value_type"),p);n.fullTypeJson=encode(objectAt(widget.member("studio_type_schema"),p));
                const auto &props=objectAt(widget.member("props"),p);if(const auto *value=props.member("default_value"))n.defaultValueJson=encode(*value);
                if(const auto *privateWidget=widget.member("studio_widget"))if(const auto *label=privateWidget->member("label"))n.widgetLabel=text(label,p,true);
                if(n.ports.size()!=1||n.ports[0].id!="value"||n.ports[0].input!=(n.role=="indicator")||n.ports[0].typeRef!=n.valueType)fail("IR_WIDGET_PORT","Widget terminal role/type differs from its declaration.",p);
                if(!exact(parse(n.fullTypeJson),parse(n.ports[0].fullTypeJson)))fail("IR_WIDGET_PORT","The terminal and its actual widget must preserve the same complete nominal schema.",p);
                map(encodedId("map-widget",n.widgetId),"widget_instance",widgetPaths.at(n.widgetId),n.id);
            } else if(n.family=="primitive") {
                n.kind=text(v.member("type"),p);
                if(n.kind==canonicalNumericCoercionId)fail("IR_COERCION_OWNER","Numeric connector coercion is derived from a real admitted edge, not an authored substitute node.",p);
                if(!canonicalNumericIeeeConstantMigrationIdentity(n.kind).empty()) {
                    std::vector<CanonicalNumericAggregatePort> signature;for(const auto &actual:n.ports){const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));if(!type)fail("IR_FIXED_CONSTANT_SCHEMA","An IEEE constant lost its exact Source type.",p);signature.push_back({actual.id,*type,actual.input,true});}
                    std::string why,terminal;if(!canonicalNumericIeeeConstantSignatureValid(n.kind,signature,&why,&terminal))fail("IR_FIXED_CONSTANT_SIGNATURE",why,p+"/"+terminal);
                }
                if(comparison_source::contracts::supports(n.kind)) {
                    std::vector<CanonicalNumericAggregatePort> signature;
                    for(const auto &actual:n.ports) {
                        const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!type)fail("IR_COMPARISON_SCHEMA","A Comparison function lost its complete Source schema.",p);
                        signature.push_back({actual.id,*type,actual.input,true});
                    }
                    std::string why,terminal;
                    if(!canonicalComparisonTreeSignatureValid(n.kind,signature,&why,&terminal))
                        fail("IR_COMPARISON_SIGNATURE",why,p+(terminal.empty()?"":"/"+terminal));
                }
                if(boolean_source::contracts::supports(n.kind)) {
                    std::vector<CanonicalNumericAggregatePort> signature;
                    for(const auto &actual:n.ports) {
                        const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!type)fail("IR_BOOLEAN_SCHEMA","A logical function lost its complete Source schema.",p);
                        signature.push_back({actual.id,*type,actual.input,true});
                    }
                    std::string why,terminal;
                    if(!canonicalBooleanTreeSignatureValid(n.kind,signature,&why,&terminal))fail("IR_BOOLEAN_SIGNATURE",why,p+(terminal.empty()?"":"/"+terminal));
                }
                if(!canonicalNumericBooleanConversionMigrationIdentity(n.kind).empty()) {
                    std::vector<CanonicalNumericAggregatePort> signature;for(const auto &actual:n.ports){const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));if(!type)fail("IR_NUMERIC_BOOLEAN_SCHEMA","Boolean conversion lost its complete type.",p);signature.push_back({actual.id,*type,actual.input,true});}
                    std::string why,terminal;if(!canonicalNumericBooleanConversionSignatureValid(n.kind,signature,&why,&terminal))fail("IR_NUMERIC_BOOLEAN_SIGNATURE",why,p+"/"+terminal);
                }
                if(!canonicalNumericTextMigrationIdentity(n.kind).empty()) {
                    std::vector<CanonicalNumericAggregatePort> signature;for(const auto &actual:n.ports){const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));if(!type)fail("IR_NUMERIC_TEXT_SCHEMA","Numeric text lost its complete Source type.",p);signature.push_back({actual.id,*type,actual.input,true});}
                    std::string why,terminal;if(!canonicalNumericTextSignatureValid(n.kind,signature,&why,&terminal))fail("IR_NUMERIC_TEXT_SIGNATURE",why,p+"/"+terminal);
                }
                if(!canonicalNumericBitReinterpretationMigrationIdentity(n.kind).empty()) {
                    std::vector<CanonicalNumericAggregatePort> signature;for(const auto &actual:n.ports){const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));if(!type)fail("IR_NUMERIC_BIT_REINTERPRETATION_SCHEMA","Bit reinterpretation lost its complete Source type.",p);signature.push_back({actual.id,*type,actual.input,true});}
                    std::string why,terminal;if(!canonicalNumericBitReinterpretationSignatureValid(n.kind,signature,&why,&terminal))fail("IR_NUMERIC_BIT_REINTERPRETATION_SIGNATURE",why,p+"/"+terminal);
                }
                if(!canonicalNumericCalibrationMigrationIdentity(n.kind).empty()) {
                    std::vector<CanonicalNumericAggregatePort> signature;for(const auto &actual:n.ports){const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));if(!type)fail("IR_NUMERIC_CALIBRATION_SCHEMA","Calibration lost its complete Source type.",p);signature.push_back({actual.id,*type,actual.input,true});}
                    std::string why,terminal;if(!canonicalNumericCalibrationSignatureValid(n.kind,signature,&why,&terminal))fail("IR_NUMERIC_CALIBRATION_SIGNATURE",why,p+"/"+terminal);
                }
                if(n.kind==canonicalNumericAccumulateId) {
                    std::vector<CanonicalNumericAggregatePort> signature;
                    for(const auto &actual:n.ports){const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!type)fail("IR_NUMERIC_ACCUMULATE_SCHEMA","Multiply Accumulate lost its complete plain scalar type.",p);
                        signature.push_back({actual.id,*type,actual.input,true});}
                    std::string why,terminal;if(!canonicalNumericAccumulateSignatureValid(n.kind,signature,&why,&terminal))
                        fail("IR_NUMERIC_ACCUMULATE_SIGNATURE",why,p+"/"+terminal);
                }
                if(canonicalNumericRandomOperation(n.kind)) {
                    std::vector<CanonicalNumericAggregatePort> signature;
                    for(const auto &actual:n.ports){const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));if(!type)fail("IR_NUMERIC_RANDOM_SCHEMA","Random lost its complete plain type.",p);signature.push_back({actual.id,*type,actual.input,true});}
                    std::string why,terminal;if(!canonicalNumericRandomSignatureValid(n.kind,signature,&why,&terminal))fail("IR_NUMERIC_RANDOM_SIGNATURE",why,p+"/"+terminal);
                }
                if(!canonicalNumericArrayReductionMigrationIdentity(n.kind).empty()) {
                    std::vector<CanonicalNumericAggregatePort> signature;for(const auto &actual:n.ports){const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));if(!type)fail("IR_NUMERIC_REDUCTION_SCHEMA","Reduction lost its complete type.",p);signature.push_back({actual.id,*type,actual.input,true});}
                    std::string why,terminal;if(!canonicalNumericArrayReductionSignatureValid(n.kind,signature,&why,&terminal))fail("IR_NUMERIC_REDUCTION_SIGNATURE",why,p+"/"+terminal);
                }
                if(canonicalNumericComplexContract(n.kind)) {
                    std::vector<CanonicalNumericAggregatePort> signature;
                    for(const auto &actual:n.ports){const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));if(!type)fail("IR_NUMERIC_COMPLEX_SCHEMA","Complex operation lost its complete type schema.",p);signature.push_back({actual.id,*type,actual.input,true});}
                    std::string why;if(!canonicalNumericComplexSignatureValid(n.kind,signature,&why))fail("IR_NUMERIC_COMPLEX_SIGNATURE",why,p);
                }
                if(runtime::numeric_aggregate::scalar_palette::supports(n.kind)||n.kind==canonicalNumericAggregateAddId) {
                    std::vector<CanonicalNumericAggregatePort> signature;
                    for(const auto &actual:n.ports) {
                        const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!type)fail("IR_NUMERIC_PALETTE_SIGNATURE","Numeric lost its complete Source schema.",p);
                        signature.push_back({actual.id,*type,actual.input,true});
                    }
                    std::string why,at;
                    if(!canonicalNumericScalarPaletteSignatureValid(n.kind,signature,&why,&at))
                        fail("IR_NUMERIC_PALETTE_SIGNATURE",why,p+(at.empty()?"":"/"+at));
                }
                if(n.kind==canonicalNumericExactAbsId||!canonicalNumericConversionTarget(n.kind).empty()) {
                    std::vector<CanonicalNumericAggregatePort> signature;
                    for(const auto &actual:n.ports) {
                        const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!type)fail("IR_NUMERIC_SIGNATURE","The numeric operation lost its complete primitive schema.",p);
                        signature.push_back({actual.id,*type,actual.input,true});
                    }
                    std::string why,at;
                    if(!canonicalNumericAggregateSignatureValid(n.kind,signature,&why,&at))
                        fail(n.kind==canonicalNumericExactAbsId?"IR_NUMERIC_EXACT_ABS_SIGNATURE":"IR_NUMERIC_CONVERSION_SIGNATURE",why,p+(at.empty()?"":"/"+at));
                }
                if(n.kind==kNativeCallScalarOperationId) {
                    if(!raw.region.empty())fail("IR_NATIVE_CALL_REGION","The scalar5 NativeCall corridor requires an actual flat root region.",p,true);
                    if(text(v.member("native_call_profile"),p)!=kNativeCallScalarProfile)
                        fail("IR_NATIVE_CALL_PROFILE","NativeCall requires its exact local scalar5 profile.",p);
                    CanonicalNativeCallDiagnostic diagnostic;
                    n.nativeCall=canonicalNativeCallScalarDescriptor(objectAt(v.member("native_call_configuration"),p+"/native_call_configuration"),
                        p+"/native_call_configuration",&diagnostic);
                    if(!n.nativeCall)fail(diagnostic.code,diagnostic.message,p,diagnostic.unsupported);
                    if(n.ports.size()!=n.nativeCall->ports.size())fail("IR_NATIVE_CALL_SIGNATURE","NativeCall lost a connector from its complete derived signature.",p);
                    for(std::size_t i=0;i<n.ports.size();++i) {
                        const auto &actual=n.ports[i];const auto &declared=n.nativeCall->ports[i];
                        const auto expected=binding::parseType(declared.type);const auto schema=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(actual.id!=declared.id||actual.input!=declared.input||actual.typeRef!=declared.type||!expected.valid()||!schema||*schema!=expected.type)
                            fail("IR_NATIVE_CALL_SIGNATURE","NativeCall connectors must retain their exact IDs, types and ABI-derived order.",p);
                    }
                }
                if(const auto *geometry=findGeometryFunction(n.kind)) {
                    std::vector<GeometryTypeEvidence> evidence;
                    const auto inputs=geometryInputPortIds(n.kind);
                    const auto outputs=geometryOutputPortIds(n.kind);
                    if(n.ports.size()!=inputs.size()+outputs.size())
                        fail("IR_GEOMETRY_SIGNATURE","GEOMETRY must preserve every declared input and output.",p);
                    for(const auto &actual:n.ports) {
                        const auto ids=actual.input?inputs:outputs;
                        if(std::find(ids.begin(),ids.end(),actual.id)==ids.end())
                            fail("IR_GEOMETRY_PORT","GEOMETRY contains an undeclared or reversed connector.",p+"/ports/"+actual.id);
                        if(actual.input) {
                            const auto type=binding::parseType(actual.typeRef);
                            if(!type.valid())fail("IR_GEOMETRY_TYPE","GEOMETRY lost its complete numeric input type.",p+"/ports/"+actual.id);
                            evidence.push_back({actual.id,type.type});
                        }
                    }
                    for(const auto &actual:n.ports) {
                        const auto expected=resolveGeometryPort(n.kind,actual.id,evidence);
                        if(!expected.resolution.accepted()||expected.resolution.type.canonical()!=actual.typeRef)
                            fail("IR_GEOMETRY_TYPE","GEOMETRY connector differs from the shared resolved contract.",p+"/ports/"+actual.id);
                    }
                }
                if(const auto *interpolation=findInterpolationFunction(n.kind)) {
                    std::vector<InterpolationTypeEvidence> evidence;
                    const auto inputs=interpolationInputPortIds(n.kind);
                    const auto outputs=interpolationOutputPortIds(n.kind);
                    if(n.ports.size()!=inputs.size()+outputs.size())
                        fail("IR_INTERPOLATION_SIGNATURE","INTERPOLATION must preserve every declared input and output.",p);
                    for(const auto &actual:n.ports) {
                        const auto ids=actual.input?inputs:outputs;
                        if(std::find(ids.begin(),ids.end(),actual.id)==ids.end())
                            fail("IR_INTERPOLATION_PORT","INTERPOLATION contains an undeclared or reversed connector.",p+"/ports/"+actual.id);
                        if(actual.input) {
                            const auto type=binding::parseType(actual.typeRef);
                            if(!type.valid())fail("IR_INTERPOLATION_TYPE","INTERPOLATION lost its complete numeric input type.",p+"/ports/"+actual.id);
                            evidence.push_back({actual.id,type.type});
                        }
                    }
                    for(const auto &actual:n.ports) {
                        const auto expected=resolveInterpolationPort(n.kind,actual.id,evidence);
                        if(!expected.resolution.accepted()||expected.resolution.type.canonical()!=actual.typeRef)
                            fail("IR_INTERPOLATION_TYPE","INTERPOLATION connector differs from the shared resolved contract.",p+"/ports/"+actual.id);
                    }
                }
                if(const auto *statistics=findStatisticsFunction(n.kind)) {
                    std::vector<StatisticsTypeEvidence> evidence;
                    if(isStatisticsExpressFunction(n.kind)&&std::none_of(n.ports.begin(),n.ports.end(),[](const auto &port){return !port.input&&port.id=="error_out";}))
                        fail("IR_STATISTICS_SIGNATURE","Statistics Express must retain its diagnostic output.",p+"/ports/error_out");
                    const auto inputs=statisticsInputPortIds(n.kind);
                    const auto outputs=statisticsOutputPortIds(n.kind);
                    if((!isStatisticsExpressFunction(n.kind)&&n.ports.size()!=inputs.size()+outputs.size())||(isStatisticsExpressFunction(n.kind)&&(n.ports.size()<inputs.size()+2||n.ports.size()>inputs.size()+outputs.size())))
                        fail("IR_STATISTICS_SIGNATURE","STATISTICS must preserve every declared input and output.",p);
                    for(const auto &actual:n.ports) {
                        const auto ids=actual.input?inputs:outputs;
                        if(std::find(ids.begin(),ids.end(),actual.id)==ids.end())
                            fail("IR_STATISTICS_PORT","STATISTICS contains an undeclared or reversed connector.",p+"/ports/"+actual.id);
                        if(actual.input) {
                            const auto type=binding::parseType(actual.typeRef);
                            if(!type.valid())fail("IR_STATISTICS_TYPE","STATISTICS lost its complete numeric input type.",p+"/ports/"+actual.id);
                            evidence.push_back({actual.id,type.type});
                        }
                    }
                    for(const auto &actual:n.ports) {
                        const auto expected=resolveStatisticsPort(n.kind,actual.id,evidence);
                        if(!expected.resolution.accepted()||expected.resolution.type.canonical()!=actual.typeRef)
                            fail("IR_STATISTICS_TYPE","STATISTICS connector differs from the shared resolved contract.",p+"/ports/"+actual.id);
                    }
                }
                if(!canonicalDiscreteArrayMigrationIdentity(n.kind).empty()||canonicalDifferentialDefinition(n.kind)||optimizationDefinition(n.kind)) {
                    std::vector<CanonicalNumericAggregatePort> signature;
                    for(const auto &actual:n.ports) {
                        const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!type)fail("IR_DIFFERENTIAL_SCHEMA","Differential connector lost its complete Source schema.",p+"/ports/"+actual.id);
                        signature.push_back({actual.id,*type,actual.input,true});
                    }
                    std::string why,at;
                    if(!(optimizationDefinition(n.kind)?optimizationSignatureValid(n.kind,signature,&why,&at):!canonicalDiscreteArrayMigrationIdentity(n.kind).empty()?canonicalDiscreteArraySignatureValid(n.kind,signature,&why,&at):canonicalDifferentialSignatureValid(n.kind,signature,&why,&at)))
                        fail("IR_DIFFERENTIAL_SIGNATURE",why,p+"/ports/"+at);
                }
                if(const auto *calculus=findCalculusFunction(n.kind)) {
                    std::vector<CalculusTypeEvidence> evidence;
                    const auto inputs=calculusInputPortIds(n.kind);
                    const auto outputs=calculusOutputPortIds(n.kind);
                    if(n.ports.size()!=inputs.size()+outputs.size())
                        fail("IR_CALCULUS_SIGNATURE","CALCULUS must preserve every declared input and output.",p);
                    for(const auto &actual:n.ports) {
                        const auto ids=actual.input?inputs:outputs;
                        if(std::find(ids.begin(),ids.end(),actual.id)==ids.end())
                            fail("IR_CALCULUS_PORT","CALCULUS contains an undeclared or reversed connector.",p+"/ports/"+actual.id);
                        if(actual.input) {
                            const auto type=binding::parseType(actual.typeRef);
                            if(!type.valid())fail("IR_CALCULUS_TYPE","CALCULUS lost its complete numeric input type.",p+"/ports/"+actual.id);
                            evidence.push_back({actual.id,type.type});
                        }
                    }
                    for(const auto &actual:n.ports) {
                        const auto expected=resolveCalculusPort(n.kind,actual.id,evidence);
                        if(!expected.resolution.accepted()||expected.resolution.type.canonical()!=actual.typeRef)
                            fail("IR_CALCULUS_TYPE","CALCULUS connector differs from the shared resolved contract.",p+"/ports/"+actual.id);
                    }
                }
                if(const auto *fitting=findFittingFunction(n.kind)) {
                    std::vector<FittingTypeEvidence> evidence;
                    const auto inputs=fittingInputPortIds(n.kind);
                    const auto outputs=fittingOutputPortIds(n.kind);
                    if(n.ports.size()!=inputs.size()+outputs.size())
                        fail("IR_FITTING_SIGNATURE","FITTING must preserve every declared input and output.",p);
                    for(const auto &actual:n.ports) {
                        const auto ids=actual.input?inputs:outputs;
                        if(std::find(ids.begin(),ids.end(),actual.id)==ids.end())
                            fail("IR_FITTING_PORT","FITTING contains an undeclared or reversed connector.",p+"/ports/"+actual.id);
                        if(actual.input) {
                            const auto type=binding::parseType(actual.typeRef);
                            if(!type.valid())fail("IR_FITTING_TYPE","FITTING lost its complete numeric input type.",p+"/ports/"+actual.id);
                            evidence.push_back({actual.id,type.type});
                        }
                    }
                    for(const auto &actual:n.ports) {
                        const auto expected=resolveFittingPort(n.kind,actual.id,evidence);
                        if(!expected.resolution.accepted()||expected.resolution.type.canonical()!=actual.typeRef)
                            fail("IR_FITTING_TYPE","FITTING connector differs from the shared resolved contract.",p+"/ports/"+actual.id);
                    }
                }
                if(const auto *blas=findBlasFunction(n.kind)) {
                    std::vector<BlasTypeEvidence> evidence;
                    const auto inputs=blasInputPortIds(blas->catalogId);
                    const auto outputs=blasOutputPortIds(blas->catalogId);
                    if(n.ports.size()!=inputs.size()+outputs.size())
                        fail("IR_BLAS_SIGNATURE","BLAS must preserve every declared input and output.",p);
                    for(const auto &actual:n.ports) {
                        const auto ids=actual.input?inputs:outputs;
                        if(std::find(ids.begin(),ids.end(),actual.id)==ids.end())
                            fail("IR_BLAS_PORT","BLAS contains an undeclared or reversed connector.",p+"/ports/"+actual.id);
                        if(actual.input) {
                            const auto type=binding::parseType(actual.typeRef);
                            if(!type.valid())fail("IR_BLAS_TYPE","BLAS lost its complete numeric input type.",p+"/ports/"+actual.id);
                            evidence.push_back({actual.id,type.type});
                        }
                    }
                    for(const auto &actual:n.ports) {
                        const auto expected=resolveBlasPort(blas->catalogId,actual.id,evidence);
                        if(!expected.resolution.accepted()||expected.resolution.type.canonical()!=actual.typeRef)
                            fail("IR_BLAS_TYPE","BLAS connector differs from the shared resolved contract.",p+"/ports/"+actual.id);
                    }
                }
                if(const auto *linear=findPolynomialFunction(n.kind)) {
                    std::vector<PolynomialTypeEvidence> evidence;
                    const auto inputs=polynomialInputPortIds(linear->catalogId);
                    const auto outputs=polynomialOutputPortIds(linear->catalogId);
                    if(n.ports.size()!=inputs.size()+outputs.size())
                        fail("IR_POLYNOMIAL_SIGNATURE","Polynomial must preserve every declared input and output.",p);
                    for(const auto &actual:n.ports) {
                        const auto ids=actual.input?inputs:outputs;
                        if(std::find(ids.begin(),ids.end(),actual.id)==ids.end())
                            fail("IR_POLYNOMIAL_PORT","Polynomial contains an undeclared or reversed connector.",p+"/ports/"+actual.id);
                        if(actual.input) {
                            const auto type=binding::parseType(actual.typeRef);
                            if(!type.valid())fail("IR_POLYNOMIAL_TYPE","Polynomial lost its complete numeric input type.",p+"/ports/"+actual.id);
                            evidence.push_back({actual.id,type.type});
                        }
                    }
                    for(const auto &actual:n.ports) {
                        const auto expected=resolvePolynomialPort(linear->catalogId,actual.id,evidence);
                        if(!expected.resolution.accepted()||expected.resolution.type.canonical()!=actual.typeRef)
                            fail("IR_POLYNOMIAL_TYPE","Polynomial connector differs from the shared resolved contract.",p+"/ports/"+actual.id);
                    }
                }
                if(const auto *linear=findLinearAlgebraFunction(n.kind)) {
                    std::vector<LinearAlgebraTypeEvidence> evidence;
                    const auto inputs=linearAlgebraInputPortIds(linear->catalogId);
                    const auto outputs=linearAlgebraOutputPortIds(linear->catalogId);
                    if(n.ports.size()!=inputs.size()+outputs.size())
                        fail("IR_LINEAR_ALGEBRA_SIGNATURE","Linear Algebra must preserve every declared input and output.",p);
                    for(const auto &actual:n.ports) {
                        const auto ids=actual.input?inputs:outputs;
                        if(std::find(ids.begin(),ids.end(),actual.id)==ids.end())
                            fail("IR_LINEAR_ALGEBRA_PORT","Linear Algebra contains an undeclared or reversed connector.",p+"/ports/"+actual.id);
                        if(actual.input) {
                            const auto type=binding::parseType(actual.typeRef);
                            if(!type.valid())fail("IR_LINEAR_ALGEBRA_TYPE","Linear Algebra lost its complete numeric input type.",p+"/ports/"+actual.id);
                            evidence.push_back({actual.id,type.type});
                        }
                    }
                    for(const auto &actual:n.ports) {
                        const auto expected=resolveLinearAlgebraPort(linear->catalogId,actual.id,evidence);
                        if(!expected.resolution.accepted()||expected.resolution.type.canonical()!=actual.typeRef)
                            fail("IR_LINEAR_ALGEBRA_TYPE","Linear Algebra connector differs from the shared resolved contract.",p+"/ports/"+actual.id);
                    }
                }
                if(n.kind=="frog.core.unbundle.by_name") {
                    const auto *version=v.member("selected_fields_version");
                    if(!version||version->type!=Json::Type::Number||version->string!="1")fail("IR_RECORD_SELECTION_VERSION","Record selection requires explicit stable field IDs version1.",p);
                    const auto &input=port(n,"cluster",true,p);const auto type=canonicalObjectReferenceReadType(parse(input.fullTypeJson));
                    if(!type||type->canonical()!=kNativeCallScalarErrorType)fail("IR_RECORD_SELECTION_SCHEMA","This composed immutable selector requires the complete scalar5 Error schema.",p);
                    std::set<std::string> selectedPorts;
                    for(const auto &selected:arrayAt(v.member("selected_fields"),p)) {
                        fields(selected,{"port","field_id"},p);const auto id=text(selected.member("port"),p),field=text(selected.member("field_id"),p);
                        const auto fieldIt=std::find(type->fieldIds.begin(),type->fieldIds.end(),field);
                        if(fieldIt==type->fieldIds.end()||!selectedPorts.insert(id).second)fail("IR_RECORD_SELECTION_FIELD","Record selection requires distinct actual output IDs and existing fields.",p);
                        const auto &output=port(n,id,false,p);const auto fieldType=type->children[static_cast<std::size_t>(fieldIt-type->fieldIds.begin())];
                        const auto outputSchema=canonicalObjectReferenceReadType(parse(output.fullTypeJson));
                        if(output.typeRef!=canonicalObjectReferencePhysicalType(fieldType).canonical()||!outputSchema||*outputSchema!=fieldType)fail("IR_RECORD_SELECTION_TYPE","Record output must retain the exact selected field type.",p);
                        n.selectedFields.push_back(field);
                    }
                    if(n.selectedFields.empty()||n.ports.size()!=n.selectedFields.size()+1)fail("IR_RECORD_SELECTION_SIGNATURE","Record selection lost its real input or selected outputs.",p);
                    std::size_t index=0;for(const auto &actual:n.ports)if(!actual.input) {
                        const auto &entry=v.member("selected_fields")->array[index++];
                        if(actual.id!=text(entry.member("port"),p))fail("IR_RECORD_SELECTION_ORDER","Stable selected fields must follow the actual output-port order.",p);
                    }
                }
                if(n.kind==canonicalScalePowerTwoId) {
                    if(n.ports.size()!=3)fail("IR_SCALE_POWER_TWO_SIGNATURE","The local Scale By Power Of 2 profile has exactly x,n,result.",p);
                    for(const auto &[id,input]:std::array<std::pair<std::string,bool>,3>{{{"x",true},{"n",true},{"result",false}}}) {
                        const auto &actual=port(n,id,input,p);
                        const auto schema=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(actual.typeRef!="f64"||!schema||schema->canonical()!="f64"||
                           !canonicalObjectReferenceFullTypeEqual(*schema,binding::parseType("f64").type))
                            fail("IR_SCALE_POWER_TWO_SIGNATURE","The local Scale By Power Of 2 profile requires plain F64 connectors without promotion.",p);
                    }
                }
                if(const auto *exponential=canonicalExponentialContract(n.kind)) {
                    const std::vector<std::pair<std::string,bool>> signature=exponential->binary?
                        std::vector<std::pair<std::string,bool>>{{"y",true},{"x",true},{"result",false}}:
                        std::vector<std::pair<std::string,bool>>{{"x",true},{"result",false}};
                    if(n.ports.size()!=signature.size())
                        fail("IR_EXPONENTIAL_SIGNATURE","The composed Exponential profile has its exact existing scalar signature.",p);
                    for(std::size_t i=0;i<signature.size();++i) {
                        const auto &[id,input]=signature[i];const auto &actual=port(n,id,input,p);
                        const auto schema=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(n.ports[i].id!=id||n.ports[i].input!=input||actual.typeRef!="f64"||!schema||
                           !canonicalObjectReferenceFullTypeEqual(*schema,binding::TypeRef::primitive("f64")))
                            fail("IR_EXPONENTIAL_SIGNATURE","The composed Exponential profile requires ordered plain F64 connectors without promotion.",p);
                    }
                }
                if(n.kind==canonicalArrayBuildOwnedId||n.kind==canonicalArrayConcatenateOwnedId) {
                    const auto &configuration=objectAt(v.member("studio_configuration"),p);
                    const auto *count=configuration.member("build_array_inputs"),*concat=configuration.member("build_array_concatenate");
                    if(n.ports.size()<2||n.ports.size()>33||!count||count->type!=Json::Type::Number||
                       count->string!=std::to_string(n.ports.size()-1)||!concat||concat->type!=Json::Type::Boolean)
                        fail("IR_BUILD_ARRAY_CONFIGURATION","Build Array must retain its exact authored arity and mode.",p);
                    const auto array=canonicalObjectReferenceReadType(parse(port(n,"result",false,p).fullTypeJson));
                    if(!array||array->kind!=binding::TypeKind::Array||array->children.size()!=1||n.ports.back().id!="result"||n.ports.back().input)
                        fail("IR_BUILD_ARRAY_SIGNATURE","Build Array requires an ordered complete Array result.",p);
                    std::vector<int> ranks;
                    for(unsigned index=0;index<n.ports.size()-1;++index) {
                        const auto &input=n.ports[index];
                        const auto schema=canonicalObjectReferenceReadType(parse(input.fullTypeJson));
                        if(!input.input||input.id!=buildArrayInputId(index)||!schema)
                            fail("IR_BUILD_ARRAY_SIGNATURE","Build Array inputs must preserve their semantic order.",p);
                        ranks.push_back(schema->kind==binding::TypeKind::Array?schema->rank:0);
                        const auto &element=schema->kind==binding::TypeKind::Array&&schema->children.size()==1?schema->children.front():*schema;
                        if(!canonicalObjectReferenceFullTypeEqual(element,array->children.front()))
                            fail("IR_BUILD_ARRAY_SIGNATURE","Build Array input and result element schemas differ.",p);
                    }
                    const auto mode=buildArrayMode(concat->boolean,ranks);
                    if(mode.invalidRanks||array->rank!=mode.outputRank||mode.concatenate!=(n.kind==canonicalArrayConcatenateOwnedId))
                        fail("IR_BUILD_ARRAY_SIGNATURE","Build Array signature does not match its effective construction mode.",p);
                }
                if(n.kind=="frog.studio.array.build.notifier1d.v1"||n.kind=="frog.studio.array.build.rendezvous1d.v1"||n.kind=="frog.studio.array.build.occurrence1d.v1") {
                    const bool rendezvousArray=n.kind=="frog.studio.array.build.rendezvous1d.v1";
                    const bool occurrenceArray=n.kind=="frog.studio.array.build.occurrence1d.v1";
                    const auto signatureCode=occurrenceArray?"IR_OCCURRENCE_ARRAY_SIGNATURE":rendezvousArray?"IR_RENDEZVOUS_ARRAY_SIGNATURE":"IR_NOTIFIER_ARRAY_SIGNATURE";
                    const auto &output=port(n,"result",false,p);
                    const auto array=canonicalObjectReferenceReadType(parse(output.fullTypeJson));
                    if(!array||array->kind!=binding::TypeKind::Array||array->rank!=1||array->children.size()!=1||
                        (occurrenceArray?!isOccurrenceReference(array->children.front()):rendezvousArray?!isRendezvousReference(array->children.front()):!notifierReferenceElementType(array->children.front()))||n.ports.size()<2||n.ports.size()>33)
                        fail(signatureCode,"Build Array requires 1..32 complete references of its declared service family and a rank-one result.",p);
                    const auto &configuration=objectAt(v.member("studio_configuration"),p);
                    const auto *count=configuration.member("build_array_inputs"),*concat=configuration.member("build_array_concatenate");
                    if(!count||count->type!=Json::Type::Number||count->string!=std::to_string(n.ports.size()-1)||!concat||concat->type!=Json::Type::Boolean||concat->boolean)
                        fail(occurrenceArray?"IR_OCCURRENCE_ARRAY_CONFIGURATION":rendezvousArray?"IR_RENDEZVOUS_ARRAY_CONFIGURATION":"IR_NOTIFIER_ARRAY_CONFIGURATION","Build Array must retain its exact non-concatenating authored arity.",p);
                    for(unsigned index=0;index<n.ports.size()-1;++index) {
                        const auto &input=port(n,buildArrayInputId(index),true,p);
                        const auto schema=canonicalObjectReferenceReadType(parse(input.fullTypeJson));
                        if(!schema||!canonicalObjectReferenceFullTypeEqual(*schema,array->children.front()))
                            fail(signatureCode,"Every Build Array input must retain the same complete nominal reference.",p);
                    }
                }
                if(isQueueFunction(n.kind)) {
                    const auto *definition=findFunctionBindingDefinition(std::string_view(n.kind).substr(std::string_view("frog.studio.").size()));
                    if(!definition||n.ports.size()!=definition->ports.size())fail("IR_QUEUE_SIGNATURE","A Queue operation requires all and only its exact typed connectors.",p);
                    std::vector<QueueTypeEvidence> evidence;
                    for(const auto &actual:n.ports)if(actual.input)evidence.push_back({actual.id,*canonicalObjectReferenceReadType(parse(actual.fullTypeJson))});
                    for(const auto &declared:definition->ports) {
                        const auto &actual=port(n,declared.portId,declared.direction==FunctionPortDirection::Input,p);const auto expected=resolveQueueFunctionPort(n.kind,sid,declared.portId,evidence);
                        const auto nominal=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!expected.resolution.accepted()||!nominal||!canonicalObjectReferenceFullTypeEqual(*nominal,expected.resolution.type))fail("IR_QUEUE_SIGNATURE","A Queue connector differs from its exact invariant element or fixed option schema.",p);
                    }
                }
                if(isEventFunction(n.kind)) {
                    DiagramNode owner;owner.id=sid;owner.type=n.kind;
                    if(const auto *configuration=v.member("studio_configuration"))
                        if(const auto *rows=configuration->member("studio_event_registration_rows"))
                            for(const auto &row:arrayAt(rows,p)) {
                                fields(row,{"id","event_kind"},p);
                                owner.eventRegistrationRows.push_back({text(row.member("id"),p),text(row.member("event_kind"),p)});
                            }
                    const auto declared=eventFunctionPorts(owner);
                    if(declared.size()!=n.ports.size())fail("IR_EVENT_FUNCTION_SIGNATURE","An Event operation must preserve all its actual typed connectors.",p);
                    std::vector<EventTypeEvidence> evidence;
                    for(const auto &actual:n.ports)if(actual.input&&std::any_of(rawEdges.begin(),rawEdges.end(),[&](const auto &edge){
                        const auto *target=edge.raw->member("to");
                        return target&&target->member("node")&&target->member("port")&&target->member("node")->string==sid&&target->member("port")->string==actual.id;
                    })) {
                        const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!type)fail("IR_EVENT_FUNCTION_TYPE","An Event input lost its complete nominal schema.",p);
                        evidence.push_back({actual.id,*type});
                    }
                    for(const auto &expected:declared) {
                        const auto &actual=port(n,expected.id,!expected.output,p);
                        const auto resolved=resolveEventFunctionPort(owner,actual.id,evidence);
                        const auto type=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!resolved.resolution.accepted()||!type||!canonicalObjectReferenceFullTypeEqual(*type,resolved.resolution.type))
                            fail("IR_EVENT_FUNCTION_TYPE","An Event connector differs from the shared complete Event type contract.",p+"/ports/"+actual.id);
                    }
                }
                if(isNotifierFunction(n.kind)) {
                    const auto catalog=std::string_view(n.kind).starts_with("frog.studio.")?
                        std::string_view(n.kind).substr(std::string_view("frog.studio.").size()):std::string_view(n.kind);
                    const auto *definition=findFunctionBindingDefinition(catalog);
                    if(!definition||n.ports.size()!=definition->ports.size())
                        fail("IR_NOTIFIER_SIGNATURE","A Notifier operation requires all and only its declared typed connectors.",p);
                    
                    std::vector<NotifierTypeEvidence> evidence;
                    for(const auto &actual:n.ports)if(actual.input)evidence.push_back({actual.id,*canonicalObjectReferenceReadType(parse(actual.fullTypeJson))});
                    for(const auto &declared:definition->ports) {
                        const auto &actual=port(n,declared.portId,declared.direction==FunctionPortDirection::Input,p);
                        const auto expected=resolveNotifierFunctionPort(n.kind,sid,declared.portId,evidence);
                        const auto actualType=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!expected.resolution.accepted()||!actualType||!canonicalObjectReferenceFullTypeEqual(*actualType,expected.resolution.type))
                            fail("IR_NOTIFIER_SIGNATURE","A Notifier connector differs from its complete invariant notification schema.",p);
                    }
                }
                if(isSemaphoreFunction(n.kind)) {
                    const auto catalog=std::string_view(n.kind).substr(std::string_view("frog.studio.").size());
                    const auto *definition=findFunctionBindingDefinition(catalog);
                    if(!definition || n.ports.size()!=definition->ports.size())
                        fail("IR_SEMAPHORE_SIGNATURE","A Semaphore operation requires all and only its declared fixed connectors.",p);
                    
                    std::vector<SemaphoreTypeEvidence> evidence;
                    for(const auto &actual:n.ports)if(actual.input)
                        evidence.push_back({actual.id,*canonicalObjectReferenceReadType(parse(actual.fullTypeJson))});
                    for(const auto &declared:definition->ports) {
                        const auto &actual=port(n,declared.portId,declared.direction==FunctionPortDirection::Input,p);
                        const auto expected=resolveSemaphoreFunctionPort(n.kind,sid,declared.portId,evidence);
                        const auto actualType=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!expected.resolution.accepted() || !actualType ||
                            !canonicalObjectReferenceFullTypeEqual(*actualType,expected.resolution.type))
                            fail("IR_SEMAPHORE_SIGNATURE","A Semaphore connector differs from its fixed nominal signature.",p);
                    }
                }
                if(isRendezvousFunction(n.kind)) {
                    const auto catalog=std::string_view(n.kind).substr(std::string_view("frog.studio.").size());
                    const auto *definition=findFunctionBindingDefinition(catalog);
                    if(!definition || n.ports.size()!=definition->ports.size())
                        fail("IR_RENDEZVOUS_SIGNATURE","A Rendezvous operation requires all and only its declared fixed connectors.",p);
                    
                    std::vector<RendezvousTypeEvidence> evidence;
                    for(const auto &actual:n.ports)if(actual.input)
                        evidence.push_back({actual.id,*canonicalObjectReferenceReadType(parse(actual.fullTypeJson))});
                    for(const auto &declared:definition->ports) {
                        const auto &actual=port(n,declared.portId,declared.direction==FunctionPortDirection::Input,p);
                        const auto expected=resolveRendezvousFunctionPort(n.kind,sid,declared.portId,evidence);
                        const auto actualType=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!expected.resolution.accepted() || !actualType ||
                            !canonicalObjectReferenceFullTypeEqual(*actualType,expected.resolution.type))
                            fail("IR_RENDEZVOUS_SIGNATURE","A Rendezvous connector differs from its fixed nominal signature.",p);
                    }
                }
                if(isOccurrenceFunction(n.kind)) {
                    const auto catalog=std::string_view(n.kind).substr(std::string_view("frog.studio.").size());
                    const auto *definition=findFunctionBindingDefinition(catalog);
                    if(!definition || n.ports.size()!=definition->ports.size())
                        fail("IR_OCCURRENCE_SIGNATURE","An Occurrence operation requires all and only its declared fixed connectors.",p);
                    
                    std::vector<OccurrenceTypeEvidence> evidence;
                    for(const auto &actual:n.ports)if(actual.input)
                        evidence.push_back({actual.id,*canonicalObjectReferenceReadType(parse(actual.fullTypeJson))});
                    for(const auto &declared:definition->ports) {
                        const auto &actual=port(n,declared.portId,declared.direction==FunctionPortDirection::Input,p);
                        const auto expected=resolveOccurrenceFunctionPort(n.kind,sid,declared.portId,evidence);
                        const auto actualType=canonicalObjectReferenceReadType(parse(actual.fullTypeJson));
                        if(!expected.resolution.accepted() || !actualType ||
                            !canonicalObjectReferenceFullTypeEqual(*actualType,expected.resolution.type))
                            fail("IR_OCCURRENCE_SIGNATURE","An Occurrence connector differs from its fixed nominal signature.",p);
                    }
                }
                if(n.kind=="frog.studio.synchronization.first_call" &&
                    (n.ports.size()!=1||n.ports.front().input||n.ports.front().id!="result"||
                     n.ports.front().typeRef!="bool"||n.ports.front().fullTypeJson!=encode(canonicalObjectReferenceWriteType(binding::TypeRef::primitive("bool")))))
                    fail("IR_FIRST_CALL_SIGNATURE","First Call? has no input and exactly one Boolean result output.",p);
                if(n.kind=="frog.studio.structures.feedback_node") {
                    const auto &cfg=objectAt(v.member("studio_feedback"),p+"/studio_feedback");
                    fields(cfg,{"profile","delay","initialization","initializer_loop_id","history"},p+"/studio_feedback");
                    if(text(cfg.member("profile"),p)!="frog.studio.feedback@1")fail("IR_FEEDBACK_PROFILE","Feedback requires its declared local state contract.",p);
                    CanonicalIrFeedback state;const auto *delay=cfg.member("delay");
                    if(!delay||delay->type!=Json::Type::Number)fail("IR_FEEDBACK_DELAY","Feedback Delay must be an exact integer in1..128.",p);
                    const auto converted=std::from_chars(delay->string.data(),delay->string.data()+delay->string.size(),state.delay);
                    if(converted.ec!=std::errc{}||converted.ptr!=delay->string.data()+delay->string.size()||!state.delay||state.delay>128)fail("IR_FEEDBACK_DELAY","Feedback Delay must be in1..128 for this target.",p);
                    state.initialization=text(cfg.member("initialization"),p);
                    if(state.initialization!="compile_or_load"&&state.initialization!="first_call"&&state.initialization!="loop_entry")fail("IR_FEEDBACK_INITIALIZATION","Feedback initialization mode is unknown.",p);
                    const auto loop=text(cfg.member("initializer_loop_id"),p,true);
                    if(state.initialization=="loop_entry") {
                        auto owner=raw.owner;bool found=false;
                        while(!owner.empty()) {
                            const auto &ancestor=rawNodes.at(owner);
                            if(owner==loop) {
                                const auto *kind=ancestor.raw->member("structure_type");
                                found=kind&&(kind->string=="while_loop"||kind->string=="timed_loop"||kind->string=="for_loop");break;
                            }
                            owner=ancestor.owner;
                        }
                        if(!found)fail("IR_FEEDBACK_SCOPE","Feedback Loop Entry must name an actual ancestor For or While loop.",p);
                        state.initializerLoopId=encodedId("node",loop);
                    } else if(!loop.empty())fail("IR_FEEDBACK_SCOPE","A global initialization cannot retain a loop owner.",p);
                    if(!cfg.member("history"))fail("IR_FEEDBACK_CONFIGURATION","Feedback History mode must be explicit.",p);
                    state.history=flag(cfg.member("history"));
                    const auto &input=port(n,"value",true,p),&output=port(n,"result",false,p),&initial=port(n,"initial_value",true,p),&enable=port(n,"enable",true,p);
                    const auto inType=canonicalObjectReferenceReadType(parse(input.fullTypeJson));
                    const auto outType=canonicalObjectReferenceReadType(parse(output.fullTypeJson));
                    const auto initType=canonicalObjectReferenceReadType(parse(initial.fullTypeJson));
                    if(!inType||!outType||!initType||!canonicalObjectReferenceFullTypeEqual(*inType,*outType)||input.typeRef!=output.typeRef)
                        fail("IR_FEEDBACK_TYPE","Feedback input and delayed output require the same complete state type.",p);
                    if((!state.history&&!canonicalObjectReferenceFullTypeEqual(*initType,*outType))||(state.history&&(initType->kind!=binding::TypeKind::Array||initType->rank!=1||initType->children.size()!=1||!canonicalObjectReferenceFullTypeEqual(initType->children.front(),*outType))))
                        fail("IR_FEEDBACK_INITIALIZER_TYPE","Single Value requires T; History requires a rank-one sequence of complete T samples.",p);
                    if(enable.typeRef!="bool"||n.ports.size()!=4)fail("IR_FEEDBACK_PORTS","Feedback has exactly value/result/initial_value/enable, with Boolean Enable.",p);
                    n.feedback=std::move(state);
                } else if(v.member("studio_feedback"))fail("IR_FEEDBACK_OWNER","Feedback state configuration belongs only to a Feedback Node.",p);
                if(isContextPathFunction(n.kind)&&(n.ports.size()!=1||n.ports.front().input||n.ports.front().id!="result"||n.ports.front().typeRef!="path"))fail("IR_CONTEXT_PATH_SIGNATURE","Context Path requires exactly one result Path output.",p);
                if(!canonicalCollectionOperationId(n.kind).empty()) {
                    const auto *configuration=v.member("studio_collection");
                    std::vector<CanonicalCollectionPort> ports;
                    for(const auto &port:n.ports) {
                        const auto schema=canonicalObjectReferenceReadType(parse(port.fullTypeJson));
                        if(!schema)fail("IR_COLLECTION_SCHEMA","Collection requires a complete exact connector schema.",p);
                        ports.push_back({port.id,*schema,port.input,true});
                    }
                    std::string why;const auto descriptor=configuration?canonicalCollectionDescribe(n.kind,*configuration,ports,&why):std::nullopt;
                    if(!descriptor)fail("IR_COLLECTION_CONFIGURATION",why.empty()?"Collection requires its declared local semantic profile.":why,p);
                    for(const auto &port:descriptor->ports) {
                        const auto &type=port.type;
                        const auto *ordered=type.kind==binding::TypeKind::Map&&type.children.size()==2?&type.children[0]:
                            type.kind==binding::TypeKind::Set&&type.children.size()==1?&type.children[0]:nullptr;
                        if(ordered&&!collectionComparableType(*ordered,&why))fail("IR_COLLECTION_COMPARISON",why,p);
                    }
                    n.collectionConfigurationJson=encode(*configuration);
                }else if(v.member("studio_collection"))fail("IR_COLLECTION_OWNER","Collection configuration belongs only to its declared operators.",p);
                if(const auto *descriptor=v.member(canonicalObjectReferenceAttribute)) {
                    CanonicalObjectReferenceDiagnostic diagnostic;
                    n.objectReference=canonicalObjectReferenceDescriptorFromAttributes(*descriptor,p+"/"+std::string(canonicalObjectReferenceAttribute)+"/configuration",&diagnostic);
                    if(!n.objectReference)fail(diagnostic.code,diagnostic.message,p,diagnostic.unsupported);
                    if(n.kind!=canonicalObjectReferenceOperation(n.objectReference->kind))fail("IR_OBJECT_REFERENCE_OPERATION","The configured reference service and operation disagree.",p);
                }
            } else if(n.family=="structure")n.kind=text(v.member("structure_type"),p);
            else fail("IR_COMPOSED_FAMILY","Node family lacks an admitted composed meaning.",p,true);
            nodeIndex[sid]=out.nodes.size();out.nodes.push_back(std::move(n));
            if(!raw.region.empty()) {
                const auto r=std::find_if(out.regions.begin(),out.regions.end(),[&](const auto &r){return r.id==raw.region;});r->memberNodeIds.push_back(out.nodes.back().id);
            }
            map(encodedId("map-node",sid),out.nodes.back().family=="structure"?"structure":"diagram_node",p,out.nodes.back().id);
        }
    }
    void edges() {
        std::set<std::pair<std::string,std::string>> drivers;
        for(const auto &e:rawEdges) {
            const auto &from=objectAt(e.raw->member("from"),e.path),&to=objectAt(e.raw->member("to"),e.path);
            const auto source=text(from.member("node"),e.path),sink=text(to.member("node"),e.path);
            if(!rawNodes.contains(source)||!rawNodes.contains(sink))fail("IR_ENDPOINT_NODE","Connection endpoint is absent.",e.path);
            const auto &a=rawNodes.at(source),&b=rawNodes.at(sink);
            auto &na=node(source),&nb=node(sink);
            const auto ap=text(from.member("port"),e.path),bp=text(to.member("port"),e.path);
            bool relocated=false;
            if(nb.feedback&&bp=="initial_value"&&nb.feedback->initialization=="loop_entry") {
                const auto loop=std::find_if(out.nodes.begin(),out.nodes.end(),[&](const auto &n){return n.id==nb.feedback->initializerLoopId;});
                relocated=loop!=out.nodes.end()&&loop->regionId==e.region;
                if(!relocated)fail("IR_FEEDBACK_INITIALIZER_SCOPE","Loop Entry captures its initializer before that loop starts; its producer must belong to the loop's external scope.",e.path);
            }
            if((a.region!=e.region&&source!=e.owner)||(b.region!=e.region&&sink!=e.owner&&!relocated))fail("IR_SCOPE_ESCAPE","A connection crosses a region without its actual owning structure's boundary or declared relocated initializer.",e.path);
            const auto &originalA=port(na,ap,false,e.path),&originalB=port(nb,bp,true,e.path);
            const auto typeA=canonicalObjectReferenceReadType(parse(originalA.fullTypeJson)),typeB=canonicalObjectReferenceReadType(parse(originalB.fullTypeJson));
            // Display field names and choice presentation remain attributed to
            // their own ports. They do not make a valid typed wire incompatible.
            // TypeRef equality retains Enum domain, representation and its
            // semantic label/value order, as the editor's binding contract does.
            const bool same=originalA.typeRef==originalB.typeRef&&typeA&&typeB&&*typeA==*typeB;
            bool declaredNumericArray=false;
            const bool receiver=!same&&typeA&&typeB&&numericImplicitReceiver(nb,bp,&declaredNumericArray)&&
                binding::connect(*typeA,*typeB,binding::ConnectionPolicy::NumericImplicit).status==binding::ResolutionStatus::Coercion;
            const bool scalarCoercion=receiver&&canonicalNumericScalarCoercionSupported(*typeA,*typeB);
            // Reuse the existing finite typed conversion operators only for
            // Array contracts which actually require this receiver schema.
            // Public/Core, structure state, Enum and reference rules stay exact.
            const bool arrayReceiver=(nb.family=="primitive"&&(nb.authoredKind.starts_with("frog.studio.array.")||nb.authoredKind.starts_with("array.")))||
                (declaredNumericArray&&typeA&&typeB&&plainNumericArray(*typeA)&&plainNumericArray(*typeB));
            const auto treeCoercion=receiver&&!scalarCoercion&&arrayReceiver?canonicalNumericTreeCoercionRecipe(*typeA,*typeB):std::nullopt;
            const bool coerce=scalarCoercion||treeCoercion.has_value();
            const bool errorBridge=!same&&typeA&&typeB&&scalarErrorRoles(*typeA)&&scalarErrorRoles(*typeB)&&
                binding::connect(*typeA,*typeB,binding::ConnectionPolicy::Exact).accepted();
            if(!same&&!coerce&&!errorBridge)fail("IR_EDGE_TYPE","Connection loses its semantic type or declared representation; no admitted typed NumericImplicit receiver permits adaptation.",e.path);
            const auto qa=qualified(e,source,ap,false),qb=qualified(e,sink,bp,true);
            const auto clone=[&](CanonicalIrNode &n,const CanonicalIrPort &old,const std::string &id) {
                if(id==old.id)return;
                if(std::none_of(n.ports.begin(),n.ports.end(),[&](const auto &p){return p.id==id&&p.input==old.input;})){auto copy=old;copy.id=id;n.ports.push_back(std::move(copy));}
            };
            // Copy before vector growth: both endpoints can be the same owner.
            const auto ca=originalA,cb=originalB;clone(na,ca,qa);clone(nb,cb,qb);
            if(!drivers.emplace(nb.id,qb).second)fail("IR_MULTIPLE_DRIVERS","A physical input has more than one writer in the same actual region.",e.path);
            const auto id=text(e.raw->member("id"),e.path);
            // Both copied endpoint IDs survive out.nodes growth, including a
            // self-edge on an owner. Do not retain any reference across push.
            const auto fromId=na.id,toId=nb.id;
            if(errorBridge) {
                scalarErrorBridge(e,a,b,id,source,sink,fromId,qa,toId,qb,ca,cb,*typeA,*typeB);
            } else if(coerce) {
                if(out.nodes.size()>=options.limits.maximumNodes||out.connections.size()+2>options.limits.maximumEdges)
                    fail("IR_RESOURCE_LIMIT","Derived numeric coercions exceed the existing FIR node/edge budget.",e.path,true);
                CanonicalIrNode cast;cast.id=encodedId("node-coercion",id);cast.sourceNodeId=sink;
                cast.family="primitive";cast.kind=treeCoercion?std::string(treeCoercion->operationId):std::string(canonicalNumericCoercionId);cast.role="data";cast.regionId=e.region;
                cast.valueType=cb.typeRef;cast.fullTypeJson=cb.fullTypeJson;
                const auto castInput=treeCoercion?std::string(treeCoercion->inputPort):std::string("in");
                cast.ports={{castInput,true,ca.typeRef,ca.fullTypeJson},{"result",false,cb.typeRef,cb.fullTypeJson}};
                auto configuration=object();configuration.object={{"profile",string(std::string(canonicalNumericCoercionProfile))},
                    {"source_edge_id",string(id)},{"source_type",string(ca.typeRef)},{"target_type",string(cb.typeRef)},
                    {"receiver_node_id",string(sink)},{"receiver_port_id",string(bp)}};
                cast.configurationJson=encode(configuration);
                std::vector<CanonicalNumericAggregatePort> signature{{castInput,*typeA,true,true},{"result",*typeB,false,true}};
                std::string why,at;if(!(canonicalNumericComplexContract(cast.kind)?canonicalNumericComplexSignatureValid(cast.kind,signature,&why):canonicalNumericAggregateSignatureValid(cast.kind,signature,&why,&at)))
                    fail("IR_NUMERIC_COERCION_SIGNATURE",why,e.path);
                const auto inputEdge=encodedId("edge-coercion-input",id),outputEdge=encodedId("edge",id),castId=cast.id;
                if(castId.size()>options.limits.maximumIrIdBytes||inputEdge.size()>options.limits.maximumIrIdBytes)
                    fail("IR_RESOURCE_LIMIT","Derived coercion identities exceed the existing FIR identity budget.",e.path,true);
                out.connections.push_back({inputEdge,id,{fromId,qa},{castId,castInput}});
                out.connections.push_back({outputEdge,id,{castId,"result"},{toId,qb}});
                map(encodedId("map-coercion-input",id),"diagram_edge",e.path,inputEdge,"connection","support_only");
                map(encodedId("map-edge",id),"diagram_edge",e.path,outputEdge,"connection");
                map(encodedId("map-coercion",id),"diagram_edge",e.path,castId,"support_object","support_only");
                map(encodedId("map-coercion-receiver",id),"diagram_node",b.path,castId,"support_object","support_only");
                if(!e.region.empty()) {
                    const auto region=std::find_if(out.regions.begin(),out.regions.end(),[&](const auto &r){return r.id==e.region;});
                    if(region==out.regions.end())fail("IR_NUMERIC_COERCION_SCOPE","The actual wire region disappeared.",e.path);
                    region->memberNodeIds.push_back(castId);
                }
                coercionOrigins.emplace(castId,&e);out.nodes.push_back(std::move(cast));
            } else {
                if(out.connections.size()>=options.limits.maximumEdges)fail("IR_RESOURCE_LIMIT","FIR edge budget exceeded.",e.path,true);
                out.connections.push_back({encodedId("edge",id),id,{fromId,qa},{toId,qb}});
                map(encodedId("map-edge",id),"diagram_edge",e.path,out.connections.back().id,"connection");
            }
        }
    }
    const Json &schema(const CanonicalIrPort &p,Json &storage)const {storage=parse(p.fullTypeJson);return storage;}
    void structures();
    void validateFileConfigurations() {
        const auto ids=canonicalFileNativeOperationIds();
        for(const auto &n:out.nodes)if(std::find(ids.begin(),ids.end(),n.kind)!=ids.end()) {
            std::string why;const auto cfg=canonicalFileNativeConfigurationFromJson(n.configurationJson,&why);
            const auto &at=rawNodes.at(n.sourceNodeId).path;
            if(!cfg)fail("IR_FILE_CONFIGURATION",why,at+"/studio_configuration");
            const auto wired=[&](std::string_view name,bool input){return std::any_of(out.connections.begin(),out.connections.end(),[&](const auto &e){
                const auto &endpoint=input?e.to:e.from;return endpoint.objectId==n.id&&endpoint.portId==name;
            });};
            const bool exists=n.kind=="frog.studio.file.exists.windows.v1",read=n.kind=="frog.studio.file.text_read.windows.v1";
            if(cfg->fileConnected!=wired(exists?"path":"file",true)||
                cfg->countConnected!=(read&&wired("count",true))||cfg->retainReference!=(!exists&&wired("file_ref",false)))
                fail("IR_FILE_WIRING","File recipe differs from its actual Source connections; defaults are not authored drivers.",at+"/studio_configuration");
            if((!read&&cfg->readLines)||(exists&&!cfg->convertEol))
                fail("IR_FILE_CONFIGURATION","File recipe contains an option unavailable for this operation.",at+"/studio_configuration");
        }
    }
    void validateScopes() {
        // An explicit shift register is state, whereas a cycle wholly inside
        // a region remains invalid. Owner boundary projections are excluded.
        std::map<std::string,std::vector<std::string>> scopes;
        for(const auto &n:out.nodes)scopes[n.regionId].push_back(n.id);
        for(const auto &[region,ids]:scopes) {
            const auto sequence=[&](const std::string &id)->const CanonicalIrStructure* {
                const auto found=std::find_if(out.structures.begin(),out.structures.end(),[&](const auto &s){return s.nodeId==id&&s.kind=="sequence_structure";});
                return found==out.structures.end()?nullptr:&*found;
            };
            const bool hasSequence=std::any_of(ids.begin(),ids.end(),[&](const auto &id){return sequence(id)!=nullptr;});
            if(hasSequence) {
                using Activity=std::pair<std::string,std::string>;
                std::set<Activity> activities;
                for(const auto &id:ids)if(const auto *s=sequence(id))for(const auto &frame:s->regionIds)activities.emplace(id,frame);else activities.emplace(id,std::string{});
                const auto project=[&](const CanonicalIrEndpoint &endpoint,bool input)->std::optional<Activity> {
                    auto id=endpoint.objectId;auto current=std::find_if(out.nodes.begin(),out.nodes.end(),[&](const auto &n){return n.id==id;});
                    std::string childRegion;
                    while(current!=out.nodes.end()&&current->regionId!=region&&!current->regionId.empty()) {
                        childRegion=current->regionId;const auto owner=std::find_if(out.regions.begin(),out.regions.end(),[&](const auto &r){return r.id==childRegion;});
                        if(owner==out.regions.end())return {};id=owner->ownerNodeId;current=std::find_if(out.nodes.begin(),out.nodes.end(),[&](const auto &n){return n.id==id;});
                    }
                    if(current==out.nodes.end()||current->regionId!=region)return {};
                    if(const auto *s=sequence(id)) {
                        if(id!=endpoint.objectId)return Activity{id,childRegion};
                        const auto transfer=std::find_if(s->sequenceTransfers.begin(),s->sequenceTransfers.end(),[&](const auto &t){return input?t.inputPortId==endpoint.portId:t.outputPortId==endpoint.portId;});
                        if(transfer==s->sequenceTransfers.end())fail("IR_SEQUENCE_ENDPOINT","A Sequence endpoint has no exact frame transfer.",id);
                        const bool exterior=input?transfer->sourceRegionId.empty():transfer->targetRegionId.empty();
                        const auto frame=input?(exterior?(s->sequenceProfile=="stacked_sequence"?s->regionIds.front():transfer->targetRegionId):transfer->sourceRegionId):
                            (exterior?(s->sequenceProfile=="stacked_sequence"?s->regionIds.back():transfer->sourceRegionId):transfer->targetRegionId);
                        return Activity{id,frame};
                    }
                    return Activity{id,{}};
                };
                std::map<Activity,unsigned> indegree;std::map<Activity,std::set<Activity>> next;
                for(const auto &a:activities)indegree[a]=0;
                const auto dependency=[&](const Activity &a,const Activity &b){if(a!=b&&activities.contains(a)&&activities.contains(b)&&next[a].insert(b).second)++indegree[b];};
                for(const auto &id:ids)if(const auto *s=sequence(id))for(std::size_t i=1;i<s->regionIds.size();++i)dependency({id,s->regionIds[i-1]},{id,s->regionIds[i]});
                for(const auto &e:out.connections) {
                    const auto target=std::find_if(out.nodes.begin(),out.nodes.end(),[&](const auto &n){return n.id==e.to.objectId;});
                    if(target!=out.nodes.end()&&target->feedback&&(e.to.portId=="value"||e.to.portId=="enable"))continue;
                    const auto a=project(e.from,false),b=project(e.to,true);if(a&&b)dependency(*a,*b);
                }
                std::deque<Activity> ready;for(const auto &[id,count]:indegree)if(!count)ready.push_back(id);std::size_t seen=0;
                while(!ready.empty()){auto id=ready.front();ready.pop_front();++seen;for(const auto &target:next[id])if(!--indegree[target])ready.push_back(target);}
                if(seen!=activities.size())fail("IR_SEQUENCE_DEPENDENCY_CYCLE","The actual Sequence frame/input/output dependency graph is cyclic; Stacked requires all external inputs before its first frame and publishes only at final exit.",region);
                continue;
            }
            std::map<std::string,unsigned> indegree;std::map<std::string,std::vector<std::string>> next;
            for(const auto &id:ids)indegree[id]=0;
            for(const auto &e:out.connections)if(edgeRegions.at(e.sourceEdgeId)==region&&indegree.contains(e.from.objectId)) {
                const auto target=std::find_if(out.nodes.begin(),out.nodes.end(),[&](const auto &n){return n.id==e.to.objectId;});
                // Only future-state writes cross the temporal boundary. An
                // initializer is still required before the previous-state read.
                if(target!=out.nodes.end()&&target->feedback&&(e.to.portId=="value"||e.to.portId=="enable"))continue;
                auto targetId=e.to.objectId;
                if(target!=out.nodes.end()&&target->feedback&&e.to.portId=="initial_value"&&target->feedback->initialization=="loop_entry"&&!indegree.contains(targetId))targetId=target->feedback->initializerLoopId;
                if(indegree.contains(targetId)){next[e.from.objectId].push_back(targetId);++indegree[targetId];}
            }
            std::deque<std::string> ready;for(const auto &[id,count]:indegree)if(!count)ready.push_back(id);std::size_t seen=0;
            while(!ready.empty()){const auto id=ready.front();ready.pop_front();++seen;for(const auto &target:next[id])if(!--indegree[target])ready.push_back(target);}
            if(seen!=ids.size())fail("IR_CYCLE","A region dataflow cycle requires explicit declared state.",region);
        }
    }
    void write();
};
void Builder::structures() {
    for(auto &n:out.nodes)if(n.family=="structure") {
        const auto &raw=rawNodes.at(n.sourceNodeId);const auto &v=*raw.raw;const auto &p=raw.path;
        CanonicalIrStructure s;s.nodeId=n.id;s.kind=n.kind;
        if(s.kind!="while_loop"&&s.kind!="timed_loop"&&s.kind!="for_loop"&&s.kind!="case"&&s.kind!="event_structure"&&s.kind!="in_place_element_structure"&&s.kind!="sequence_structure")fail("IR_STRUCTURE_UNSUPPORTED","Structure has no composed native realization.",p,true);
        for(const auto &r:out.regions)if(r.ownerNodeId==n.id)s.regionIds.push_back(r.id);
        if(s.regionIds.empty())fail("IR_STRUCTURE_REGION","A structure requires explicit owned regions.",p);
        if(s.kind=="timed_loop") {
            const auto &config=objectAt(v.member("studio_timed_loop"),p+"/studio_timed_loop");
            fields(config,{"profile","schedule","options"},p);
            if(text(config.member("profile"),p)!=timed_loop::profile)fail("IR_TIMED_LOOP_PROFILE","Unknown Timed Loop semantic profile.",p);
            try {
                (void)timed_loop::readSchedule(timed_loop::member(config,"schedule"));
                (void)timed_loop::readOptions(timed_loop::member(config,"options"));
            }catch(const std::exception &e){fail("IR_TIMED_LOOP_CONFIGURATION",e.what(),p);}
            for(const auto &port:n.ports)if(port.id.starts_with("timed.")) {
                bool known=false;
                for(const auto &role:kDiagramTimedLoopTimingNodes)if(port.id.starts_with(role.portPrefix)) {
                    const auto field=std::string_view(port.id).substr(role.portPrefix.size());
                    const auto *spec=diagramStructureShelfFieldById(role.fields,field.substr(0,field.find('#')));
                    if(!spec||port.input!=(role.direction=="input"))fail("IR_TIMED_LOOP_PORT","Unknown timing field or direction.",p+":"+port.id);
                    const auto actual=canonicalObjectReferenceReadType(parse(port.fullTypeJson));
                    const auto expected=binding::parseType(spec->valueType);
                    if(!actual||!expected.valid()||actual->canonical()!=expected.type.canonical())fail("IR_TIMED_LOOP_PORT_TYPE","Timing terminal lost its exact declared type.",p+":"+port.id);
                    known=true;break;
                }
                if(!known)fail("IR_TIMED_LOOP_PORT","Timing terminal has no declared role.",p+":"+port.id);
            }
            s.timedLoopConfigurationJson=encode(config);
        }else if(v.member("studio_timed_loop"))fail("IR_TIMED_LOOP_OWNER","Timing configuration belongs only to a Timed Loop.",p);
        if(s.kind=="sequence_structure") {
            if(s.regionIds.size()>128)fail("IR_SEQUENCE_FRAME_LIMIT","A Sequence has at most128 explicitly ordered frames.",p);
            const auto &config=objectAt(v.member("studio_sequence"),p+"/studio_sequence");
            fields(config,{"profile","semantic_profile","transfers"},p+"/studio_sequence");
            if(text(config.member("profile"),p)!="frog.studio.sequence@1")fail("IR_SEQUENCE_PROFILE","Sequence requires its explicit local semantic contract.",p);
            s.sequenceProfile=text(config.member("semantic_profile"),p);
            if(s.sequenceProfile!="flat_sequence"&&s.sequenceProfile!="stacked_sequence")fail("IR_SEQUENCE_PROFILE","Unknown Sequence semantic profile.",p);
            const auto region=[&](const std::string &frame)->std::string {
                if(frame.empty())return {};
                const auto found=std::find_if(out.regions.begin(),out.regions.end(),[&](const auto &r){return r.ownerNodeId==n.id&&r.sourceRegionId==frame;});
                if(found==out.regions.end())fail("IR_SEQUENCE_FRAME","A transfer names no frame owned by this Sequence.",p);return found->id;
            };
            const auto &boundary=objectAt(v.member("boundary"),p);std::set<std::string> ids;
            for(const auto &entry:arrayAt(config.member("transfers"),p)) {
                fields(entry,{"id","source_frame","target_frame","use_default_if_unwired"},p);
                CanonicalIrSequenceTransfer t;t.id=text(entry.member("id"),p);
                if(!ids.insert(t.id).second)fail("IR_SEQUENCE_TRANSFER_ID","A Sequence transfer identity must be unique.",p);
                t.sourceRegionId=region(text(entry.member("source_frame"),p,true));t.targetRegionId=region(text(entry.member("target_frame"),p,true));
                if(t.sourceRegionId.empty()&&t.targetRegionId.empty())fail("IR_SEQUENCE_TRANSFER_SCOPE","A Sequence transfer must touch one of its real frames.",p);
                if(!t.sourceRegionId.empty()&&!t.targetRegionId.empty()&&
                    std::find(s.regionIds.begin(),s.regionIds.end(),t.sourceRegionId)>=std::find(s.regionIds.begin(),s.regionIds.end(),t.targetRegionId))
                    fail("IR_SEQUENCE_BACKWARD_READ","A passage must carry a value to a strictly later frame.",p);
                t.inputPortId=diagram_identity::tunnelInputId(t.id);t.outputPortId=diagram_identity::tunnelOutputId(t.id);
                auto input=std::find_if(n.ports.begin(),n.ports.end(),[&](const auto &p){return p.input&&p.id==t.inputPortId;});
                auto output=std::find_if(n.ports.begin(),n.ports.end(),[&](const auto &p){return !p.input&&p.id==t.outputPortId;});
                if(input==n.ports.end()&&output==n.ports.end())fail("IR_SEQUENCE_TRANSFER_TYPE","A Sequence transfer requires actual complete connector evidence.",p);
                const auto known=input==n.ports.end()?*output:*input;
                const bool missingInput=input==n.ports.end(),missingOutput=output==n.ports.end();
                if(missingInput)n.ports.push_back({t.inputPortId,true,known.typeRef,known.fullTypeJson});
                if(missingOutput)n.ports.push_back({t.outputPortId,false,known.typeRef,known.fullTypeJson});
                const auto &a=port(n,t.inputPortId,true,p),&b=port(n,t.outputPortId,false,p);
                if(a.typeRef!=b.typeRef||!canonicalObjectReferenceFullTypeEqual(*canonicalObjectReferenceReadType(parse(a.fullTypeJson)),*canonicalObjectReferenceReadType(parse(b.fullTypeJson))))
                    fail("IR_SEQUENCE_TRANSFER_TYPE","Both faces of a Sequence transfer preserve the same complete nominal schema.",p);
                t.typeRef=a.typeRef;t.fullTypeJson=a.fullTypeJson;t.useDefaultIfUnwired=flag(entry.member("use_default_if_unwired"));
                if(t.sourceRegionId.empty()&&t.useDefaultIfUnwired)fail("IR_SEQUENCE_INPUT_DEFAULT","An exterior input requires an actual source.",p);
                unsigned declarations=0;
                for(const auto side:{"inputs","outputs"})for(const auto &declared:arrayAt(boundary.member(side),p))if(text(declared.member("id"),p)==t.id) {
                    ++declarations;const auto nominal=canonicalObjectReferenceReadType(parse(t.fullTypeJson));
                    if(!nominal||nominal->canonical()!=text(declared.member("type"),p))fail("IR_SEQUENCE_TRANSFER_TYPE","The saved boundary type differs from its actual connector schema.",p);
                }
                if(declarations!=1)fail("IR_SEQUENCE_BOUNDARY","Each Sequence transfer has exactly one authored boundary declaration.",p);
                const auto writer=std::find_if(out.connections.begin(),out.connections.end(),[&](const auto &e){return e.to.objectId==n.id&&e.to.portId==t.inputPortId;});
                if(writer==out.connections.end()&&!t.useDefaultIfUnwired)fail("IR_SEQUENCE_MISSING_WRITER","A Sequence transfer requires its real writer or explicit default.",p);
                s.sequenceTransfers.push_back(std::move(t));
            }
            const auto count=arrayAt(boundary.member("inputs"),p).size()+arrayAt(boundary.member("outputs"),p).size();
            if(count!=s.sequenceTransfers.size()||n.ports.size()!=2*s.sequenceTransfers.size())fail("IR_SEQUENCE_BOUNDARY","No Sequence boundary or intrinsic port may be silently ignored.",p);
            for(const auto &edge:out.connections)for(const bool input:{false,true}) {
                const auto &endpoint=input?edge.to:edge.from;if(endpoint.objectId!=n.id)continue;
                const auto transfer=std::find_if(s.sequenceTransfers.begin(),s.sequenceTransfers.end(),[&](const auto &t){return input?t.inputPortId==endpoint.portId:t.outputPortId==endpoint.portId;});
                if(transfer==s.sequenceTransfers.end())fail("IR_SEQUENCE_ENDPOINT","A Sequence wire endpoint has no actual transfer.",p);
                const auto &face=input?transfer->sourceRegionId:transfer->targetRegionId;
                if(edgeRegions.at(edge.sourceEdgeId)!=(face.empty()?n.regionId:face))
                    fail("IR_SEQUENCE_TRANSFER_SCOPE","A wire must connect the transfer face in its exact authored frame or exterior region.",p);
            }
            out.structures.push_back(std::move(s));continue;
        }
        if(s.kind=="in_place_element_structure") {
            if(s.regionIds.size()!=1||out.regions.at(std::size_t(std::find_if(out.regions.begin(),out.regions.end(),[&](const auto &r){return r.id==s.regionIds.front();})-out.regions.begin())).sourceRegionId!="body")
                fail("IR_IPE_REGION","An IPE has exactly one explicitly owned body region.",p);
            std::string why;const auto *config=v.member("studio_in_place");
            if(!config||!canonicalInPlaceReadConfiguration(*config,&why))fail("IR_IPE_CONFIGURATION",why.empty()?"IPE needs its explicit semantic configuration.":why,p);
            s.inPlaceConfigurationJson=encode(*config);
        } else if(v.member("studio_in_place"))fail("IR_IPE_OWNER","IPE configuration belongs only to an IPE structure.",p);
        const auto &terminals=objectAt(v.member("structure_terminals"),p);
        const auto terminal=[&](std::string_view name,bool body)->std::string {
            const auto *t=terminals.member(name);if(!t)return {};
            const auto *id=t->member(body?"body_port":"outer_port");return id?text(id,p):std::string{};
        };
        if(s.kind=="while_loop"||s.kind=="timed_loop") {
            if(s.regionIds.size()!=1)fail("IR_LOOP_REGION","A While has exactly one explicit body.",p);
            s.iterationPortId=terminal("index",true);s.conditionPortId=terminal("condition",true);
            const auto *condition=terminals.member("condition");
            s.stopIfTrue=flag(v.member("while_loop_stop_if_true"))||(condition&&condition->member("role")&&condition->member("role")->string=="stop_if_true");
        } else if(s.kind=="for_loop") {
            if(s.regionIds.size()!=1)fail("IR_LOOP_REGION","A For has exactly one explicit body.",p);
            s.iterationPortId=terminal("index",true);s.countPortId=terminal("count",false);s.countValuePortId=terminal("count",true);
        } else if(s.kind=="case") {s.selectorPortId=terminal("selector",false);s.selectedCasePortId=terminal("selector",true);}
        else if(s.kind=="event_structure")s.timeoutPortId=terminal("timeout",false);
        std::map<std::string,CanonicalIrShiftRegister> groups;
        const auto &boundary=objectAt(v.member("boundary"),p);
        fields(boundary,{"inputs","outputs"},p+"/boundary");std::set<std::string> boundaryIds;
        for(const auto side:{"inputs","outputs"})for(const auto &entry:arrayAt(boundary.member(side),p+"/boundary/"+side)) {
            fields(entry,{"id","type","shift_register_pair_id","shift_register_history_index","tunnel_mode","indexing_policy","conditional","use_default_if_unwired"},p+"/boundary/"+side);
            const bool output=std::string_view(side)=="outputs";const auto tid=text(entry.member("id"),p);
            if(!boundaryIds.insert(tid).second)fail("IR_TUNNEL_ID","Each logical tunnel must have one distinct identity.",p);
            const auto input=diagram_identity::tunnelInputId(tid),read=diagram_identity::tunnelOutputId(tid);
            auto inputPort=std::find_if(n.ports.begin(),n.ports.end(),[&](const auto &p){return p.input&&p.id==input;});
            auto readPort=std::find_if(n.ports.begin(),n.ports.end(),[&](const auto &p){return !p.input&&p.id==read;});
            if(inputPort==n.ports.end()&&readPort==n.ports.end())fail("IR_TUNNEL_TYPE","An actual tunnel requires complete connector type evidence.",p);
            const auto mode=entry.member("tunnel_mode")?text(entry.member("tunnel_mode"),p):"last_value";
            const auto policy=entry.member("indexing_policy")?text(entry.member("indexing_policy"),p):"leading_dimension";
            if(mode!="last_value"&&mode!="indexing")fail("IR_TUNNEL_MODE_UNSUPPORTED","This tunnel mode has no declared native realization.",p,true);
            if(policy!="leading_dimension"&&policy!="atomic_elements")fail("IR_TUNNEL_POLICY","The tunnel indexing policy is unknown.",p);
            if(mode=="indexing"&&(s.kind!="for_loop"||entry.member("shift_register_pair_id")))fail("IR_TUNNEL_MODE_UNSUPPORTED","Native indexing requires an actual For loop tunnel.",p,true);
            const bool conditional=flag(entry.member("conditional"));
            if(conditional&&(!output||s.kind!="for_loop"||mode!="indexing"||entry.member("shift_register_pair_id")))
                fail("IR_TUNNEL_MODE_UNSUPPORTED","Conditional collection requires a For output indexing tunnel.",p+":"+tid,true);
            const bool needInput=inputPort==n.ports.end(),needRead=readPort==n.ports.end();
            if(mode=="indexing"&&(needInput||needRead))fail("IR_TUNNEL_TYPE","Indexed tunnels require distinct complete schemas for both faces.",p);
            const auto known=!needInput?*inputPort:*readPort;
            const auto nominal=canonicalObjectReferenceReadType(parse(known.fullTypeJson));
            if(!nominal||nominal->canonical()!=text(entry.member("type"),p))fail("IR_TUNNEL_TYPE","The authored tunnel type and its complete source schema differ.",p);
            if(needInput)n.ports.push_back({input,true,known.typeRef,known.fullTypeJson});
            if(needRead)n.ports.push_back({read,false,known.typeRef,known.fullTypeJson});
            const auto inputSchema=canonicalObjectReferenceReadType(parse(port(n,input,true,p).fullTypeJson));
            const auto readSchema=canonicalObjectReferenceReadType(parse(port(n,read,false,p).fullTypeJson));
            if(!inputSchema||!readSchema)fail("IR_TUNNEL_TYPE","A tunnel lost its exact face schemas.",p);
            if(mode=="indexing") {
                const auto projected=diagramIndexedTunnelType(*inputSchema,output,policy=="atomic_elements"?DiagramTunnelIndexingPolicy::AtomicElements:DiagramTunnelIndexingPolicy::LeadingDimension);
                if(!projected.accepted()||!canonicalObjectReferenceFullTypeEqual(projected.type,*readSchema))
                    fail("IR_TUNNEL_TYPE","The indexed faces disagree with the common source projection.",p);
            }else if(!canonicalObjectReferenceFullTypeEqual(*inputSchema,*readSchema))fail("IR_TUNNEL_TYPE","A LastValue tunnel cannot change its complete schema.",p);
            if(const auto *pair=entry.member("shift_register_pair_id")) {
                if(s.kind!="while_loop"&&s.kind!="timed_loop"&&s.kind!="for_loop")fail("IR_SHIFT_REGISTER_OWNER","A shift register must belong to its actual loop.",p);
                auto &g=groups[text(pair,p)];g.pairId=text(pair,p);
                if(output) {if(!g.writePortId.empty())fail("IR_SHIFT_REGISTER_PAIR","A shift register has one writer.",p);g.writePortId=input;g.resultPortId=read;}
                else {
                    std::size_t history=0;if(const auto *h=entry.member("shift_register_history_index")) {const auto parsed=std::from_chars(h->string.data(),h->string.data()+h->string.size(),history);if(parsed.ec!=std::errc{}||parsed.ptr!=h->string.data()+h->string.size()||history>1024)fail("IR_SHIFT_REGISTER_HISTORY","History index must remain exact and bounded.",p);}
                    g.stages.push_back({tid,input,read,known.typeRef,history});
                }
            } else {
                CanonicalIrTunnel tunnel;tunnel.id=tid;tunnel.mode=mode;tunnel.indexingPolicy=policy;
                const auto &inside=port(n,output?input:read,output,p);
                tunnel.typeRef=inside.typeRef;tunnel.fullTypeJson=inside.fullTypeJson;
                if(mode=="indexing") {const auto &outside=port(n,output?read:input,!output,p);tunnel.externalTypeRef=outside.typeRef;tunnel.externalFullTypeJson=outside.fullTypeJson;}
                if(conditional) {
                    tunnel.conditionPortId=diagramTunnelConditionPortId(tid);
                    const auto &condition=port(n,tunnel.conditionPortId,true,p);
                    const auto schema=canonicalObjectReferenceReadType(parse(condition.fullTypeJson));
                    if(condition.typeRef!="bool"||!schema||schema->canonical()!="bool")
                        fail("IR_TUNNEL_CONDITION_TYPE","A conditional output requires an exact Boolean guard.",p+":"+tid);
                    const auto writer=std::find_if(out.connections.begin(),out.connections.end(),[&](const auto &e){return e.to.objectId==n.id&&e.to.portId==tunnel.conditionPortId;});
                    if(writer==out.connections.end())fail("IR_TUNNEL_CONDITION_MISSING","Wire the conditional output's Boolean guard inside its For body.",p+":"+tid);
                    if(s.regionIds.size()!=1||edgeRegions.at(writer->sourceEdgeId)!=s.regionIds.front())
                        fail("IR_TUNNEL_CONDITION_SCOPE","A conditional output guard must be written in its exact For body.",p+":"+tid);
                }
                tunnel.useDefaultIfUnwired=flag(entry.member("use_default_if_unwired"));
                if(output) {
                    tunnel.resultPortId=read;
                    for(const auto &rid:s.regionIds) {
                        const auto id=(s.kind=="case"||s.kind=="event_structure")?input+"@"+rid:input;
                        const bool connected=std::any_of(out.connections.begin(),out.connections.end(),[&](const auto &e){return e.to.objectId==n.id&&e.to.portId==id;});
                        if(connected)tunnel.writers.push_back({rid,id});
                        else if(!tunnel.useDefaultIfUnwired)fail("IR_TUNNEL_MISSING_WRITER","Every applicable region must write this output tunnel, unless its authored default policy is enabled.",p+":"+tid);
                    }
                } else {tunnel.inputPortId=input;tunnel.readPortId=read;}
                s.tunnels.push_back(std::move(tunnel));
            }
        }
        for(auto &[_,g]:groups) {
            if(g.stages.empty()||g.writePortId.empty())fail("IR_SHIFT_REGISTER_PAIR","A shift register requires a writer and declared history.",p);
            std::sort(g.stages.begin(),g.stages.end(),[](const auto &a,const auto &b){return a.historyIndex<b.historyIndex;});
            const auto &writer=port(n,g.writePortId,true,p);
            for(std::size_t i=0;i<g.stages.size();++i)if(g.stages[i].historyIndex!=i||g.stages[i].typeRef!=writer.typeRef)fail("IR_SHIFT_REGISTER_TYPE","The complete shift-register history must retain one exact physical type.",p);
            s.shiftRegisters.push_back(std::move(g));
        }
        const auto &rs=arrayAt(v.member("regions"),p+"/regions");
        if(s.kind=="case") {
            const auto &selector=port(n,s.selectorPortId,true,p);const auto full=parse(selector.fullTypeJson);
            const auto nominal=canonicalObjectReferenceReadType(full);if(!nominal)fail("IR_CASE_TYPE","Case selector lost its nominal schema.",p);
            unsigned defaults=0;std::set<std::string> labels;
            for(const auto &r:rs) {
                const auto sid=text(r.member("id"),p),rid=encodedId("region",n.sourceNodeId)+":"+encodedId("",sid).substr(1);
                const bool isDefault=flag(r.member("default"));if(isDefault&&++defaults>1)fail("IR_CASE_DEFAULT","A Case has at most one default.",p);
                const auto &matches=arrayAt(r.member("matches"),p+"/matches");
                if(matches.empty()&&!isDefault)fail("IR_CASE_MATCH","A nondefault region requires a semantic match.",p);
                for(const auto &m:matches) {
                    const auto kind=text(m.member("kind"),p);Json value;
                    if(kind=="boolean_exact") {if(selector.typeRef!="bool"||!m.member("value")||m.member("value")->type!=Json::Type::Boolean)fail("IR_CASE_MATCH","Boolean match disagrees with selector.",p);value=*m.member("value");}
                    else if(kind=="string_exact") {if(selector.typeRef!="string")fail("IR_CASE_MATCH","String match disagrees with selector.",p);value=string(text(m.member("value"),p,true));}
                    else if(kind=="integer_exact") {value=parse(text(m.member("value"),p));if(!typedScalar(value,selector.typeRef))fail("IR_CASE_MATCH","Integer match is outside the exact selector type.",p);}
                    else if(kind=="enum_item") {
                        if(nominal->kind!=binding::TypeKind::Enum)fail("IR_CASE_MATCH","Enum matches need the actual nominal Enum definition.",p);
                        const auto item=text(m.member("item"),p);const auto c=std::find_if(nominal->choices.begin(),nominal->choices.end(),[&](const auto &c){return c.id==item;});
                        if(c==nominal->choices.end())fail("IR_CASE_ENUM_ITEM","Enum match names no item in the actual selector domain.",p);value=parse(std::to_string(c->value));
                    } else if(kind=="error_status") {
                        if(nominal->kind!=binding::TypeKind::Cluster)fail("IR_CASE_MATCH","Error Status Case needs an actual Error Record.",p);
                        const auto i=std::find(nominal->fieldIds.begin(),nominal->fieldIds.end(),"status");
                        if(i==nominal->fieldIds.end()||nominal->children[std::size_t(i-nominal->fieldIds.begin())].canonical()!="bool")fail("IR_CASE_MATCH","Error Status Case requires the exact Boolean status field.",p);
                        s.caseSelectorFieldPath={"status"};const auto *v=m.member("value");
                        if(v&&v->type==Json::Type::Boolean)value=*v;
                        else {const auto t=text(v,p);if(t!="true"&&t!="false")fail("IR_CASE_MATCH","Error status match must be Boolean.",p);value=boolean(t=="true");}
                    } else fail("IR_CASE_MATCH_UNSUPPORTED","This Case match requires its own declared native realization.",p,true);
                    if(!labels.insert(encode(value)).second)fail("IR_CASE_MATCH_DUPLICATE","Case labels overlap.",p);
                    if(!isDefault)s.caseArms.push_back({rid,encode(value),false});
                }
                if(isDefault)s.caseArms.push_back({rid,{},true});
            }
            if(!defaults&&!(selector.typeRef=="bool"&&labels.size()==2)&&!(s.caseSelectorFieldPath==std::vector<std::string>{"status"}&&labels.size()==2)&&!(nominal->kind==binding::TypeKind::Enum&&labels.size()==nominal->choices.size()))fail("IR_CASE_COVERAGE","A nonexhaustive selector requires an explicit default region.",p);
        } else if(s.kind=="event_structure") {
            auto event=objectAt(v.member("studio_event"),p+"/studio_event");
            auto *cases=&event.object.at("cases");if(cases->type!=Json::Type::Array||cases->array.size()!=rs.size())fail("IR_EVENT_CASES","Event descriptors must retain every actual event region.",p);
            for(auto &c:cases->array) {
                const auto sid=text(c.member("id"),p),rid=encodedId("region",n.sourceNodeId)+":"+encodedId("",sid).substr(1);
                const auto original=std::find_if(rs.begin(),rs.end(),[&](const auto &r){return r.member("id")&&r.member("id")->string==sid;});
                if(original==rs.end()||!c.member("event")||!original->member("event")||!exact(*c.member("event"),*original->member("event")))fail("IR_EVENT_CASES","Event subscription metadata differs from its actual region.",p);
                c.object["fir_region_id"]=string(rid);auto fields=array();
                for(const auto &np:n.ports)if(!np.input&&np.id.starts_with("event_data.")&&np.id.ends_with("@"+rid)) {
                    const auto end=np.id.rfind('@');const auto field=np.id.substr(11,end-11);auto entry=object();
                    entry.object={{"id",string(field)},{"port",string(np.id)},{"type",string(np.typeRef)},{"studio_type_schema",parse(np.fullTypeJson)}};fields.array.push_back(std::move(entry));
                }
                c.object["fir_fields"]=std::move(fields);
            }
            s.eventConfigurationJson=encode(event);
        }
        // The unqualified logical writers remain in Source mappings. Only the
        // active region-qualified physical inputs belong to the SDK artifact.
        if(s.kind=="case"||s.kind=="event_structure") {
            std::set<std::string> qualifiedBases;
            for(const auto &p:n.ports)if(const auto at=p.id.find('@');at!=std::string::npos)qualifiedBases.insert(p.id.substr(0,at));
            std::erase_if(n.ports,[&](const auto &p){return qualifiedBases.contains(p.id);});
        }
        out.structures.push_back(std::move(s));
    }
}
void Builder::write() {
    auto root=object(),meta=object(),unit=object(),objects=array(),connections=array(),regions=array(),maps=array(),relations=array();
    for(const auto &n:out.nodes) {
        const auto &raw=rawNodes.at(n.sourceNodeId);auto record=object(),attributes=object(),classification=object(),ports=array(),attr=object();
        const auto coercion=coercionOrigins.find(n.id);const auto errorBridge=errorBridgeOrigins.find(n.id);
        const RawEdge *supportEdge=coercion!=coercionOrigins.end()?coercion->second:
            errorBridge!=errorBridgeOrigins.end()?errorBridge->second.edge:nullptr;
        const bool support=supportEdge!=nullptr;
        const bool sourceEndpoint=errorBridge!=errorBridgeOrigins.end()&&errorBridge->second.sourceEndpoint;
        classification.object={{"primary_or_support",string(support?"support":"primary")},{"profile_id",string(std::string(composedSourceProfile))}};
        attributes.object={{"source_node_id",string(n.sourceNodeId)},{"region_id",string(n.regionId)},{"value_type",string(n.valueType)},{"authored_type",string(n.authoredKind)}};
        if(support)attributes.object["source_edge_id"]=*supportEdge->raw->member("id");
        if(!n.configurationJson.empty())attributes.object["configuration"]=parse(n.configurationJson);
        if(!n.fullTypeJson.empty())attributes.object["studio_type_schema"]=parse(n.fullTypeJson);
        if(n.literalValueJson)attributes.object["literal_value"]=parse(*n.literalValueJson);
        if(!n.literalProfile.empty())attributes.object["constant_profile"]=string(n.literalProfile);
        if(n.defaultValueJson)attributes.object["default_value"]=parse(*n.defaultValueJson);
        if(!n.widgetId.empty()) {attributes.object["widget_id"]=string(n.widgetId);attributes.object["widget_class"]=string(n.widgetClass);attributes.object["widget_label"]=string(n.widgetLabel);}
        if(n.objectReference)attributes.object[std::string(canonicalObjectReferenceAttribute)]=canonicalObjectReferenceAttributes(*n.objectReference);
        if(n.nativeCall) {
            attributes.object["frog_studio_native_call"]=canonicalNativeCallScalarAttributes(*n.nativeCall);
            auto requirement=object();requirement.object={{"requirement_id",string(std::string(kNativeCallProviderRequirement))},
                {"capabilities",strings({"native_call.scalar5.exact"})},{"binding",string("local_runtime_service")},{"version",string(std::string(kNativeCallScalarProfile))}};
            auto model=object();model.object={{"call_class",string("external_provider")},{"effect",string("provider_native")},
                {"status_model",string("local_result_tuple")},{"status_outputs",strings({"error_out"})},{"provider_requirement",std::move(requirement)}};
            attributes.object["call_model"]=std::move(model);
        }
        if(!n.selectedFields.empty())attributes.object["selected_fields"]=strings(n.selectedFields);
        if(n.kind==canonicalNumericAccumulateId) {
            attributes.object["effect"]=string("runtime_instance_state");
            attributes.object["state_access"]=string("read_accumulate");
            attributes.object["state_profile"]=string(std::string(canonicalNumericAccumulateProfile));
        }
        if(canonicalNumericRandomOperation(n.kind)) {
            attributes.object["effect"]=string("runtime_instance_state");
            attributes.object["state_access"]=string("read_advance");
            attributes.object["state_profile"]=string(std::string(canonicalNumericRandomProfile));
        }
        if(n.kind=="frog.studio.synchronization.first_call") {
            // A no-input operation is not a constant: its result belongs to an
            // actual visit of this node in its execution data-space.
            attributes.object["effect"]=string("runtime_instance_state");
            attributes.object["state_access"]=string("read_consume");
        }
        if(isEventFunction(n.kind)) {
            attributes.object["effect"]=string("runtime_synchronization");
            attributes.object["state_access"]=string("event_resource_operation");
        }
        if(isNotifierFunction(n.kind)) {
            attributes.object["effect"]=string("runtime_synchronization");
            attributes.object["state_access"]=string(isNotifierWaitFunction(n.kind)?"wait_receive":"resource_operation");
            if(isNotifierMultipleFunction(n.kind))attributes.object["notification_profile"]=string(std::string(kNotifierMultipleProfile));
        }
        if(isSemaphoreFunction(n.kind)) {
            attributes.object["effect"]=string("runtime_synchronization");
            attributes.object["state_access"]=string(isSemaphoreAcquireFunction(n.kind)?"wait_acquire":"resource_operation");
        }
        if(isRendezvousFunction(n.kind)) {
            attributes.object["effect"]=string("runtime_synchronization");
            attributes.object["state_access"]=string(isRendezvousWaitFunction(n.kind)?"wait_at_rendezvous":"resource_operation");
        }
        if(isOccurrenceFunction(n.kind)) {
            attributes.object["effect"]=string("runtime_synchronization");
            attributes.object["state_access"]=string(isOccurrenceWaitFunction(n.kind)?"wait_occurrence":
                n.kind=="frog.studio.synchronization.occurrences.generate"?"instance_signal_identity":"signal_occurrence");
        }
        if(std::find(canonicalFileNativeOperationIds().begin(),canonicalFileNativeOperationIds().end(),n.kind)!=canonicalFileNativeOperationIds().end()) {
            attributes.object["effect"]=string("external_filesystem_io");
            attributes.object["state_access"]=string("instance_file_cursor_or_explicit_path");
        }
        if(!n.collectionConfigurationJson.empty())attributes.object["collection"]=parse(n.collectionConfigurationJson);
        if(n.feedback) {
            auto feedback=object();Json delay;delay.type=Json::Type::Number;delay.string=std::to_string(n.feedback->delay);
            feedback.object={{"profile",string("frog.studio.feedback@1")},{"delay",std::move(delay)},
                {"initialization",string(n.feedback->initialization)},{"initializer_loop_id",string(n.feedback->initializerLoopId)},
                {"history",boolean(n.feedback->history)}};
            attributes.object["feedback"]=std::move(feedback);
        }
        for(const auto &p:n.ports) {auto v=object(),metadata=object();
            const auto originalPort=support?text(supportEdge->raw->member(sourceEndpoint?"from":"to")->member("port"),supportEdge->path):p.id.substr(0,p.id.find('@'));
            metadata.object={{"source_node_id",string(n.sourceNodeId)},{"source_port_id",string(originalPort)}};
            if(support)metadata.object["source_edge_id"]=*supportEdge->raw->member("id");
            v.object={{"id",string(p.id)},{"direction",string(p.input?"in":"out")},{"type_ref",string(p.typeRef)},{"studio_type_schema",parse(p.fullTypeJson)},{"metadata",std::move(metadata)}};ports.array.push_back(std::move(v));}
        if(n.family=="structure") {
            const auto s=std::find_if(out.structures.begin(),out.structures.end(),[&](const auto &s){return s.nodeId==n.id;});
            // Retain the exact source semantic descriptors, plus the explicit
            // normalized transfer identities used by downstream consumers.
            auto semantics=object();semantics.object={{"terminals",*raw.raw->member("structure_terminals")},{"boundary",*raw.raw->member("boundary")},{"stop_if_true",boolean(s->stopIfTrue)}};
            auto tunnels=array();
            for(const auto &t:s->tunnels) {
                auto value=object(),writers=array();
                for(const auto &w:t.writers){auto writer=object();writer.object={{"region_id",string(w.regionId)},{"input_port",string(w.inputPortId)}};writers.array.push_back(std::move(writer));}
                value.object={{"id",string(t.id)},{"type_ref",string(t.typeRef)},{"studio_type_schema",parse(t.fullTypeJson)},
                    {"input_port",string(t.inputPortId)},{"read_port",string(t.readPortId)},{"result_port",string(t.resultPortId)},
                    {"writers",std::move(writers)},{"use_default_if_unwired",boolean(t.useDefaultIfUnwired)},
                    {"mode",string(t.mode)},{"indexing_policy",string(t.indexingPolicy)}};
                if(t.mode=="indexing") {
                    value.object["external_type_ref"]=string(t.externalTypeRef);
                    value.object["external_studio_type_schema"]=parse(t.externalFullTypeJson);
                }
                if(!t.conditionPortId.empty())value.object["condition_port"]=string(t.conditionPortId);
                tunnels.array.push_back(std::move(value));
            }
            semantics.object["tunnels"]=std::move(tunnels);auto arms=array();for(const auto &a:s->caseArms){auto v=object();v.object={{"region_id",string(a.regionId)},{"default",boolean(a.defaultArm)}};if(a.matchJson)v.object["match"]=parse(*a.matchJson);arms.array.push_back(std::move(v));}
            semantics.object["case_arms"]=std::move(arms);semantics.object["selector_field_path"]=strings(s->caseSelectorFieldPath);
            if(!s->eventConfigurationJson.empty())semantics.object["events"]=parse(s->eventConfigurationJson);
            if(!s->timedLoopConfigurationJson.empty())semantics.object["timed_loop"]=parse(s->timedLoopConfigurationJson);
            if(!s->inPlaceConfigurationJson.empty())semantics.object["in_place"]=parse(s->inPlaceConfigurationJson);
            if(!s->sequenceProfile.empty()) {
                auto sequence=object(),transfers=array();
                for(const auto &t:s->sequenceTransfers){auto transfer=object();transfer.object={{"id",string(t.id)},
                    {"type_ref",string(t.typeRef)},{"studio_type_schema",parse(t.fullTypeJson)},
                    {"input_port",string(t.inputPortId)},{"output_port",string(t.outputPortId)},
                    {"source_region",string(t.sourceRegionId)},{"target_region",string(t.targetRegionId)},
                    {"use_default_if_unwired",boolean(t.useDefaultIfUnwired)}};transfers.array.push_back(std::move(transfer));}
                sequence.object={{"profile",string("frog.studio.sequence@1")},{"semantic_profile",string(s->sequenceProfile)},{"transfers",std::move(transfers)}};
                semantics.object["sequence"]=std::move(sequence);
            }
            attributes.object["structure"]=std::move(semantics);record.object["regions_owned"]=strings(s->regionIds);
        }
        attr.object={{"source_refs",support?strings({ref(supportEdge->path),ref(raw.path)}):strings({ref(raw.path)})},{"mode",string("direct")}};
        record.object.insert({{"id",string(n.id)},{"family",string(n.family)},{"kind",string(n.kind)},{"role",string(n.role)},{"classification",std::move(classification)},{"attributes",std::move(attributes)},{"ports",std::move(ports)},{"attribution",std::move(attr)}});objects.array.push_back(std::move(record));
    }
    for(const auto &e:out.connections) {auto v=object(),a=object(),b=object();a.object={{"object_id",string(e.from.objectId)},{"port_id",string(e.from.portId)}};b.object={{"object_id",string(e.to.objectId)},{"port_id",string(e.to.portId)}};v.object={{"id",string(e.id)},{"kind",string("dataflow")},{"from",std::move(a)},{"to",std::move(b)},{"source_edge_id",string(e.sourceEdgeId)}};connections.array.push_back(std::move(v));}
    for(const auto &r:out.regions) {auto v=object();v.object={{"id",string(r.id)},{"owner_object_id",string(r.ownerNodeId)},{"role",string(r.role)},{"member_object_ids",strings(r.memberNodeIds)}};regions.array.push_back(std::move(v));map(encodedId("map-region",r.id),"other",r.sourcePointer,r.id,"region");}
    for(const auto &m:out.maps) {auto v=object();v.object={{"id",string(m.id)},{"source_kind",string(m.sourceKind)},{"source_ref",string(m.sourceRef)},{"target_kind",string(m.targetKind)},{"target_refs",strings(m.targetRefs)},{"relation",string(m.relation)}};maps.array.push_back(std::move(v));}
    for(const auto key:{"interface","icon","connector","ide","host","execution_policy"})if(source.section(key)) {CanonicalIrCorrespondence c{"section:"+std::string(key),ref("/"+std::string(key)),"declaration_reference",{}};auto v=object();v.object={{"id",string(c.id)},{"source_ref",string(c.sourceRef)},{"relation_kind",string(c.relationKind)},{"target_refs",array()}};relations.array.push_back(std::move(v));out.correspondence.push_back(std::move(c));}
    if(out.maps.size()+out.correspondence.size()>options.limits.maximumMappingRecords)fail("IR_RESOURCE_LIMIT","Mapping budget exceeded.","/",true);
    meta.object={{"producer",string("frog.native.canonical-structured-ir")},{"producer_version",string("2")},{"profile",string(std::string(composedSourceProfile))},{"source_sha256",string(out.hash)}};
    unit.object={{"id",string(out.unitId)},{"objects",std::move(objects)},{"connections",std::move(connections)},{"regions",std::move(regions)},{"source_map",std::move(maps)},{"correspondence",std::move(relations)}};
    root.object={{"schema_family",string("frog.execution_ir")},{"schema_version",string("0.1")},{"document_id",string(out.documentId)},{"metadata",std::move(meta)},{"unit",std::move(unit)}};
    out.bytes=encode(root);if(out.bytes.size()>options.limits.maximumIrBytes)fail("IR_RESOURCE_LIMIT","Composed FIR exceeds its existing byte budget.","/",true);
}
}
bool composed(const FrogSourceDocument &s) noexcept {
    const auto *d=s.section("diagram"),*p=d?d->member("profile"):nullptr;
    return p&&p->type==Json::Type::String&&p->string==composedSourceProfile;
}
std::optional<Draft> buildComposed(const FrogSourceDocument &s,std::string_view documentId,
    const CanonicalIrReadOptions &options,std::vector<CanonicalIrDiagnostic> *diagnostics) {
    try {
        if(!composed(s)||s.envelopeStatus()!=FrogSourceEnvelopeStatus::Recognized||!s.diagnostics().empty())fail("IR_COMPOSED_SOURCE","A recognized internally consistent composed Source is required.","/");
        Builder b{s,options};b.out.documentId=documentId;b.out.unitId="unit:main";b.out.hash="sha256:"+hashing::sha256Hex(s.originalUtf8());
        {const CanonicalFrontendPhaseScope semantic(options.metrics,CanonicalFrontendPhase::Semantics);
            b.declarations();b.index(objectAt(s.section("diagram"),"/diagram"),"/diagram");b.nodes();b.edges();b.validateFileConfigurations();b.structures();b.validateScopes();}
        {const CanonicalFrontendPhaseScope construction(options.metrics,CanonicalFrontendPhase::FirConstruction);b.write();}
        return std::move(b.out);
    } catch(const Failure &f){if(diagnostics)diagnostics->push_back(f.diagnostic);return {};}
    catch(const std::exception &f){if(diagnostics)diagnostics->push_back({"IR_COMPOSED_INVALID",f.what(),"/",false});return {};}
}
}
