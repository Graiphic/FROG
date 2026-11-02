#pragma once
#include "frog_native_engine/HypergeometricBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::hypergeometric_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
constexpr std::array<std::pair<std::string_view, std::string_view>, 3> identities{{
    {"math.hypergeometric.gauss", "gauss"}, {"math.hypergeometric.kummer_m", "kummer"}, {"math.hypergeometric.tricomi_u", "tricomi"}}};
TypeRef realTree(const TypeRef &input) {
    if (!numeric_tree_math::aggregate(input)) return TypeRef::primitive("f64");
    auto result = input;
    for (auto &child : result.children) child = realTree(child);
    return result;
}
HypergeometricTypeResult rejected(ResolutionStatus status, std::string code, std::string message) {
    return {{status, {}, std::move(code), std::move(message)}, false};
}

}
}
