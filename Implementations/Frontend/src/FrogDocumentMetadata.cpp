#include "frog_native_engine/FrogDraftData.hpp"

#include "frog_native_engine/WidgetInstance.hpp"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>

namespace frog::native_engine {

std::string resolvedFrogProgramName(
    std::string_view documentTitle,
    const FrogMetadataProperties &metadata) {
    if (!metadata.useDocumentName && !metadata.customProgramName.empty()) {
        return metadata.customProgramName;
    }
    return documentTitle.empty() ? std::string{ "Untitled 1" } : std::string(documentTitle);
}

std::string normalizeFrogProgramVersion(std::string_view version) {
    std::array<std::uint64_t, 4> components{};
    std::size_t start = 0U;
    for (std::size_t index = 0U; index < components.size(); ++index) {
        const std::size_t end = version.find('.', start);
        const std::string_view token = version.substr(
            start,
            end == std::string_view::npos ? version.size() - start : end - start);
        if (token.empty() || (index > 0U && token.size() > 1U)) {
            return "0.0.0.0";
        }
        std::uint64_t value = 0U;
        for (const char ch : token) {
            if (ch < '0' || ch > '9') {
                return "0.0.0.0";
            }
            if (value > ((std::numeric_limits<std::uint64_t>::max)() - 9U) / 10U) {
                return "0.0.0.0";
            }
            value = (value * 10U) + static_cast<std::uint64_t>(ch - '0');
        }
        if (index > 0U && value > 9U) {
            return "0.0.0.0";
        }
        components[index] = value;
        if (end == std::string_view::npos) {
            for (std::size_t remaining = index + 1U; remaining < components.size(); ++remaining) {
                components[remaining] = 0U;
            }
            break;
        }
        start = end + 1U;
        if (index + 1U == components.size() && start < version.size()) {
            return "0.0.0.0";
        }
    }
    std::ostringstream output;
    output << components[0] << '.' << components[1] << '.' << components[2] << '.' << components[3];
    return output.str();
}

std::string incrementFrogProgramVersion(std::string_view version) {
    const std::string normalized = normalizeFrogProgramVersion(version);
    std::array<std::uint64_t, 4> components{};
    std::stringstream input(normalized);
    char separator = '\0';
    input >> components[0] >> separator >> components[1] >> separator >>
        components[2] >> separator >> components[3];
    for (std::size_t index = components.size() - 1U; index > 0U; --index) {
        if (components[index] < 9U) {
            ++components[index];
            std::ostringstream output;
            output << components[0] << '.' << components[1] << '.' << components[2] << '.' << components[3];
            return output.str();
        }
        components[index] = 0U;
    }
    ++components[0];
    std::ostringstream output;
    output << components[0] << '.' << components[1] << '.' << components[2] << '.' << components[3];
    return output.str();
}

std::string formatDiagramNumericValue(const DiagramNode &node) {
    std::string rawText = node.text.empty() ? std::string{ "0" } : node.text;
    WidgetInstance formatting;
    formatting.valueType = node.valueType.empty() ? std::string{ "u8" } : node.valueType;
    formatting.numericDisplayFormat = node.numericDisplayFormat;
    formatting.numericDisplayDigits = node.numericDisplayDigits;
    formatting.numericPrecisionType = node.numericPrecisionType;
    formatting.numericHideTrailingZeros = node.numericHideTrailingZeros;
    formatting.numericExponentMultipleOfThree = node.numericExponentMultipleOfThree;
    formatting.numericMinimumFieldWidth = node.numericMinimumFieldWidth;
    formatting.numericPadding = node.numericPadding;
    formatting.numericAlignment = node.numericAlignment;
    formatting.numericSignDisplay = node.numericSignDisplay;
    formatting.numericLocale = node.numericLocale;
    formatting.numericDisplayPrefix = node.numericDisplayPrefix;
    formatting.numericDisplaySuffix = node.numericDisplaySuffix;
    formatting.numericRadixShowPrefix = node.numericRadixShowPrefix;
    formatting.numericRadixUppercase = node.numericRadixUppercase;
    formatting.numericRadixGroupSize = node.numericRadixGroupSize;
    formatting.numericRadixInterpretation = node.numericRadixInterpretation;
    formatting.numericCustomFormatString = node.numericCustomFormatString;
    if (numericCanonicalValueTypeIsComplex(formatting.valueType)) {
        formatting.classId = "frog.widgets.numeric_control";
        if (!setWidgetNumericComplexValueLexeme(formatting, rawText)) return rawText;
        return formatWidgetNumericComplexValue(formatting);
    }
    char *end = nullptr;
    const double value = std::strtod(rawText.c_str(), &end);
    if (end == rawText.c_str() || end == nullptr || *end != '\0') return rawText;
    return formatNumericValue(formatting, value);
}

} // namespace frog::native_engine
