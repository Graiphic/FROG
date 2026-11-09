// SPDX-License-Identifier: Apache-2.0
#pragma once
// Source/FIR admission only. Mutable accumulator state belongs to a realization.
#include "frog/semantics/numeric_scalar_palette_contracts.hpp"

namespace frog::runtime::numeric_aggregate::accumulate {
inline constexpr std::string_view profile="frog.studio.numeric.multiply_accumulate.instance-owned@1";
inline constexpr std::string_view operationId="frog.studio.numeric.multiply_accumulate.instance-owned.v1";
inline constexpr std::array<std::string_view,4> inputIds{"x","y","initial_value","initialize"};
inline bool supports(std::string_view id)noexcept{return id==operationId;}
using Failure=scalar_palette::Failure;
[[noreturn]] inline void fail(std::string_view code,std::string_view message,std::string_view port={}) {
    throw Failure{"native.numeric.multiply_accumulate."+std::string(code),std::string(message),std::string(port)};
}
template<class Port>struct Signature {
    std::array<const Port*,4> inputs{};
    const Port *output=nullptr;
    rt::ValueType type;
};
template<class Port>inline Signature<Port> signature(std::string_view id,std::span<const Port> ports) {
    if(!supports(id)||ports.size()!=5)fail("signature_invalid","Multiply Accumulate requires its exact operation and five connectors.");
    Signature<Port> result;
    for(const auto &p:ports){
        if(p.type!=rt::valueTypeFromCanonicalName(p.type.canonicalName)||
            p.fullTypeJson.empty()||p.fullTypeJson!=scalar_palette::scalarSchema(p.type))
            fail("schema_invalid","Multiply Accumulate requires complete plain scalar schemas; aggregate and nominal types are not erased.",p.id);
        if(!p.input&&p.id=="result"&&!result.output)result.output=&p;
        else {
            const auto found=std::find(inputIds.begin(),inputIds.end(),p.id);
            if(!p.input||found==inputIds.end()||result.inputs[found-inputIds.begin()])
                fail("signature_invalid","Multiply Accumulate received an unknown or duplicate connector.",p.id);
            result.inputs[found-inputIds.begin()]=&p;
        }
    }
    if(!result.output||std::any_of(result.inputs.begin(),result.inputs.end(),[](auto p){return !p;}))
        fail("signature_invalid","Multiply Accumulate lost a declared connector.");
    if(result.inputs[3]->type!=rt::valueTypeFromCanonicalName("bool"))
        fail("signature_invalid","initialize must be a true Boolean scalar.","initialize");
    std::string common;
    for(std::size_t i=0;i<3;++i){const auto &p=*result.inputs[i];
        if(!studio_numeric::supportedPhysicalType(p.type.canonicalName))
            fail("type_unsupported","Multiply Accumulate accepts supported real/integer scalars only.",p.id);
        common=common.empty()?p.type.canonicalName:std::string(studio_numeric::binaryPromotionType(common,p.type.canonicalName));
        if(common.empty())fail("type_unsupported","The input promotion has no supported exact physical representation.",p.id);
    }
    result.type=rt::valueTypeFromCanonicalName(common);
    if(!studio_numeric::supportedPhysicalType(common)||result.output->type!=result.type)
        fail("signature_invalid","The result and accumulator must retain the exact common numeric type.","result");
    return result;
}
template<class Port>inline bool validateSignature(std::string_view id,std::span<const Port> ports,
    std::string *message=nullptr,std::string *at=nullptr)noexcept {
    try{static_cast<void>(signature(id,ports));return true;}
    catch(const Failure &e){if(message)*message=e.message;if(at)*at=e.port;}
    catch(...){if(message)*message="Multiply Accumulate signature validation failed without admitting a partial type.";}
    return false;
}
}
