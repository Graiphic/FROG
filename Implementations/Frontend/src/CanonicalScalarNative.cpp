#include "frog_native_engine/CanonicalScalarNative.hpp"
#include <array>
#include <charconv>

namespace frog::native_engine {
namespace {
using Domain=CanonicalScalarDomain;
using Policy=CanonicalScalarPolicy;
constexpr auto contracts=std::to_array<CanonicalScalarNativeContract>({
    {"numeric.add","frog.core.add","frog.private.native.add.checked64.v1",Domain::ExistingAdd,2,"a","b","result","Public core finite-f64/checked-i64/u64; distinct from versioned private Studio modulo/IEEE arithmetic"},
    {"numeric.add","frog.studio.numeric.add.reduce.ieee-wrap64.v1",canonicalNumericAddReductionProfile,Domain::ExistingAdd,2,"a","b","result","Exact same-type i64/u64 modulo 2^64 or strict f64 IEEE;2..16 actual ordered operands, canonical left fold, no reassociation, conversion or deduplication",Policy::StudioIeeeWrap64V1,16},
    {"numeric.add","frog.studio.numeric.add.reduce.wrap32.v1",canonicalNumericAddReductionProfile,Domain::ExistingAdd,2,"a","b","result","Exact same-type i32/u32 modulo 2^32;2..16 actual ordered operands, canonical left fold, no conversion or deduplication",Policy::StudioWrap32V1,16},
    {"numeric.subtract","frog.core.sub","frog.private.native.scalar.exact.v1",Domain::FiniteF64Arithmetic,2,"a","b","result","f64 only; finite result required; nonfinite result faults before publication"},
    {"numeric.multiply","frog.core.mul","frog.private.native.scalar.exact.v1",Domain::FiniteF64Arithmetic,2,"a","b","result","f64 only; finite result required; nonfinite result faults before publication"},
    {"numeric.divide","frog.core.div","frog.private.native.scalar.exact.v1",Domain::FiniteF64Arithmetic,2,"a","b","result","f64 only; zero division/nonfinite result faults; integer-to-f64 conversion is not inserted"},
    {"comparison.equal","frog.core.equal","frog.private.native.scalar.exact.v1",Domain::ExactEquality,2,"a","b","result","Exact same-type bool/i64/u64 or finite-f64 inputs; nonfinite operands fault; bool result"},
    {"comparison.not_equal","frog.core.not_equal","frog.private.native.scalar.exact.v1",Domain::ExactEquality,2,"a","b","result","Exact same-type bool/i64/u64 or finite-f64 inputs; nonfinite operands fault; bool result"},
    {"comparison.less","frog.core.less","frog.private.native.scalar.exact.v1",Domain::ExactOrder,2,"a","b","result","Exact same-type i64/u64 or finite-f64 inputs; nonfinite operands fault; bool result"},
    {"comparison.less_or_equal","frog.core.less_or_equal","frog.private.native.scalar.exact.v1",Domain::ExactOrder,2,"a","b","result","Exact same-type i64/u64 or finite-f64 inputs; nonfinite operands fault; bool result"},
    {"comparison.greater","frog.core.greater","frog.private.native.scalar.exact.v1",Domain::ExactOrder,2,"a","b","result","Exact same-type i64/u64 or finite-f64 inputs; nonfinite operands fault; bool result"},
    {"comparison.greater_or_equal","frog.core.greater_or_equal","frog.private.native.scalar.exact.v1",Domain::ExactOrder,2,"a","b","result","Exact same-type i64/u64 or finite-f64 inputs; nonfinite operands fault; bool result"},
    {"boolean.and","frog.core.and","frog.private.native.scalar.exact.v1",Domain::Boolean,2,"a","b","result","bool only; conjunction"},
    {"boolean.or","frog.core.or","frog.private.native.scalar.exact.v1",Domain::Boolean,2,"a","b","result","bool only; disjunction"},
    {"boolean.xor","frog.core.xor","frog.private.native.scalar.exact.v1",Domain::Boolean,2,"a","b","result","bool only; exclusive disjunction"},
    {"boolean.not","frog.core.not","frog.private.native.scalar.exact.v1",Domain::Boolean,1,"in","","result","bool only; negation"},
    {"boolean.and","frog.studio.boolean.and.reduce.v1",canonicalBooleanReductionProfile,Domain::Boolean,2,"a","b","result","Exact bool only;2..16 actual ordered operands; conjunction; invert once after fold where specified",Policy::CoreCheckedFinite,16},
    {"boolean.or","frog.studio.boolean.or.reduce.v1",canonicalBooleanReductionProfile,Domain::Boolean,2,"a","b","result","Exact bool only;2..16 actual ordered operands; disjunction; invert once after fold where specified",Policy::CoreCheckedFinite,16},
    {"boolean.xor","frog.studio.boolean.xor.reduce.v1",canonicalBooleanReductionProfile,Domain::Boolean,2,"a","b","result","Exact bool only;2..16 actual ordered operands; parity; invert once after fold where specified",Policy::CoreCheckedFinite,16},
    {"boolean.nand","frog.studio.boolean.nand.reduce.v1",canonicalBooleanReductionProfile,Domain::Boolean,2,"a","b","result","Exact bool only;2..16 actual ordered operands; negated conjunction; invert once after fold where specified",Policy::CoreCheckedFinite,16},
    {"boolean.nor","frog.studio.boolean.nor.reduce.v1",canonicalBooleanReductionProfile,Domain::Boolean,2,"a","b","result","Exact bool only;2..16 actual ordered operands; negated disjunction; invert once after fold where specified",Policy::CoreCheckedFinite,16},
    {"boolean.xnor","frog.studio.boolean.xnor.reduce.v1",canonicalBooleanReductionProfile,Domain::Boolean,2,"a","b","result","Exact bool only;2..16 actual ordered operands; negated parity; invert once after fold where specified",Policy::CoreCheckedFinite,16},
    {"boolean.implies","frog.studio.boolean.implies.v1","frog.studio.boolean.implies@1",Domain::Boolean,2,"a","b","result","Exact bool only; ordered binary !a || b; no numeric/lifted coercion"},
    {"numeric.add","frog.studio.numeric.add.ieee-wrap64.v1","frog.studio.numeric.ieee-wrap64@1",Domain::ExistingAdd,2,"a","b","result","Same-type i64/u64 modulo 2^64; strict f64 IEEE addition including nonfinite operands/results; no implicit conversion",Policy::StudioIeeeWrap64V1},
    {"numeric.subtract","frog.studio.numeric.sub.ieee-wrap64.v1","frog.studio.numeric.ieee-wrap64@1",Domain::ExistingAdd,2,"a","b","result","Same-type i64/u64 modulo 2^64; strict f64 IEEE subtraction including nonfinite operands/results; no implicit conversion",Policy::StudioIeeeWrap64V1},
    {"numeric.multiply","frog.studio.numeric.mul.ieee-wrap64.v1","frog.studio.numeric.ieee-wrap64@1",Domain::ExistingAdd,2,"a","b","result","Same-type i64/u64 modulo 2^64; strict f64 IEEE multiplication including nonfinite operands/results; no implicit conversion",Policy::StudioIeeeWrap64V1},
    {"numeric.divide","frog.studio.numeric.div.ieee-wrap64.v1","frog.studio.numeric.ieee-wrap64@1",Domain::FiniteF64Arithmetic,2,"a","b","result","f64 only; strict IEEE division including signed zero, infinity and NaN; no implicit integer conversion or integer quotient",Policy::StudioIeeeWrap64V1},
    {"numeric.divide","frog.studio.numeric.div.integer-real64.v1","frog.studio.numeric.integer-real64@1",Domain::IntegerDivideReal64,2,"a","b","result","Two same-type i32/u32/i64/u64 operands convert separately to f64 before IEEE division; output f64, no truncated integer quotient; mixed types remain explicitly unsupported",Policy::StudioIeeeWrap64V1},
    {"comparison.equal","frog.studio.comparison.equal.ieee64.v1","frog.studio.comparison.ieee64@1",Domain::ExactEquality,2,"a","b","result","Same-type bool/i64/u64 or strict f64 IEEE equality; unordered NaN is false",Policy::StudioIeeeWrap64V1},
    {"comparison.not_equal","frog.studio.comparison.not_equal.ieee64.v1","frog.studio.comparison.ieee64@1",Domain::ExactEquality,2,"a","b","result","Same-type bool/i64/u64 or strict f64 IEEE inequality; unordered NaN is true",Policy::StudioIeeeWrap64V1},
    {"comparison.less","frog.studio.comparison.less.ieee64.v1","frog.studio.comparison.ieee64@1",Domain::ExactOrder,2,"a","b","result","Exact same-type i64/u64 or strict f64 IEEE ordered less; unordered NaN is false",Policy::StudioIeeeWrap64V1},
    {"comparison.less_or_equal","frog.studio.comparison.less_or_equal.ieee64.v1","frog.studio.comparison.ieee64@1",Domain::ExactOrder,2,"a","b","result","Exact same-type i64/u64 or strict f64 IEEE ordered less-or-equal; unordered NaN is false",Policy::StudioIeeeWrap64V1},
    {"comparison.greater","frog.studio.comparison.greater.ieee64.v1","frog.studio.comparison.ieee64@1",Domain::ExactOrder,2,"a","b","result","Exact same-type i64/u64 or strict f64 IEEE ordered greater; unordered NaN is false",Policy::StudioIeeeWrap64V1},
    {"comparison.greater_or_equal","frog.studio.comparison.greater_or_equal.ieee64.v1","frog.studio.comparison.ieee64@1",Domain::ExactOrder,2,"a","b","result","Exact same-type i64/u64 or strict f64 IEEE ordered greater-or-equal; unordered NaN is false",Policy::StudioIeeeWrap64V1},
    {"numeric.negate","frog.core.neg","frog.private.native.unary.checked64@1",Domain::UnarySignedChecked,1,"in","","result","Exact i64/f64; signed minimum overflow and nonfinite operands/results fault before publication",Policy::CoreCheckedFinite},
    {"numeric.absolute_value","frog.core.abs","frog.private.native.unary.checked64@1",Domain::UnarySignedChecked,1,"in","","result","Exact i64/f64; signed minimum overflow and nonfinite operands/results fault before publication",Policy::CoreCheckedFinite},
    {"numeric.round_nearest","frog.math.round","frog.private.math.integer-rounding@1",Domain::UnaryIntegerRounding,1,"in","","result","Published pure value identity preserved; exact integer identity implemented. f64 public Round is not realized under the author's FENV-dependent policy.",Policy::MathValueV1},
    {"numeric.round_nearest","frog.studio.numeric.round_nearest.fenv64.v1","frog.studio.numeric.rounding-fenv@1",Domain::UnaryRounding,1,"in","","result","Local authored execution only: exact same-type integer identity; f64 nearbyint observes the current FENV (default ties-to-even), with no pure/readnone promise.",Policy::StudioRoundingFenvV1},
    {"numeric.round_nearest","frog.studio.numeric.round_nearest.even64.v1","frog.studio.numeric.rounding-even@1",Domain::UnaryRounding,1,"in","","result","Studio Round: exact same-type integer identity; binary64 nearest with ties to even, independent of caller FENV; signed zero and IEEE special-value bits preserved. Historical explicit fenv64.v1 remains supported.",Policy::StudioRoundingEvenV1},
    {"numeric.square","frog.studio.numeric.square.ieee-wrap64.v1","frog.studio.numeric.square-ieee-wrap64@1",Domain::UnarySame,1,"in","","result","Same-type exact i64/u64 modulo 2^64 or IEEE f64 square; unary only, no second driver or implicit cast",Policy::StudioIeeeWrap64V1},
    {"numeric.square","frog.studio.numeric.square.wrap32.v1","frog.studio.numeric.square-wrap32@1",Domain::UnarySame,1,"in","","result","Same-type exact i32/u32 square modulo 2^32; unary only",Policy::StudioWrap32V1},
    {"numeric.square_root","frog.studio.numeric.square_root.real64.v1","frog.studio.numeric.unary-real64@1",Domain::UnaryReal64,1,"in","","result","Exact i32/u32/i64/u64 inputs convert to f64 before sqrt; f64 input stays f64; IEEE NaN for negative real values",Policy::StudioIeeeWrap64V1},
    {"numeric.reciprocal","frog.studio.numeric.reciprocal.real64.v1","frog.studio.numeric.unary-real64@1",Domain::UnaryReal64,1,"in","","result","Exact i32/u32/i64/u64 inputs convert to f64 before reciprocal; f64 input stays f64; IEEE signed infinity for zero",Policy::StudioIeeeWrap64V1},
    {"numeric.round_negative_infinity","frog.math.floor","frog.private.math.directed-rounding@1",Domain::UnaryRounding,1,"in","","result","Published pure value identity preserved; exact same-type integer identity, f64 mathematical Floor/Ceil including IEEE special values. No environmental flag effects are claimed.",Policy::MathValueV1},
    {"numeric.round_positive_infinity","frog.math.ceil","frog.private.math.directed-rounding@1",Domain::UnaryRounding,1,"in","","result","Published pure value identity preserved; exact same-type integer identity, f64 mathematical Floor/Ceil including IEEE special values. No environmental flag effects are claimed.",Policy::MathValueV1},
    {"numeric.increment","frog.studio.numeric.increment.ieee-wrap64.v1","frog.studio.numeric.unary-ieee-wrap64@1",Domain::UnarySame,1,"in","","result","Exact same-type i64/u64/f64; integer modulo-width without signed overflow UB; f64 IEEE only in 64-bit profile; no implicit conversion",Policy::StudioIeeeWrap64V1},
    {"numeric.decrement","frog.studio.numeric.decrement.ieee-wrap64.v1","frog.studio.numeric.unary-ieee-wrap64@1",Domain::UnarySame,1,"in","","result","Exact same-type i64/u64/f64; integer modulo-width without signed overflow UB; f64 IEEE only in 64-bit profile; no implicit conversion",Policy::StudioIeeeWrap64V1},
    {"numeric.negate","frog.studio.numeric.negate.ieee-wrap64.v1","frog.studio.numeric.unary-ieee-wrap64@1",Domain::UnarySigned,1,"in","","result","Exact same-type i64/f64; integer modulo-width without signed overflow UB; f64 IEEE only in 64-bit profile; no implicit conversion",Policy::StudioIeeeWrap64V1},
    {"numeric.absolute_value","frog.studio.numeric.absolute_value.ieee-wrap64.v1","frog.studio.numeric.unary-ieee-wrap64@1",Domain::UnarySigned,1,"in","","result","Exact same-type i64/f64; integer modulo-width without signed overflow UB; f64 IEEE only in 64-bit profile; no implicit conversion",Policy::StudioIeeeWrap64V1},
    {"numeric.increment","frog.studio.numeric.increment.wrap32.v1","frog.studio.numeric.unary-wrap32@1",Domain::UnarySame,1,"in","","result","Exact same-type i32/u32; integer modulo-width without signed overflow UB; f64 IEEE only in 64-bit profile; no implicit conversion",Policy::StudioWrap32V1},
    {"numeric.decrement","frog.studio.numeric.decrement.wrap32.v1","frog.studio.numeric.unary-wrap32@1",Domain::UnarySame,1,"in","","result","Exact same-type i32/u32; integer modulo-width without signed overflow UB; f64 IEEE only in 64-bit profile; no implicit conversion",Policy::StudioWrap32V1},
    {"numeric.negate","frog.studio.numeric.negate.wrap32.v1","frog.studio.numeric.unary-wrap32@1",Domain::UnarySigned,1,"in","","result","Exact same-type i32; integer modulo-width without signed overflow UB; f64 IEEE only in 64-bit profile; no implicit conversion",Policy::StudioWrap32V1},
    {"numeric.absolute_value","frog.studio.numeric.absolute_value.wrap32.v1","frog.studio.numeric.unary-wrap32@1",Domain::UnarySigned,1,"in","","result","Exact same-type i32; integer modulo-width without signed overflow UB; f64 IEEE only in 64-bit profile; no implicit conversion",Policy::StudioWrap32V1},
    {"numeric.add","frog.studio.numeric.add.wrap32.v1","frog.studio.numeric.wrap32@1",Domain::ExistingAdd,2,"a","b","result","Exact same-type i32/u32 modulo 2^32, restored signed/unsigned carrier64; no f64 or implicit conversion",Policy::StudioWrap32V1},
    {"numeric.subtract","frog.studio.numeric.sub.wrap32.v1","frog.studio.numeric.wrap32@1",Domain::ExistingAdd,2,"a","b","result","Exact same-type i32/u32 modulo 2^32, restored signed/unsigned carrier64; no f64 or implicit conversion",Policy::StudioWrap32V1},
    {"numeric.multiply","frog.studio.numeric.mul.wrap32.v1","frog.studio.numeric.wrap32@1",Domain::ExistingAdd,2,"a","b","result","Exact same-type i32/u32 modulo 2^32, restored signed/unsigned carrier64; no f64 or implicit conversion",Policy::StudioWrap32V1},
    {"comparison.equal","frog.studio.comparison.equal.exact32.v1","frog.studio.comparison.exact32@1",Domain::ExactEquality,2,"a","b","result","Exact same-type i32/u32 comparison with bool result; no floating or nonfinite permission",Policy::StudioWrap32V1},
    {"comparison.not_equal","frog.studio.comparison.not_equal.exact32.v1","frog.studio.comparison.exact32@1",Domain::ExactEquality,2,"a","b","result","Exact same-type i32/u32 comparison with bool result; no floating or nonfinite permission",Policy::StudioWrap32V1},
    {"comparison.less","frog.studio.comparison.less.exact32.v1","frog.studio.comparison.exact32@1",Domain::ExactOrder,2,"a","b","result","Exact same-type i32/u32 comparison with bool result; no floating or nonfinite permission",Policy::StudioWrap32V1},
    {"comparison.less_or_equal","frog.studio.comparison.less_or_equal.exact32.v1","frog.studio.comparison.exact32@1",Domain::ExactOrder,2,"a","b","result","Exact same-type i32/u32 comparison with bool result; no floating or nonfinite permission",Policy::StudioWrap32V1},
    {"comparison.greater","frog.studio.comparison.greater.exact32.v1","frog.studio.comparison.exact32@1",Domain::ExactOrder,2,"a","b","result","Exact same-type i32/u32 comparison with bool result; no floating or nonfinite permission",Policy::StudioWrap32V1},
    {"comparison.greater_or_equal","frog.studio.comparison.greater_or_equal.exact32.v1","frog.studio.comparison.exact32@1",Domain::ExactOrder,2,"a","b","result","Exact same-type i32/u32 comparison with bool result; no floating or nonfinite permission",Policy::StudioWrap32V1},
    {"comparison.equal_zero","frog.studio.comparison.equal_zero.exact-ieee.v1",canonicalComparisonZeroProfile,Domain::ZeroComparison,1,"x","","result","Exact i32/u32/i64/u64 or IEEE f64 compared with typed zero; bool result, unordered NaN is true only for Not Equal; no implicit conversion or synthetic second operand",Policy::StudioComparisonZeroV1},
    {"comparison.not_equal_zero","frog.studio.comparison.not_equal_zero.exact-ieee.v1",canonicalComparisonZeroProfile,Domain::ZeroComparison,1,"x","","result","Exact i32/u32/i64/u64 or IEEE f64 compared with typed zero; bool result, unordered NaN is true only for Not Equal; no implicit conversion or synthetic second operand",Policy::StudioComparisonZeroV1},
    {"comparison.greater_zero","frog.studio.comparison.greater_zero.exact-ieee.v1",canonicalComparisonZeroProfile,Domain::ZeroComparison,1,"x","","result","Exact i32/u32/i64/u64 or IEEE f64 compared with typed zero; bool result, unordered NaN is true only for Not Equal; no implicit conversion or synthetic second operand",Policy::StudioComparisonZeroV1},
    {"comparison.less_zero","frog.studio.comparison.less_zero.exact-ieee.v1",canonicalComparisonZeroProfile,Domain::ZeroComparison,1,"x","","result","Exact i32/u32/i64/u64 or IEEE f64 compared with typed zero; bool result, unordered NaN is true only for Not Equal; no implicit conversion or synthetic second operand",Policy::StudioComparisonZeroV1},
    {"comparison.greater_or_equal_zero","frog.studio.comparison.greater_or_equal_zero.exact-ieee.v1",canonicalComparisonZeroProfile,Domain::ZeroComparison,1,"x","","result","Exact i32/u32/i64/u64 or IEEE f64 compared with typed zero; bool result, unordered NaN is true only for Not Equal; no implicit conversion or synthetic second operand",Policy::StudioComparisonZeroV1},
    {"comparison.less_or_equal_zero","frog.studio.comparison.less_or_equal_zero.exact-ieee.v1",canonicalComparisonZeroProfile,Domain::ZeroComparison,1,"x","","result","Exact i32/u32/i64/u64 or IEEE f64 compared with typed zero; bool result, unordered NaN is true only for Not Equal; no implicit conversion or synthetic second operand",Policy::StudioComparisonZeroV1}
});
}
std::span<const CanonicalScalarNativeContract> canonicalScalarNativeContracts() noexcept {return contracts;}
const CanonicalScalarNativeContract *canonicalScalarNativeContract(std::string_view id) noexcept {
    for(const auto &contract:contracts)if(contract.operationId==id)return &contract;return nullptr;
}
std::string_view canonicalScalarMigrationIdentity(std::string_view id) noexcept {
    if(id=="frog.math.square")return "frog.studio.numeric.square.ieee-wrap64.v1";
    if(id=="frog.math.sqrt")return "frog.studio.numeric.square_root.real64.v1";
    if(id=="frog.math.reciprocal")return "frog.studio.numeric.reciprocal.real64.v1";
    if(id=="frog.math.round"||id=="frog.studio.numeric.round_nearest")return "frog.studio.numeric.round_nearest.even64.v1";
    if(const auto *contract=canonicalScalarNativeContract(id)) {
        if(contract->policy!=Policy::CoreCheckedFinite)return contract->operationId;
        for(const auto &candidate:contracts)if(candidate.catalogId==contract->catalogId&&candidate.policy==Policy::StudioIeeeWrap64V1&&candidate.maximumInputCount==contract->maximumInputCount)return candidate.operationId;
        return contract->operationId;
    }
    constexpr std::string_view privatePrefix="frog.studio.";
    if(id.starts_with(privatePrefix)) {
        id.remove_prefix(privatePrefix.size());
        for(const auto &contract:contracts)if(contract.catalogId==id&&contract.policy==Policy::StudioIeeeWrap64V1&&!contract.maximumInputCount)return contract.operationId;
        for(const auto &contract:contracts)if(contract.catalogId==id)return contract.operationId;
    }
    return {};
}
std::string_view canonicalScalarMigrationIdentityForType(std::string_view id,std::string_view type) noexcept {
    if(type.empty())return {};
    // An explicit narrow contract never silently changes width.
    if(const auto *given=canonicalScalarNativeContract(id);given&&given->policy==Policy::StudioWrap32V1)
        return canonicalScalarNativeOperandAccepted(*given,type)?given->operationId:std::string_view{};
    const auto legacy=canonicalScalarMigrationIdentity(id);
    const auto *contract=canonicalScalarNativeContract(legacy);
    if(!contract)return {};
    if(contract->policy==Policy::StudioComparisonZeroV1||contract->policy==Policy::MathValueV1||contract->policy==Policy::StudioRoundingFenvV1||contract->policy==Policy::StudioRoundingEvenV1)return canonicalScalarNativeOperandAccepted(*contract,type)?legacy:std::string_view{};
    if(contract->catalogId=="numeric.divide"&&type!="f64") {
        constexpr auto integerDivision="frog.studio.numeric.div.integer-real64.v1";
        const auto *promoted=canonicalScalarNativeContract(integerDivision);
        return promoted&&canonicalScalarNativeOperandAccepted(*promoted,type)?integerDivision:std::string_view{};
    }
    if(contract->domain==Domain::UnaryReal64)return canonicalScalarNativeOperandAccepted(*contract,type)?legacy:std::string_view{};
    if(type=="i32"||type=="u32") {
        for(const auto &candidate:contracts)if(candidate.catalogId==contract->catalogId&&candidate.policy==Policy::StudioWrap32V1&&candidate.maximumInputCount==contract->maximumInputCount&&canonicalScalarNativeOperandAccepted(candidate,type))return candidate.operationId;
        return {};
    }
    return canonicalScalarNativeOperandAccepted(*contract,type)?legacy:std::string_view{};
}
std::optional<std::size_t> canonicalBooleanInputOrdinal(std::string_view port) noexcept {
    if(port=="a")return 0;if(port=="b")return 1;
    if(!port.starts_with("input_"))return {};const auto suffix=port.substr(6);
    std::size_t ordinal=0;const auto parsed=std::from_chars(suffix.data(),suffix.data()+suffix.size(),ordinal);
    if(suffix.empty()||suffix.front()=='0'||parsed.ec!=std::errc{}||parsed.ptr!=suffix.data()+suffix.size()||ordinal<3||ordinal>16)return {};
    return ordinal-1;
}
bool canonicalBooleanInputPortsValid(std::span<const std::string> ports) noexcept {
    if(ports.size()<2||ports.size()>16||ports[0]!="a"||ports[1]!="b")return false;
    std::size_t previous=1;for(std::size_t i=2;i<ports.size();++i){const auto ordinal=canonicalBooleanInputOrdinal(ports[i]);if(!ordinal||*ordinal<=previous)return false;previous=*ordinal;}return true;
}
std::optional<std::size_t> canonicalNumericAddInputOrdinal(std::string_view port) noexcept {return canonicalBooleanInputOrdinal(port);}
bool canonicalNumericAddInputPortsValid(std::span<const std::string> ports) noexcept {return canonicalBooleanInputPortsValid(ports);}
std::string_view canonicalScalarMigrationIdentityForArity(std::string_view id,std::size_t count) noexcept {
    const auto mapped=canonicalScalarMigrationIdentity(id);const auto *contract=canonicalScalarNativeContract(mapped);if(!contract)return {};
    if(contract->catalogId=="numeric.add") {
        if(count<2||count>16)return {};
        const auto maximum=count>2?16U:0U;
        for(const auto &candidate:contracts)if(candidate.catalogId==contract->catalogId&&candidate.policy==contract->policy&&candidate.maximumInputCount==maximum)return candidate.operationId;
        return {};
    }
    if(contract->domain!=Domain::Boolean)return count==contract->inputCount?mapped:std::string_view{};
    if(count==contract->inputCount)return mapped;
    if(count<2||count>16)return {};
    for(const auto &candidate:contracts)if(candidate.catalogId==contract->catalogId&&candidate.maximumInputCount==16)return candidate.operationId;
    return {};
}
bool canonicalScalarNativeOperandAccepted(const CanonicalScalarNativeContract &contract,std::string_view type) noexcept {
    if(contract.domain==Domain::IntegerDivideReal64)return type=="i32"||type=="u32"||type=="i64"||type=="u64";
    if(contract.domain==Domain::UnaryReal64)return type=="i32"||type=="u32"||type=="i64"||type=="u64"||type=="f64";
    if(contract.domain==Domain::ZeroComparison)return type=="i32"||type=="u32"||type=="i64"||type=="u64"||type=="f64";
    if(contract.domain==Domain::UnarySignedChecked)return type=="i64"||type=="f64";
    if(contract.domain==Domain::UnaryIntegerRounding)return type=="i32"||type=="u32"||type=="i64"||type=="u64";
    if(contract.domain==Domain::UnaryRounding)return type=="i32"||type=="u32"||type=="i64"||type=="u64"||type=="f64";
    if(contract.policy==Policy::StudioWrap32V1)return type=="i32"||(type=="u32"&&contract.domain!=Domain::UnarySigned);
    if(contract.domain==Domain::UnarySigned)return type=="i64"||type=="f64";
    if(contract.domain==Domain::Boolean)return type=="bool";
    if(contract.domain==Domain::FiniteF64Arithmetic)return type=="f64";
    return type=="i64"||type=="u64"||type=="f64"||(contract.domain==Domain::ExactEquality&&type=="bool");
}
}
