#pragma once
// One immutable Source/model contract. No widget, Canvas or execution service.
#include <algorithm>
#include <array>
#include <string_view>

namespace frog::native_engine {
inline constexpr std::string_view kDefaultButtonMechanicalAction = "switch_when_pressed";
inline constexpr std::array<std::string_view, 6> kButtonMechanicalActions{
    "switch_when_pressed",
    "switch_when_released",
    "switch_until_released",
    "latch_when_pressed",
    "latch_when_released",
    "latch_until_released"
};

[[nodiscard]] inline std::string_view defaultButtonMechanicalAction() noexcept {
    return kDefaultButtonMechanicalAction;
}

[[nodiscard]] inline bool isSupportedButtonMechanicalAction(std::string_view action) noexcept {
    return std::find(kButtonMechanicalActions.begin(), kButtonMechanicalActions.end(), action) !=
        kButtonMechanicalActions.end();
}
}
