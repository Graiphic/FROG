#pragma once
#include "frog_native_engine/TrigonometricBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/TrigonometricReal64.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::trigonometric_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
using Operation=TrigonometricRealOperation;
Operation operation(std::string_view id) noexcept {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    constexpr std::pair<std::string_view, Operation> functions[]{
        {"math.trigonometric.sin", Operation::Sin},
        {"math.trigonometric.cos", Operation::Cos},
        {"math.trigonometric.tan", Operation::Tan},
        {"math.trigonometric.csc", Operation::Csc},
        {"math.trigonometric.sec", Operation::Sec},
        {"math.trigonometric.cot", Operation::Cot},
        {"math.trigonometric.acsc", Operation::Acsc},
        {"math.trigonometric.asec", Operation::Asec},
        {"math.trigonometric.acot", Operation::Acot},
        {"math.trigonometric.asin", Operation::Asin},
        {"math.trigonometric.acos", Operation::Acos},
        {"math.trigonometric.atan", Operation::Atan},
        {"math.trigonometric.atan2", Operation::Atan2},
        {"math.trigonometric.sincos", Operation::SinCos},
        {"math.trigonometric.sinc", Operation::Sinc},
    };
    for (const auto &[name, value] : functions) if (id == name) return value;
    return Operation::Unknown;
}


TrigonometricTypeResult failure(ResolutionStatus status, std::string code, std::string message) {
    return {{status, {}, std::move(code), std::move(message)}, false};
}

}
}
