#pragma once
#include "frog_native_engine/FittingBinding.hpp"
#include "frog_native_engine/OptimizationBinding.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/FrogDraftData.hpp"

namespace frog::native_engine {
inline constexpr std::string_view fittingModelReferenceOperation="frog.studio.math.fitting.model_reference.owned.v1";
inline constexpr std::string_view fittingModelReferenceSchema="ref<connectivity.document.strict,cluster<in_x:f64,in_parameters:array<f64>,in_data:variant,out_value:f64,out_gradient:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>";
inline bool nativeNumericalModelReference(std::string_view id) noexcept {
 return id==fittingModelReferenceOperation||optimizationReferenceKind(id)!=0;
}
inline std::string_view nativeNumericalModelSchema(std::string_view id) noexcept {
 return id==fittingModelReferenceOperation?fittingModelReferenceSchema:optimizationReferenceSchema(optimizationReferenceKind(id));
}
inline std::string_view optimizationRecipeProfile(unsigned kind) noexcept {
 constexpr std::string_view profiles[]{"frog.studio.math.optimization.objective.scalar.source@1","frog.studio.math.optimization.objective.constrained.source@1","frog.studio.math.optimization.objective.vector.source@1"};
 return kind>=1&&kind<=3?profiles[kind-1]:std::string_view{};
}
inline unsigned optimizationRecipeKind(std::string_view profile) noexcept {
 for(unsigned k=1;k<=3;++k)if(profile==optimizationRecipeProfile(k))return k;return 0;
}
struct OptimizationCallableRole {std::string_view id,type;bool input;};
inline std::span<const OptimizationCallableRole> optimizationCallableRoles(unsigned kind) noexcept {
 static constexpr OptimizationCallableRole scalar[]{
  {"parameters","array<f64>",true},{"data","variant",true},{"value","f64",false},{"diagnostic","cluster<status:bool,code:i32,source:string>",false}};
 static constexpr OptimizationCallableRole constrained[]{
  {"parameters","array<f64>",true},{"data","variant",true},{"value","f64",false},{"equalities","array<f64>",false},{"inequalities","array<f64>",false},{"diagnostic","cluster<status:bool,code:i32,source:string>",false}};
 static constexpr OptimizationCallableRole vector[]{
  {"parameters","array<f64>",true},{"data","variant",true},{"value","array<f64>",false},{"equalities","array<f64>",false},{"inequalities","array<f64>",false},{"diagnostic","cluster<status:bool,code:i32,source:string>",false}};
 if(kind==1)return scalar;if(kind==2)return constrained;if(kind==3)return vector;return {};
}
// Projects the shared semantic laws for the authored variant. It does not
// specialize by layout, resolve dependencies, evaluate a model or repair data.
[[nodiscard]] const FunctionBindingDefinition *functionBindingDefinitionForNode(
    const DiagramNode &) noexcept;
[[nodiscard]] const FunctionBindingDefinition *fittingBindingDefinition(
    std::string_view catalogId,std::string_view variant = {}) noexcept;
}
