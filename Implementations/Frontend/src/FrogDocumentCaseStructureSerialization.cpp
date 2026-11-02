#include "FrogDocumentCaseStructureSerialization.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"
#include "frog_native_engine/BindingType.hpp"

#include <sstream>

namespace frog::native_engine::document_internal {

std::string serializeCaseStructureProperties(const DiagramNode &node, int indent) {
    std::ostringstream output;
    if (node.caseStructureManualOrder) {
        appendIndent(output, indent);
        output << "\"case_structure_manual_order\": true,\n";
    }
    appendIndent(output, indent);
    output << "\"case_structure_subdiagram_label_visible\": "
           << (node.structureSubdiagramLabelVisible ? "true" : "false")
           << ",\n";
    appendIndent(output, indent);
    output << "\"case_structure_true_subdiagram_label_text\": ";
    appendJsonString(output, node.caseStructureTrueSubdiagramLabelText);
    output << ",\n";
    appendIndent(output, indent);
    output << "\"case_structure_false_subdiagram_label_text\": ";
    appendJsonString(output, node.caseStructureFalseSubdiagramLabelText);
    output << ",\n";
    appendIndent(output, indent);
    output << "\"case_structure_subdiagram_label_height\": "
           << clampStructureSubdiagramLabelHeight(
                node.structureSubdiagramLabelHeight)
           << ",\n";
    if (node.structureSubdiagramLabelBorderRgb.has_value()) {
        appendIndent(output, indent);
        output << "\"structure_subdiagram_label_border_rgb\": "
               << node.structureSubdiagramLabelBorderRgb.value()
               << ",\n";
    }
    appendSubdiagramLabelFont(output, node, indent);
    // boundary is the canonical executable contract. structure_tunnels only
    // preserves authoring placement for those same public boundary ids.
    appendStructureTunnelPlacements(output, node, indent);
    appendColorProperty(output, indent, "case_structure_true_background_rgb",
        node.caseStructureTrueBackgroundRgb, "case_structure_true_background_opacity_percent",
        node.caseStructureTrueBackgroundOpacityPercent);
    appendColorProperty(output, indent, "case_structure_true_subframe_rgb",
        node.caseStructureTrueSubframeRgb, "case_structure_true_subframe_opacity_percent",
        node.caseStructureTrueSubframeOpacityPercent);
    appendColorProperty(output, indent, "case_structure_false_background_rgb",
        node.caseStructureFalseBackgroundRgb, "case_structure_false_background_opacity_percent",
        node.caseStructureFalseBackgroundOpacityPercent);
    appendColorProperty(output, indent, "case_structure_false_subframe_rgb",
        node.caseStructureFalseSubframeRgb, "case_structure_false_subframe_opacity_percent",
        node.caseStructureFalseSubframeOpacityPercent);
    return output.str();
}

std::string serializeCaseStructureSelectorTerminal(
    const DiagramNode &node,
    int indent) {
    std::ostringstream output;
    const std::string_view selectorType =
        node.caseStructureSelectorValueType.empty()
        ? std::string_view{ "bool" }
        : std::string_view{ node.caseStructureSelectorValueType };
    const bool stringSelector = selectorType == "string";
    const bool enumSelector = binding::parseType(selectorType).type.kind == binding::TypeKind::Enum;
    appendIndent(output, indent);
    output << "\"binding_profile\": \"frog.typed-binding@1\",\n";
    appendIndent(output, indent);
    output << "\"structure_terminals\": {\n";
    appendIndent(output, indent + 1);
    output << "\"selector\": {\n";
    appendIndent(output, indent + 2);
    output << "\"type\": ";
    appendJsonString(output, selectorType);
    output << ",\n";
    appendIndent(output, indent + 2);
    output << "\"outer_visible\": true,\n";
    appendIndent(output, indent + 2);
    output << "\"exposed_in_body\": true,\n";
    appendIndent(output, indent + 2);
    output << "\"read_only\": true,\n";
    appendIndent(output, indent + 2);
    output << "\"outer_port\": \"selector\",\n";
    appendIndent(output, indent + 2);
    output << "\"body_port\": \"selected_case\",\n";
    appendIndent(output, indent + 2);
    output << "\"role\": \"selector\"";
    if (stringSelector ||
        (enumSelector && !node.caseStructureSelectorEnumItems.empty())) {
        output << ",\n";
    } else {
        output << '\n';
    }
    if (stringSelector) {
        appendIndent(output, indent + 2);
        output << "\"match_policy\": {\n";
        appendIndent(output, indent + 3);
        output << "\"case_sensitivity\": \""
               << (node.caseStructureStringCaseInsensitive
                    ? "insensitive"
                    : "sensitive")
               << "\",\n";
        appendIndent(output, indent + 3);
        output << "\"normalization\": \"none\"\n";
        appendIndent(output, indent + 2);
        output << '}' << (enumSelector &&
            !node.caseStructureSelectorEnumItems.empty() ? "," : "")
               << '\n';
    }
    if (enumSelector && !node.caseStructureSelectorEnumItems.empty()) {
        appendIndent(output, indent + 2);
        output << "\"enum_items\": [\n";
        for (std::size_t index = 0U;
             index < node.caseStructureSelectorEnumItems.size(); ++index) {
            const auto &item = node.caseStructureSelectorEnumItems[index];
            appendIndent(output, indent + 3);
            output << "{\"item_id\": ";
            appendJsonString(
                output,
                item.id.empty()
                    ? "item_" + std::to_string(index + 1U)
                    : item.id);
            output << ", \"display_name\": ";
            appendJsonString(output, item.label);
            output << ", \"ordinal\": " << item.value;
            // Private authoring metadata: omitted legacy flags retain their
            // original defaults, without changing the public Enum definition.
            if (item.disabled) output << ", \"disabled\": true";
            if (!item.visible) output << ", \"visible\": false";
            output << '}'
                   << (index + 1U <
                           node.caseStructureSelectorEnumItems.size()
                       ? ","
                       : "")
                   << '\n';
        }
        appendIndent(output, indent + 2);
        output << "]\n";
    }
    appendIndent(output, indent + 1);
    output << "}\n";
    appendIndent(output, indent);
    output << "},\n";
    return output.str();
}

} // namespace frog::native_engine::document_internal
