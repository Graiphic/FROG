#pragma once
#include <span>
#include <string_view>
namespace frog::native_engine {
// Canonical Error value schema shared by palette, structure ports and Source.
inline constexpr std::string_view kErrorClusterType = "cluster<status:bool,code:i32,source:string>";
[[nodiscard]] std::string_view canonicalErrorHandlingMigrationIdentity(std::string_view) noexcept;
[[nodiscard]] std::span<const std::string_view> canonicalErrorHandlingOperationIds() noexcept;
}
