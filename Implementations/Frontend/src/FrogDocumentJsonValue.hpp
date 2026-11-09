#pragma once

#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/FrogJson.hpp"

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine::document_internal {

using JsonValue = source::JsonValue;

[[nodiscard]] const JsonValue *objectField(
    const JsonValue &value,
    std::string_view name) noexcept;
[[nodiscard]] JsonValue *mutableObjectField(
    JsonValue &value,
    std::string_view name);
[[nodiscard]] std::optional<std::string> jsonStringField(
    const JsonValue &value,
    std::string_view name);
[[nodiscard]] std::optional<double> jsonNumberField(
    const JsonValue &value,
    std::string_view name);
[[nodiscard]] std::optional<bool> jsonBooleanField(
    const JsonValue &value,
    std::string_view name);
[[nodiscard]] std::vector<std::string> jsonStringArrayField(
    const JsonValue &value,
    std::string_view name);
[[nodiscard]] std::vector<std::uint32_t> jsonUnsignedArrayField(
    const JsonValue &value,
    std::string_view name);
[[nodiscard]] std::optional<std::uint32_t> jsonUnsignedField(
    const JsonValue &value,
    std::string_view name);
[[nodiscard]] std::optional<double> parseCssPixelValue(std::string_view text);
[[nodiscard]] std::optional<double> jsonPixelField(
    const JsonValue &value,
    std::string_view name);

void flattenCanonicalStructureRegions(JsonValue &diagram);
[[nodiscard]] std::string_view canonicalStructureTypeForNodeType(
    std::string_view nodeType);
[[nodiscard]] std::string_view nodeTypeForCanonicalStructure(
    std::string_view structureType);
void deserializeStructureProperties(
    const JsonValue &nodeValue,
    DiagramNode &node);

} // namespace frog::native_engine::document_internal
