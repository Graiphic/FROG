#pragma once
// Shared Source/FIR contract projection; no numerical evaluator or backend.
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/OrdinaryDifferentialBinding.hpp"
#include "frog_native_engine/PartialDifferentialBinding.hpp"
#include <algorithm>
#include <set>
namespace frog::native_engine {
inline const FunctionBindingDefinition *canonicalDifferentialDefinition(std::string_view id) {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    if (!isOrdinaryDifferentialFunction(id) && !isPartialDifferentialFunction(id)) return nullptr;
    return findFunctionBindingDefinition(id);
}
inline std::string_view canonicalDifferentialMigrationIdentity(std::string_view id) {
    const auto *definition = canonicalDifferentialDefinition(id);
    return definition ? std::string_view(definition->definitionId) : std::string_view{};
}
inline bool canonicalDifferentialSignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports, std::string *message = nullptr,
    std::string *at = nullptr) {
    const auto reject = [&](std::string why, std::string_view port = {}) {
        if (message) *message = std::move(why);
        if (at) *at = port;
        return false;
    };
    const auto *definition = canonicalDifferentialDefinition(id);
    if (!definition || definition->ports.size() != ports.size())
        return reject("Differential equations must retain their complete registered connector signature.");
    std::set<std::pair<bool, std::string>> seen;
    for (const auto &port : ports) {
        const auto found = std::find_if(definition->ports.begin(), definition->ports.end(), [&](const auto &expected) {
            return expected.portId == port.id && (expected.direction == FunctionPortDirection::Input) == port.input;
        });
        if (found == definition->ports.end() || !seen.emplace(port.input, port.id).second)
            return reject("Unknown, reversed or duplicate differential connector.", port.id);
        const auto type = binding::parseType(found->defaultValueType);
        if (!type.valid() || !binding::fullTypeEqual(type.type, port.type))
            return reject("Differential connector must retain its exact complete registered type.", port.id);
        if (port.input && found->requirement == FunctionPortRequirement::Required && !port.connected)
            return reject("A required differential input has no source.", port.id);
    }
    return true;
}
}
