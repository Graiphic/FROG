#pragma once
// Stable portable boundary identities, shared by Source/FIR and the editor.
// These constructors have no dependency on graphical models or type resolution.
#include <string>
#include <string_view>
#include <optional>

namespace frog::native_engine::diagram_identity {
[[nodiscard]] inline std::string tunnelInputId(std::string_view id) { return "tunnel:" + std::string(id) + ":input"; }
[[nodiscard]] inline std::string tunnelOutputId(std::string_view id) { return "tunnel:" + std::string(id) + ":output"; }

enum class TunnelFace { Input, Output, Condition };
struct TunnelPortView { std::string_view id; TunnelFace face; };

[[nodiscard]] inline std::optional<TunnelPortView> parseTunnelPort(std::string_view port) noexcept {
    constexpr std::string_view prefix = "tunnel:";
    if (!port.starts_with(prefix)) return std::nullopt;
    port.remove_prefix(prefix.size());
    // The stable identity itself may contain colons.
    const auto separator = port.rfind(':');
    if (separator == std::string_view::npos) return std::nullopt;
    const auto id = port.substr(0, separator);
    const auto face = port.substr(separator + 1);
    if (face == "input") return TunnelPortView{id, TunnelFace::Input};
    if (face == "output") return TunnelPortView{id, TunnelFace::Output};
    if (face == "condition") return TunnelPortView{id, TunnelFace::Condition};
    return std::nullopt;
}
}
