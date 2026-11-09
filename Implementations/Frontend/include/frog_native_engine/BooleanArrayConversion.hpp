#pragma once

#include <string_view>

namespace frog::native_engine {

enum class BooleanArraySignExtensionMode { Never, IfOutputIsSigned, Always };

inline constexpr bool isBooleanArrayToNumber(std::string_view type) noexcept {
    if (type.starts_with("frog.studio.")) type.remove_prefix(12);
    return type == "boolean.array_to_number" ||
        type == "numeric.conversion.boolean_array_to_number" ||
        type == "array.conversion.boolean_array_to_number";
}

inline constexpr std::string_view booleanArraySignExtensionModeToken(
    BooleanArraySignExtensionMode mode) noexcept {
    switch (mode) {
    case BooleanArraySignExtensionMode::Never: return "never";
    case BooleanArraySignExtensionMode::Always: return "always";
    default: return "if_output_is_signed";
    }
}

inline constexpr BooleanArraySignExtensionMode parseBooleanArraySignExtensionMode(
    std::string_view token) noexcept {
    if (token == "never") return BooleanArraySignExtensionMode::Never;
    if (token == "always") return BooleanArraySignExtensionMode::Always;
    // Missing/unknown metadata uses the documented legacy default.
    return BooleanArraySignExtensionMode::IfOutputIsSigned;
}

// Runtime contract: element zero is the least significant bit. Keep only
// the output's bit width, and pad a shorter array with this bit. For an empty
// array, the caller supplies false as lastElement. The output representation
// supplies its signedness, including fixed-point representations.
inline constexpr bool booleanArraySignExtensionBit(BooleanArraySignExtensionMode mode,
    bool outputIsSigned, bool lastElement) noexcept {
    return lastElement && (mode == BooleanArraySignExtensionMode::Always ||
        (mode == BooleanArraySignExtensionMode::IfOutputIsSigned && outputIsSigned));
}

} // namespace frog::native_engine
