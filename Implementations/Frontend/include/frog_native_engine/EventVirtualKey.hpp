#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

namespace frog::native_engine {
// FROG-local ordinal enum v1. These are NOT Windows VK codes or NI binary values.
inline constexpr std::array<std::string_view,55> eventVirtualKeyLabels{
    "ASCII",
    "Enter",
    "Return",
    "Shift",
    "Ctrl",
    "Alt",
    "Cmd",
    "Opt",
    "Caps",
    "NumLock",
    "Clear",
    "Break",
    "Up",
    "Down",
    "Left",
    "Right",
    "Insert",
    "Delete",
    "Home",
    "End",
    "PageUp",
    "PageDown",
    "Escape",
    "Help",
    "Pause",
    "ScrollLock",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "Play",
    "VolumeUp",
    "VolumeDown",
    "Mute",
    "Two Byte Char",
};
inline binding::TypeRef eventVirtualKeyType() {
    auto type=binding::TypeRef::enumeration("frog.event.virtual_key.v1",binding::TypeRef::primitive("u32"));
    type.choiceKind=binding::ChoiceKind::Enum;
    for(const auto label:eventVirtualKeyLabels)
        type.choices.push_back({std::string(label),std::string(label),static_cast<std::uint32_t>(type.choices.size())});
    return type;
}
// Windows adapter, no Windows headers or environmental access. Character is a
// Unicode scalar from the existing host translation, never an UTF-8 byte count.
// Unclassified native keys have no enum value; callers preserve their raw data.
inline std::optional<std::uint32_t> eventVirtualKeyFromWindows(std::uint32_t key,bool extended,
    std::uint32_t character=0) noexcept {
    if(key>=0x70 && key<=0x87)return 26+(key-0x70); // F1 .. F24
    switch(key) {
    case 0x0d:return extended?1U:2U; // keypad Enter / main Return
    case 0x10:case 0xa0:case 0xa1:return 3;
    case 0x11:case 0xa2:case 0xa3:return 4;
    case 0x12:case 0xa4:case 0xa5:return 5;
    case 0x14:return 8;case 0x90:return 9;case 0x0c:return 10;case 0x03:return 11;
    case 0x26:return 12;case 0x28:return 13;case 0x25:return 14;case 0x27:return 15;
    case 0x2d:return 16;case 0x2e:return 17;case 0x24:return 18;case 0x23:return 19;
    case 0x21:return 20;case 0x22:return 21;case 0x1b:return 22;case 0x2f:return 23;
    case 0x13:return 24;case 0x91:return 25;case 0xfa:return 50; // VK_PLAY, not MEDIA_PLAY_PAUSE
    case 0xaf:return 51;case 0xae:return 52;case 0xad:return 53;
    default:break;
    }
    // Ordinary characters belong to one category; Windows/Cmd/Opt and IME
    // control keys are not guessed. Two Byte Char requires a separate verified
    // IME contract and is deliberately not inferred from Unicode/UTF-8 width.
    if(character || key==0x08 || key==0x09 || key==0x20 ||
        (key>=0x30 && key<=0x39) || (key>=0x41 && key<=0x5a) ||
        (key>=0x60 && key<=0x6f) || (key>=0xba && key<=0xc0) ||
        (key>=0xdb && key<=0xdf) || key==0xe2)return 0;
    return {};
}
}
