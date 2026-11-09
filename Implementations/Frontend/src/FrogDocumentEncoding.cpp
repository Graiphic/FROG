#include "FrogDocumentEncoding.hpp"

#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace frog::native_engine::document_internal {

std::string formatNumber(double value) {
    std::ostringstream output;
    output << std::fixed << std::setprecision(3) << value;
    auto text = output.str();
    while (text.size() > 1 && text.back() == '0') {
        text.pop_back();
    }
    if (!text.empty() && text.back() == '.') {
        text.pop_back();
    }
    return text.empty() ? std::string{ "0" } : text;
}
std::string normalizedInterfaceConnectionRequirement(std::string_view value) {
    if (value == "required" || value == "optional" || value == "recommended") {
        return std::string(value);
    }
    return "recommended";
}

std::string base64Encode(const std::vector<std::uint8_t> &bytes) {
    static constexpr char kTable[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string output;
    output.reserve(((bytes.size() + 2U) / 3U) * 4U);

    std::size_t index = 0;
    while (index < bytes.size()) {
        const std::uint32_t a = bytes[index++];
        const std::uint32_t b = index < bytes.size() ? bytes[index++] : 0U;
        const std::uint32_t c = index < bytes.size() ? bytes[index++] : 0U;
        const std::uint32_t triple = (a << 16U) | (b << 8U) | c;

        output.push_back(kTable[(triple >> 18U) & 0x3FU]);
        output.push_back(kTable[(triple >> 12U) & 0x3FU]);
        output.push_back(index - 1U <= bytes.size() ? kTable[(triple >> 6U) & 0x3FU] : '=');
        output.push_back(index <= bytes.size() ? kTable[triple & 0x3FU] : '=');
    }

    const auto remainder = bytes.size() % 3U;
    if (remainder == 1U) {
        output[output.size() - 2U] = '=';
        output[output.size() - 1U] = '=';
    } else if (remainder == 2U) {
        output[output.size() - 1U] = '=';
    }
    return output;
}

namespace {

int base64Value(char ch) noexcept {
    if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A';
    }
    if (ch >= 'a' && ch <= 'z') {
        return 26 + (ch - 'a');
    }
    if (ch >= '0' && ch <= '9') {
        return 52 + (ch - '0');
    }
    if (ch == '+') {
        return 62;
    }
    if (ch == '/') {
        return 63;
    }
    return -1;
}

} // namespace

std::vector<std::uint8_t> base64Decode(std::string_view text) {
    std::vector<std::uint8_t> output;
    int value = 0;
    int bits = -8;
    for (const char ch : text) {
        if (ch == '=') {
            break;
        }
        if (std::isspace(static_cast<unsigned char>(ch)) != 0) {
            continue;
        }
        const int decoded = base64Value(ch);
        if (decoded < 0) {
            return {};
        }
        value = (value << 6) + decoded;
        bits += 6;
        if (bits >= 0) {
            output.push_back(static_cast<std::uint8_t>((value >> bits) & 0xFF));
            bits -= 8;
        }
    }
    return output;
}

void canonicalizeFunctionEndpoint(
    std::span<const DiagramNode> nodes,
    DiagramEdgeEndpoint &endpoint) {
    const auto node = std::find_if(
        nodes.begin(),
        nodes.end(),
        [&](const DiagramNode &candidate) {
            return candidate.id == endpoint.nodeId;
        });
    if (node == nodes.end() || node->kind != "primitive") {
        return;
    }
    if (const auto *entry = findFunctionCatalogEntryByDiagramNodeType(node->type);
        entry != nullptr) {
        endpoint.portId = std::string(canonicalFunctionBindingPortId(
            entry->catalogId,
            endpoint.portId));
    }
}

void canonicalizeFunctionEdge(
    std::span<const DiagramNode> nodes,
    DiagramEdge &edge) {
    canonicalizeFunctionEndpoint(nodes, edge.from);
    canonicalizeFunctionEndpoint(nodes, edge.to);
}

} // namespace frog::native_engine::document_internal
