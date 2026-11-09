#include "FrogDocumentTimedLoopSerialization.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"
#include "FrogDocumentJsonWriter.hpp"
#include "frog_native_engine/TimedLoopConfiguration.hpp"

#include "frog_native_engine/DiagramStructureShelfCatalog.hpp"
#include "frog_native_engine/FrogDraftData.hpp"

#include <sstream>

namespace frog::native_engine::document_internal {
namespace {

void appendTimingNode(
    std::ostringstream &out,
    int level,
    DiagramTimedLoopTimingNodeRole timingRole,
    bool comma) {
    const auto &spec = diagramTimedLoopTimingNodeSpec(timingRole);
    appendIndent(out, level); out << '"' << spec.id << "\": { \"role\": \""
        << spec.semanticRole << "\", \"direction\": \"" << spec.direction
        << "\", \"fields\": [\n";
    const auto fields = spec.fields;
    for (std::size_t index = 0; index < fields.size(); ++index) {
        appendIndent(out, level + 1); out << "{ \"id\": "; appendJsonString(out, fields[index].id);
        out << ", \"label\": "; appendJsonString(out, fields[index].label);
        out << ", \"type\": "; appendJsonString(out, fields[index].valueType);
        out << " }" << (index + 1U < fields.size() ? "," : "") << '\n';
    }
    appendIndent(out, level); out << "] }" << (comma ? "," : "") << '\n';
}

} // namespace

std::string serializeTimedLoopStructureProperties(
    const DiagramNode &node,
    std::string_view bodyDiagram,
    int level) {
    std::ostringstream out;
    appendSubdiagramLabelAppearance(out, node, level);
    appendIndent(out, level);
    out << "\"timed_loop_subdiagram_label_text\": ";
    appendJsonString(out, node.forLoopSubdiagramLabelText);
    out << ",\n";
    const auto number = [&](std::string_view name, auto value) {
        appendIndent(out, level); out << '"' << name << "\": " << value << ",\n";
    };
    const auto stringArray = [&](std::string_view name,
                                 const std::vector<std::string> &values) {
        appendIndent(out, level); out << '"' << name << "\": [";
        for (std::size_t index = 0U; index < values.size(); ++index) {
            if (index > 0U) out << ", ";
            appendJsonString(out, values[index]);
        }
        out << "],\n";
    };
    number("timed_loop_iteration_normalized_x", clampWhileLoopTerminalNormalizedPosition(node.timedLoopIterationNormalizedX));
    number("timed_loop_iteration_normalized_y", clampWhileLoopTerminalNormalizedPosition(node.timedLoopIterationNormalizedY));
    number("timed_loop_condition_normalized_x", clampWhileLoopTerminalNormalizedPosition(node.timedLoopConditionNormalizedX));
    number("timed_loop_condition_normalized_y", clampWhileLoopTerminalNormalizedPosition(node.timedLoopConditionNormalizedY));
    number("timed_loop_initial_visible_row_count", clampTimedLoopVisibleRowCount(
        node.timedLoopInitialVisibleRowCount, diagramTimedLoopMaximumVisibleRows(
            DiagramTimedLoopTimingNodeRole::Initial)));
    number("timed_loop_feedback_visible_row_count", clampTimedLoopVisibleRowCount(
        node.timedLoopFeedbackVisibleRowCount, diagramTimedLoopMaximumVisibleRows(
            DiagramTimedLoopTimingNodeRole::Feedback)));
    number("timed_loop_update_visible_row_count", clampTimedLoopVisibleRowCount(
        node.timedLoopUpdateVisibleRowCount, diagramTimedLoopMaximumVisibleRows(
            DiagramTimedLoopTimingNodeRole::Update)));
    number("timed_loop_result_visible_row_count", clampTimedLoopVisibleRowCount(
        node.timedLoopResultVisibleRowCount, diagramTimedLoopMaximumVisibleRows(
            DiagramTimedLoopTimingNodeRole::Result)));
    stringArray("timed_loop_initial_fields", node.timedLoopInitialFields);
    stringArray("timed_loop_feedback_fields", node.timedLoopFeedbackFields);
    stringArray("timed_loop_update_fields", node.timedLoopUpdateFields);
    stringArray("timed_loop_result_fields", node.timedLoopResultFields);
    number("timed_loop_initial_timing_node_normalized_y",
        clampEventStructureDataNodeNormalizedY(node.timedLoopInitialTimingNodeNormalizedY));
    number("timed_loop_feedback_timing_node_normalized_y",
        clampEventStructureDataNodeNormalizedY(node.timedLoopFeedbackTimingNodeNormalizedY));
    number("timed_loop_update_timing_node_normalized_y",
        clampEventStructureDataNodeNormalizedY(node.timedLoopUpdateTimingNodeNormalizedY));
    number("timed_loop_result_timing_node_normalized_y",
        clampEventStructureDataNodeNormalizedY(node.timedLoopResultTimingNodeNormalizedY));
    appendColorProperty(out, level, "timed_loop_background_rgb",
        node.timedLoopBackgroundRgb, "timed_loop_background_opacity_percent",
        node.timedLoopBackgroundOpacityPercent);
    appendColorProperty(out, level, "timed_loop_subframe_rgb",
        node.timedLoopSubframeRgb, "timed_loop_subframe_opacity_percent",
        node.timedLoopSubframeOpacityPercent);
    appendStructureTunnels(out, node, level);
    appendIndent(out, level); out << "\"timed_loop_schedule\": {\n";
    appendIndent(out, level + 1); out << "\"timing_source\": "; appendJsonString(out, node.timedLoopSchedule.timingSource); out << ",\n";
    number("period", node.timedLoopSchedule.period); number("offset", node.timedLoopSchedule.offset);
    number("deadline", node.timedLoopSchedule.deadline); number("timeout", node.timedLoopSchedule.timeout);
    number("priority", node.timedLoopSchedule.priority); number("processor", node.timedLoopSchedule.processor);
    appendIndent(out, level + 1); out << "\"late_iteration_policy\": { \"missed_periods\": ";
    appendJsonString(out, node.timedLoopSchedule.missedPeriods); out << ", \"phase\": ";
    appendJsonString(out, node.timedLoopSchedule.phasePolicy); out << " }\n";
    appendIndent(out, level); out << "},\n";
    appendIndent(out, level); out << "\"studio_timed_loop_options\": ";
    appendJsonValue(out,timed_loop::optionsJson(node.timedLoopOptions));out << ",\n";
    appendIndent(out, level); out << "\"timing_nodes\": {\n";
    appendTimingNode(out, level + 1, DiagramTimedLoopTimingNodeRole::Initial, true);
    appendTimingNode(out, level + 1, DiagramTimedLoopTimingNodeRole::Feedback, true);
    appendTimingNode(out, level + 1, DiagramTimedLoopTimingNodeRole::Update, true);
    appendTimingNode(out, level + 1, DiagramTimedLoopTimingNodeRole::Result, false);
    appendIndent(out, level); out << "},\n";
    appendIndent(out, level); out << "\"structure_terminals\": {\n";
    appendIndent(out, level + 1); out << "\"condition\": { \"type\": \"bool\", \"outer_visible\": false, \"exposed_in_body\": true, \"read_only\": false, \"body_port\": \"condition\", \"role\": \"continue_while_true\" },\n";
    appendIndent(out, level + 1); out << "\"index\": { \"type\": \"i64\", \"outer_visible\": false, \"exposed_in_body\": true, \"read_only\": true, \"body_port\": \"iteration\", \"role\": \"index\" }\n";
    appendIndent(out, level); out << "},\n";
    appendIndent(out, level); out << "\"regions\": [\n";
    appendIndent(out, level + 1); out << "{\n";
    appendIndent(out, level + 2); out << "\"id\": \"" << kTimedLoopBodyRegionId << "\",\n" << bodyDiagram;
    appendIndent(out, level + 1); out << "}\n";
    appendIndent(out, level); out << "],\n";
    return out.str();
}

} // namespace frog::native_engine::document_internal
