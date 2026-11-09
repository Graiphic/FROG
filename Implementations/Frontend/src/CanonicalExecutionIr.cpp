#include "frog_native_engine/CanonicalFrontendMetrics.hpp"
#include "frog_native_engine/CanonicalExecutionIr.hpp"
#include "CanonicalExecutionIrStorage.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "CanonicalStructuredExecutionIr.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <charconv>
#include <cmath>
#include <deque>
#include <map>
#include <set>
#include <stdexcept>

namespace frog::native_engine {
namespace {
using Json=source::JsonValue;
struct IntakeError {CanonicalIrDiagnostic diagnostic;};
[[noreturn]] void fail(std::string code,std::string message,std::string location,bool unsupported=false) {
    throw IntakeError{{std::move(code),std::move(message),std::move(location),unsupported}};
}
const Json &object(const Json *value,std::string_view at) {
    if(!value || value->type!=Json::Type::Object)fail("IR_OBJECT","An object is required.",std::string(at));
    return *value;
}
const std::vector<Json> &array(const Json *value,std::string_view at,std::size_t maximum,bool optional=false) {
    static const std::vector<Json> empty;
    if(!value && optional)return empty;
    if(!value || value->type!=Json::Type::Array)fail("IR_ARRAY","An array is required.",std::string(at));
    if(value->array.size()>maximum)fail("IR_RESOURCE_LIMIT","Collection exceeds the admitted work budget.",std::string(at),true);
    return value->array;
}
std::string text(const Json *value,std::string_view at,bool optional=false) {
    if(!value && optional)return {};
    if(!value || value->type!=Json::Type::String || value->string.empty())fail("IR_STRING","A nonempty string is required.",std::string(at));
    return value->string;
}
void fields(const Json &value,std::initializer_list<std::string_view> allowed,std::string_view at) {
    for(const auto &[key,_]:value.object)
        if(std::find(allowed.begin(),allowed.end(),key)==allowed.end())
            fail("IR_UNSUPPORTED_FIELD","A field is outside the admitted canonical corridor: "+key,std::string(at),true);
}
bool canonicalId(std::string_view id) {
    return !id.empty() && std::all_of(id.begin(),id.end(),[](unsigned char c){
        return (c>='a'&&c<='z') || (c>='A'&&c<='Z') || (c>='0'&&c<='9') || c=='.' || c=='_' || c==':' || c=='-';});
}
std::string id(const Json *value,std::string_view at,std::size_t maximum,bool canonical=true) {
    auto result=text(value,at);
    if(result.size()>maximum)fail("IR_RESOURCE_LIMIT","Identity exceeds the admitted byte budget.",std::string(at),true);
    if(canonical && !canonicalId(result))fail("IR_ID","Canonical IR identity has invalid syntax.",std::string(at));
    return result;
}
double number(const Json *value,std::string_view at) {
    if(!value || value->type!=Json::Type::Number)fail("IR_F64_VALUE","An actual f64 number is required.",std::string(at));
    double result=0;
    const auto parsed=std::from_chars(value->string.data(),value->string.data()+value->string.size(),result);
    if(parsed.ec!=std::errc{} || parsed.ptr!=value->string.data()+value->string.size() || !std::isfinite(result))
        fail("IR_F64_VALUE","The numeric value must be a finite representable f64.",std::string(at));
    // Preserve the source reader's exact integer policy: an integer token must
    // not silently become a different integer through a binary64 projection.
    if(value->string.find_first_of(".eE")==std::string::npos) {
        // JSON's integer -0 has the same admitted integer value as 0 in the
        // canonical producer. Floating -0.0 remains a distinct f64 bit value.
        if(value->string=="-0")return 0.;
        std::array<char,512> decimal{};
        const auto written=std::to_chars(decimal.data(),decimal.data()+decimal.size(),result,std::chars_format::fixed,0);
        if(written.ec!=std::errc{} || std::string_view(decimal.data(),static_cast<std::size_t>(written.ptr-decimal.data()))!=value->string)
            fail("IR_F64_VALUE","The integer literal is not exactly representable in f64.",std::string(at));
    }
    return result;
}
void stringArray(const Json *value,std::string_view at,std::size_t maximum,bool optional=true) {
    for(const auto &entry:array(value,at,maximum,optional))
        if(entry.type!=Json::Type::String)fail("IR_STRING", "A string array is required.",std::string(at));
}
void validateMetadata(const Json *value,std::string_view at) {
    if(value)object(value,at);
}
bool sameNumber(double a,double b) {return std::bit_cast<std::uint64_t>(a)==std::bit_cast<std::uint64_t>(b);}
struct SourceItem {const Json *value;std::string pointer;};
const Json &resolvePointer(const Json &root,std::string_view pointer) {
    if(pointer.empty())return root;
    if(pointer.front()!='/')fail("IR_SOURCE_REF","A source reference must use a JSON pointer.",std::string(pointer));
    const Json *value=&root;
    while(!pointer.empty()) {
        pointer.remove_prefix(1);
        const auto end=pointer.find('/');const auto token=pointer.substr(0,end);
        std::string key;
        for(std::size_t i=0;i<token.size();++i) {
            if(token[i]=='~') {
                if(++i==token.size() || (token[i]!='0' && token[i]!='1'))fail("IR_SOURCE_REF","Invalid JSON pointer escape.",std::string(pointer));
                key+=token[i]=='0'?'~':'/';
            } else key+=token[i];
        }
        if(value->type==Json::Type::Object)value=value->member(key);
        else if(value->type==Json::Type::Array) {
            std::size_t index=0;const auto read=std::from_chars(key.data(),key.data()+key.size(),index);
            if(key.empty() || (key.size()>1 && key.front()=='0') || read.ec!=std::errc{} || read.ptr!=key.data()+key.size() || index>=value->array.size())value=nullptr;
            else value=&value->array[index];
        } else value=nullptr;
        if(!value)fail("IR_SOURCE_REF","Source attribution does not resolve into the authoritative source.",std::string(pointer));
        if(end==pointer.npos)break;
        pointer.remove_prefix(end);
    }
    return *value;
}
struct Intake {
    const Json &tree;const FrogSourceDocument &source;const CanonicalIrReadOptions &options;
    std::string documentId,unitId,hash;
    std::vector<CanonicalIrNode> nodes;
    std::vector<CanonicalIrConnection> connections;
    std::vector<CanonicalIrSourceMap> maps;
    std::vector<CanonicalIrCorrespondence> relations;
    std::map<std::string,SourceItem> sourceNodes,sourceEdges,widgets;
    std::map<std::string,std::size_t> nodeIndex;
    std::set<std::string> allIds;
    std::map<std::string,std::vector<std::string>> objectCorrespondence;
    std::map<std::string,std::string> widgetParticipants;
    const CanonicalIrLimits &limits()const{return options.limits;}
    std::string ref(std::string_view pointer)const{return options.sourceUri+"#"+std::string(pointer);}
    const Json &sourceRef(std::string_view uri)const {
        const auto prefix=options.sourceUri+"#";
        if(!uri.starts_with(prefix))fail("IR_SOURCE_REF","Source URI differs from the exact authoritative source identity.",std::string(uri));
        return resolvePointer(source.root(),uri.substr(prefix.size()));
    }
    std::string unique(const Json *value,std::string_view at) {
        auto valueId=id(value,at,limits().maximumIrIdBytes);
        if(!allIds.insert(valueId).second)fail("IR_DUPLICATE_ID","IR identity is duplicated.",std::string(at));
        return valueId;
    }
    std::vector<std::string> targets(const Json *value,std::string_view at,bool optional=false) {
        std::vector<std::string> result;std::set<std::string> seen;
        for(const auto &entry:array(value,at,limits().maximumNodes,optional)) {
            auto valueId=id(&entry,at,limits().maximumIrIdBytes);
            if(!seen.insert(valueId).second)fail("IR_DUPLICATE_TARGET","A target reference is duplicated.",std::string(at));
            result.push_back(std::move(valueId));
        }
        return result;
    }
    std::set<std::string> attribution(const Json &record,std::string_view at) {
        const auto &origin=object(record.member("attribution"),at);fields(origin,{"source_refs","mode"},at);
        const auto mode=text(origin.member("mode"),at);
        if(mode!="direct" && mode!="multi_contributor")fail("IR_ATTRIBUTION","Unsupported primary attribution mode.",std::string(at));
        std::set<std::string> refs;
        for(const auto &entry:array(origin.member("source_refs"),at,8)) {
            const auto uri=text(&entry,at);sourceRef(uri);
            if(!refs.insert(uri).second)fail("IR_ATTRIBUTION","An attribution contributor is duplicated.",std::string(at));
        }
        if(refs.empty() || (mode=="direct" && refs.size()!=1) || (mode=="multi_contributor" && refs.size()<2))
            fail("IR_ATTRIBUTION","Attribution mode does not match its contributors.",std::string(at));
        return refs;
    }
    void collect(std::map<std::string,SourceItem> &inventory,const Json *items,std::string_view pointer,std::size_t maximum) {
        const auto &values=array(items,pointer,maximum);
        for(std::size_t i=0;i<values.size();++i) {
            const auto at=std::string(pointer)+"/"+std::to_string(i);const auto &entry=object(&values[i],at);
            const auto valueId=id(entry.member("id"),at,limits().maximumSourceIdBytes,false);
            if(!inventory.emplace(valueId,SourceItem{&entry,at}).second)fail("IR_SOURCE_DUPLICATE_ID","Authoritative source identity is duplicated.",at);
        }
    }
    void validateSource() {
        if(source.envelopeStatus()!=FrogSourceEnvelopeStatus::Recognized)
            fail("IR_SOURCE_UNSUPPORTED","Authoritative source is outside the current source envelope.","/",true);
        const auto &root=object(&source.root(),"/");
        fields(root,{"spec_version","metadata","interface","diagram","front_panel","connector","icon","ide","cache","host","execution_policy"},"/");
        const auto &boundary=object(root.member("interface"),"/interface");fields(boundary,{"inputs","outputs"},"/interface");
        for(const auto side:{"inputs","outputs"})if(!array(boundary.member(side),"/interface",limits().maximumNodes,true).empty())
            fail("IR_PUBLIC_INTERFACE_UNSUPPORTED","Public ports require a real public boundary adapter.","/interface",true);
        for(const auto section:{"host","execution_policy"}) {
            if(const auto *policy=root.member(section);policy && !object(policy,section).object.empty())
                fail("IR_SOURCE_POLICY_UNSUPPORTED","Source policy is not carried by this initial adapter.",section,true);
        }
        const auto &diagram=object(root.member("diagram"),"/diagram");fields(diagram,{"nodes","edges","annotations","wire_fragments"},"/diagram");
        if(!array(diagram.member("wire_fragments"),"/diagram/wire_fragments",limits().maximumEdges,true).empty())
            fail("IR_INCOMPLETE_SOURCE","Incomplete source wires cannot enter native IR intake.","/diagram/wire_fragments");
        collect(sourceNodes,diagram.member("nodes"),"/diagram/nodes",limits().maximumNodes);
        collect(sourceEdges,diagram.member("edges"),"/diagram/edges",limits().maximumEdges);
        for(const auto &[_,item]:sourceNodes) {
            const auto family=text(item.value->member("kind"),item.pointer);
            if(family!="primitive" && family!="widget_value")
                fail("IR_SOURCE_FAMILY_UNSUPPORTED","Source execution family has no implementation in this corridor.",item.pointer,true);
        }
        const auto &panel=object(root.member("front_panel"),"/front_panel");fields(panel,{"canvas","widgets"},"/front_panel");
        collect(widgets,panel.member("widgets"),"/front_panel/widgets",limits().maximumWidgets);
        for(const auto &[wid,item]:widgets) {
            const auto &widget=*item.value;fields(widget,{"id","role","widget","class_ref","value_type","layout","props","parts","doc","tags"},item.pointer);
            const auto klass=text(widget.member("widget")?widget.member("widget"):widget.member("class_ref"),item.pointer);
            const auto role=text(widget.member("role"),item.pointer);
            if((klass!="frog.ui.standard.numeric_control" && klass!="frog.widgets.numeric_control" && klass!="frog.ui.standard.numeric_indicator" && klass!="frog.widgets.numeric_indicator") ||
                role!=(klass.ends_with("_control")?"control":"indicator"))fail("IR_WIDGET_UNSUPPORTED","Widget class and role must match the admitted Numeric family.",item.pointer,true);
            if(widget.member("widget") && widget.member("class_ref") && text(widget.member("class_ref"),item.pointer)!=klass)
                fail("IR_SOURCE_CLASS_CONFLICT","Widget class references conflict.",item.pointer);
            if(text(widget.member("value_type"),item.pointer)!="f64")fail("IR_TYPE_UNSUPPORTED","Only f64 value participation is admitted.",item.pointer,true);
            if(const auto *props=widget.member("props")) {
                const auto at=item.pointer+"/props";
                object(props,at);fields(*props,{"default_value","data_type.representation",
                    "data_type.named_numeric_size","representation.kind","data_entry.use_type_defaults"},at);
                // The real Numeric writer names the same f64 representation in
                // all three forms. Admit only that exact quartet, not arbitrary
                // properties or a switch to the structured/composed corridor.
                for(const auto &[name,expected]:{std::pair<std::string_view,std::string_view>{"data_type.representation","f64"},
                    {"data_type.named_numeric_size","Float64"},{"representation.kind","float64"}})
                    if(const auto *value=props->member(name);value &&
                       (value->type!=Json::Type::String || value->string!=expected))
                        fail("IR_SOURCE_TYPE_CONFLICT","Numeric representation metadata differs from the declared f64.",at+"/"+std::string(name));
                if(const auto *defaults=props->member("data_entry.use_type_defaults");defaults && defaults->type!=Json::Type::Boolean)
                    fail("IR_WIDGET_PROPERTY_INVALID","Numeric data_entry.use_type_defaults must be Boolean.",at+"/data_entry.use_type_defaults");
                // This is interactive input policy, never a new dataflow value.
                // true uses the finite f64 domain and natural step 1. false with
                // no overrides has those same effective defaults. Custom range,
                // response and step fields stay explicitly outside this subset.
                // Keep the property verbatim in the authoritative source sealed
                // by FIR; the existing native Numeric view owns its realization.
                if(props->member("default_value"))number(props->member("default_value"),at+"/default_value");
            }
            const auto *props=widget.member("props");
            if(role=="control" && (!props || !props->member("default_value")))fail("IR_DEFAULT_UNSUPPORTED","No implicit control default is invented.",item.pointer,true);
            if(const auto *parts=widget.member("parts")) {
                object(parts,item.pointer);fields(*parts,{"label","caption"},item.pointer);
                for(const auto &[name,part]:parts->object) {
                    const auto at=item.pointer+"/parts/"+name;
                    object(&part,at);fields(part,{"class","class_ref","props","layout"},at);
                    const auto klass=text(part.member("class")?part.member("class"):part.member("class_ref"),at);
                    if(klass!="frog.ui.standard.label_part" && klass!="frog.widgets.label")
                        fail("IR_WIDGET_PART_UNSUPPORTED","Only published label/caption parts are admitted.",at,true);
                    if(part.member("class") && part.member("class_ref") && text(part.member("class_ref"),at)!=klass)
                        fail("IR_SOURCE_CLASS_CONFLICT","Widget part class references conflict.",at);
                    if(const auto *partProps=part.member("props")) {
                        object(partProps,at);fields(*partProps,{"text","visible","style"},at);
                        if(const auto *label=partProps->member("text");label && label->type!=Json::Type::String)
                            fail("IR_STRING","Label/caption text must be a string.",at);
                        if(const auto *visible=partProps->member("visible");visible && visible->type!=Json::Type::Boolean)
                            fail("IR_BOOLEAN","Label/caption visibility must be Boolean.",at);
                    }
                }
            }
        }
    }
    void readNodes(const Json &unit) {
        for(const auto &raw:array(unit.member("objects"),"/unit/objects",limits().maximumNodes)) {
            const auto &record=object(&raw,"/unit/objects");
            fields(record,{"id","family","kind","role","classification","ports","attributes","attribution","correspondence_refs"},"/unit/objects");
            CanonicalIrNode node;node.id=unique(record.member("id"),"/unit/objects/id");
            node.valueType="f64";
            node.family=text(record.member("family"),node.id);node.kind=text(record.member("kind"),node.id);
            if(node.family!="primitive" && node.family!="widget_value")fail("IR_FAMILY_UNSUPPORTED","The canonical execution family is not implemented by this native intake.",node.id,true);
            const auto &attrs=object(record.member("attributes"),node.id);
            fields(attrs,node.family=="primitive"?std::initializer_list<std::string_view>{"source_node_id"}:std::initializer_list<std::string_view>{"source_node_id","widget_id","widget_class","value_type","default_value"},node.id);
            node.sourceNodeId=id(attrs.member("source_node_id"),node.id,limits().maximumSourceIdBytes,false);
            const auto sourceNode=sourceNodes.find(node.sourceNodeId);
            if(sourceNode==sourceNodes.end() || text(sourceNode->second.value->member("kind"),node.id)!=node.family)
                fail("IR_SOURCE_NODE_MISMATCH","IR object does not match an actual source node family.",node.id);
            const auto &sourceNodeValue=*sourceNode->second.value;
            fields(sourceNodeValue,node.family=="primitive"?std::initializer_list<std::string_view>{"id","kind","type","layout","doc","tags"}:std::initializer_list<std::string_view>{"id","kind","widget","layout","doc","tags"},sourceNode->second.pointer);
            std::set<std::string> expected{ref(sourceNode->second.pointer)};
            const auto &classification=object(record.member("classification"),node.id);fields(classification,{"primary_or_support","library_id"},node.id);
            if(text(classification.member("primary_or_support"),node.id)!="primary")fail("IR_CLASSIFICATION","Admitted objects must remain primary execution objects.",node.id);
            if(node.family=="primitive") {
                if(record.member("role"))fail("IR_ROLE_UNSUPPORTED","A role on the admitted primitive has no defined execution meaning.",node.id,true);
                if(node.kind!="frog.core.add" || text(sourceNodeValue.member("type"),node.id)!=node.kind)
                    fail("IR_PRIMITIVE_UNSUPPORTED","Only the real two-input frog.core.add primitive is admitted.",node.id,true);
                if(text(classification.member("library_id"),node.id)!="frog.core")fail("IR_CLASSIFICATION","Add must retain its public core-library identity.",node.id);
            } else {
                if(classification.member("library_id"))fail("IR_CLASSIFICATION_UNSUPPORTED","Widget participation has no admitted execution-library classification.",node.id,true);
                if(node.kind!="widget_value")fail("IR_WIDGET_KIND","Natural widget participation must remain widget_value.",node.id);
                node.widgetId=id(attrs.member("widget_id"),node.id,limits().maximumSourceIdBytes,false);
                const auto widget=widgets.find(node.widgetId);
                if(widget==widgets.end() || text(sourceNodeValue.member("widget"),node.id)!=node.widgetId)
                    fail("IR_WIDGET_MISMATCH","Widget participation does not refer to its actual declared widget.",node.id);
                if(!widgetParticipants.emplace(node.widgetId,node.id).second)fail("IR_WIDGET_UNSUPPORTED","Repeated natural widget participation is not admitted.",node.id,true);
                const auto &decl=*widget->second.value;
                node.role=text(record.member("role"),node.id);node.widgetClass=text(attrs.member("widget_class"),node.id);
                if(node.role!=text(decl.member("role"),node.id) || node.widgetClass!=text(decl.member("widget")?decl.member("widget"):decl.member("class_ref"),node.id) || text(attrs.member("value_type"),node.id)!="f64")
                    fail("IR_WIDGET_MISMATCH","IR widget role/class/type differs from authoritative source.",node.id);
                const auto *props=decl.member("props");const auto *sourceDefault=props?props->member("default_value"):nullptr;
                const auto *irDefault=attrs.member("default_value");
                if(bool(sourceDefault)!=bool(irDefault))fail("IR_DEFAULT_MISMATCH","IR default presence differs from source.",node.id);
                if(sourceDefault) {node.defaultValue=number(irDefault,node.id);node.defaultValueJson=sourceDefault->string;if(!sameNumber(*node.defaultValue,number(sourceDefault,node.id)))fail("IR_DEFAULT_MISMATCH","IR default changed the original f64 bits.",node.id);}
                expected.insert(ref(widget->second.pointer));
            }
            const std::map<std::string,bool> expectedPorts=node.family=="primitive"?std::map<std::string,bool>{{"a",true},{"b",true},{"result",false}}:
                std::map<std::string,bool>{{"value",node.role=="indicator"}};
            std::set<std::string> seen;
            for(const auto &portRaw:array(record.member("ports"),node.id,3)) {
                const auto &port=object(&portRaw,node.id);fields(port,{"id","direction","role","type_ref","boundary_kind","metadata"},node.id);
                CanonicalIrPort p;p.id=id(port.member("id"),node.id,limits().maximumSourceIdBytes);
                const auto found=expectedPorts.find(p.id);
                if(found==expectedPorts.end() || !seen.insert(p.id).second)fail("IR_PORT_SIGNATURE","A port is absent from the real admitted signature, or duplicated.",node.id);
                p.input=found->second;p.typeRef=text(port.member("type_ref"),node.id);
                if(text(port.member("direction"),node.id)!=(p.input?"in":"out") || p.typeRef!="f64" || text(port.member("role"),node.id)!="data" || text(port.member("boundary_kind"),node.id)!="none")
                    fail("IR_PORT_SIGNATURE","A port lost its actual direction, resolved type or participation role.",node.id);
                const auto &metadata=object(port.member("metadata"),node.id);fields(metadata,{"source_node_id","source_port_id"},node.id);
                if(text(metadata.member("source_node_id"),node.id)!=node.sourceNodeId || text(metadata.member("source_port_id"),node.id)!=p.id)
                    fail("IR_PORT_SOURCE","A port lost its owning node/terminal source identity.",node.id);
                node.ports.push_back(std::move(p));
            }
            if(seen.size()!=expectedPorts.size())fail("IR_PORT_SIGNATURE","Required actual ports are missing.",node.id);
            if(attribution(record,node.id)!=expected)fail("IR_ATTRIBUTION","Object contributors differ from authoritative source.",node.id);
            objectCorrespondence[node.id]=targets(record.member("correspondence_refs"),node.id);
            nodeIndex.emplace(node.id,nodes.size());nodes.push_back(std::move(node));
        }
        // Empty Source has empty FIR; coverage still forbids erasing authored nodes.
        if(nodes.size()!=sourceNodes.size())fail("IR_SOURCE_NODE_COVERAGE","Every actual source node must have one admitted IR object.","/unit/objects");
        std::set<std::string> sourceIds;for(const auto &node:nodes)sourceIds.insert(node.sourceNodeId);
        if(sourceIds.size()!=sourceNodes.size())fail("IR_SOURCE_NODE_COVERAGE","A source node has multiple IR participants or was erased.","/unit/objects");
    }
    const CanonicalIrPort &port(const CanonicalIrEndpoint &endpoint,bool input)const {
        const auto found=nodeIndex.find(endpoint.objectId);
        if(found==nodeIndex.end())fail("IR_ENDPOINT","Connection object does not exist.",endpoint.objectId);
        const auto &node=nodes[found->second];
        const auto p=std::find_if(node.ports.begin(),node.ports.end(),[&](const auto &p){return p.id==endpoint.portId;});
        if(p==node.ports.end() || p->input!=input)fail("IR_ENDPOINT","Connection does not reach an actual port with the correct direction.",endpoint.objectId);
        return *p;
    }
    void readEdges(const Json &unit) {
        std::set<std::pair<std::string,std::string>> drivers;std::set<std::string> sourceIds;
        std::map<std::string,std::vector<std::string>> adjacency;std::map<std::string,std::size_t> indegree;
        for(const auto &node:nodes)indegree[node.id]=0;
        for(const auto &raw:array(unit.member("connections"),"/unit/connections",limits().maximumEdges)) {
            const auto &record=object(&raw,"/unit/connections");fields(record,{"id","kind","from","to","attribution","metadata"},"/unit/connections");
            CanonicalIrConnection edge;edge.id=unique(record.member("id"),"/unit/connections/id");
            if(text(record.member("kind"),edge.id)!="dataflow")fail("IR_CONNECTION_UNSUPPORTED","Boundary/state/UI sequencing is not ordinary dataflow.",edge.id,true);
            const auto &metadata=object(record.member("metadata"),edge.id);fields(metadata,{"source_edge_id","type_ref"},edge.id);
            edge.sourceEdgeId=id(metadata.member("source_edge_id"),edge.id,limits().maximumSourceIdBytes,false);
            const auto original=sourceEdges.find(edge.sourceEdgeId);
            if(original==sourceEdges.end() || !sourceIds.insert(edge.sourceEdgeId).second || text(metadata.member("type_ref"),edge.id)!="f64")
                fail("IR_SOURCE_EDGE_MISMATCH","IR connection identity/type does not match a unique real source edge.",edge.id);
            fields(*original->second.value,{"id","from","to","bends"},original->second.pointer);
            for(const auto side:{"from","to"}) {
                auto &endpoint=std::string_view(side)=="from"?edge.from:edge.to;
                const auto &value=object(record.member(side),edge.id);fields(value,{"object_id","port_id"},edge.id);
                endpoint.objectId=id(value.member("object_id"),edge.id,limits().maximumIrIdBytes);
                endpoint.portId=id(value.member("port_id"),edge.id,limits().maximumSourceIdBytes);
                port(endpoint,std::string_view(side)=="to");
                const auto &sourceEnd=object(original->second.value->member(side),edge.id);fields(sourceEnd,{"node","port"},edge.id);
                if(text(sourceEnd.member("node"),edge.id)!=nodes[nodeIndex.at(endpoint.objectId)].sourceNodeId || text(sourceEnd.member("port"),edge.id)!=endpoint.portId)
                    fail("IR_SOURCE_EDGE_MISMATCH","IR connection changed an actual source terminal association.",edge.id);
            }
            if(!drivers.emplace(edge.to.objectId,edge.to.portId).second)fail("IR_MULTIPLE_DRIVERS","An input has multiple ordinary drivers.",edge.id);
            adjacency[edge.from.objectId].push_back(edge.to.objectId);++indegree[edge.to.objectId];
            const std::set<std::string> expected{ref(original->second.pointer)};
            if(attribution(record,edge.id)!=expected)fail("IR_ATTRIBUTION","Edge attribution no longer reaches the original real connection.",edge.id);
            connections.push_back(std::move(edge));
        }
        if(connections.size()!=sourceEdges.size())fail("IR_SOURCE_EDGE_COVERAGE","Every actual source connection must remain present.","/unit/connections");
        for(const auto &node:nodes)for(const auto &p:node.ports)if(p.input && !drivers.contains({node.id,p.id}))fail("IR_MISSING_DRIVER","An admitted input lost its real driver.",node.id);
        std::deque<std::string> queue;for(const auto &[node,degree]:indegree)if(degree==0)queue.push_back(node);
        std::size_t visited=0;while(!queue.empty()) {auto node=std::move(queue.front());queue.pop_front();++visited;for(const auto &dest:adjacency[node])if(--indegree[dest]==0)queue.push_back(dest);}
        if(visited!=nodes.size())fail("IR_COMBINATIONAL_CYCLE","Native intake cannot invent memory to repair a cycle.","/unit/connections");
    }
    void readMappings(const Json &unit) {
        std::set<std::string> mappedNodes,mappedEdges,mappedWidgets,mappedDefaults;
        std::map<std::string,const CanonicalIrConnection *> edgeIndex;for(const auto &edge:connections)edgeIndex.emplace(edge.id,&edge);
        for(const auto &raw:array(unit.member("source_map"),"/unit/source_map",limits().maximumMappingRecords)) {
            const auto &record=object(&raw,"/unit/source_map");fields(record,{"id","source_kind","source_ref","target_kind","target_refs","relation","notes"},"/unit/source_map");
            stringArray(record.member("notes"),"/unit/source_map/notes",limits().maximumMappingRecords);
            CanonicalIrSourceMap map;map.id=unique(record.member("id"),"/unit/source_map/id");map.sourceKind=text(record.member("source_kind"),map.id);
            map.sourceRef=text(record.member("source_ref"),map.id);const auto &origin=sourceRef(map.sourceRef);
            map.targetKind=text(record.member("target_kind"),map.id);map.relation=text(record.member("relation"),map.id);map.targetRefs=targets(record.member("target_refs"),map.id);
            if(map.sourceKind=="diagram_node") {
                if(map.targetKind!="object" || map.relation!="direct_primary" || map.targetRefs.size()!=1 || !nodeIndex.contains(map.targetRefs.front()) ||
                    &origin!=sourceNodes.at(nodes[nodeIndex.at(map.targetRefs.front())].sourceNodeId).value || !mappedNodes.insert(map.targetRefs.front()).second)
                    fail("IR_SOURCE_MAP","A diagram-node mapping lost its actual one-to-one contribution.",map.id);
            } else if(map.sourceKind=="diagram_edge") {
                if(map.targetKind!="connection" || map.relation!="direct_primary" || map.targetRefs.size()!=1 || !edgeIndex.contains(map.targetRefs.front()) ||
                    &origin!=sourceEdges.at(edgeIndex.at(map.targetRefs.front())->sourceEdgeId).value || !mappedEdges.insert(map.targetRefs.front()).second)
                    fail("IR_SOURCE_MAP","An edge mapping lost actual edge identity or attribution.",map.id);
            } else if(map.sourceKind=="widget_instance") {
                const auto widget=std::find_if(widgets.begin(),widgets.end(),[&](const auto &entry){return entry.second.value==&origin;});
                if(widget==widgets.end() || !mappedWidgets.insert(widget->first).second || map.relation!="non_primary")fail("IR_WIDGET_MAP","Widget declarations must remain distinct and non-primary.",map.id);
                const auto participant=widgetParticipants.find(widget->first);
                if(participant==widgetParticipants.end()) {if(map.targetKind!="none" || !map.targetRefs.empty())fail("IR_WIDGET_MAP","An unused declaration must not invent execution participation.",map.id);}
                else if(map.targetKind!="object" || map.targetRefs!=std::vector<std::string>{participant->second})fail("IR_WIDGET_MAP","Widget declaration changed its actual participant.",map.id);
            } else if(map.sourceKind=="widget_member") {
                const auto widget=std::find_if(widgets.begin(),widgets.end(),[&](const auto &entry){const auto *props=entry.second.value->member("props");return props && props->member("default_value")==&origin;});
                if(widget==widgets.end())fail("IR_WIDGET_MAP","Only the actual admitted numeric default is a value contributor.",map.id);
                if(!mappedDefaults.insert(widget->first).second)fail("IR_WIDGET_MAP","A numeric default mapping is duplicated.",map.id);
                const auto participant=widgetParticipants.find(widget->first);
                if(participant==widgetParticipants.end()) {if(map.targetKind!="none" || map.relation!="non_primary" || !map.targetRefs.empty())fail("IR_WIDGET_MAP","Unused default acquired execution authority.",map.id);}
                else if(map.targetKind!="object" || map.relation!="direct_primary" || map.targetRefs!=std::vector<std::string>{participant->second})fail("IR_WIDGET_MAP","Numeric default attribution changed its participant.",map.id);
            } else fail("IR_MAPPING_UNSUPPORTED","Source mapping family is outside this admitted corridor.",map.id,true);
            maps.push_back(std::move(map));
        }
        if(mappedNodes.size()!=nodes.size() || mappedEdges.size()!=connections.size() || mappedWidgets.size()!=widgets.size())
            fail("IR_SOURCE_MAP_COVERAGE","Nodes, edges and widget declarations require complete source maps.","/unit/source_map");
        for(const auto &[wid,item]:widgets) {
            const auto *props=item.value->member("props");
            if(props && props->member("default_value") && !mappedDefaults.contains(wid))
                fail("IR_SOURCE_MAP_COVERAGE","An explicit numeric default lost its source contribution.",item.pointer);
        }
        std::map<std::string,const CanonicalIrCorrespondence *> byId;
        for(const auto &raw:array(unit.member("correspondence"),"/unit/correspondence",limits().maximumMappingRecords)) {
            const auto &record=object(&raw,"/unit/correspondence");fields(record,{"id","source_ref","relation_kind","target_refs","metadata"},"/unit/correspondence");
            validateMetadata(record.member("metadata"),"/unit/correspondence/metadata");
            CanonicalIrCorrespondence relation;relation.id=unique(record.member("id"),"/unit/correspondence/id");relation.sourceRef=text(record.member("source_ref"),relation.id);sourceRef(relation.sourceRef);
            relation.relationKind=text(record.member("relation_kind"),relation.id);relation.targetRefs=targets(record.member("target_refs"),relation.id,true);
            if(relation.relationKind!="declaration_reference" && relation.relationKind!="non_primary_outcome")fail("IR_CORRESPONDENCE_UNSUPPORTED","Correspondence family is outside the admitted corridor.",relation.id,true);
            if(relation.relationKind=="non_primary_outcome" && !relation.targetRefs.empty())fail("IR_CORRESPONDENCE","Support-only source cannot acquire primary execution authority.",relation.id);
            for(const auto &target:relation.targetRefs)if(!nodeIndex.contains(target))fail("IR_CORRESPONDENCE","Correspondence target is missing.",relation.id);
            if(relation.relationKind=="declaration_reference") {
                if(relation.targetRefs.empty()) {
                    const auto unusedWidget=std::find_if(widgets.begin(),widgets.end(),[&](const auto &entry){
                        const auto classField=entry.second.value->member("widget")?"widget":"class_ref";
                        return relation.sourceRef==ref(entry.second.pointer) || relation.sourceRef==ref(entry.second.pointer+"/"+classField);
                    });
                    if(unusedWidget==widgets.end() || widgetParticipants.contains(unusedWidget->first))
                        fail("IR_CORRESPONDENCE","An empty declaration reference must describe an actual unused widget declaration.",relation.id);
                    const auto &declMeta=object(record.member("metadata"),relation.id);
                    if(relation.sourceRef==ref(unusedWidget->second.pointer)) {
                        fields(declMeta,{"widget_id"},relation.id);
                        if(text(declMeta.member("widget_id"),relation.id)!=unusedWidget->first)
                            fail("IR_CORRESPONDENCE","Unused widget declaration metadata changed its identity.",relation.id);
                    } else {
                        fields(declMeta,{"class_id"},relation.id);
                        const auto classField=unusedWidget->second.value->member("widget")?"widget":"class_ref";
                        if(text(declMeta.member("class_id"),relation.id)!=text(unusedWidget->second.value->member(classField),relation.id))
                            fail("IR_CORRESPONDENCE","Unused widget declaration metadata changed its class.",relation.id);
                    }
                    relations.push_back(std::move(relation));continue;
                }
                if(relation.targetRefs.size()!=1)fail("IR_CORRESPONDENCE","This corridor has one actual widget participant per declaration.",relation.id);
                const auto &target=nodes[nodeIndex.at(relation.targetRefs.front())];
                if(target.family!="widget_value")fail("IR_CORRESPONDENCE","A declaration reference cannot replace a primary primitive.",relation.id);
                const auto &widget=widgets.at(target.widgetId);
                const auto &declMeta=object(record.member("metadata"),relation.id);
                if(relation.sourceRef==ref(widget.pointer)) {
                    fields(declMeta,{"widget_id"},relation.id);
                    if(text(declMeta.member("widget_id"),relation.id)!=target.widgetId)
                        fail("IR_CORRESPONDENCE","Widget declaration metadata changed its actual identity.",relation.id);
                } else {
                    const auto classField=widget.value->member("widget")?"widget":"class_ref";
                    if(relation.sourceRef!=ref(widget.pointer+"/"+classField))
                        fail("IR_CORRESPONDENCE","A class declaration reference no longer identifies its actual widget class.",relation.id);
                    fields(declMeta,{"class_id"},relation.id);
                    if(text(declMeta.member("class_id"),relation.id)!=target.widgetClass)
                        fail("IR_CORRESPONDENCE","Class declaration metadata changed its actual class.",relation.id);
                }
            }
            relations.push_back(std::move(relation));
        }
        for(const auto &relation:relations)byId.emplace(relation.id,&relation);
        for(const auto &node:nodes) {
            const auto &references=objectCorrespondence.at(node.id);
            if(node.family=="primitive" && !references.empty())fail("IR_CORRESPONDENCE","A primitive cannot replace widget declaration participation.",node.id);
            if(node.family=="widget_value") {
                if(references.size()!=1 || !byId.contains(references.front()))fail("IR_CORRESPONDENCE","A widget participant lost its declared object correspondence.",node.id);
                const auto &relation=*byId.at(references.front());
                if(relation.relationKind!="declaration_reference" || relation.sourceRef!=ref(widgets.at(node.widgetId).pointer) || relation.targetRefs!=std::vector<std::string>{node.id})
                    fail("IR_CORRESPONDENCE","Widget declaration and execution participation no longer agree.",node.id);
            }
        }
    }
    void run() {
        validateSource();object(&tree,"/");fields(tree,{"schema_family","schema_version","document_id","metadata","unit"},"/");
        if(text(tree.member("schema_family"),"/")!="frog.execution_ir" || text(tree.member("schema_version"),"/")!="0.1")
            fail("IR_FAMILY_UNSUPPORTED","Input is not the canonical open Execution IR family/version.","/",true);
        documentId=unique(tree.member("document_id"),"/document_id");
        const auto &metadata=object(tree.member("metadata"),"/metadata");fields(metadata,{"producer","producer_version","generated_at","notes"},"/metadata");
        for(const auto field:{"producer","producer_version","generated_at"})
            if(metadata.member(field))text(metadata.member(field),"/metadata");
        hash="sha256:"+hashing::sha256Hex(source.originalUtf8());
        std::size_t hashNotes=0,uriNotes=0;
        for(const auto &note:array(metadata.member("notes"),"/metadata/notes",16)) {
            const auto value=text(&note,"/metadata/notes");
            if(value.starts_with("Exact source bytes: ")) {++hashNotes;if(value!="Exact source bytes: "+hash)fail("IR_SOURCE_HASH_MISMATCH","IR belongs to different authoritative source bytes.","/metadata/notes");}
            if(value.starts_with("Source URI: ")) {++uriNotes;if(value!="Source URI: "+options.sourceUri)fail("IR_SOURCE_URI_MISMATCH","IR belongs to a different source origin.","/metadata/notes");}
        }
        if(hashNotes!=1 || uriNotes!=1)fail("IR_SOURCE_IDENTITY","This admitted source/IR corridor needs one exact-byte hash and source URI.","/metadata/notes");
        const auto &unit=object(tree.member("unit"),"/unit");fields(unit,{"id","metadata","objects","connections","regions","source_map","correspondence"},"/unit");
        validateMetadata(unit.member("metadata"),"/unit/metadata");
        if(const auto *unitMeta=unit.member("metadata")) {
            fields(*unitMeta,{"program_name","profile_set","notes"},"/unit/metadata");
            if(unitMeta->member("program_name"))text(unitMeta->member("program_name"),"/unit/metadata");
            for(const auto &profile:array(unitMeta->member("profile_set"),"/unit/metadata/profile_set",limits().maximumNodes,true))text(&profile,"/unit/metadata/profile_set");
            stringArray(unitMeta->member("notes"),"/unit/metadata/notes",limits().maximumMappingRecords);
        }
        unitId=unique(unit.member("id"),"/unit/id");
        if(!array(unit.member("regions"),"/unit/regions",limits().maximumNodes).empty())fail("IR_REGIONS_UNSUPPORTED","Structured regions require their own native corridor.","/unit/regions",true);
        readNodes(unit);readEdges(unit);readMappings(unit);
    }
};

std::string quoteCanonicalJson(std::string_view value) {
    constexpr char digits[]="0123456789abcdef";std::string result="\"";
    for(const unsigned char byte:value) {
        if(byte=='"' || byte=='\\'){result+='\\';result+=static_cast<char>(byte);}
        else if(byte<32){result+="\\u00";result+=digits[byte>>4];result+=digits[byte&15];}
        else result+=static_cast<char>(byte);
    }
    result+='"';return result;
}
std::string encodedId(std::string_view prefix,std::string_view sourceId) {
    constexpr char digits[]="0123456789abcdef";std::string result(prefix);result+=':';
    for(const unsigned char byte:sourceId){result+=digits[byte>>4];result+=digits[byte&15];}
    return result;
}
struct CanonicalDraft {
    Intake &admission;
    std::vector<std::string> objects,connections,maps,relations;
    std::size_t collectedBytes=0;
    void append(std::vector<std::string> &collection,std::string record) {
        const auto maximum=admission.limits().maximumIrBytes;
        if(record.size()>maximum || collectedBytes>maximum-record.size())
            fail("IR_RESOURCE_LIMIT","Embedded canonical construction exceeds its bounded artifact budget.","/",true);
        collectedBytes+=record.size();collection.push_back(std::move(record));
    }
    static std::string list(const std::vector<std::string> &records) {
        std::string result="[";bool first=true;
        for(const auto &record:records){if(!first)result+=',';first=false;result+=record;}
        result+=']';return result;
    }
    static std::string target(std::string_view value){return value.empty()?"[]":"["+quoteCanonicalJson(value)+"]";}
    void mapping(std::string name,std::string_view kind,std::string_view pointer,std::string_view targetId,
        std::string_view targetKind,std::string_view relation="direct_primary") {
        append(maps,"{\"id\":"+quoteCanonicalJson(name)+",\"source_kind\":"+quoteCanonicalJson(kind)+",\"source_ref\":"+quoteCanonicalJson(admission.ref(pointer))+
            ",\"target_kind\":"+quoteCanonicalJson(targetKind)+",\"target_refs\":"+target(targetId)+",\"relation\":"+quoteCanonicalJson(relation)+"}");
    }
    void correspondence(std::string name,std::string_view pointer,std::string_view targetId={},
        std::string_view kind="non_primary_outcome",std::string_view metadataJson={}) {
        std::string record="{\"id\":"+quoteCanonicalJson(name)+",\"source_ref\":"+quoteCanonicalJson(admission.ref(pointer))+
            ",\"relation_kind\":"+quoteCanonicalJson(kind)+",\"target_refs\":"+target(targetId);
        if(!metadataJson.empty())record+=",\"metadata\":"+std::string(metadataJson);
        append(relations,record+"}");
    }
    std::string port(std::string_view node,std::string_view name,bool input) {
        return "{\"id\":"+quoteCanonicalJson(name)+",\"direction\":"+quoteCanonicalJson(input?"in":"out")+
            ",\"role\":\"data\",\"type_ref\":\"f64\",\"boundary_kind\":\"none\",\"metadata\":{\"source_node_id\":"+
            quoteCanonicalJson(node)+",\"source_port_id\":"+quoteCanonicalJson(name)+"}}";
    }
    std::string construct(std::string_view documentId) {
        const auto &source=admission.source.root();const auto &diagram=object(source.member("diagram"),"/diagram");
        std::map<std::string,std::string> participants;
        for(const auto &raw:array(diagram.member("nodes"),"/diagram/nodes",admission.limits().maximumNodes)) {
            const auto sourceId=text(raw.member("id"),"/diagram/nodes");const auto &item=admission.sourceNodes.at(sourceId);
            const auto family=text(raw.member("kind"),item.pointer);const auto objectId=encodedId("node",sourceId);
            std::string role,attrs="{\"source_node_id\":"+quoteCanonicalJson(sourceId),ports,correspondenceRefs="[]";
            std::string kind=family=="primitive"?text(raw.member("type"),item.pointer):"widget_value";
            std::string attribution="["+quoteCanonicalJson(admission.ref(item.pointer));
            if(family=="widget_value") {
                const auto widgetId=text(raw.member("widget"),item.pointer);const auto found=admission.widgets.find(widgetId);
                if(found==admission.widgets.end())fail("IR_WIDGET_MISMATCH","Source participant has no actual declared widget.",item.pointer);
                const auto &decl=*found->second.value;role=text(decl.member("role"),found->second.pointer);
                const auto klass=text(decl.member("widget")?decl.member("widget"):decl.member("class_ref"),found->second.pointer);
                attrs+=",\"widget_id\":"+quoteCanonicalJson(widgetId)+",\"widget_class\":"+quoteCanonicalJson(klass)+",\"value_type\":\"f64\"";
                if(const auto *props=decl.member("props");props && props->member("default_value"))
                    attrs+=",\"default_value\":"+props->member("default_value")->string;
                ports="["+port(sourceId,"value",role=="indicator")+"]";
                attribution+=","+quoteCanonicalJson(admission.ref(found->second.pointer));
                correspondenceRefs=target(encodedId("declaration",widgetId));
                if(!participants.emplace(widgetId,objectId).second)
                    fail("IR_WIDGET_UNSUPPORTED","Repeated natural widget participation has no implementation in this corridor.",item.pointer,true);
            } else ports="["+port(sourceId,"a",true)+","+port(sourceId,"b",true)+","+port(sourceId,"result",false)+"]";
            std::string record="{\"id\":"+quoteCanonicalJson(objectId)+",\"family\":"+quoteCanonicalJson(family)+",\"kind\":"+quoteCanonicalJson(kind)+
                ",\"classification\":{\"primary_or_support\":\"primary\""+(family=="primitive"?",\"library_id\":\"frog.core\"":"")+
                "},\"ports\":"+ports+",\"attributes\":"+attrs+"},\"attribution\":{\"source_refs\":"+attribution+
                "],\"mode\":"+quoteCanonicalJson(family=="widget_value"?"multi_contributor":"direct")+"},\"correspondence_refs\":"+correspondenceRefs;
            if(!role.empty())record+=",\"role\":"+quoteCanonicalJson(role);append(objects,record+"}");
            mapping(encodedId("map-node",sourceId),"diagram_node",item.pointer,objectId,"object");
            for(const auto field:{"layout","doc","tags"})if(raw.member(field))correspondence(encodedId("node-"+std::string(field),sourceId),item.pointer+"/"+field);
        }
        for(const auto &raw:array(diagram.member("edges"),"/diagram/edges",admission.limits().maximumEdges)) {
            const auto sourceId=text(raw.member("id"),"/diagram/edges");const auto &item=admission.sourceEdges.at(sourceId);
            const auto edgeId=encodedId("edge",sourceId);
            const auto endpoint=[&](std::string_view side) {
                const auto &end=object(raw.member(side),item.pointer);
                return "{\"object_id\":"+quoteCanonicalJson(encodedId("node",text(end.member("node"),item.pointer)))+
                    ",\"port_id\":"+quoteCanonicalJson(text(end.member("port"),item.pointer))+"}";
            };
            append(connections,"{\"id\":"+quoteCanonicalJson(edgeId)+",\"kind\":\"dataflow\",\"from\":"+endpoint("from")+",\"to\":"+endpoint("to")+
                ",\"attribution\":{\"source_refs\":["+quoteCanonicalJson(admission.ref(item.pointer))+"],\"mode\":\"direct\"},\"metadata\":{\"source_edge_id\":"+
                quoteCanonicalJson(sourceId)+",\"type_ref\":\"f64\"}}");
            mapping(encodedId("map-edge",sourceId),"diagram_edge",item.pointer,edgeId,"connection");
            if(raw.member("bends"))correspondence(encodedId("edge-bends",sourceId),item.pointer+"/bends");
        }
        const auto &panel=object(source.member("front_panel"),"/front_panel");
        for(const auto &decl:array(panel.member("widgets"),"/front_panel/widgets",admission.limits().maximumWidgets)) {
            const auto wid=text(decl.member("id"),"/front_panel/widgets");const auto &item=admission.widgets.at(wid);
            const auto found=participants.find(wid);const auto targetId=found==participants.end()?std::string{}:found->second;
            mapping(encodedId("map-widget",wid),"widget_instance",item.pointer,targetId,targetId.empty()?"none":"object","non_primary");
            if(!targetId.empty()) {
                correspondence(encodedId("declaration",wid),item.pointer,targetId,"declaration_reference","{\"widget_id\":"+quoteCanonicalJson(wid)+"}");
                const auto classField=decl.member("widget")?"widget":"class_ref";
                correspondence(encodedId("widget-class",wid),item.pointer+"/"+classField,targetId,"declaration_reference",
                    "{\"class_id\":"+quoteCanonicalJson(text(decl.member(classField),item.pointer))+"}");
            } else {
                correspondence(encodedId("declaration",wid),item.pointer,{},"declaration_reference","{\"widget_id\":"+quoteCanonicalJson(wid)+"}");
                const auto classField=decl.member("widget")?"widget":"class_ref";
                correspondence(encodedId("widget-class",wid),item.pointer+"/"+classField,{},"declaration_reference",
                    "{\"class_id\":"+quoteCanonicalJson(text(decl.member(classField),item.pointer))+"}");
            }
            if(const auto *props=decl.member("props");props && props->member("default_value"))
                mapping(encodedId("map-default",wid),"widget_member",item.pointer+"/props/default_value",targetId,
                    targetId.empty()?"none":"object",targetId.empty()?"non_primary":"direct_primary");
            for(const auto field:{"layout","parts","doc","tags"})if(decl.member(field))correspondence(encodedId("widget-"+std::string(field),wid),item.pointer+"/"+field);
        }
        for(const auto section:{"connector","icon","ide","cache"})if(source.member(section))correspondence("section:"+std::string(section),"/"+std::string(section));
        if(panel.member("canvas"))correspondence("panel:canvas","/front_panel/canvas");
        if(diagram.member("annotations"))correspondence("diagram:annotations","/diagram/annotations");
        const auto result="{\"schema_family\":\"frog.execution_ir\",\"schema_version\":\"0.1\",\"document_id\":"+quoteCanonicalJson(documentId)+
            ",\"metadata\":{\"producer\":\"frog.native.canonical-execution-ir\",\"producer_version\":\"1\",\"notes\":["+
            quoteCanonicalJson("Accepted subset: frog.canonical.widget-f64-add.v1")+","+
            quoteCanonicalJson("Exact source bytes: sha256:"+hashing::sha256Hex(admission.source.originalUtf8()))+","+quoteCanonicalJson("Source URI: "+admission.options.sourceUri)+
            "]},\"unit\":{\"id\":\"unit:main\",\"objects\":"+list(objects)+",\"connections\":"+list(connections)+
            ",\"regions\":[],\"source_map\":"+list(maps)+",\"correspondence\":"+list(relations)+"}}";
        if(result.size()>admission.limits().maximumIrBytes)fail("IR_RESOURCE_LIMIT","Embedded artifact exceeds its bounded byte admission.","/",true);
        return result;
    }
};
}

std::optional<CanonicalExecutionIr> CanonicalExecutionIr::read(std::string_view bytes,const FrogSourceDocument &sourceDoc,
    const CanonicalIrReadOptions &options,std::vector<CanonicalIrDiagnostic> *diagnostics) {
    const CanonicalFrontendPhaseScope sealing(options.metrics,CanonicalFrontendPhase::Sealing);
    if(diagnostics)diagnostics->clear();
    try {
        const auto &limit=options.limits;const CanonicalIrLimits ceilings;
        const std::array requested{limit.maximumIrBytes,limit.maximumSourceBytes,limit.maximumNodes,limit.maximumEdges,limit.maximumWidgets,limit.maximumSourceIdBytes,limit.maximumIrIdBytes,limit.maximumMappingRecords};
        const std::array maximum{ceilings.maximumIrBytes,ceilings.maximumSourceBytes,ceilings.maximumNodes,ceilings.maximumEdges,ceilings.maximumWidgets,ceilings.maximumSourceIdBytes,ceilings.maximumIrIdBytes,ceilings.maximumMappingRecords};
        for(std::size_t i=0;i<requested.size();++i)if(!requested[i] || requested[i]>maximum[i])fail("IR_RESOURCE_POLICY","Admission budgets may only lower the declared native intake ceilings.","/",true);
        if(bytes.size()>limit.maximumIrBytes || sourceDoc.originalUtf8().size()>limit.maximumSourceBytes)fail("IR_RESOURCE_LIMIT","Input bytes exceed native intake budgets.","/",true);
        if(options.sourceUri.empty() || options.sourceUri.find('#')!=options.sourceUri.npos || options.sourceUri.size()>8192)fail("IR_SOURCE_URI","An explicit, bounded source-origin URI without a fragment is required.","/");
        std::string error;auto tree=source::parseJson(bytes,&error,source::JsonNumberPolicy::PreserveLexeme);
        if(!tree)fail(error.find("supported maximum")!=error.npos?"IR_RESOURCE_LIMIT":"IR_JSON",error,"/",error.find("supported maximum")!=error.npos);
        if(structured_ir::required(sourceDoc)) {
            const auto documentId=id(tree->member("document_id"),"/document_id",limit.maximumSourceIdBytes);
            if(documentId=="unit:main")fail("IR_ID","IR document identity must differ from the root unit.","/document_id");
            auto draft=structured_ir::build(sourceDoc,documentId,options,diagnostics);
            if(!draft || !structured_ir::matches(bytes,*draft,diagnostics))return {};
            auto storage=std::make_shared<Storage>(sourceDoc);storage->original=bytes;storage->tree=std::move(*tree);
            storage->documentId=std::move(draft->documentId);storage->unitId=std::move(draft->unitId);storage->sourceUri=options.sourceUri;storage->sourceHash=std::move(draft->hash);
            storage->nodes=std::move(draft->nodes);storage->edges=std::move(draft->connections);storage->maps=std::move(draft->maps);storage->correspondence=std::move(draft->correspondence);
            storage->regions=std::move(draft->regions);storage->structures=std::move(draft->structures);
            return CanonicalExecutionIr(std::move(storage));
        }
        Intake intake{*tree,sourceDoc,options};intake.run();
        auto storage=std::make_shared<Storage>(sourceDoc);storage->original=bytes;storage->tree=std::move(*tree);
        storage->documentId=std::move(intake.documentId);storage->unitId=std::move(intake.unitId);storage->sourceUri=options.sourceUri;storage->sourceHash=std::move(intake.hash);
        storage->nodes=std::move(intake.nodes);storage->edges=std::move(intake.connections);storage->maps=std::move(intake.maps);storage->correspondence=std::move(intake.relations);
        return CanonicalExecutionIr(std::move(storage));
    } catch(const IntakeError &error) {if(diagnostics)diagnostics->push_back(error.diagnostic);return {};}
}
std::optional<CanonicalExecutionIr> CanonicalExecutionIr::derive(const FrogSourceDocument &sourceDoc,std::string_view documentId,
    const CanonicalIrReadOptions &options,std::vector<CanonicalIrDiagnostic> *diagnostics) {
    if(diagnostics)diagnostics->clear();
    try {
        const auto &limit=options.limits;const CanonicalIrLimits ceilings;
        const std::array requested{limit.maximumIrBytes,limit.maximumSourceBytes,limit.maximumNodes,limit.maximumEdges,limit.maximumWidgets,limit.maximumSourceIdBytes,limit.maximumIrIdBytes,limit.maximumMappingRecords};
        const std::array maximum{ceilings.maximumIrBytes,ceilings.maximumSourceBytes,ceilings.maximumNodes,ceilings.maximumEdges,ceilings.maximumWidgets,ceilings.maximumSourceIdBytes,ceilings.maximumIrIdBytes,ceilings.maximumMappingRecords};
        for(std::size_t i=0;i<requested.size();++i)if(!requested[i] || requested[i]>maximum[i])fail("IR_RESOURCE_POLICY","Admission budgets may only lower the declared native intake ceilings.","/",true);
        if(sourceDoc.originalUtf8().size()>limit.maximumSourceBytes)fail("IR_RESOURCE_LIMIT","Source exceeds embedded admission byte budget.","/",true);
        if(options.sourceUri.empty() || options.sourceUri.find('#')!=options.sourceUri.npos || options.sourceUri.size()>8192)fail("IR_SOURCE_URI","An explicit bounded source-origin URI is required.","/");
        if(documentId.size()>limit.maximumSourceIdBytes)fail("IR_RESOURCE_LIMIT","IR document identity exceeds construction byte budget.","/document_id",true);
        if(!canonicalId(documentId) || documentId=="unit:main")fail("IR_ID","A valid distinct caller-owned IR document identity is required.","/document_id");
        if(structured_ir::required(sourceDoc)) {
            auto draft=structured_ir::build(sourceDoc,documentId,options,diagnostics);
            if(!draft)return {};
            return read(draft->bytes,sourceDoc,options,diagnostics);
        }
        const Json unused;Intake admission{unused,sourceDoc,options};
        {const CanonicalFrontendPhaseScope semantic(options.metrics,CanonicalFrontendPhase::Semantics);admission.validateSource();}
        CanonicalDraft draft{admission};std::string bytes;
        {const CanonicalFrontendPhaseScope construction(options.metrics,CanonicalFrontendPhase::FirConstruction);bytes=draft.construct(documentId);}
        return read(bytes,sourceDoc,options,diagnostics);
    } catch(const IntakeError &error) {if(diagnostics)diagnostics->push_back(error.diagnostic);return {};}
}
std::string_view CanonicalExecutionIr::originalUtf8()const noexcept{return storage_->original;}
const source::JsonValue &CanonicalExecutionIr::root()const noexcept{return storage_->tree;}
const FrogSourceDocument &CanonicalExecutionIr::authoritativeSource()const noexcept{return storage_->source;}
std::string_view CanonicalExecutionIr::documentId()const noexcept{return storage_->documentId;}
std::string_view CanonicalExecutionIr::unitId()const noexcept{return storage_->unitId;}
std::string_view CanonicalExecutionIr::sourceUri()const noexcept{return storage_->sourceUri;}
std::string_view CanonicalExecutionIr::sourceHash()const noexcept{return storage_->sourceHash;}
std::span<const CanonicalIrNode> CanonicalExecutionIr::nodes()const noexcept{return storage_->nodes;}
std::span<const CanonicalIrConnection> CanonicalExecutionIr::connections()const noexcept{return storage_->edges;}
std::span<const CanonicalIrSourceMap> CanonicalExecutionIr::sourceMap()const noexcept{return storage_->maps;}
std::span<const CanonicalIrCorrespondence> CanonicalExecutionIr::correspondence()const noexcept{return storage_->correspondence;}
std::span<const CanonicalIrRegion> CanonicalExecutionIr::regions()const noexcept{return storage_->regions;}
std::span<const CanonicalIrStructure> CanonicalExecutionIr::structures()const noexcept{return storage_->structures;}
const CanonicalIrEventScope *CanonicalExecutionIr::eventScope()const noexcept{return storage_->eventScope?&*storage_->eventScope:nullptr;}
}
