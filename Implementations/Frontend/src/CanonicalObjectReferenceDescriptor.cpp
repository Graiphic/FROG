#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "frog_native_engine/graph/XYGraphReferenceContract.hpp"
#include "frog_native_engine/ArrayCapacity.hpp"
#include "frog_native_engine/SemaphoreReferenceLiteral.hpp"
#include "frog_native_engine/RendezvousReferenceLiteral.hpp"
#include "frog_native_engine/OccurrenceReferenceLiteral.hpp"
#include "frog_native_engine/QueueReferenceLiteral.hpp"
#include "frog_native_engine/NotifierReferenceLiteral.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "frog_native_engine/ApplicationControlSourceCodec.hpp"
#include "frog_native_engine/ApplicationControlSchema.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <set>
#include <sstream>
#include <stdexcept>

namespace frog::native_engine {
bool isNullQueueReferenceConfiguration(const source::JsonValue &configuration,const binding::TypeRef &type) {
    if(!isQueueReference(type)||!type.known()||type.children.size()!=1)return false;
    const auto value=application_control_source::readConfiguration(configuration);
    if(!value||!isNullQueueReferenceValue(*value)||!canonicalObjectReferenceFullTypeEqual(type.children.front(),value->valueType))return false;
    const auto descriptor=canonicalObjectReferenceDescriptor(configuration,{},type);
    return descriptor&&descriptor->kind==CanonicalObjectReferenceKind::Resolve&&
        descriptor->effects==std::vector<std::string>{"transport_typed_null"}&&
        canonical_json::exact(configuration,application_control_source::writeConfiguration(*value));
}
namespace {
using namespace canonical_json;
constexpr std::size_t maximumBytes=16'000'000,maximumEntries=4096,maximumDepth=32;
bool fields(const Json &v,std::initializer_list<std::string_view> names) {
    if(v.type!=Json::Type::Object)return false;
    return std::all_of(v.object.begin(),v.object.end(),[&](const auto &entry){
        return std::find(names.begin(),names.end(),entry.first)!=names.end();});
}
Json configuration(const ApplicationControlNode &cfg) {
    return application_control_source::writeConfiguration(cfg);
}
std::optional<ApplicationControlNode> decode(const Json &value,std::string &why) {
    if(!application_control_source::preflight(value,&why)||!fields(value,{"profile","kind","expected_class","value_schema","target","properties","method_id","parameters"})) {
        why="The full object-reference configuration exceeds its closed fields or bounded schema.";return {};
    }
    auto cfg=application_control_source::readConfiguration(value,&why);
    if(!cfg)return {};
    if(!exact(value,configuration(*cfg))){why="Unknown or lossy object-reference configuration metadata is not admitted.";return {};}
    return cfg;
}
bool valueType(const binding::TypeRef &root,bool reference,bool complex=false) {
    std::vector<const binding::TypeRef *> work{&root};std::size_t count=0;
    while(!work.empty()) {
        const auto &t=*work.back();work.pop_back();if(++count>maximumEntries||!t.known())return false;
        if(t.kind==binding::TypeKind::Reference){if(!reference||t.children.size()>1)return false;continue;}
        if(t.kind==binding::TypeKind::Cluster){if(t.children.empty()||t.children.size()!=t.fieldIds.size())return false;for(const auto &c:t.children)work.push_back(&c);continue;}
        if(t.kind==binding::TypeKind::Array){if(t.rank<1||t.rank>16||t.children.size()!=1)return false;work.push_back(&t.children.front());continue;}
        if(t.kind==binding::TypeKind::Map||t.kind==binding::TypeKind::Set){if(t.children.size()!=(t.kind==binding::TypeKind::Map?2u:1u))return false;for(const auto &c:t.children)work.push_back(&c);continue;}
        if(t.kind==binding::TypeKind::Enum){if(t.children.size()!=1||!t.children.front().integer()||t.choices.empty())return false;work.push_back(&t.children.front());continue;}
        if(t.kind!=binding::TypeKind::Primitive||!(t.name=="bool"||t.name=="i4"||t.name=="u4"||t.name=="i8"||t.name=="u8"||t.name=="i16"||t.name=="u16"||t.name=="i32"||t.name=="u32"||t.name=="i64"||t.name=="u64"||t.name=="f32"||t.name=="f64"||(complex&&(t.name=="c32"||t.name=="c64"))||t.name=="string"||t.name=="path"||t.name=="timestamp"||t.name=="variant"))return false;
    }
    return true;
}
}
std::string_view canonicalObjectReferenceOperation(CanonicalObjectReferenceKind kind)noexcept {
    switch(kind) {
    case CanonicalObjectReferenceKind::Resolve:return "frog.studio.object_reference.resolve.v1";
    case CanonicalObjectReferenceKind::Property:return "frog.studio.object_reference.property.v1";
    case CanonicalObjectReferenceKind::Invoke:return "frog.studio.object_reference.invoke.v1";
    case CanonicalObjectReferenceKind::Construct:return "frog.studio.record.construct.v1";
    case CanonicalObjectReferenceKind::Update:return "frog.studio.record.update.v1";
    case CanonicalObjectReferenceKind::Select:return "frog.studio.record.select.v1";
    case CanonicalObjectReferenceKind::Close:return "frog.studio.object_reference.close_borrowed.v1";
    }
    return {};
}
std::optional<CanonicalObjectReferenceDescriptor> canonicalObjectReferenceDescriptor(const Json &value,
    std::string_view pointer,const binding::TypeRef &wired,CanonicalObjectReferenceDiagnostic *diagnostic) {
    const auto fail=[&](std::string code,std::string why,bool unsupported=false)->std::optional<CanonicalObjectReferenceDescriptor>{
        if(diagnostic)*diagnostic={std::move(code),std::move(why),std::string(pointer),unsupported};return {};};
    try {
        std::string why;const auto cfg=decode(value,why);
        if(!cfg)return fail("canonical.object_reference.configuration_invalid",why);
        const auto type=wired.known()?wired:binding::TypeRef::reference(cfg->expectedClass,cfg->valueType.known()?std::optional{cfg->valueType}:std::nullopt);
        if(cfg->kind==ApplicationControlKind::Close&&
            (type.kind!=binding::TypeKind::Reference||
             (type.name!="frog.object"&&!binding::referenceClassAssignable(type.name,"frog.control"))))
            return fail("canonical.object_reference.close_target_unavailable","Close Reference in this local profile accepts only a borrowed widget reference, never an owned resource.",true);
        const bool synchronizationNull=(isNullSemaphoreReferenceValue(*cfg)&&isSemaphoreReference(type))||
            (isNullRendezvousReferenceValue(*cfg)&&isRendezvousReference(type))||
            (isNullOccurrenceReferenceValue(*cfg)&&isOccurrenceReference(type))||
            ((isNullQueueReferenceValue(*cfg)&&isQueueReference(type)||
              isNullNotifierReferenceValue(*cfg)&&isNotifierReference(type))&&type.children.size()==1&&
             canonicalObjectReferenceFullTypeEqual(type.children.front(),cfg->valueType));
        const bool genericControlView=type.kind==binding::TypeKind::Reference&&type.children.empty()&&
            (type.name=="frog.control"||type.name=="frog.object")&&!cfg->valueType.known();
        if(!synchronizationNull&&!genericControlView&&(type.kind!=binding::TypeKind::Reference||!type.known()||type.children.size()!=1||!valueType(type.children.front(),false)))
            return fail("canonical.object_reference.type_unavailable","The local profile requires a complete exact scalar/String/Record value schema and a typed borrowed reference.",true);
        if(!binding::referenceClassAssignable(type.name,cfg->expectedClass))return fail("canonical.object_reference.class_invalid","The actual reference view is not assignable to the configured expected class.");
        if(!synchronizationNull&&!genericControlView&&!canonicalObjectReferenceFullTypeEqual(type.children.front(),cfg->valueType))return fail("canonical.object_reference.schema_invalid","The effective target and saved complete value schema differ.");
        // An explicitly generic Control/Object reference keeps its nominal
        // view. Value crosses that view as a typed Variant; the actual target
        // schema and capabilities are checked by the instance registry.
        const auto valueSchema=type.children.empty()?binding::TypeRef::primitive("variant"):type.children.front();
        if(cfg->target.kind!=ApplicationControlTargetKind::Widget&&cfg->target.kind!=ApplicationControlTargetKind::Unbound)
            return fail("canonical.object_reference.target_unavailable","Value and Reinitialize require an actual declared widget target or a typed input reference.",true);
        if(cfg->target.kind==ApplicationControlTargetKind::Widget&&cfg->target.widgetId.empty())
            return fail("canonical.object_reference.target_invalid","The typed reference has no actual target identity.");
        CanonicalObjectReferenceDescriptor result;result.profile=canonicalObjectReferenceProfile;
        const bool xyTarget=type.name==xy::classId;
        result.configurationSourcePointer=pointer;result.configurationJson=encode(value);
        result.configurationSha256=hashing::sha256Hex(result.configurationJson);
        if(cfg->kind==ApplicationControlKind::Reference){result.kind=CanonicalObjectReferenceKind::Resolve;
            result.effects=cfg->target.kind==ApplicationControlTargetKind::Unbound?std::vector<std::string>{"transport_typed_null"}:std::vector<std::string>{"borrow_reference"};}
        else if(cfg->kind==ApplicationControlKind::Property) {
            result.kind=CanonicalObjectReferenceKind::Property;
            if(cfg->properties.empty()||cfg->properties.size()>32)return fail("canonical.object_reference.configuration_invalid","Property needs a bounded nonempty list of stable rows.");
            for(const auto &row:cfg->properties) {
                const bool label=row.memberId=="label.text"&&row.mode==ApplicationControlPropertyMode::Read;
                const bool signaling=row.memberId=="value_signaling";
                if(signaling) {
                    const auto member=applicationControlMember(type.name,valueSchema,row.memberId);
                    if(row.mode!=ApplicationControlPropertyMode::Write)
                        return fail("canonical.object_reference.write_only","Value (Signaling) is write only.");
                    if(!member || !member->runtimeAvailable || !member->writable)
                        return fail("canonical.object_reference.member_unavailable","This reference class has no Value (Signaling) writer.",true);
                }
                const auto graph=xyTarget?xy_reference::member(row.memberId):std::nullopt;
                const bool graphProperty=xy_reference::supported(row.memberId) && graph && graph->kind==ApplicationControlMemberKind::Property && graph->runtimeAvailable &&
                    (row.mode==ApplicationControlPropertyMode::Read?graph->readable:graph->writable);
                if(row.memberId!="value"&&!signaling&&!label&&!graphProperty)return fail("canonical.object_reference.member_unavailable","The declared class has no native executor for this property.",true);
                const std::string effect=label?"read_declared_label":signaling?"write_value_signaling":graphProperty?"xy_presentation":row.mode==ApplicationControlPropertyMode::Read?"read_value":"write_value_silent";
                if(std::find(result.effects.begin(),result.effects.end(),effect)==result.effects.end())result.effects.push_back(effect);
                if(signaling&&std::find(result.effects.begin(),result.effects.end(),"publish_value_change")==result.effects.end())
                    result.effects.push_back("publish_value_change");
            }
        }else if(cfg->kind==ApplicationControlKind::Close) {
            if(!cfg->properties.empty()||!cfg->methodId.empty()||!cfg->parameters.empty())
                return fail("canonical.object_reference.configuration_invalid","Close Reference has no property rows, method or parameter bindings.");
            result.kind=CanonicalObjectReferenceKind::Close;result.effects={"close_borrowed_reference"};
        }else {
            const auto graph=xyTarget?xy_reference::member(cfg->methodId):std::nullopt;
            const bool graphMethod=graph && graph->kind==ApplicationControlMemberKind::Method && graph->runtimeAvailable;
            if(cfg->methodId!="reinitialize"&&!graphMethod)return fail("canonical.object_reference.method_unavailable","The declared class has no native executor for this method.",true);
            if(!graphMethod&&!cfg->parameters.empty())return fail("canonical.object_reference.configuration_invalid","The actual Reinitialize method has no parameter bindings.");
            if(graphMethod)for(const auto &binding:cfg->parameters)if(std::none_of(graph->parameters.begin(),graph->parameters.end(),[&](const auto &parameter){return parameter.id==binding.id;}))return fail("canonical.object_reference.configuration_invalid","A Graph XY method parameter is not declared by its exact signature.");
            result.kind=CanonicalObjectReferenceKind::Invoke;result.effects={graphMethod?"xy_presentation":"reinitialize"};
        }
        const auto port=[&](std::string id,const binding::TypeRef &t,bool input,bool required=false){
            result.ports.push_back({std::move(id),canonicalObjectReferencePhysicalType(t).canonical(),encode(canonicalObjectReferenceWriteType(t)),input,required,{}});};
        if(cfg->kind==ApplicationControlKind::Reference)port("reference_out",type,false);
        else {
            if(applicationControlHasReferenceInput(*cfg))
                port("reference_in",type,true,cfg->target.kind==ApplicationControlTargetKind::Unbound);
            if(cfg->kind!=ApplicationControlKind::Close)port("reference_out",type,false);
            const auto error=binding::TypeRef::cluster({"status","code","source"},{binding::TypeRef::primitive("bool"),binding::TypeRef::primitive("i32"),binding::TypeRef::primitive("string")});
            port("error_in",error,true);port("error_out",error,false);
            for(const auto &row:cfg->properties) {
                const auto graph=xyTarget?xy_reference::member(row.memberId):std::nullopt;
                port("property:"+row.id,graph?graph->type:row.memberId=="label.text"?binding::TypeRef::primitive("string"):valueSchema,row.mode==ApplicationControlPropertyMode::Write,row.mode==ApplicationControlPropertyMode::Write);
            }
            if(cfg->kind==ApplicationControlKind::Invoke && xyTarget && cfg->methodId!="reinitialize") {
                const auto method=xy_reference::member(cfg->methodId);
                for(const auto &parameter:applicationControlMethodPorts(*method,cfg->parameters)) {
                    port(parameter.id,parameter.type,parameter.direction==ApplicationControlPortDirection::Input,parameter.required);
                    result.ports.back().defaultValueJson=parameter.defaultValue;
                }
            }
        }
        auto attributes=canonicalObjectReferenceAttributes(result);
        // Paths/Source attribution do not define semantics. Configuration,
        // complete ordered schemas, requirements and effects do.
        attributes.object.erase("configuration_ref");
        result.semanticKey=encode(attributes);return result;
    }catch(const std::exception &error){return fail("canonical.object_reference.configuration_invalid",error.what());}
}
source::JsonValue canonicalObjectReferenceAttributes(const CanonicalObjectReferenceDescriptor &descriptor) {
    auto config=source::parseJson(descriptor.configurationJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
    if(!config)throw std::invalid_argument("The object-reference descriptor has no valid full configuration JSON.");
    auto value=object();value.object={{"profile",string(descriptor.profile)},{"operation",string(std::string(canonicalObjectReferenceOperation(descriptor.kind)))},
        {"configuration",std::move(*config)},{"configuration_sha256",string(descriptor.configurationSha256)},
        {"configuration_ref",string(descriptor.configurationSourcePointer)}};
    auto ports=array();for(const auto &port:descriptor.ports) {
        auto schema=source::parseJson(port.fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
        if(!schema)throw std::invalid_argument("The object-reference port lost its full schema JSON.");
        auto p=object();p.object={{"id",string(port.id)},{"direction",string(port.input?"input":"output")},
            {"type",string(port.type)},{"schema",std::move(*schema)},{"required",boolean(port.required)}};
        if(port.defaultValueJson)p.object.emplace("default",string(*port.defaultValueJson));ports.array.push_back(std::move(p));
    }
    value.object.emplace("ports",std::move(ports));auto effects=array();
    for(const auto &effect:descriptor.effects)effects.array.push_back(string(effect));value.object.emplace("effects",std::move(effects));
    auto paths=array();for(const auto &path:descriptor.fieldPaths){auto p=array();for(const auto &id:path)p.array.push_back(string(id));paths.array.push_back(std::move(p));}
    value.object.emplace("field_paths",std::move(paths));return value;
}
namespace {
const binding::TypeRef *fieldType(const binding::TypeRef &root,const std::vector<std::string> &path) {
    const auto *type=&root;if(path.empty()||path.size()>32)return nullptr;
    for(const auto &id:path) {
        if(type->kind!=binding::TypeKind::Cluster)return nullptr;
        const auto field=std::find(type->fieldIds.begin(),type->fieldIds.end(),id);
        if(field==type->fieldIds.end())return nullptr;type=&type->children[static_cast<std::size_t>(field-type->fieldIds.begin())];
    }
    return type;
}
bool replacementType(const binding::TypeRef &actual,const binding::TypeRef &expected) {
    if(canonicalObjectReferenceFullTypeEqual(actual,expected))return true;
    // Class ancestry is checked again by the concrete closed provider before
    // native admission and by its actual authority on every use. No global
    // SDK Exact edge or non-reference value conversion is changed here.
    return actual.kind==binding::TypeKind::Reference&&expected.kind==binding::TypeKind::Reference&&
        actual.children.size()==expected.children.size()&&actual.children.size()==1&&
        canonicalObjectReferenceFullTypeEqual(actual.children.front(),expected.children.front());
}
bool literalValid(const binding::TypeRef &type,const Json &literal,std::size_t &nodes,unsigned depth=0) {
    if(depth>32||++nodes>4096)return false;
    if(type.kind==binding::TypeKind::Reference) {
        const auto *config=literal.member(canonicalObjectReferenceAttribute);
        if(literal.type!=Json::Type::Object||literal.object.size()!=1||!config)return false;
        const auto descriptor=canonicalObjectReferenceDescriptor(*config,{});
        if(!descriptor||descriptor->kind!=CanonicalObjectReferenceKind::Resolve||descriptor->ports.size()!=1)return false;
        const auto full=source::parseJson(descriptor->ports.front().fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
        const auto actual=full?canonicalObjectReferenceReadType(*full):std::nullopt;
        return actual&&canonicalObjectReferenceFullTypeEqual(*actual,type);
    }
    if(type.kind==binding::TypeKind::Cluster) {
        if(literal.type!=Json::Type::Object||literal.object.size()!=type.fieldIds.size()||type.children.size()!=type.fieldIds.size())return false;
        for(std::size_t i=0;i<type.children.size();++i){const auto *value=literal.member(type.fieldIds[i]);if(!value||!literalValid(type.children[i],*value,nodes,depth+1))return false;}return true;
    }
    if(type.kind==binding::TypeKind::Map||type.kind==binding::TypeKind::Set) {
        const bool map=type.kind==binding::TypeKind::Map;
        if(type.children.size()!=(map?2u:1u)||literal.type!=Json::Type::Object||literal.object.size()!=1)return false;
        const auto *items=literal.member(map?"entries":"elements");
        if(!items||items->type!=Json::Type::Array||items->array.size()>4096)return false;
        for(const auto &item:items->array) {
            if(map) {
                const auto *key=item.member("key"),*value=item.member("value");
                if(item.type!=Json::Type::Object||item.object.size()!=2||!key||!value||
                    !literalValid(type.children[0],*key,nodes,depth+1)||!literalValid(type.children[1],*value,nodes,depth+1))return false;
            }else if(!literalValid(type.children.front(),item,nodes,depth+1))return false;
        }return true;
    }
    if(type.kind==binding::TypeKind::Array) {
        if(type.children.size()!=1||literal.type!=Json::Type::Object||literal.object.size()!=2)return false;
        const auto *shape=literal.member("shape"),*items=literal.member("elements");
        if(!shape||shape->type!=Json::Type::Array||shape->array.size()!=type.rank||!items||items->type!=Json::Type::Array)return false;
        std::vector<std::uint32_t> extents;
        for(const auto &extent:shape->array) {
            std::uint32_t size=0;const auto parsed=std::from_chars(extent.string.data(),extent.string.data()+extent.string.size(),size);
            if(extent.type!=Json::Type::Number||parsed.ec!=std::errc{}||parsed.ptr!=extent.string.data()+extent.string.size())return false;
            extents.push_back(size);
        }
        const auto count=checkedArrayElementCount(extents);
        if(!count||*count!=items->array.size())return false;
        for(const auto &item:items->array){std::size_t elementNodes=0;if(!literalValid(type.children.front(),item,elementNodes,depth+1))return false;}
        return true;
    }
    if(type.kind==binding::TypeKind::Enum) {
        if(type.children.size()!=1||literal.type!=Json::Type::Number)return false;
        std::uint64_t value=0;const auto parsed=std::from_chars(literal.string.data(),literal.string.data()+literal.string.size(),value);
        if(parsed.ec!=std::errc{}||parsed.ptr!=literal.string.data()+literal.string.size()||
           std::none_of(type.choices.begin(),type.choices.end(),[&](const auto &choice){return choice.value==value;}))return false;
        return literalValid(type.children.front(),literal,nodes,depth+1);
    }
    if(type.kind!=binding::TypeKind::Primitive)return false;
    if(type.name=="variant") {
        if(literal.type!=Json::Type::Object)return false;
        if(const auto *empty=literal.member("empty")) {
            const auto *attributes=literal.member("attributes");
            if(empty->type!=Json::Type::Boolean||!empty->boolean||
               !fields(literal,{"empty","attributes"})||literal.object.size()!=(attributes?2u:1u))return false;
            if(attributes){if(attributes->type!=Json::Type::Object||attributes->object.size()>64)return false;
                for(const auto &[id,value]:attributes->object)if(id.empty()||!literalValid(type,value,nodes,depth+1))return false;}
            return true;
        }
        const auto *schema=literal.member("value_schema"),*payload=literal.member("value"),*attributes=literal.member("attributes");
        if(literal.type!=Json::Type::Object||literal.object.size()!=3||!schema||!payload||!attributes||attributes->type!=Json::Type::Object||attributes->object.size()>64)return false;
        const auto full=canonicalObjectReferenceReadType(*schema);
        if(!full||!literalValid(*full,*payload,nodes,depth+1))return false;
        for(const auto &[id,value]:attributes->object)if(id.empty()||!literalValid(type,value,nodes,depth+1))return false;
        return true;
    }
    if(type.name=="timestamp") {
        const auto *seconds=literal.member("seconds"),*fraction=literal.member("fraction");
        if(literal.type!=Json::Type::Object||literal.object.size()!=2||!seconds||!fraction)return false;
        const auto read=[](const Json &value,auto result) {
            if(value.type!=Json::Type::Number&&value.type!=Json::Type::String)return false;
            const auto parsed=std::from_chars(value.string.data(),value.string.data()+value.string.size(),result);
            return parsed.ec==std::errc{}&&parsed.ptr==value.string.data()+value.string.size();
        };
        return read(*seconds,std::int64_t{})&&read(*fraction,std::uint64_t{});
    }
    if(type.name=="c32"||type.name=="c64") {
        if(literal.type!=Json::Type::Object||literal.object.size()!=2)return false;
        const auto *real=literal.member("real"),*imaginary=literal.member("imaginary");
        const auto component=binding::TypeRef::primitive(type.name=="c32"?"f32":"f64");
        return real&&imaginary&&literalValid(component,*real,nodes,depth+1)&&literalValid(component,*imaginary,nodes,depth+1);
    }
    if(type.name=="f32") {
        if(literal.type!=Json::Type::Number)return false;float number=0;
        const auto parsed=std::from_chars(literal.string.data(),literal.string.data()+literal.string.size(),number);
        return parsed.ec==std::errc{}&&parsed.ptr==literal.string.data()+literal.string.size()&&std::isfinite(number);
    }
    if(type.name=="string"||type.name=="path")return literal.type==Json::Type::String&&literal.string.size()<=1'000'000;
    if(type.name=="i4"||type.name=="i8"||type.name=="i16") {
        if(literal.type!=Json::Type::Number)return false;std::int32_t value=0;const auto p=std::from_chars(literal.string.data(),literal.string.data()+literal.string.size(),value);
        return p.ec==std::errc{}&&p.ptr==literal.string.data()+literal.string.size()&&(type.name=="i4"?(value>=-8&&value<=7):type.name=="i8"?(value>=-128&&value<=127):(value>=-32768&&value<=32767));
    }
    if(type.name=="u4"||type.name=="u8"||type.name=="u16") {
        if(literal.type!=Json::Type::Number)return false;std::uint32_t value=0;const auto p=std::from_chars(literal.string.data(),literal.string.data()+literal.string.size(),value);
        return p.ec==std::errc{}&&p.ptr==literal.string.data()+literal.string.size()&&value<=(type.name=="u4"?15u:type.name=="u8"?255u:65535u);
    }
    return typedScalar(literal,type.name);
}
void seal(CanonicalObjectReferenceDescriptor &descriptor,const Json &configuration) {
    descriptor.configurationJson=encode(configuration);descriptor.configurationSha256=hashing::sha256Hex(descriptor.configurationJson);
    auto attrs=canonicalObjectReferenceAttributes(descriptor);attrs.object.erase("configuration_ref");descriptor.semanticKey=encode(attrs);
}
}
std::optional<CanonicalObjectReferenceDescriptor> canonicalObjectReferenceRecordDescriptor(
    CanonicalObjectReferenceKind kind,const binding::TypeRef &record,
    std::vector<CanonicalObjectReferencePort> ports,std::vector<std::vector<std::string>> paths,
    const Json *literal,std::string_view pointer,CanonicalObjectReferenceDiagnostic *diagnostic) {
    const auto fail=[&](std::string code,std::string why)->std::optional<CanonicalObjectReferenceDescriptor>{
        if(diagnostic)*diagnostic={std::move(code),std::move(why),std::string(pointer),false};return {};};
    try {
        if(kind!=CanonicalObjectReferenceKind::Resolve&&kind!=CanonicalObjectReferenceKind::Construct&&kind!=CanonicalObjectReferenceKind::Update&&kind!=CanonicalObjectReferenceKind::Select)
            return fail("canonical.object_reference.record_invalid","The closed Record instruction kind is invalid.");
        if(!valueType(record,true,kind==CanonicalObjectReferenceKind::Resolve)||(kind!=CanonicalObjectReferenceKind::Resolve&&record.kind!=binding::TypeKind::Cluster)||ports.empty()||ports.size()>128)
            return fail("canonical.object_reference.record_invalid","Record operations need their complete bounded exact schema and real ports.");
        CanonicalObjectReferenceDescriptor result;result.profile=canonicalObjectReferenceProfile;result.kind=kind;
        result.configurationSourcePointer=pointer;result.ports=std::move(ports);result.fieldPaths=std::move(paths);result.effects={"transport_reference_value"};
        auto config=object();config.object={{"kind",string(kind==CanonicalObjectReferenceKind::Resolve?"literal":kind==CanonicalObjectReferenceKind::Construct?"construct":kind==CanonicalObjectReferenceKind::Update?"update":"select")},
            {"schema",canonicalObjectReferenceWriteType(record)}};
        std::set<std::string> ids;std::vector<binding::TypeRef> types;
        for(const auto &port:result.ports) {
            auto json=source::parseJson(port.fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);auto type=json?canonicalObjectReferenceReadType(*json):std::nullopt;
            if(port.id.empty()||port.id.size()>1024||!ids.insert(port.id).second||!type||!valueType(*type,true,kind==CanonicalObjectReferenceKind::Resolve)||canonicalObjectReferencePhysicalType(*type).canonical()!=port.type||port.defaultValueJson)
                return fail("canonical.object_reference.port_invalid","A Record port lost its full exact schema, stable identity or inactive-default policy.");
            types.push_back(std::move(*type));
        }
        if(kind==CanonicalObjectReferenceKind::Resolve) {
            std::size_t nodes=0;
            if(!literal||result.ports.size()!=1||result.ports.front().input||result.ports.front().id!="value"||!result.fieldPaths.empty()||
                !canonicalObjectReferenceFullTypeEqual(types.front(),record)||!literalValid(record,*literal,nodes))
                return fail("canonical.object_reference.literal_invalid","The real literal has no complete exact typed value tree.");
            config.object["value"]=*literal;
        }else {
            std::vector<std::size_t> fields;std::size_t rails=0,results=0;
            for(std::size_t i=0;i<result.ports.size();++i) {
                const auto &port=result.ports[i];
                if(port.input&&port.id=="cluster") {
                    if(kind==CanonicalObjectReferenceKind::Construct||!port.required||!canonicalObjectReferenceFullTypeEqual(types[i],record))return fail("canonical.object_reference.rail_invalid","An actual Record rail requires its complete exact connected schema.");++rails;
                }else if(!port.input&&port.id=="result") {
                    if(kind==CanonicalObjectReferenceKind::Select||!canonicalObjectReferenceFullTypeEqual(types[i],record))return fail("canonical.object_reference.result_invalid","The complete resulting Record schema changed.");++results;
                }else {
                    if(port.input==(kind==CanonicalObjectReferenceKind::Select)||!port.id.starts_with("element")||port.id.size()<=7)
                        return fail("canonical.object_reference.port_invalid","A physical Record field needs its exact elementN direction and identity.");
                    unsigned ordinal=0;const auto parsed=std::from_chars(port.id.data()+7,port.id.data()+port.id.size(),ordinal);
                    if(parsed.ec!=std::errc{}||parsed.ptr!=port.id.data()+port.id.size()||!ordinal||ordinal>32||port.id!="element"+std::to_string(ordinal)||
                        (port.input&&!port.required))return fail("canonical.object_reference.port_invalid","A physical element port must be an actual bounded ordinal and a connected Required input.");fields.push_back(i);
                }
            }
            if(rails!=(kind==CanonicalObjectReferenceKind::Construct?0U:1U)||results!=(kind==CanonicalObjectReferenceKind::Select?0U:1U)||fields.size()!=result.fieldPaths.size())
                return fail("canonical.object_reference.record_invalid","The rail/result/field path tuple is incomplete.");
            std::set<std::vector<std::string>> replaced;
            for(std::size_t j=0;j<fields.size();++j) {
                const auto *selected=fieldType(record,result.fieldPaths[j]);
                if(!selected||(kind==CanonicalObjectReferenceKind::Select?!canonicalObjectReferenceFullTypeEqual(types[fields[j]],*selected):!replacementType(types[fields[j]],*selected)))
                    return fail("canonical.object_reference.field_invalid","A selected/replaced field lost its complete nested schema.");
                if(kind!=CanonicalObjectReferenceKind::Select&&!replaced.insert(result.fieldPaths[j]).second)
                    return fail("canonical.object_reference.field_invalid","Two replacement rows cannot ambiguously target the same field.");
            }
            if(kind==CanonicalObjectReferenceKind::Construct) {
                if(fields.size()!=record.fieldIds.size())return fail("canonical.object_reference.construct_invalid","Construction requires every heterogeneous field input.");
                for(std::size_t i=0;i<fields.size();++i)if(result.ports[fields[i]].id!="element"+std::to_string(i+1)||result.fieldPaths[i]!=std::vector<std::string>{record.fieldIds[i]})
                    return fail("canonical.object_reference.construct_invalid","Construction preserves contiguous elementN order and stable field identities.");
            }
        }
        seal(result,config);return result;
    }catch(const std::exception &error){return fail("canonical.object_reference.record_invalid",error.what());}
}
std::optional<CanonicalObjectReferenceDescriptor> canonicalObjectReferenceDescriptorFromAttributes(const Json &attrs,
    std::string_view pointer,CanonicalObjectReferenceDiagnostic *diagnostic) {
    const auto fail=[&](std::string why)->std::optional<CanonicalObjectReferenceDescriptor>{
        if(diagnostic)*diagnostic={"canonical.object_reference.attributes_invalid",std::move(why),std::string(pointer),false};return {};};
    try {
        if(!fields(attrs,{"profile","operation","configuration","configuration_sha256","configuration_ref","ports","effects","field_paths"}))return fail("Unknown local object attribute fields.");
        const auto *profile=attrs.member("profile"),*config=attrs.member("configuration"),*rawPorts=attrs.member("ports"),*rawPaths=attrs.member("field_paths");
        if(!profile||profile->type!=Json::Type::String||profile->string!=canonicalObjectReferenceProfile||!config||!rawPorts||rawPorts->type!=Json::Type::Array||!rawPaths||rawPaths->type!=Json::Type::Array)
            return fail("The complete explicit local profile/configuration/signature is required.");
        const auto *kind=config->member("kind");std::optional<CanonicalObjectReferenceDescriptor> descriptor;
        if(kind&&kind->type==Json::Type::String&&(kind->string=="literal"||kind->string=="construct"||kind->string=="update"||kind->string=="select")) {
            const auto *schema=config->member("schema");auto type=schema?canonicalObjectReferenceReadType(*schema,diagnostic):std::nullopt;if(!type)return {};
            if(!fields(*config,kind->string=="literal"?std::initializer_list<std::string_view>{"kind","schema","value"}:std::initializer_list<std::string_view>{"kind","schema"}))return fail("Unknown local Record configuration fields.");
            std::vector<CanonicalObjectReferencePort> ports;std::vector<std::vector<std::string>> paths;
            if(rawPorts->array.size()>128||rawPaths->array.size()>64)return fail("The local port/path metadata is unbounded.");
            for(const auto &p:rawPorts->array) {
                const auto *id=p.member("id"),*direction=p.member("direction"),*name=p.member("type"),*full=p.member("schema"),*required=p.member("required");
                if(!fields(p,{"id","direction","type","schema","required"})||!id||id->type!=Json::Type::String||!direction||direction->type!=Json::Type::String||
                    (direction->string!="input"&&direction->string!="output")||!name||name->type!=Json::Type::String||!full||!required||required->type!=Json::Type::Boolean)return fail("A local port descriptor is incomplete.");
                ports.push_back({id->string,name->string,encode(*full),direction->string=="input",required->boolean,{}});
            }
            for(const auto &p:rawPaths->array){if(p.type!=Json::Type::Array||p.array.size()>32)return fail("Invalid full field-ID path.");std::vector<std::string> path;for(const auto &id:p.array){if(id.type!=Json::Type::String||id.string.empty())return fail("A field path needs explicit stable IDs.");path.push_back(id.string);}paths.push_back(std::move(path));}
            const auto operation=kind->string=="literal"?CanonicalObjectReferenceKind::Resolve:kind->string=="construct"?CanonicalObjectReferenceKind::Construct:kind->string=="update"?CanonicalObjectReferenceKind::Update:CanonicalObjectReferenceKind::Select;
            descriptor=canonicalObjectReferenceRecordDescriptor(operation,*type,std::move(ports),std::move(paths),config->member("value"),pointer,diagnostic);
        }else {
            binding::TypeRef wired;
            // Port signatures carry the exact actual nominal view separately
            // from the immutable configured expected class. Revalidate it;
            // never obtain its semantics from a SHA or Source side channel.
            for(const auto &p:rawPorts->array){const auto *id=p.member("id"),*schema=p.member("schema");if(id&&id->type==Json::Type::String&&(id->string=="reference_in"||id->string=="reference_out")&&schema){const auto parsed=canonicalObjectReferenceReadType(*schema,diagnostic);if(!parsed)return {};wired=*parsed;break;}}
            descriptor=canonicalObjectReferenceDescriptor(*config,pointer,wired,diagnostic);
        }
        if(!descriptor)return {};
        if(!exact(attrs,canonicalObjectReferenceAttributes(*descriptor)))return fail("Full local configuration, SHA, signature, effects or field paths were tampered.");return descriptor;
    }catch(const std::exception &error){return fail(error.what());}
}
}
