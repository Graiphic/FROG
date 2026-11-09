#pragma once
#include <string_view>

namespace frog::native_engine {
// Catalog identity only. Runtime capability and semantics are checked separately.
// Palette-only rows and historical aliases retain their previous diagnostics.
[[nodiscard]] bool isKnownFunctionCatalogIdentity(std::string_view id) noexcept;
} // namespace frog::native_engine
