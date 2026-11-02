#include "frog_native_engine/ContextPathIdentity.hpp"
#include "frog_native_engine/CanonicalDraftMigration.hpp"
#include "frog_native_engine/CanonicalRecordConstant.hpp"
#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "frog_native_engine/CanonicalScalarNative.hpp"
#include "frog_native_engine/FunctionCatalog.hpp"
#include "frog_native_engine/DiagramStructureCatalog.hpp"
#include "frog_native_engine/CanonicalScalarSelection.hpp"
#include "frog_native_engine/ComparisonBinding.hpp"
#include "frog_native_engine/CanonicalStringNative.hpp"
#include "frog_native_engine/CanonicalArrayNative.hpp"
#include "frog_native_engine/CanonicalNativeCallScalar.hpp"
#include "frog_native_engine/ExponentialFunctionIdentity.hpp"
#include "frog_native_engine/DocumentTextDefaults.hpp"
#include "frog_native_engine/NumericValueLexeme.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "frog_native_engine/HyperbolicFunctionIdentity.hpp"
#include "frog_native_engine/GatingFunctionIdentity.hpp"
#include "frog_native_engine/DiscreteMathFunctionIdentity.hpp"
#include "frog_native_engine/TrigonometricFunctionIdentity.hpp"
#include "frog_native_engine/BesselFunctionIdentity.hpp"
#include "frog_native_engine/GammaFunctionIdentity.hpp"
#include "frog_native_engine/HypergeometricFunctionIdentity.hpp"
#include "frog_native_engine/EllipticFunctionIdentity.hpp"
#include "frog_native_engine/ExponentialIntegralsFunctionIdentity.hpp"
#include "frog_native_engine/ErrorFunctionsFunctionIdentity.hpp"
#include "frog_native_engine/EllipticParabolicFunctionIdentity.hpp"
#include "frog_native_engine/ButtonMechanicalAction.hpp"
#include "CanonicalJsonSupport.hpp"
#include "CanonicalPrivateInterfaceMap.hpp"
#include "CanonicalDraftPresentation.hpp"
#include "CanonicalComposedDraftMigration.hpp"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <iterator>
#include <set>

