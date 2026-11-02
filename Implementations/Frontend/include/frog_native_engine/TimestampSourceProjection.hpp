#pragma once
// Exact projection of the CURRENT widget's binary64 seconds to the existing
// signed64.64 Source carrier. It creates no extra precision or persisted state.
#include "frog_native_engine/FrogJson.hpp"
#include <bit>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <optional>
#include <string>

namespace frog::native_engine::timestamp_source {
struct Parts {std::int64_t seconds;std::uint64_t fraction;};
inline std::optional<Parts> fromBinary64(double value)noexcept{
    if(!std::isfinite(value)||value>=0x1p63||value< -0x1p63)return {};
    const auto encoded=std::bit_cast<std::uint64_t>(value);const bool negative=(encoded>>63)!=0;
    const auto exponent=static_cast<unsigned>((encoded>>52)&2047);auto significand=encoded&UINT64_C(0x000fffffffffffff);
    if(exponent)significand|=UINT64_C(0x0010000000000000);
    if(!significand)return Parts{0,0};
    const int shift=(exponent?static_cast<int>(exponent)-1023-52:-1074)+64;
    std::uint64_t high=0,low=0;
    if(shift>=64)high=significand<<static_cast<unsigned>(shift-64);
    else if(shift>=0){low=significand<<static_cast<unsigned>(shift);high=shift?significand>>(64-static_cast<unsigned>(shift)):0;}
    else {const auto right=static_cast<unsigned>(-shift);
        // Refuse bits finer than the target's64 fractional positions. Silent
        // truncation/rounding here would invent a different widget value.
        if(right>=64||(significand&((UINT64_C(1)<<right)-1))!=0)return {};
        low=significand>>right;
    }
    if(negative){high=~high+(low==0?1:0);low=~low+1;}
    return Parts{std::bit_cast<std::int64_t>(high),low};
}
inline std::optional<source::JsonValue> json(double value){
    const auto parts=fromBinary64(value);if(!parts)return {};
    source::JsonValue result;result.type=source::JsonValue::Type::Object;
    source::JsonValue seconds,fraction;seconds.type=fraction.type=source::JsonValue::Type::String;
    seconds.string=std::to_string(parts->seconds);fraction.string=std::to_string(parts->fraction);
    result.object={{"seconds",std::move(seconds)},{"fraction",std::move(fraction)}};return result;
}
inline std::optional<source::JsonValue> lexical(std::string_view token){
    double value=0;const auto parsed=std::from_chars(token.data(),token.data()+token.size(),value);
    if(parsed.ec!=std::errc{}||parsed.ptr!=token.data()+token.size())return {};return json(value);
}
}
