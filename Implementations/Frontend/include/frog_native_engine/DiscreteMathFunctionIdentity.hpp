#pragma once
#include <array>
#include <string_view>

namespace frog::native_engine {
inline constexpr std::array<std::string_view, 8> discreteMathOperationNames{
    "gcd", "lcm", "factorial", "prime_factor", "check_prime", "permute", "binomial", "continued_fraction"
};
// Admission depends on exact public identities, never translated names or SVGs.
inline bool canonicalDiscreteMathIdentity(std::string_view id) noexcept {
    constexpr std::string_view prefix = "frog.studio.math.discrete.";
    if (!id.starts_with(prefix)) return false;
    id.remove_prefix(prefix.size());
    for (const auto name : discreteMathOperationNames) if (id == name) return true;
    return false;
}
inline bool canonicalDiscreteMathScalarIdentity(std::string_view id) noexcept {
    return canonicalDiscreteMathIdentity(id) &&
        id != "frog.studio.math.discrete.prime_factor" &&
        id != "frog.studio.math.discrete.continued_fraction";
}
inline bool canonicalDiscreteMathNonFiniteOutput(std::string_view id, std::string_view port) noexcept {
    return ((id == "frog.studio.math.discrete.factorial" || id == "frog.studio.math.discrete.permute") && port == "result") ||
        (id == "frog.studio.math.discrete.binomial" && port == "coefficient");
}
}
