#pragma once
#include "frog_native_engine/FittingBinding.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/FrogDraftData.hpp"

namespace frog::native_engine {
inline constexpr std::string_view fittingModelReferenceOperation="frog.studio.math.fitting.model_reference.owned.v1";
inline constexpr std::string_view fittingModelReferenceSchema="ref<connectivity.document.strict,cluster<in_x:f64,in_parameters:array<f64>,in_data:variant,out_value:f64,out_gradient:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>";
// Projects the shared semantic laws for the authored variant. It does not
// specialize by layout, resolve dependencies, evaluate a model or repair data.
[[nodiscard]] const FunctionBindingDefinition *functionBindingDefinitionForNode(
    const DiagramNode &) noexcept;
[[nodiscard]] const FunctionBindingDefinition *fittingBindingDefinition(
    std::string_view catalogId,std::string_view variant = {}) noexcept;
}
