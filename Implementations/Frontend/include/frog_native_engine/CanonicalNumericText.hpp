#pragma once
// Source-only signature adaptation of actual unary Numeric identities. No
// Runtime execution, JSON parser, prepared value or LLVM symbol is called here.
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog/semantics/numeric_text_contracts.hpp"

namespace frog::native_engine {
inline const runtime::numeric_text::Contract *canonicalNumericTextContract(std::string_view id) noexcept {
    if(const auto *direct=runtime::numeric_text::contract(id))return direct->aggregate?direct:nullptr;
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    const auto *c=runtime::numeric_text::contract(id);return c&&c->aggregate?c:nullptr;
}
inline std::string_view canonicalNumericTextMigrationIdentity(std::string_view id) noexcept {
    const auto *c=canonicalNumericTextContract(id);return c?c->id:std::string_view{};
}
namespace numeric_text_source_detail {
inline bool plain(const binding::TypeRef &t){return t.kind==binding::TypeKind::Primitive&&binding::fullTypeEqual(t,binding::TypeRef::primitive(t.name));}
inline bool types(const runtime::numeric_text::Contract &c,const binding::TypeRef &input,const binding::TypeRef &output,std::size_t &visited,unsigned depth=0){
    using K=binding::TypeKind;if(depth>32||++visited>4096)return false;
    if(c.kind!=runtime::numeric_text::Kind::Automatic){
        if(input.kind==K::Array)return output.kind==K::Array&&input.rank==output.rank&&input.children.size()==1&&output.children.size()==1&&types(c,input.children[0],output.children[0],visited,depth+1);
        if(input.kind==K::Cluster){
            if(output.kind!=K::Cluster||input.children.size()!=output.children.size()||input.fieldIds!=output.fieldIds||input.fieldNames!=output.fieldNames)return false;
            for(std::size_t i=0;i<input.children.size();++i)if(!types(c,input.children[i],output.children[i],visited,depth+1))return false;return true;
        }
    }
    if(!plain(input)||!plain(output)||output.name!=runtime::numeric_text::outputLeaf(c))return false;
    if(runtime::numeric_text::parses(c.kind))return input.name=="string";
    return runtime::numeric_text::numeric(input.name)||(c.kind==runtime::numeric_text::Kind::Automatic&&(input.name=="bool"||input.name=="string"||input.name=="path"));
}
}
inline bool canonicalNumericTextSignatureValid(std::string_view id,std::span<const CanonicalNumericAggregatePort> ports,
    std::string *message=nullptr,std::string *at=nullptr) noexcept {
    const auto reject=[&](const char *why,const char *port=""){if(message)*message=why;if(at)*at=port;return false;};
    try {
        const auto *c=canonicalNumericTextContract(id);if(!c||ports.size()!=2)return reject("Numeric text requires the actual unary value/result signature.");
        const CanonicalNumericAggregatePort *input=nullptr,*output=nullptr;
        for(const auto &p:ports){if(!p.type.known())return reject("Numeric text cannot admit an unknown full Source type.");
            if(p.input){if(input||p.id!="value"||!p.connected)return reject("Numeric text requires one driven value input.","value");input=&p;}
            else{if(output||p.id!="result")return reject("Numeric text requires one result output.","result");output=&p;}}
        std::size_t visited=0;if(!input||!output||!numeric_text_source_detail::types(*c,input->type,output->type,visited))return reject("Numeric text must preserve its declared primitive mapping and exact aggregate shape; nominal types cannot be erased.");return true;
    }catch(...){return reject("Numeric text Source validation refused an incomplete signature.");}
}
}
