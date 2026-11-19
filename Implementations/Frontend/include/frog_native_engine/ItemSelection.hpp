// SPDX-License-Identifier: Apache-2.0
#pragma once

#include <algorithm>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace frog::native_engine::item_selection {

enum class Mode { None, Single, Multiple, Range };
struct Item {
    std::string_view id;
    bool enabled=true,visible=true;
};
struct State {
    Mode mode=Mode::Single;
    std::vector<std::string> selected;
    std::string active,anchor;
    bool operator==(const State &) const=default;
};
struct Modifiers {bool control=false,shift=false;};

inline std::string_view modeName(Mode mode) noexcept {
    switch(mode){case Mode::None:return "none";case Mode::Single:return "single";
        case Mode::Multiple:return "multiple";case Mode::Range:return "range";}
    return {};
}
inline std::optional<Mode> parseMode(std::string_view text) noexcept {
    if(text=="none")return Mode::None;if(text=="single")return Mode::Single;
    if(text=="multiple")return Mode::Multiple;if(text=="range")return Mode::Range;
    return {};
}
inline bool selectable(const Item &item) noexcept{return item.enabled&&item.visible;}
inline std::optional<std::size_t> index(std::span<const Item> items,std::string_view id) {
    if(id.empty())return {};
    for(std::size_t i=0;i<items.size();++i)if(items[i].id==id)return i;
    return {};
}
inline std::string validate(const State &state,std::span<const Item> items) {
    if(items.size()>65536)return "items: the finite selection profile admits at most 65,536 items";
    std::unordered_map<std::string_view,std::size_t> ids;ids.reserve(items.size());
    for(std::size_t i=0;i<items.size();++i)
        if(items[i].id.empty()||!ids.emplace(items[i].id,i).second)return "items.id: item identities must be nonempty and unique";
    if(state.mode==Mode::None&&!state.selected.empty())return "selection.selected_ids: selection mode none requires an empty selection";
    if(state.mode==Mode::Single&&state.selected.size()>1)return "selection.selected_ids: single selection contains multiple items";
    std::unordered_set<std::string_view> selected;
    for(const auto &id:state.selected) {
        const auto found=ids.find(id);
        if(found==ids.end()||!selectable(items[found->second]))return "selection.selected_ids: selected item is absent, hidden or disabled";
        if(!selected.insert(id).second)return "selection.selected_ids: duplicate selected identity";
    }
    for(const auto *id:{&state.active,&state.anchor})if(!id->empty()) {
        const auto found=ids.find(*id);
        if(found==ids.end()||!selectable(items[found->second]))return "selection.active/anchor: item is absent, hidden or disabled";
    }
    if(state.mode==Mode::Range&&!selected.empty()) {
        bool inside=false,ended=false;
        for(const auto &item:items)if(selectable(item)) {
            if(selected.contains(item.id)){if(ended)return "selection.selected_ids: range selection must be contiguous";inside=true;}
            else if(inside)ended=true;
        }
    }
    return {};
}

// An authored item order defines range order; neither labels nor host rows
// identify values. Invisible/disabled entries are never selected implicitly.
inline bool select(State &state,std::span<const Item> items,std::size_t target,
    Modifiers modifiers={},bool focusOnly=false,bool toggle=false) {
    if(target>=items.size()||!selectable(items[target]))return false;
    const auto before=state;const auto targetId=std::string(items[target].id);
    state.active=targetId;
    if(state.mode==Mode::None)return state!=before;
    if(focusOnly){if(state.anchor.empty())state.anchor=targetId;return state!=before;}
    if(state.mode==Mode::Single){state.selected={targetId};state.anchor=targetId;return state!=before;}
    std::unordered_set<std::string_view> selected;
    if(state.mode==Mode::Multiple&&modifiers.control)
        for(const auto &id:before.selected)selected.insert(id);
    if(modifiers.shift) {
        const auto anchor=index(items,state.anchor).value_or(index(items,before.active).value_or(target));
        if(state.anchor.empty())state.anchor=std::string(items[anchor].id);
        for(auto i=(std::min)(anchor,target);i<=(std::max)(anchor,target);++i)
            if(selectable(items[i]))selected.insert(items[i].id);
    }else {
        if(state.mode==Mode::Multiple&&(modifiers.control||toggle)) {
            if(toggle&&!modifiers.control)for(const auto &id:before.selected)selected.insert(id);
            if(selected.contains(targetId))selected.erase(targetId);else selected.insert(items[target].id);
        }else selected.insert(items[target].id);
        state.anchor=targetId;
    }
    state.selected.clear();state.selected.reserve(selected.size());
    for(const auto &item:items)if(selected.contains(item.id))state.selected.emplace_back(item.id);
    return state!=before;
}
inline bool selectAll(State &state,std::span<const Item> items) {
    if(state.mode!=Mode::Multiple&&state.mode!=Mode::Range)return false;
    const auto before=state;state.selected.clear();
    for(const auto &item:items)if(selectable(item))state.selected.emplace_back(item.id);
    if(!state.selected.empty())state.anchor=state.selected.front(),state.active=state.selected.back();
    return state!=before;
}
enum class Navigation { Previous,Next,First,Last };
inline std::optional<std::size_t> navigate(const State &state,std::span<const Item> items,Navigation direction) {
    if(items.empty())return {};
    const auto current=index(items,state.active);
    const bool reverse=direction==Navigation::Previous||direction==Navigation::Last;
    const auto size=static_cast<std::ptrdiff_t>(items.size());
    auto next=direction==Navigation::First?0:direction==Navigation::Last?size-1:
        current?static_cast<std::ptrdiff_t>(*current)+(reverse?-1:1):(reverse?size-1:0);
    for(;next>=0&&next<size;next+=reverse?-1:1)if(selectable(items[std::size_t(next)]))return std::size_t(next);
    return {};
}

} // namespace frog::native_engine::item_selection
