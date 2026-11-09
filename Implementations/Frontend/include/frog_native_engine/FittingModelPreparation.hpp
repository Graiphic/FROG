#pragma once
#include "frog_native_engine/DraftFrontendPreparation.hpp"
#include "frog_native_engine/CanonicalExecutionIr.hpp"
#include <functional>

namespace frog::native_engine {
struct FittingModelPreparationLimits {
 std::size_t maximumModels=kMaximumFrogSourceTransactionEdits;
 std::size_t maximumDepth=32;
 std::size_t maximumClosureBytes=64U*1024U*1024U;
 CanonicalIrLimits fir;
};
struct FittingModelInputMapping {
 std::string semanticId,interfacePortId,widgetId,type;
};
struct FittingModelOutputMapping {
 std::string semanticId,interfacePortId,widgetId,type;
 // The actual indicator endpoint is the guaranteed native publication point.
 std::string nodeId,portId;
 // Also retain its exact incoming producer for source navigation/auditing.
 std::string producerNodeId,producerPortId;
};
struct PreparedFittingModelSource {
 std::string referenceNodeId,ownerSourceUri,sourcePointer;
 DraftDependencyResult source;
 std::shared_ptr<const DraftFrontendPreparation> preparation;
 std::shared_ptr<const FrogSourceDocument> canonicalSource;
 std::shared_ptr<const CanonicalExecutionIr> fir;
 std::vector<FittingModelInputMapping> inputs;
 std::vector<FittingModelOutputMapping> outputs;
 std::vector<std::shared_ptr<const PreparedFittingModelSource>> children;
 std::string closureFingerprint;
 // The existing immutable callable owner serves distinct exact profiles.
 // Appended fields preserve existing Fitting aggregate/source compatibility.
 std::string profile="frog.studio.math.fitting.model.source@1";
 unsigned dimensions=1;
};
struct FittingModelPreparation {
 std::vector<std::shared_ptr<const PreparedFittingModelSource>> models;
 std::vector<DraftSubFrogFrozenSource> frozenSources;
 std::vector<DocumentDiagnostic> diagnostics;
 std::string closureFingerprint;
 bool incomplete=false;
 [[nodiscard]] bool accepted()const noexcept{return diagnostics.empty()&&!incomplete;}
};
// No disk, runtime, LLVM, Canvas or repairs. The host supplies immutable
// resolver contents and a cancellation/limit checkpoint; checkpoint exceptions
// propagate to its operation owner, as in prepareDraftFrontend.
[[nodiscard]] FittingModelPreparation prepareFittingModelSources(
 const DraftFrontendPreparation &,const ConditionalDisableCompilationContext &,
 const DraftDependencyResolver &,const FittingModelPreparationLimits & = {},
 const std::function<void()> &checkpoint={});
[[nodiscard]] FittingModelPreparation prepareFittingModelSources(
 FrogDraftDataView,std::string_view sourceUri,const ConditionalDisableCompilationContext &,
 const DraftDependencyResolver &,const FittingModelPreparationLimits & = {},
 const std::function<void()> &checkpoint={});
// An immutable Source transaction, never an edit of the original draft. The
// authored node's existing studio_configuration.call is the required edit
// precondition. Shared ordinary SubFROG/type preparation remains untouched.
[[nodiscard]] std::optional<FrogSourceDocument> attachPreparedFittingModels(
 const FrogSourceDocument &,const FittingModelPreparation &,
 std::vector<DocumentDiagnostic> *diagnostics=nullptr,
 const FittingModelPreparationLimits & = {},const std::function<void()> &checkpoint={});
[[nodiscard]] std::string preparedFittingModelRecipeJson(const PreparedFittingModelSource &);
}
