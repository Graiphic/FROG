#pragma once
#include "frog_native_engine/InPlaceElementContract.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include <optional>
#include <span>

namespace frog::native_engine {
// Explicit Studio extension. This does not promote the blocked public IPE
// authoring declaration to a published FROG contract.
inline constexpr std::string_view canonicalInPlaceProfile="frog.studio.ipe@1";
[[nodiscard]] std::optional<std::vector<DiagramInPlacePair>> canonicalInPlaceReadPairs(
    const source::JsonValue &, bool authored, std::string *diagnostic=nullptr);
[[nodiscard]] source::JsonValue canonicalInPlaceConfiguration(std::span<const DiagramInPlacePair>);
[[nodiscard]] std::optional<std::vector<DiagramInPlacePair>> canonicalInPlaceReadConfiguration(
    const source::JsonValue &, std::string *diagnostic=nullptr);
}
