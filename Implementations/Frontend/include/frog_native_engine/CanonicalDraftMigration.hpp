#pragma once
#include "FrogSourceDocument.hpp"
#include "DocumentDiagnostic.hpp"
#include <optional>

namespace frog::native_engine {
struct CanonicalDraftMigrationDiagnostic {
    std::string code,path,message;
    bool blocking=true;
    DocumentDiagnosticDomain domain=DocumentDiagnosticDomain::Compatibility;
};
struct CanonicalDraftMigrationIdentity {
    std::string kind,id,originalPointer,publicPointer;
    bool generated=false;
};
struct CanonicalDraftMigrationControl {
    std::string widgetId,type,valueJson;
};
// Private host-only handoff from the actual authored binding resolver. It is
// tied to exact source bytes; no agent argument, destination hint or numeric
// value is a substitute for accepted, complete source-operand identities.
struct CanonicalDraftScalarOperandType {
    std::string inputPortId,sourceNodeId,sourcePortId,valueType;
    bool accepted=false,complete=false;
};
struct CanonicalDraftScalarResolution {
    std::string nodeId,authoredOperation;
    std::vector<CanonicalDraftScalarOperandType> operands;
};
// Complete authored connector evidence for the local composed-program profile.
// The producer is the common, read-only TypeResolver at preparation time.  An
// input records its real source endpoint when one exists; no inferred canvas
// enclosure, display label or destination promotion establishes an identity.
struct CanonicalDraftPortResolution {
    std::string nodeId,portId;
    bool input=false;
    std::string fullTypeJson;
    std::string sourceNodeId,sourcePortId;
};
struct CanonicalDraftWidgetResolution {
    std::string widgetId,fullTypeJson,currentWidgetJson,defaultWidgetJson;
    // Exact typed values from the same hash-bound model, including compact Arrays.
    std::string currentValueJson,defaultValueJson;
};
struct CanonicalDraftResolvedTypes {
    std::string originalSha256;
    std::vector<CanonicalDraftScalarResolution> scalars;
    std::vector<CanonicalDraftPortResolution> ports;
    // Exact private codec records from the same current model. Defaults use
    // the existing identity-based compatibility policy on a detached value.
    std::vector<CanonicalDraftWidgetResolution> widgets;
};
// One finite historical widget contract, shared by ordinary/composed migration
// and the pre-projection guard. The existing model codec supplies exact facts;
// this operation never invents a value or edits the authored source.
struct CanonicalDraftWidgetNormalization {
    std::optional<source::JsonValue> current;
    std::vector<CanonicalDraftMigrationDiagnostic> diagnostics;
};
[[nodiscard]] CanonicalDraftWidgetNormalization normalizeCanonicalDraftWidget(
    const source::JsonValue &,const std::string &path,const CanonicalDraftResolvedTypes *);
// Preparing a proposal neither edits a document nor writes a file. The caller
// explicitly accepts a new public-source document at a distinct destination.
// Private scalar literals are marked frog.studio.scalar-constant@1 in the
// derived source; this is a local profile, not a claim that the unmarked public
// constant family is closed. Literal nodes retain exact values and introduce
// no Front Panel controls. Region identities remain owner-qualified in the
// attribution map; diagram membership always comes from explicit regions.
struct CanonicalDraftMigrationResult {
    std::shared_ptr<const std::string> originalSource;
    std::string originalUri,originalSha256;
    std::shared_ptr<const FrogSourceDocument> publicSource;
    std::vector<CanonicalDraftMigrationIdentity> identities;
    std::vector<CanonicalDraftMigrationControl> controls;
    std::vector<CanonicalDraftMigrationDiagnostic> diagnostics;
    [[nodiscard]] bool accepted()const noexcept{return bool(publicSource);}
};
[[nodiscard]] CanonicalDraftMigrationResult prepareCanonicalDraftMigration(
    std::string_view originalUtf8,std::string_view originalUri);
[[nodiscard]] CanonicalDraftMigrationResult prepareCanonicalDraftMigration(
    std::string_view originalUtf8,std::string_view originalUri,const CanonicalDraftResolvedTypes &);
}
