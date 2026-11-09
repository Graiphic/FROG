#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "frog_native_engine/ApplicationControlSourceCodec.hpp"
#include "CanonicalJsonSupport.hpp"
namespace frog::native_engine {
bool canonicalObjectReferenceFullTypeEqual(const binding::TypeRef &a,const binding::TypeRef &b) {
    std::vector<std::pair<const binding::TypeRef *,const binding::TypeRef *>> work{{&a,&b}};
    while(!work.empty()) {
        const auto [x,y]=work.back();work.pop_back();
        if(*x!=*y||x->fieldNames!=y->fieldNames||x->choiceKind!=y->choiceKind||x->choices!=y->choices)return false;
        for(std::size_t i=0;i<x->children.size();++i)work.emplace_back(&x->children[i],&y->children[i]);
    }
    return true;
}
binding::TypeRef canonicalObjectReferencePhysicalType(const binding::TypeRef &type) {
    if(type.kind==binding::TypeKind::Enum) {
        if(type.children.size()!=1||!type.children.front().integer())return {};
        return canonicalObjectReferencePhysicalType(type.children.front());
    }
    auto result=type;
    for(auto &child:result.children)child=canonicalObjectReferencePhysicalType(child);
    return result;
}
source::JsonValue canonicalObjectReferenceWriteType(const binding::TypeRef &type) {
    return application_control_source::writeType(type);
}
std::optional<binding::TypeRef> canonicalObjectReferenceReadType(const source::JsonValue &value,CanonicalObjectReferenceDiagnostic *diagnostic) {
    std::string why;const auto decoded=application_control_source::readType(value,&why);
    if(!decoded){if(diagnostic)*diagnostic={"canonical.object_reference.schema_invalid",std::move(why),{},false};return {};}
    if(!canonical_json::exact(value,application_control_source::writeType(*decoded))){if(diagnostic)*diagnostic={"canonical.object_reference.schema_invalid","Full schema metadata does not roundtrip exactly.",{},false};return {};}
    return decoded;
}
}
