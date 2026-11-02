#pragma once
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace frog::native_engine {
// Local execution contracts, distinct from the public abstract frog.text law.
inline constexpr std::string_view canonicalStringBytesProfile="frog.studio.string.bytes@1";
inline constexpr std::string_view canonicalStringConstantProfile="frog.studio.string-constant.bytes@1";
inline constexpr std::size_t canonicalStringMaximumBytes=1'000'000;
inline constexpr std::size_t canonicalStringMaximumInputs=16;
enum class CanonicalStringOperation { LengthBytes, ConcatenateBytes };
struct CanonicalStringNativeContract {
    CanonicalStringOperation operation;
    std::string_view catalogId,operationId,semanticVersion,outputType;
    std::size_t minimumInputs,maximumInputs;
};
[[nodiscard]] std::span<const CanonicalStringNativeContract> canonicalStringNativeContracts() noexcept;
[[nodiscard]] const CanonicalStringNativeContract *canonicalStringNativeContract(std::string_view operationId) noexcept;
[[nodiscard]] std::string_view canonicalStringMigrationIdentity(std::string_view authoredCatalogId) noexcept;
// Owned compiled value operations use the composed path, with heterogeneous
// operands and atomic tuples. Keep their raw-byte identity separate from the
// older UTF-8 scalar Length/Concat corridor above. Numeric's two byte conversion
// palette aliases resolve to the same operations; no duplicate Runtime IDs.
[[nodiscard]] std::string_view canonicalStringValueMigrationIdentity(std::string_view authoredCatalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> canonicalStringValueOperationIds() noexcept;
// Canonical ordering of the real persisted Concat endpoints; never geometry.
[[nodiscard]] std::optional<std::size_t> canonicalStringConcatPortOrdinal(std::string_view) noexcept;
[[nodiscard]] bool canonicalStringInputPortsValid(const CanonicalStringNativeContract &,
    std::span<const std::string>) noexcept;
// Bounded callers check their byte budget before this allocation-free walk.
// U+0000 is data. Invalid sequences/surrogates are never repaired or normalized.
[[nodiscard]] bool canonicalStringUtf8Valid(std::string_view) noexcept;
}
