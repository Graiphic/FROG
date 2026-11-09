#pragma once
#include <array>
#include <string_view>
namespace frog::native_engine {
inline constexpr std::array<std::string_view,6> exponentialIntegralsOperationIds{
    "frog.studio.math.special.exponential_integrals.expn",
    "frog.studio.math.special.exponential_integrals.si",
    "frog.studio.math.special.exponential_integrals.ci",
    "frog.studio.math.special.exponential_integrals.dilogarithm",
    "frog.studio.math.special.exponential_integrals.shi",
    "frog.studio.math.special.exponential_integrals.chi"
};
inline constexpr std::array<std::string_view,6> exponentialIntegralsNumericKeys{"expn","si","ci","dilogarithm","shi","chi"};
inline bool canonicalExponentialIntegralsIdentity(std::string_view id) noexcept {
    for(const auto operation:exponentialIntegralsOperationIds)if(id==operation)return true;
    return false;
}
inline bool canonicalExponentialIntegralsNonFiniteOutput(std::string_view id,std::string_view port) noexcept {
    return port=="result"&&canonicalExponentialIntegralsIdentity(id);
}
}
