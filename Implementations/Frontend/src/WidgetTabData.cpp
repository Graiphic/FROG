// SPDX-License-Identifier: Apache-2.0
// Extracted from the existing Studio data owner; no editor or runtime.
#include "frog_native_engine/TabControl.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
namespace frog::native_engine {

bool isTabWidget(const WidgetInstance &widget) noexcept {
    return widget.classId == kTabControlWidgetClass || widget.classId == kTabIndicatorWidgetClass;
}

void initializeTabControl(WidgetInstance &widget) {
    if (!isTabWidget(widget)) return;
    widget.tabContainer = TabContainerState{};
    widget.ringItems.clear();
    for (std::size_t i = 0; i < 2U; ++i) {
        auto id = widget.instanceId + ".page-" + std::to_string(widget.tabContainer->nextPageSerial++);
        widget.ringItems.push_back({"Page " + std::to_string(i + 1U), static_cast<std::uint32_t>(i), false, std::move(id), true});
    }
    widget.tabContainer->editorPageId = widget.ringItems.front().id;
    widget.numericRepresentationKind = "uint32";
    widget.valueType = "u32";
    widget.enumDomainId = "frog.studio." + widget.instanceId;
    widget.numericValue = 0;
    widget.initialValue = widget.ringItems.front().id;
    widget.ringAllowUndefinedValues = false;
}

bool tabPagesValid(const WidgetInstance &widget) noexcept {
    if (!isTabWidget(widget) || !widget.tabContainer || widget.ringItems.empty() ||
        widget.ringItems.size() > (std::numeric_limits<std::uint32_t>::max)()) return false;
    for (std::size_t i = 0; i < widget.ringItems.size(); ++i) {
        const auto &page = widget.ringItems[i];
        if (page.id.empty() || page.label.empty() || page.value != i) return false;
        for (std::size_t j = 0; j < i; ++j) if (widget.ringItems[j].id == page.id) return false;
    }
    return true;
}

std::optional<std::size_t> tabPageIndex(const WidgetInstance &widget, std::string_view pageId) noexcept {
    for (std::size_t i = 0; i < widget.ringItems.size(); ++i)
        if (widget.ringItems[i].id == pageId) return i;
    return {};
}

std::string_view tabPageCaption(const RingItem &page) noexcept {
    return page.captionMatchesLabel ? std::string_view(page.label) : std::string_view(page.caption);
}

std::string tabSelectedPageId(const WidgetInstance &widget) {
    if (!isTabWidget(widget) || widget.ringItems.empty()) return {};
    const auto selected = widget.numericValue.value_or(0);
    if (std::isfinite(selected) && selected >= 0 && selected < widget.ringItems.size() && std::floor(selected) == selected)
        return widget.ringItems[static_cast<std::size_t>(selected)].id;
    return widget.ringItems.front().id;
}

std::string tabEditorPageId(const WidgetInstance &widget) {
    if (!isTabWidget(widget) || widget.ringItems.empty()) return {};
    if (widget.tabContainer && tabPageIndex(widget, widget.tabContainer->editorPageId))
        return widget.tabContainer->editorPageId;
    return widget.ringItems.front().id;
}

bool selectTabEditorPage(WidgetInstance &widget, std::string_view pageId) {
    if (!isTabWidget(widget) || !tabPageIndex(widget, pageId)) return false;
    if (!widget.tabContainer) widget.tabContainer = TabContainerState{};
    if (widget.tabContainer->editorPageId == pageId) return false;
    widget.tabContainer->editorPageId = pageId;
    return true;
}

bool setTabSelectedPage(WidgetInstance &widget, std::uint32_t index) {
    if (!isTabWidget(widget) || index >= widget.ringItems.size()) return false;
    const auto &page = widget.ringItems[index];
    if (page.disabled || !page.visible || (widget.numericValue == double(index) && widget.initialValue == page.id)) return false;
    widget.numericValue = index;
    widget.initialValue = page.id;
    return true;
}

bool setTabSelectedPage(WidgetInstance &widget, std::string_view pageId) {
    const auto index = tabPageIndex(widget, pageId);
    return index && setTabSelectedPage(widget, static_cast<std::uint32_t>(*index));
}


}
