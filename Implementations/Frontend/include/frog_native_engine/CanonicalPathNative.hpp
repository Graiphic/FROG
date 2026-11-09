#pragma once
#include <span>
#include <string_view>
namespace frog::native_engine {
// Exact authored identities. Path, String and context-service contracts remain distinct.
[[nodiscard]] std::string_view canonicalPathValueMigrationIdentity(std::string_view id) noexcept;
[[nodiscard]] std::span<const std::string_view> canonicalPathValueOperationIds() noexcept;
}
