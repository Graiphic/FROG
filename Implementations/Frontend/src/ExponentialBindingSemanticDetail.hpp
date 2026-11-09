#pragma once
#include "frog_native_engine/ExponentialBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::exponential_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
enum class Operation { Unknown, Exp, Expm1, Pow10, Pow2, Power, Root, Ln, Log1p, Log10, Log2, LogBase };
Operation operation(std::string_view id) noexcept {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    constexpr std::pair<std::string_view, Operation> functions[]{
        {"math.exponential.exponential", Operation::Exp},
        {"math.exponential.exponential_minus_one", Operation::Expm1},
        {"math.exponential.power_of_10", Operation::Pow10},
        {"math.exponential.power_of_2", Operation::Pow2},
        {"math.exponential.power_of_x", Operation::Power},
        {"math.exponential.y_th_root_of_x", Operation::Root},
        {"math.exponential.natural_logarithm", Operation::Ln},
        {"math.exponential.natural_logarithm_plus_one", Operation::Log1p},
        {"math.exponential.logarithm_base_10", Operation::Log10},
        {"math.exponential.logarithm_base_2", Operation::Log2},
        {"math.exponential.logarithm_base_x", Operation::LogBase},
    };
    for (const auto &[name, value] : functions) if (id == name) return value;
    return Operation::Unknown;
}
bool binary(Operation op) noexcept {
    return op == Operation::Power || op == Operation::Root || op == Operation::LogBase;
}
using namespace numeric_tree_math;
ExponentialTypeResult failure(ResolutionStatus status, std::string code, std::string message) {
    return {{status, {}, std::move(code), std::move(message)}, false};
}


}
}
