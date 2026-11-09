#pragma once
#include "FrogDocumentJsonValue.hpp"
#include <iosfwd>

namespace frog::native_engine::document_internal {
// Private draft boundary. This is not a public spec_version source/FIR codec.
void appendApplicationControl(std::ostringstream &, const ApplicationControlNode &, int indent);
[[nodiscard]] bool readApplicationControl(const JsonValue &node, DiagramNode &, std::string *error);
}
