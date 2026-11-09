// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "frog_native_engine/CanonicalNumericScalarPalette.hpp"
#include "frog/semantics/numeric_accumulate_contract.hpp"

namespace frog::native_engine {
inline constexpr auto canonicalNumericAccumulateId=runtime::numeric_aggregate::accumulate::operationId;
inline constexpr auto canonicalNumericAccumulateProfile=runtime::numeric_aggregate::accumulate::profile;
inline std::string_view canonicalNumericAccumulateMigrationIdentity(std::string_view authored)noexcept {
    if(authored==canonicalNumericAccumulateId)return authored;
    if(authored.starts_with("frog.studio."))authored.remove_prefix(12);
    return authored=="numeric.multiply_accumulate"?canonicalNumericAccumulateId:std::string_view{};
}
inline bool canonicalNumericAccumulateSignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports,std::string *message=nullptr,std::string *at=nullptr)noexcept {
    try{
        const auto actual=numeric_scalar_palette_detail::ports(ports);
        return runtime::numeric_aggregate::accumulate::validateSignature(id,
            std::span<const runtime::numeric_aggregate::SourcePort>(actual),message,at);
    }catch(...){if(message)*message="Multiply Accumulate requires complete plain numeric scalars and a Boolean initialize input.";return false;}
}
}
