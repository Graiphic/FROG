#pragma once
// The same scalar connector admission is used by Source and Runtime.
// Value arithmetic remains in the private Runtime provider.
#include "frog/semantics/numeric_aggregate_contracts.hpp"
#include <algorithm>
#include <array>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace frog::runtime::numeric_aggregate::scalar_palette {
namespace numeric=studio_numeric;
enum class Kind {QuotientRemainder,Sign,Scale,MaxMin,InRange,LogicalShift,Rotate,
    CarryLeft,CarryRight,Split,Join,SwapBytes,SwapWords,MantissaExponent,
    Subtract,Multiply,Divide,Increment,Decrement,Negate,Absolute,Round,Floor,Ceil,
    Square,SquareRoot,Reciprocal};
struct Spec {
    std::string_view id,catalog;
    Kind kind;
    std::array<std::string_view,16> inputs;
    std::array<std::string_view,2> outputs;
    bool includeLower=false,includeUpper=false;
    unsigned minimumInputs=0; // zero means the exact fixed input list
};
inline constexpr std::array<Spec,30> specs{{
    {"frog.studio.numeric.quotient_remainder.owned.v1","numeric.quotient_remainder",Kind::QuotientRemainder,{"x","y"},{"quotient","remainder"}},
    {"frog.studio.numeric.sign.owned.v1","numeric.sign",Kind::Sign,{"in"},{"result"}},
    {"frog.studio.numeric.scale_power_two.owned.v1","numeric.scale_power_two",Kind::Scale,{"x","n"},{"result"}},
    {"frog.studio.numeric.max_min.owned.v1","numeric.max_min",Kind::MaxMin,{"x","y"},{"max","min"}},
    {"frog.studio.numeric.in_range.open-open.owned.v1","numeric.in_range",Kind::InRange,{"upper_limit","x","lower_limit"},{"coerced","in_range"},false,false},
    {"frog.studio.numeric.in_range.closed-open.owned.v1","numeric.in_range",Kind::InRange,{"upper_limit","x","lower_limit"},{"coerced","in_range"},true,false},
    {"frog.studio.numeric.in_range.open-closed.owned.v1","numeric.in_range",Kind::InRange,{"upper_limit","x","lower_limit"},{"coerced","in_range"},false,true},
    {"frog.studio.numeric.in_range.closed-closed.owned.v1","numeric.in_range",Kind::InRange,{"upper_limit","x","lower_limit"},{"coerced","in_range"},true,true},
    {"frog.studio.numeric.logical_shift.owned.v1","numeric.data_manipulation.logical_shift",Kind::LogicalShift,{"x","y"},{"result"}},
    {"frog.studio.numeric.rotate.owned.v1","numeric.data_manipulation.rotate",Kind::Rotate,{"x","y"},{"result"}},
    {"frog.studio.numeric.rotate_left_with_carry.owned.v1","numeric.data_manipulation.rotate_left_with_carry",Kind::CarryLeft,{"value_in","carry"},{"value_out","carry_out"}},
    {"frog.studio.numeric.rotate_right_with_carry.owned.v1","numeric.data_manipulation.rotate_right_with_carry",Kind::CarryRight,{"value_in","carry"},{"value_out","carry_out"}},
    {"frog.studio.numeric.split_number.owned.v1","numeric.data_manipulation.split_number",Kind::Split,{"x"},{"hi","lo"}},
    {"frog.studio.numeric.join_numbers.owned.v1","numeric.data_manipulation.join_numbers",Kind::Join,{"hi","lo"},{"result"}},
    {"frog.studio.numeric.swap_bytes.owned.v1","numeric.data_manipulation.swap_bytes",Kind::SwapBytes,{"x"},{"result"}},
    {"frog.studio.numeric.swap_words.owned.v1","numeric.data_manipulation.swap_words",Kind::SwapWords,{"x"},{"result"}},
    {"frog.studio.numeric.mantissa_exponent.owned.v1","numeric.data_manipulation.mantissa_exponent",Kind::MantissaExponent,{"number"},{"mantissa","exponent"}},
    {"frog.studio.numeric.subtract.owned.v1","numeric.subtract",Kind::Subtract,{"a","b"},{"result"}},
    {"frog.studio.numeric.multiply.owned.v1","numeric.multiply",Kind::Multiply,{"a","b","input_3","input_4","input_5","input_6","input_7","input_8","input_9","input_10","input_11","input_12","input_13","input_14","input_15","input_16"},{"result"},false,false,2},
    {"frog.studio.numeric.divide.owned.v1","numeric.divide",Kind::Divide,{"a","b"},{"result"}},
    {"frog.studio.numeric.increment.owned.v1","numeric.increment",Kind::Increment,{"in"},{"result"}},
    {"frog.studio.numeric.decrement.owned.v1","numeric.decrement",Kind::Decrement,{"in"},{"result"}},
    {"frog.studio.numeric.negate.owned.v1","numeric.negate",Kind::Negate,{"in"},{"result"}},
    {"frog.studio.numeric.absolute_value.owned.v1","numeric.absolute_value",Kind::Absolute,{"in"},{"result"}},
    {"frog.studio.numeric.round_nearest.owned.v1","numeric.round_nearest",Kind::Round,{"in"},{"result"}},
    {"frog.studio.numeric.round_negative_infinity.owned.v1","numeric.round_negative_infinity",Kind::Floor,{"in"},{"result"}},
    {"frog.studio.numeric.round_positive_infinity.owned.v1","numeric.round_positive_infinity",Kind::Ceil,{"in"},{"result"}},
    {"frog.studio.numeric.square.owned.v1","numeric.square",Kind::Square,{"in"},{"result"}},
    {"frog.studio.numeric.square_root.owned.v1","numeric.square_root",Kind::SquareRoot,{"in"},{"result"}},
    {"frog.studio.numeric.reciprocal.owned.v1","numeric.reciprocal",Kind::Reciprocal,{"in"},{"result"}}
}};
inline const Spec *find(std::string_view id) noexcept {
    const auto it=std::find_if(specs.begin(),specs.end(),[&](const auto &s){return s.id==id;});
    return it==specs.end()?nullptr:&*it;
}
inline bool supports(std::string_view id) noexcept {return find(id)!=nullptr;}
struct Failure {std::string code,message,port;};
[[noreturn]] inline void fail(std::string_view code,std::string_view message,std::string_view port={}) {
    throw Failure{"native.numeric.palette."+std::string(code),std::string(message),std::string(port)};
}
inline std::string scalarSchema(const rt::ValueType &type) {
    // Header-only schema admission keeps the autonomous frontend free of a
    // link-time dependency on the Runtime implementation of plainTypeSchema.
    if(type!=rt::valueTypeFromCanonicalName(type.canonicalName) ||
        (type.canonicalName!="bool"&&!numeric::supportedPhysicalType(type.canonicalName)))return {};
    return "{\"canonical\":\""+type.canonicalName+"\",\"children\":[],\"choice_kind\":\"none\",\"choices\":[],\"field_names\":[]}";
}
inline bool isInteger(const rt::ValueType &type) noexcept {
    const auto t=numeric::physicalType(type.canonicalName);
    return t.kind==numeric::PhysicalType::Kind::Signed || t.kind==numeric::PhysicalType::Kind::Unsigned;
}
inline std::size_t count(const auto &ids) noexcept {
    return static_cast<std::size_t>(std::count_if(ids.begin(),ids.end(),[](auto id){return !id.empty();}));
}
template<class Port> struct SignatureFor {
    const Spec *spec=nullptr;
    std::array<const Port *,16> inputs{};
    std::array<const Port *,2> outputs{};
    rt::ValueType common;
    std::size_t inputCount=0;
};
template<class Port>
inline SignatureFor<Port> signature(std::string_view operation,std::span<const Port> ports) {
    SignatureFor<Port> s;s.spec=find(operation);
    if(!s.spec)fail("operation_unavailable","The scalar Numeric extension does not own this operation.");
    const auto &spec=*s.spec;
    const auto actualInputCount=ports.size()-count(spec.outputs);
    if((!spec.minimumInputs && actualInputCount!=count(spec.inputs)) ||
        (spec.minimumInputs && (actualInputCount<spec.minimumInputs || actualInputCount>count(spec.inputs))))
        fail("signature_invalid","The scalar Numeric operation lost or acquired a connector.");
    for(const auto &port:ports) {
        if(port.type!=rt::valueTypeFromCanonicalName(port.type.canonicalName) ||
            scalarSchema(port.type).empty() || port.fullTypeJson!=scalarSchema(port.type))
            fail("schema_invalid","Scalar Numeric requires complete plain physical numeric or declared Boolean schemas.",port.id);
        bool found=false;
        if(port.input)for(std::size_t i=0;i<spec.inputs.size();++i)if(!spec.inputs[i].empty()&&port.id==spec.inputs[i]) {
            if(s.inputs[i])fail("signature_invalid","Scalar Numeric received a duplicate input.",port.id);
            if(!port.connected && !((spec.kind==Kind::CarryLeft || spec.kind==Kind::CarryRight)&&port.id=="carry"))
                fail("signature_invalid","Scalar Numeric requires its driven numeric operands.",port.id);
            s.inputs[i]=&port;++s.inputCount;found=true;break;
        }
        if(!port.input)for(std::size_t i=0;i<spec.outputs.size();++i)if(!spec.outputs[i].empty()&&port.id==spec.outputs[i]) {
            if(s.outputs[i])fail("signature_invalid","Scalar Numeric received a duplicate output.",port.id);
            s.outputs[i]=&port;found=true;break;
        }
        if(!found)fail("signature_invalid","Scalar Numeric received an undeclared connector.",port.id);
    }
    const auto expect=[&](const Port *port,const rt::ValueType &type) {
        if(!port || port->type!=type)fail("signature_invalid","Scalar Numeric result must preserve the exact declared physical type.",port?port->id:"");
    };
    const auto numericInput=[&](std::size_t index) {
        const auto *p=s.inputs[index];
        if(!p || !numeric::supportedPhysicalType(p->type.canonicalName))
            fail("signature_invalid","This connector requires a plain real or integer scalar.",p?p->id:"");
    };
    if(s.inputCount!=actualInputCount)fail("signature_invalid","Scalar Numeric lost a declared input.");
    if(!s.inputs[0] || (spec.minimumInputs&&!s.inputs[1]))fail("signature_invalid","Scalar Numeric lost a required first operand.");
    for(std::size_t i=0;i<s.inputs.size();++i)if(s.inputs[i]) {
        if((spec.kind==Kind::CarryLeft || spec.kind==Kind::CarryRight)&&i==1) {
            expect(s.inputs[i],rt::valueTypeFromCanonicalName("bool"));
        } else numericInput(i);
    }
    s.common=s.inputs[0]->type;
    if(spec.kind==Kind::QuotientRemainder || spec.kind==Kind::MaxMin || spec.kind==Kind::InRange || spec.kind==Kind::Join ||
        spec.kind==Kind::Subtract || spec.kind==Kind::Multiply || spec.kind==Kind::Divide) {
        std::string promoted;
        for(const auto *p:s.inputs)if(p)promoted=promoted.empty()?p->type.canonicalName:
            std::string(numeric::binaryPromotionType(promoted,p->type.canonicalName));
        // Divide changes a logical integral fold into DBL before allocating a
        // carrier. I64/U64 may therefore have a logical I128 common type while
        // requiring no I128 value at all; each exact input converts to DBL.
        if(spec.kind==Kind::Divide){const auto folded=numeric::physicalType(promoted);
            if(folded.kind==numeric::PhysicalType::Kind::Signed||folded.kind==numeric::PhysicalType::Kind::Unsigned)promoted="f64";}
        if(!numeric::supportedPhysicalType(promoted))
            fail("carrier_unavailable","The complete Numeric promotion requires a value carrier not admitted by this profile.");
        s.common=rt::valueTypeFromCanonicalName(promoted);
    }
    if((spec.kind==Kind::Divide || spec.kind==Kind::SquareRoot || spec.kind==Kind::Reciprocal)&&isInteger(s.common))
        s.common=rt::valueTypeFromCanonicalName("f64");
    if((spec.kind==Kind::Negate||spec.kind==Kind::Absolute)&&
        numeric::physicalType(s.inputs[0]->type.canonicalName).kind==numeric::PhysicalType::Kind::Unsigned)
        fail("signature_invalid","The actual Numeric authoring contract rejects unsigned Negate and Absolute.");
    if(spec.kind==Kind::LogicalShift || spec.kind==Kind::Rotate || spec.kind==Kind::CarryLeft ||
        spec.kind==Kind::CarryRight || spec.kind==Kind::Split || spec.kind==Kind::Join ||
        spec.kind==Kind::SwapBytes || spec.kind==Kind::SwapWords) {
        if(!isInteger(s.inputs[0]->type) || s.inputs[0]->type.bitWidth<8 ||
            (spec.kind==Kind::Join&&(!isInteger(s.inputs[1]->type)||s.inputs[1]->type.bitWidth<8)))
            fail("signature_invalid","This data manipulation operation requires an exact 8/16/32/64-bit integer representation.");
    }
    if(spec.kind==Kind::Split) {
        const auto width=(std::max)(8U,static_cast<unsigned>(s.common.bitWidth)/2U);
        const auto out=rt::valueTypeFromCanonicalName("u"+std::to_string(width));
        expect(s.outputs[0],out);expect(s.outputs[1],out);
    } else if(spec.kind==Kind::Join) {
        if(s.common.bitWidth>32)fail("signature_invalid","Join Numbers accepts 8-, 16- or 32-bit halves only.");
        expect(s.outputs[0],rt::valueTypeFromCanonicalName("u"+std::to_string(s.common.bitWidth*2)));
    } else if(spec.kind==Kind::MantissaExponent) {
        s.common=rt::valueTypeFromCanonicalName(isInteger(s.inputs[0]->type)?"f64":s.inputs[0]->type.canonicalName);
        expect(s.outputs[0],s.common);expect(s.outputs[1],s.common);
    } else {
        expect(s.outputs[0],s.common);
        if(s.outputs[1])expect(s.outputs[1],spec.kind==Kind::InRange || spec.kind==Kind::CarryLeft ||
            spec.kind==Kind::CarryRight?rt::valueTypeFromCanonicalName("bool"):s.common);
    }
    return s;
}
template<class Port>
inline bool validateSignature(std::string_view operation,std::span<const Port> ports,
    std::string *message=nullptr,std::string *port=nullptr) noexcept {
    try{static_cast<void>(signature(operation,ports));return true;}
    catch(const Failure &e){if(message)*message=e.message;if(port)*port=e.port;}
    catch(...){if(message)*message="Scalar Numeric signature validation failed without admitting a partial schema.";}
    return false;
}
} // namespace frog::runtime::numeric_aggregate::scalar_palette
