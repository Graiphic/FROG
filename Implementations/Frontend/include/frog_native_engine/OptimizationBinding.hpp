#pragma once
// Source/FIR and authoring projection only. Numerical algorithms are owned by
// Runtime; a strict callable describes a FROG interface, never a UI reference.
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>
#include <set>
namespace frog::native_engine {
inline unsigned optimizationReferenceKindForFunction(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    if(id=="math.optimization.constrained")return 2;
    if(id=="math.optimization.global")return 3;
    if(id.starts_with("math.optimization.unconstrained.")&&id.ends_with(".reference"))return 1;
    return 0;
}
inline std::string_view optimizationReferenceOperation(unsigned kind) noexcept {
    constexpr std::array<std::string_view,3> ids{
        "frog.studio.math.optimization.objective_reference.scalar.v1",
        "frog.studio.math.optimization.objective_reference.constrained.v1",
        "frog.studio.math.optimization.objective_reference.vector.v1"};
    return kind>=1&&kind<=3?ids[kind-1]:std::string_view{};
}
inline unsigned optimizationReferenceKind(std::string_view id) noexcept {
    for(unsigned k=1;k<=3;++k)if(id==optimizationReferenceOperation(k))return k;return 0;
}
inline std::string_view optimizationReferenceSchema(unsigned kind) noexcept {
    constexpr std::array<std::string_view,3> types{
        "ref<connectivity.document.strict,cluster<in_parameters:array<f64>,in_data:variant,out_value:f64,out_diagnostic:cluster<status:bool,code:i32,source:string>>>",
        "ref<connectivity.document.strict,cluster<in_parameters:array<f64>,in_data:variant,out_value:f64,out_equalities:array<f64>,out_inequalities:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>",
        "ref<connectivity.document.strict,cluster<in_parameters:array<f64>,in_data:variant,out_value:array<f64>,out_equalities:array<f64>,out_inequalities:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>"};
    return kind>=1&&kind<=3?types[kind-1]:std::string_view{};
}
inline const FunctionBindingDefinition *optimizationDefinition(std::string_view id) {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    return id.starts_with("math.optimization.")?findFunctionBindingDefinition(id):nullptr;
}
inline bool optimizationSignatureValid(std::string_view id,std::span<const CanonicalNumericAggregatePort> ports,
    std::string *message=nullptr,std::string *at=nullptr) {
    const auto reject=[&](std::string why,std::string_view port={}){if(message)*message=std::move(why);if(at)*at=port;return false;};
    const auto *definition=optimizationDefinition(id);
    if(!definition||definition->ports.size()!=ports.size())return reject("Optimization requires its complete registered connector signature.");
    std::set<std::pair<bool,std::string>> seen;
    for(const auto &port:ports){auto p=std::find_if(definition->ports.begin(),definition->ports.end(),[&](const auto &expected){return expected.portId==port.id&&(expected.direction==FunctionPortDirection::Input)==port.input;});
        if(p==definition->ports.end()||!seen.emplace(port.input,port.id).second)return reject("Unknown, reversed or duplicate Optimization connector.",port.id);
        auto type=binding::parseType(p->defaultValueType);if(!type.valid()||!(type.type==port.type))return reject("Optimization requires its exact real rank, constraint record or strict objective type.",port.id);
        if(port.input&&p->requirement==FunctionPortRequirement::Required&&!port.connected)return reject("A required Optimization input has no source.",port.id);}
    return true;
}
}
