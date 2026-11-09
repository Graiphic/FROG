#pragma once
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <set>
namespace frog::native_engine {
inline constexpr std::string_view canonicalContinuedFractionOwned = "frog.studio.math.discrete.continued_fraction.owned.v1";
inline constexpr std::string_view canonicalPrimeFactorOwned = "frog.studio.math.discrete.prime_factor.owned.v1";
inline std::string_view canonicalDiscreteArrayCatalog(std::string_view id) noexcept {
    if (id == canonicalContinuedFractionOwned) return "math.discrete.continued_fraction";
    if (id == canonicalPrimeFactorOwned) return "math.discrete.prime_factor";
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    return id == "math.discrete.continued_fraction" || id == "math.discrete.prime_factor" ? id : std::string_view{};
}
inline std::string_view canonicalDiscreteArrayMigrationIdentity(std::string_view id) noexcept {
    const auto catalog = canonicalDiscreteArrayCatalog(id);
    return catalog == "math.discrete.continued_fraction" ? canonicalContinuedFractionOwned :
        catalog == "math.discrete.prime_factor" ? canonicalPrimeFactorOwned : std::string_view{};
}
inline bool canonicalDiscreteArraySignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports, std::string *message = nullptr,
    std::string *at = nullptr) {
    const auto reject = [&](std::string text, std::string_view port = {}) {
        if (message) *message = std::move(text); if (at) *at = port; return false;
    };
    const auto catalog = canonicalDiscreteArrayCatalog(id);
    const auto *definition = catalog.empty() ? nullptr : findFunctionBindingDefinition(catalog);
    if (!definition || definition->ports.size() != ports.size()) return reject("Discrete Array signature must preserve every declared connector.");
    std::set<std::pair<bool, std::string>> seen;
    for (const auto &p : ports) {
        const auto f = std::find_if(definition->ports.begin(), definition->ports.end(), [&](const auto &f) { return f.portId == p.id && (f.direction == FunctionPortDirection::Input) == p.input; });
        if (f == definition->ports.end() || !seen.emplace(p.input,p.id).second || (p.input && !p.connected)) return reject("Undeclared, reversed, duplicate or undriven Discrete connector.",p.id);
        const auto expected = binding::parseType(f->defaultValueType);
        if (!expected.valid() || !binding::fullTypeEqual(expected.type,p.type)) return reject("Discrete Array signature must retain its exact type and rank.",p.id);
    }
    return true;
}
}
