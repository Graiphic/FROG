#include "FrogDocumentStaticVisualSerialization.hpp"
#include "FrogDocumentJsonWriter.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"
#include "FrogDocumentEncoding.hpp"
#include "frog_native_engine/FrogWidgetData.hpp"
#include "frog_native_engine/WidgetPlacementContract.hpp"

#include <cstdint>
#include <sstream>
#include <vector>

namespace frog::native_engine::document_internal {

std::string staticVisualMediaType(const StaticVisual &visual) {
    if (!visual.mediaType.empty()) {
        return visual.mediaType;
    }
    return visual.kind == StaticVisualKind::Svg ? "image/svg+xml" : "image/dib";
}

namespace {

std::string staticVisualMediaRef(const StaticVisual &visual) {
    if (!visual.mediaRef.empty()) {
        return visual.mediaRef;
    }
    if (visual.kind == StaticVisualKind::Svg && !visual.svgText.empty()) {
        const std::vector<std::uint8_t> bytes(visual.svgText.begin(), visual.svgText.end());
        return "data:image/svg+xml;base64," + base64Encode(bytes);
    }
    if (visual.kind == StaticVisualKind::Dib && !visual.dibBytes.empty()) {
        return "data:image/dib;base64," + base64Encode(visual.dibBytes);
    }
    return {};
}

void appendLayout(std::ostringstream &output, const Rect &bounds, int indent, bool comma = true) {
    appendIndent(output, indent);
    output << "\"layout\": {\n";
    appendJsonNumberField(output, indent + 1, "x", bounds.x);
    appendJsonNumberField(output, indent + 1, "y", bounds.y);
    appendJsonNumberField(output, indent + 1, "width", bounds.width);
    appendJsonNumberField(output, indent + 1, "height", bounds.height);
    appendJsonField(output, indent + 1, "origin", "placement_bounds.top_left");
    appendJsonField(output, indent + 1, "bounds_ref", "placement_bounds", false);
    appendIndent(output, indent);
    output << '}';
    if (comma) {
        output << ',';
    }
    output << '\n';
}

} // namespace

void appendStaticVisual(
    std::ostringstream &output,
    const SceneObject &object,
    const StaticVisual &visual,
    int indent,
    bool comma) {
    appendIndent(output, indent);
    output << "{\n";
    appendJsonField(output, indent + 1, "id", object.id);
    if (object.locked) {
        appendJsonBooleanField(output, indent + 1, "locked", object.locked);
    }
    if (object.hidden) {
        appendJsonBooleanField(output, indent + 1, "hidden", object.hidden);
    }
    if (!object.parentTabId.empty() || !object.parentTabPageId.empty()) {
        appendJsonField(output, indent + 1, "studio_tab_parent_id", object.parentTabId);
        appendJsonField(output, indent + 1, "studio_tab_page_id", object.parentTabPageId);
    }
    appendJsonField(output, indent + 1, "type", "staticVisual");
    appendJsonField(output, indent + 1, "class_ref", "frog.widgets.image_static");
    appendJsonField(output, indent + 1, "role", "support");
    appendBounds(output, object.bounds, indent + 1);
    appendLayout(output, object.bounds, indent + 1);

    appendIndent(output, indent + 1);
    output << "\"props\": {\n";
    appendJsonField(output, indent + 2, "label.text", visual.sourceName);
    appendJsonBooleanField(output, indent + 2, "label.visible", false);
    appendJsonField(output, indent + 2, "media_ref", staticVisualMediaRef(visual));
    appendJsonField(output, indent + 2, "media_type", staticVisualMediaType(visual));
    appendJsonField(output, indent + 2, "media.original_format", visual.kind == StaticVisualKind::Svg ? "svg" : "dib");
    appendJsonNumberField(output, indent + 2, "media.intrinsic_width", visual.naturalWidth);
    appendJsonNumberField(output, indent + 2, "media.intrinsic_height", visual.naturalHeight);
    appendJsonField(output, indent + 2, "media.fit", "contain");
    appendJsonBooleanField(output, indent + 2, "media.preserve_aspect_ratio", visual.preserveAspectRatio);
    appendJsonNumberField(output, indent + 2, "transform.rotation_quarter_turns", visual.rotationQuarterTurns);
    appendJsonBooleanField(output, indent + 2, "transform.flip_horizontal", visual.flipHorizontal);
    appendJsonBooleanField(output, indent + 2, "transform.flip_vertical", visual.flipVertical);
    appendJsonNumberField(
        output,
        indent + 2,
        "layout.aura_band_px",
        kDefaultPlacementAuraBandPx);
    appendJsonBooleanField(output, indent + 2, "interaction.visible", true);
    appendJsonBooleanField(output, indent + 2, "interaction.enabled", false);
    appendJsonBooleanField(output, indent + 2, "interaction.read_only", true);
    appendJsonField(output, indent + 2, "realization.variant", "rectangular", false);
    appendIndent(output, indent + 1);
    output << "},\n";
    appendIndent(output, indent + 1);
    output << "\"visual\": {\n";
    appendJsonField(output, indent + 2, "asset_ref", "asset:image_static_rectangular_svg", false);
    appendIndent(output, indent + 1);
    output << "},\n";
    appendIndent(output, indent + 1);
    output << "\"studio\": {\n";
    appendIndent(output, indent + 2);
    output << "\"staticVisual\": {\n";
    appendJsonField(output, indent + 3, "kind", visual.kind == StaticVisualKind::Dib ? "dib" : "svg");
    appendJsonField(output, indent + 3, "sourceName", visual.sourceName);
    appendJsonNumberField(output, indent + 3, "naturalWidth", visual.naturalWidth);
    appendJsonNumberField(output, indent + 3, "naturalHeight", visual.naturalHeight);
    appendJsonBooleanField(output, indent + 3, "preserveAspectRatio", visual.preserveAspectRatio);
    appendJsonNumberField(output, indent + 3, "rotationQuarterTurns", visual.rotationQuarterTurns);
    appendJsonBooleanField(output, indent + 3, "flipHorizontal", visual.flipHorizontal);
    appendJsonBooleanField(output, indent + 3, "flipVertical", visual.flipVertical);
    if (visual.kind == StaticVisualKind::Svg) {
        appendJsonField(output, indent + 3, "svgText", visual.svgText, false);
    } else {
        appendJsonField(output, indent + 3, "dibBase64",
            staticVisualUsesEncodedRasterSource(visual) ? std::string{} : base64Encode(visual.dibBytes), false);
    }
    appendIndent(output, indent + 2);
    output << "}\n";
    appendIndent(output, indent + 1);
    output << "}\n";
    appendIndent(output, indent);
    output << '}';
    if (comma) {
        output << ',';
    }
    output << '\n';
}

} // namespace frog::native_engine::document_internal
