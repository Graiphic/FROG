#include "FrogDocumentDisabledStructureSerialization.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"

#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/DisabledStructureData.hpp"

#include <sstream>

namespace frog::native_engine::document_internal {

std::string serializeDisabledStructureProperties(
    const DiagramNode &node,
    const std::function<std::string(std::string_view)> &serializeRegion,
    int indent) {
    std::ostringstream output;
    appendSubdiagramLabelAppearance(output, node, indent);
    appendStructureTunnels(output, node, indent);
    appendIndent(output, indent);
    output << "\"structure_terminals\": {},\n";
    appendIndent(output, indent);
    output << "\"regions\": [\n";
    const auto appendRegion = [&](std::string_view id, const DiagramDisabledRegion *region,
                                  std::string_view activation,
                                  bool commaAfter) {
        appendIndent(output, indent + 1);
        output << "{\n";
        appendIndent(output, indent + 2);
        output << "\"id\": "; appendJsonString(output, id); output << ",\n";
        appendIndent(output, indent + 2);
        output << "\"activation\": ";
        appendJsonString(output, activation);
        output << ",\n";
        if (!node.disabledStructureRegions.empty()) {
            appendIndent(output, indent + 2);
            if (region) {
                output << "\"studio_region_id\": "; appendJsonString(output, region->id); output << ",\n";
            } else output << "\"studio_placeholder\": true,\n";
        }
        if (region) output << serializeRegion(region->id);
        else { appendIndent(output, indent + 2); output << "\"diagram\": {\"nodes\": [], \"edges\": []}\n"; }
        appendIndent(output, indent + 1);
        output << '}' << (commaAfter ? "," : "") << '\n';
    };
    const auto regions = disabledStructureRegions(node);
    const DiagramDisabledRegion *enabled = nullptr, *disabled = nullptr;
    for (const auto &region : regions) {
        if (region.enabled && !enabled) enabled = &region;
        if (!region.enabled && !disabled) disabled = &region;
    }
    // The public profile requires precisely these two buckets. Extra authoring
    // regions live in an extension, never duplicated (including nested bodies).
    appendRegion(kDisabledStructureEnabledRegionId, enabled, "enabled", true);
    appendRegion(kDisabledStructureDisabledRegionId, disabled, "disabled", false);
    appendIndent(output, indent);
    output << "],\n";
    if (!node.disabledStructureRegions.empty()) {
        appendIndent(output, indent); output << "\"studio_disabled_regions\": [";
        for (std::size_t i = 0; i < regions.size(); ++i) {
            if (i) output << ',';
            output << "{\"id\":"; appendJsonString(output, regions[i].id);
            output << ",\"enabled\":" << (regions[i].enabled ? "true" : "false");
            output << ",\"subdiagram_label_text\":";
            appendJsonString(output, regions[i].subdiagramLabelText); output << '}';
        }
        output << "],\n";
        appendIndent(output, indent); output << "\"studio_disabled_extra_regions\": [\n";
        bool first = true;
        for (const auto &region : regions) if (&region != enabled && &region != disabled) {
            if (!first) output << ",\n";
            appendRegion(region.id, &region, "disabled", false); first = false;
        }
        appendIndent(output, indent); output << "],\n";
    }
    if (!node.disabledStructureVisibleRegionId.empty()) {
        appendIndent(output, indent); output << "\"disabled_structure_visible_region_id\": ";
        appendJsonString(output, disabledStructureVisibleRegionId(node)); output << ",\n";
    }
    appendIndent(output, indent);
    output << "\"disabled_structure_enabled_visible\": "
           << (disabledStructureVisibleRegionEnabled(node) ? "true" : "false") << ",\n";
    appendColorProperty(output, indent, "disabled_structure_background_rgb",
        node.disabledStructureBackgroundRgb,
        "disabled_structure_background_opacity_percent",
        node.disabledStructureBackgroundOpacityPercent,
        true);
    appendColorProperty(output, indent, "disabled_structure_subframe_rgb",
        node.disabledStructureSubframeRgb,
        "disabled_structure_subframe_opacity_percent",
        node.disabledStructureSubframeOpacityPercent,
        true);
    appendColorProperty(output, indent, "disabled_structure_veil_rgb",
        node.disabledStructureVeilRgb,
        "disabled_structure_veil_opacity_percent",
        node.disabledStructureVeilOpacityPercent,
        true);
    return output.str();
}

} // namespace frog::native_engine::document_internal
