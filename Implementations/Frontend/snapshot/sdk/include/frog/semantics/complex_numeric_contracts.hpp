#pragma once
#include "frog/semantics/numeric_aggregate_contracts.hpp"

namespace frog::runtime::complex_numeric {
namespace rt=sdk::runtime;
inline constexpr std::string_view profile="frog.studio.numeric.complex-finite@1";
// Ports remain those of the real catalogue, including both outputs.
struct Contract {std::string_view catalogId,operationId;std::span<const std::string_view> inputs,outputs;};
namespace contract_detail {
inline constexpr std::string_view z[]{"z"},rtheta[]{"r","theta"},reim[]{"re","im"},value[]{"value"};
inline constexpr std::string_view result[]{"result"};
inline constexpr Contract rows[]{
    {"numeric.complex.conjugate","frog.studio.numeric.complex.conjugate",z,result},
    {"numeric.complex.polar_to_complex","frog.studio.numeric.complex.polar_to_complex",rtheta,z},
    {"numeric.complex.complex_to_polar","frog.studio.numeric.complex.complex_to_polar",z,rtheta},
    {"numeric.complex.real_imaginary_to_complex","frog.studio.numeric.complex.real_imaginary_to_complex",reim,z},
    {"numeric.complex.complex_to_real_imaginary","frog.studio.numeric.complex.complex_to_real_imaginary",z,reim},
    {"numeric.complex.real_imaginary_to_polar","frog.studio.numeric.complex.real_imaginary_to_polar",reim,rtheta},
    {"numeric.complex.polar_to_real_imaginary","frog.studio.numeric.complex.polar_to_real_imaginary",rtheta,reim},
    {"numeric.conversion.to_csg","frog.studio.numeric.conversion.to_csg",value,result},
    {"numeric.conversion.to_cdb","frog.studio.numeric.conversion.to_cdb",value,result}
};
inline bool complex(const rt::ValueType &t){return t.canonicalName=="c32"||t.canonicalName=="c64";}
inline bool real(const rt::ValueType &t){return studio_numeric::supportedPhysicalType(t.canonicalName);}
inline std::string_view realType(const rt::ValueType &t){if(t.canonicalName=="c32")return "f32";if(t.canonicalName=="c64")return "f64";return t.canonicalName;}
inline bool fromComplex(const Contract &c){return c.inputs.size()==1&&c.inputs.front()=="z";}
inline bool conversion(const Contract &c){return c.inputs.front()=="value";}
inline bool complexOutput(const Contract &c){return c.catalogId=="numeric.complex.conjugate"||c.outputs.front()=="z"||conversion(c);}
inline std::string expectedType(const Contract &c,std::span<const rt::ValueType> inputs) {
    if(c.catalogId=="numeric.conversion.to_csg")return "c32";
    if(c.catalogId=="numeric.conversion.to_cdb")return "c64";
    auto common=realType(inputs.front());
    if(inputs.size()==2)common=studio_numeric::binaryPromotionType(common,realType(inputs[1]));
    return std::string(complexOutput(c)?(common=="f32"?"c32":"c64"):(common=="f32"?"f32":"f64"));
}
}
using namespace contract_detail;
inline std::span<const Contract> contracts() noexcept{return rows;}
inline const Contract *contract(std::string_view id) noexcept {
    for(const auto &row:rows)if(row.catalogId==id||row.operationId==id)return &row;return nullptr;
}
inline bool signatureValid(std::string_view id,std::span<const rt::ValueType> inputs,
    std::span<const rt::ValueType> outputs,std::string *message=nullptr) noexcept {
    const auto reject=[&](std::string_view why){if(message)*message=why;return false;};
    try {
        const auto *c=contract(id);if(!c||inputs.size()!=c->inputs.size()||outputs.size()!=c->outputs.size())return reject("Complex operation has an unknown identity or wrong semantic arity.");
        for(const auto &t:inputs) {
            if(t!=rt::valueTypeFromCanonicalName(t.canonicalName))return reject("Complex leaf input type is not canonical.");
            if(fromComplex(*c)) {if(t.canonicalName!="f32"&&t.canonicalName!="f64"&&!complex(t))return reject("This complex decomposition accepts only f32/f64/c32/c64 leaves.");}
            else if(conversion(*c)) {if(!real(t)&&!complex(t))return reject("This finite complex conversion has no exact source value carrier.");}
            else if(!real(t))return reject("Rectangular/polar inputs require real numeric leaves.");
        }
        const auto expected=rt::valueTypeFromCanonicalName(expectedType(*c,inputs));
        for(const auto &t:outputs)if(t!=expected)return reject("Complex output type differs from the actual authoring resolver's precision.");
        return true;
    }catch(...){return reject("Complex signature validation failed without admitting a partial descriptor.");}
}
} // namespace frog::runtime::complex_numeric
