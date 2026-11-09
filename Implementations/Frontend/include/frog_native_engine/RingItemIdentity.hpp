#pragma once
#include "frog_native_engine/WidgetInstance.hpp"
#include <unordered_set>

namespace frog::native_engine {
// Keep the first occurrence of every authored identity. Reserve all identities
// before allocating new ones so insertion before an existing item cannot steal it.
inline bool normalizeRingItemIdentities(std::vector<RingItem> &items) {
    std::unordered_set<std::string> reserved, seen;
    for (const auto &item : items) if (!item.id.empty()) reserved.insert(item.id);
    bool changed = false;
    std::size_t suffix = 0;
    for (auto &item : items) {
        if (!item.id.empty() && seen.insert(item.id).second) continue;
        std::string id;
        do { id = "item_" + std::to_string(suffix++); } while (!reserved.insert(id).second);
        item.id = std::move(id);
        seen.insert(item.id);
        changed = true;
    }
    return changed;
}
}
