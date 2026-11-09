#pragma once

#include "frog_native_engine/WidgetInstance.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

inline constexpr std::string_view kTabControlWidgetClass = "frog.studio.widgets.tab_control";
inline constexpr std::string_view kTabIndicatorWidgetClass = "frog.studio.widgets.tab_indicator";

[[nodiscard]] bool isTabWidget(const WidgetInstance &widget) noexcept;
void initializeTabControl(WidgetInstance &widget);
[[nodiscard]] bool tabPagesValid(const WidgetInstance &widget) noexcept;
[[nodiscard]] std::string tabEditorPageId(const WidgetInstance &widget);
[[nodiscard]] std::string tabSelectedPageId(const WidgetInstance &widget);
[[nodiscard]] std::optional<std::size_t> tabPageIndex(const WidgetInstance &widget, std::string_view pageId) noexcept;
[[nodiscard]] std::string_view tabPageCaption(const RingItem &page) noexcept;
[[nodiscard]] bool selectTabEditorPage(WidgetInstance &widget, std::string_view pageId);
// Invalid input leaves the previous valid selection unchanged.
[[nodiscard]] bool setTabSelectedPage(WidgetInstance &widget, std::uint32_t index);
[[nodiscard]] bool setTabSelectedPage(WidgetInstance &widget, std::string_view pageId);
[[nodiscard]] std::optional<std::string> addTabPage(WidgetInstance &widget, std::string_view targetPageId, bool before);
[[nodiscard]] bool renameTabPage(WidgetInstance &widget, std::string_view pageId, std::string name);
// A matching caption uses the label rename owner; an independent caption is presentation only.
[[nodiscard]] bool renameTabPageCaption(WidgetInstance &widget, std::string_view pageId, std::string caption);
[[nodiscard]] bool setTabPageCaptionMatchesLabel(WidgetInstance &widget, std::string_view pageId, bool matches);
[[nodiscard]] bool rearrangeTabPages(WidgetInstance &widget, const std::vector<std::string> &pageIds);
// Model-only removal. Canvas::removeTabPage also removes the page descendants.
[[nodiscard]] bool removeTabPage(WidgetInstance &widget, std::string_view pageId);
[[nodiscard]] double tabHeaderHeight(const WidgetInstance &widget) noexcept;
[[nodiscard]] Rect tabContentBounds(const WidgetInstance &widget) noexcept;

} // namespace frog::native_engine
