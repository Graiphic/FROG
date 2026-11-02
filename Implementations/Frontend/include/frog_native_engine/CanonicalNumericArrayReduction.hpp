#pragma once
// Source-only schema admission. No Runtime descriptor, value walker,
// WorkBudget, LLVM or editor callback is required by this header.
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog/semantics/studio_numeric_promotion.hpp"
#include <functional>

namespace frog::native_engine {
inline constexpr std::string_view canonicalNumericArraySumId="frog.studio.numeric.add_array_elements.owned.v1";
inline constexpr std::string_view canonicalNumericArrayProductId="frog.studio.numeric.multiply_array_elements.owned.v1";
inline std::string_view canonicalNumericArrayReductionMigrationIdentity(std::string_view id)noexcept{
    if(id==canonicalNumericArraySumId||id==canonicalNumericArrayProductId)return id;
    if(id=="numeric.add_array_elements"||id=="frog.studio.numeric.add_array_elements")return canonicalNumericArraySumId;
    if(id=="numeric.multiply_array_elements"||id=="frog.studio.numeric.multiply_array_elements")return canonicalNumericArrayProductId;
    return {};
}
inline bool canonicalNumericArrayReductionSignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports,std::string *message=nullptr,std::string *at=nullptr)noexcept{
    const auto refuse=[&](std::string_view why,std::string_view port={}){if(message)*message=why;if(at)*at=port;return false;};
    try{
        if((id!=canonicalNumericArraySumId&&id!=canonicalNumericArrayProductId)||ports.size()!=2)
            return refuse("Numeric Array reduction requires its exact private identity and value/result connectors.");
        const CanonicalNumericAggregatePort *input=nullptr,*output=nullptr;
        for(const auto &p:ports){if(p.input&&p.id=="value"&&p.connected&&!input)input=&p;
            else if(!p.input&&p.id=="result"&&!output)output=&p;
            else return refuse("Numeric Array reduction received an unknown, duplicate or undriven connector.",p.id);}
        if(!input||!output||input->type.kind!=binding::TypeKind::Array||input->type.children.size()!=1||input->type.rank<1||input->type.rank>16||!input->type.known())
            return refuse("Numeric Array reduction requires one exact bounded-rank Array.","value");
        const auto &element=input->type.children.front();
        if(!binding::fullTypeEqual(element,output->type))
            return refuse("Numeric Array reduction must retain its complete element result schema, fields and names.","result");
        // There is no promotion between elements: every cell has this ONE
        // exact common element type. Share the physical-type whitelist with
        // the arithmetic kernels; do not recompute a global F64 leaf type.
        std::size_t nodes=0;std::function<bool(const binding::TypeRef&,unsigned)> supported;
        supported=[&](const binding::TypeRef &type,unsigned depth){
            if(depth>32||++nodes>4096||!type.known())return false;
            if(type.kind==binding::TypeKind::Primitive)return (runtime::studio_numeric::supportedPhysicalType(type.name)||type.name=="c32"||type.name=="c64")&&
                binding::fullTypeEqual(type,binding::TypeRef::primitive(type.name));
            if(type.kind==binding::TypeKind::Array){if(type.children.size()!=1||type.children.front().kind==binding::TypeKind::Array)return false;
                return supported(type.children.front(),depth+1);}
            if(type.kind==binding::TypeKind::Cluster){if(type.children.empty()||type.children.size()!=type.fieldIds.size())return false;
                for(const auto &child:type.children)if(!supported(child,depth+1))return false;return true;}
            return false;
        };
        if(!supported(element,0))return refuse("Numeric Array reduction has no admitted exact primitive carrier for one of its numeric leaves; Enum/Bool/fixed/extended are not erased.","value");
        return true;
    }catch(...){return refuse("Numeric Array reduction schema validation failed without admitting a partial schema.");}
}
}
