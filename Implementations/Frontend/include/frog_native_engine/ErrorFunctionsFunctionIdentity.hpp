#pragma once
#include <array>
#include <string_view>
namespace frog::native_engine {
inline constexpr std::array<std::string_view,7> errorFunctionsOperationIds{
    "frog.studio.math.special.error_functions.erf",
    "frog.studio.math.special.error_functions.erfc",
    "frog.studio.math.special.error_functions.erfcx",
    "frog.studio.math.special.error_functions.erfinv",
    "frog.studio.math.special.error_functions.erfcinv",
    "frog.studio.math.special.error_functions.fresnel",
    "frog.studio.math.special.error_functions.dawson"
};
inline constexpr std::array<std::string_view,7> errorFunctionsNumericKeys{"erf","erfc","erfcx","erfinv","erfcinv","fresnel","dawson"};
inline bool canonicalErrorFunctionsIdentity(std::string_view id) noexcept {
    for(const auto operation:errorFunctionsOperationIds)if(id==operation)return true;
    return false;
}
inline bool canonicalErrorFunctionsNonFiniteOutput(std::string_view id,std::string_view port) noexcept {
    if(!canonicalErrorFunctionsIdentity(id))return false;
    if(id==errorFunctionsOperationIds[5])return port=="cosine_integral"||port=="sine_integral";
    return port=="result";
}
} // namespace frog::native_engine
