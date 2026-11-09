#pragma once
#include "frog_native_engine/DiscreteMathBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::discretemath_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
enum class Operation {Unknown,Gcd,Lcm,Factorial,PrimeFactor,CheckPrime,Permute,Binomial,ContinuedFraction};
Operation operation(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    constexpr std::pair<std::string_view,Operation> names[]{
        {"math.discrete.gcd",Operation::Gcd},{"math.discrete.lcm",Operation::Lcm},
        {"math.discrete.factorial",Operation::Factorial},{"math.discrete.prime_factor",Operation::PrimeFactor},
        {"math.discrete.check_prime",Operation::CheckPrime},{"math.discrete.permute",Operation::Permute},
        {"math.discrete.binomial",Operation::Binomial},{"math.discrete.continued_fraction",Operation::ContinuedFraction}};
    for(const auto &[name,op]:names)if(id==name)return op;
    return Operation::Unknown;
}
TypeRef declaredType(Operation op,std::string_view port) {
    if(port=="error")return TypeRef::primitive("i32");
    if(op==Operation::Gcd)return TypeRef::primitive("i32");
    if(op==Operation::Lcm)return TypeRef::primitive(port=="lcm"?"i64":"i32");
    if(op==Operation::Factorial)return TypeRef::primitive(port=="n"?"i32":"f64");
    if(op==Operation::PrimeFactor)return port=="n"?TypeRef::primitive("i64"):TypeRef::array(TypeRef::primitive("i64"));
    if(op==Operation::CheckPrime)return TypeRef::primitive(port=="n"?"i64":"bool");
    if(op==Operation::Permute||op==Operation::Binomial)return TypeRef::primitive(port=="n"||port=="k"?"u32":"f64");
    if(op==Operation::ContinuedFraction)return port=="A"||port=="B"?TypeRef::array(TypeRef::primitive("f64")):TypeRef::primitive("f64");
    return {};
}
bool inputMatches(const TypeRef &actual,const TypeRef &expected) {
    if(expected.kind!=TypeKind::Array)return actual==expected;
    auto unfixed=actual;unfixed.extent.reset();return unfixed==expected;
}
DiscreteMathTypeResult typeFailure(ResolutionStatus status,std::string code,std::string message) {
    return {{status,{},std::move(code),std::move(message)},false};
}

}
}
