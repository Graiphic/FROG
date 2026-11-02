#include "FrogDocumentIdentity.hpp"

#include <array>

namespace frog::native_engine::document_internal {
namespace {

template <std::size_t PrefixCount>
int serialFromKnownPrefixes(
    std::string_view id,
    const std::array<std::string_view, PrefixCount> &prefixes) {
    for (const auto prefix : prefixes) {
        if (id.rfind(prefix, 0) != 0) {
            continue;
        }
        int serial = 0;
        for (std::size_t index = prefix.size(); index < id.size(); ++index) {
            const char ch = id[index];
            if (ch < '0' || ch > '9') {
                return 0;
            }
            serial = (serial * 10) + (ch - '0');
        }
        return serial;
    }
    return 0;
}

} // namespace

const WidgetCatalogEntry *documentWidgetCatalogEntry(std::string_view classId) noexcept {
    if (classId == "frog.widgets.cluster") {
        return findWidgetCatalogEntryByClassId("frog.widgets.cluster_control");
    }
    if (const auto *entry = findWidgetCatalogEntryByClassId(classId); entry != nullptr) {
        return entry;
    }
    if (classId == "frog.widgets.boolean_indicator") {
        return findWidgetCatalogEntryByClassId("frog.widgets.boolean_control");
    }
    if (classId == "frog.widgets.array_indicator") {
        return findWidgetCatalogEntryByClassId("frog.widgets.array_control");
    }
    if (classId == "frog.widgets.cluster_indicator") {
        return findWidgetCatalogEntryByClassId("frog.widgets.cluster_control");
    }
    if (classId == "frog.studio.widgets.tab_indicator") {
        return findWidgetCatalogEntryByClassId("frog.studio.widgets.tab_control");
    }
    if (classId == "frog.widgets.picture_control") {
        return findWidgetCatalogEntryByClassId("frog.widgets.picture_indicator");
    }
    return nullptr;
}

int widgetSerialFromId(std::string_view id) {
    return serialFromKnownPrefixes(id, std::array<std::string_view, 1>{ "widget-" });
}

int diagramNodeSerialFromId(std::string_view id) {
    return serialFromKnownPrefixes(id, std::array<std::string_view, 2>{ "node_", "node-" });
}

int diagramEdgeSerialFromId(std::string_view id) {
    return serialFromKnownPrefixes(id, std::array<std::string_view, 2>{ "edge_", "edge-" });
}

int diagramWireFragmentSerialFromId(std::string_view id) {
    return serialFromKnownPrefixes(
        id,
        std::array<std::string_view, 4>{
            "wire_fragment_",
            "wire-fragment-",
            "fragment_",
            "fragment-" });
}

} // namespace frog::native_engine::document_internal
