#pragma once
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
namespace frog::native_engine {
// Local Studio profile. These are not narrowed aliases of public Collections.
inline constexpr std::string_view canonicalArrayReal1dProfile="frog.studio.array.real1d@1";
inline constexpr std::string_view canonicalArraySourceType="array<f64>";
inline constexpr std::size_t canonicalArrayMaximumBuildInputs=32;
enum class CanonicalArrayOperation {Build,Index,Size};
struct CanonicalArrayNativeContract {CanonicalArrayOperation operation;std::string_view catalogId,operationId,outputPort,outputType;};
[[nodiscard]] std::span<const CanonicalArrayNativeContract> canonicalArrayNativeContracts()noexcept;
[[nodiscard]] const CanonicalArrayNativeContract *canonicalArrayNativeContract(std::string_view)noexcept;
[[nodiscard]] std::string_view canonicalArrayMigrationIdentity(std::string_view)noexcept;
// Generic owned carriers are distinct from the historical packed-f64 profile.
inline constexpr std::string_view canonicalArrayOwnedProfile="frog.studio.array.owned@1";
// Historical rank-one Index stays versioned. Extended Index retains real
// dimensions, implicit groups and full-schema intrinsic defaults.
inline constexpr std::string_view canonicalArrayIndexExtendedId="frog.studio.array.index.owned.v2";
inline constexpr std::string_view canonicalMatrixBuildRowsId="frog.studio.array.matrix.build.rows.owned.v1";
inline constexpr std::string_view canonicalArrayIndexOwnedId="frog.studio.array.index.owned.v1";
inline constexpr std::string_view canonicalArrayBuildOwnedId="frog.studio.array.build.owned.v1";
inline constexpr std::string_view canonicalArrayConcatenateOwnedId="frog.studio.array.concatenate.owned.v1";
[[nodiscard]] std::string_view canonicalArrayValueMigrationIdentity(std::string_view)noexcept;
[[nodiscard]] std::span<const std::string_view> canonicalArrayValueOperationIds()noexcept;
[[nodiscard]] std::string canonicalArrayBuildPort(std::size_t);
[[nodiscard]] std::optional<std::size_t> canonicalArrayBuildPortOrdinal(std::string_view)noexcept;
[[nodiscard]] bool canonicalArrayBuildPortsValid(std::span<const std::string>)noexcept;
}