namespace frog::native_engine {
namespace {
using namespace canonical_json;
using Domain=DocumentDiagnosticDomain;
DocumentDiagnosticDomain operationDomain(std::string_view type) {
    return findFunctionCatalogEntryByDiagramNodeType(type)||findFunctionCatalogEntryByPrimitiveType(type)?
        Domain::FrontendCapability:Domain::Compatibility;
}
DocumentDiagnosticDomain structureDomain(const Json &node) {
    const auto *type=node.member("type");
    return type&&type->type==Json::Type::String&&isDiagramStructureNodeType(type->string)?
        Domain::FrontendCapability:Domain::Compatibility;
}
struct Refusal {std::string code,path,message;Domain domain;};
[[noreturn]] void refuse(std::string path,std::string message,std::string code,Domain domain){
    throw Refusal{std::move(code),std::move(path),std::move(message),domain};
}
const Json &obj(const Json *v,const std::string &path){if(!v||v->type!=Json::Type::Object)refuse(path,"Expected an explicit object.", "migration.unsupported", Domain::Integrity);return *v;}
const std::vector<Json> &arr(const Json *v,const std::string &path){if(!v||v->type!=Json::Type::Array)refuse(path,"Expected an explicit array.", "migration.unsupported", Domain::Integrity);return v->array;}
std::string txt(const Json *v,const std::string &path){if(!v||v->type!=Json::Type::String||v->string.empty())refuse(path,"Expected a nonempty string.", "migration.unsupported", Domain::Integrity);return v->string;}
bool flag(const Json *v,const std::string &path){if(!v||v->type!=Json::Type::Boolean)refuse(path,"Expected an explicit Boolean.", "migration.unsupported", Domain::Integrity);return v->boolean;}
std::optional<std::string> caseInteger(const Json *value,std::string_view type) {
    if(!value||value->type!=Json::Type::String||value->string.empty())return {};
    const auto &text=value->string;const std::size_t first=text.front()=='-'?1U:0U;
    if(first==text.size()||!std::all_of(text.begin()+first,text.end(),[](char c){return c>='0'&&c<='9';}))return {};
    if(type=="i64"){std::int64_t n=0;const auto r=std::from_chars(text.data(),text.data()+text.size(),n);if(r.ec==std::errc{}&&r.ptr==text.data()+text.size())return std::to_string(n);}
    else if(type=="u64"){std::uint64_t n=0;const auto r=std::from_chars(text.data(),text.data()+text.size(),n);if(r.ec==std::errc{}&&r.ptr==text.data()+text.size())return std::to_string(n);}
    return {};
}
void empty(const Json *v,const std::string &path){if(v&&!arr(v,path).empty())refuse(path,"This migration corridor does not discard unsupported elements.", "migration.unsupported", Domain::Compatibility);}
void layout(const Json &v,const std::string &path){
    obj(&v,path);for(const auto name:{"x","y","width","height"}){
        const auto *n=v.member(name);if(!n||!typedScalar(*n,"f64"))refuse(path,"Layout must contain finite coordinates and dimensions.", "migration.unsupported", Domain::Semantics);
        if((std::string_view(name)=="width"||std::string_view(name)=="height")&&std::stod(n->string)<=0)refuse(path,"Layout dimensions must be positive.", "migration.unsupported", Domain::Semantics);}
}
std::optional<Json> widgetScalarState(const Json &raw,std::string_view type) {
    if(type=="i32"||type=="u32"||type=="i64"||type=="u64") {
        if(raw.type!=Json::Type::Number)return {};
        // The real writer uses general floating formatting for the exact32
        // display cache (e.g. -2.147483648e+09). Canonicalize the DECIMAL
        // lexeme before the existing width check; never round it via double.
        const auto lexical=numeric_value::canonicalWideIntegerLexeme(raw.string,type=="i32"?"i64":type=="u32"?"u64":type);
        auto normalized=lexical?source::parseJson(*lexical,nullptr,source::JsonNumberPolicy::PreserveLexeme):std::nullopt;
        if(normalized&&typedScalar(*normalized,type))return normalized;return {};
    }
    return typedScalar(raw,type)?std::optional<Json>{raw}:std::nullopt;
}
Json value(const Json &widget,std::string_view type,const std::string &path,bool gammaLimit=false,bool exactIntegerState=false){
    if(type=="string"||type=="path") {
        // Real String/Path indicators persist current text in props.text.value,
        // not in the Numeric/Boolean state object. Require that exact channel;
        // no implicit empty value or synthetic control is introduced here.
        if(const auto *state=widget.member("state");state&&!obj(state,path+"/state").object.empty())refuse(path+"/state","String/Path indicators cannot carry Numeric/Boolean state.","migration.record_indicator_state_invalid", Domain::Integrity);
        const auto &props=obj(widget.member("props"),path+"/props");const auto *text=props.member("text.value");
        if(!text||text->type!=Json::Type::String)refuse(path+"/props/text.value","The real String/Path indicator requires its explicit authored text value.","migration.record_indicator_state_unavailable", Domain::Semantics);return *text;
    }
    const auto &state=obj(widget.member("state"),path+"/state");
    for(const auto &[key,_]:state.object)if(key!="value"&&key!="initial_value"&&key!="numeric_value_present")
        refuse(path+"/state/"+key,"Unknown widget state needs an explicit migration contract.","migration.widget_state_unsupported", Domain::Compatibility);
    if(const auto *lex=state.member("initial_value")){
        if(lex->type!=Json::Type::String)refuse(path+"/state/initial_value","Exact numeric state must be a string.", "migration.unsupported", Domain::Integrity);
        const auto *present=state.member("numeric_value_present");
        if(!present||present->type!=Json::Type::Boolean||state.member("value"))refuse(path+"/state","Lexical numeric state requires its Boolean cache-presence flag and no conflicting numeric value.","migration.widget_state_invalid", Domain::Semantics);
        if(gammaLimit&&type=="f64"&&(lex->string=="+Infinity"||lex->string=="+Inf"))return string("+Infinity");
        if(!exactIntegerState||(type!="i64"&&type!="u64"))refuse(path+"/state/initial_value","Only admitted revision-4 exact integer lexical state is supported by this adapter.","migration.exact_state_revision_required", Domain::Compatibility);
        Json raw;raw.type=Json::Type::Number;raw.string=lex->string;
        const auto parsed=widgetScalarState(raw,type);
        if(!parsed)refuse(path+"/state/initial_value","Numeric state is outside its exact declared type.","migration.widget_state_invalid", Domain::Semantics);return *parsed;
    }
    if(state.member("numeric_value_present"))refuse(path+"/state","Numeric cache presence without lexical state has no admitted meaning.","migration.widget_state_invalid", Domain::Semantics);
    const auto *v=state.member("value");const auto parsed=v?widgetScalarState(*v,type):std::nullopt;
    if(!parsed)refuse(path+"/state/value","A real, correctly typed value is required; no default is inferred.","migration.widget_state_invalid", Domain::Semantics);return *parsed;
}
std::optional<double> authoredFixedNumericConstant(const Json &node) {
    const auto *kind=node.member("kind"),*type=node.member("type");
    if(!kind||kind->type!=Json::Type::String||kind->string!="primitive"||!type||type->type!=Json::Type::String)return {};
    std::string_view id=type->string;if(id.starts_with("frog.studio."))id.remove_prefix(12);
    if(!id.starts_with("numeric.other_constants."))return {};
    const auto *entry=findFunctionCatalogEntry(id);
    return entry?functionFixedNumericConstantValue(*entry):std::nullopt;
}
std::optional<std::string_view> authoredFixedStringConstant(const Json &node) {
    const auto *kind=node.member("kind"),*type=node.member("type");
    if(!kind||kind->type!=Json::Type::String||kind->string!="primitive"||!type||type->type!=Json::Type::String)return {};
    std::string_view id=type->string;if(id.starts_with("frog.studio."))id.remove_prefix(12);
    const auto *entry=findFunctionCatalogEntry(id);
    return entry?functionFixedStringConstantValue(*entry):std::nullopt;
}
std::string_view scalarLiteralType(const Json &node) {
    const auto *kind=node.member("kind"),*type=node.member("type");
    if(!kind||kind->type!=Json::Type::String||kind->string!="primitive"||!type||type->type!=Json::Type::String)return {};
    if(authoredFixedNumericConstant(node))return "f64";
    if(type->string=="frog.studio.boolean.true_constant"||type->string=="boolean.true_constant"||
       type->string=="frog.studio.boolean.false_constant"||type->string=="boolean.false_constant")return "bool";
    if(type->string=="frog.studio.numeric.numeric_constant"||type->string=="numeric.numeric_constant") {
        const auto *valueType=node.member("value_type");
        if(valueType&&valueType->type==Json::Type::String)return valueType->string;
    }
    return {};
}
Json literalValue(const Json &node,std::string_view type,const std::string &path) {
    if(const auto fixed=authoredFixedNumericConstant(node)) {
        if(type!="f64"||!std::isfinite(*fixed))refuse(path+"/type","The compiled fixed-constant profile admits finite f64 constants only; IEEE infinity requires its explicit value transport.","migration.constant_type_unsupported", Domain::FrontendCapability);
        char buffer[64];
        const auto formatted=std::to_chars(buffer,buffer+sizeof(buffer),*fixed,std::chars_format::general,std::numeric_limits<double>::max_digits10);
        if(formatted.ec!=std::errc{})refuse(path,"Fixed numeric constant formatting failed.","migration.constant_value_invalid", Domain::Control);
        Json result;result.type=Json::Type::Number;result.string.assign(buffer,formatted.ptr);return result;
    }

    if(type!="bool"&&type!="f64"&&type!="i32"&&type!="u32"&&type!="i64"&&type!="u64")
        refuse(path+"/value_type","Only exact bool/f64/i32/u32/i64/u64 scalar constants belong to frog.studio.scalar-constant@1.","migration.constant_type_unsupported", Domain::FrontendCapability);
    const auto *raw=node.member("text");std::string_view text;
    if(raw){if(raw->type!=Json::Type::String)refuse(path+"/text","The authored constant text must be a string.", "migration.unsupported", Domain::Integrity);text=raw->string;}
    if(type=="bool") {
        if(text.empty())return boolean(txt(node.member("type"),path).find("true_constant")!=std::string::npos);
        if(text=="true"||text=="TRUE"||text=="1")return boolean(true);
        if(text=="false"||text=="FALSE"||text=="0")return boolean(false);
        refuse(path+"/text","The authored Boolean literal is not one of the existing exact Boolean spellings.","migration.constant_value_invalid", Domain::Semantics);
    }
    const auto first=text.find_first_not_of(" \t\r\n");
    if(first==text.npos)refuse(path+"/text","A numeric constant requires its real authored value; no zero is inferred.","migration.constant_value_invalid", Domain::Semantics);
    text=text.substr(first,text.find_last_not_of(" \t\r\n")-first+1);if(text.starts_with('+'))text.remove_prefix(1);
    std::optional<Json> parsed;
    if(type=="i32"||type=="u32"||type=="i64"||type=="u64") {
        const auto integer=numeric_value::canonicalWideIntegerLexeme(text,type=="i32"?"i64":type=="u32"?"u64":type);
        if(integer)parsed=source::parseJson(*integer,nullptr,source::JsonNumberPolicy::PreserveLexeme);
    } else {
        parsed=source::parseJson(text,nullptr,source::JsonNumberPolicy::PreserveLexeme);
        if(!parsed||!typedScalar(*parsed,"f64")) {
            // The editor also accepts e.g. +.5. Canonicalize only f64 through
            // its own IEEE carrier; integers never pass through this path.
            double number=0;const auto converted=std::from_chars(text.data(),text.data()+text.size(),number);
            if(converted.ec==std::errc{}&&converted.ptr==text.data()+text.size()&&std::isfinite(number)) {
                char encoded[64];const auto written=std::to_chars(encoded,encoded+sizeof encoded,number,std::chars_format::general,std::numeric_limits<double>::max_digits10);
                if(written.ec==std::errc{})parsed=source::parseJson(std::string_view(encoded,static_cast<std::size_t>(written.ptr-encoded)),nullptr,source::JsonNumberPolicy::PreserveLexeme);
            }
        }
    }
    if(!parsed||!typedScalar(*parsed,type))refuse(path+"/text","The scalar literal is outside its exact declared type.","migration.constant_value_invalid", Domain::Semantics);
    return *parsed;
}

bool recordLiteral(const Json &node) {
    const auto *kind=node.member("kind"),*type=node.member("type");
    return kind&&kind->type==Json::Type::String&&kind->string=="primitive"&&type&&type->type==Json::Type::String&&
        (type->string=="frog.studio.cluster.constant"||type->string=="cluster.constant"||type->string=="frog.studio.error.constant");
}
std::string_view authoredTextLiteral(const Json &node) {
    const auto *kind=node.member("kind"),*type=node.member("type");
    if(!kind||kind->type!=Json::Type::String||kind->string!="primitive"||!type||type->type!=Json::Type::String)return {};
    if(type->string=="frog.studio.string.constant"||type->string=="string.constant"||authoredFixedStringConstant(node))return "string";
    if(type->string=="frog.studio.path.constant"||type->string=="path.constant")return "path";return {};
}
void literalTextPresentation(const Json &node,std::string_view type,const std::string &path) {
    draft_presentation::literalText(node,type,path,[](auto at,auto message,auto code) {
        refuse(std::move(at),std::move(message),std::move(code), Domain::Compatibility);
    });
}
bool authoredObjectNode(const Json &node) { return node.member("studio_application_control")!=nullptr; }
bool objectType(const binding::TypeRef &type) {
    if(type.kind==binding::TypeKind::Reference)return true;
    return std::any_of(type.children.begin(),type.children.end(),[](const auto &child){return objectType(child);});
}
std::string_view recordFunction(const Json &node) {
    const auto *type=node.member("type");if(!type||type->type!=Json::Type::String)return {};
    std::string_view id=type->string;if(id.starts_with("frog.studio."))id.remove_prefix(12);
    if(id=="cluster.bundle"||id=="cluster.bundle_by_name"||id=="cluster.unbundle"||id=="cluster.unbundle_by_name")return id;
    return {};
}
bool authoredUnbundle(const Json &node) {
    const auto *kind=node.member("kind"),*type=node.member("type");
    return kind&&kind->type==Json::Type::String&&kind->string=="primitive"&&type&&type->type==Json::Type::String&&
        (type->string=="frog.studio.cluster.unbundle_by_name"||type->string=="cluster.unbundle_by_name");
}
std::string pointerToken(std::string_view value) {
    std::string result;for(char ch:value){if(ch=='~')result+="~0";else if(ch=='/')result+="~1";else result+=ch;}return result;
}
std::string ownedIdentity(const Json *value,const std::string &path,const char *code) {
    if(!value||value->type!=Json::Type::String||value->string.empty())refuse(path,"Owned literal identities must be explicit nonempty strings.",code, Domain::Integrity);return value->string;
}
unsigned unsignedField(const Json *value,const std::string &path,unsigned maximum) {
    if(!value||!typedScalar(*value,"u32"))refuse(path,"An exact unsigned authored order/count is required.","migration.record_identity_invalid", Domain::Integrity);
    unsigned result=0;const auto parsed=std::from_chars(value->string.data(),value->string.data()+value->string.size(),result);
    if(parsed.ec!=std::errc{}||parsed.ptr!=value->string.data()+value->string.size()||result>maximum)refuse(path,"Authored order/count exceeds its exact contract.","migration.record_identity_invalid", Domain::Semantics);
    return result;
}
// This reader uses the writer's stable data ownership, never rectangles,
// labels or a second interpretation of the runtime Record schema.
struct AuthoredRecords {
    struct Literal {binding::TypeRef type;Json value;std::vector<std::size_t> fields;};
    const std::vector<Json> &nodes,&edges;std::string path;
    std::map<std::string,std::size_t> byId;
    std::map<std::string,std::vector<std::size_t>> children;
    std::map<std::string,Literal> literals;
    std::set<std::string> owned,active;
    AuthoredRecords(const std::vector<Json> &ns,const std::vector<Json> &es,std::string scope):nodes(ns),edges(es),path(std::move(scope)) {
        for(std::size_t i=0;i<nodes.size();++i) {
            const auto at=where(i);const auto id=txt(nodes[i].member("id"),at+"/id");
            if(!byId.emplace(id,i).second)refuse(at,"Duplicate authored identity.","migration.identity_duplicate", Domain::Integrity);
            if(const auto *owner=nodes[i].member("constant_owner_node_id")) {
                children[ownedIdentity(owner,at+"/constant_owner_node_id","migration.record_owner_invalid")].push_back(i);owned.insert(id);
            }
        }
        for(const auto &[owner,fields]:children) {
            const auto found=byId.find(owner);
            if(found==byId.end()||!recordLiteral(nodes[found->second]))refuse(where(fields.front())+"/constant_owner_node_id","A literal component must name a real Cluster constant in the same explicit subdiagram.","migration.record_owner_invalid", Domain::Semantics);
        }
        for(std::size_t e=0;e<edges.size();++e)for(const auto side:{"from","to"}) {
            const auto &endpoint=obj(edges[e].member(side),path+"/edges/"+std::to_string(e)+"/"+side);
            if(owned.contains(txt(endpoint.member("node"),path)))refuse(path+"/edges/"+std::to_string(e)+"/"+side,"A Cluster literal component is data, not an independently wired node.","migration.record_component_wired", Domain::Semantics);
        }
        for(const auto &[id,i]:byId)if(recordLiteral(nodes[i]))build(id,0);
    }
    std::string where(std::size_t i)const{return path+"/nodes/"+std::to_string(i);}
    Literal build(const std::string &id,unsigned depth) {
        if(const auto found=literals.find(id);found!=literals.end())return found->second;
        const auto at=where(byId.at(id));const auto &node=nodes[byId.at(id)];
        if(txt(node.member("kind"),at+"/kind")!="primitive")refuse(at+"/kind","A Cluster literal component must be a real primitive literal, never a comment, widget terminal or structure.","migration.record_component_kind_invalid", Domain::Semantics);
        if(depth>32||!active.insert(id).second)refuse(at,"Literal ownership exceeds depth32 or contains a cycle.","migration.record_owner_invalid", Domain::Semantics);
        Literal result;
        if(recordLiteral(node)) {
            if(const auto *collapsed=node.member("cluster_constant_collapsed"))flag(collapsed,at+"/cluster_constant_collapsed");
            if(const auto *sizing=node.member("cluster_constant_autosizing"))txt(sizing,at+"/cluster_constant_autosizing");
            for(const auto dimension:{"cluster_expanded_width","cluster_expanded_height"})if(const auto *number=node.member(dimension);number&&(!typedScalar(*number,"f64")||std::stod(number->string)<=0))refuse(at+"/"+dimension,"Expanded Cluster presentation must retain its finite positive dimensions.","migration.record_presentation_invalid", Domain::Semantics);
            result.fields=children[id];
            if(result.fields.empty())refuse(at,"An empty authored Cluster literal has no complete data schema.","migration.record_empty_literal", Domain::Semantics);
            if(result.fields.size()>64)refuse(at,"The bounded Record carrier supports at most64 explicit fields.","migration.record_field_count_unavailable", Domain::FrontendCapability);
            std::map<unsigned,std::size_t> order;std::set<std::string> unique;
            for(const auto child:result.fields) {
                const auto cp=where(child),field=ownedIdentity(nodes[child].member("constant_field_id"),cp+"/constant_field_id","migration.record_identity_invalid");
                if(!unique.insert(field).second)refuse(cp+"/constant_field_id","Cluster field IDs must be unique independently of their display names.","migration.record_identity_invalid", Domain::Integrity);
                const auto ordinal=unsignedField(nodes[child].member("constant_field_order"),cp+"/constant_field_order",(std::numeric_limits<unsigned>::max)());
                if(!order.emplace(ordinal,child).second)refuse(cp+"/constant_field_order","Cluster field order must be explicit and unique.","migration.record_identity_invalid", Domain::Integrity);
            }
            std::vector<std::string> ids,names;std::vector<binding::TypeRef> types;result.value=object();result.fields.clear();
            for(const auto &[_,child]:order) {
                const auto field=txt(nodes[child].member("constant_field_id"),where(child));const auto part=build(txt(nodes[child].member("id"),where(child)),depth+1);
                result.fields.push_back(child);ids.push_back(field);types.push_back(part.type);result.value.object.emplace(field,part.value);
                const auto *label=nodes[child].member("label");names.push_back(label&&label->type==Json::Type::String?label->string:field);
            }
            result.type=binding::TypeRef::cluster(std::move(ids),std::move(types));result.type.fieldNames=std::move(names);CanonicalRecordDiagnostic diagnostic;
            if(!objectType(result.type)&&(!canonicalRecordTransportType(result.type.canonical(),&diagnostic)||!canonicalRecordLiteralValid(result.type,result.value,&diagnostic)))refuse(at,diagnostic.message,
                diagnostic.domain==Domain::FrontendCapability||diagnostic.domain==Domain::Control?"migration.record_transport_unavailable":"migration.record_literal_invalid", diagnostic.domain);
            if(const auto *hint=node.member("value_type");hint&&txt(hint,at)!=result.type.canonical())refuse(at+"/value_type","The saved literal hint conflicts with the actual stable field schema.","migration.record_type_conflict", Domain::Semantics);
        } else if(authoredObjectNode(node)) {
            CanonicalObjectReferenceDiagnostic diagnostic;const auto descriptor=canonicalObjectReferenceDescriptor(*node.member("studio_application_control"),at+"/studio_application_control",{},&diagnostic);
            if(!descriptor||descriptor->kind!=CanonicalObjectReferenceKind::Resolve)refuse(at,descriptor?"Only real Reference constants may be contained literal data.":diagnostic.message,"migration.object_reference.literal_invalid", !descriptor&&diagnostic.unsupported?Domain::FrontendCapability:Domain::Semantics);
            const auto full=source::parseJson(descriptor->ports.front().fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
            const auto type=full?canonicalObjectReferenceReadType(*full,&diagnostic):std::nullopt;
            if(!type)refuse(at,diagnostic.message,"migration.object_reference.schema_invalid", diagnostic.unsupported?Domain::FrontendCapability:Domain::Semantics);result.type=*type;
            result.value=object();result.value.object.emplace(std::string(canonicalObjectReferenceAttribute),*node.member("studio_application_control"));
        } else if(const auto scalar=scalarLiteralType(node);!scalar.empty()) {
            if(scalar=="bool")if(const auto *hint=node.member("value_type");hint&&txt(hint,at)!="bool")refuse(at+"/value_type","Boolean literal type conflicts with its actual catalog.","migration.record_type_conflict", Domain::Semantics);
            result.type=binding::TypeRef::primitive(std::string(scalar));result.value=literalValue(node,scalar,at);
        } else {
            const auto type=txt(node.member("type"),at+"/type");
            const auto literalType=(type=="frog.studio.string.constant"||type=="string.constant")?"string":(type=="frog.studio.path.constant"||type=="path.constant")?"path":"";
            if(!*literalType)refuse(at,"A Record field must be a finite exact scalar, String/Path literal or another immutable Record; Array/reference/control transport remains unavailable.","migration.record_transport_unavailable", Domain::FrontendCapability);
            literalTextPresentation(node,literalType,at);
            const auto *raw=node.member("text");if(raw&&raw->type!=Json::Type::String)refuse(at+"/text","The real literal text must remain an exact UTF-8 string.","migration.record_literal_invalid", Domain::Semantics);
            result.type=binding::TypeRef::primitive(literalType);result.value=raw?*raw:string("");
            if(const auto *hint=node.member("value_type");hint&&txt(hint,at)!=literalType)refuse(at+"/value_type","The saved literal type conflicts with its real catalog.","migration.record_type_conflict", Domain::Semantics);
        }
        active.erase(id);literals.emplace(id,result);return result;
    }
    unsigned rows(const Json &node,const std::string &at)const {
        if(const auto *show=node.member("cluster_show_full_names"))flag(show,at+"/cluster_show_full_names");
        if(const auto *names=node.member("cluster_field_names")) {
            const auto &values=arr(names,at+"/cluster_field_names");if(values.size()>32)refuse(at,"Hidden/visible row names exceed the writer's 32-row configuration.","migration.unbundle_selection_invalid", Domain::Semantics);
            for(const auto &name:values)if(name.type!=Json::Type::String)refuse(at,"Cluster row labels must remain text; they are never used as field IDs.","migration.unbundle_selection_invalid", Domain::Semantics);
        }
        if(const auto *paths=node.member("cluster_field_id_paths")) {
            const auto &values=arr(paths,at+"/cluster_field_id_paths");if(values.size()>32)refuse(at,"Stable row configuration exceeds the writer's 32-row limit.","migration.unbundle_selection_invalid", Domain::Semantics);
            for(const auto &entry:values)for(const auto &part:arr(&entry,at))if(part.type!=Json::Type::String||part.string.empty())refuse(at,"Stable paths contain explicit nonempty IDs only.","migration.unbundle_selection_invalid", Domain::Semantics);
        }
        const auto *count=node.member("cluster_function_terminals");const auto result=count?unsignedField(count,at+"/cluster_function_terminals",32):1;
        if(!result)refuse(at,"Unbundle must have at least one active output.","migration.unbundle_selection_invalid", Domain::Semantics);return result;
    }
    std::string selected(const Json &node,unsigned row,const std::string &at)const {
        const auto *raw=node.member("cluster_field_id_paths");
        if(!raw||raw->type!=Json::Type::Array||row>=raw->array.size())refuse(at+"/cluster_field_id_paths","Unbundle migration requires its persisted stable field IDs; names are never reinterpreted as IDs.","migration.unbundle_stable_selection_required", Domain::Semantics);
        const auto &ids=arr(&raw->array[row],at+"/cluster_field_id_paths/"+std::to_string(row));
        if(ids.empty())refuse(at+"/cluster_field_id_paths/"+std::to_string(row),"An unfinished Unbundle row has no stable selected field.","migration.unbundle_selection_incomplete", Domain::Semantics);
        if(ids.size()!=1)refuse(at+"/cluster_field_id_paths/"+std::to_string(row),"This corridor extracts one exact top-level field per row. Select a nested Record then use a second Unbundle; multi-level paths require a separate explicit lowering.","migration.unbundle_nested_path_unavailable", Domain::FrontendCapability);
        return txt(&ids[0],at+"/cluster_field_id_paths/"+std::to_string(row)+"/0");
    }
    binding::TypeRef output(const std::string &id,const std::string &port,std::set<std::string> &visiting)const {
        const auto found=byId.find(id);if(found==byId.end())refuse(path,"Record driver must belong to the same explicit subdiagram.","migration.unbundle_driver_invalid", Domain::Semantics);
        const auto at=where(found->second);const auto &node=nodes[found->second];
        if(const auto literal=literals.find(id);literal!=literals.end()&&port=="value"&&!owned.contains(id))return literal->second.type;
        if(!authoredUnbundle(node))refuse(at,"Unbundle requires an immutable Record or another admitted exact Unbundle output.","migration.unbundle_driver_unavailable", Domain::FrontendCapability);
        if(!visiting.insert(id).second)refuse(at,"Unbundle input dependencies contain a cycle.","migration.unbundle_driver_invalid", Domain::Semantics);
        const auto input=cluster(node,at,visiting);const auto count=rows(node,at);
        for(unsigned row=0;row<count;++row)if(port=="element"+std::to_string(row+1)) {
            const auto field=selected(node,row,at);const auto match=std::find(input.fieldIds.begin(),input.fieldIds.end(),field);
            if(match==input.fieldIds.end())refuse(at,"The selected stable field no longer exists.","migration.unbundle_field_missing", Domain::Semantics);
            visiting.erase(id);return input.children[std::size_t(match-input.fieldIds.begin())];
        }
        refuse(at,"Unbundle output does not name an active authored row.","migration.unbundle_selection_invalid", Domain::Semantics);
    }
    binding::TypeRef cluster(const Json &node,const std::string &at,std::set<std::string> &visiting)const {
        const auto id=txt(node.member("id"),at);const Json *source=nullptr;
        for(const auto &edge:edges) {
            const auto &to=obj(edge.member("to"),path);
            if(txt(to.member("node"),path)==id&&txt(to.member("port"),path)=="cluster") {
                if(source)refuse(at,"Unbundle has multiple input drivers.","migration.unbundle_driver_invalid", Domain::Semantics);source=&obj(edge.member("from"),path);
            }
        }
        if(!source)refuse(at,"Unbundle has no real Record driver.","migration.unbundle_driver_invalid", Domain::Semantics);
        auto type=output(txt(source->member("node"),at),txt(source->member("port"),at),visiting);
        if(type.kind!=binding::TypeKind::Cluster)refuse(at,"The actual Unbundle driver is not a Record.","migration.unbundle_driver_invalid", Domain::Semantics);return type;
    }
};


// Closed authored type traversal for the local object/Record instructions.
// No UI, catalogue, graph evaluator or implicit Value dereference participates.
struct AuthoredObjectReferences {
    AuthoredRecords &records;const std::map<std::string,std::string> &widgets;
    std::map<std::pair<std::string,std::string>,binding::TypeRef> cache;
    std::map<std::string,CanonicalObjectReferenceDescriptor> descriptors;
    std::set<std::pair<std::string,std::string>> active;
    const Json *driver(std::string_view id,std::string_view port)const {
        const Json *found=nullptr;
        for(const auto &edge:records.edges){const auto &to=obj(edge.member("to"),records.path);
            if(txt(to.member("node"),records.path)==id&&txt(to.member("port"),records.path)==port){if(found)refuse(records.path,"An object input has multiple actual drivers.","migration.object_reference.driver_invalid", Domain::Semantics);found=&obj(edge.member("from"),records.path);}}
        return found;
    }
    binding::TypeRef input(std::string_view id,std::string_view port) {
        const auto *from=driver(id,port);if(!from)refuse(records.path,"The actual typed input has no driver: "+std::string(id)+":"+std::string(port),"migration.object_reference.missing_driver", Domain::Semantics);
        return output(txt(from->member("node"),records.path),txt(from->member("port"),records.path));
    }
    CanonicalObjectReferencePort port(std::string id,const binding::TypeRef &type,bool input)const {
        return {std::move(id),type.canonical(),encode(canonicalObjectReferenceWriteType(type)),input,true,{}};
    }
    unsigned rows(const Json &node,const std::string &at)const {
        if(node.member("cluster_function_terminals"))return records.rows(node,at);
        return recordFunction(node).ends_with("_by_name")?1U:2U;
    }
    std::vector<std::string> path(const Json &node,const binding::TypeRef &schema,unsigned row,const std::string &at)const {
        if(recordFunction(node).ends_with("_by_name")){
            const auto *paths=node.member("cluster_field_id_paths");if(!paths||paths->type!=Json::Type::Array||row>=paths->array.size())refuse(at,"By Name needs its actual persisted stable field path.","migration.object_reference.field_incomplete", Domain::Semantics);
            std::vector<std::string> result;for(const auto &part:arr(&paths->array[row],at))result.push_back(txt(&part,at));
            if(result.empty()||result.size()>32)refuse(at,"By Name requires a complete bounded stable field path.","migration.object_reference.field_incomplete", Domain::Semantics);return result;
        }
        if(row>=schema.fieldIds.size())refuse(at,"The actual ordinal is outside the complete Record schema.","migration.object_reference.field_missing", Domain::Semantics);return {schema.fieldIds[row]};
    }
    binding::TypeRef selected(const binding::TypeRef &schema,const std::vector<std::string> &path,const std::string &at)const {
        // Traverse stable owned children; assigning a child over its parent
        // destroys the source subobject before all of its fields are copied.
        const auto *type=&schema;
        for(const auto &id:path){if(type->kind!=binding::TypeKind::Cluster)refuse(at,"A field path traverses a non-Record value.","migration.object_reference.field_invalid", Domain::Semantics);
            const auto f=std::find(type->fieldIds.begin(),type->fieldIds.end(),id);if(f==type->fieldIds.end())refuse(at,"A selected stable field no longer exists.","migration.object_reference.field_missing", Domain::Semantics);type=&type->children[std::size_t(f-type->fieldIds.begin())];}return *type;
    }
    CanonicalObjectReferenceDescriptor describe(const std::string &id) {
        if(const auto found=descriptors.find(id);found!=descriptors.end())return found->second;
        const auto found=records.byId.find(id);if(found==records.byId.end())refuse(records.path,"Unknown object operation identity.", "migration.unsupported", Domain::Compatibility);
        const auto &node=records.nodes[found->second];const auto at=records.where(found->second);CanonicalObjectReferenceDiagnostic diagnostic;
        std::optional<CanonicalObjectReferenceDescriptor> descriptor;
        if(authoredObjectNode(node)){
            auto wired=binding::TypeRef{};if(driver(id,"reference_in"))wired=input(id,"reference_in");
            descriptor=canonicalObjectReferenceDescriptor(*node.member("studio_application_control"),at+"/studio_application_control",wired,&diagnostic);
            if(descriptor){const auto actual=txt(node.member("type"),at);std::string_view expected=descriptor->kind==CanonicalObjectReferenceKind::Resolve?"application_control.reference":descriptor->kind==CanonicalObjectReferenceKind::Property?"application_control.property_node":descriptor->kind==CanonicalObjectReferenceKind::Close?"application_control.close_reference":"application_control.invoke_node";
                std::string_view normalized=actual;if(normalized.starts_with("frog.studio."))normalized.remove_prefix(12);
                if(normalized!=expected)refuse(at,"The full configuration kind contradicts its real authored operation.","migration.object_reference.configuration_invalid", Domain::Semantics);}
        }else if(const auto literal=records.literals.find(id);literal!=records.literals.end()){
            descriptor=canonicalObjectReferenceRecordDescriptor(CanonicalObjectReferenceKind::Resolve,literal->second.type,{port("value",literal->second.type,false)},{},&literal->second.value,at,&diagnostic);
        }else {
            const auto function=recordFunction(node);if(function.empty())refuse(at,"This authored operation has no local object/Record contract.","migration.object_reference.operation_unavailable", Domain::FrontendCapability);
            const bool bundle=function.starts_with("cluster.bundle"),rail=driver(id,"cluster")!=nullptr;
            const auto count=rows(node,at);binding::TypeRef schema;
            std::vector<CanonicalObjectReferencePort> ports;std::vector<std::vector<std::string>> paths;
            if(rail){schema=input(id,"cluster");if(schema.kind!=binding::TypeKind::Cluster)refuse(at,"The real cluster rail is not a Record.","migration.object_reference.schema_invalid", Domain::Semantics);ports.push_back(port("cluster",schema,true));}
            else if(!bundle)refuse(at,"Unbundle requires its actual complete Record driver.","migration.object_reference.missing_driver", Domain::Semantics);
            else if(function.ends_with("_by_name"))refuse(at,"Bundle By Name requires its actual complete schema rail.","migration.object_reference.missing_driver", Domain::Semantics);
            else {std::vector<std::string> ids,names;std::vector<binding::TypeRef> types;
                for(unsigned row=0;row<count;++row){const auto key="element"+std::to_string(row+1);ids.push_back(key);names.push_back("Element "+std::to_string(row+1));types.push_back(input(id,key));}
                schema=binding::TypeRef::cluster(std::move(ids),std::move(types));schema.fieldNames=std::move(names);}
            for(unsigned row=0;row<count;++row){const auto key="element"+std::to_string(row+1);if(bundle&&rail&&!driver(id,key))continue;
                const auto field=path(node,schema,row,at);const auto type=bundle?input(id,key):selected(schema,field,at);
                ports.push_back(port(key,type,bundle));paths.push_back(field);}
            if(bundle)ports.push_back(port("result",schema,false));
            descriptor=canonicalObjectReferenceRecordDescriptor(bundle?(rail?CanonicalObjectReferenceKind::Update:CanonicalObjectReferenceKind::Construct):CanonicalObjectReferenceKind::Select,schema,std::move(ports),std::move(paths),nullptr,at,&diagnostic);
        }
        if(!descriptor)refuse(at,diagnostic.message,"migration.object_reference.configuration_invalid", diagnostic.unsupported?Domain::FrontendCapability:Domain::Semantics);descriptors.emplace(id,*descriptor);return *descriptor;
    }
    binding::TypeRef output(const std::string &id,const std::string &outputPort) {
        const auto key=std::make_pair(id,outputPort);if(const auto found=cache.find(key);found!=cache.end())return found->second;
        if(active.size()>32||!active.insert(key).second)refuse(records.path,"Object/Record type dependencies contain a cycle or exceed depth32.","migration.object_reference.type_cycle", Domain::Semantics);
        const auto found=records.byId.find(id);if(found==records.byId.end())refuse(records.path,"The actual driver is absent from its explicit scope.", "migration.unsupported", Domain::Integrity);
        const auto &node=records.nodes[found->second];const auto at=records.where(found->second);binding::TypeRef result;
        if(authoredObjectNode(node)||!recordFunction(node).empty()){
            const auto descriptor=describe(id);const auto p=std::find_if(descriptor.ports.begin(),descriptor.ports.end(),[&](const auto &p){return !p.input&&p.id==outputPort;});
            if(p==descriptor.ports.end())refuse(at,"The edge names no real active output.","migration.object_reference.port_invalid", Domain::Semantics);
            const auto json=source::parseJson(p->fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);const auto type=json?canonicalObjectReferenceReadType(*json):std::nullopt;if(!type)refuse(at,"The complete output schema was lost.", "migration.unsupported", Domain::Integrity);result=*type;
        }else if(const auto literal=records.literals.find(id);literal!=records.literals.end()&&outputPort=="value")result=literal->second.type;
        else if(const auto scalar=scalarLiteralType(node);!scalar.empty()&&outputPort=="value")result=binding::TypeRef::primitive(std::string(scalar));
        else if(txt(node.member("kind"),at)=="widget_value"&&outputPort=="value"){
            const auto widget=txt(node.member("widget"),at);const auto w=widgets.find(widget);if(w==widgets.end())refuse(at,"The terminal has no actual widget declaration.", "migration.unsupported", Domain::Semantics);result=binding::TypeRef::primitive(w->second);
        }else if(!authoredTextLiteral(node).empty()&&outputPort=="value")result=binding::TypeRef::primitive(std::string(authoredTextLiteral(node)));
        else refuse(at,"The input needs an explicitly typed authored literal, widget or local Record/reference output.","migration.object_reference.driver_unavailable", Domain::FrontendCapability);
        active.erase(key);cache.emplace(key,result);return result;
    }
    bool handles(const std::string &id) {
        const auto &node=records.nodes[records.byId.at(id)];if(authoredObjectNode(node))return true;
        if(const auto literal=records.literals.find(id);literal!=records.literals.end())return objectType(literal->second.type);
        const auto kind=recordFunction(node);if(kind.empty())return false;
        if(kind!="cluster.unbundle_by_name")return true;
        const auto schema=input(id,"cluster");if(objectType(schema))return true;
        const auto *from=driver(id,"cluster");const auto &source=records.nodes[records.byId.at(txt(from->member("node"),records.path))];
        if(!recordFunction(source).empty()&&!authoredUnbundle(source))return true;
        const auto *paths=node.member("cluster_field_id_paths");return paths&&std::any_of(paths->array.begin(),paths->array.end(),[](const auto &p){return p.array.size()>1;});
    }
};

bool addedStructurePresentation(std::string_view key) {
    // Exact writer fields only: future execution fields may not be accepted
    // merely because their name shares an authoring prefix.
    static constexpr std::string_view names[]{
        "structure_subdiagram_label_alignment",
        "for_loop_conditional_terminal","for_loop_stop_if_true","for_loop_condition_normalized_x","for_loop_condition_normalized_y",
        "for_loop_iteration_normalized_x","for_loop_iteration_normalized_y","for_loop_iteration_visible",
        "for_loop_subdiagram_label_visible","for_loop_subdiagram_label_text","for_loop_subdiagram_label_height",
        "for_loop_background_rgb","for_loop_background_opacity_percent","for_loop_subframe_rgb","for_loop_subframe_opacity_percent",
        "case_structure_radix","case_structure_true_visible","case_structure_visible_region_id","case_structure_selector_normalized_y","case_structure_manual_order",
        "case_structure_subdiagram_label_visible","case_structure_true_subdiagram_label_text","case_structure_false_subdiagram_label_text","case_structure_subdiagram_label_height",
        "case_structure_true_background_rgb","case_structure_true_background_opacity_percent","case_structure_true_subframe_rgb","case_structure_true_subframe_opacity_percent",
        "case_structure_false_background_rgb","case_structure_false_background_opacity_percent","case_structure_false_subframe_rgb","case_structure_false_subframe_opacity_percent"};
    return std::find(std::begin(names),std::end(names),key)!=std::end(names);
}
struct Migration {
    CanonicalDraftMigrationResult &out;
    const CanonicalDraftResolvedTypes *resolvedTypes=nullptr;
    std::set<std::string> consumedResolutions;
    bool exactIntegerState=false;
    bool nativeCallDraft7=false,hasNativeCall=false,objectDraft=false,hasObjectReferences=false;
    bool hasScalarArithmetic=false;
    bool hasBooleanReduction=false;
    bool hasScalar32=false;
    bool hasUnaryProfile=false;
    bool hasComparisonZero=false;
    bool hasStringBytes=false;
    bool hasArrayReal1d=false;
    std::set<std::string> nodes,edges,widgets,all;
    std::map<std::string,std::string> widgetTypes;
    std::set<std::string> gammaLimitWidgets,nonGammaLimitWidgets;
    std::string_view selectionIdentity(const Json &node,const std::vector<Json> &sourceEdges,const std::string &path) {
        const auto type=txt(node.member("type"),path+"/type");const auto *contract=canonicalScalarSelectionAuthoredContract(type);if(!contract)return {};
        const auto id=txt(node.member("id"),path+"/id");
        // The plain migration API may propose canonical Source, whose exact
        // FIR edges still validate types. Common preparation supplies actual
        // source-connector resolutions to reject editor promotion up front.
        if(!resolvedTypes)return contract->operationId;
        const auto found=std::find_if(resolvedTypes->scalars.begin(),resolvedTypes->scalars.end(),[&](const auto &r){return r.nodeId==id;});
        if(found==resolvedTypes->scalars.end()||found->authoredOperation!=type||found->operands.size()!=contract->inputs.size()||!consumedResolutions.insert(id).second)
            refuse(path,"Scalar selection requires the complete current authored connector resolutions.","migration.scalar_resolution_incomplete", Domain::Semantics);
        std::string dataType;std::size_t edges=0;
        for(const auto &edge:sourceEdges)if(txt(obj(edge.member("to"),path).member("node"),path)==id)++edges;
        if(edges!=contract->inputs.size())refuse(path,"Every scalar selection input needs exactly one real driver.","migration.scalar_resolution_incomplete", Domain::Semantics);
        for(std::size_t i=0;i<contract->inputs.size();++i){const auto port=contract->inputs[i];const auto &actual=found->operands[i];const Json *driver=nullptr;
            if(actual.inputPortId!=port||!actual.accepted||!actual.complete||actual.valueType.empty())refuse(path,"Every scalar selection source must be accepted and complete.","migration.scalar_resolution_incomplete", Domain::Semantics);
            for(const auto &edge:sourceEdges){const auto &to=obj(edge.member("to"),path);if(txt(to.member("node"),path)==id&&txt(to.member("port"),path)==port){if(driver)refuse(path,"Scalar selection has a duplicate driver.","migration.scalar_resolution_incomplete", Domain::Semantics);driver=&obj(edge.member("from"),path);}}
            if(!driver||txt(driver->member("node"),path)!=actual.sourceNodeId||txt(driver->member("port"),path)!=actual.sourcePortId)refuse(path,"Scalar selection resolution differs from its exact Source driver.","migration.scalar_resolution_incomplete", Domain::Semantics);
            if(contract->kind==CanonicalScalarSelectionKind::Select&&port=="select"){if(actual.valueType!="bool")refuse(path,"The local Select condition is exact bool, not Error or numeric.","migration.scalar_type_unavailable", Domain::Semantics);continue;}
            if(!canonicalScalarSelectionTypeAccepted(*contract,actual.valueType))refuse(path,"This local scalar selection does not admit that data type.","migration.scalar_type_unavailable", Domain::FrontendCapability);
            if(!dataType.empty()&&dataType!=actual.valueType)refuse(path,"Scalar selection data require identical source types, without coercion.","migration.scalar_mixed_operands_unavailable", Domain::FrontendCapability);dataType=actual.valueType;
        }
        return contract->operationId;
    }
    std::string_view scalarIdentity(const Json &node,const std::vector<Json> &sourceEdges,const std::string &path) {
        const auto type=txt(node.member("type"),path+"/type");const auto legacy=canonicalScalarMigrationIdentity(type);
        const auto *contract=canonicalScalarNativeContract(legacy);if(!contract)return legacy;
        const auto id=txt(node.member("id"),path+"/id");
        std::vector<std::string> inputs;
        if((contract->domain==CanonicalScalarDomain::Boolean||contract->catalogId=="numeric.add")&&contract->inputCount==2) {
            std::map<std::size_t,std::string> ordered;
            for(const auto &edge:sourceEdges)if(txt(obj(edge.member("to"),path).member("node"),path)==id) {
                const auto port=txt(edge.member("to")->member("port"),path);const auto ordinal=canonicalBooleanInputOrdinal(port);
                if(!ordinal||!ordered.emplace(*ordinal,port).second)refuse(path,contract->catalogId=="numeric.add"?"Add operands require unique actual a,b,input_3..input_16 connectors.":"Boolean operands require unique actual a,b,input_3..input_16 connectors.",contract->catalogId=="numeric.add"?"migration.numeric_add_ports_invalid":"migration.boolean_ports_invalid", Domain::Semantics);
            }
            for(const auto &[ordinal,port]:ordered)inputs.push_back(port);
            if(!canonicalBooleanInputPortsValid(inputs)||canonicalScalarMigrationIdentityForArity(type,inputs.size()).empty())refuse(path,contract->catalogId=="numeric.add"?"Add reduction requires2..16 real operands in exact canonical order.":"Boolean reductions require2..16 real operands; Implies remains binary.",contract->catalogId=="numeric.add"?"migration.numeric_add_ports_invalid":"migration.boolean_ports_invalid", Domain::Semantics);
        }else for(std::size_t i=0;i<contract->inputCount;++i)inputs.push_back(std::string(i?contract->inputB:contract->inputA));
        const auto actualIdentity=canonicalScalarMigrationIdentityForArity(type,inputs.size());
        if(!resolvedTypes)return actualIdentity;
        const auto found=std::find_if(resolvedTypes->scalars.begin(),resolvedTypes->scalars.end(),[&](const auto &entry){return entry.nodeId==id;});
        if(found==resolvedTypes->scalars.end()||found->authoredOperation!=type||found->operands.size()!=inputs.size()||!consumedResolutions.insert(id).second)
            refuse(path,"The exact authored operation has no complete current binding resolution.","migration.scalar_resolution_incomplete", Domain::Semantics);
        const bool compare=contract->domain==CanonicalScalarDomain::ExactEquality||contract->domain==CanonicalScalarDomain::ExactOrder;
        std::string operandType;
        for(std::size_t i=0;i<inputs.size();++i) {
            const std::string port=compare?(i?"y":"x"):inputs[i];const auto &actual=found->operands[i];
            if(actual.inputPortId!=port||!actual.accepted||!actual.complete||actual.sourceNodeId.empty()||actual.sourcePortId.empty()||actual.valueType.empty())
                refuse(path,"Both actual source operands must have accepted, complete binding resolutions.","migration.scalar_resolution_incomplete", Domain::Semantics);
            const Json *driver=nullptr;
            for(const auto &edge:sourceEdges) {
                const auto &to=obj(edge.member("to"),path);
                if(txt(to.member("node"),path)==id&&txt(to.member("port"),path)==port) {
                    if(driver)refuse(path,"Resolved scalar inputs require one actual driver each.","migration.scalar_resolution_incomplete", Domain::Semantics);driver=&obj(edge.member("from"),path);
                }
            }
            if(!driver||txt(driver->member("node"),path)!=actual.sourceNodeId||txt(driver->member("port"),path)!=actual.sourcePortId)
                refuse(path,"Resolved operand provenance does not match the exact saved source edge.","migration.scalar_resolution_incomplete", Domain::Semantics);
            if(i&&operandType!=actual.valueType)refuse(path,"This compiled authored corridor requires identical actual operand types; no implicit numeric promotion is inserted.","migration.scalar_mixed_operands_unavailable", Domain::FrontendCapability);
            operandType=actual.valueType;
        }
        const auto selected=canonicalScalarMigrationIdentityForType(actualIdentity,operandType);
        if(selected.empty())refuse(path,"This exact resolved scalar type has no admitted authored implementation; unqualified widths, mixed operands and implicit conversions remain unavailable.","migration.scalar_type_unavailable", Domain::FrontendCapability);
        return selected;
    }
    static void terminal(const Json &terminals,const char *name,const char *type,const char *role,const char *body,bool outer,const std::string &path) {
        const auto at=path+"/structure_terminals/"+name;const auto &t=obj(terminals.member(name),at);
        for(const auto &[key,_]:t.object)if(key!="type"&&key!="role"&&key!="body_port"&&key!="outer_port"&&key!="outer_visible"&&key!="exposed_in_body"&&key!="read_only")refuse(at+"/"+key,"Unknown terminal contract field.", "migration.unsupported", Domain::Compatibility);
        if(txt(t.member("type"),at)!=type||txt(t.member("role"),at)!=role||txt(t.member("body_port"),at)!=body||
           flag(t.member("outer_visible"),at)!=outer||!flag(t.member("exposed_in_body"),at)||!flag(t.member("read_only"),at)||
           (outer?(txt(t.member("outer_port"),at)!=name):t.member("outer_port")!=nullptr))
            refuse(at,"The terminal differs from the admitted exact public scalar role.","migration.structure_terminal_unsupported", Domain::Semantics);
    }
    void identity(const std::string &kind,const std::string &id,const std::string &from,const std::string &to,bool generated=false){
        auto &ids=kind=="widget"?widgets:kind=="edge"?edges:nodes;
        if(!ids.insert(id).second)refuse(from,"Duplicate source identity.","migration.identity_duplicate", Domain::Integrity);
        all.insert(id);out.identities.push_back({kind,id,from,to,generated});
    }
    std::string fresh(std::string base){std::string id=base;for(unsigned n=2;all.contains(id);++n)id=base+":"+std::to_string(n);all.insert(id);return id;}
    void reserve(const Json &scope,const std::string &path="/diagram"){
        const auto &entries=arr(scope.member("nodes"),path+"/nodes");
        for(std::size_t i=0;i<entries.size();++i){
            const auto &n=entries[i];const auto at=path+"/nodes/"+std::to_string(i);
            const auto *kind=n.member("kind"),*type=n.member("type");
            if(kind&&kind->type==Json::Type::String&&kind->string=="primitive"&&type&&type->type==Json::Type::String) {
                std::string_view operation=type->string;
                if(operation.starts_with("frog.studio."))operation.remove_prefix(12);
                if(operation.starts_with("math.pde."))
                    refuse(at,"Partial Differential Equations requires typed PDEProblem/Array/callable transport and compiled services not admitted by the current LLVM target. The explicit Simulation operators remain separate; no fallback or compiled copy was started.","migration.pde_aggregate_capability_unavailable", Domain::FrontendCapability);
                if(operation.starts_with("math.ode.")||operation=="math.dae.radau_iia5"||operation=="math.dae.radau_iia5.reference")
                    refuse(at,"Ordinary Differential Equations "+type->string+" requires Array, cluster and, for Reference instances, typed callable transport not admitted by the current LLVM execution target. The authored document, wires and execution configuration are preserved; no compiled copy or fallback was started.","migration.ode_aggregate_capability_unavailable", Domain::FrontendCapability);
            }
            if(kind&&kind->type==Json::Type::String&&kind->string=="widget_value"&&n.member("widget")&&n.member("id")){
                const auto wid=txt(n.member("widget"),at),nodeId=txt(n.member("id"),at);bool connected=false,onlyLimit=true;
                for(const auto &edge:arr(scope.member("edges"),path+"/edges")){
                    const auto *from=edge.member("from"),*to=edge.member("to");if(!from||!from->member("node")||from->member("node")->string!=nodeId)continue;
                    connected=true;if(!from->member("port")||from->member("port")->string!="value"||!to||!to->member("node")||!to->member("port")||to->member("port")->string!="a"){onlyLimit=false;continue;}
                    const auto target=std::find_if(entries.begin(),entries.end(),[&](const auto &other){return other.member("id")&&other.member("id")->string==to->member("node")->string;});
                    if(target==entries.end()||!target->member("type")||!canonicalGammaInfiniteLimit(target->member("type")->string,"a"))onlyLimit=false;
                }
                if(connected&&onlyLimit)gammaLimitWidgets.insert(wid);else nonGammaLimitWidgets.insert(wid);
            }
            if(kind&&kind->type==Json::Type::String&&kind->string=="primitive"&&type&&type->type==Json::Type::String&&
                canonicalDiscreteMathIdentity(type->string)&&!canonicalDiscreteMathScalarIdentity(type->string))
                refuse(at,"Discrete Math "+type->string+" requires Array transport not provided by the current native execution target; local Highlight is available, without native fallback.","migration.discrete_array_capability_unavailable", Domain::FrontendCapability);
            if(kind&&kind->type==Json::Type::String&&kind->string=="primitive"&&type&&type->type==Json::Type::String&&canonicalBesselComplexIdentity(type->string))
                refuse(at,"Bessel Hankel/Kelvin requires complex f64 transport absent from this native target; the real complex local Highlight operator remains separate, with no native fallback.","migration.bessel_complex_capability_unavailable", Domain::FrontendCapability);
            all.insert(txt(n.member("id"),at+"/id"));
            if(const auto *rs=n.member("regions")){
                const auto &regions=arr(rs,at+"/regions");
                for(std::size_t r=0;r<regions.size();++r){const auto nested=at+"/regions/"+std::to_string(r)+"/diagram";reserve(obj(regions[r].member("diagram"),nested),nested);}
            }
        }
        for(const auto &e:arr(scope.member("edges"),path+"/edges"))all.insert(txt(e.member("id"),path+"/edges/id"));
    }
    Json panel(const Json &raw){
        Json result=object(),list=array();
        if(const auto *canvas=raw.member("canvas"))result.object["canvas"]=*canvas;
        const auto &entries=arr(raw.member("widgets"),"/frontPanel/widgets");
        if(entries.size()>1024)refuse("/frontPanel/widgets","Widget budget exceeded.", "migration.unsupported", Domain::Control);
        for(std::size_t i=0;i<entries.size();++i){
            const auto from="/frontPanel/widgets/"+std::to_string(i),to="/front_panel/widgets/"+std::to_string(i);
            const auto &authored=obj(&entries[i],from);std::optional<Json> normalized;
            if(const auto *legacyClass=authored.member("class_ref");legacyClass&&
                legacyClass->type==Json::Type::String&&legacyClass->string=="frog.widgets.numeric_control") {
                // This finite historical shape is interpreted by the portable
                // model codec, never by a second defaults/alias decoder here.
                for(const auto &[key,_]:authored.object)
                    if(key!="id"&&key!="role"&&key!="class_ref"&&key!="label"&&key!="layout")
                        refuse(from+"/"+key,"This legacy numeric shape has an unaccounted field; no value or configuration was discarded.","migration.legacy_widget_field_unavailable", Domain::Compatibility);
                const auto id=txt(authored.member("id"),from+"/id");
                if(txt(authored.member("role"),from+"/role")!="control")
                    refuse(from+"/role","The authored role conflicts with its numeric control class.","migration.legacy_widget_invalid", Domain::Semantics);
                const auto *label=authored.member("label");
                if(!label||label->type!=Json::Type::String)
                    refuse(from+"/label","The legacy numeric label must be text.","migration.legacy_widget_invalid", Domain::Semantics);
                const auto &rect=obj(authored.member("layout"),from+"/layout");layout(rect,from+"/layout");
                for(const auto &[key,_]:rect.object)if(key!="x"&&key!="y"&&key!="width"&&key!="height")
                    refuse(from+"/layout/"+key,"The legacy numeric frame has an unknown component.","migration.legacy_widget_field_unavailable", Domain::Compatibility);
                if(!resolvedTypes||resolvedTypes->widgets.size()>1024)
                    refuse(from,"The legacy widget requires its exact private-model codec witness.","migration.legacy_widget_resolution_required", Domain::Semantics);
                const CanonicalDraftWidgetResolution *witness=nullptr;
                for(const auto &candidate:resolvedTypes->widgets)if(candidate.widgetId==id) {
                    if(witness)refuse(from,"Duplicate model-codec widget witness.","migration.legacy_widget_resolution_invalid", Domain::Integrity);
                    witness=&candidate;
                }
                if(!witness||witness->currentWidgetJson.size()>8U*1024U*1024U)
                    refuse(from,"Missing or oversized model-codec widget witness.","migration.legacy_widget_resolution_invalid", Domain::Integrity);
                // Reuse the portable full-schema codec and its existing bound;
                // a physical f64 record cannot attest a different nominal type.
                if(witness->fullTypeJson.empty()||witness->fullTypeJson.size()>65'536)
                    refuse(from,"Missing or oversized model-codec widget schema.","migration.legacy_widget_resolution_invalid", Domain::Integrity);
                const auto schemaJson=source::parseJson(witness->fullTypeJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                const auto schema=schemaJson?canonicalObjectReferenceReadType(*schemaJson):std::nullopt;
                if(!schema||!canonicalObjectReferenceFullTypeEqual(*schema,binding::TypeRef::primitive("f64")))
                    refuse(from,"The model-codec widget schema is not exactly f64.","migration.legacy_widget_resolution_invalid", Domain::Integrity);
                normalized=source::parseJson(witness->currentWidgetJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                if(!normalized||normalized->type!=Json::Type::Object)
                    refuse(from,"The model-codec widget record is malformed.","migration.legacy_widget_resolution_invalid", Domain::Integrity);
                const auto &record=*normalized;
                if(txt(record.member("id"),from)!=id||txt(record.member("classId"),from)!=legacyClass->string||
                    txt(record.member("role"),from)!="control"||txt(record.member("valueType"),from)!="f64"||
                    !record.member("label")||!exact(*record.member("label"),*label))
                    refuse(from,"The model-codec widget changed identity, class, role, type or label.","migration.legacy_widget_resolution_invalid", Domain::Integrity);
                const auto &bounds=obj(record.member("bounds"),from+"/bounds");layout(bounds,from+"/bounds");
                for(const auto key:{"x","y","width","height"})
                    if(std::stod(bounds.member(key)->string)!=std::stod(rect.member(key)->string))
                        refuse(from+"/layout/"+key,"The model-codec frame differs from its actual source frame.","migration.legacy_widget_resolution_invalid", Domain::Integrity);
                const auto current=source::parseJson(witness->currentValueJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                if(!current||!typedScalar(*current,"f64")||!exact(value(record,"f64",from),*current))
                    refuse(from,"The model-codec state disagrees with its exact typed current value.","migration.legacy_widget_resolution_invalid", Domain::Integrity);
            }
            const auto &w=normalized?*normalized:authored;
            for(const auto &[key,_]:w.object)if(key!="id"&&key!="classId"&&key!="role"&&key!="valueType"&&key!="label"&&key!="bounds"&&key!="props"&&key!="runtime"&&key!="state"&&key!="typography"&&key!="geometry"&&key!="binding")
                refuse(from+"/"+key,"Unknown widget fields need an explicit migration contract.", "migration.unsupported", Domain::Compatibility);
            const auto id=txt(w.member("id"),from+"/id"),type=txt(w.member("valueType"),from+"/valueType");
            const auto klass=txt(w.member("classId"),from+"/classId"),role=txt(w.member("role"),from+"/role");
            const bool numeric=klass=="frog.widgets.numeric_control"||klass=="frog.widgets.numeric_indicator";
            const bool booleanClass=klass=="frog.widgets.boolean_control"||klass=="frog.widgets.boolean_indicator"||klass=="frog.widgets.button";
            const bool textIndicator=(klass=="frog.widgets.string_indicator"&&type=="string")||(klass=="frog.widgets.path_indicator"&&type=="path");
            if(klass.find("array")!=std::string::npos)refuse(from,"The first compiled Array corridor uses real scalar controls/constants and Numeric indicators; Array widgets are not admitted.","migration.array_widget_unavailable", Domain::FrontendCapability);
            if(klass.find("cluster")!=std::string::npos)refuse(from,"Immutable diagram Record literals do not admit Front Panel Cluster controls/indicators or synthesize replacements.","migration.record_widget_unavailable", Domain::FrontendCapability);
            if((!numeric&&!booleanClass&&!textIndicator)||(numeric&&type!="f64"&&type!="i32"&&type!="u32"&&type!="i64"&&type!="u64")||(booleanClass&&type!="bool"))
                refuse(from,"Only exact Numeric/Boolean values and real String/Path indicators belong to this migration corridor.", "migration.unsupported",
                    booleanClass||klass=="frog.widgets.string_indicator"||klass=="frog.widgets.path_indicator"?Domain::Semantics:
                    numeric||klass=="frog.widgets.string_control"||klass=="frog.widgets.path_control"?Domain::FrontendCapability:Domain::Compatibility);
            if((role!="control"&&role!="indicator")||
                (klass.ends_with("_indicator")&&role!="indicator")||(klass.ends_with("_control")&&role!="control"))refuse(from,"Widget role and declared class conflict.", "migration.unsupported", Domain::Semantics);
            if(w.member("binding"))refuse(from+"/binding","Public interface bindings require a separately validated migration.", "migration.unsupported", Domain::FrontendCapability);
            const bool gammaLimit=role=="control"&&gammaLimitWidgets.contains(id)&&!nonGammaLimitWidgets.contains(id);
            const auto current=value(w,type,from,gammaLimit,exactIntegerState);auto initial=current;
            Json props=object();
            if(const auto *p=w.member("props")){
                obj(p,from+"/props");for(const auto &[key,v]:p->object){
                    if(key=="data.default"){
                        if(v.type!=Json::Type::String)refuse(from+"/props/data.default","Stored default must retain its widget record.", "migration.unsupported", Domain::Integrity);
                        auto saved=source::parseJson(v.string,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                        const Json *stored=saved?&*saved:nullptr;bool storedExact=exactIntegerState;
                        if(stored&&stored->member("format")) {
                            for(const auto &[key,_]:stored->object)if(key!="format"&&key!="draft_revision"&&key!="widget")
                                refuse(from+"/props/data.default/"+key,"Unknown stored-value envelope field.","migration.default_state_unsupported", Domain::Compatibility);
                            const auto *revision=stored->member("draft_revision");
                            if(txt(stored->member("format"),from)!="frog.widget.value.draft"||!revision||revision->type!=Json::Type::Number||revision->string!="4")
                                refuse(from+"/props/data.default","Only the explicit revision-4 exact integer stored-value envelope is admitted.","migration.default_state_unsupported", Domain::Compatibility);
                            stored=&obj(stored->member("widget"),from+"/props/data.default/widget");storedExact=true;
                        }
                        if(!stored||txt(stored->member("id"),from)!=id||txt(stored->member("classId"),from)!=klass||txt(stored->member("valueType"),from)!=type)
                            refuse(from+"/props/data.default","Stored default has a different widget identity or type.", "migration.unsupported", Domain::Semantics);
                        initial=value(*stored,type,from+"/props/data.default",gammaLimit,storedExact);
                    }else if(textIndicator&&key=="text.value") {
                        // Its exact writer value has already been migrated to
                        // default_value; this private property is not public.
                    }else if(klass=="frog.widgets.string_indicator"&&key=="text.horizontal_alignment") {
                        if(v.type!=Json::Type::String || (v.string!="left"&&v.string!="center"&&v.string!="right"))
                            refuse(from+"/props/"+key,"String indicator alignment must be left, center or right.","migration.record_indicator_state_invalid", Domain::Semantics);
                        // This private presentation remains in the exact
                        // preserved_authoring_source, like the text layout
                        // flags below. It is not a headless value contract.
                    }else if(klass=="frog.widgets.path_indicator"&&type=="path"&&role=="indicator"&&(key=="folder.visible"||key=="button.visible")) {
                        // The real Path writer emits these presentation flags
                        // after Create Indicator. Keep the actual view/source;
                        // they are not executable public widget properties.
                        if(v.type!=Json::Type::Boolean)refuse(from+"/props/"+key,"Path indicator visibility metadata must remain Boolean.","migration.record_indicator_state_invalid", Domain::Semantics);
                    }else if(textIndicator&&(key=="layout.size_to_text"||key=="interaction.text.single_line"||key=="interaction.text.wrap_at_word_breaks"||key=="interaction.text.update_while_typing")) {
                        flag(&v,from+"/props/"+key); // Original UI metadata remains recoverable.
                    }else if(textIndicator&&(key=="interaction.path.prompt"||key=="interaction.path.pattern_label"||key=="interaction.path.pattern"||key=="interaction.path.selection_mode"||key=="interaction.path.existence_mode"||key=="interaction.path.start_path")) {
                        if(v.type!=Json::Type::String)refuse(from+"/props/"+key,"Path presentation metadata must remain text.","migration.record_indicator_state_invalid", Domain::Semantics);
                    }else if(key=="caption.independent"||key=="caption.visible") {
                        flag(&v,from+"/props/"+key);
                    }else if(key=="caption.text") {
                        if(v.type!=Json::Type::String)refuse(from+"/props/"+key,"Caption text must remain text, including an explicitly empty caption.", "migration.unsupported", Domain::Integrity);
                    }else if(key!="layout.aura_band_px"&&key!="label.visible"){
                        if(key.starts_with("behavior.")&&key!="behavior.mechanical_action")refuse(from+"/props/"+key,"Unknown widget behavior is not silently discarded.", "migration.unsupported", Domain::Compatibility);
                        props.object[key]=v;
                    }
                }
            }
            if(const auto *mechanical=props.member("behavior.mechanical_action");mechanical &&
                ((klass!="frog.widgets.button"&&klass!="frog.widgets.boolean_control"&&klass!="frog.widgets.boolean_indicator") ||
                 mechanical->type!=Json::Type::String || !isSupportedButtonMechanicalAction(mechanical->string)))
                refuse(from+"/props/behavior.mechanical_action","Mechanical action must name one of the six supported Boolean/Button actions; indicators retain it as inactive metadata.", "migration.unsupported", Domain::Compatibility);
            props.object["default_value"]=initial;
            Json converted=object();converted.object={{"id",string(id)},{"role",string(role)},{"class_ref",string(klass)},
                {"value_type",string(type)},{"props",std::move(props)}};
            const auto &bounds=obj(w.member("bounds"),from+"/bounds");layout(bounds,from+"/bounds");converted.object["layout"]=bounds;
            Json parts=object();
            if(const auto *label=w.member("label")){
                if(label->type!=Json::Type::String)refuse(from+"/label","Label must be text.", "migration.unsupported", Domain::Integrity);
                Json part=object(),partProps=object();partProps.object["text"]=*label;
                bool visible=true;if(const auto *p=w.member("props"))if(const auto *v=p->member("label.visible"))visible=flag(v,from+"/props/label.visible");
                partProps.object["visible"]=boolean(visible);part.object={{"class",string("frog.ui.standard.label_part")},{"props",std::move(partProps)}};
                parts.object["label"]=std::move(part);
            }
            if(const auto *privateProps=w.member("props")) {
                const auto *independent=privateProps->member("caption.independent");
                const bool independentCaption=independent&&flag(independent,from+"/props/caption.independent");
                const auto *caption=privateProps->member("caption.text"),*visible=privateProps->member("caption.visible");
                // The existing public text part represents presentation. The
                // private marker is never an executable widget property.
                if(independentCaption||caption||visible) {
                    const auto name=independentCaption?"caption":"label";
                    if(!parts.member(name)) {
                        Json part=object();part.object={{"class",string("frog.ui.standard.label_part")},{"props",object()}};
                        parts.object[name]=std::move(part);
                    }
                    auto &partProps=parts.object[name].object["props"];
                    if(caption)partProps.object["text"]=*caption;
                    else if(independentCaption)partProps.object["text"]=string("");
                    if(visible)partProps.object["visible"]=*visible;
                    else if(independentCaption)partProps.object["visible"]=boolean(false);
                }
            }
            if(!parts.object.empty())converted.object["parts"]=std::move(parts);
            identity("widget",id,from,to);widgetTypes.emplace(id,type);
            if(role=="control")out.controls.push_back({id,type,encode(current)});
            list.array.push_back(std::move(converted));
        }
        result.object["widgets"]=std::move(list);return result;
    }
    Json scope(const Json &raw,const std::string &path,const std::string &publicPath={}){
        const auto &targetPath=publicPath.empty()?path:publicPath;
        obj(&raw,path);empty(raw.member("wire_fragments"),path+"/wire_fragments");
        Json result=object(),nodeList=array(),edgeList=array();std::set<std::string> comparisonNodes;
        const auto &ns=arr(raw.member("nodes"),path+"/nodes");
        if(nodes.size()+ns.size()>1024)refuse(path,"Node budget exceeded.", "migration.unsupported", Domain::Control);
        const auto &sourceEdges=arr(raw.member("edges"),path+"/edges");
        if(edges.size()+sourceEdges.size()>4096)refuse(path,"Edge budget exceeded.", "migration.unsupported", Domain::Control);
        AuthoredRecords records(ns,sourceEdges,path);
        AuthoredObjectReferences objects{records,widgetTypes};
        std::set<std::string> unbundleNodes;
        std::map<std::string,std::string> stringNodes;
        for(std::size_t i=0;i<ns.size();++i){
            const auto at=path+"/nodes/"+std::to_string(i);const auto &n=obj(&ns[i],at);
            const auto id=txt(n.member("id"),at+"/id"),kind=txt(n.member("kind"),at+"/kind");
            if(const auto *alignment=n.member("structure_subdiagram_label_alignment")) {
                if(kind!="structure" || alignment->type!=Json::Type::String ||
                    (alignment->string!="left" && alignment->string!="center" && alignment->string!="right"))
                    refuse(at+"/structure_subdiagram_label_alignment",
                        "Subdiagram label alignment requires a structure owner and left, center or right.","migration.presentation_invalid", Domain::Semantics);
                // The exact authored value remains in recoverability provenance;
                // it does not create an executable node, port or dependency.
            }
            if(kind=="comment") {
                draft_presentation::comment(n,raw,at,[](auto path,auto message){
                    refuse(std::move(path),std::move(message),"migration.presentation_invalid", Domain::Compatibility);
                });
                identity("presentation",id,at,std::string(draft_presentation::provenance)+at);
                continue;
            }
            const bool nativeCall=kind=="primitive"&&n.member("type")&&n.member("type")->type==Json::Type::String&&n.member("type")->string==kNativeCallDiagramNodeType;
            const auto literalType=scalarLiteralType(n);const bool record=recordLiteral(n),unbundle=authoredUnbundle(n),owned=records.owned.contains(id);
            if(const auto *automatic=n.member("constant_text_bounds_automatic")) {
                if(literalType.empty()&&!record&&authoredTextLiteral(n).empty())
                    refuse(at+"/constant_text_bounds_automatic","Automatic constant text bounds require an actual literal owner.","migration.presentation_invalid", Domain::Semantics);
                if(automatic->type!=Json::Type::Boolean)
                    refuse(at+"/constant_text_bounds_automatic","Automatic constant text bounds must be Boolean.","migration.presentation_invalid", Domain::Semantics);
            }
            bool comparisonElements=false;
            if(const auto *mode=n.member("comparison_aggregates")) {
                const auto *authored=n.member("type");
                const auto *entry=authored&&authored->type==Json::Type::String?
                    findFunctionCatalogEntryByDiagramNodeType(authored->string):nullptr;
                comparisonElements=kind=="primitive"&&entry&&comparisonHasMode(entry->catalogId)&&
                    mode->type==Json::Type::Boolean&&!mode->boolean;
                if(!comparisonElements)refuse(at+"/comparison_aggregates",
                    "The exact scalar profile can retain only its actual Compare Elements=false configuration.","migration.comparison_mode_invalid", Domain::Semantics);
            }
            const bool localObject=objects.handles(id);
            const auto target=targetPath+"/nodes/"+std::to_string(nodeList.array.size());
            Json item=object();item.object={{"id",string(id)},{"kind",string(kind)}};
            if(const auto *rect=n.member("layout")){layout(*rect,at+"/layout");item.object["layout"]=*rect;}
            for(const auto &[key,v]:n.object)if((key.starts_with("studio_")&&!(nativeCall&&nativeCallDraft7&&key=="studio_native_call")&&!(objectDraft&&key=="studio_application_control"))||key=="state"||key=="initial_value")
                refuse(at+"/"+key,"Configured operations, state and literal ownership need their own semantic migration.", "migration.unsupported", Domain::Compatibility);
            for(const auto &[key,_]:n.object)if(key!="id"&&key!="kind"&&key!="type"&&key!="widget"&&key!="value_type"&&key!="layout"&&key!="text"&&
                key!="structure_type"&&key!="boundary"&&key!="structure_terminals"&&key!="structure_tunnels"&&key!="regions"&&key!="palette_catalog_id"&&
                !(nativeCall&&nativeCallDraft7&&key=="studio_native_call")&&!(objectDraft&&key=="studio_application_control")&&
                !(key=="comparison_aggregates"&&comparisonElements)&&key!="hidden"&&key!="locked"&&key!="group"&&key!="font"&&key!="display_format"&&key!="constant_text_bounds_automatic"&&key!="binding_profile"&&key!="case_profile"&&
                key!="structure_auto_grow"&&key!="structure_subdiagram_label_border_rgb"&&
                !(owned&&(key=="constant_owner_node_id"||key=="constant_field_id"||key=="constant_field_order"))&&
                !((owned||authoredTextLiteral(n)=="string")&&!authoredTextLiteral(n).empty()&&key=="string_constant")&&
                !(canonicalStringMigrationIdentity(n.member("type")&&n.member("type")->type==Json::Type::String?n.member("type")->string:"").size()&&key=="string_function_terminals")&&
                !(canonicalArrayMigrationIdentity(n.member("type")&&n.member("type")->type==Json::Type::String?n.member("type")->string:"").size()&&
                    (key=="array_function_terminals"||key=="build_array_inputs"||key=="build_array_concatenate"||key=="index_array_outputs"))&&
                !(owned&&authoredTextLiteral(n)=="path"&&key=="path_constant")&&
                !(record&&(key=="cluster_constant_collapsed"||key=="cluster_constant_autosizing"||key=="cluster_expanded_width"||key=="cluster_expanded_height"))&&
                !((unbundle||!recordFunction(n).empty())&&(key=="cluster_function_terminals"||key=="cluster_field_names"||key=="cluster_field_id_paths"||key=="cluster_show_full_names"))&&!key.starts_with("label")&&!key.starts_with("while_loop_")&&!addedStructurePresentation(key))
                refuse(at+"/"+key,"Unknown node fields are not silently erased during semantic migration.", "migration.unsupported", Domain::Compatibility);
            // Owned literal nodes are data components, never copied as executable nodes.
            // Their actual IDs are attributed into the one immutable Record value below.
            if(authoredObjectNode(n)&&!objectDraft)refuse(at,"Application Control needs its identified revision6/7 writer.","migration.object_reference.revision_unavailable", Domain::Compatibility);
            if(owned)continue;
            identity("node",id,at,target);
            if(localObject) {
                auto descriptor=objects.describe(id);descriptor.configurationSourcePointer=target+"/"+std::string(canonicalObjectReferenceAttribute)+"/configuration";hasObjectReferences=true;
                item.object[std::string(canonicalObjectReferenceAttribute)]=canonicalObjectReferenceAttributes(descriptor);
                if(record){const auto &literal=records.literals.at(id);item.object["kind"]=string("constant");item.object["type"]=string(literal.type.canonical());item.object["constant_profile"]=string(std::string(canonicalObjectReferenceProfile));item.object["value"]=literal.value;}
                else item.object["type"]=string(std::string(canonicalObjectReferenceOperation(descriptor.kind)));
                out.identities.push_back({"object_reference_configuration",id,at,target+"/"+std::string(canonicalObjectReferenceAttribute),false});
                if(record){const auto components=[&](const auto &self,const std::string &owner,const std::string &valuePath)->void{
                    for(const auto child:records.literals.at(owner).fields){const auto &field=ns[child];const auto childId=txt(field.member("id"),records.where(child)),fieldId=txt(field.member("constant_field_id"),records.where(child));const auto cp=valuePath+"/"+pointerToken(fieldId);identity("literal_component",childId,records.where(child),cp);if(recordLiteral(field))self(self,childId,cp);}};components(components,id,target+"/value");}
            }else if(nativeCall) {
                if(!nativeCallDraft7)refuse(at,"Configured NativeCall requires its explicitly identified revision-7 writer.","migration.native_call_revision_unavailable", Domain::Compatibility);
                std::string preflightError;
                const auto &rawConfiguration=obj(n.member("studio_native_call"),at+"/studio_native_call");
                if(!nativeCallScalarConfigurationPreflight(rawConfiguration,&preflightError))refuse(at+"/studio_native_call",preflightError,"migration.native_call_configuration_invalid", Domain::Semantics);
                CanonicalNativeCallDiagnostic diagnostic;
                const auto descriptor=canonicalNativeCallScalarDescriptor(obj(n.member("studio_native_call"),at+"/studio_native_call"),target+"/native_call_configuration",&diagnostic);
                if(!descriptor)refuse(at+"/studio_native_call",diagnostic.message,diagnostic.unsupported?"migration.native_call_signature_unavailable":"migration.native_call_configuration_invalid", diagnostic.unsupported?Domain::FrontendCapability:Domain::Semantics);
                item.object["type"]=string(std::string(kNativeCallScalarOperationId));
                item.object["native_call_profile"]=string(std::string(kNativeCallScalarProfile));
                item.object["native_call_configuration"]=*source::parseJson(descriptor->configurationJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
                out.identities.push_back({"native_call_configuration",id,at+"/studio_native_call",target+"/native_call_configuration",false});
                for(const auto &port:descriptor->ports)out.identities.push_back({"native_call_port",id+":"+port.id,at+"/studio_native_call",target+"/native_call_configuration",false});
                hasNativeCall=true;
            }else if(record) {
                const auto &literal=records.literals.at(id);item.object["kind"]=string("constant");item.object["type"]=string(literal.type.canonical());
                item.object["constant_profile"]=string(std::string(canonicalImmutableRecordProfile));item.object["value"]=literal.value;
                const auto components=[&](const auto &self,const std::string &owner,const std::string &valuePath)->void {
                    for(const auto child:records.literals.at(owner).fields) {
                        const auto &field=ns[child];const auto childId=txt(field.member("id"),records.where(child));const auto fieldId=txt(field.member("constant_field_id"),records.where(child));
                        const auto componentPath=valuePath+"/"+pointerToken(fieldId);identity("literal_component",childId,records.where(child),componentPath);
                        if(recordLiteral(field))self(self,childId,componentPath);
                    }
                };components(components,id,target+"/value");
            }else if(unbundle) {
                std::set<std::string> visiting{id};const auto schema=records.cluster(n,at,visiting);const auto count=records.rows(n,at);Json selected=array();
                for(unsigned row=0;row<count;++row) {
                    const auto field=records.selected(n,row,at);
                    if(std::find(schema.fieldIds.begin(),schema.fieldIds.end(),field)==schema.fieldIds.end())refuse(at+"/cluster_field_id_paths/"+std::to_string(row),"Selected stable field no longer exists.","migration.unbundle_field_missing", Domain::Semantics);
                    Json entry=object();entry.object={{"port",string("element"+std::to_string(row+1))},{"field_id",string(field)}};selected.array.push_back(std::move(entry));
                    out.identities.push_back({"selected_field",id+":element"+std::to_string(row+1),at+"/cluster_field_id_paths/"+std::to_string(row)+"/0",target+"/selected_fields/"+std::to_string(row)+"/field_id",false});
                }
                item.object["type"]=string("frog.core.unbundle.by_name");item.object["selected_fields_version"]=*source::parseJson("1",nullptr,source::JsonNumberPolicy::PreserveLexeme);item.object["selected_fields"]=std::move(selected);unbundleNodes.insert(id);
            }else if(authoredTextLiteral(n)=="string") {
                literalTextPresentation(n,"string",at);
                const auto *rawText=n.member("text");
                if(rawText&&rawText->type!=Json::Type::String)refuse(at+"/text","A real String literal must retain JSON String data.","migration.string_literal_invalid", Domain::Semantics);
                const auto fixed=authoredFixedStringConstant(n);
                if(fixed&&rawText&&rawText->string!=*fixed)refuse(at+"/text","Fixed String literal text contradicts its actual catalog.","migration.fixed_string_value_invalid", Domain::Semantics);
                const std::string_view actualText=fixed?*fixed:rawText?std::string_view(rawText->string):std::string_view{};
                if(fixed)if(const auto *hint=n.member("value_type");hint&&(hint->type!=Json::Type::String||hint->string!="string"))refuse(at+"/value_type","Fixed String literal type contradicts its actual catalog.","migration.fixed_string_type_invalid", Domain::Semantics);
                if(actualText.size()>canonicalStringMaximumBytes)refuse(at+"/text","The String literal exceeds the local 1 000 000-byte capability.","migration.string_resource_limit", Domain::Control);
                if(!canonicalStringUtf8Valid(actualText))refuse(at+"/text","String2 requires exact valid UTF-8 including NUL, without repair or normalization.","migration.string_literal_encoding", Domain::Semantics);
                const auto literal=string(std::string(actualText));
                if(const auto *hint=n.member("value_type");hint&&txt(hint,at+"/value_type")!="string")refuse(at+"/value_type","String literal type differs from its actual catalog.","migration.string_literal_invalid", Domain::Semantics);
                item.object["kind"]=string("constant");item.object["type"]=string("string");
                item.object["constant_profile"]=string(std::string(canonicalStringConstantProfile));item.object["value"]=literal;
                out.identities.push_back({"literal_value",id+":value",rawText?at+"/text":at+"/type",target+"/value",false});hasStringBytes=true;
            }else if(!literalType.empty()) {
                if(literalType=="bool")if(const auto *valueType=n.member("value_type");valueType&&txt(valueType,at+"/value_type")!="bool")refuse(at,"Boolean literal type conflicts with its actual catalog.","migration.constant_type_unsupported", Domain::Semantics);
                item.object["kind"]=string("constant");item.object["type"]=string(std::string(literalType));
                item.object["value"]=literalValue(n,literalType,at);item.object["constant_profile"]=string("frog.studio.scalar-constant@1");
                out.identities.push_back({"literal_value",id+":value",n.member("text")?at+"/text":at+"/type",target+"/value",false});
            }else if(kind=="widget_value"){
                const auto wid=txt(n.member("widget"),at+"/widget");if(!widgetTypes.contains(wid))refuse(at,"Widget terminal has no admitted declaration.", "migration.unsupported", Domain::Semantics);
                if(const auto *t=n.member("type");t&&txt(t,at)!=widgetTypes.at(wid))refuse(at,"Widget terminal type conflicts with its declaration.", "migration.unsupported", Domain::Semantics);
                item.object["widget"]=string(wid);
            }else if(kind=="primitive"&&!canonicalArrayMigrationIdentity(txt(n.member("type"),at+"/type")).empty()) {
                const auto operation=canonicalArrayMigrationIdentity(txt(n.member("type"),at+"/type"));
                const auto &contract=*canonicalArrayNativeContract(operation);
                const auto countField=[&](std::string_view key){const auto *v=n.member(key);if(!v||v->type!=Json::Type::Number||!typedScalar(*v,"u32"))refuse(at+"/"+std::string(key),"Array row count must be an exact unsigned integer.","migration.array_configuration_invalid", Domain::Semantics);const auto count=unsignedField(v,at+"/"+std::string(key),(std::numeric_limits<unsigned>::max)());if(!count||count>32)refuse(at+"/"+std::string(key),"Array row count must remain1..32.","migration.array_configuration_invalid", Domain::Semantics);return count;};
                if(n.member("array_function_terminals"))static_cast<void>(countField("array_function_terminals"));
                std::vector<std::string> ports;
                if(contract.operation==CanonicalArrayOperation::Build) {
                    const auto count=countField("build_array_inputs");
                    if(!count)refuse(at+"/build_array_inputs","Build requires its actual1..32 persisted inputs.","migration.array_configuration_invalid", Domain::Semantics);
                    const auto *mode=n.member("build_array_concatenate");if(!mode||mode->type!=Json::Type::Boolean)refuse(at+"/build_array_concatenate","The real Build mode must remain Boolean.","migration.array_configuration_invalid", Domain::Semantics);
                    if(mode->boolean)refuse(at+"/build_array_concatenate","Array concatenation/lifting is outside the scalar Build real1d capability.","migration.array_mode_unavailable", Domain::FrontendCapability);
                    if(n.member("index_array_outputs"))refuse(at,"Build cannot acquire Index configuration.","migration.array_configuration_invalid", Domain::Semantics);
                    for(std::size_t p=0;p<count;++p)ports.push_back(canonicalArrayBuildPort(p));
                } else {
                    if(n.member("build_array_inputs")||n.member("build_array_concatenate"))refuse(at,"Only Build owns its arity/mode fields.","migration.array_configuration_invalid", Domain::Semantics);
                    if(contract.operation==CanonicalArrayOperation::Index) {
                        const auto count=countField("index_array_outputs");
                        if(!count)refuse(at+"/index_array_outputs","Index output groups must remain1..32.","migration.array_configuration_invalid", Domain::Semantics);
                        if(count!=1)refuse(at+"/index_array_outputs","The first native Index corridor admits one real output group.","migration.array_mode_unavailable", Domain::FrontendCapability);
                        ports={"array","index"};
                    } else {if(n.member("index_array_outputs"))refuse(at,"Size cannot acquire Index configuration.","migration.array_configuration_invalid", Domain::Semantics);ports={"array"};}
                }
                std::map<std::string,unsigned> actualDrivers;
                for(const auto &edge:sourceEdges) {
                    const auto &sink=obj(edge.member("to"),at);if(txt(sink.member("node"),at)!=id)continue;
                    const auto port=txt(sink.member("port"),at);
                    if(contract.operation==CanonicalArrayOperation::Build&&port=="input_2")refuse(at,"The saved legacy input_2 alias requires a separately attributed canonicalization; this first corridor keeps actual array_b identities.","migration.array_legacy_port_unavailable", Domain::Compatibility);
                    if(std::find(ports.begin(),ports.end(),port)==ports.end())refuse(at,"An Array input does not name an active real catalog endpoint.","migration.array_port_invalid", Domain::Semantics);
                    if(++actualDrivers[port]!=1)refuse(at,"An Array input has multiple real drivers.","migration.array_driver_invalid", Domain::Semantics);
                }
                for(const auto &port:ports)if(!actualDrivers.contains(port)) {
                    if(contract.operation==CanonicalArrayOperation::Index&&port=="index")refuse(at,"Native Index requires the actual i32 index driver; its authored optional default is not synthesized.","migration.array_index_default_unavailable", Domain::FrontendCapability);
                    refuse(at,"A required Array input has no actual driver.","migration.array_driver_invalid", Domain::Semantics);
                }
                item.object["type"]=string(std::string(operation));item.object["array_execution_profile"]=string(std::string(canonicalArrayReal1dProfile));
                if(contract.operation==CanonicalArrayOperation::Build) {
                    auto inputs=array();for(std::size_t p=0;p<ports.size();++p){inputs.array.push_back(string(ports[p]));
                        const auto driver=std::find_if(sourceEdges.begin(),sourceEdges.end(),[&](const auto &edge){const auto *to=edge.member("to");return to&&to->member("node")&&to->member("node")->string==id&&to->member("port")&&to->member("port")->string==ports[p];});
                        if(driver==sourceEdges.end())refuse(at,"The actual Build port lost its source edge.","migration.array_driver_invalid", Domain::Semantics);
                        out.identities.push_back({"array_input",id+":"+ports[p],path+"/edges/"+std::to_string(std::size_t(driver-sourceEdges.begin()))+"/to/port",target+"/input_ports/"+std::to_string(p),false});}
                    item.object["input_ports"]=std::move(inputs);
                    out.identities.push_back({"array_mode",id,at+"/build_array_concatenate",target+"/array_execution_profile",false});
                } else out.identities.push_back({"array_profile",id,at+"/type",target+"/array_execution_profile",false});
                hasArrayReal1d=true;
            }else if(kind=="primitive"&&!canonicalStringMigrationIdentity(txt(n.member("type"),at+"/type")).empty()) {
                const auto operation=canonicalStringMigrationIdentity(txt(n.member("type"),at+"/type"));
                const auto &contract=*canonicalStringNativeContract(operation);
                if(const auto *count=n.member("string_function_terminals");count&&unsignedField(count,at+"/string_function_terminals",32)==0)
                    refuse(at+"/string_function_terminals","The actual writer's presentation count is from one to 32.","migration.string_port_invalid", Domain::Semantics);
                std::map<std::size_t,std::string> ordered;
                std::map<std::string,unsigned> driverCounts;
                for(const auto &edge:sourceEdges) {
                    const auto &sink=obj(edge.member("to"),at);if(txt(sink.member("node"),at)!=id)continue;
                    auto port=txt(sink.member("port"),at);
                    if(contract.operation==CanonicalStringOperation::LengthBytes&&port=="string")port="value";
                    const auto ordinal=contract.operation==CanonicalStringOperation::LengthBytes?
                        (port=="value"?std::optional<std::size_t>{0}:std::nullopt):canonicalStringConcatPortOrdinal(port);
                    if(!ordinal)refuse(at,"String2 input is not an actual admitted catalog endpoint.","migration.string_port_invalid", Domain::Semantics);
                    if(++driverCounts[port]!=1)refuse(at,"A String input has multiple actual drivers.","migration.string_driver_invalid", Domain::Semantics);
                    ordered.emplace(*ordinal,port);
                    const auto &source=obj(edge.member("from"),at);const auto sourceId=txt(source.member("node"),at),sourcePort=txt(source.member("port"),at);
                    const auto found=records.byId.find(sourceId);
                    if(found!=records.byId.end()&&(recordLiteral(ns[found->second])||authoredUnbundle(ns[found->second]))) {
                        std::set<std::string> visiting;const auto actual=records.output(sourceId,sourcePort,visiting);
                        if(actual.kind==binding::TypeKind::Cluster||actual.kind==binding::TypeKind::Array)
                            refuse(at,"The first String2 capability admits scalar String inputs only, without Array/Cluster lifting.","migration.string_type_unavailable", Domain::FrontendCapability);
                    }
                }
                if(!driverCounts.contains(contract.operation==CanonicalStringOperation::LengthBytes?"value":"a"))
                    refuse(at,"The required String input has no actual driver.","migration.string_driver_invalid", Domain::Semantics);
                if(contract.operation==CanonicalStringOperation::ConcatenateBytes&&!driverCounts.contains("b"))
                    refuse(at,"This limited String2 capability requires a real b driver; no empty optional value is invented.","migration.string_optional_default_unavailable", Domain::FrontendCapability);
                std::vector<std::string> ports;for(const auto &[ordinal,port]:ordered)ports.push_back(port);
                if(!canonicalStringInputPortsValid(contract,ports))refuse(at,"The actual ordered String inputs violate their local contract.","migration.string_port_invalid", Domain::Semantics);
                item.object["type"]=string(std::string(operation));
                if(contract.operation==CanonicalStringOperation::ConcatenateBytes) {
                    auto inputs=array();for(std::size_t p=0;p<ports.size();++p) {
                        inputs.array.push_back(string(ports[p]));
                        const auto driver=std::find_if(sourceEdges.begin(),sourceEdges.end(),[&](const auto &edge){const auto *to=edge.member("to");return to&&to->member("node")&&to->member("node")->string==id&&to->member("port")&&to->member("port")->string==ports[p];});
                        if(driver==sourceEdges.end())refuse(at,"The actual Concat port lost its exact source edge.","migration.string_driver_invalid", Domain::Semantics);
                        out.identities.push_back({"string_input",id+":"+ports[p],path+"/edges/"+std::to_string(std::size_t(driver-sourceEdges.begin()))+"/to/port",target+"/input_ports/"+std::to_string(p),false});
                    }item.object["input_ports"]=std::move(inputs);
                }
                stringNodes.emplace(id,operation);hasStringBytes=true;
            }else if(kind=="primitive"){
                const auto type=txt(n.member("type"),at+"/type");
                const auto selection=this->selectionIdentity(n,sourceEdges,at);
                const auto scalarIdentity=selection.empty()?this->scalarIdentity(n,sourceEdges,at):selection;
                if(const auto *scalar=canonicalScalarNativeContract(scalarIdentity)) {
                    if(scalarIdentity.starts_with("frog.studio.boolean."))hasBooleanReduction=true;
                    if(scalar->inputCount==1&&scalar->domain!=CanonicalScalarDomain::Boolean&&scalar->domain!=CanonicalScalarDomain::ZeroComparison)hasUnaryProfile=true;
                    if(scalar->domain==CanonicalScalarDomain::ZeroComparison)hasComparisonZero=true;
                    if(scalar->domain==CanonicalScalarDomain::ExactEquality||scalar->domain==CanonicalScalarDomain::ExactOrder)comparisonNodes.insert(id);
                    if(scalar->policy==CanonicalScalarPolicy::StudioWrap32V1)hasScalar32=true;
                    else if(scalar->domain==CanonicalScalarDomain::ExistingAdd||scalar->domain==CanonicalScalarDomain::FiniteF64Arithmetic)hasScalarArithmetic=true;
                }
                if(scalarIdentity.empty()&&!isContextPathFunction(type)&&!canonicalHyperbolicIdentity(type)&&!canonicalGatingIdentity(type)&&!canonicalDiscreteMathScalarIdentity(type)&&!canonicalTrigonometricIdentity(type)&&!canonicalBesselIdentity(type)&&!canonicalGammaIdentity(type)&&!canonicalHypergeometricIdentity(type)&&!canonicalEllipticIdentity(type)&&!canonicalExponentialIntegralsIdentity(type)&&!canonicalErrorFunctionsIdentity(type)&&!canonicalEllipticParabolicIdentity(type)&&!canonicalExponentialIdentity(type))refuse(at,"Primitive has no admitted public migration in this corridor.", "migration.unsupported", operationDomain(type));
                item.object["type"]=string(isContextPathFunction(type)?(type.starts_with("frog.studio.")?type:"frog.studio."+type):std::string(scalarIdentity.empty()?std::string_view(type):scalarIdentity));
                if(const auto *contract=canonicalScalarNativeContract(scalarIdentity);contract&&contract->maximumInputCount) {
                    std::map<std::size_t,std::pair<std::string,std::size_t>> ordered;
                    for(std::size_t e=0;e<sourceEdges.size();++e){const auto &to=obj(sourceEdges[e].member("to"),at);if(txt(to.member("node"),at)==id){const auto port=txt(to.member("port"),at);ordered.emplace(*canonicalBooleanInputOrdinal(port),std::pair{port,e});}}
                    auto ports=array();for(const auto &[ordinal,input]:ordered){const auto position=ports.array.size();ports.array.push_back(string(input.first));out.identities.push_back({contract->catalogId=="numeric.add"?"numeric_add_input":"boolean_input",id+":"+input.first,path+"/edges/"+std::to_string(input.second)+"/to/port",target+"/input_ports/"+std::to_string(position),false});}
                    item.object["input_ports"]=std::move(ports);
                }
            }else if(kind=="structure"){
                const auto structureType=txt(n.member("structure_type"),at);
                if(structureType!="case"&&n.member("case_profile"))refuse(at+"/case_profile","A Case editing profile belongs only to Case.","migration.case_profile_unsupported", Domain::Semantics);
                if(structureType=="for_loop"||structureType=="case") {
                    const auto &boundary=obj(n.member("boundary"),at+"/boundary");
                    for(const auto &[key,_]:boundary.object)if(key!="inputs"&&key!="outputs")refuse(at+"/boundary/"+key,"Unsupported structure-boundary field.", "migration.unsupported", Domain::Compatibility);
                    empty(boundary.member("inputs"),at+"/boundary/inputs");empty(boundary.member("outputs"),at+"/boundary/outputs");empty(n.member("structure_tunnels"),at+"/structure_tunnels");
                    const auto &terminals=obj(n.member("structure_terminals"),at+"/structure_terminals");const auto &regions=arr(n.member("regions"),at+"/regions");
                    Json convertedRegions=array();std::string caseType;
                    if(structureType=="for_loop") {
                        if(const auto *conditional=n.member("for_loop_conditional_terminal");conditional&&flag(conditional,at))refuse(at+"/for_loop_conditional_terminal","Conditional For execution needs its own early-stop admission contract.","migration.for_conditional_unavailable", Domain::FrontendCapability);
                        if(terminals.object.size()!=2||!terminals.member("count")||!terminals.member("index"))refuse(at+"/structure_terminals","A nonconditional For has exactly count and index terminals.","migration.for_conditional_unavailable", Domain::Semantics);
                        terminal(terminals,"count","i64","count","count_value",true,at);terminal(terminals,"index","i64","index","iteration",false,at);
                        if(regions.size()!=1||txt(regions[0].member("id"),at)!="body")refuse(at+"/regions","A For must own exactly one explicit body region.", "migration.unsupported", Domain::Semantics);
                    } else {
                        if(const auto *profile=n.member("case_profile");profile&&txt(profile,at)!="frog.case-editing@1")refuse(at+"/case_profile","Unknown Case editing profile.","migration.case_profile_unsupported", Domain::Compatibility);
                        if(const auto *profile=n.member("binding_profile");profile&&txt(profile,at)!="frog.typed-binding@1")refuse(at+"/binding_profile","Unknown Case terminal binding profile.", "migration.unsupported", Domain::Compatibility);
                        if(terminals.object.size()!=1||!terminals.member("selector"))refuse(at+"/structure_terminals","A Case has exactly one typed selector.","migration.case_selector_unavailable", Domain::Semantics);
                        const auto &selector=obj(terminals.member("selector"),at);caseType=txt(selector.member("type"),at);
                        if(caseType!="bool"&&caseType!="i64"&&caseType!="u64")refuse(at+"/structure_terminals/selector/type","Only exact bool, i64 and u64 Case selectors are admitted; String, Enum and other widths need separate contracts.","migration.case_selector_unavailable", Domain::FrontendCapability);
                        terminal(terminals,"selector",caseType.c_str(),"selector","selected_case",true,at);
                        if((caseType=="bool"&&regions.size()!=2)||regions.empty())refuse(at+"/regions","Boolean Case needs two true/false arms; integer Case needs explicit regions and one default.","migration.case_match_unavailable", Domain::Semantics);
                    }
                    std::set<std::string> regionIds,integerMatches;std::set<bool> matches;std::size_t defaultCount=0;
                    for(std::size_t r=0;r<regions.size();++r) {
                        const auto rp=at+"/regions/"+std::to_string(r);const auto &region=obj(&regions[r],rp);const auto rid=txt(region.member("id"),rp+"/id");
                        if(!regionIds.insert(rid).second)refuse(rp,"Duplicate region identity within the structure.","migration.identity_duplicate", Domain::Integrity);
                        for(const auto &[key,_]:region.object)if(key!="id"&&key!="diagram"&&key!="title"&&key!="match"&&key!="matches"&&key!="default"&&key!="draft"&&key!="draft_text")refuse(rp+"/"+key,"Unknown region semantics are not discarded.", "migration.unsupported", Domain::Compatibility);
                        if(region.member("draft_text")||(region.member("draft")&&flag(region.member("draft"),rp)))refuse(rp,"Draft Case arms are not executable.","migration.case_match_unavailable", Domain::Semantics);
                        const bool fallback=region.member("default")&&flag(region.member("default"),rp);
                        if(fallback&&(caseType=="bool"||structureType!="case"))refuse(rp,"A Boolean Case/For cannot declare a default arm.","migration.case_match_unavailable", Domain::Semantics);
                        if(fallback&&++defaultCount>1)refuse(rp,"An integer Case requires exactly one default arm.","migration.case_default_duplicate", Domain::Semantics);
                        Json converted=object();converted.object["id"]=string(rid);
                        if(structureType=="case"&&caseType!="bool") {
                            if(region.member("match"))refuse(rp,"Integer Case labels require explicit integer_exact decimal-string matchers.","migration.case_match_unavailable", Domain::Compatibility);
                            bool explicitMatch=false;
                            if(const auto *many=region.member("matches")) {
                                const auto &entries=arr(many,rp+"/matches");if(entries.size()>1)refuse(rp,"This integer Case corridor admits one exact matcher per region, not ranges or unions.","migration.case_match_unavailable", Domain::FrontendCapability);
                                if(!entries.empty()) {
                                    const auto &entry=obj(&entries.front(),rp+"/matches/0");
                                    if(entry.object.size()!=2||txt(entry.member("kind"),rp)!="integer_exact")refuse(rp,"Only integer_exact decimal-string Case matchers are admitted.","migration.case_match_unavailable", Domain::Compatibility);
                                    const auto exact=caseInteger(entry.member("value"),caseType);
                                    if(!exact)refuse(rp+"/matches/0/value","The decimal Case value is outside the exact selector type.","migration.case_match_invalid", Domain::Semantics);
                                    if(!integerMatches.insert(*exact).second)refuse(rp,"Integer Case values overlap across regions.","migration.case_match_duplicate", Domain::Semantics);
                                    explicitMatch=true;
                                }
                                converted.object["matches"]=*many;
                            }
                            if(!explicitMatch&&!fallback)refuse(rp,"An integer Case region needs its exact matcher or default flag.","migration.case_match_invalid", Domain::Semantics);
                            if(fallback)converted.object["default"]=boolean(true);
                        }else if(structureType=="case") {
                            const Json *match=region.member("match");
                            if(const auto *many=region.member("matches")) {
                                const auto &entries=arr(many,rp+"/matches");if(match||entries.size()!=1)refuse(rp,"Each Boolean arm needs exactly one unambiguous match.","migration.case_match_unavailable", Domain::Semantics);
                                const auto &entry=obj(&entries[0],rp+"/matches/0");
                                if(entry.object.size()!=2||txt(entry.member("kind"),rp)!="boolean_exact")refuse(rp,"Only exact Boolean Case matchers are admitted.","migration.case_match_unavailable", Domain::Compatibility);match=entry.member("value");
                            }
                            if(!match||match->type!=Json::Type::Boolean||!matches.insert(match->boolean).second)refuse(rp,"Boolean Case needs one true and one false arm, without overlapping matches.","migration.case_match_unavailable", Domain::Semantics);
                            converted.object["match"]=*match;
                        }else if(region.member("match")||region.member("matches"))refuse(rp,"A For body cannot declare Case matching semantics.", "migration.unsupported", Domain::Semantics);
                        converted.object["diagram"]=scope(obj(region.member("diagram"),rp+"/diagram"),rp+"/diagram",target+"/regions/"+std::to_string(r)+"/diagram");
                        const auto qualified=std::to_string(id.size())+":"+id+":"+rid;
                        out.identities.push_back({"region",qualified,rp,target+"/regions/"+std::to_string(r),false});convertedRegions.array.push_back(std::move(converted));
                    }
                    if(structureType=="case"&&n.member("case_profile"))item.object["case_profile"]=*n.member("case_profile");
                    if(structureType=="case"&&caseType!="bool"&&defaultCount!=1)refuse(at+"/regions","The exact integer Case profile requires one explicit default arm.","migration.case_default_required", Domain::Semantics);
                    item.object["structure_type"]=string(structureType);item.object["boundary"]=boundary;item.object["structure_terminals"]=terminals;item.object["regions"]=std::move(convertedRegions);
                    for(const auto &[key,value]:n.object)if(key.starts_with("for_loop_")||key.starts_with("case_structure_"))item.object[key]=value;
                    nodeList.array.push_back(std::move(item));continue;
                }
                if(structureType!="while_loop")refuse(at,"Only public While, nonconditional For and exact bool/i64/u64 Case structures are admitted.", "migration.unsupported", structureDomain(n));
                const auto &boundary=obj(n.member("boundary"),at+"/boundary");empty(boundary.member("inputs"),at+"/boundary/inputs");empty(boundary.member("outputs"),at+"/boundary/outputs");
                empty(n.member("structure_tunnels"),at+"/structure_tunnels");
                const auto &terminals=obj(n.member("structure_terminals"),at+"/structure_terminals");
                auto converted=terminals;auto &condition=converted.object.at("condition");const auto &index=obj(terminals.member("index"),at+"/structure_terminals/index");
                if(txt(index.member("type"),at)!="i64"||txt(index.member("body_port"),at)!="iteration"||txt(index.member("role"),at)!="index"||
                    flag(index.member("outer_visible"),at)||!flag(index.member("exposed_in_body"),at)||!flag(index.member("read_only"),at))refuse(at,"Iteration terminal does not match public I64 semantics.", "migration.unsupported", Domain::Semantics);
                const auto &c=obj(&condition,at+"/structure_terminals/condition");const auto role=txt(c.member("role"),at);
                if(txt(c.member("type"),at)!="bool"||txt(c.member("body_port"),at)!="condition"||flag(c.member("outer_visible"),at)||
                    !flag(c.member("exposed_in_body"),at)||flag(c.member("read_only"),at)||(role!="stop_if_true"&&role!="continue_while_true"))refuse(at,"Unsupported While condition contract.", "migration.unsupported", Domain::Compatibility);
                bool invert=role=="stop_if_true";if(const auto *stop=n.member("while_loop_stop_if_true"))if(flag(stop,at)!=invert)refuse(at,"While condition role conflicts with its saved stop mode.", "migration.unsupported", Domain::Semantics);
                condition.object["role"]=string("continue_while_true");
                const auto &regions=arr(n.member("regions"),at+"/regions");if(regions.size()!=1||txt(regions[0].member("id"),at)!="body")refuse(at,"A While must own exactly one explicit body.", "migration.unsupported", Domain::Semantics);
                auto body=scope(obj(regions[0].member("diagram"),at+"/regions/0/diagram"),at+"/regions/0/diagram",target+"/regions/0/diagram");
                if(invert){
                    const auto notId=fresh(id+":continue_not"),edgeId=fresh(id+":continue_edge");std::size_t found=0;
                    for(auto &edge:body.object.at("edges").array){auto &to=edge.object.at("to");
                        if(txt(to.member("node"),at)==id&&txt(to.member("port"),at)=="condition"){
                            to.object["node"]=string(notId);to.object["port"]=string("in");++found;}}
                    if(found!=1)refuse(at,"Stop-if-true migration requires exactly one real condition driver.", "migration.unsupported", Domain::Semantics);
                    Json logicalNot=object();logicalNot.object={{"id",string(notId)},{"kind",string("primitive")},{"type",string("frog.core.not")}};
                    // This is a new, explicitly generated semantic node. Its
                    // authoring placement is derived from its owner's frame;
                    // original node geometry is never repurposed or changed.
                    if(const auto *frame=n.member("layout")){
                        Json rect=object();const double width=std::stod(frame->member("width")->string),height=std::stod(frame->member("height")->string);
                        const double size=(std::min)(48.,(std::min)(width/3.,height/3.));
                        const auto number=[](double v){return *source::parseJson(std::to_string(v),nullptr,source::JsonNumberPolicy::PreserveLexeme);};
                        rect.object={{"x",number(std::stod(frame->member("x")->string)+width-size-36)},
                            {"y",number(std::stod(frame->member("y")->string)+height-size-64)},{"width",number(size)},{"height",number(size)}};
                        logicalNot.object["layout"]=std::move(rect);
                    }
                    const auto bodyPath=target+"/regions/0/diagram";const auto ni=body.object.at("nodes").array.size(),ei=body.object.at("edges").array.size();
                    body.object.at("nodes").array.push_back(std::move(logicalNot));
                    Json from=object(),to=object(),bridge=object();from.object={{"node",string(notId)},{"port",string("result")}};to.object={{"node",string(id)},{"port",string("condition")}};
                    bridge.object={{"id",string(edgeId)},{"from",std::move(from)},{"to",std::move(to)}};body.object.at("edges").array.push_back(std::move(bridge));
                    identity("node",notId,at+"/structure_terminals/condition",bodyPath+"/nodes/"+std::to_string(ni),true);
                    identity("edge",edgeId,at+"/structure_terminals/condition",bodyPath+"/edges/"+std::to_string(ei),true);
                }
                Json region=object(),regionList=array();region.object={{"id",string("body")},{"diagram",std::move(body)}};regionList.array.push_back(std::move(region));
                item.object["structure_type"]=string("while_loop");item.object["boundary"]=boundary;item.object["structure_terminals"]=std::move(converted);item.object["regions"]=std::move(regionList);
                for(const auto &[key,v]:n.object)if(key.starts_with("while_loop_")&&key!="while_loop_stop_if_true")item.object[key]=v;
            }else if(kind=="constant")refuse(at,"The public constant family is not closed by the source contract; use an explicit typed input.","migration.constant_contract_unavailable", Domain::FrontendCapability);
            else refuse(at,"Node family has no admitted migration.", "migration.unsupported", Domain::Compatibility);
            nodeList.array.push_back(std::move(item));
        }
        const auto &es=arr(raw.member("edges"),path+"/edges");if(edges.size()+es.size()>4096)refuse(path,"Edge budget exceeded.", "migration.unsupported", Domain::Control);
        for(std::size_t i=0;i<es.size();++i){const auto at=path+"/edges/"+std::to_string(i);const auto &e=obj(&es[i],at);const auto id=txt(e.member("id"),at+"/id");
            if(const auto *label=e.member("label")) {
                draft_presentation::wireLabel(*label,at+"/label",[](auto path,auto message){
                    refuse(std::move(path),std::move(message),"migration.presentation_invalid", Domain::Compatibility);
                });
                out.identities.push_back({"presentation",id+":label",at+"/label",std::string(draft_presentation::provenance)+at+"/label",false});
            }
            const auto edgeTarget=targetPath+"/edges/"+std::to_string(i);identity("edge",id,at,edgeTarget);Json item=object();item.object["id"]=string(id);
            for(const auto side:{"from","to"}){const auto &endpoint=obj(e.member(side),at+"/"+side);const auto node=txt(endpoint.member("node"),at),port=txt(endpoint.member("port"),at);item.object[side]=endpoint;
                if(const auto op=stringNodes.find(node);op!=stringNodes.end()) {
                    const auto *contract=canonicalStringNativeContract(op->second);
                    if(contract->operation==CanonicalStringOperation::LengthBytes&&
                        ((std::string_view(side)=="to"&&port=="string")||(std::string_view(side)=="from"&&port=="length"))) {
                        item.object[side].object["port"]=string(std::string_view(side)=="to"?"value":"result");
                        out.identities.push_back({"port",id+":"+side+":"+port,at+"/"+side+"/port",edgeTarget+"/"+side+"/port",false});
                    }
                }
                if(std::string_view(side)=="to"&&comparisonNodes.contains(node)&&(port=="x"||port=="y")) {
                    item.object[side].object["port"]=string(port=="x"?"a":"b");
                    // The derived endpoint name changes; its source token and
                    // the exact original draft remain attributable/recoverable.
                    const auto token=at+"/"+side+"/port";
                    out.identities.push_back({"port",id+":"+side+":"+port,token,edgeTarget+"/"+side+"/port",false});
                }
                if(std::string_view(side)=="to"&&unbundleNodes.contains(node)&&port=="cluster") {
                    item.object[side].object["port"]=string("record");out.identities.push_back({"port",id+":to:cluster",at+"/to/port",edgeTarget+"/to/port",false});
                }
            }
            if(const auto *bends=e.member("bends"))item.object["bends"]=*bends;edgeList.array.push_back(std::move(item));}
        result.object={{"nodes",std::move(nodeList)},{"edges",std::move(edgeList)}};return result;
    }
};
}
static CanonicalDraftMigrationResult prepareCanonicalDraftMigrationImpl(std::string_view bytes,std::string_view uri,const CanonicalDraftResolvedTypes *resolved){
    CanonicalDraftMigrationResult result;result.originalUri=uri;
    if(bytes.size()>8U*1024U*1024U){result.diagnostics.push_back({"migration.resource_limit","/","Draft exceeds the bounded migration source budget.",true,Domain::Control});return result;}
    result.originalSource=std::make_shared<const std::string>(bytes);result.originalSha256=hashing::sha256Hex(bytes);
    try{
        if(resolved) {
            if(resolved->originalSha256!=result.originalSha256)refuse("/","Binding resolution belongs to different exact source bytes.","migration.scalar_resolution_stale", Domain::Integrity);
            if(resolved->scalars.size()>1024)refuse("/","Binding resolution exceeds the source node budget.","migration.scalar_resolution_invalid", Domain::Integrity);
            std::set<std::string> ids;for(const auto &entry:resolved->scalars)if(entry.nodeId.empty()||entry.authoredOperation.empty()||entry.operands.empty()||entry.operands.size()>16||!ids.insert(entry.nodeId).second)
                refuse("/","Binding resolution contains a missing/duplicate identity or invalid arity.","migration.scalar_resolution_invalid", Domain::Integrity);
        }
        if(uri.empty()||uri.size()>8192||uri.find('#')!=uri.npos)refuse("/","An explicit bounded original URI without a fragment is required.", "migration.unsupported", Domain::Integrity);
        std::string error;auto parsed=source::parseJson(bytes,&error,source::JsonNumberPolicy::PreserveLexeme);if(!parsed)refuse("/",error,"migration.json", Domain::Integrity);
        const auto &raw=obj(&*parsed,"/");if(raw.member("spec_version")||txt(raw.member("format"),"/format")!="frog.document.draft"||
            !raw.member("draft_revision")||raw.member("draft_revision")->type!=Json::Type::Number||
            (raw.member("draft_revision")->string!="2"&&raw.member("draft_revision")->string!="4"&&raw.member("draft_revision")->string!="5"&&raw.member("draft_revision")->string!="6"&&raw.member("draft_revision")->string!="7"))refuse("/","Only identified Studio drafts 2/4 and the closed scalar5 NativeCall revision-7 corridor are admitted.", "migration.unsupported", Domain::Compatibility);
        // Private drafts may omit metadata; their existing reader keeps the
        // normal document defaults. Only the derived public envelope needs an
        // explicit object. Never treat explicit null/wrong types as absence.
        const auto *metadata=raw.member("metadata");
        if(metadata&&metadata->type!=Json::Type::Object)
            refuse("/metadata","File metadata must be an object.","migration.metadata_invalid", Domain::Integrity);
        // Check private launch metadata before either migration corridor can
        // consume the draft. It stays in authored-source provenance only;
        // it never changes public ports, operation semantics or the FIR graph.
        if(const auto *native=raw.member("native_options")) {
            obj(native,"/native_options");
            for(const auto &[key,_]:native->object)
                if(key!="version"&&key!="target_id"&&key!="backend_id"&&key!="optimization_profile")
                    refuse("/native_options/"+key,"Unknown native launch configuration field.","migration.native_options_invalid", Domain::Compatibility);
            const auto *version=native->member("version");
            if(!version||version->type!=Json::Type::Number||version->string!="1")
                refuse("/native_options/version","Native launch configuration requires explicit version1.","migration.native_options_invalid", Domain::Compatibility);
            for(const auto name:{"target_id","backend_id","optimization_profile"}) {
                const auto *value=native->member(name);
                if(!value||value->type!=Json::Type::String||value->string.empty()||value->string.size()>1024||value->string.find('\0')!=std::string::npos)
                    refuse(std::string("/native_options/")+name,"Native launch choices must be explicit bounded strings.","migration.native_options_invalid", Domain::Integrity);
            }
        }
        if(canonicalDraftNeedsComposition(raw,resolved))
            return prepareCanonicalComposedDraftMigration(bytes,uri,resolved);
        const bool nativeCallDraft7=raw.member("draft_revision")->string=="7";
        const bool objectDraft=raw.member("draft_revision")->string=="6"||nativeCallDraft7;
        if(objectDraft) {
            // Detect the actual configured node and all real scopes before any
            // conversion. Revision7 is not a global Connectivity admission.
            std::vector<const Json*> scopes{&obj(raw.member("diagram"),"/diagram")};
            std::size_t visited=0,calls=0,objects=0;bool structure=false;
            while(!scopes.empty()) {
                const auto *scope=scopes.back();scopes.pop_back();
                const auto &nodes=arr(scope->member("nodes"),"/diagram/nodes");
                if(nodes.size()>1024-visited)refuse("/diagram","NativeCall scan exceeds the existing node budget.","migration.resource_limit", Domain::Control);visited+=nodes.size();
                for(const auto &node:nodes) {
                    if(const auto *type=node.member("type");type&&type->type==Json::Type::String&&type->string==kNativeCallDiagramNodeType&&node.member("studio_native_call"))++calls;
                    if(authoredObjectNode(node))++objects;
                    if(const auto *regions=node.member("regions")) {
                        structure=true;const auto &entries=arr(regions,"/diagram/regions");
                        if(entries.size()>1024||scopes.size()>1024-entries.size())refuse("/diagram","Region scan exceeds the existing node budget.","migration.resource_limit", Domain::Control);
                        for(const auto &region:entries)scopes.push_back(&obj(region.member("diagram"),"/diagram/regions/diagram"));
                    }
                    if(const auto *kind=node.member("kind");kind&&kind->type==Json::Type::String&&kind->string=="structure")structure=true;
                }
            }
            if(!calls&&!objects)refuse("/draft_revision","This writer revision needs an actual configured scalar5 NativeCall or local Object Reference, not unrelated private extensions.","migration.configured_revision_unavailable", Domain::Compatibility);
            if(calls&&structure)refuse("/diagram","This first NativeCall corridor is flat; authored regions are not flattened.","migration.native_call_region_unavailable", Domain::Semantics);
        }
        for(const auto &[key,_]:raw.object)if(key!="format"&&key!="draft_revision"&&key!="language"&&key!="title"&&key!="metadata"&&key!="interface"&&key!="interface_map"&&key!="frontPanel"&&key!="diagram"&&key!="host"&&key!="execution_policy"&&key!="text_defaults"&&key!="ide"&&key!="native_options"&&key!="icon")
            refuse("/"+key,"This source section needs a separately specified migration.", "migration.unsupported", Domain::Compatibility);
        if(const auto *icon=raw.member("icon"))draft_presentation::icon(*icon,[](auto path,auto message){
            refuse(std::move(path),std::move(message),"migration.presentation_invalid", Domain::Compatibility);
        });
        if(const auto *map=raw.member("interface_map")) {
            if(const auto issue=private_interface_map::validate(*map,"/interface_map"))
                refuse(issue->path,issue->message,std::string(private_interface_map::diagnosticCode(issue->kind,false)), issue->kind==private_interface_map::IssueKind::UnknownField?Domain::Compatibility:Domain::Integrity);
            result.diagnostics.push_back({"migration.interface_map_preserved","/interface_map",
                "Retained the validated private presentation map in the exact authored-source provenance; no public ports, widget associations or executable boundary bindings were inferred.",false});
        }
        const auto *textDefaults=raw.member("text_defaults");
        if(textDefaults) {
            obj(textDefaults,"/text_defaults");
            for(const auto &[key,_]:textDefaults->object)
                if(key!="point_size"&&key!="font_family")
                    refuse("/text_defaults/"+key,"Unknown document typography requires an explicit migration contract.", "migration.unsupported", Domain::Compatibility);
            const auto *points=textDefaults->member("point_size");
            if(!points||!typedScalar(*points,"f64")||!validDocumentTextPointSize(std::stod(points->string)))
                refuse("/text_defaults/point_size","Document font size must be a finite number from 6 to 144 pt.","migration.text_defaults_invalid", Domain::Semantics);
            if(const auto *family=textDefaults->member("font_family");family&&
                (family->type!=Json::Type::String||!validDocumentTextFontFamily(family->string)))
                refuse("/text_defaults/font_family","Document font family must be nonempty UTF-8 of at most 256 bytes without NUL.","migration.text_defaults_invalid", Domain::Semantics);
        }
        // An unconnected private document may omit its declaration or retain
        // JSON null. Both mean that no public interface has been authored;
        // they do not waive validation of an actual declaration.
        Json interface=object();interface.object={{"inputs",array()},{"outputs",array()}};
        if(const auto *declared=raw.member("interface");declared&&declared->type!=Json::Type::Null) {
            const auto &actual=obj(declared,"/interface");
            for(const auto &[key,_]:actual.object)if(key!="inputs"&&key!="outputs")
                refuse("/interface/"+key,"An authored interface field needs its own migration contract.","migration.interface_unsupported", Domain::Compatibility);
            empty(actual.member("inputs"),"/interface/inputs");empty(actual.member("outputs"),"/interface/outputs");
        }
        Migration migration{result};migration.resolvedTypes=resolved;migration.exactIntegerState=raw.member("draft_revision")->string=="4"||objectDraft;migration.nativeCallDraft7=nativeCallDraft7;migration.objectDraft=objectDraft;
        migration.reserve(obj(raw.member("diagram"),"/diagram"));
        Json publicDocument=object();publicDocument.object={{"spec_version",string("0.1")},{"metadata",metadata?*metadata:object()},{"interface",interface}};
        publicDocument.object["front_panel"]=migration.panel(obj(raw.member("frontPanel"),"/frontPanel"));
        publicDocument.object["diagram"]=migration.scope(obj(raw.member("diagram"),"/diagram"),"/diagram");
        if(resolved&&migration.consumedResolutions.size()!=resolved->scalars.size())refuse("/diagram","Binding context contains an operation absent from this exact migration.","migration.scalar_resolution_invalid", Domain::Integrity);
        if(const auto *host=raw.member("host"))publicDocument.object["host"]=*host;
        if(const auto *policy=raw.member("execution_policy")){
            obj(policy,"/execution_policy");const auto *version=policy->member("version");
            if(!version||version->type!=Json::Type::Number||(version->string!="1"&&version->string!="2"))refuse("/execution_policy/version","Unsupported private execution-policy version.", "migration.unsupported", Domain::Compatibility);
            Json effective=object();auto one=source::parseJson("1",nullptr,source::JsonNumberPolicy::PreserveLexeme);effective.object["version"]=*one;
            effective.object["debugging_allowed"]=boolean(flag(policy->member("debugging_allowed"),"/execution_policy/debugging_allowed"));
            for(const auto &[key,v]:policy->object){if(key=="version"||key=="debugging_allowed")continue;
                const bool neutral=(key=="priority"&&v.type==Json::Type::String&&v.string=="normal")||
                    (key=="reentrancy"&&v.type==Json::Type::String&&v.string=="non_reentrant")||
                    (key=="execution_system"&&v.type==Json::Type::String&&v.string=="caller")||
                    (key=="automatic_error_handling"&&v.type==Json::Type::Boolean&&v.boolean)||
                    ((key=="suspend_when_called"||key=="clear_indicators_when_called")&&v.type==Json::Type::Boolean&&!v.boolean);
                if(!neutral)refuse("/execution_policy/"+key,"Nondefault private execution policy has no public migration contract.", "migration.unsupported", Domain::FrontendCapability);}
            publicDocument.object["execution_policy"]=std::move(effective);
        }
        Json ide=raw.member("ide")?obj(raw.member("ide"),"/ide"):object();
        // Private authored defaults become public, non-executable IDE metadata.
        // The original private section also remains in recoverability below.
        if(textDefaults)ide.object["text_defaults"]=*textDefaults;
        Json recovery=ide.member("recoverability")?obj(ide.member("recoverability"),"/ide/recoverability"):object();
        Json provenance=object();provenance.object={{"original_uri",string(std::string(uri))},{"original_sha256",string(result.originalSha256)},
            {"profile",string("frog.studio.explicit-draft-to-public@1")},{"preserved_authoring_source",*parsed}};
        recovery.object["graiphic_studio_migration"]=std::move(provenance);ide.object["recoverability"]=std::move(recovery);publicDocument.object["ide"]=std::move(ide);
        auto accepted=FrogSourceDocument::read(encode(publicDocument),&error);
        if(!accepted||accepted->envelopeStatus()!=FrogSourceEnvelopeStatus::Recognized||!accepted->diagnostics().empty())refuse("/",error.empty()?"Migrated public source failed source-envelope inspection.":error,"migration.public_source_invalid", Domain::Semantics);
        result.publicSource=std::make_shared<const FrogSourceDocument>(std::move(*accepted));
        if(nativeCallDraft7&&!migration.hasNativeCall&&!migration.hasObjectReferences)refuse("/diagram","The exact configured NativeCall was not migrated.","migration.native_call_configuration_invalid", Domain::Semantics);
        if(migration.hasObjectReferences)result.diagnostics.push_back({"migration.object_reference_execution_profile","/diagram","The local object-reference profile retains complete schemas, stable nested fields and target identity. Reference transport does not read Value. Explicit Value/Reinitialize effects use the real instance store; Reference needs effect opt-in and AOT is unavailable.",false});
        if(migration.hasNativeCall)result.diagnostics.push_back({"migration.native_call_execution_profile","/diagram","Local scalar5 NativeCall retains its full codec JSON, actual scalar ABI order, default presence and complete error/mirror tuple. This first corridor is flat; Reference needs explicit local-DLL opt-in and AOT/Ui/pointers/callbacks remain unavailable.",false});
        if(migration.hasArrayReal1d)result.diagnostics.push_back({"migration.array_execution_profile","/diagram","Local frog.studio.array.real1d@1: real finite f64 Build1..32, rank1 dynamic array<f64>, explicit wired i32 Index with ARRAY_INDEX_RANGE and Size i32. No public Collections alias, Array widget/literal, Array state/region transfer, Record/String/Event composition or AOT is implied.",false});
        if(migration.hasStringBytes)result.diagnostics.push_back({"migration.string_execution_profile","/diagram","String2 uses the local frog.studio.string.bytes@1 profile: valid UTF-8 including NUL, byte Length i32 and ordered scalar Concat with real a/b drivers. No public frog.text alias, Array/Cluster lifting, String controls or owned AOT is implied.",false});
        if(migration.hasBooleanReduction)result.diagnostics.push_back({"migration.boolean_execution_profile","/diagram","Local frog.studio.boolean.reduce@1 uses exact bool a,b plus actual ascending input_3..input_16 operands; NAND/NOR/XNOR invert once after the complete fold. Implies is a separate local binary !a||b. Existing Core AND/OR/XOR/NOT remain fixed; integer, Array and Cluster coercion/lifting are unavailable.",false});
        if(migration.hasComparisonZero)result.diagnostics.push_back({"migration.comparison_zero_execution_profile","/diagram","Local frog.studio.comparison.zero-exact-ieee@1: one actual x operand of exact i32/u32/i64/u64/f64 compared with typed zero, bool result, IEEE unordered Not Equal only. No zero node, binary port remapping, implicit conversion or aggregate lifting is introduced.",false});
        if(migration.hasUnaryProfile)result.diagnostics.push_back({"migration.unary_execution_profile","/diagram","Authored Unary7 uses exact width private integer wrapping; Negate/Absolute reject unsigned. Authored Round maps to round_nearest.even64.v1 with fixed nearest/ties-to-even; explicitly persisted historical fenv64.v1 keeps caller-FENV semantics. Published Math Round is not redefined; Floor/Ceil keep pure value identities. No cast, f32, complex or aggregate lifting is inserted.",false});
        if(migration.hasScalar32)result.diagnostics.push_back({"migration.scalar32_execution_profile","/diagram","Resolved exact same-type i32/u32 arithmetic selects frog.studio.numeric.wrap32@1 and comparisons select frog.studio.comparison.exact32@1. Arithmetic wraps modulo2^32 and comparisons retain signed/unsigned32 ordering. The separate explicit integer-real64 Divide profile converts its two identical integer operands to f64; public Core contracts are unchanged.",false});
        if(migration.hasScalarArithmetic)result.diagnostics.push_back({"migration.scalar_execution_profile","/diagram",
            "Authored arithmetic selects frog.studio.numeric.ieee-wrap64@1: exact same-type i64/u64 Add/Sub/Mul wrap modulo 2^64, while f64 arithmetic retains strict IEEE signed-zero, infinity and NaN behavior. The distinct integer-real64 Divide profile converts identical i32/u32/i64/u64 operands to f64 before division; mixed operands remain unavailable. Public frog.core arithmetic keeps its distinct checked/finite execution profile.",false});
        result.diagnostics.push_back({"migration.explicit_copy","/","Prepared a distinct public source. Private authoring state is retained only as non-executable recoverability metadata; native validation and admission remain mandatory.",false});
    }catch(const Refusal &failure){result.publicSource.reset();result.diagnostics.push_back({failure.code,failure.path,failure.message,true,failure.domain});}
    catch(const std::exception &failure){result.publicSource.reset();result.diagnostics.push_back({"migration.invalid_source","/",failure.what(),true,Domain::Control});}
    return result;
}
CanonicalDraftMigrationResult prepareCanonicalDraftMigration(std::string_view bytes,std::string_view uri){return prepareCanonicalDraftMigrationImpl(bytes,uri,nullptr);}
CanonicalDraftMigrationResult prepareCanonicalDraftMigration(std::string_view bytes,std::string_view uri,const CanonicalDraftResolvedTypes &types){return prepareCanonicalDraftMigrationImpl(bytes,uri,&types);}
}
