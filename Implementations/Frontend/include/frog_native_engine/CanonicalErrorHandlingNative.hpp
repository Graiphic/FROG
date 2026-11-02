#pragma once
#include <span>
#include <string_view>
namespace frog::native_engine {
[[nodiscard]] std::string_view canonicalErrorHandlingMigrationIdentity(std::string_view) noexcept;
[[nodiscard]] std::span<const std::string_view> canonicalErrorHandlingOperationIds() noexcept;
}
