#pragma once
#include <span>
#include <string_view>
namespace frog::native_engine {
// Authoring identities only. The compiled Runtime owns algorithms/signatures.
[[nodiscard]] std::string_view canonicalTimestampMigrationIdentity(std::string_view) noexcept;
[[nodiscard]] std::span<const std::string_view> canonicalTimestampOperationIds() noexcept;
}
