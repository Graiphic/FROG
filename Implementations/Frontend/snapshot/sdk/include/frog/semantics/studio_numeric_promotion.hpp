#pragma once
// SDK-free extraction of Studio's existing real/integer type fold. Logical
// I128/U128 intermediates are types only; this header creates no value carrier.
#include <array>
#include <string_view>

namespace frog::runtime::studio_numeric {
struct PhysicalType {enum class Kind {Unknown,Signed,Unsigned,Real};Kind kind=Kind::Unknown;unsigned bits=0;};
[[nodiscard]] inline constexpr PhysicalType physicalType(std::string_view name) noexcept {
    if(name=="f32")return {PhysicalType::Kind::Real,32};if(name=="f64")return {PhysicalType::Kind::Real,64};
    constexpr std::array<unsigned,6> widths{4,8,16,32,64,128};
    constexpr std::array<std::string_view,6> signedNames{"i4","i8","i16","i32","i64","i128"},unsignedNames{"u4","u8","u16","u32","u64","u128"};
    for(unsigned i=0;i<widths.size();++i){if(name==signedNames[i])return {PhysicalType::Kind::Signed,widths[i]};if(name==unsignedNames[i])return {PhysicalType::Kind::Unsigned,widths[i]};}
    return {};
}
[[nodiscard]] inline constexpr bool supportedPhysicalType(std::string_view name) noexcept {
    const auto type=physicalType(name);return type.kind!=PhysicalType::Kind::Unknown && type.bits<=64;
}
[[nodiscard]] inline constexpr std::string_view binaryPromotionType(std::string_view left,std::string_view right) noexcept {
    const auto a=physicalType(left),b=physicalType(right);
    if(a.kind==PhysicalType::Kind::Unknown || b.kind==PhysicalType::Kind::Unknown)return {};
    if(left==right)return left;
    if(a.kind==PhysicalType::Kind::Real || b.kind==PhysicalType::Kind::Real)
        return left=="f64" || right=="f64"?"f64":"f32";
    auto bits=a.bits>b.bits?a.bits:b.bits;
    const bool unsignedResult=a.kind==PhysicalType::Kind::Unsigned && b.kind==PhysicalType::Kind::Unsigned;
    // Preserve the author's established signed/unsigned rank increase, even
    // where a mathematically smaller containing representation could suffice.
    if(a.kind!=b.kind)bits=bits<8?8:bits<16?16:bits<32?32:bits<64?64:128;
    if(a.kind!=b.kind && (a.bits>=bits || b.bits>=bits))bits=bits<128?bits*2:128;
    switch(bits) {case 4:return unsignedResult?"u4":"i4";case 8:return unsignedResult?"u8":"i8";case 16:return unsignedResult?"u16":"i16";
    case 32:return unsignedResult?"u32":"i32";case 64:return unsignedResult?"u64":"i64";default:return unsignedResult?"u128":"i128";}
}
}
