#pragma once
#include <cstddef>
#include <optional>
#include <string>
#include <span>
#include <string_view>

namespace frog::native_engine {
// Private admission inventory for the existing real LLVM scalar instructions.
// This is not a new public FROG primitive, provider ABI, or graph evaluator.
enum class CanonicalScalarDomain { ExistingAdd, FiniteF64Arithmetic, ExactEquality, ExactOrder, Boolean, UnarySame, UnarySigned, UnarySignedChecked, UnaryRounding, UnaryIntegerRounding, ZeroComparison, UnaryReal64, IntegerDivideReal64 };
enum class CanonicalScalarPolicy { CoreCheckedFinite, StudioIeeeWrap64V1, StudioWrap32V1, MathValueV1, StudioRoundingFenvV1, StudioComparisonZeroV1, StudioRoundingEvenV1 };
inline constexpr std::string_view canonicalComparisonZeroProfile="frog.studio.comparison.zero-exact-ieee@1";
struct CanonicalScalarNativeContract {
    std::string_view catalogId, operationId, semanticVersion;
    CanonicalScalarDomain domain;
    std::size_t inputCount;
    std::string_view inputA, inputB, output;
    std::string_view arithmeticPolicy;
    CanonicalScalarPolicy policy=CanonicalScalarPolicy::CoreCheckedFinite;
    // Zero preserves a fixed inputCount; local Boolean and Add reductions use16.
    std::size_t maximumInputCount=0;
};
[[nodiscard]] std::span<const CanonicalScalarNativeContract> canonicalScalarNativeContracts() noexcept;
[[nodiscard]] const CanonicalScalarNativeContract *canonicalScalarNativeContract(std::string_view operation) noexcept;
// Authored arithmetic/comparison -> versioned private Studio policy mapping.
// Core public Source keeps its separate checked/finite contract. Boolean
// operations whose exact behavior is shared retain their core identity.
// Unknown identities return empty; names/geometry never infer semantics.
[[nodiscard]] std::string_view canonicalScalarMigrationIdentity(std::string_view authoredOperation) noexcept;
// A migration hook, not a type resolver. Its caller must supply the actual
// resolved same-type operand domain; empty/unknown and narrow division refuse.
// Direct canonical Source IDs remain governed by their own exact contracts.
[[nodiscard]] std::string_view canonicalScalarMigrationIdentityForType(
    std::string_view authoredOperation,std::string_view resolvedOperandType) noexcept;
inline constexpr std::string_view canonicalBooleanReductionProfile="frog.studio.boolean.reduce@1";
inline constexpr std::string_view canonicalNumericAddReductionProfile="frog.studio.numeric.add.reduce@1";
[[nodiscard]] std::optional<std::size_t> canonicalNumericAddInputOrdinal(std::string_view) noexcept;
[[nodiscard]] bool canonicalNumericAddInputPortsValid(std::span<const std::string>) noexcept;
[[nodiscard]] std::optional<std::size_t> canonicalBooleanInputOrdinal(std::string_view) noexcept;
[[nodiscard]] bool canonicalBooleanInputPortsValid(std::span<const std::string>) noexcept;
// Actual connected input order: a,b,then ascending input_3..input_16.
// Unconnected optional editor slots do not invent executable operands.
[[nodiscard]] std::string_view canonicalScalarMigrationIdentityForArity(std::string_view,std::size_t) noexcept;
[[nodiscard]] bool canonicalScalarNativeOperandAccepted(const CanonicalScalarNativeContract &, std::string_view type) noexcept;
}
