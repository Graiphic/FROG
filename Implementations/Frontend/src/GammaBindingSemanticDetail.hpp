#pragma once
#include "frog_native_engine/GammaBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::gamma_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
constexpr std::array<std::pair<std::string_view,std::string_view>,9> identities{{
    {"math.gamma.complete","gamma"}, {"math.gamma.regularized_lower","gamma_p"},
    {"math.gamma.stirling_leading","stirling"}, {"math.beta.complete","beta"},
    {"math.beta.regularized_lower","beta_p"}, {"math.gamma.log_factorial","ln_factorial"},
    {"math.gamma.log_complete","ln_gamma"}, {"math.gamma.regularized_upper","gamma_q"},
    {"math.gamma.digamma","digamma"}}};
GammaTypeResult rejected(ResolutionStatus status,std::string code,std::string message) {
    return {{status,{},std::move(code),std::move(message)},false};
}
bool realScalar(const TypeRef &type) {
    return type.kind==TypeKind::Primitive && type.children.empty() &&
        (type.name=="f64" || type.name=="f32" || numeric_tree_math::integerWidth(type.name));
}

}
}
