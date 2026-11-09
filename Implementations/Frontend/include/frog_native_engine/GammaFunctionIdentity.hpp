#pragma once
#include <array>
#include <limits>
#include <optional>
#include <string_view>

namespace frog::native_engine {
inline constexpr std::array<std::string_view,9> gammaOperationIds{
    "frog.studio.math.gamma.complete","frog.studio.math.gamma.regularized_lower",
    "frog.studio.math.gamma.stirling_leading","frog.studio.math.beta.complete",
    "frog.studio.math.beta.regularized_lower","frog.studio.math.gamma.log_factorial",
    "frog.studio.math.gamma.log_complete","frog.studio.math.gamma.regularized_upper",
    "frog.studio.math.gamma.digamma"
};
inline bool canonicalGammaIdentity(std::string_view id) noexcept {
    for(const auto operation:gammaOperationIds)if(id==operation)return true;
    return false;
}
inline bool canonicalGammaNonFiniteOutput(std::string_view id,std::string_view port) noexcept {
    return port=="value"&&canonicalGammaIdentity(id);
}
inline bool canonicalGammaInfiniteLimit(std::string_view id,std::string_view port) noexcept {
    return port=="a"&&(id=="frog.studio.math.gamma.regularized_lower"||id=="frog.studio.math.gamma.regularized_upper");
}
inline std::optional<double> canonicalGammaInputDefault(std::string_view id,std::string_view port) noexcept {
    if(port!="a")return {};
    if(id=="frog.studio.math.gamma.regularized_lower")return std::numeric_limits<double>::infinity();
    if(id=="frog.studio.math.beta.regularized_lower")return 1.;
    return {};
}
inline bool canonicalGammaOptionalInput(std::string_view id,std::string_view port) noexcept {
    return canonicalGammaInputDefault(id,port).has_value();
}
}
