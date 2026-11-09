#pragma once
#include <array>
#include <optional>
#include <string_view>

namespace frog::native_engine {
inline constexpr std::array<std::string_view,4> ellipticOperationIds{
    "frog.studio.math.elliptic.first_kind.complete_k",
    "frog.studio.math.elliptic.first_kind.incomplete_f",
    "frog.studio.math.elliptic.second_kind.complete_e",
    "frog.studio.math.elliptic.second_kind.incomplete_e"
};
inline constexpr std::array<std::string_view,4> ellipticNumericKeys{
    "complete_k","incomplete_f","complete_e","incomplete_e"
};
inline bool canonicalEllipticIdentity(std::string_view id) noexcept {
    for(const auto operation:ellipticOperationIds)if(id==operation)return true;
    return false;
}
inline bool canonicalEllipticNonFiniteOutput(std::string_view id,std::string_view port) noexcept {
    return port=="value"&&canonicalEllipticIdentity(id);
}
inline std::optional<double> canonicalEllipticInputDefault(std::string_view id,std::string_view port) noexcept {
    if(port=="a"&&(id==ellipticOperationIds[1]||id==ellipticOperationIds[3]))return 0x1.921fb54442d18p+0;
    return {};
}
inline bool canonicalEllipticOptionalInput(std::string_view id,std::string_view port) noexcept {
    return canonicalEllipticInputDefault(id,port).has_value();
}
}
