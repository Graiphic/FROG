#pragma once
#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <string_view>

namespace frog::native_engine::hashing {
// Exact byte digest, shared by source attribution and PE metadata inspection.
// Admission/read budgets belong to callers; no UI, JSON or Runtime contract.
[[nodiscard]] std::array<std::uint8_t,32> sha256(std::span<const std::uint8_t> bytes);
[[nodiscard]] std::string sha256Hex(std::string_view bytes);
}
