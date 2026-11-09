#include "FrogDocumentIdeSerialization.hpp"
#include "FrogDocumentJsonWriter.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace frog::native_engine::document_internal {

bool hasIdeFrontPanelPreferences(FrogWidgetDataView canvas) noexcept {
    const auto viewportOrigin = canvas.ideFrontPanelViewportOrigin();
    return canvas.ideFrontPanelBackgroundRgb().has_value() ||
        canvas.ideFrontPanelGridRgb().has_value() ||
        !canvas.ideFrontPanelGridVisible() ||
        !canvas.ideFrontPanelSnapToGrid() ||
        canvas.ideFrontPanelZoomPercent() != 100 ||
        std::abs(viewportOrigin.x) >= 0.001 ||
        std::abs(viewportOrigin.y) >= 0.001;
}

bool hasIdeDiagramPreferences(FrogWidgetDataView canvas) noexcept {
    return canvas.ideDiagramBackgroundRgb().has_value() ||
        canvas.ideDiagramGridRgb().has_value();
}

bool hasIdePreferences(FrogWidgetDataView canvas) noexcept {
    return hasIdeFrontPanelPreferences(canvas) || hasIdeDiagramPreferences(canvas);
}

void appendEditorWindows(std::ostringstream &output, const FrogEditorWindows &windows, int indent) {
    appendIndent(output, indent);
    output << "\"windows\": {\n";
    appendIndent(output, indent + 1);
    output << "\"coordinate_space\": \"screen_pixels\"";
    const auto appendWindow = [&](std::string_view name,
                                  const std::optional<FrogEditorWindowPlacement> &placement) {
        if (!placement || !validFrogEditorWindowPlacement(*placement)) return;
        output << ",\n";
        appendIndent(output, indent + 1);
        appendJsonString(output, name);
        output << ": {\n";
        appendBounds(output, placement->bounds, indent + 2);
        appendJsonBooleanField(output, indent + 2, "maximized", placement->maximized, false);
        appendIndent(output, indent + 1);
        output << '}';
    };
    appendWindow("front_panel", windows.frontPanel);
    appendWindow("diagram", windows.diagram);
    appendWindow("frog_source", windows.frogSource);
    output << '\n';
    appendIndent(output, indent);
    output << "},\n";
}

