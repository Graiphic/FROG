#include "frog_native_engine/CanonicalScalarSelection.hpp"
#include <array>
namespace frog::native_engine {
namespace {
constexpr std::array<std::string_view,3> selectInputs{"true","select","false"};
constexpr std::array<std::string_view,1> selectOutputs{"result"};
constexpr std::array<std::string_view,2> minmaxInputs{"x","y"},minmaxOutputs{"max","min"};
const std::array<CanonicalScalarSelectionContract,3> contracts{{
    {"structures.select","frog.studio.scalar.select.exact.v1",CanonicalScalarSelectionKind::Select,selectInputs,selectOutputs},
    {"numeric.max_min","frog.studio.scalar.maxmin.exact.v1",CanonicalScalarSelectionKind::MaxMin,minmaxInputs,minmaxOutputs},
    {"numeric.max_min","frog.studio.scalar.maxmin.nan-aware.v2",CanonicalScalarSelectionKind::MaxMin,minmaxInputs,minmaxOutputs,canonicalScalarMaxMinNanProfile,true}}};
}
std::span<const CanonicalScalarSelectionContract> canonicalScalarSelectionContracts() noexcept {return contracts;}
const CanonicalScalarSelectionContract *canonicalScalarSelectionContract(std::string_view id) noexcept {
    for(const auto &c:contracts)if(c.operationId==id)return &c;return nullptr;
}
const CanonicalScalarSelectionContract *canonicalScalarSelectionAuthoredContract(std::string_view id) noexcept {
    if(const auto *c=canonicalScalarSelectionContract(id))return c;
    constexpr std::string_view prefix="frog.studio.";if(!id.starts_with(prefix))return nullptr;id.remove_prefix(prefix.size());
    if(id=="numeric.max_min")return canonicalScalarSelectionContract("frog.studio.scalar.maxmin.nan-aware.v2");
    for(const auto &c:contracts)if(c.catalogId==id)return &c;return nullptr;
}
bool canonicalScalarSelectionTypeAccepted(const CanonicalScalarSelectionContract &c,std::string_view type) noexcept {
    return type=="i32"||type=="u32"||type=="i64"||type=="u64"||type=="f64"||(c.kind==CanonicalScalarSelectionKind::Select&&type=="bool");
}
}
