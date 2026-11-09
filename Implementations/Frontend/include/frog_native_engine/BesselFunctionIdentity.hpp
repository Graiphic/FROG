#pragma once
#include <array>
#include <string_view>

namespace frog::native_engine {
inline constexpr std::array<std::string_view,12> besselOperationNames{
    "bessel_cyl_j","bessel_cyl_y","hankel_cyl","bessel_mod_i","bessel_mod_k",
    "bessel_sph_j","bessel_sph_y","hankel_sph","airy_ai_bi","struve_h","kelvin_be","kelvin_ke"
};
inline bool canonicalBesselIdentity(std::string_view id) noexcept {
    constexpr std::string_view prefix="frog.studio.math.special.";
    if(!id.starts_with(prefix))return false;
    id.remove_prefix(prefix.size());
    for(const auto name:besselOperationNames)if(id==name)return true;
    return false;
}
inline bool canonicalBesselComplexIdentity(std::string_view id) noexcept {
    return id=="frog.studio.math.special.hankel_cyl"||id=="frog.studio.math.special.hankel_sph"||
        id=="frog.studio.math.special.kelvin_be"||id=="frog.studio.math.special.kelvin_ke";
}
inline bool canonicalBesselNonFiniteOutput(std::string_view id,std::string_view port) noexcept {
    return canonicalBesselIdentity(id)&&!canonicalBesselComplexIdentity(id)&&
        (id=="frog.studio.math.special.airy_ai_bi"?(port=="ai"||port=="bi"):port=="value");
}
}
