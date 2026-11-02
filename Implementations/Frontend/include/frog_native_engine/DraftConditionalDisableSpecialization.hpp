#pragma once
#include "frog_native_engine/ConditionalDisable.hpp"
#include <span>

namespace frog::native_engine {
struct ConditionalDisableDiagnostic {
    std::string code;
    std::string subjectId;
    std::string message;
    bool incomplete=false;
};
struct ConditionalDisableSelectionTrace {
    std::string structureId;
    ConditionalDisableSelection selection;
    std::string regionId;
};
struct ConditionalDisableAnalysis {
    ConditionalDisableSymbolMap symbols;
    std::vector<ConditionalDisableSelectionTrace> selections;
    std::vector<ConditionalDisableDiagnostic> diagnostics;
    // Length-prefixed canonical semantics, independent of layout/visible region.
    std::string semanticKey;
};
struct DraftConditionalDisableSpecialization : ConditionalDisableAnalysis { FrogDraftData document; };
// Borrowed authored nodes selected by the same compile-time activity owner as
// specialization. The input addresses remain owned by the caller. No graph is
// copied/lowered and no defaults, types, geometry or runtime plan are produced.
// Conditional containers themselves are omitted; selection failure leaves
// their children inactive. This narrow view does not establish source validity.
[[nodiscard]] std::vector<const DiagramNode *> activeConditionalDisableDiagramNodes(
    std::span<const DiagramNode> nodes, const FrogSourceProperties &properties,
    const ConditionalDisableCompilationContext &target);

// Creates an immutable build projection. The authored source and every Front
// Panel object remain untouched. Only selected bodies enter semantic checking,
// dependency discovery and lowering; all source IDs/ownership are checked.
// Resolved Use Default If Unwired outputs receive compiler-default constants
// only in their unwired regions. Authored wires and values are never replaced.
[[nodiscard]] DraftConditionalDisableSpecialization specializeConditionalDisableData(
    const FrogDraftData &source, const ConditionalDisableCompilationContext &target);
}
