#include "FrogDocumentConditionalDisableSerialization.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"

#include "frog_native_engine/FrogDraftData.hpp"

#include <algorithm>
#include <sstream>

namespace frog::native_engine::document_internal {
namespace {

void appendCondition(
    std::ostringstream &output,
    const DiagramConditionalDisableCase &conditionalCase,
    int indent) {
    appendIndent(output, indent);
    output << "\"condition\": { \"any\": [";
    for (std::size_t termIndex = 0U;
         termIndex < conditionalCase.anyOf.size(); ++termIndex) {
        const auto &term = conditionalCase.anyOf[termIndex];
        if (termIndex > 0U) output << ", ";
        output << "{ \"all\": [";
        for (std::size_t comparisonIndex = 0U;
             comparisonIndex < term.comparisons.size(); ++comparisonIndex) {
            const auto &comparison = term.comparisons[comparisonIndex];
            if (comparisonIndex > 0U) output << ", ";
            output << "{ \"symbol\": ";
            appendJsonString(output, comparison.symbol);
            output << ", \"operator\": ";
            appendJsonString(output, comparison.comparison);
            output << ", \"value\": ";
            appendJsonString(output, comparison.value);
            output << " }";
        }
        output << "] }";
    }
    output << "] },\n";
}

} // namespace

std::string serializeConditionalDisableProperties(
    const DiagramNode &node,
    const std::function<std::string(std::string_view)> &serializeRegion,
    int indent) {
    std::ostringstream output;
    appendStructureTunnels(output, node, indent);
    appendSubdiagramLabelAppearance(output, node, indent);
    appendIndent(output, indent);
    output << "\"structure_terminals\": {},\n";
    appendIndent(output, indent);
    output << "\"regions\": [\n";
    for (std::size_t index = 0U;
         index < node.conditionalDisableCases.size(); ++index) {
        const auto &conditionalCase = node.conditionalDisableCases[index];
        appendIndent(output, indent + 1);
        output << "{\n";
        appendIndent(output, indent + 2);
        output << "\"id\": "; appendJsonString(output, conditionalCase.id);
        output << ",\n";
        if (!conditionalCase.subdiagramLabelText.empty()) {
            appendIndent(output, indent + 2);
            output << "\"subdiagram_label_text\": ";
            appendJsonString(output, conditionalCase.subdiagramLabelText);
            output << ",\n";
        }
        if (conditionalCase.isDefault) {
            appendIndent(output, indent + 2);
            output << "\"default\": true,\n";
        }
        if (conditionalCase.conditionDraft) {
            appendIndent(output, indent + 2);
            output << "\"condition_draft\": true,\n";
        }
        if ((!conditionalCase.isDefault && !conditionalCase.conditionDraft) || !conditionalCase.anyOf.empty()) {
            appendCondition(output, conditionalCase, indent + 2);
        }
        output << serializeRegion(conditionalCase.id);
        appendIndent(output, indent + 1);
        output << '}'
               << (index + 1U < node.conditionalDisableCases.size() ? "," : "")
               << '\n';
    }
    appendIndent(output, indent);
    output << "],\n";
    appendIndent(output, indent);
    output << "\"conditional_disable_visible_case_index\": "
           << (std::min)(node.conditionalDisableVisibleCaseIndex,
                node.conditionalDisableCases.empty()
                    ? 0U : node.conditionalDisableCases.size() - 1U)
           << ",\n";
    appendColorProperty(output, indent,
        "conditional_disable_background_rgb",
        node.conditionalDisableBackgroundRgb,
        "conditional_disable_background_opacity_percent",
        node.conditionalDisableBackgroundOpacityPercent,
        true);
    appendColorProperty(output, indent,
        "conditional_disable_subframe_rgb",
        node.conditionalDisableSubframeRgb,
        "conditional_disable_subframe_opacity_percent",
        node.conditionalDisableSubframeOpacityPercent,
        true);
    appendColorProperty(output, indent,
        "conditional_disable_veil_rgb",
        node.conditionalDisableVeilRgb,
        "conditional_disable_veil_opacity_percent",
        node.conditionalDisableVeilOpacityPercent,
        true);
    return output.str();
}

} // namespace frog::native_engine::document_internal
