#pragma once
#include "ExponentialReal64.hpp"
#include <array>
#include <string_view>

namespace frog::native_engine {
// Local versioned Studio profile, not new public Core semantics.
inline constexpr std::string_view exponentialReal64SemanticVersion="frog.studio.exponential.real64-ieee@1";
struct CanonicalExponentialContract {
    std::string_view catalogId,operationId;
    ExponentialReal64Operation operation;
    bool binary=false;
};
inline constexpr auto exponentialReal64Contracts=std::to_array<CanonicalExponentialContract>({
    {"math.exponential.exponential","frog.studio.math.exponential.exponential",ExponentialReal64Operation::Exp},
    {"math.exponential.exponential_minus_one","frog.studio.math.exponential.exponential_minus_one",ExponentialReal64Operation::Expm1},
    {"math.exponential.power_of_10","frog.studio.math.exponential.power_of_10",ExponentialReal64Operation::Pow10},
    {"math.exponential.power_of_2","frog.studio.math.exponential.power_of_2",ExponentialReal64Operation::Pow2},
    {"math.exponential.power_of_x","frog.studio.math.exponential.power_of_x",ExponentialReal64Operation::Power,true},
    {"math.exponential.y_th_root_of_x","frog.studio.math.exponential.y_th_root_of_x",ExponentialReal64Operation::Root,true},
    {"math.exponential.natural_logarithm","frog.studio.math.exponential.natural_logarithm",ExponentialReal64Operation::Ln},
    {"math.exponential.natural_logarithm_plus_one","frog.studio.math.exponential.natural_logarithm_plus_one",ExponentialReal64Operation::Log1p},
    {"math.exponential.logarithm_base_10","frog.studio.math.exponential.logarithm_base_10",ExponentialReal64Operation::Log10},
    {"math.exponential.logarithm_base_2","frog.studio.math.exponential.logarithm_base_2",ExponentialReal64Operation::Log2},
    {"math.exponential.logarithm_base_x","frog.studio.math.exponential.logarithm_base_x",ExponentialReal64Operation::LogBase,true}
});
[[nodiscard]] inline const CanonicalExponentialContract *canonicalExponentialContract(std::string_view id) noexcept {
    for(const auto &contract:exponentialReal64Contracts)if(contract.operationId==id)return &contract;
    return nullptr;
}
[[nodiscard]] inline bool canonicalExponentialIdentity(std::string_view id) noexcept {
    return canonicalExponentialContract(id)!=nullptr;
}
}
