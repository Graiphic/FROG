#pragma once
#include "HyperbolicReal64.hpp"
#include <array>
#include <string_view>

namespace frog::native_engine {
// Source admission uses exact published identities, independent of palette text.
inline constexpr std::array<std::string_view, 12> hyperbolicOperationNames{
    "sinh", "cosh", "tanh", "sech", "csch", "coth", "asinh", "acosh", "atanh", "asech", "acsch", "acoth"
};
inline bool canonicalHyperbolicIdentity(std::string_view id) noexcept {
    constexpr std::string_view prefix="frog.studio.math.hyperbolic.";
    if(!id.starts_with(prefix))return false;
    id.remove_prefix(prefix.size());
    for(const auto name:hyperbolicOperationNames)if(id==name)return true;
    return false;
}
inline constexpr std::string_view hyperbolicReal64SemanticVersion="frog.hyperbolic.ieee.v1";
inline constexpr std::string_view hyperbolicReal64AbiProfile="frog.private.hyperbolic.real64-ieee.c.v1.win64";
struct HyperbolicReal64Contract {std::string_view catalogId,operationId,symbol;HyperbolicRealOperation operation;};
inline constexpr std::array<HyperbolicReal64Contract,12> hyperbolicReal64Contracts{{
    {"math.hyperbolic.sinh","frog.studio.math.hyperbolic.sinh","frog_studio_hyperbolic_sinh_f64_v1",HyperbolicRealOperation::Sinh},
    {"math.hyperbolic.cosh","frog.studio.math.hyperbolic.cosh","frog_studio_hyperbolic_cosh_f64_v1",HyperbolicRealOperation::Cosh},
    {"math.hyperbolic.tanh","frog.studio.math.hyperbolic.tanh","frog_studio_hyperbolic_tanh_f64_v1",HyperbolicRealOperation::Tanh},
    {"math.hyperbolic.sech","frog.studio.math.hyperbolic.sech","frog_studio_hyperbolic_sech_f64_v1",HyperbolicRealOperation::Sech},
    {"math.hyperbolic.csch","frog.studio.math.hyperbolic.csch","frog_studio_hyperbolic_csch_f64_v1",HyperbolicRealOperation::Csch},
    {"math.hyperbolic.coth","frog.studio.math.hyperbolic.coth","frog_studio_hyperbolic_coth_f64_v1",HyperbolicRealOperation::Coth},
    {"math.hyperbolic.asinh","frog.studio.math.hyperbolic.asinh","frog_studio_hyperbolic_asinh_f64_v1",HyperbolicRealOperation::Asinh},
    {"math.hyperbolic.acosh","frog.studio.math.hyperbolic.acosh","frog_studio_hyperbolic_acosh_f64_v1",HyperbolicRealOperation::Acosh},
    {"math.hyperbolic.atanh","frog.studio.math.hyperbolic.atanh","frog_studio_hyperbolic_atanh_f64_v1",HyperbolicRealOperation::Atanh},
    {"math.hyperbolic.asech","frog.studio.math.hyperbolic.asech","frog_studio_hyperbolic_asech_f64_v1",HyperbolicRealOperation::Asech},
    {"math.hyperbolic.acsch","frog.studio.math.hyperbolic.acsch","frog_studio_hyperbolic_acsch_f64_v1",HyperbolicRealOperation::Acsch},
    {"math.hyperbolic.acoth","frog.studio.math.hyperbolic.acoth","frog_studio_hyperbolic_acoth_f64_v1",HyperbolicRealOperation::Acoth},
}};
[[nodiscard]] inline const HyperbolicReal64Contract *canonicalHyperbolicContract(std::string_view operationId) noexcept {
    for(const auto &contract:hyperbolicReal64Contracts)if(contract.operationId==operationId)return &contract;return nullptr;
}
}
