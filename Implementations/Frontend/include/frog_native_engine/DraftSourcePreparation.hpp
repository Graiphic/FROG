#pragma once
#include "frog_native_engine/DraftDependency.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/TypeDefinitionSemantics.hpp"
#include "frog_native_engine/ConditionalDisable.hpp"
#include "frog_native_engine/DocumentDiagnostic.hpp"

namespace frog::native_engine {
struct DraftTypeDefinitionDependency {
    DraftDependencyResult source;
    std::string definitionId;
    std::uint64_t revision = 0;
    std::string schemaFingerprint;
};
struct DraftTypeDefinitionResolution {
    std::vector<TypeDefinitionDiagnostic> diagnostics;
    std::vector<DraftTypeDefinitionDependency> dependencies;
    std::string schemaFingerprint;
    bool changed = false;
    [[nodiscard]] bool accepted() const noexcept {return diagnostics.empty();}
};
struct DraftResolvedTypeDefinitionSource {
    std::string sourceUtf8;
    DraftTypeDefinitionResolution resolution;
    bool linked = false;
    [[nodiscard]] bool accepted() const noexcept {return resolution.accepted();}
};
[[nodiscard]] bool draftHasTypeDefinitionLinks(const FrogDraftDataView &);
[[nodiscard]] DraftTypeDefinitionResolution resolveDraftTypeDefinitionLinks(
    FrogDraftData &, std::string_view ownerUri, const DraftDependencyResolver &,
    bool acceptProposed = false, std::string_view acceptedInstanceId = {});
[[nodiscard]] DraftResolvedTypeDefinitionSource prepareDraftTypeDefinitionSource(
    std::string_view sourceUtf8, std::string_view sourceUri,
    const DraftDependencyResolver &, bool acceptProposed = false,
    bool inspectSubprograms = true);
[[nodiscard]] DraftTypeDefinitionResolution inspectDraftTypeDefinitionDependencies(
    std::string_view sourceUtf8, std::string_view sourceUri, const DraftDependencyResolver &);

[[nodiscard]] std::optional<WidgetInstance> typeDefinitionConstantWidget(
    const std::vector<DiagramNode> &, std::string_view nodeId, std::string *error = nullptr);
[[nodiscard]] bool installTypeDefinitionConstantWidget(
    std::vector<DiagramNode> &, std::string_view nodeId, const WidgetInstance &, std::string *error = nullptr);

struct DraftSubFrogExecutionDiagnostic {
    std::string code, sourceUri, sourcePath, message, callsiteId;
    bool unsupported = false;
    DocumentDiagnosticDomain domain = DocumentDiagnosticDomain::Semantics;
};
struct DraftSubFrogFrozenSource {
    DraftDependencyResult source;
    std::string typeDefinitionSchemaFingerprint;
};
struct DraftSubFrogExecutionIdentity {
    std::string projectedId, sourceUri, sourcePointer, callsiteId, callPath;
    bool generated = false;
};
struct DraftSubFrogExecutionInput {
    std::shared_ptr<const FrogDraftData> rootDocument;
    std::string_view sourceUtf8;
    std::string sourceUri;
    DraftDependencyResolver resolver;
    ConditionalDisableCompilationContext compilationContext;
    bool rootSourceFieldsValidated = false;
};
struct DraftSubFrogExecutionProjection {
    FrogDraftData document;
    bool changed = false;
    std::string dependencyFingerprint;
    std::vector<DraftSubFrogFrozenSource> frozenSources;
    std::vector<DraftSubFrogExecutionIdentity> identities;
    std::vector<DraftSubFrogExecutionDiagnostic> diagnostics;
    [[nodiscard]] bool accepted() const noexcept {return diagnostics.empty();}
};
[[nodiscard]] std::optional<std::string> prepareExecutionStorageSource(
    std::string_view sourceUtf8, std::string *error = nullptr);
[[nodiscard]] DraftSubFrogExecutionProjection projectDraftSubFrogExecution(
    const DraftSubFrogExecutionInput &);
}
