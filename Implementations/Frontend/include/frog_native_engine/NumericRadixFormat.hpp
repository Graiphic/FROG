#pragma once
#include "frog_native_engine/NumericValueLexeme.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
namespace frog::native_engine {
inline std::optional<std::string> numericRadix128Text(
    std::string_view source, std::string_view valueType, unsigned base,
    bool rawBits, bool uppercase) {
        if ((valueType != "i128" && valueType != "u128") ||
            (base != 2 && base != 8 && base != 16)) return {};
        const auto canonical = numeric_value::canonicalWideIntegerLexeme(source, valueType);
        if (!canonical) return {};
        const bool negative = canonical->starts_with('-');
        auto decimal = std::string_view(*canonical);
        if (negative) decimal.remove_prefix(1);
        std::array<std::uint32_t, 4> limbs{};
        for (const char digit : decimal) {
            std::uint64_t carry = unsigned(digit - '0');
            for (auto &limb : limbs) {
                carry += std::uint64_t(limb) * 10;
                limb = std::uint32_t(carry); carry >>= 32;
            }
        }
        if (negative && rawBits) {
            std::uint64_t carry = 1;
            for (auto &limb : limbs) {
                carry += std::uint64_t(std::uint32_t(~limb));
                limb = std::uint32_t(carry); carry >>= 32;
            }
        }
        const char *digits = uppercase
            ? "0123456789ABCDEF" : "0123456789abcdef";
        std::string result;
        do {
            std::uint64_t remainder = 0;
            for (std::size_t i = limbs.size(); i-- > 0;) {
                const auto part = (remainder << 32) | limbs[i];
                limbs[i] = std::uint32_t(part / base); remainder = part % base;
            }
            result.push_back(digits[remainder]);
        } while (std::any_of(limbs.begin(), limbs.end(), [](auto limb) { return limb != 0; }));
        if (rawBits) {
            const unsigned bitsPerDigit = base == 2 ? 1U : base == 8 ? 3U : 4U;
            result.resize((128U + bitsPerDigit - 1U) / bitsPerDigit, '0');
        } else if (negative) result.push_back('-');
        std::reverse(result.begin(), result.end());
        return result;
}

}
