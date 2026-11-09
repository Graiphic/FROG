#pragma once
#include "frog_native_engine/EllipticBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::elliptic_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
constexpr std::array<std::pair<std::string_view, std::string_view>, 4> identities{{
    {"math.elliptic.first_kind.complete_k", "complete_k"},
    {"math.elliptic.first_kind.incomplete_f", "incomplete_f"},
    {"math.elliptic.second_kind.complete_e", "complete_e"},
    {"math.elliptic.second_kind.incomplete_e", "incomplete_e"}}};
TypeRef realTree(const TypeRef &input) {
    if (!numeric_tree_math::aggregate(input)) return TypeRef::primitive("f64");
    auto result = input;
    for (auto &child : result.children) child = realTree(child);
    return result;
}
EllipticTypeResult rejected(ResolutionStatus status, std::string code, std::string message) {
    return {{status, {}, std::move(code), std::move(message)}, false};
}

}
}
