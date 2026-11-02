#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
namespace frog::native_engine {
inline std::span<const DiagramDisabledRegion> disabledStructureRegions(const DiagramNode &node) {
    static const DiagramDisabledRegion legacy[]{{"enabled", true}, {"disabled", false}};
    return node.disabledStructureRegions.empty() ? std::span<const DiagramDisabledRegion>(legacy) : node.disabledStructureRegions;
}
inline std::string_view disabledStructureVisibleRegionId(const DiagramNode &node) {
    const auto regions = disabledStructureRegions(node);
    const auto requested = node.disabledStructureVisibleRegionId.empty() ?
        (node.disabledStructureEnabledVisible ? kDisabledStructureEnabledRegionId : kDisabledStructureDisabledRegionId) :
        std::string_view(node.disabledStructureVisibleRegionId);
    for (const auto &region : regions) if (region.id == requested) return region.id;
    return regions.front().id;
}
inline bool disabledStructureVisibleRegionEnabled(const DiagramNode &node) {
    const auto id = disabledStructureVisibleRegionId(node);
    for (const auto &region : disabledStructureRegions(node)) if (region.id == id) return region.enabled;
    return false;
}
}
