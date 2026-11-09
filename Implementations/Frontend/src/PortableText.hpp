#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace frog::native_engine::portable_internal {

inline constexpr std::string_view kAsciiWhitespace = " \t\n\r\f\v";
inline constexpr std::string_view kJsonWhitespace = " \t\n\r";

[[nodiscard]] inline std::string_view trimAsciiWhitespace(
    std::string_view value,
    std::string_view whitespace = kAsciiWhitespace) noexcept {
    const auto first = value.find_first_not_of(whitespace);
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = value.find_last_not_of(whitespace);
    return value.substr(first, last - first + 1U);
}

[[nodiscard]] inline int asciiHexDigit(char value) noexcept {
    if (value >= '0' && value <= '9') {
        return value - '0';
    }
    if (value >= 'a' && value <= 'f') {
        return 10 + (value - 'a');
    }
    if (value >= 'A' && value <= 'F') {
        return 10 + (value - 'A');
    }
    return -1;
}

inline void appendUtf8CodePoint(
    std::string &output,
    std::uint32_t codePoint) {
    if (codePoint <= 0x7FU) {
        output.push_back(static_cast<char>(codePoint));
    } else if (codePoint <= 0x7FFU) {
        output.push_back(static_cast<char>(0xC0U | (codePoint >> 6U)));
        output.push_back(static_cast<char>(0x80U | (codePoint & 0x3FU)));
    } else if (codePoint <= 0xFFFFU) {
        output.push_back(static_cast<char>(0xE0U | (codePoint >> 12U)));
        output.push_back(static_cast<char>(0x80U | ((codePoint >> 6U) & 0x3FU)));
        output.push_back(static_cast<char>(0x80U | (codePoint & 0x3FU)));
    } else if (codePoint <= 0x10FFFFU) {
        output.push_back(static_cast<char>(0xF0U | (codePoint >> 18U)));
        output.push_back(static_cast<char>(0x80U | ((codePoint >> 12U) & 0x3FU)));
        output.push_back(static_cast<char>(0x80U | ((codePoint >> 6U) & 0x3FU)));
        output.push_back(static_cast<char>(0x80U | (codePoint & 0x3FU)));
    }
}

} // namespace frog::native_engine::portable_internal
