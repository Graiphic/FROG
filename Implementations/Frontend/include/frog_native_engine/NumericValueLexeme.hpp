#pragma once
#include <array>
#include <limits>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

// Shared exact numeric lexical transport. This does not declare a language
// conversion, Data Entry policy, a public Source validator or a runtime value.
namespace frog::native_engine::numeric_value {
inline constexpr std::size_t maximumDecimalNumberBytes=64U*1024U*1024U;
// Exact decimal token scanning shared with the source JSON reader. This is
// lexical transport only: no widgets, document codec, geometry or execution.
inline std::optional<std::size_t> scanDecimalNumber(std::string_view text) {
    std::size_t index=0;
    const auto digit=[&] { return index<text.size() && text[index]>='0' && text[index]<='9'; };
    if(index<text.size() && text[index]=='-')++index;
    if(!digit())return {};
    if(text[index]=='0')++index;else while(digit())++index;
    if(digit())return {};
    if(index<text.size() && text[index]=='.') {
        ++index;if(!digit())return {};while(digit())++index;
    }
    if(index<text.size() && (text[index]=='e' || text[index]=='E')) {
        ++index;if(index<text.size() && (text[index]=='+' || text[index]=='-'))++index;
        if(!digit())return {};while(digit())++index;
    }
    return index;
}

inline std::optional<std::string> canonicalWideIntegerLexeme(
    std::string_view text,std::string_view valueType) {
    const bool signedType=valueType=="i64" || valueType=="i128";
    const std::string_view positive=valueType=="u64"?"18446744073709551615":
        valueType=="i64"?"9223372036854775807":
        valueType=="u128"?"340282366920938463463374607431768211455":
        valueType=="i128"?"170141183460469231731687303715884105727":"";
    const std::string_view negativeLimit=valueType=="i64"?"9223372036854775808":
        valueType=="i128"?"170141183460469231731687303715884105728":"";
    if(positive.empty())return {};
    if(text.size()>maximumDecimalNumberBytes)return {};
    const auto consumed=scanDecimalNumber(text);
    if(!consumed || *consumed!=text.size())return {};
    const bool negative=text.starts_with('-');
    if(negative)text.remove_prefix(1);
    const auto exponentPosition=text.find_first_of("eE");
    const auto mantissa=text.substr(0,exponentPosition);
    std::int64_t exponent=0;
    if(exponentPosition!=text.npos) {
        auto exponentText=text.substr(exponentPosition+1);
        if(exponentText.starts_with('+'))exponentText.remove_prefix(1);
        const auto converted=std::from_chars(exponentText.data(),exponentText.data()+exponentText.size(),exponent);
        if(converted.ec!=std::errc{} || converted.ptr!=exponentText.data()+exponentText.size())return {};
    }
    std::string digits;
    std::int64_t fractional=0;
    bool fraction=false;
    for(const char character:mantissa) {
        if(character=='.'){fraction=true;continue;}
        digits+=character;
        if(fraction)++fractional;
    }
    const auto significant=digits.find_first_not_of('0');
    if(significant==digits.npos)return std::string{"0"};
    if(negative && !signedType)return {};
    digits.erase(0,significant);
    const auto limit=negative?negativeLimit:positive;
    // Bound by the actual fixed-width domain before allocating expansion.
    if(exponent>=fractional) {
        const auto zeros=static_cast<std::uint64_t>(exponent-fractional);
        if(digits.size()>limit.size() || zeros>limit.size()-digits.size())return {};
        digits.append(static_cast<std::size_t>(zeros),'0');
    } else {
        // Avoid signed overflow for an extreme negative exponent.
        if(exponent>=0) {
            const auto remove=static_cast<std::size_t>(fractional-exponent);
            if(remove>=digits.size() || digits.find_first_not_of('0',digits.size()-remove)!=digits.npos)return {};
            digits.resize(digits.size()-remove);
        } else {
            const auto removed=std::uint64_t(fractional)+std::uint64_t(-(exponent+1))+1U;
            if(removed>=digits.size() || digits.find_first_not_of('0',digits.size()-static_cast<std::size_t>(removed))!=digits.npos)return {};
            digits.resize(digits.size()-static_cast<std::size_t>(removed));
        }
    }
    if(digits.size()>limit.size() || (digits.size()==limit.size() && digits>limit))return {};
    if(negative)digits.insert(0,1,'-');
    return digits;
}

inline std::optional<double> wideIntegerCacheProjection(std::string_view text,std::string_view valueType) {
    if(!canonicalWideIntegerLexeme(text,valueType))return {};
    double projection=0;
    const auto converted=std::from_chars(text.data(),text.data()+text.size(),projection);
    if(converted.ec!=std::errc{} || converted.ptr!=text.data()+text.size() || !std::isfinite(projection))return {};
    return projection;
}


// Private editor transport, not a public literal grammar or execution value.
// The complete token is validated before either component can be published.
struct ComplexNumber { double real = 0.0; double imaginary = 0.0; };
inline std::string_view trimComplexNumberToken(std::string_view text) noexcept {
    constexpr std::string_view spaces = " \t\r\n";
    const auto first = text.find_first_not_of(spaces);
    return first == text.npos ? std::string_view{} :
        text.substr(first, text.find_last_not_of(spaces) - first + 1);
}
inline std::optional<ComplexNumber> parseComplexNumberImpl(std::string_view text, std::string_view type, bool runtimeIeee) {
    if ((type != "c32" && type != "c64") || text.size() > maximumDecimalNumberBytes) return {};
    text = trimComplexNumberToken(text);
    if (text.empty()) return {};
    const auto component = [runtimeIeee,type](std::string_view token) -> std::optional<double> {
        token = trimComplexNumberToken(token);
        const bool negative = token.starts_with('-');
        if (negative || token.starts_with('+')) token = trimComplexNumberToken(token.substr(1));
        if (token.empty() || token.starts_with('+') || token.starts_with('-')) return {};
        if(type=="c32") {
            float value=0;
            const auto parsed=std::from_chars(token.data(),token.data()+token.size(),value);
            if(parsed.ec!=std::errc{}||parsed.ptr!=token.data()+token.size()||(!runtimeIeee&&!std::isfinite(value)))return {};
            return negative?-static_cast<double>(value):static_cast<double>(value);
        }
        double value = 0.0;
        const auto parsed = std::from_chars(token.data(), token.data() + token.size(), value);
        if (parsed.ec != std::errc{} || parsed.ptr != token.data() + token.size() || (!runtimeIeee && !std::isfinite(value))) return {};
        return negative ? -value : value;
    };
    ComplexNumber result;
    if (text.ends_with('i')) {
        text.remove_suffix(1);
        text = trimComplexNumberToken(text);
        std::size_t separator = text.npos;
        for (std::size_t i = 1; i < text.size(); ++i) {
            if ((text[i] == '+' || text[i] == '-') && text[i - 1] != 'e' && text[i - 1] != 'E') {
                if (separator != text.npos) return {};
                separator = i;
            }
        }
        auto imaginary = separator == text.npos ? text : text.substr(separator);
        if (separator != text.npos) {
            const auto real = component(text.substr(0, separator));
            if (!real) return {};
            result.real = *real;
        }
        imaginary = trimComplexNumberToken(imaginary);
        if (imaginary.empty() || imaginary == "+") result.imaginary = 1.0;
        else if (imaginary == "-") result.imaginary = -1.0;
        else {
            const auto number = component(imaginary);
            if (!number) return {};
            result.imaginary = *number;
        }
    } else {
        const auto real = component(text);
        if (!real) return {};
        result.real = *real;
    }
    if (type == "c32") {
        const double maximum = (std::numeric_limits<float>::max)();
        if ((std::isfinite(result.real) && std::abs(result.real) > maximum) ||
            (std::isfinite(result.imaginary) && std::abs(result.imaginary) > maximum)) return {};
        result.real = static_cast<double>(static_cast<float>(result.real));
        result.imaginary = static_cast<double>(static_cast<float>(result.imaginary));
    }
    return result;
}
inline std::optional<ComplexNumber> parseComplexNumber(std::string_view text, std::string_view type) {
    return parseComplexNumberImpl(text, type, false);
}
// Runtime IEEE values are separate from the finite editor literal grammar.
inline std::optional<ComplexNumber> parseRuntimeComplexNumber(std::string_view text, std::string_view type) {
    return parseComplexNumberImpl(text, type, true);
}
inline std::optional<std::string> runtimeComplexNumberLexeme(ComplexNumber value, std::string_view type) {
    if (type != "c32" && type != "c64") return {};
    const auto encode = [type](double number) -> std::optional<std::string> {
        std::array<char, 64> buffer{};
        if (type == "c32" && std::isfinite(number) && std::abs(number) > (std::numeric_limits<float>::max)()) return {};
        const auto result = type == "c32"
            ? std::to_chars(buffer.data(), buffer.data() + buffer.size(), static_cast<float>(number), std::chars_format::general)
            : std::to_chars(buffer.data(), buffer.data() + buffer.size(), number, std::chars_format::general);
        if (result.ec != std::errc{}) return {};
        return std::string(buffer.data(), result.ptr);
    };
    const auto real = encode(value.real), imaginary = encode(std::abs(value.imaginary));
    if (!real || !imaginary) return {};
    return *real + (std::signbit(value.imaginary) ? "-" : "+") + *imaginary + "i";
}
inline std::optional<std::string> complexNumberLexeme(ComplexNumber value, std::string_view type) {
    if (!std::isfinite(value.real) || !std::isfinite(value.imaginary)) return {};
    return runtimeComplexNumberLexeme(value, type);
}

} // namespace frog::native_engine::numeric_value