void appendIdePreferences(std::ostringstream &output, FrogWidgetDataView canvas,
    const FrogEditorWindows &windows, int indent, bool comma) {
    appendIndent(output, indent);
    output << "\"ide\": {\n";
    if (windows.frontPanel || windows.diagram || windows.frogSource) {
        appendEditorWindows(output, windows, indent + 1);
    }
    appendIndent(output, indent + 1);
    output << "\"front_panel\": {\n";

    const bool hasBackground = canvas.ideFrontPanelBackgroundRgb().has_value();
    const bool hasGrid = canvas.ideFrontPanelGridRgb().has_value();
    const bool hasGridVisibilityOverride = !canvas.ideFrontPanelGridVisible();
    const bool hasSnapOverride = !canvas.ideFrontPanelSnapToGrid();
    const bool hasZoomOverride = canvas.ideFrontPanelZoomPercent() != 100;
    const auto viewportOrigin = canvas.ideFrontPanelViewportOrigin();
    const bool hasViewportOrigin =
        std::abs(viewportOrigin.x) >= 0.001 ||
        std::abs(viewportOrigin.y) >= 0.001;
    if (hasBackground) {
        const int opacity = std::clamp(canvas.ideFrontPanelBackgroundOpacityPercent(), 0, 100);
        const std::string background = opacity == 0
            ? std::string{ "transparent" }
            : formatRgbHex(canvas.ideFrontPanelBackgroundRgb().value_or(0x000000U));
        appendJsonField(
            output,
            indent + 2,
            "background_color",
            background,
            opacity != 100 || hasGrid || hasGridVisibilityOverride || hasSnapOverride || hasZoomOverride || hasViewportOrigin);
        if (opacity != 100) {
            appendJsonIntegerField(
                output,
                indent + 2,
                "background_opacity",
                static_cast<std::uint32_t>(opacity),
                hasGrid || hasGridVisibilityOverride || hasSnapOverride || hasZoomOverride || hasViewportOrigin);
        }
    }
    if (hasGrid) {
        const int opacity = std::clamp(canvas.ideFrontPanelGridOpacityPercent(), 0, 100);
        const std::string grid = opacity == 0
            ? std::string{ "transparent" }
            : formatRgbHex(canvas.ideFrontPanelGridRgb().value_or(0x000000U));
        appendJsonField(
            output,
            indent + 2,
            "grid_color",
            grid,
            opacity != 100 || hasGridVisibilityOverride || hasSnapOverride || hasZoomOverride || hasViewportOrigin);
        if (opacity != 100) {
            appendJsonIntegerField(
                output,
                indent + 2,
                "grid_opacity",
                static_cast<std::uint32_t>(opacity),
                hasGridVisibilityOverride || hasSnapOverride || hasZoomOverride || hasViewportOrigin);
        }
    }
    if (hasGridVisibilityOverride) {
        appendJsonBooleanField(
            output,
            indent + 2,
            "grid_visible",
            canvas.ideFrontPanelGridVisible(),
            hasSnapOverride || hasZoomOverride || hasViewportOrigin);
    }
    if (hasSnapOverride) {
        appendJsonBooleanField(
            output,
            indent + 2,
            "snap_to_grid",
            canvas.ideFrontPanelSnapToGrid(),
            hasZoomOverride || hasViewportOrigin);
    }
    if (hasZoomOverride) {
        appendJsonIntegerField(
            output,
            indent + 2,
            "zoom_percent",
            static_cast<std::uint32_t>(canvas.ideFrontPanelZoomPercent()),
            hasViewportOrigin);
    }
    if (hasViewportOrigin) {
        appendIndent(output, indent + 2);
        output << "\"viewport_origin\": {\n";
        appendJsonNumberField(output, indent + 3, "x", viewportOrigin.x);
        appendJsonNumberField(output, indent + 3, "y", viewportOrigin.y, false);
        appendIndent(output, indent + 2);
        output << "}\n";
    }

    appendIndent(output, indent + 1);
    output << '}';
    if (hasIdeDiagramPreferences(canvas)) {
        output << ",\n";
        appendIndent(output, indent + 1);
        output << "\"diagram\": {\n";

        const bool hasDiagramBackground = canvas.ideDiagramBackgroundRgb().has_value();
        const bool hasDiagramGrid = canvas.ideDiagramGridRgb().has_value();
        if (hasDiagramBackground) {
            const int opacity = std::clamp(canvas.ideDiagramBackgroundOpacityPercent(), 0, 100);
            const std::string background = opacity == 0
                ? std::string{ "transparent" }
                : formatRgbHex(canvas.ideDiagramBackgroundRgb().value_or(0x000000U));
            appendJsonField(
                output,
                indent + 2,
                "background_color",
                background,
                opacity != 100 || hasDiagramGrid);
            if (opacity != 100) {
                appendJsonIntegerField(
                    output,
                    indent + 2,
                    "background_opacity",
                    static_cast<std::uint32_t>(opacity),
                    hasDiagramGrid);
            }
        }
        if (hasDiagramGrid) {
            const int opacity = std::clamp(canvas.ideDiagramGridOpacityPercent(), 0, 100);
            const std::string grid = opacity == 0
                ? std::string{ "transparent" }
                : formatRgbHex(canvas.ideDiagramGridRgb().value_or(0x000000U));
            appendJsonField(
                output,
                indent + 2,
                "grid_color",
                grid,
                opacity != 100);
            if (opacity != 100) {
                appendJsonIntegerField(
                    output,
                    indent + 2,
                    "grid_opacity",
                    static_cast<std::uint32_t>(opacity),
                    false);
            }
        }

        appendIndent(output, indent + 1);
        output << "}\n";
    } else {
        output << '\n';
    }
    appendIndent(output, indent);
    output << '}';
    if (comma) {
        output << ',';
    }
    output << '\n';
}

} // namespace frog::native_engine::document_internal
