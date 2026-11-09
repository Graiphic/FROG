#include "frog_native_engine/CanonicalRecordConstant.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <set>

namespace frog::native_engine {
namespace {
struct Budget {std::size_t nodes=0,bytes=0;};
// BindingType is the shared authoring parser. Bound nesting before invoking
// its recursive descent, while ignoring angle brackets in exact quoted IDs.
bool boundedSyntaxDepth(std::string_view text) noexcept {
    unsigned depth=0;bool quoted=false,escaped=false;
    for(const char ch:text) {
        if(quoted){if(escaped)escaped=false;else if(ch=='\\')escaped=true;else if(ch=='"')quoted=false;continue;}
        if(ch=='"')quoted=true;
        else if(ch=='<'){if(++depth>32)return false;}
        else if(ch=='>'&&depth)--depth;
    }
    return true;
}
bool reject(CanonicalRecordDiagnostic *out,std::string code,std::string message,
    DocumentDiagnosticDomain domain=DocumentDiagnosticDomain::Semantics) {
    if(out)*out={std::move(code),std::move(message),domain};return false;
}
bool type(const binding::TypeRef &value,unsigned depth,Budget &budget,CanonicalRecordDiagnostic *out) {
    if(depth>32||++budget.nodes>4096)return reject(out,"IR_RECORD_RESOURCE_LIMIT","Record type exceeds depth 32 or 4096 values.",DocumentDiagnosticDomain::Control);
    if(value.kind==binding::TypeKind::Cluster) {
        if(value.children.size()>64||value.fieldIds.size()!=value.children.size())return reject(out,"IR_RECORD_TYPE","A Record has at most 64 ordered, explicitly identified fields.");
        std::set<std::string> ids;
        for(std::size_t i=0;i<value.children.size();++i) {
            const auto &id=value.fieldIds[i];
            if(id.empty()||id.size()>1024||!ids.insert(id).second||std::any_of(id.begin(),id.end(),[](unsigned char c){return c<32;}))return reject(out,"IR_RECORD_FIELD_ID","Record fields need distinct exact identities of 1–1024 UTF-8 bytes without structural control bytes.");
            if(!type(value.children[i],depth+1,budget,out))return false;
        }
        return true;
    }
    if(value.kind==binding::TypeKind::Primitive&&(value.name=="bool"||value.name=="i32"||value.name=="u32"||value.name=="i64"||value.name=="u64"||value.name=="f64"||value.name=="string"||value.name=="path"))return true;
    return reject(out,"IR_RECORD_TRANSPORT_UNSUPPORTED","The immutable Record profile admits exact bool/i32/u32/i64/u64/f64/string/path and nested Records only; no Array, Enum, reference or implicit cast.",DocumentDiagnosticDomain::FrontendCapability);
}
bool literal(const binding::TypeRef &schema,const source::JsonValue &value,unsigned depth,Budget &budget,CanonicalRecordDiagnostic *out) {
    if(depth>32||++budget.nodes>4096)return reject(out,"IR_RECORD_RESOURCE_LIMIT","Record value exceeds depth 32 or 4096 values.",DocumentDiagnosticDomain::Control);
    if(schema.kind==binding::TypeKind::Cluster) {
        if(value.type!=source::JsonValue::Type::Object||value.object.size()!=schema.fieldIds.size())return reject(out,"IR_RECORD_LITERAL_SHAPE","Record JSON must contain exactly the declared direct field identities.");
        for(std::size_t i=0;i<schema.fieldIds.size();++i) {
            const auto *field=value.member(schema.fieldIds[i]);if(!field)return reject(out,"IR_RECORD_LITERAL_FIELD","A declared Record field is missing; fields are never inferred from JSON values or labels.");
            if(!literal(schema.children[i],*field,depth+1,budget,out))return false;
        }
        return true;
    }
    if(schema.name=="string"||schema.name=="path") {
        if(value.type!=source::JsonValue::Type::String)return reject(out,"IR_RECORD_LITERAL_TYPE","A String/Path field needs its exact JSON string, including any embedded NUL.");
        if(value.string.size()>1'000'000U||budget.bytes>16'000'000U-value.string.size())return reject(out,"IR_RECORD_RESOURCE_LIMIT","Record strings exceed the SDK's 1 000 000 per-string or 16 000 000 aggregate byte budget.",DocumentDiagnosticDomain::Control);
        budget.bytes+=value.string.size();return true;
    }
    return canonical_json::typedScalar(value,schema.name)||reject(out,"IR_RECORD_LITERAL_TYPE","Record scalar JSON does not match its exact declared width/type; no numeric projection is used.");
}
}
std::optional<binding::TypeRef> canonicalRecordTransportType(std::string_view text,CanonicalRecordDiagnostic *out) {
    if(out)*out={};if(text.empty()||text.size()>=65536||!boundedSyntaxDepth(text)){reject(out,"IR_RECORD_RESOURCE_LIMIT","Record type descriptor must remain below 65 536 bytes and depth 32 before parsing.",DocumentDiagnosticDomain::Control);return {};}
    auto parsed=binding::parseType(text);if(!parsed.valid()||parsed.legacy||parsed.type.canonical()!=text||parsed.type.kind!=binding::TypeKind::Cluster){reject(out,"IR_RECORD_TYPE","An explicit canonical cluster type with exact ordered field IDs is required.");return {};}
    Budget budget;if(!type(parsed.type,0,budget,out))return {};return std::move(parsed.type);
}
bool canonicalRecordLiteralValid(const binding::TypeRef &schema,const source::JsonValue &value,CanonicalRecordDiagnostic *out) {
    if(out)*out={};if(schema.kind!=binding::TypeKind::Cluster)return reject(out,"IR_RECORD_TYPE","The immutable Record literal needs its declared Record schema.");
    Budget types;if(!type(schema,0,types,out))return false;
    Budget values;return literal(schema,value,0,values,out);
}
bool canonicalRecordOutputIdentity(std::string_view value,std::size_t maximumBytes) noexcept {
    return !value.empty()&&value.size()<=std::min<std::size_t>(maximumBytes,1024)&&value!="record"&&std::all_of(value.begin(),value.end(),[](unsigned char c){return (c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='.'||c=='_'||c==':'||c=='-';});
}
}
