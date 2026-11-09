#pragma once

#include "frog_native_engine/FrogJson.hpp"

namespace frog::native_engine::source::internal {

// Private Studio widget/draft payloads use a heap worklist. They retain the
// public lexical rules and byte/string/value budgets, but do not mistake JSON
// wrapper depth for an authoring limit on nested containers. Callers must admit
// the private draft authority before using a document candidate. Public source
// readers continue to use parseJson and its published nesting profile.
[[nodiscard]] std::optional<JsonValue> parseAuthoringJsonCandidate(std::string_view source);
void clearJsonTree(JsonValue &value);

// Compatibility decoder for historical private drafts only. Raw bytes in
// string values are retained; keys, escapes, grammar and limits remain strict.
// The result is only a candidate: callers must verify the complete root draft
// authority and supported historical revision before admitting it. Public
// source readers must always use parseJson instead.
[[nodiscard]] std::optional<JsonValue> parseLegacyByteStringDraftCandidate(
    std::string_view source);

} // namespace frog::native_engine::source::internal
