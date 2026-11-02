#pragma once
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>

namespace frog::native_engine::numeric_text {
// Shared portable presentation only. No widget/model, Runtime state or I/O.
inline std::string trimTrailingZeros(std::string text) {
    const auto exponent = text.find_first_of("eE");
    const auto mantissaEnd = exponent == std::string::npos ? text.size() : exponent;
    const auto decimal = text.find('.');
    if (decimal == std::string::npos || decimal >= mantissaEnd) {
        return text;
    }
    std::size_t trim = mantissaEnd;
    while (trim > decimal + 1U && text[trim - 1U] == '0') {
        --trim;
    }
    if (trim == decimal + 1U) {
        --trim;
    }
    text.erase(trim, mantissaEnd - trim);
    return text;
}

// The caller retains its existing precision/domain/style admission policy.
inline std::string automaticNotation(double value, int digits, bool showPoint) {
    std::ostringstream output;
    output.imbue(std::locale::classic());
    if (showPoint) {
        output << std::showpoint;
    }
    output << std::setprecision(digits) << std::defaultfloat << value;
    return output.str();
}
}
