#pragma once

#include <cstddef>
#include <string_view>
#include <unordered_map>

namespace frog::native_engine::math_variant {

// The catalog and variant descriptors own stable string views for the process.
inline std::string_view canonical(std::string_view id) noexcept {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    if (id.ends_with(".owned.v1")) id.remove_suffix(9);
    return id;
}

struct Match {
    std::size_t function;
    std::size_t variant;
    bool explicitVariant;
};

class Lookup {
public:
    template<class Descriptions, class Laws>
    Lookup(const Descriptions &descriptions, const Laws &laws)
        : functionCount_(descriptions.size()) {
        for (std::size_t i = 0; i < descriptions.size(); ++i) {
            insert(descriptions[i].catalogId, {i, 0, false});
            const auto &variants = laws[i].variants;
            for (std::size_t j = 0; j < variants.size(); ++j)
                insert(canonical(variants[j].descriptor.operationId), {i, j, true});
        }
    }

    Match find(std::string_view id) const noexcept {
        id = canonical(id);
        if (id.starts_with(prefix_)) {
            const auto it = ids_.find(id);
            if (it != ids_.end()) return it->second;
        }
        return {functionCount_, 0, false};
    }

private:
    void insert(std::string_view id, Match match) {
        // Derive the common prefix from the registered keys. An alias outside
        // the usual namespace automatically shortens or clears this filter.
        if (ids_.empty()) prefix_ = id;
        else {
            std::size_t n = 0;
            while (n < prefix_.size() && n < id.size() && prefix_[n] == id[n]) ++n;
            prefix_ = prefix_.substr(0, n);
        }
        // The original ordered lookup returned the first matching identity.
        ids_.try_emplace(id, match);
    }

    std::size_t functionCount_;
    std::string_view prefix_;
    std::unordered_map<std::string_view, Match> ids_;
};

} // namespace frog::native_engine::math_variant
