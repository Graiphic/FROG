#pragma once

#include "frog_native_engine/Geometry.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include <cstdint>
#include <iosfwd>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine::document_internal {

// Shared emission rules for private document sections. Number emission must
// preserve the exact authored geometry through a save/load round trip.
// Emit only the retained legacy private presentation map. No declaration,
// port, type or executable behavior is inferred from these source strings.
void appendPrivateInterfaceMap(std::ostringstream &output, int indent,
    std::string_view interfaceBindingMapProfileId,
    std::string_view interfaceMapLayoutPatternId,
    const std::vector<std::string> &interfaceMapVisualTransforms);

std::string formatRgbHex(std::uint32_t rgb);
void appendJsonField(std::ostringstream &output, int indent, std::string_view name,
    std::string_view value, bool comma = true);
void appendJsonNumberField(std::ostringstream &output, int indent, std::string_view name,
    double value, bool comma = true);
void appendJsonIntegerField(std::ostringstream &output, int indent, std::string_view name,
    std::uint32_t value, bool comma = true);
void appendJsonBooleanField(std::ostringstream &output, int indent, std::string_view name,
    bool value, bool comma = true);
void appendBounds(std::ostringstream &output, const Rect &bounds, int indent, bool comma = true);
void appendJsonValue(std::ostringstream &output, const source::JsonValue &value);

} // namespace frog::native_engine::document_internal
