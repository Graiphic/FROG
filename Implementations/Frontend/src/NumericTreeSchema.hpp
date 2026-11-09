#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_set>
#include <utility>
namespace frog::native_engine::numeric_tree_math {
using binding::TypeKind;
using binding::TypeRef;
inline constexpr std::size_t maximumDepth = 128;
inline constexpr std::size_t maximumSchemaNodes = 65536;

inline bool aggregate(const TypeRef &type) noexcept {
    return type.kind == TypeKind::Array || type.kind == TypeKind::Cluster;
}
inline unsigned integerWidth(std::string_view name) noexcept {
    if (name == "i4" || name == "u4") return 4;
    if (name == "i8" || name == "u8") return 8;
    if (name == "i16" || name == "u16") return 16;
    if (name == "i32" || name == "u32") return 32;
    if (name == "i64" || name == "u64") return 64;
    return 0;
}
inline bool complexType(const TypeRef &type) noexcept {
    return type.kind == TypeKind::Primitive && (type.name == "c32" || type.name == "c64");
}
inline bool matrixType(const TypeRef &type) noexcept {
    // A distinct Matrix transport is not an algorithm provider. Keep nominal
    // legacy Matrix references separate too; no rank-2 Array substitution.
    return type.kind==TypeKind::Matrix || (!aggregate(type) && (type.name.find("matrix") != std::string::npos ||
        type.name.find("Matrix") != std::string::npos));
}
inline bool supportedLeaf(const TypeRef &type) noexcept {
    return type.kind == TypeKind::Primitive && (integerWidth(type.name) ||
        type.name == "f32" || type.name == "f64" || complexType(type));
}
inline std::string validateSchema(const TypeRef &type, std::size_t depth, std::size_t &nodes,
    std::string_view profile = "Exponential", bool allowComplex = true) {
    if (depth > maximumDepth || ++nodes > maximumSchemaNodes)
        return "The numeric schema exceeds the " + std::string(profile) + " profile budget.";
    if (matrixType(type)) return "Matrix calculation requires an actual Matrix algorithm provider; a Matrix cannot be evaluated as an element-wise array.";
    if (type.kind == TypeKind::Unknown) return "The operand type is not established.";
    if (type.kind == TypeKind::Array) {
        if (type.children.size() != 1 || type.rank == 0 || type.rank > 16 ||
            (type.extent && (type.rank != 1 || *type.extent == 0)))
            return "The numeric array has an invalid rank or element schema.";
        return validateSchema(type.children.front(), depth + 1, nodes, profile, allowComplex);
    }
    if (type.kind == TypeKind::Cluster) {
        if (type.children.empty() || type.fieldIds.size() != type.children.size() ||
            (!type.fieldNames.empty() && type.fieldNames.size() != type.children.size()))
            return "A numeric Cluster requires its complete ordered field schema.";
        std::unordered_set<std::string_view> ids;
        for (const auto &id : type.fieldIds) if (id.empty() || !ids.insert(id).second)
            return "Numeric Cluster field identities must be nonempty and unique.";
        for (const auto &field : type.children) {
            auto reason = validateSchema(field, depth + 1, nodes, profile, allowComplex);
            if (!reason.empty()) return reason;
        }
        return {};
    }
    if (!supportedLeaf(type) || !type.children.empty() || !type.fieldIds.empty())
        return std::string(profile) + (allowComplex
            ? " calculation accepts f32/f64, c32/c64 or 4/8/16/32/64-bit integer leaves; Boolean, Enum, references, fixed-point and extended precision are unsupported."
            : " calculation accepts real f32/f64 or 4/8/16/32/64-bit integer leaves; complex, Boolean, Enum, references, fixed-point and extended precision are unsupported.");
    if (!allowComplex && complexType(type))
        return std::string(profile) + " calculation accepts real f32/f64 or 4/8/16/32/64-bit integer leaves only; complex operands are unsupported.";
    return {};
}
inline TypeRef resultLeaf(const TypeRef &type) {
    return TypeRef::primitive(integerWidth(type.name) ? "f64" : type.name);
}
inline TypeRef resultTree(const TypeRef &type) {
    if (!aggregate(type)) return resultLeaf(type);
    auto result = type;
    for (auto &field : result.children) field = resultTree(field);
    return result;
}
inline TypeRef combineTypes(const TypeRef &x, const TypeRef &y, bool scalarLift) {
    if (!aggregate(x) && !aggregate(y)) {
        const auto a = resultLeaf(x), b = resultLeaf(y);
        const bool single = (a.name == "f32" || a.name == "c32") && (b.name == "f32" || b.name == "c32");
        return TypeRef::primitive(complexType(a) || complexType(b) ?
            single ? "c32" : "c64" : single ? "f32" : "f64");
    }
    if (aggregate(x) != aggregate(y)) {
        if (!scalarLift) throw std::runtime_error("Aggregate operands must have the same recursive structure; only a whole scalar operand can be lifted.");
        auto result = aggregate(x) ? x : y;
        for (auto &field : result.children)
            field = aggregate(x) ? combineTypes(field, y, true) : combineTypes(x, field, true);
        return result;
    }
    if (x.kind != y.kind || (x.kind == TypeKind::Array && (x.rank != y.rank || x.extent != y.extent)) ||
        (x.kind == TypeKind::Cluster && (x.fieldIds != y.fieldIds || x.children.size() != y.children.size())))
        throw std::runtime_error("Aggregate operands require equal array ranks/extents and identical ordered Cluster field identities.");
    auto result = x;
    for (std::size_t index = 0; index < result.children.size(); ++index)
        result.children[index] = combineTypes(x.children[index], y.children[index], false);
    return result;
}
}
