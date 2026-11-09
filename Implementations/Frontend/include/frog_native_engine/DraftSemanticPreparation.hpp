#pragma once
#include "frog_native_engine/CanonicalDraftPortResolution.hpp"
#include "frog_native_engine/ConditionalDisable.hpp"
#include "frog_native_engine/DocumentDiagnostic.hpp"
namespace frog::native_engine {
[[nodiscard]] CanonicalDraftResolvedTypes resolveCanonicalDraftTypes(FrogDraftDataView,std::string sourceSha256,bool &coercion);
[[nodiscard]] CanonicalDraftMigrationResult prepareDraftSemanticSource(const FrogDraftData &,std::string_view,std::string_view);
[[nodiscard]] std::vector<DocumentDiagnostic> inspectDraftSubFrogSemantics(const FrogDraftData &,const ConditionalDisableCompilationContext &);
}
