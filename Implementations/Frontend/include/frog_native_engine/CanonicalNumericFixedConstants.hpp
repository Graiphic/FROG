#pragma once
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog/semantics/numeric_fixed_constants.hpp"

namespace frog::native_engine {
inline std::string_view canonicalNumericIeeeConstantMigrationIdentity(std::string_view id) noexcept {
    namespace constants=runtime::numeric_aggregate::fixed_constants;
    if(const auto *entry=constants::contract(id))return entry->operationId;
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    if(const auto *entry=constants::contract(id))return entry->operationId;
    return {};
}
inline bool canonicalNumericIeeeConstantSignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports,std::string *message=nullptr,std::string *at=nullptr) noexcept {
    const auto refuse=[&](std::string_view why){if(message)*message=why;if(at)*at="value";return false;};
    try {
        if(!runtime::numeric_aggregate::fixed_constants::supports(id)||ports.size()!=1)
            return refuse("An IEEE fixed constant has exactly one value output and no inputs.");
        const auto &output=ports.front();
        if(output.input||output.id!="value"||!binding::fullTypeEqual(output.type,binding::TypeRef::primitive("f64")))
            return refuse("An IEEE fixed constant must preserve its actual plain F64 value producer.");
        return true;
    }catch(...){return refuse("IEEE fixed-constant Source validation failed without admitting a partial descriptor.");}
}
}
