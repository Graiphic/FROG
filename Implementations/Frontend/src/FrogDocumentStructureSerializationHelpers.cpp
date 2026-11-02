#include "FrogDocumentStructureSerializationHelpers.hpp"

#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/StructureTunnelMode.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <iomanip>
#include <stdexcept>

namespace frog::native_engine::document_internal {

void appendIndent(std::ostringstream &output, int level) {
    for (int index = 0; index < level; ++index) {
        output << "  ";
    }
}

void appendJsonString(
    std::ostringstream &output,
    std::string_view value) {
    output << '"';
    // Emit ordinary UTF-8/base64 spans together. Escapes retain the exact
    // historical spelling; no bytes are decoded, normalized or cached.
    std::size_t runBegin = 0;
    for (std::size_t index = 0; index < value.size(); ++index) {
        const auto ch = static_cast<unsigned char>(value[index]);
        if (ch >= 0x20U && ch != '"' && ch != '\\') continue;
        if (index != runBegin)
            output.write(value.data() + runBegin, static_cast<std::streamsize>(index - runBegin));
        switch (ch) {
        case '"': output << "\\\""; break;
        case '\\': output << "\\\\"; break;
        case '\b': output << "\\b"; break;
        case '\f': output << "\\f"; break;
        case '\n': output << "\\n"; break;
        case '\r': output << "\\r"; break;
        case '\t': output << "\\t"; break;
        default:
            if (ch < 0x20U) {
                output << "\\u" << std::hex << std::setw(4)
                       << std::setfill('0') << static_cast<unsigned int>(ch)
                       << std::dec << std::setfill(' ');
            }
            break;
        }
        runBegin = index + 1U;
    }
    if (runBegin != value.size())
        output.write(value.data() + runBegin, static_cast<std::streamsize>(value.size() - runBegin));
    output << '"';
}

void appendColorProperty(
    std::ostringstream &output,
    int indent,
    std::string_view name,
    const std::optional<std::uint32_t> &rgb,
    std::string_view opacityName,
    int opacityPercent,
    bool clampOpacity) {
    if (!rgb.has_value()) {
        return;
    }
    appendIndent(output, indent);
    output << '"' << name << "\": " << *rgb << ",\n";
    appendIndent(output, indent);
    output << '"' << opacityName << "\": "
           << (clampOpacity ? (std::clamp)(opacityPercent, 0, 100)
                            : opacityPercent)
           << ",\n";
}

void appendSubdiagramLabelFont(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent) {
    appendIndent(output, indent);
    output << "\"label_font\": {\n";
    appendIndent(output, indent + 1);
    output << "\"family\": ";
    appendJsonString(output, node.labelFontFamily);
    output << ",\n";
    appendIndent(output, indent + 1);
    output << "\"style\": ";
    appendJsonString(output, node.labelTextStyle);
    output << ",\n";
    appendIndent(output, indent + 1);
    output << "\"use_default_size\": " << (node.labelFontPointSizeUsesDefault ? "true" : "false");
    output << ",\n";
    appendIndent(output, indent + 1);
    output << "\"point_size\": " << node.labelFontPointSize;
    if (node.labelTextRgb.has_value()) {
        output << ",\n";
        appendIndent(output, indent + 1);
        output << "\"text_rgb\": " << node.labelTextRgb.value();
    }
    if (node.labelBackgroundRgb.has_value()) {
        output << ",\n";
        appendIndent(output, indent + 1);
        output << "\"background_rgb\": " << node.labelBackgroundRgb.value();
    }
    output << '\n';
    appendIndent(output, indent);
    output << "},\n";
}

void appendStructureTunnels(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent) {
    appendStructureTunnelPlacements(output, node, indent);
    appendStructureBoundary(output, node, indent);
}

void appendSubdiagramLabelAppearance(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent) {
    appendIndent(output, indent);
    output << "\"structure_subdiagram_label_visible\": "
           << (node.structureSubdiagramLabelVisible ? "true" : "false") << ",\n";
    appendIndent(output, indent);
    output << "\"structure_subdiagram_label_height\": "
           << clampStructureSubdiagramLabelHeight(node.structureSubdiagramLabelHeight) << ",\n";
    if (node.structureSubdiagramLabelBorderRgb) {
        appendIndent(output, indent);
        output << "\"structure_subdiagram_label_border_rgb\": "
               << *node.structureSubdiagramLabelBorderRgb << ",\n";
    }
    appendSubdiagramLabelFont(output, node, indent);
}

void appendStructureTunnelPlacements(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent) {
    appendIndent(output, indent);
    output << "\"structure_tunnels\": [";
    if (!node.caseStructureTunnels.empty()) {
        output << '\n';
    }
    for (std::size_t index = 0U;
         index < node.caseStructureTunnels.size();
         ++index) {
        const auto &tunnel = node.caseStructureTunnels[index];
        appendIndent(output, indent + 1);
        output << "{ \"id\": ";
        appendJsonString(output, tunnel.id);
        output << ", \"value_type\": ";
        appendJsonString(output, tunnel.valueType);
        output << ", \"side\": ";
        appendJsonString(output, normalizeCaseStructureTunnelSide(tunnel.side));
        // Pointer placement produces ratios of actual boundary pixels. Keep
        // the complete authored position through Save/Read instead of the
        // stream's default six significant digits.
        std::array<char,64> position{};
        const auto encoded=std::to_chars(position.data(),position.data()+position.size(),
            clampCaseStructureTunnelNormalizedPosition(tunnel.normalizedPosition));
        if(encoded.ec!=std::errc{})throw std::runtime_error("Cannot serialize tunnel position.");
        output << ", \"normalized_position\": "
               << std::string_view(position.data(),static_cast<std::size_t>(encoded.ptr-position.data()))
               << ", \"output_from_structure\": "
               << (tunnel.outputFromStructure ? "true" : "false");
        if (!tunnel.linkedInputTunnelId.empty()) {
            output << ", \"linked_input_tunnel_id\": ";
            appendJsonString(output, tunnel.linkedInputTunnelId);
        }
        if (!tunnel.shiftRegisterPairId.empty()) {
            output << ", \"shift_register_pair_id\": ";
            appendJsonString(output, tunnel.shiftRegisterPairId);
        }
        if (tunnel.shiftRegisterHistoryIndex != 0)
            output << ", \"shift_register_history_index\": " << tunnel.shiftRegisterHistoryIndex;
        if (tunnel.useDefaultIfUnwired) {
            output << ", \"use_default_if_unwired\": true";
        }
        if (tunnel.mode != DiagramTunnelMode::LastValue) {
            output << ", \"tunnel_mode\": ";
            appendJsonString(output, diagramTunnelModeName(tunnel.mode));
        }
        if (tunnel.conditional) output << ", \"conditional\": true";
        if(tunnel.indexingPolicy!=DiagramTunnelIndexingPolicy::LeadingDimension) {output << ", \"indexing_policy\": ";appendJsonString(output,diagramTunnelIndexingPolicyName(tunnel.indexingPolicy));}
        if (!tunnel.sequenceFrameId.empty()) {
            output << ", \"studio_sequence_frame\": ";
            appendJsonString(output, tunnel.sequenceFrameId);
        }
        if (!tunnel.sequenceOuterFrameId.empty()) {
            output << ", \"studio_sequence_outer_frame\": ";
            appendJsonString(output, tunnel.sequenceOuterFrameId);
        }
        if (!tunnel.sequenceLegacyFrameId.empty()) {
            output << ", \"studio_sequence_legacy_frame\": ";
            appendJsonString(output, tunnel.sequenceLegacyFrameId);
        }
        if (!tunnel.sequenceLegacyEdgeId.empty()) {
            output << ", \"studio_sequence_legacy_edge\": ";
            appendJsonString(output, tunnel.sequenceLegacyEdgeId);
        }
        output << " }"
               << (index + 1U < node.caseStructureTunnels.size() ? "," : "")
               << '\n';
    }
    if (!node.caseStructureTunnels.empty()) {
        appendIndent(output, indent);
    }
    output << "],\n";
}

void appendStructureBoundary(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent) {
    appendIndent(output, indent);
    output << "\"boundary\": {\n";
    const auto appendBoundary = [&](std::string_view name,
                                    bool outputBoundary) {
        appendIndent(output, indent + 1);
        output << '"' << name << "\": [";
        bool first = true;
        for (const auto &tunnel : node.caseStructureTunnels) {
            if (tunnel.outputFromStructure != outputBoundary) {
                continue;
            }
            output << (first ? "\n" : ",\n");
            appendIndent(output, indent + 2);
            output << "{ \"id\": ";
            appendJsonString(output, tunnel.id);
            output << ", \"type\": ";
            appendJsonString(output, tunnel.valueType);
            if (!tunnel.shiftRegisterPairId.empty()) {
                output << ", \"shift_register_pair_id\": ";
                appendJsonString(output, tunnel.shiftRegisterPairId);
            }
            if (tunnel.shiftRegisterHistoryIndex != 0)
                output << ", \"shift_register_history_index\": " << tunnel.shiftRegisterHistoryIndex;
            if (tunnel.outputFromStructure && tunnel.useDefaultIfUnwired) {
                output << ", \"use_default_if_unwired\": true";
            }
            if (tunnel.mode != DiagramTunnelMode::LastValue) {
                output << ", \"tunnel_mode\": ";
                appendJsonString(output, diagramTunnelModeName(tunnel.mode));
            }
            if (tunnel.conditional) output << ", \"conditional\": true";
            if(tunnel.indexingPolicy!=DiagramTunnelIndexingPolicy::LeadingDimension) {output << ", \"indexing_policy\": ";appendJsonString(output,diagramTunnelIndexingPolicyName(tunnel.indexingPolicy));}
            output << " }";
            first = false;
        }
        if (!first) {
            output << '\n';
            appendIndent(output, indent + 1);
        }
        output << ']' << (outputBoundary ? '\n' : ',');
        if (!outputBoundary) {
            output << '\n';
        }
    };
    appendBoundary("inputs", false);
    appendBoundary("outputs", true);
    appendIndent(output, indent);
    output << "},\n";
}

} // namespace frog::native_engine::document_internal
