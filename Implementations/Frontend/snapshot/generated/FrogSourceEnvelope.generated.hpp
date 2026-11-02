// Generated from the public FROG source schema. Do not edit.
#pragma once

#include <array>
#include <string_view>

namespace frog::native_engine::source::envelope {

inline constexpr std::array<std::string_view, 4> requiredSections{{
    "spec_version",
    "metadata",
    "interface",
    "diagram",
}};

inline constexpr std::array<std::string_view, 11> knownSections{{
    "cache",
    "connector",
    "diagram",
    "execution_policy",
    "front_panel",
    "host",
    "icon",
    "ide",
    "interface",
    "metadata",
    "spec_version",
}};

} // namespace frog::native_engine::source::envelope
