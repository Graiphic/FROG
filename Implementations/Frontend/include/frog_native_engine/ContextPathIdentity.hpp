#pragma once
#include <string_view>

namespace frog::native_engine {
// Authoring identity shared by the two palette accesses, rendering and lowering.
// Context paths are evaluated operations; they are never editable Path literals.
[[nodiscard]] constexpr bool isContextPathFunction(std::string_view id) noexcept {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    return id == "path.current_frog_path" || id == "path.default_directory" ||
        id == "path.temporary_directory" || id == "path.application_directory";
}
}
