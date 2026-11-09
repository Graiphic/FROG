#include "frog_native_engine/ApplicationControlSourceCodec.hpp"
#include <charconv>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
namespace frog::native_engine::application_control_source {

namespace {
using JsonValue=source::JsonValue;
const JsonValue *objectField(const JsonValue &v,std::string_view key){return v.member(key);}
std::optional<std::string> jsonStringField(const JsonValue &v,std::string_view key){const auto *p=v.member(key);return p&&p->type==JsonValue::Type::String?std::optional{p->string}:std::nullopt;}
std::optional<bool> jsonBooleanField(const JsonValue &v,std::string_view key){const auto *p=v.member(key);return p&&p->type==JsonValue::Type::Boolean?std::optional{p->boolean}:std::nullopt;}
std::vector<std::string> jsonStringArrayField(const JsonValue &v,std::string_view key){std::vector<std::string> out;const auto *p=v.member(key);if(p&&p->type==JsonValue::Type::Array)for(const auto &x:p->array)if(x.type==JsonValue::Type::String)out.push_back(x.string);return out;}
std::optional<std::uint32_t> jsonUnsignedField(const JsonValue &v,std::string_view key,ReadPolicy policy){
    const auto *p=v.member(key);if(!p||p->type!=JsonValue::Type::Number)return {};
    if(policy==ReadPolicy::ExactLocalProfile||!std::isfinite(p->number)){
        std::uint32_t n=0;const auto r=std::from_chars(p->string.data(),p->string.data()+p->string.size(),n);
        return r.ec==std::errc{}&&r.ptr==p->string.data()+p->string.size()?std::optional{n}:std::nullopt;
    }
    if(p->number<0||p->number>(std::numeric_limits<std::uint32_t>::max)())return {};
    return static_cast<std::uint32_t>(p->number);
}
void appendIndent(std::ostringstream &out,int n){for(int i=0;i<n;++i)out<<"  ";}
void appendJsonString(std::ostringstream &out,std::string_view value){
    out<<'"';std::size_t run=0;
    for(std::size_t i=0;i<value.size();++i){const auto c=static_cast<unsigned char>(value[i]);if(c>=32&&c!='"'&&c!='\\')continue;
        if(i!=run)out.write(value.data()+run,static_cast<std::streamsize>(i-run));
        switch(c){case '"':out<<"\\\"";break;case '\\':out<<"\\\\";break;case '\b':out<<"\\b";break;case '\f':out<<"\\f";break;case '\n':out<<"\\n";break;case '\r':out<<"\\r";break;case '\t':out<<"\\t";break;
        default:out<<"\\u"<<std::hex<<std::setw(4)<<std::setfill('0')<<static_cast<unsigned int>(c)<<std::dec<<std::setfill(' ');break;}run=i+1;}
    if(run!=value.size())out.write(value.data()+run,static_cast<std::streamsize>(value.size()-run));out<<'"';
}
void appendJsonField(std::ostringstream &out,int n,std::string_view key,std::string_view value,bool comma=true){appendIndent(out,n);appendJsonString(out,key);out<<": ";appendJsonString(out,value);out<<(comma?",\n":"\n");}
void appendJsonBooleanField(std::ostringstream &out,int n,std::string_view key,bool value,bool comma=true){appendIndent(out,n);appendJsonString(out,key);out<<": "<<(value?"true":"false")<<(comma?",\n":"\n");}
void appendJsonIntegerField(std::ostringstream &out,int n,std::string_view key,std::uint32_t value,bool comma=true){appendIndent(out,n);appendJsonString(out,key);out<<": "<<value<<(comma?",\n":"\n");}
std::string_view kindToken(ApplicationControlKind k) {
    switch(k) {
    case ApplicationControlKind::Reference: return "reference";
    case ApplicationControlKind::Property: return "property";
    case ApplicationControlKind::Invoke: return "invoke";
    case ApplicationControlKind::Close: return "close";
    }
    return {};
}
std::string_view targetToken(ApplicationControlTargetKind k) {
    switch(k) {
    case ApplicationControlTargetKind::Unbound: return "unbound";
    case ApplicationControlTargetKind::ThisFrog: return "this_frog";
    case ApplicationControlTargetKind::ThisApplication: return "this_application";
    case ApplicationControlTargetKind::FrontPanel: return "front_panel";
    case ApplicationControlTargetKind::Widget: return "widget";
    }
    return {};
}
void appendType(std::ostringstream &out,const binding::TypeRef &t,int n) {
    out << "{\n";
    appendJsonField(out,n+1,"canonical",t.canonical());
    appendJsonField(out,n+1,"choice_kind",t.choiceKind==binding::ChoiceKind::Enum?"enum":
        t.choiceKind==binding::ChoiceKind::Ring?"ring":t.choiceKind==binding::ChoiceKind::Dropdown?"dropdown":"none");
    appendIndent(out,n+1);out << "\"field_names\": [";
    for(std::size_t i=0;i<t.fieldNames.size();++i) {if(i)out << ',';appendJsonString(out,t.fieldNames[i]);}
    out << "],\n";appendIndent(out,n+1);out << "\"choices\": [";
    for(std::size_t i=0;i<t.choices.size();++i) {
        if(i)out << ',';const auto &c=t.choices[i];out << '\n';appendIndent(out,n+2);out << "{\n";
        appendJsonField(out,n+3,"id",c.id);appendJsonField(out,n+3,"label",c.label);
        appendJsonIntegerField(out,n+3,"value",c.value);appendJsonBooleanField(out,n+3,"disabled",c.disabled);
        appendJsonBooleanField(out,n+3,"visible",c.visible,false);appendIndent(out,n+2);out << '}';
    }
    out << "],\n";appendIndent(out,n+1);out << "\"children\": [";
    for(std::size_t i=0;i<t.children.size();++i) {
        if(i)out << ',';out << '\n';appendIndent(out,n+2);appendType(out,t.children[i],n+2);
    }
    out << "]\n";appendIndent(out,n);out << '}';
}
bool decodeType(const JsonValue &v,binding::TypeRef &t,std::string &why,ReadPolicy policy) {
    const auto spelling=jsonStringField(v,"canonical");
    if(v.type!=JsonValue::Type::Object || !spelling) {why="Missing reference value schema.";return false;}
    auto parsed=binding::parseType(*spelling);
    if(!parsed.valid()) {why=parsed.error;return false;}
    t=std::move(parsed.type);
    const auto *children=objectField(v,"children");
    if(!children || children->type!=JsonValue::Type::Array || children->array.size()!=t.children.size()) {
        why="Reference schema children do not match its canonical type.";return false;
    }
    for(std::size_t i=0;i<t.children.size();++i) {
        binding::TypeRef child;
        if(!decodeType(children->array[i],child,why,policy))return false;
        if(child.canonical()!=t.children[i].canonical()) {why="Reference schema child type mismatch.";return false;}
        t.children[i]=std::move(child);
    }
    t.fieldNames=jsonStringArrayField(v,"field_names");
    if(!t.fieldNames.empty() && t.fieldNames.size()!=t.fieldIds.size()) {
        why="Reference schema field names do not match field identities.";return false;
    }
    const auto choice=jsonStringField(v,"choice_kind").value_or("none");
    if(choice=="none") t.choiceKind=binding::ChoiceKind::None;
    else if(choice=="enum")t.choiceKind=binding::ChoiceKind::Enum;
    else if(choice=="ring")t.choiceKind=binding::ChoiceKind::Ring;
    else if(choice=="dropdown")t.choiceKind=binding::ChoiceKind::Dropdown;
    else {why="Unknown reference schema choice kind.";return false;}
    const auto *choices=objectField(v,"choices");
    if(!choices || choices->type!=JsonValue::Type::Array) {why="Missing reference schema choices.";return false;}
    for(const auto &c:choices->array) {
        const auto id=jsonStringField(c,"id"),label=jsonStringField(c,"label");
        const auto value=jsonUnsignedField(c,"value",policy);
        const auto disabled=jsonBooleanField(c,"disabled"),visible=jsonBooleanField(c,"visible");
        if(!id || !label || !value || !disabled || !visible) {why="Invalid reference choice metadata.";return false;}
        t.choices.push_back({*id,*label,*value,*disabled,*visible});
    }
    return true;
}
}

bool preflight(const JsonValue &root,std::string *error){
    const auto fail=[&](const char *why){if(error)*error=why;return false;};
    struct Work{const JsonValue *value;unsigned depth;};std::vector<Work> work{{&root,0}};std::size_t count=0,bytes=0;
    while(!work.empty()){
        const auto [v,depth]=work.back();work.pop_back();
        if(++count>4096||depth>96||v->string.size()>16'000'000-bytes)return fail("The complete object configuration exceeds its bounded schema or payload.");bytes+=v->string.size();
        if(v->type==JsonValue::Type::Object)for(const auto &[key,value]:v->object){
            if(key.size()>16'000'000-bytes||work.size()>=4096-count)return fail("Object configuration metadata exceeds its bounded size.");bytes+=key.size();
            if(key=="canonical"){
                if(value.type!=JsonValue::Type::String)return fail("A canonical type must be a string.");
                std::size_t nesting=0;bool quote=false,escape=false;
                for(char ch:value.string){if(quote){if(escape)escape=false;else if(ch=='\\')escape=true;else if(ch=='"')quote=false;continue;}
                    if(ch=='"')quote=true;else if(ch=='<'){if(++nesting>32)return fail("The canonical type exceeds depth32.");}
                    else if(ch=='>'){if(!nesting)return fail("Unbalanced canonical type.");--nesting;}}
                if(nesting||quote||escape)return fail("Incomplete canonical type.");
            }
            work.push_back({&value,depth+1});
        }else if(v->type==JsonValue::Type::Array)for(const auto &value:v->array){if(work.size()>=4096-count)return fail("Object configuration exceeds its node bound.");work.push_back({&value,depth+1});}
    }
    return true;
}
void appendConfiguration(std::ostringstream &out,const ApplicationControlNode &c,int n) {
    appendIndent(out,n);out << "\"studio_application_control\": {\n";
    appendJsonField(out,n+1,"profile","frog.studio.application_control@1");
    appendJsonField(out,n+1,"kind",kindToken(c.kind));appendJsonField(out,n+1,"expected_class",c.expectedClass);
    appendIndent(out,n+1);out << "\"value_schema\": ";appendType(out,c.valueType,n+1);out << ",\n";
    appendIndent(out,n+1);out << "\"target\": {\n";
    appendJsonField(out,n+2,"kind",targetToken(c.target.kind));appendJsonField(out,n+2,"owner_document",c.target.ownerDocumentId);
    appendJsonField(out,n+2,"widget_id",c.target.widgetId,false);appendIndent(out,n+1);out << "},\n";
    appendIndent(out,n+1);out << "\"properties\": [";
    for(std::size_t i=0;i<c.properties.size();++i) {
        if(i)out << ',';const auto &r=c.properties[i];out << '\n';appendIndent(out,n+2);out << "{\n";
        appendJsonField(out,n+3,"id",r.id);appendJsonField(out,n+3,"member_id",r.memberId);
        appendJsonField(out,n+3,"mode",r.mode==ApplicationControlPropertyMode::Write?"write":"read",false);
        appendIndent(out,n+2);out << '}';
    }
    out << "],\n";appendJsonField(out,n+1,"method_id",c.methodId);
    appendIndent(out,n+1);out << "\"parameters\": [";
    for(std::size_t i=0;i<c.parameters.size();++i) {
        if(i)out << ',';out << '\n';appendIndent(out,n+2);out << "{\n";
        appendJsonField(out,n+3,"id",c.parameters[i].id);
        appendJsonBooleanField(out,n+3,"enabled",c.parameters[i].enabled,false);appendIndent(out,n+2);out << '}';
    }
    out << "]\n";appendIndent(out,n);out << "},\n";
}
std::optional<ApplicationControlNode> readConfiguration(const JsonValue &value,std::string *error,ReadPolicy policy){
    const auto *v=&value;const auto fail=[&](std::string why)->std::optional<ApplicationControlNode>{if(error)*error=std::move(why);return {};};
    if(policy==ReadPolicy::ExactLocalProfile&&!preflight(value,error))return {};
    if(v->type!=JsonValue::Type::Object||jsonStringField(*v,"profile")!="frog.studio.application_control@1")return fail("Unsupported private application-control profile.");
    ApplicationControlNode c;
    const auto kind=jsonStringField(*v,"kind").value_or("");
    if(kind=="reference")c.kind=ApplicationControlKind::Reference;
    else if(kind=="property")c.kind=ApplicationControlKind::Property;
    else if(kind=="invoke")c.kind=ApplicationControlKind::Invoke;
    else if(kind=="close")c.kind=ApplicationControlKind::Close;
    else return fail("Unknown operation kind.");
    const auto cls=jsonStringField(*v,"expected_class");if(!cls || cls->empty())return fail("Missing expected class.");
    c.expectedClass=*cls;
    const auto *schema=objectField(*v,"value_schema");std::string why;
    if(!schema || !decodeType(*schema,c.valueType,why,policy))return fail(why.empty()?"Missing schema.":why);
    const auto *target=objectField(*v,"target");if(!target)return fail("Missing symbolic target.");
    const auto targetKind=jsonStringField(*target,"kind").value_or("");
    if(targetKind=="unbound")c.target.kind=ApplicationControlTargetKind::Unbound;
    else if(targetKind=="this_frog")c.target.kind=ApplicationControlTargetKind::ThisFrog;
    else if(targetKind=="this_application")c.target.kind=ApplicationControlTargetKind::ThisApplication;
    else if(targetKind=="front_panel")c.target.kind=ApplicationControlTargetKind::FrontPanel;
    else if(targetKind=="widget")c.target.kind=ApplicationControlTargetKind::Widget;
    else return fail("Unknown target kind.");
    c.target.ownerDocumentId=jsonStringField(*target,"owner_document").value_or("");
    c.target.widgetId=jsonStringField(*target,"widget_id").value_or("");
    const auto *rows=objectField(*v,"properties");const auto *params=objectField(*v,"parameters");
    if(!rows || rows->type!=JsonValue::Type::Array || !params || params->type!=JsonValue::Type::Array)
        return fail("Property rows and method bindings must be arrays.");
    std::unordered_set<std::string> ids;
    for(const auto &r:rows->array) {
        const auto id=jsonStringField(r,"id"),member=jsonStringField(r,"member_id"),mode=jsonStringField(r,"mode");
        if(!id || id->empty() || !ids.insert(*id).second || !member || !mode || (*mode!="read" && *mode!="write"))
            return fail("Property row identities or modes are invalid.");
        c.properties.push_back({*id,*member,*mode=="write"?ApplicationControlPropertyMode::Write:ApplicationControlPropertyMode::Read});
    }
    c.methodId=jsonStringField(*v,"method_id").value_or("");ids.clear();
    for(const auto &p:params->array) {
        const auto id=jsonStringField(p,"id");const auto enabled=jsonBooleanField(p,"enabled");
        if(!id || id->empty() || !ids.insert(*id).second || !enabled)return fail("Method parameter identities are invalid.");
        c.parameters.push_back({*id,*enabled});
    }
    return c;
}

source::JsonValue writeConfiguration(const ApplicationControlNode &cfg){
    std::ostringstream out;out<<"{\n";appendConfiguration(out,cfg,1);auto bytes=out.str();
    const auto last=bytes.find_last_not_of(" \t\r\n");if(last==bytes.npos||bytes[last]!=',')throw std::logic_error("ApplicationControl codec lost its object terminator.");
    bytes.erase(last,1);bytes+="}\n";auto root=source::parseJson(bytes,nullptr,source::JsonNumberPolicy::PreserveLexeme);
    if(!root||!root->member("studio_application_control"))throw std::logic_error("ApplicationControl codec lost its object.");return *root->member("studio_application_control");
}
source::JsonValue writeType(const binding::TypeRef &type){ApplicationControlNode cfg;cfg.valueType=type;return *writeConfiguration(cfg).member("value_schema");}
std::optional<binding::TypeRef> readType(const JsonValue &value,std::string *error,ReadPolicy policy){
    if(policy==ReadPolicy::ExactLocalProfile&&!preflight(value,error))return {};binding::TypeRef type;std::string why;
    if(!decodeType(value,type,why,policy)){if(error)*error=std::move(why);return {};}return type;
}
}

namespace frog::native_engine {std::string applicationControlPropertyPortId(std::string_view id){return "property:"+std::string(id);}}
