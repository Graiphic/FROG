#pragma once

#include <cstdint>
#include <optional>
#include <sstream>
#include <string_view>

namespace frog::native_engine {

struct DiagramNode;

namespace document_internal {

void appendIndent(std::ostringstream &output, int level);

void appendJsonString(
    std::ostringstream &output,
    std::string_view value);

void appendColorProperty(
    std::ostringstream &output,
    int indent,
    std::string_view name,
    const std::optional<std::uint32_t> &rgb,
    std::string_view opacityName,
    int opacityPercent,
    bool clampOpacity = false);

void appendSubdiagramLabelFont(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent);

void appendStructureTunnels(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent);

void appendSubdiagramLabelAppearance(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent);

void appendStructureTunnelPlacements(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent);

void appendStructureBoundary(
    std::ostringstream &output,
    const DiagramNode &node,
    int indent);

} // namespace document_internal
} // namespace frog::native_engine
