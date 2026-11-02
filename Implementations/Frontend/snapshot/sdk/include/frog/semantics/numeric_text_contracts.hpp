#pragma once
#include <algorithm>
#include <array>
#include <string_view>

namespace frog::runtime::numeric_text {
enum class Kind {Automatic,Decimal,Hexadecimal,Octal,Fixed,Scientific,Engineering,ParseDecimal,ParseHexadecimal,ParseOctal,ParseReal,StringToBytes,BytesToString};
struct Contract {std::string_view catalog,id;Kind kind;bool aggregate=true;};
inline constexpr std::string_view profile="frog.studio.numeric.unary-text.ascii-dot-prefix@1";
inline constexpr std::array<Contract,13> contracts{{
    {"numeric.conversion.format_into_string","frog.studio.numeric.conversion.format_into_string.unary-typed.v1",Kind::Automatic},
    {"numeric.conversion.number_to_decimal_string","frog.studio.numeric.conversion.number_to_decimal_string.unary-typed.v1",Kind::Decimal},
    {"numeric.conversion.number_to_hex_string","frog.studio.numeric.conversion.number_to_hex_string.unary-typed.v1",Kind::Hexadecimal},
    {"numeric.conversion.number_to_octal_string","frog.studio.numeric.conversion.number_to_octal_string.unary-typed.v1",Kind::Octal},
    {"numeric.conversion.number_to_fractional_string","frog.studio.numeric.conversion.number_to_fractional_string.unary-typed.v1",Kind::Fixed},
    {"numeric.conversion.number_to_exponential_string","frog.studio.numeric.conversion.number_to_exponential_string.unary-typed.v1",Kind::Scientific},
    {"numeric.conversion.number_to_engineering_string","frog.studio.numeric.conversion.number_to_engineering_string.unary-typed.v1",Kind::Engineering},
    {"numeric.conversion.decimal_string_to_number","frog.studio.numeric.conversion.decimal_string_to_number.unary-typed.v1",Kind::ParseDecimal},
    {"numeric.conversion.hex_string_to_number","frog.studio.numeric.conversion.hex_string_to_number.unary-typed.v1",Kind::ParseHexadecimal},
    {"numeric.conversion.octal_string_to_number","frog.studio.numeric.conversion.octal_string_to_number.unary-typed.v1",Kind::ParseOctal},
    {"numeric.conversion.floating_string_to_number","frog.studio.numeric.conversion.floating_string_to_number.unary-typed.v1",Kind::ParseReal},
    // Existing String operations, never duplicated in numeric_aggregate.
    {"numeric.conversion.string_to_byte_array","frog.studio.array.conversion.string_to_byte_array.bytes.v1",Kind::StringToBytes,false},
    {"numeric.conversion.byte_array_to_string","frog.studio.array.conversion.byte_array_to_string.bytes.v1",Kind::BytesToString,false}
}};
[[nodiscard]] inline const Contract *contract(std::string_view id) noexcept {
    for(const auto &c:contracts)if(c.catalog==id||c.id==id)return &c;return nullptr;
}
[[nodiscard]] inline bool supports(std::string_view id) noexcept {const auto *c=contract(id);return c&&c->aggregate;}
inline constexpr auto ids=[] {std::array<std::string_view,11> result{};std::size_t at=0;for(const auto &c:contracts)if(c.aggregate)result[at++]=c.id;return result;}();
[[nodiscard]] inline std::string_view operationIdForCatalog(std::string_view id) noexcept {const auto *c=contract(id);return c&&c->aggregate?c->id:std::string_view{};}
[[nodiscard]] inline bool numeric(std::string_view name) noexcept {
    constexpr std::array names{"i4","u4","i8","i16","i32","i64","u8","u16","u32","u64","f32","f64"};
    return std::find(names.begin(),names.end(),name)!=names.end();
}
[[nodiscard]] inline bool parses(Kind k) noexcept {return k==Kind::ParseDecimal||k==Kind::ParseHexadecimal||k==Kind::ParseOctal||k==Kind::ParseReal;}
[[nodiscard]] inline std::string_view outputLeaf(const Contract &c) noexcept {
    return c.kind==Kind::ParseDecimal?"i32":c.kind==Kind::ParseHexadecimal||c.kind==Kind::ParseOctal?"u32":c.kind==Kind::ParseReal?"f64":"string";
}
}
