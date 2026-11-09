#pragma once
#include "frog_native_engine/FrogJson.hpp"
namespace frog::native_engine {
// The envelope selects one source authority. A public Source declaration
// cannot simultaneously carry any Studio-draft authority field. Inspect the
// existing parsed tree; this check never reads, rewrites or reparses a document.
[[nodiscard]] inline bool mixedFrogSourceAuthorities(const source::JsonValue &root) noexcept {
    return root.member("spec_version")&&(root.member("format")||root.member("draft_revision")||root.member("frontPanel"));
}
// Exact historical authoring JSON policy shared with the production reader.
// Strict Source callers must keep using FrogSourceDocument instead.
[[nodiscard]] std::optional<source::JsonValue> readFrogDraftJson(std::string_view,std::string * = nullptr);
}
