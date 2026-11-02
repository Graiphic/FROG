#include "FrogDocumentForLoopSerialization.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"

#include "frog_native_engine/FrogDraftData.hpp"

#include <sstream>

namespace frog::native_engine::document_internal {

std::string serializeForLoopStructureProperties(
    const DiagramNode &node,
    std::string_view bodyDiagram,
    int indent) {
    std::ostringstream output;
    appendIndent(output, indent);
    output << "\"for_loop_conditional_terminal\": "
           << (node.forLoopConditionalTerminal ? "true" : "false") << ",\n";
    appendIndent(output, indent);
    output << "\"for_loop_stop_if_true\": "
           << (node.whileLoopStopIfTrue ? "true" : "false") << ",\n";
    appendIndent(output, indent);
    output << "\"for_loop_condition_normalized_x\": "
           << clampWhileLoopTerminalNormalizedPosition(node.whileLoopConditionNormalizedX) << ",\n";
    appendIndent(output, indent);
    output << "\"for_loop_condition_normalized_y\": "
           << clampWhileLoopTerminalNormalizedPosition(node.whileLoopConditionNormalizedY) << ",\n";
    appendIndent(output, indent);
    output << "\"for_loop_iteration_normalized_x\": "
           << clampForLoopTerminalNormalizedPosition(
                node.forLoopIterationNormalizedX) << ",\n";
    appendIndent(output, indent);
    output << "\"for_loop_iteration_normalized_y\": "
           << clampForLoopTerminalNormalizedPosition(
                node.forLoopIterationNormalizedY) << ",\n";
    appendIndent(output, indent);
    output << "\"for_loop_subdiagram_label_visible\": "
           << (node.structureSubdiagramLabelVisible ? "true" : "false")
           << ",\n";
    appendIndent(output, indent);
    output << "\"for_loop_subdiagram_label_text\": ";
    appendJsonString(output, node.forLoopSubdiagramLabelText);
    output << ",\n";
    appendIndent(output, indent);
    output << "\"for_loop_subdiagram_label_height\": "
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
    appendColorProperty(output, indent, "for_loop_background_rgb",
        node.forLoopBackgroundRgb, "for_loop_background_opacity_percent",
        node.forLoopBackgroundOpacityPercent);
    appendColorProperty(output, indent, "for_loop_subframe_rgb",
        node.forLoopSubframeRgb, "for_loop_subframe_opacity_percent",
        node.forLoopSubframeOpacityPercent);
    appendStructureTunnels(output, node, indent);
    appendIndent(output, indent);
    output << "\"structure_terminals\": {\n";
    if (node.forLoopConditionalTerminal) {
        appendIndent(output, indent + 1);
        output << "\"condition\": { \"type\": \"bool\", \"outer_visible\": false, "
               << "\"exposed_in_body\": true, \"read_only\": false, \"body_port\": \"condition\", \"role\": \""
               << (node.whileLoopStopIfTrue ? "stop_if_true" : "continue_while_true") << "\" },\n";
    }
    appendIndent(output, indent + 1);
    output << "\"count\": {\n";
    appendIndent(output, indent + 2);
    output << "\"type\": \"i64\",\n";
    appendIndent(output, indent + 2);
    output << "\"outer_visible\": true,\n";
    appendIndent(output, indent + 2);
    output << "\"exposed_in_body\": true,\n";
    appendIndent(output, indent + 2);
    output << "\"read_only\": true,\n";
    appendIndent(output, indent + 2);
    output << "\"outer_port\": \"count\",\n";
    appendIndent(output, indent + 2);
    output << "\"body_port\": \"count_value\",\n";
    appendIndent(output, indent + 2);
    output << "\"role\": \"count\"\n";
    appendIndent(output, indent + 1);
    output << "},\n";
    appendIndent(output, indent + 1);
    output << "\"index\": {\n";
    appendIndent(output, indent + 2);
    output << "\"type\": \"i64\",\n";
    appendIndent(output, indent + 2);
    output << "\"outer_visible\": false,\n";
    appendIndent(output, indent + 2);
    output << "\"exposed_in_body\": true,\n";
    appendIndent(output, indent + 2);
    output << "\"read_only\": true,\n";
    appendIndent(output, indent + 2);
    output << "\"body_port\": \"iteration\",\n";
    appendIndent(output, indent + 2);
    output << "\"role\": \"index\"\n";
    appendIndent(output, indent + 1);
    output << "}\n";
    appendIndent(output, indent);
    output << "},\n";
    appendIndent(output, indent);
    output << "\"regions\": [\n";
    appendIndent(output, indent + 1);
    output << "{\n";
    appendIndent(output, indent + 2);
    output << "\"id\": \"" << kForLoopBodyRegionId << "\",\n";
    output << bodyDiagram;
    appendIndent(output, indent + 1);
    output << "}\n";
    appendIndent(output, indent);
    output << "],\n";
    return output.str();
}

} // namespace frog::native_engine::document_internal
