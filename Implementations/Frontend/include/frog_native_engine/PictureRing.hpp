#pragma once

#include "frog_native_engine/RingItemIdentity.hpp"

#include <algorithm>
#include <utility>

namespace frog::native_engine {

inline constexpr std::size_t kPictureRingMaximumItems = 128;

struct PictureRingSelection {
    std::uint32_t current;
    std::uint32_t initial;
};

// A Picture Ring transports the same unsigned integer as a default Ring.
// Image identities are used only to retain its selections across edits.
inline PictureRingSelection replacePictureRingItems(std::vector<RingItem> &catalog,
    std::vector<RingItem> edited, std::uint32_t current, std::uint32_t initial) {
    normalizeRingItemIdentities(edited);
    for (std::size_t index = 0; index < edited.size(); ++index) {
        edited[index].value = static_cast<std::uint32_t>(index);
    }

    const auto retain = [&](std::uint32_t index) -> std::uint32_t {
        if (edited.empty() || catalog.empty()) return 0;
        if (index >= catalog.size()) return index;
        const auto &id = catalog[index].id;
        const auto found = std::find_if(edited.begin(), edited.end(), [&](const RingItem &item) {
            return item.id == id;
        });
        return found == edited.end() ? 0U : found->value;
    };
    const PictureRingSelection selected{retain(current), retain(initial)};
    catalog = std::move(edited);
    return selected;
}

} // namespace frog::native_engine
