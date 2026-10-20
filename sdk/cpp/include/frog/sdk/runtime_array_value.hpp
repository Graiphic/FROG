#pragma once
// Private versioned packed Array extension; no mutable caller alias is retained.
#include "frog/sdk/runtime_contract.hpp"
#include <cmath>
#include <limits>
#include <new>
namespace frog::sdk::runtime {
[[nodiscard]] inline F64ArrayCreateResult makeRuntimeF64Array(std::span<const double> values,
    const RuntimeValueOwner &owner, std::size_t availablePayloadBytes) {
    static_assert(sizeof(double)==8 && std::numeric_limits<double>::is_iec559);
    if(!runtimeValueOwnerValid(owner))return {{},F64ArrayFailure::OwnerInvalid};
    if(values.size()>kMaximumF64ArrayElements)return {{},F64ArrayFailure::ElementLimit};
    if(availablePayloadBytes>kMaximumF64ArrayPayloadBytes ||
        values.size()>availablePayloadBytes/sizeof(double))return {{},F64ArrayFailure::PayloadLimit};
    for(double value:values)if(!std::isfinite(value))return {{},F64ArrayFailure::NonFinite};
    try {
        std::vector<double> copy;
        if(!values.empty())copy.assign(values.begin(),values.end());
        return {RuntimeF64Array{new ImmutableF64Array(owner,std::move(copy))},F64ArrayFailure::None};
    }catch(const std::bad_alloc &) {return {{},F64ArrayFailure::ResourceFailure};}
}
} // namespace frog::sdk::runtime
