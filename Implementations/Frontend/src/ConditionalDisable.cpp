#include "frog_native_engine/ConditionalDisable.hpp"

#include <utility>

namespace frog::native_engine {

ConditionalDisableSymbolMap resolveConditionalDisableSymbols(
    const ConditionalDisableCompilationContext &context) {
    ConditionalDisableSymbolMap resolved = context.builtInSymbols;
    for (const auto &[name, value] : context.projectSymbols) {
        resolved[name] = value;
    }
    for (const auto &[name, value] : context.targetSymbols) {
        resolved[name] = value;
    }
    // Backend-owned built-ins cannot be forged by project/target custom symbols.
    for (const auto &[name, value] : context.builtInSymbols)
        if (isConditionalDisableBuiltInSymbol(name)) resolved[name] = value;
    return resolved;
}

bool isConditionalDisableBuiltInSymbol(std::string_view name) noexcept {
    return name == "OS" || name == "TARGET_TYPE" || name == "TARGET_BITNESS" ||
        name == "CPU" || name == "RUN_TIME_ENGINE";
}

ConditionalDisableCompilationContext conditionalDisableContextForDocument(
    const FrogSourceProperties &properties, const ConditionalDisableCompilationContext &target) {
    auto result = target;
    result.projectSymbols.insert(properties.compilationProjectSymbols.begin(), properties.compilationProjectSymbols.end());
    result.targetSymbols.insert(properties.compilationTargetSymbols.begin(), properties.compilationTargetSymbols.end());
    return result;
}

std::string validateConditionalDisableCases(
    const std::vector<DiagramConditionalDisableCase> &cases, bool allowDrafts) {
    if (cases.empty() || cases.size() > 128U) return "Expected 1 to 128 subdiagrams.";
    bool hasDefault = false;
    for (std::size_t i = 0; i < cases.size(); ++i) {
        const auto &region = cases[i];
        if (region.id.empty()) return "Subdiagram IDs cannot be empty.";
        for (std::size_t j = 0; j < i; ++j)
            if (cases[j].id == region.id) return "Duplicate subdiagram ID: " + region.id;
        if (region.conditionDraft) {
            if (region.isDefault || !region.anyOf.empty()) return "A condition draft cannot also be Default or a condition.";
            if (!allowDrafts) return "Configure the unfinished condition in subdiagram: " + region.id;
            continue;
        }
        if (region.isDefault) {
            if (hasDefault) return "Only one Default subdiagram is allowed.";
            if (!region.anyOf.empty()) return "Default cannot also have a condition.";
            hasDefault = true;
            continue;
        }
        if (region.anyOf.empty() || region.anyOf.size() > 64U)
            return "A condition requires 1 to 64 OR groups: " + region.id;
        for (const auto &term : region.anyOf) {
            if (term.comparisons.empty() || term.comparisons.size() > 64U)
                return "An AND group requires 1 to 64 comparisons: " + region.id;
            for (const auto &comparison : term.comparisons) {
                if (comparison.symbol.empty()) return "A symbol name cannot be empty: " + region.id;
                if (comparison.comparison != "==" && comparison.comparison != "!=")
                    return "Unsupported comparison operator: " + comparison.comparison;
            }
        }
    }
    return {};
}

namespace {
enum class Match { False, True, Unresolved };
Match evaluate(const DiagramConditionalDisableCase &conditionalCase,
    const ConditionalDisableSymbolMap &symbols, std::string &missingSymbol) {
    for (const auto &term : conditionalCase.anyOf) {
        bool termMatches = true;
        for (const auto &comparison : term.comparisons) {
            const auto symbol = symbols.find(comparison.symbol);
            if (symbol == symbols.end()) {
                missingSymbol = comparison.symbol;
                return Match::Unresolved;
            }
            const bool equal = symbol->second == comparison.value;
            if ((comparison.comparison == "==" && !equal) ||
                (comparison.comparison == "!=" && equal)) {
                termMatches = false;
                break;
            }
        }
        if (termMatches) return Match::True;
    }
    return Match::False;
}
} // namespace

ConditionalDisableSelection resolveConditionalDisableSelection(
    const std::vector<DiagramConditionalDisableCase> &cases,
    const ConditionalDisableSymbolMap &symbols) {
    if (const auto error = validateConditionalDisableCases(cases); !error.empty())
        return {ConditionalDisableSelectionStatus::Invalid, {}, "conditional_disable.invalid", error};
    std::optional<std::size_t> defaultIndex;
    for (std::size_t index = 0U; index < cases.size(); ++index) {
        if (cases[index].isDefault) {
            defaultIndex = index;
            continue;
        }
        std::string missing;
        const auto match = evaluate(cases[index], symbols, missing);
        if (match == Match::Unresolved)
            return {ConditionalDisableSelectionStatus::Unresolved, {}, "conditional_disable.symbol_unresolved",
                "Missing compilation symbol '" + missing + "' in subdiagram '" + cases[index].id + "'."};
        if (match == Match::True) return {ConditionalDisableSelectionStatus::Selected, index, {}, {}};
    }
    if (defaultIndex) return {ConditionalDisableSelectionStatus::Selected, defaultIndex, {}, {}};
    return {ConditionalDisableSelectionStatus::NoMatch, {}, "conditional_disable.no_selection",
        "No condition matches and the structure has no Default subdiagram."};
}

bool evaluateConditionalDisableCase(const DiagramConditionalDisableCase &conditionalCase,
    const ConditionalDisableSymbolMap &symbols) {
    if (conditionalCase.isDefault || !validateConditionalDisableCases({conditionalCase}).empty()) return false;
    std::string missing;
    return evaluate(conditionalCase, symbols, missing) == Match::True;
}

std::optional<std::size_t> resolveConditionalDisableCaseIndex(
    const std::vector<DiagramConditionalDisableCase> &cases,
    const ConditionalDisableSymbolMap &symbols) {
    return resolveConditionalDisableSelection(cases, symbols).index;
}

std::string conditionalDisableCaseDisplayExpression(
    const DiagramConditionalDisableCase &conditionalCase) {
    if (conditionalCase.isDefault) return "Default";
    std::string expression;
    for (std::size_t termIndex = 0U;
         termIndex < conditionalCase.anyOf.size(); ++termIndex) {
        const auto &term = conditionalCase.anyOf[termIndex];
        if (term.comparisons.empty()) continue;
        if (!expression.empty()) expression += " OR ";
        const bool parenthesize = conditionalCase.anyOf.size() > 1U &&
            term.comparisons.size() > 1U;
        if (parenthesize) expression += '(';
        for (std::size_t comparisonIndex = 0U;
             comparisonIndex < term.comparisons.size(); ++comparisonIndex) {
            if (comparisonIndex > 0U) expression += " AND ";
            const auto &comparison = term.comparisons[comparisonIndex];
            expression += comparison.symbol;
            expression += ' ';
            expression += comparison.comparison;
            expression += ' ';
            expression += comparison.value;
        }
        if (parenthesize) expression += ')';
    }
    return expression.empty() ? "Invalid condition" : expression;
}

} // namespace frog::native_engine
