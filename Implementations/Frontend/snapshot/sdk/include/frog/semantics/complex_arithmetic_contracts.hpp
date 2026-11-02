#pragma once
#include "frog/semantics/complex_numeric_contracts.hpp"
#include <array>

namespace frog::runtime::complex_numeric::arithmetic {
enum class Kind {Add,Subtract,Multiply,Divide,Increment,Decrement,Negate,Absolute,
    RoundNearest,Floor,Ceil,Square,SquareRoot,Reciprocal,Sign};
inline constexpr std::string_view profile="frog.studio.numeric.complex-arithmetic-finite@1";
struct Spec {std::string_view catalogId,operationId;Kind kind;std::span<const std::string_view> inputs;unsigned minimumInputs=0;};
inline constexpr std::string_view unary[]{"in"},binary[]{"a","b"},variadic[]{"a","b","input_3","input_4","input_5","input_6","input_7","input_8","input_9","input_10","input_11","input_12","input_13","input_14","input_15","input_16"};
inline constexpr std::array<Spec,15> specs{{
    {"numeric.add",numeric_aggregate::addOperationId,Kind::Add,variadic,2},
    {"numeric.subtract","frog.studio.numeric.subtract.owned.v1",Kind::Subtract,binary},
    {"numeric.multiply","frog.studio.numeric.multiply.owned.v1",Kind::Multiply,variadic,2},
    {"numeric.divide","frog.studio.numeric.divide.owned.v1",Kind::Divide,binary},
    {"numeric.increment","frog.studio.numeric.increment.owned.v1",Kind::Increment,unary},
    {"numeric.decrement","frog.studio.numeric.decrement.owned.v1",Kind::Decrement,unary},
    {"numeric.negate","frog.studio.numeric.negate.owned.v1",Kind::Negate,unary},
    {"numeric.absolute_value","frog.studio.numeric.absolute_value.owned.v1",Kind::Absolute,unary},
    {"numeric.round_nearest","frog.studio.numeric.round_nearest.owned.v1",Kind::RoundNearest,unary},
    {"numeric.round_negative_infinity","frog.studio.numeric.round_negative_infinity.owned.v1",Kind::Floor,unary},
    {"numeric.round_positive_infinity","frog.studio.numeric.round_positive_infinity.owned.v1",Kind::Ceil,unary},
    {"numeric.square","frog.studio.numeric.square.owned.v1",Kind::Square,unary},
    {"numeric.square_root","frog.studio.numeric.square_root.owned.v1",Kind::SquareRoot,unary},
    {"numeric.reciprocal","frog.studio.numeric.reciprocal.owned.v1",Kind::Reciprocal,unary},
    {"numeric.sign","frog.studio.numeric.sign.owned.v1",Kind::Sign,unary}
}};
[[nodiscard]] inline constexpr const Spec *spec(std::string_view id)noexcept {for(const auto &row:specs)if(row.operationId==id||row.catalogId==id)return &row;return nullptr;}
[[nodiscard]] inline constexpr bool complexType(std::string_view type)noexcept{return type=="c32"||type=="c64";}
[[nodiscard]] inline bool hasComplex(std::span<const rt::ValueType> types)noexcept{for(const auto &type:types)if(complexType(type.canonicalName))return true;return false;}
// Pure metadata-only admission: this inline function can be reused by the
// autonomous Source helper without importing a Runtime/LLVM function symbol.
[[nodiscard]] inline bool signatureValid(std::string_view id,std::span<const rt::ValueType> inputs,
    std::span<const rt::ValueType> outputs,std::string *message=nullptr)noexcept{
    const auto reject=[&](std::string_view why){if(message)*message=why;return false;};
    try{const auto *row=spec(id);if(!row||outputs.size()!=1||inputs.size()>row->inputs.size()||
            (row->minimumInputs?inputs.size()<row->minimumInputs:inputs.size()!=row->inputs.size()))return reject("Complex arithmetic lost its actual semantic connector inventory.");
        bool c64=false,seen=false;
        for(const auto &type:inputs){if(type!=rt::valueTypeFromCanonicalName(type.canonicalName)||(!complexType(type.canonicalName)&&!studio_numeric::supportedPhysicalType(type.canonicalName)))return reject("This complex arithmetic leaf still lacks its exact declared physical carrier.");seen=seen||complexType(type.canonicalName);c64=c64||type.canonicalName=="c64"||type.canonicalName=="f64";}
        if(!seen)return reject("Real-only arithmetic remains owned by its existing scalar provider.");
        if(row->inputs.size()==1&&!complexType(inputs.front().canonicalName))return reject("The complex unary kernel requires its actual complex operand.");
        const auto result=rt::valueTypeFromCanonicalName(row->kind==Kind::Absolute?(c64?"f64":"f32"):(c64?"c64":"c32"));
        return outputs.front()==result?true:reject("Complex arithmetic result must preserve the exact resolver precision and Abs real projection.");
    }catch(...){return reject("Complex arithmetic metadata validation failed without admitting a partial signature.");}
}
}
