#pragma once
#include <array>
#include <string_view>
namespace frog::native_engine {
inline constexpr std::array<std::string_view,2> ellipticParabolicOperationIds{
    "frog.studio.math.special.elliptic_parabolic_functions.jacobi_elliptic",
    "frog.studio.math.special.elliptic_parabolic_functions.parabolic_cylinder"
};
inline constexpr std::array<std::string_view,2> ellipticParabolicNumericKeys{"jacobi_elliptic","parabolic_cylinder"};
inline bool canonicalEllipticParabolicIdentity(std::string_view id) noexcept {
    for (const auto operation : ellipticParabolicOperationIds) if (id == operation) return true;
    return false;
}
inline bool canonicalEllipticParabolicNonFiniteOutput(std::string_view id, std::string_view port) noexcept {
    if (id == ellipticParabolicOperationIds[0])
        return port == "cn" || port == "dn" || port == "sn" || port == "amplitude";
    return id == ellipticParabolicOperationIds[1] && port == "value";
}
} // namespace frog::native_engine
