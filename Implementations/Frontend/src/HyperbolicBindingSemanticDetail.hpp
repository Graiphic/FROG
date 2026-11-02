#pragma once
#include "frog_native_engine/HyperbolicBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/HyperbolicReal64.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::hyperbolic_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
using Operation=HyperbolicRealOperation;
Operation operation(std::string_view id) noexcept {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    constexpr std::pair<std::string_view, Operation> functions[]{
        {"math.hyperbolic.sinh", Operation::Sinh},
        {"math.hyperbolic.cosh", Operation::Cosh},
        {"math.hyperbolic.tanh", Operation::Tanh},
        {"math.hyperbolic.sech", Operation::Sech},
        {"math.hyperbolic.csch", Operation::Csch},
        {"math.hyperbolic.coth", Operation::Coth},
        {"math.hyperbolic.asinh", Operation::Asinh},
        {"math.hyperbolic.acosh", Operation::Acosh},
        {"math.hyperbolic.atanh", Operation::Atanh},
        {"math.hyperbolic.asech", Operation::Asech},
        {"math.hyperbolic.acsch", Operation::Acsch},
        {"math.hyperbolic.acoth", Operation::Acoth},
    };
    for (const auto &[name, value] : functions) if (id == name) return value;
    return Operation::Unknown;
}


HyperbolicTypeResult failure(ResolutionStatus status, std::string code, std::string message) {
    return {{status, {}, std::move(code), std::move(message)}, false};
}

}
}
