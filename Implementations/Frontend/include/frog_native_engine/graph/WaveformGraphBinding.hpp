#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include <functional>

namespace frog::native_engine::graph {
// Inference is applied only at read/authoring synchronization boundaries.
// Explicit schemas and nonempty authored defaults retain their exact type.
[[nodiscard]] bool synchronizeTypes(std::vector<DiagramNode> &,
    std::span<const DiagramEdge>, FrogWidgetDataView,
    const std::function<WidgetInstance *(std::string_view)> &mutableWidget);

template <typename Widgets>
[[nodiscard]] bool synchronizeTypes(std::vector<DiagramNode> &nodes,
    std::span<const DiagramEdge> edges, Widgets &widgets) {
    return synchronizeTypes(nodes, edges, static_cast<FrogWidgetDataView>(widgets),
        [&widgets](std::string_view id) { return widgets.widgetById(id); });
}
}
