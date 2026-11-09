#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/BindingType.hpp"
#include "frog/semantics/studio_numeric_promotion.hpp"
#include <algorithm>
#include <array>
#include <iterator>
#include <optional>
#include <string>
#include <vector>

namespace frog::native_engine {
namespace {
struct NumericTypeShape {
    std::string scalarType;
    std::vector<std::optional<std::size_t>> extents;
    bool legacyStudioArray = false;
    binding::TypeRef shape;
};


std::optional<NumericTypeShape> parseNumericTypeShape(
    std::string_view valueType,
    std::size_t depth = 0U) {
    const auto parsedType = binding::parseType(valueType);
    if (!parsedType.valid() || !parsedType.type.known() || depth > 32U) {
        return std::nullopt;
    }
    NumericTypeShape result;
    result.shape = parsedType.type;
    auto *leaf = &result.shape;
    while (leaf->kind == binding::TypeKind::Array) {
        for (std::size_t index = 0; index < leaf->rank; ++index) result.extents.push_back(leaf->extent);
        leaf = &leaf->children.front();
    }
    // Recognition is distinct from an installed arithmetic policy. Never let
    // resolveNumericBinaryValueType's legacy scalar fallback invent f64 here.
    if (!leaf->numeric() || !isNumericScalarValueType(leaf->name)) return std::nullopt;
    result.scalarType = leaf->name;
    result.legacyStudioArray = valueType.starts_with("array:") && result.extents.size() == 1U;
    *leaf = binding::TypeRef::primitive("f64");
    return result;
}


} // namespace

FunctionTypeResolutionResult resolveNumericAddValueTypes(
    std::span<const std::string_view> operandTypes) {
    FunctionTypeResolutionResult result;
    if (operandTypes.empty()) {
        return result;
    }

    std::optional<binding::TypeRef> arrayShape;
    bool preserveLegacyStudioArray = true;
    std::string promotedScalar;
    for (std::size_t index = 0U; index < operandTypes.size(); ++index) {
        const auto parsed = parseNumericTypeShape(operandTypes[index]);
        if (!parsed.has_value()) {
            result.status = FunctionTypeResolutionStatus::InvalidOperand;
            result.operandIndex = index;
            return result;
        }
        if (!parsed->extents.empty()) {
            if (arrayShape.has_value() && arrayShape.value() != parsed->shape) {
                result.status = FunctionTypeResolutionStatus::IncompatibleArrayShape;
                result.operandIndex = index;
                return result;
            }
            arrayShape = parsed->shape;
            preserveLegacyStudioArray = preserveLegacyStudioArray &&
                parsed->legacyStudioArray;
        }
        promotedScalar = promotedScalar.empty()
            ? parsed->scalarType
            : resolveNumericBinaryValueType(promotedScalar, parsed->scalarType);
    }

    result.status = FunctionTypeResolutionStatus::Resolved;
    if (arrayShape) {
        auto *leaf = &*arrayShape;
        while (leaf->kind == binding::TypeKind::Array) leaf = &leaf->children.front();
        *leaf = binding::TypeRef::primitive(promotedScalar);
        result.resolvedValueType = preserveLegacyStudioArray ? "array:" + promotedScalar : arrayShape->canonical();
    } else result.resolvedValueType = promotedScalar;
    return result;
}

bool isNumericScalarValueType(std::string_view valueType) noexcept {
    static constexpr std::array<std::string_view, 26> numericTypes{ {
        "f16", "bf16", "f32", "f64", "i4", "i8", "i16", "i32", "i64",
        "u4", "u8", "u16", "u32", "u64", "c32", "c64", "fxp", "decimal",
        "f8e4m3", "f8e5m2", "i128", "u128", "f80", "f128", "biguint",
        "cfx"
    } };
    return std::find(numericTypes.begin(), numericTypes.end(), valueType) != numericTypes.end();
}

bool numericConnectionRequiresCoercion(
    std::string_view sourceType,
    std::string_view targetType) noexcept {
    return sourceType != targetType &&
        isNumericScalarValueType(sourceType) &&
        isNumericScalarValueType(targetType);
}

std::string resolveNumericBinaryValueType(
    std::string_view left,
    std::string_view right) {
    // This SDK-free subset is shared with the versioned native owned Add
    // kernel. All other established representations keep the existing law.
    if(const auto shared=frog::runtime::studio_numeric::binaryPromotionType(left,right);!shared.empty())
        return std::string(shared);
    if (!isNumericScalarValueType(left)) {
        left = "f64";
    }
    if (!isNumericScalarValueType(right)) {
        right = "f64";
    }
    if (left == right) {
        return std::string(left);
    }

    const auto isOneOf = [](std::string_view value, auto... candidates) {
        return ((value == candidates) || ...);
    };
    if (isOneOf(left, "cfx") || isOneOf(right, "cfx")) {
        return "cfx";
    }
    if (isOneOf(left, "c32", "c64") || isOneOf(right, "c32", "c64")) {
        const bool requires64 = isOneOf(left, "c64", "f64", "f80", "f128", "decimal") ||
            isOneOf(right, "c64", "f64", "f80", "f128", "decimal");
        return requires64 ? "c64" : "c32";
    }
    if (left == "decimal" || right == "decimal") {
        return "decimal";
    }

    static constexpr std::array<std::string_view, 8> floatingByRank{ {
        "f8e4m3", "f8e5m2", "f16", "bf16", "f32", "f64", "f80", "f128"
    } };
    const auto floatRank = [](std::string_view value) {
        const auto found = std::find(floatingByRank.begin(), floatingByRank.end(), value);
        return found == floatingByRank.end()
            ? -1
            : static_cast<int>(std::distance(floatingByRank.begin(), found));
    };
    const int leftFloatRank = floatRank(left);
    const int rightFloatRank = floatRank(right);
    if (leftFloatRank >= 0 || rightFloatRank >= 0) {
        return std::string(floatingByRank[static_cast<std::size_t>((std::max)(
            leftFloatRank,
            rightFloatRank))]);
    }
    if (left == "fxp" || right == "fxp") {
        return "fxp";
    }

    const auto integerBits = [](std::string_view value) {
        if (value.ends_with("128")) return 128;
        if (value.ends_with("64")) return 64;
        if (value.ends_with("32")) return 32;
        if (value.ends_with("16")) return 16;
        if (value.ends_with("8")) return 8;
        return 4;
    };
    const bool leftUnsigned = left.starts_with('u') || left == "biguint";
    const bool rightUnsigned = right.starts_with('u') || right == "biguint";
    if (left == "biguint" || right == "biguint") {
        return leftUnsigned && rightUnsigned ? "biguint" : "f128";
    }
    int bits = (std::max)(integerBits(left), integerBits(right));
    if (leftUnsigned != rightUnsigned) {
        bits = bits < 8 ? 8 : bits < 16 ? 16 : bits < 32 ? 32 : bits < 64 ? 64 : 128;
        if (integerBits(left) >= bits || integerBits(right) >= bits) {
            bits = bits < 128 ? bits * 2 : 128;
        }
        return bits == 128 ? "i128" : "i" + std::to_string(bits);
    }
    const char prefix = leftUnsigned ? 'u' : 'i';
    return std::string(1, prefix) + std::to_string(bits);
}

} // namespace frog::native_engine
