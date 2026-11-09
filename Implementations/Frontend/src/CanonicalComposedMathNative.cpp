#include "frog_native_engine/CanonicalComposedMathNative.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/HyperbolicFunctionIdentity.hpp"
#include "frog_native_engine/TrigonometricReal64.hpp"
#include <algorithm>
#include <set>

namespace frog::native_engine {
namespace {
std::string_view catalog(std::string_view id) noexcept {
    for(const auto &entry:trigonometricReal64Contracts)
        if(id==entry.catalogId||id==entry.operationId)return entry.catalogId;
    for(const auto &entry:hyperbolicReal64Contracts)
        if(id==entry.catalogId||id==entry.operationId)return entry.catalogId;
    return {};
}
}
std::string_view canonicalComposedMathMigrationIdentity(std::string_view id) noexcept {
    for(const auto &entry:trigonometricReal64Contracts)
        if(id==entry.catalogId||id==entry.operationId)return entry.operationId;
    for(const auto &entry:hyperbolicReal64Contracts)
        if(id==entry.catalogId||id==entry.operationId)return entry.operationId;
    return {};
}
bool canonicalComposedMathSignatureValid(std::string_view id,
    std::span<const CanonicalComposedMathPort> actual,std::string *message,std::string *port) {
    if(message)message->clear();if(port)port->clear();
    const auto reject=[&](std::string reason,std::string_view connector={}) {
        if(message)*message=std::move(reason);if(port)*port=connector;return false;
    };
    const auto identity=catalog(id);
    const auto *definition=identity.empty()?nullptr:findFunctionBindingDefinition(identity);
    if(!definition)return reject("No existing exact Trigonometric/Hyperbolic scalar contract identifies this operation.");
    if(actual.size()!=definition->ports.size())return reject("The composed F64 profile requires all and only the existing fixed connectors.");
    std::set<std::pair<bool,std::string>> seen;
    const auto f64=binding::TypeRef::primitive("f64");
    for(const auto &value:actual) {
        if(!seen.emplace(value.input,value.id).second)return reject("Duplicate composed math connector.",value.id);
        const auto expected=std::find_if(definition->ports.begin(),definition->ports.end(),[&](const auto &candidate) {
            return candidate.portId==value.id&&(candidate.direction==FunctionPortDirection::Input)==value.input;
        });
        if(expected==definition->ports.end())return reject("A connector is absent from the existing math contract.",value.id);
        if(expected->cardinality!=FunctionPortCardinality::Single||expected->defaultValueType!="f64")
            return reject("The existing math signature no longer describes the fixed scalar F64 profile.",value.id);
        if(!binding::fullTypeEqual(value.type,f64))
            return reject("Composed math requires exact primitive F64 inputs and outputs; no conversion or aggregate lifting is implied.",value.id);
        if(value.input&&!value.connected)return reject("Each math input requires its actual source; no unwired default is synthesized.",value.id);
    }
    return true;
}
}
