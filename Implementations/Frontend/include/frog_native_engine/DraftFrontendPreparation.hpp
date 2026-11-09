#pragma once
#include "frog_native_engine/DraftSourcePreparation.hpp"
#include "frog_native_engine/CanonicalDraftMigration.hpp"
#include <functional>

namespace frog::native_engine {
struct DraftFrontendPreparation {
    std::shared_ptr<const FrogSourceDocument> source;
    // Exact authored root, independent of every TypeDef/Conditional/call projection.
    std::shared_ptr<const std::string> originalDraft;
    std::string originalSourceUri;
    std::string originalSourceSha256;
    unsigned sourceRevision = 0;
    std::shared_ptr<const FrogDraftData> document;
    std::string projectedDraft;
    std::string typeDefinitionSchemaFingerprint;
    std::string subprogramDependencyFingerprint;
    std::vector<CanonicalDraftMigrationIdentity> identities;
    std::vector<CanonicalDraftMigrationControl> controls;
    std::vector<DraftSubFrogFrozenSource> dependencies;
    std::vector<DraftSubFrogExecutionIdentity> dependencyIdentities;
    std::vector<DocumentDiagnostic> diagnostics;
    [[nodiscard]] bool accepted()const noexcept {return bool(source);}
};
// Attributes existing identities only. A node:port subject is separated only
// against a unique real node ID; ambiguous IDs remain explicitly unlocated.
void attributeDraftSourceDiagnostic(DocumentDiagnostic &,FrogDraftDataView,
    const source::JsonValue &original);
// The production draft adapter, now upstream of both the C API and Studio.
// Reads and projects immutable data only. Never repairs or publishes a document.
[[nodiscard]] DraftFrontendPreparation prepareDraftFrontend(
    std::string_view bytes,std::string_view uri,
    const ConditionalDisableCompilationContext &,const DraftDependencyResolver &,
    const std::function<void()> &checkpoint = {});
}
