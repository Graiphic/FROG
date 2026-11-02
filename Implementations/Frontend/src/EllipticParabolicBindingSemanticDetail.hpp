#pragma once
#include "frog_native_engine/EllipticParabolicBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::ellipticparabolic_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
TypeRef realTree(const TypeRef &type) {
    if (!numeric_tree_math::aggregate(type)) return TypeRef::primitive("f64");
    auto result = type;
    for (auto &child : result.children) child = realTree(child);
    return result;
}
EllipticParabolicTypeResult rejected(ResolutionStatus status, std::string code, std::string message) {
    return {{status, {}, "math.elliptic_parabolic." + code, std::move(message)}, false};
}

}
}
