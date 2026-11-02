#pragma once
#include <array>
#include <string_view>
namespace frog::native_engine {
inline constexpr std::array<std::string_view,15> trigonometricOperationNames{
    "sin","cos","tan","csc","sec","cot","acsc","asec","acot","asin","acos","atan","atan2","sincos","sinc"
};
inline bool canonicalTrigonometricIdentity(std::string_view id) noexcept {
    constexpr std::string_view prefix="frog.studio.math.trigonometric.";
    if(!id.starts_with(prefix))return false;
    id.remove_prefix(prefix.size());
    for(const auto name:trigonometricOperationNames)if(id==name)return true;
    return false;
}
inline bool canonicalTrigonometricUnaryIdentity(std::string_view id) noexcept {
    return canonicalTrigonometricIdentity(id) && id!="frog.studio.math.trigonometric.atan2" &&
        id!="frog.studio.math.trigonometric.sincos";
}
inline bool canonicalTrigonometricNonFiniteOutput(std::string_view id,std::string_view port) noexcept {
    if(!canonicalTrigonometricIdentity(id))return false;
    return id=="frog.studio.math.trigonometric.sincos" ? port=="sin"||port=="cos" : port=="result";
}
}
