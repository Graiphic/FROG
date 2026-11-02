#pragma once
#include <cstddef>
#include <span>
#include <string_view>
namespace frog::native_engine {
// Local exact scalar subset of the existing authored Select/Max & Min.
// This does not redefine their generic binding or a public Core primitive.
inline constexpr std::string_view canonicalScalarSelectionProfile="frog.studio.scalar-selection@1";
inline constexpr std::string_view canonicalScalarMaxMinNanProfile="frog.studio.scalar-maxmin.nan-aware@2";
enum class CanonicalScalarSelectionKind {Select,MaxMin};
struct CanonicalScalarSelectionContract {
    std::string_view catalogId,operationId;
    CanonicalScalarSelectionKind kind;
    std::span<const std::string_view> inputs,outputs;
    std::string_view semanticVersion=canonicalScalarSelectionProfile;
    bool ignoreSingleNaN=false;
};
[[nodiscard]] std::span<const CanonicalScalarSelectionContract> canonicalScalarSelectionContracts() noexcept;
[[nodiscard]] const CanonicalScalarSelectionContract *canonicalScalarSelectionContract(std::string_view localOperation) noexcept;
[[nodiscard]] const CanonicalScalarSelectionContract *canonicalScalarSelectionAuthoredContract(std::string_view authoredOperation) noexcept;
[[nodiscard]] bool canonicalScalarSelectionTypeAccepted(const CanonicalScalarSelectionContract &,std::string_view exactDataType) noexcept;
}
