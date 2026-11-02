#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

struct DiagramEdge;
struct DiagramEdgeEndpoint;
struct DiagramNode;

}

namespace frog::native_engine::document_internal {

// Shared private-draft textual conventions used by both reader and writer.
std::string formatNumber(double value);

std::string normalizedInterfaceConnectionRequirement(std::string_view value);

std::string base64Encode(const std::vector<std::uint8_t> &bytes);

std::vector<std::uint8_t> base64Decode(std::string_view text);

void canonicalizeFunctionEndpoint(
    std::span<const DiagramNode> nodes,
    DiagramEdgeEndpoint &endpoint);

void canonicalizeFunctionEdge(
    std::span<const DiagramNode> nodes,
    DiagramEdge &edge);

} // namespace frog::native_engine::document_internal
