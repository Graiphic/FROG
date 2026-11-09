#include "frog_native_engine/ContainedWidgetSchemaMetadata.hpp"

#include <unordered_map>
#include <utility>
#include <vector>

namespace frog::native_engine {
namespace {
template <class Widget>
bool collectPostOrder(Widget &root, std::vector<Widget *> &order) {
    struct Frame { Widget *widget; bool expanded; };
    enum class Visit { Active, Complete };
    std::unordered_map<Widget *, Visit> visits;
    std::vector<Frame> pending{{&root, false}};
    while (!pending.empty()) {
        const auto frame = pending.back();
        pending.pop_back();
        if (frame.expanded) {
            visits[frame.widget] = Visit::Complete;
            order.push_back(frame.widget);
            continue;
        }
        const auto found = visits.find(frame.widget);
        if (found != visits.end()) {
            if (found->second == Visit::Active) return false;
            continue;
        }
        visits.emplace(frame.widget, Visit::Active);
        pending.push_back({frame.widget, true});
        // Both overloads walk existing state only: a mutable accessor would
        // synthesize an absent container state while checking a legacy schema.
        const WidgetInstance &source = *frame.widget;
        if (const auto *cluster = clusterContainerState(source)) {
            for (auto field = cluster->elements.rbegin(); field != cluster->elements.rend(); ++field)
                if (field->widget) pending.push_back({field->widget.get(), false});
        } else if (const auto *array = arrayContainerState(source); array && array->elementTemplate) {
            pending.push_back({array->elementTemplate.get(), false});
        }
    }
    return true;
}
}

bool containedWidgetSchemaHasCycle(const WidgetInstance &widget) {
    std::vector<const WidgetInstance *> order;
    return !collectPostOrder(widget, order);
}

bool refreshContainedWidgetSchemaMetadata(WidgetInstance &widget) {
    std::vector<WidgetInstance *> order;
    if (!collectPostOrder(widget, order)) return false;
    // The nonmutating walk completes before writes. These pointers refer to the
    // mutable root's actual shared prototype/field objects, not detached cells.
    for (auto *entry : order) {
        auto &current = *entry;
        if (const auto *cluster = clusterContainerState(std::as_const(current))) {
            std::string spelling = "cluster:";
            for (std::size_t index = 0; index < cluster->elements.size(); ++index) {
                if (index) spelling += '|';
                const auto &field = cluster->elements[index];
                spelling += field.widget ? field.widget->valueType : "unknown";
            }
            current.valueType = std::move(spelling);
        } else if (isArrayWidget(current) && current.arrayContainer) {
            auto *array = &*current.arrayContainer;
            if (array->elementTemplate) {
                const auto &element = *array->elementTemplate;
                array->elementClassId = element.classId;
                array->elementValueType = element.valueType;
                array->elementRole = element.role;
            } else {
                array->elementClassId.clear();
                array->elementValueType.clear();
            }
            current.valueType = "frog.array<" +
                (array->elementTemplate ? array->elementTemplate->valueType : std::string{"unknown"}) +
                ",rank=" + std::to_string(array->rank) + ">";
        }
    }
    return true;
}
}
