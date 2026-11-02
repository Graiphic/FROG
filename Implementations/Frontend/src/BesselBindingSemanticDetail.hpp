#pragma once
#include "frog_native_engine/BesselBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::bessel_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
constexpr std::array<std::string_view, 12> keys{
    "bessel_cyl_j", "bessel_cyl_y", "hankel_cyl", "bessel_mod_i", "bessel_mod_k",
    "bessel_sph_j", "bessel_sph_y", "hankel_sph", "airy_ai_bi", "struve_h", "kelvin_be", "kelvin_ke"};
bool integerOrder(std::string_view key) noexcept {
    return key == "bessel_mod_k" || key == "bessel_sph_j" || key == "bessel_sph_y" ||
        key == "hankel_sph" || key == "kelvin_be" || key == "kelvin_ke";
}
bool complexResult(std::string_view key) noexcept {
    return key == "hankel_cyl" || key == "hankel_sph" || key == "kelvin_be" || key == "kelvin_ke";
}
TypeRef resultTree(const TypeRef &x, bool complex) {
    if (!numeric_tree_math::aggregate(x)) return TypeRef::primitive(complex ? "c64" : "f64");
    auto result = x;
    for (auto &child : result.children) child = resultTree(child, complex);
    return result;
}
BesselTypeResult failure(ResolutionStatus status, std::string code, std::string message) {
    return {{status, {}, std::move(code), std::move(message)}, false};
}
bool integerScalar(const TypeRef &type) noexcept {
    return type.kind == TypeKind::Primitive && numeric_tree_math::integerWidth(type.name) && type.children.empty();
}

}
}
