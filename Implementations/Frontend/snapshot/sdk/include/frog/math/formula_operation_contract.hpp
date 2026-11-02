#pragma once
// SPDX-License-Identifier: Apache-2.0
#include <string_view>
#include <optional>
#include <iterator>
namespace frog::math::formula {
inline constexpr std::string_view profile="frog.studio.formula.evaluation@1";
enum class Operation {
    EvalString, SingleScalar, SingleArray, MultiScalar, MultiArray, ParsedString,
    Node, ParsedNode, YOptimal, YParameters, Surface, SurfaceParameters,
    XYZ, XYZParameters, XY, XYOptimal, XYParameters, Polar, PolarOptimal
};

namespace operation_contract_detail {
constexpr std::string_view names[]={"eval_string","single_scalar","single_array","multi_scalar","multi_array","parsed_string","node","parsed_node","y_optimal","y_parameters","surface","surface_parameters","xyz","xyz_parameters","xy","xy_optimal","xy_parameters","polar","polar_optimal"};
constexpr std::string_view ids[]={"frog.studio.math.formula.eval_string","frog.studio.math.formula.single_scalar","frog.studio.math.formula.single_array","frog.studio.math.formula.multi_scalar","frog.studio.math.formula.multi_array","frog.studio.math.formula.parsed_string","frog.studio.math.formula.node","frog.studio.math.formula.parsed_node","frog.studio.math.formula.y_optimal","frog.studio.math.formula.y_parameters","frog.studio.math.formula.surface","frog.studio.math.formula.surface_parameters","frog.studio.math.formula.xyz","frog.studio.math.formula.xyz_parameters","frog.studio.math.formula.xy","frog.studio.math.formula.xy_optimal","frog.studio.math.formula.xy_parameters","frog.studio.math.formula.polar","frog.studio.math.formula.polar_optimal"};
}
inline std::optional<Operation> parseOperationId(std::string_view id)noexcept{for(std::size_t i=0;i<std::size(operation_contract_detail::ids);++i)if(id==operation_contract_detail::ids[i])return static_cast<Operation>(i);return{};}
inline std::string_view operationId(Operation op)noexcept{const auto i=static_cast<unsigned>(op);return i<std::size(operation_contract_detail::ids)?operation_contract_detail::ids[i]:std::string_view{};}
inline std::optional<Operation> operation(std::string_view id)noexcept{if(const auto o=parseOperationId(id))return o;if(id.starts_with("math.formula."))id.remove_prefix(13);for(std::size_t i=0;i<std::size(operation_contract_detail::names);++i)if(id==operation_contract_detail::names[i])return static_cast<Operation>(i);return{};}
inline std::string_view operationName(Operation op)noexcept{const auto i=static_cast<unsigned>(op);return i<std::size(operation_contract_detail::names)?operation_contract_detail::names[i]:std::string_view{};}
}
