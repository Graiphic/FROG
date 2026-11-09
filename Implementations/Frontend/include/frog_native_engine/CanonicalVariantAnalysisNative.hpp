#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string_view>
namespace frog::native_engine {
[[nodiscard]] std::string_view canonicalVariantPrimitiveMigrationIdentity(std::string_view) noexcept;
[[nodiscard]] std::string_view canonicalVariantAnalysisMigrationIdentity(std::string_view) noexcept;
[[nodiscard]] std::span<const std::string_view> canonicalVariantAnalysisOperationIds() noexcept;
// Only the compiled service's actual own connector contracts. This supplies
// exact Enum definitions already held by GS; it never retags a user widget.
[[nodiscard]] std::optional<binding::TypeRef> canonicalVariantAnalysisInternalPortType(
    std::string_view catalog,std::string_view port,bool input);
}
