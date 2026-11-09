#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/ConditionalDisable.hpp"
#include "frog_native_engine/DocumentDiagnostic.hpp"
#include <span>
namespace frog::native_engine {
struct DocumentReadiness {
    bool parsable = false;
    bool editable = false;
    bool structurallyValid = false;
    bool semanticallyValid = false;
    bool buildable = false;
    bool runnable = false;
    std::vector<DocumentDiagnostic> diagnostics;
    // An admission boundary can stop before complete Source validation. False
    // validity flags then mean unknown, not proof of an invalid document.
    // Build/Run remain blocked; any established source error takes precedence.
    bool validationIncomplete = false;

    [[nodiscard]] bool hasDiagnostic(std::string_view code) const noexcept;
};

// Read-only inspection of an existing private frog.document.draft declaration.
// Source entries and identity/order are preserved, including invalid entries;
// this view neither authors an interface nor establishes public validity/FIR.
struct FrogDraftDeclaredPort {
    bool input = true;
    std::size_t ordinal = 0U;
    std::string sourcePath;
    std::string sourcePortUtf8;
    std::string id;
    std::string typeExpression;
    binding::TypeParseResult parsedType;
    std::optional<std::string> sourceName;
    std::optional<std::string> sourceLabel;
    std::string connection = "recommended";
    bool connectionExplicit = false;
};

struct FrogDraftInterfaceInspection {
    FrogDraftInterface declaration; // shares the immutable retained source
    bool inspectable = false;
    // Current private declaration rules only. A true value does not establish
    // type support, widget realization, public semantic validity or execution.
    bool contractValid = false;
    std::vector<FrogDraftDeclaredPort> ports;
    std::vector<DocumentDiagnostic> diagnostics;
};

[[nodiscard]] FrogDraftInterfaceInspection inspectFrogDraftInterface(
    const FrogDraftInterface &declaration);

// Read-only private declaration/binding refusals for explicit authoring.
// Reuses the readiness owners for interface IDs, policies, widget bindings and
// active FROG-call port IDs. It borrows source state and document compilation
// properties; it neither runs full semantic checking nor touches a UI cache.
// The canvas supplies explicit existing bindings only, never the declaration.
// This result does not establish public-source validity, FIR or executability.
[[nodiscard]] std::vector<DocumentDiagnostic> inspectFrogDraftInterfaceBindings(
    const std::optional<FrogDraftInterface> &declaration,
    FrogWidgetDataView canvas, std::span<const DiagramNode> diagramNodes,
    const FrogSourceProperties &properties,
    const ConditionalDisableCompilationContext &compilationContext);

void addDocumentDiagnostic(DocumentReadiness &,DocumentDiagnosticSeverity,DocumentDiagnosticDomain,std::string,std::string,std::string,bool,bool,bool,bool);
void validateDeclaredDraftInterface(const std::optional<FrogDraftInterface> &,FrogWidgetDataView,std::span<const DiagramNode *const>,DocumentReadiness &);
void validatePublicInterface(FrogWidgetDataView,DocumentReadiness &);
void validateFrogCallPortIdentities(const DiagramNode &,DocumentReadiness &);
void locateDraftDiagnostics(std::span<const DiagramNode>,std::span<const DiagramEdge>,std::span<const DiagramWireFragment>,DocumentReadiness &);
}
