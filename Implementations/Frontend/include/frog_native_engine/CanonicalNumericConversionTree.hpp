#pragma once
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog_native_engine/CanonicalNumericRing.hpp"
#include "frog/semantics/studio_numeric_promotion.hpp"
#include <functional>
namespace frog::native_engine {
inline bool canonicalNumericConversionTreeSignatureValid(std::string_view operation,
    std::span<const CanonicalNumericAggregatePort> ports,std::string *message=nullptr,std::string *at=nullptr) noexcept {
    const auto reject=[&](std::string_view why,std::string_view port={}){if(message)*message=why;if(at)*at=port;return false;};
    try {
        const auto target=canonicalNumericConversionTarget(operation);if(target.empty()||ports.size()!=2||ports[0].id!="value"||!ports[0].input||!ports[0].connected||ports[1].id!="result"||ports[1].input)return reject("Explicit Numeric conversion requires its actual value/result connectors.");
        bool complexProjectionUnqualified=false,ringMetadataInvalid=false;std::size_t nodes=0;std::function<std::optional<binding::TypeRef>(const binding::TypeRef&,unsigned)> convert;
        convert=[&](const binding::TypeRef &type,unsigned depth)->std::optional<binding::TypeRef>{
            if(depth>32||++nodes>4096||!type.known())return {};
            if(type.kind==binding::TypeKind::Array||type.kind==binding::TypeKind::Cluster){
                if(type.children.empty()||(type.kind==binding::TypeKind::Array&&(type.children.size()!=1||type.rank<1||type.rank>16)))return {};
                auto output=type;for(std::size_t i=0;i<type.children.size();++i){auto child=convert(type.children[i],depth+1);if(!child)return {};output.children[i]=std::move(*child);}return output;
            }
            if(type.kind==binding::TypeKind::Enum){if(type.children.size()!=1||type.choices.empty()||type.children.front().name.empty()||type.children.front().name.front()!='u'||!runtime::studio_numeric::supportedPhysicalType(type.children.front().name))return {};return binding::TypeRef::primitive(std::string(target));}
            if(type.kind!=binding::TypeKind::Primitive)return {};
            if(type.name=="timestamp"){if(target!="f64"&&target!="i64"&&target!="u32"&&target!="u64")return {};return binding::TypeRef::primitive(std::string(target));}
            if(type.name=="c32"||type.name=="c64"){complexProjectionUnqualified=true;return {};}
            if(type.choiceKind==binding::ChoiceKind::Ring){
                if(!canonicalNumericRingMetadataValid(type)){ringMetadataInvalid=true;return {};}
            }else if(!runtime::studio_numeric::supportedPhysicalType(type.name)||!binding::fullTypeEqual(type,binding::TypeRef::primitive(type.name)))return {};
            return binding::TypeRef::primitive(std::string(target));
        };
        const auto expected=convert(ports[0].type,0);if(!expected){
            if(complexProjectionUnqualified)return reject("The Complex carrier exists, but its projection/conversion rule to a real or integer destination is not qualified.","value");
            if(ringMetadataInvalid)return reject("The Ring choices must be exactly representable in its actual supported numeric representation; no choice is silently rounded.","value");
            return reject("One source leaf still lacks an exact Numeric conversion carrier or complete nominal descriptor.","value");
        }
        return binding::fullTypeEqual(*expected,ports[1].type)?true:reject("Numeric conversion must preserve all ranks, fields and names while replacing only its explicitly converted leaves.","result");
    }catch(...){return reject("Numeric conversion Source validation failed without erasing a type.");}
}
}
