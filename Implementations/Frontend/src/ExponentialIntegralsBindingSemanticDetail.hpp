#pragma once
#include "frog_native_engine/ExponentialIntegralsBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::exponentialintegrals_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
constexpr std::array<std::string_view, 6> keys{"expn", "si", "ci", "dilogarithm", "shi", "chi"};
TypeRef realTree(const TypeRef &input) {
    if (!numeric_tree_math::aggregate(input)) return TypeRef::primitive("f64");
    auto result = input;
    for (auto &child : result.children) child = realTree(child);
    return result;
}
ExponentialIntegralsTypeResult failure(ResolutionStatus status, std::string code, std::string message) {
    return {{status, {}, std::move(code), std::move(message)}, false};
}
bool exactOrderType(const TypeRef &type) {
    return type == TypeRef::primitive("i32");
}

}
}
