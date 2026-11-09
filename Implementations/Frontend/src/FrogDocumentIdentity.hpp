#pragma once

#include "frog_native_engine/WidgetCatalog.hpp"

#include <string_view>

namespace frog::native_engine::document_internal {

// Resolves persisted widget class identities, including the explicit aliases
// retained for documents authored before indicator profiles became distinct.
const WidgetCatalogEntry *documentWidgetCatalogEntry(std::string_view classId) noexcept;

int widgetSerialFromId(std::string_view id);
int diagramNodeSerialFromId(std::string_view id);
int diagramEdgeSerialFromId(std::string_view id);
int diagramWireFragmentSerialFromId(std::string_view id);

} // namespace frog::native_engine::document_internal
