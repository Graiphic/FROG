#pragma once

#include "frog_native_engine/FrogDraftData.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace frog::native_engine {

using ConditionalDisableSymbolMap =
    std::unordered_map<std::string, std::string>;

struct ConditionalDisableCompilationContext {
    ConditionalDisableSymbolMap builtInSymbols;
    ConditionalDisableSymbolMap projectSymbols;
    ConditionalDisableSymbolMap targetSymbols;
};

enum class ConditionalDisableSelectionStatus { Selected, Invalid, Unresolved, NoMatch };

struct ConditionalDisableSelection {
    ConditionalDisableSelectionStatus status = ConditionalDisableSelectionStatus::NoMatch;
    std::optional<std::size_t> index;
    std::string code;
    std::string message;
};

// Validate the complete AST before evaluating it, including masked branches.
// Evaluation itself short-circuits AND/OR in source order. A missing symbol on
// the evaluated path is unresolved (not false); it cannot select Default.
[[nodiscard]] ConditionalDisableSelection resolveConditionalDisableSelection(
    const std::vector<DiagramConditionalDisableCase> &cases,
    const ConditionalDisableSymbolMap &symbols);
[[nodiscard]] std::string validateConditionalDisableCases(
    const std::vector<DiagramConditionalDisableCase> &cases, bool allowDrafts = false);


[[nodiscard]] bool isConditionalDisableBuiltInSymbol(std::string_view name) noexcept;
[[nodiscard]] ConditionalDisableCompilationContext conditionalDisableContextForDocument(
    const FrogSourceProperties &properties, const ConditionalDisableCompilationContext &target);

[[nodiscard]] ConditionalDisableSymbolMap resolveConditionalDisableSymbols(
    const ConditionalDisableCompilationContext &context);

[[nodiscard]] bool evaluateConditionalDisableCase(
    const DiagramConditionalDisableCase &conditionalCase,
    const ConditionalDisableSymbolMap &symbols);

[[nodiscard]] std::optional<std::size_t> resolveConditionalDisableCaseIndex(
    const std::vector<DiagramConditionalDisableCase> &cases,
    const ConditionalDisableSymbolMap &symbols);

[[nodiscard]] std::string conditionalDisableCaseDisplayExpression(
    const DiagramConditionalDisableCase &conditionalCase);

} // namespace frog::native_engine
