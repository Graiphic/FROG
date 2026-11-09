// SPDX-License-Identifier: Apache-2.0
// Extracted from the existing Studio data owner; no editor or runtime.
#include "frog_native_engine/WidgetCatalog.hpp"
#include "WidgetCatalog.generated.hpp"
#include <algorithm>
namespace frog::native_engine {

const std::vector<WidgetCatalogEntry> &widgetCatalogEntries() noexcept {
    return generated::widgets::catalogEntries();
}

const WidgetCatalogEntry *findWidgetCatalogEntryByClassId(std::string_view classId) noexcept {
    if(classId=="frog.studio.widgets.collection"){static const WidgetCatalogEntry collection{WidgetFamily::DataContainers,"Collection",std::string(classId),"control","unknown",{},{},{},{},{},{},{220,96}};return &collection;}
    if(classId==kReferenceWidgetClass){static const WidgetCatalogEntry reference{WidgetFamily::DataContainers,"Reference",std::string(kReferenceWidgetClass),"control","ref<frog.object>",{},{},{},{},{},{},{110,26}};return &reference;}
    const auto &entries = widgetCatalogEntries();
    const auto found = std::find_if(entries.begin(), entries.end(), [classId](const WidgetCatalogEntry &entry) {
        return entry.classId == classId;
    });

    return found == entries.end() ? nullptr : &(*found);
}


}
