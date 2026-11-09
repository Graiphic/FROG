#include "frog_native_engine/graph/WaveformGraphBinding.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include "frog_native_engine/WidgetDataOperations.hpp"
#include <algorithm>
#include <exception>

namespace frog::native_engine::graph {
namespace {
bool emptyInitialValue(const State &state) {
    if (std::any_of(state.initialData.series.begin(), state.initialData.series.end(),
            [](const auto &series) { return !series.y.empty(); })) return false;
    try {
        const auto data = decodeTypedValue(state.datatype, initialValueSource(state));
        return data && std::none_of(data->series.begin(), data->series.end(),
            [](const auto &series) { return !series.y.empty(); });
    } catch (const std::exception &) { return false; }
}
}

bool synchronizeTypes(std::vector<DiagramNode> &nodes, std::span<const DiagramEdge> edges,
    FrogWidgetDataView canvas,
    const std::function<WidgetInstance *(std::string_view)> &mutableWidget) {
    if (!mutableWidget) return false;
    bool changed = false;
    binding::TypeResolver types(nodes, edges, canvas);
    for (auto &node : nodes) {
        if (node.kind != "widget_value") continue;
        const auto *widget = canvas.widgetById(node.widgetId);
        if (!widget || widget->classId != classId || !widget->waveformGraph || widget->waveformGraph->preservedSource ||
            widget->role != "indicator" || widget->waveformGraph->datatypeExplicit ||
            widget->typeDefinition || !emptyInitialValue(*widget->waveformGraph)) continue;
        if (widget->defaultData) {
            const auto defaults = data_operations::decode(*widget->defaultData);
            if (!defaults || !defaults->waveformGraph || defaults->waveformGraph->preservedSource ||
                !emptyInitialValue(*defaults->waveformGraph)) continue;
        }
        const DiagramEdge *connection = nullptr;
        for (const auto &edge : edges) if (edge.to.nodeId == node.id && edge.to.portId == "value") {
            if (connection) { connection = nullptr; break; }
            connection = &edge;
        }
        if (!connection) continue;
        const auto source = types.resolve(connection->from);
        if (!source.complete || !source.resolution.accepted() ||
            !validateType(source.resolution.type).empty()) continue;
        const auto &type = source.resolution.type;
        const auto canonical = type.canonical();
        if (binding::fullTypeEqual(type, widget->waveformGraph->datatype) &&
            widget->valueType == canonical && node.type == canonical && node.valueType == canonical) continue;
        auto next = *widget;
        if (!setTypedInitialData(*next.waveformGraph, type, emptyValue(type))) continue;
        next.valueType = canonical;
        if (widget->defaultData) {
            next.defaultData.reset();
            const auto encoded = data_operations::encode(next);
            if (encoded.empty()) continue;
            next.defaultData = encoded;
        }
        auto *target = mutableWidget(node.widgetId);
        if (!target) continue;
        *target = std::move(next);
        node.type = canonical;
        node.valueType = canonical;
        changed = true;
    }
    return changed;
}
}
