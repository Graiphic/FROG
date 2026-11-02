#include "frog_native_engine/FunctionCatalogIdentity.hpp"
#include <algorithm>
#include <array>

namespace frog::native_engine {
namespace {
const auto &catalogIdentities() noexcept {
    static const auto ids=[] {
        auto entries=std::to_array<std::string_view>({
#define FROG_FUNCTION_ENTRY(family, catalogId, ...) catalogId,
#include "FunctionCatalogEntries.inc"
#include "../manifests/connectivity/entries.generated.inc"
#include "../manifests/math/entries.inc"
#include "../manifests/file/entries.inc"
#undef FROG_FUNCTION_ENTRY
        });
        std::sort(entries.begin(),entries.end());
        return entries;
    }();
    return ids;
}
struct Alias {std::string_view id, canonical;};
constexpr auto aliases=std::to_array<Alias>({
#define FROG_FUNCTION_ALIAS(alias, canonical) Alias{alias,canonical},
#include "FunctionCatalogLegacyAliases.inc"
#undef FROG_FUNCTION_ALIAS
});
} // namespace
bool isKnownFunctionCatalogIdentity(std::string_view id) noexcept {
    // The fixed array is initialized and sorted once without allocating.
    // Queries do not scan the catalog or require visual metadata.
    const auto &ids=catalogIdentities();
    if(std::binary_search(ids.begin(),ids.end(),id))return true;
    for(const auto &alias:aliases)if(alias.id==id)
        return std::binary_search(ids.begin(),ids.end(),alias.canonical);
    return false;
}
} // namespace frog::native_engine
