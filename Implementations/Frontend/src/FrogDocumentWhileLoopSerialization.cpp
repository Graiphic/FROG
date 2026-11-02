#include "FrogDocumentWhileLoopSerialization.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"

#include "frog_native_engine/FrogDraftData.hpp"

#include <sstream>

namespace frog::native_engine::document_internal {

std::string serializeWhileLoopStructureProperties(
    const DiagramNode &node,
    std::string_view bodyDiagram,
    int indent) {
    std::ostringstream output;
    const auto appendPosition = [&](std::string_view name, double value) {
        appendIndent(output, indent);
        output << '"' << name << "\": "
               << clampWhileLoopTerminalNormalizedPosition(value) << ",\n";
    };
    appendPosition("while_loop_iteration_normalized_x",
        node.whileLoopIterationNormalizedX);
    appendPosition("while_loop_iteration_normalized_y",
        node.whileLoopIterationNormalizedY);
    appendPosition("while_loop_condition_normalized_x",
        node.whileLoopConditionNormalizedX);
    appendPosition("while_loop_condition_normalized_y",
        node.whileLoopConditionNormalizedY);
    appendIndent(output, indent);
    output << "\"while_loop_iteration_visible\": "
           << (node.whileLoopIterationVisible ? "true" : "false")
           << ",\n";
    appendIndent(output, indent);
    output << "\"while_loop_condition_visible\": "
           << (node.whileLoopConditionVisible ? "true" : "false")
           << ",\n";
    appendIndent(output, indent);
    output << "\"while_loop_stop_if_true\": "
           << (node.whileLoopStopIfTrue ? "true" : "false") << ",\n";
    appendIndent(output, indent);
    output << "\"while_loop_subdiagram_label_visible\": "
           << (node.structureSubdiagramLabelVisible ? "true" : "false")
           << ",\n";
    appendIndent(output, indent);
    output << "\"while_loop_subdiagram_label_text\": ";
    appendJsonString(output, node.forLoopSubdiagramLabelText);
    output << ",\n";
    appendIndent(output, indent);
    output << "\"while_loop_subdiagram_label_height\": "
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
    appendColorProperty(output, indent, "while_loop_background_rgb",
        node.whileLoopBackgroundRgb, "while_loop_background_opacity_percent",
        node.whileLoopBackgroundOpacityPercent);
    appendColorProperty(output, indent, "while_loop_subframe_rgb",
        node.whileLoopSubframeRgb, "while_loop_subframe_opacity_percent",
        node.whileLoopSubframeOpacityPercent);
    appendStructureTunnels(output, node, indent);

    // The terminal mode is explicit: the red state stops when True and the
    // green state continues when True. The iteration index remains I64.
    appendIndent(output, indent);
    output << "\"structure_terminals\": {\n";
    appendIndent(output, indent + 1);
    output << "\"condition\": {\n";
    appendIndent(output, indent + 2);
    output << "\"type\": \"bool\",\n";
    appendIndent(output, indent + 2);
    output << "\"outer_visible\": false,\n";
    appendIndent(output, indent + 2);
    output << "\"exposed_in_body\": true,\n";
    appendIndent(output, indent + 2);
    output << "\"read_only\": false,\n";
    appendIndent(output, indent + 2);
    output << "\"body_port\": \"condition\",\n";
    appendIndent(output, indent + 2);
    output << "\"role\": \""
           << (node.whileLoopStopIfTrue
               ? "stop_if_true"
               : "continue_while_true")
           << "\"\n";
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
    output << "\"id\": \"" << kWhileLoopBodyRegionId << "\",\n";
    output << bodyDiagram;
    appendIndent(output, indent + 1);
    output << "}\n";
    appendIndent(output, indent);
    output << "],\n";
    return output.str();
}

} // namespace frog::native_engine::document_internal
