#pragma once

#include "WidgetInstance.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace frog::native_engine {

// A field is addressed by identity, never by its current ordinal. An empty
// fieldId denotes the shared Array element (indices identify its displayed value).
struct ContainedWidgetStep {
    std::string fieldId;
    std::vector<std::uint32_t> indices;
    bool operator==(const ContainedWidgetStep &) const = default;
};
using ContainedWidgetPath = std::vector<ContainedWidgetStep>;

inline WidgetInstance *containedWidget(WidgetInstance &root, const ContainedWidgetPath &path) {
    auto *widget = &root;
    for (const auto &step : path) {
        if (step.fieldId.empty()) {
            auto *array = arrayContainerState(*widget);
            widget = array && array->elementTemplate ? array->elementTemplate.get() : nullptr;
        } else {
            auto *cluster = clusterContainerState(*widget);
            if (!cluster) return nullptr;
            const auto field = std::find_if(cluster->elements.begin(), cluster->elements.end(),
                [&](const auto &candidate) { return candidate.fieldId == step.fieldId; });
            widget = field != cluster->elements.end() ? field->widget.get() : nullptr;
        }
        if (!widget) return nullptr;
    }
    return widget;
}

inline const WidgetInstance *containedWidget(const WidgetInstance &root, const ContainedWidgetPath &path) {
    const auto *widget = &root;
    for (const auto &step : path) {
        if (step.fieldId.empty()) {
            const auto *array = arrayContainerState(*widget);
            widget = array && array->elementTemplate ? array->elementTemplate.get() : nullptr;
        } else {
            const auto *cluster = clusterContainerState(*widget);
            if (!cluster) return nullptr;
            const auto field = std::find_if(cluster->elements.begin(), cluster->elements.end(),
                [&](const auto &candidate) { return candidate.fieldId == step.fieldId; });
            widget = field != cluster->elements.end() ? field->widget.get() : nullptr;
        }
        if (!widget) return nullptr;
    }
    return widget;
}

} // namespace frog::native_engine
