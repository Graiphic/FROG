#pragma once
#include <array>
#include <string_view>

namespace frog::native_engine {
inline constexpr std::array<std::string_view, 3> gatingOperationNames{"spike", "square", "step"};
// Published Source identities are exact, independently of names or SVG text.
inline bool canonicalGatingIdentity(std::string_view id) noexcept {
    constexpr std::string_view prefix="frog.studio.math.gating.";
    if(!id.starts_with(prefix))return false;
    id.remove_prefix(prefix.size());
    for(const auto name:gatingOperationNames)if(id==name)return true;
    return false;
}
}
