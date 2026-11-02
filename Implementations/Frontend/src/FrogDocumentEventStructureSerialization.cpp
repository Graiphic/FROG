#include "FrogDocumentEventStructureSerialization.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"

#include "frog_native_engine/DiagramStructureShelfCatalog.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/EventStructureSchema.hpp"

#include <algorithm>
#include <sstream>
#include <iomanip>
#include <limits>

namespace frog::native_engine::document_internal {

std::string serializeEventStructureProperties(
    const DiagramNode &node,
    const std::vector<std::string> &caseDiagrams,
    int indent) {
    std::ostringstream output;
    output << std::setprecision(std::numeric_limits<double>::max_digits10);
    appendSubdiagramLabelAppearance(output, node, indent);
    appendIndent(output, indent);
    output << "\"event_structure_visible_case_index\": "
           << (std::min)(node.eventStructureVisibleCaseIndex,
                node.eventStructureCases.empty()
                    ? 0U
                    : node.eventStructureCases.size() - 1U) << ",\n";
    appendIndent(output, indent);
    output << "\"studio_event_dynamic_terminals_visible\": "
           << (node.eventDynamicTerminalsVisible ? "true" : "false") << ",\n";
    appendIndent(output, indent);
    output << "\"event_structure_data_node_normalized_y\": "
           << clampEventStructureDataNodeNormalizedY(
                node.eventStructureDataNodeNormalizedY) << ",\n";
    appendIndent(output, indent);
    output << "\"studio_event_response_node_normalized_y\": "
           << clampEventStructureDataNodeNormalizedY(
                node.eventStructureResponseNodeNormalizedY) << ",\n";
    appendColorProperty(output, indent, "event_structure_background_rgb",
        node.eventStructureBackgroundRgb,
        "event_structure_background_opacity_percent",
        node.eventStructureBackgroundOpacityPercent);
    appendColorProperty(output, indent, "event_structure_subframe_rgb",
        node.eventStructureSubframeRgb,
        "event_structure_subframe_opacity_percent",
        node.eventStructureSubframeOpacityPercent);
    appendStructureTunnels(output, node, indent);

    appendIndent(output, indent);
    output << "\"structure_terminals\": {\n";
    appendIndent(output, indent + 1);
    output << "\"timeout\": { \"type\": \"i64\", \"outer_visible\": true, "
              "\"exposed_in_body\": true, \"read_only\": true, "
              "\"outer_port\": \"timeout\", \"body_port\": \"timeout_value\", "
              "\"role\": \"timeout_milliseconds\" },\n";
    appendIndent(output, indent + 1);
    output << "\"event_data\": { \"outer_visible\": false, "
              "\"exposed_in_body\": true, \"read_only\": true, "
              "\"role\": \"event_data\", \"fields\": [";
    const auto eventDataFields = diagramEventStructureDataFields();
    for (std::size_t index = 0U; index < eventDataFields.size(); ++index) {
        if (index > 0U) output << ", ";
        const auto &field = eventDataFields[index];
        output << "{ \"id\": "; appendJsonString(output, field.id);
        output << ", \"type\": "; appendJsonString(output, field.valueType);
        output << ", \"body_port\": ";
        appendJsonString(output, std::string("event_data.") +
            std::string(field.id));
        output << " }";
    }
    output << "] }\n";
    appendIndent(output, indent);
    output << "},\n";

    appendIndent(output, indent);
    output << "\"regions\": [\n";
    for (std::size_t index = 0U; index < node.eventStructureCases.size(); ++index) {
        const auto &eventCase = node.eventStructureCases[index];
        appendIndent(output, indent + 1);
        output << "{\n";
        appendIndent(output, indent + 2);
        output << "\"id\": "; appendJsonString(output, eventCase.id); output << ",\n";
        appendIndent(output, indent + 2);
        output << "\"event_label\": "; appendJsonString(output, eventCase.label); output << ",\n";
        appendIndent(output, indent + 2);
        output << "\"subdiagram_label_text\": ";
        appendJsonString(output, eventCase.subdiagramLabelText); output << ",\n";
        if (eventCase.eventDataNodeNormalizedY) {
            appendIndent(output, indent + 2);
            output << "\"studio_event_data_node_normalized_y\": "
                   << clampEventStructureDataNodeNormalizedY(*eventCase.eventDataNodeNormalizedY) << ",\n";
        }
        if (!eventCase.eventDataNodeVisible) {
            appendIndent(output, indent + 2);
            output << "\"studio_event_data_node_visible\": false,\n";
        }
        appendIndent(output, indent + 2);
        output << "\"event\": { \"kind\": ";
        // Multiple specifiers and source-specific editor events are authoring
        // extensions of "user", not new public executable descriptor kinds.
        const bool multiple = eventCase.eventBindings.size() > 1;
        const auto kind = eventCase.eventBindings.empty() ? eventCase.eventKind : eventCase.eventBindings.front().kind;
        const auto source = eventCase.eventBindings.empty() ? eventCase.eventSource : eventCase.eventBindings.front().source;
        const bool studioOnly = multiple || (!eventCase.eventBindings.empty() &&
            (!isPublicEventDescriptorKind(kind) ||
             (eventBindingSourceScope(eventCase.eventBindings.front()) != EventSourceScope::Application &&
              eventBindingSourceScope(eventCase.eventBindings.front()) != EventSourceScope::Control)));
        appendJsonString(output, studioOnly || kind.empty()
            ? std::string_view{ "user" }
            : std::string_view{ kind });
        if (!studioOnly && !source.empty()) {
            output << ", \"source\": ";
            appendJsonString(output, source);
        }
        if (!eventCase.eventBindings.empty()) {
            output << ", \"studio_bindings\": [";
            for (std::size_t binding = 0; binding < eventCase.eventBindings.size(); ++binding) {
                if (binding) output << ", ";
                const auto &value = eventCase.eventBindings[binding];
                output << "{ \"kind\": "; appendJsonString(output, value.kind);
                output << ", \"source\": "; appendJsonString(output, value.source);
                if (!value.sourceScope.empty()) {
                    output << ", \"source_scope\": "; appendJsonString(output, value.sourceScope);
                }
                output << " }";
            }
            output << "]";
        }
        output << " },\n";
        appendIndent(output, indent + 2);
        output << "\"event_data_fields\": [";
        for (std::size_t field = 0U; field < eventCase.eventDataFields.size(); ++field) {
            if (field > 0U) output << ", ";
            appendJsonString(output, eventCase.eventDataFields[field]);
        }
        output << "],\n";
        if (eventCase.eventResponseFields) {
            appendIndent(output, indent + 2);
            output << "\"studio_event_response_fields\": [";
            for (std::size_t field = 0; field < eventCase.eventResponseFields->size(); ++field) {
                if (field) output << ", ";
                appendJsonString(output, (*eventCase.eventResponseFields)[field]);
            }
            output << "],\n";
        }
        if (index < caseDiagrams.size()) output << caseDiagrams[index];
        appendIndent(output, indent + 1);
        output << '}' << (index + 1U < node.eventStructureCases.size() ? "," : "")
               << '\n';
    }
    appendIndent(output, indent);
    output << "],\n";
    return output.str();
}

} // namespace frog::native_engine::document_internal
