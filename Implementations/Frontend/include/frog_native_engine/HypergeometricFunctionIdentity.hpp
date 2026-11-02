#pragma once
#include <array>
#include <string_view>

namespace frog::native_engine {
inline constexpr std::array<std::string_view,3> hypergeometricOperationIds{
    "frog.studio.math.hypergeometric.gauss",
    "frog.studio.math.hypergeometric.kummer_m",
    "frog.studio.math.hypergeometric.tricomi_u"
};
inline constexpr std::array<std::string_view,3> hypergeometricNumericKeys{"gauss","kummer","tricomi"};
inline bool canonicalHypergeometricIdentity(std::string_view id) noexcept {
    for(const auto operation:hypergeometricOperationIds)if(id==operation)return true;
    return false;
}
inline bool canonicalHypergeometricNonFiniteOutput(std::string_view id,std::string_view port) noexcept {
    return port=="value"&&canonicalHypergeometricIdentity(id);
}
}
