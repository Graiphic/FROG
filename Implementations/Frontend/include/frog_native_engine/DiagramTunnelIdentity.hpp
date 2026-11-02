#pragma once
// Stable portable boundary identities, shared by Source/FIR and the editor.
// These constructors have no dependency on graphical models or type resolution.
#include <string>
#include <string_view>

namespace frog::native_engine::diagram_identity {
[[nodiscard]] inline std::string tunnelInputId(std::string_view id) { return "tunnel:" + std::string(id) + ":input"; }
[[nodiscard]] inline std::string tunnelOutputId(std::string_view id) { return "tunnel:" + std::string(id) + ":output"; }
}
