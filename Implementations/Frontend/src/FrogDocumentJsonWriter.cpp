#include "FrogDocumentJsonWriter.hpp"
#include "FrogDocumentStructureSerializationHelpers.hpp"
#include "frog_native_engine/WidgetInstance.hpp"
#include <array>
#include <charconv>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace frog::native_engine::document_internal {

void appendJsonValue(std::ostringstream &output, const source::JsonValue &value) {
    using Json = source::JsonValue;
    switch (value.type) {
    case Json::Type::Null: output << "null"; break;
    case Json::Type::Boolean: output << (value.boolean ? "true" : "false"); break;
    case Json::Type::Number: {
        if (!value.string.empty()) output << value.string;
        else { std::array<char, 64> buffer{}; const auto encoded = std::to_chars(buffer.data(), buffer.data()+buffer.size(), value.number);
            if (encoded.ec != std::errc{}) throw std::runtime_error("Unable to encode private JSON number.");
            output.write(buffer.data(), encoded.ptr-buffer.data()); }
        break;
    }
    case Json::Type::String: appendJsonString(output, value.string); break;
    case Json::Type::Array: {
        output << '['; bool first = true;
        for (const auto &child : value.array) { if (!first) output << ','; first = false; appendJsonValue(output, child); }
        output << ']'; break;
    }
    case Json::Type::Object: {
        output << '{'; bool first = true;
        for (const auto &[key, child] : value.object) {
            if (!first) output << ','; first = false;
            appendJsonString(output, key); output << ':'; appendJsonValue(output, child);
        }
        output << '}'; break;
    }
    }
}

void appendPrivateInterfaceMap(std::ostringstream &output, int indent, std::string_view interfaceBindingMapProfileId,
                        std::string_view interfaceMapLayoutPatternId, const std::vector<std::string> &interfaceMapVisualTransforms) {
    appendIndent(output, indent);
    output << "\"interface_map\": {\n";
    const bool hasProfile = !interfaceBindingMapProfileId.empty();
    const bool hasPattern = !interfaceMapLayoutPatternId.empty();
    const bool hasTransforms = !interfaceMapVisualTransforms.empty();
    if (hasProfile) {
        appendJsonField(output, indent + 1, "binding_map_profile", interfaceBindingMapProfileId,
                        hasPattern || hasTransforms);
    }
    if (hasPattern) {
        appendJsonField(output, indent + 1, "layout_pattern_id", interfaceMapLayoutPatternId, hasTransforms);
    }
    if (hasTransforms) {
        appendIndent(output, indent + 1);
        output << "\"visual_transforms\": [";
        for (std::size_t index = 0; index < interfaceMapVisualTransforms.size(); ++index) {
            if (index > 0U) {
                output << ", ";
            }
            appendJsonString(output, interfaceMapVisualTransforms[index]);
        }
        output << "]\n";
    }
    appendIndent(output, indent);
    output << "},\n";
}

std::string formatRgbHex(std::uint32_t rgb) {
    if (isTransparentColorModel(rgb)) {
        return "transparent";
    }
    std::ostringstream output;
    output << '#'
           << std::uppercase
           << std::hex
           << std::setw(6)
           << std::setfill('0')
           << (rgb & 0x00FFFFFFU);
    return output.str();
}
void appendJsonField(
    std::ostringstream &output,
    int indent,
    std::string_view name,
    std::string_view value,
    bool comma) {
    appendIndent(output, indent);
    appendJsonString(output, name);
    output << ": ";
    appendJsonString(output, value);
    if (comma) {
        output << ',';
    }
    output << '\n';
}

void appendJsonNumberField(
    std::ostringstream &output,
    int indent,
    std::string_view name,
    double value,
    bool comma) {
    appendIndent(output, indent);
    appendJsonString(output, name);
    // Geometry comes from real zoomed gestures, including pixel midpoints.
    // Three-decimal rounding can move an edge by a pixel on reopening. Emit
    // the shortest JSON number that round-trips the original double exactly.
    std::array<char,64> number{};
    const auto result=std::to_chars(number.data(),number.data()+number.size(),value);
    if(result.ec!=std::errc{}) throw std::runtime_error("Cannot serialize document geometry.");
    output << ": " << std::string_view(number.data(),static_cast<std::size_t>(result.ptr-number.data()));
    if (comma) {
        output << ',';
    }
    output << '\n';
}

void appendJsonIntegerField(
    std::ostringstream &output,
    int indent,
    std::string_view name,
    std::uint32_t value,
    bool comma) {
    appendIndent(output, indent);
    appendJsonString(output, name);
    output << ": " << value;
    if (comma) {
        output << ',';
    }
    output << '\n';
}

void appendJsonBooleanField(
    std::ostringstream &output,
    int indent,
    std::string_view name,
    bool value,
    bool comma) {
    appendIndent(output, indent);
    appendJsonString(output, name);
    output << ": " << (value ? "true" : "false");
    if (comma) {
        output << ',';
    }
    output << '\n';
}

void appendBounds(std::ostringstream &output, const Rect &bounds, int indent, bool comma) {
    appendIndent(output, indent);
    output << "\"bounds\": {\n";
    appendJsonNumberField(output, indent + 1, "x", bounds.x);
    appendJsonNumberField(output, indent + 1, "y", bounds.y);
    appendJsonNumberField(output, indent + 1, "width", bounds.width);
    appendJsonNumberField(output, indent + 1, "height", bounds.height, false);
    appendIndent(output, indent);
    output << '}';
    if (comma) {
        output << ',';
    }
    output << '\n';
}

} // namespace frog::native_engine::document_internal
