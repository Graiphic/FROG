#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/DocumentDiagnostic.hpp"
#include <unordered_set>
#include <vector>
namespace frog::native_engine {
namespace binding { class TypeResolver; }
struct DraftConnectionInspection {
    std::vector<DocumentDiagnostic> diagnostics;
    std::unordered_set<std::string> connectedPorts;
    bool structurallyValid=true;
};
// The original read-only Diagram wire validation, shared by the Frontend and
// Studio readiness. Call on the active semantic projection. A borrowed resolver
// avoids rebuilding its graph index and must address the same schema-specialized
// Cluster view as the caller; neither it nor document pointers escape.
[[nodiscard]] DraftConnectionInspection inspectDraftTypedConnections(
    FrogDraftDataView,binding::TypeResolver *existingResolver=nullptr);
}
