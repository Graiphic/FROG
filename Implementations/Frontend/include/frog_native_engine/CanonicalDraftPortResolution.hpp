#pragma once
#include "CanonicalDraftMigration.hpp"
#include "FrogDraftData.hpp"
namespace frog::native_engine {
// Runtime-integration owner, not linked back from source-document.  Reads the
// current common document model and its actual semantic regions exactly once.
// The caller reports returned diagnostics before attempting native adaptation.
std::vector<CanonicalDraftMigrationDiagnostic> appendCanonicalDraftComposedPorts(
    FrogDraftDataView ,CanonicalDraftResolvedTypes &);
}
