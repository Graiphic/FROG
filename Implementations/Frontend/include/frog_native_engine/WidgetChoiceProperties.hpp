#pragma once
#include "frog_native_engine/WidgetInstance.hpp"
#include <span>
#include <unordered_set>

namespace frog::native_engine {
// The item editor and authoring providers share the committed-list invariant.
inline bool choiceItemValuesAreUnique(std::span<const RingItem> items) {
    std::unordered_set<std::uint32_t> values;
    for(const auto &item:items)if(!values.insert(item.value).second)return false;
    return true;
}
inline bool choiceItemLabelsAreUnique(std::span<const RingItem> items) {
    std::unordered_set<std::string> labels;
    for(const auto &item:items) {
        constexpr std::string_view whitespace=" \t\r\n\v\f";
        const auto first=item.label.find_first_not_of(whitespace);
        const auto last=item.label.find_last_not_of(whitespace);
        const auto label=first==std::string::npos?std::string{}:item.label.substr(first,last-first+1);
        if(!labels.insert(label).second)return false;
    }
    return true;
}
[[nodiscard]] bool setChoiceItemStrings(WidgetInstance &widget,std::span<const std::string> strings);
} // namespace frog::native_engine
