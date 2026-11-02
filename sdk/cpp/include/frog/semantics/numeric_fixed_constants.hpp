#pragma once
#include <array>
#include <string_view>

// Private metadata, readable by Source without linking the Runtime. Only the
// two IEEE catalogue constants need a compiled zero-input realization; the
// eighteen finite presets keep FunctionCatalog's existing literal evaluator.
namespace frog::runtime::numeric_aggregate::fixed_constants {
struct Contract {std::string_view catalogId,operationId;bool negative;};
inline constexpr std::string_view profile="frog.studio.numeric.fixed-ieee-constant@1";
inline constexpr std::array<Contract,2> contracts{{
    {"numeric.other_constants.positive_infinity","frog.studio.numeric.constant.positive-infinity.ieee64.v1",false},
    {"numeric.other_constants.negative_infinity","frog.studio.numeric.constant.negative-infinity.ieee64.v1",true}
}};
inline constexpr const Contract *contract(std::string_view id) noexcept {
    for(const auto &entry:contracts)if(entry.operationId==id||entry.catalogId==id)return &entry;return nullptr;
}
inline constexpr bool supports(std::string_view id) noexcept {
    const auto *entry=contract(id);return entry&&entry->operationId==id;
}
inline constexpr bool ieeeOutput(std::string_view operation,std::string_view port,std::string_view type) noexcept {
    return supports(operation)&&port=="value"&&type=="f64";
}
}
