#pragma once
#include "frog_native_engine/CanonicalDraftMigration.hpp"

namespace frog::native_engine {
struct FrogDraftDataView;
// Reuses the ordinary adapter's closed raw-field validators before immutable
// call specialization. The sole call key deferred is an actual frog_call.call.
// typeDefinitionsValidated is a private owner handoff: accepted()+linked from
// prepareResolvedTypeDefinitionSource on THESE exact input bytes, not a user
// option. Every deferred type_definition must also match its parsed owner.
[[nodiscard]] std::vector<CanonicalDraftMigrationDiagnostic> validateCanonicalSubFrogProjectionSource(
    std::string_view originalUtf8,FrogDraftDataView ,
    bool typeDefinitionsValidated = false);
inline constexpr std::string_view canonicalComposedProgramProfile="frog.studio.composed-program@1";
// Pure source adaptation.  This is an extension of the existing draft adapter,
// not a second editable model, FIR representation or execution realization.
bool canonicalDraftNeedsComposition(const source::JsonValue &,const CanonicalDraftResolvedTypes *resolved=nullptr);
CanonicalDraftMigrationResult prepareCanonicalComposedDraftMigration(
    std::string_view originalUtf8,std::string_view originalUri,
    const CanonicalDraftResolvedTypes *);
}
