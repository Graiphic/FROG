#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>

namespace frog::native_engine {
inline constexpr std::string_view canonicalNumericAggregateProfile="frog.studio.numeric.aggregate-left-fold@1";
inline constexpr std::string_view canonicalNumericAggregateAddId="frog.studio.numeric.add.owned-promote.v1";
inline constexpr std::string_view canonicalNumericCoercionId="frog.studio.numeric.coerce.ieee-even-modulo.v1";
inline constexpr std::string_view canonicalNumericCoercionProfile="frog.studio.numeric.coercion.ieee-even-modulo@1";
inline constexpr std::string_view canonicalNumericExactAbsId="frog.studio.numeric.absolute.exact-width.v1";
struct CanonicalNumericAggregatePort {std::string id;binding::TypeRef type;bool input=false,connected=true;};
// Scalar-only connector adaptation. This says nothing about a receiver's
// policy: its real NumericImplicit contract must also be checked by the caller.
[[nodiscard]] bool canonicalNumericScalarCoercionSupported(const binding::TypeRef &source,
    const binding::TypeRef &target) noexcept;
struct CanonicalNumericCoercionRecipe {std::string_view operationId,inputPort;};
// A derived Array receiver can reuse an existing typed conversion tree when
// its complete result is exactly the receiver schema. Nominal types, arbitrary
// shape changes and unqualified Complex projections are never erased here.
[[nodiscard]] std::optional<CanonicalNumericCoercionRecipe> canonicalNumericTreeCoercionRecipe(
    const binding::TypeRef &source,const binding::TypeRef &target) noexcept;
// Nonempty only for an authored explicit conversion with a fixed destination.
[[nodiscard]] std::string_view canonicalNumericConversionTarget(std::string_view) noexcept;
[[nodiscard]] std::string_view canonicalNumericAggregateMigrationIdentity(std::string_view catalog) noexcept;
[[nodiscard]] bool canonicalNumericAggregateOperationSupported(std::string_view) noexcept;
// The scalar coercion/exact-Abs contracts use ordered in/result connectors.
// Add retains actual driven a,b,input_3..16 and finite primitives/Arrays.
// Nominal types/Records/complex/fixed-point stay outside these contracts.
[[nodiscard]] bool canonicalNumericAggregateSignatureValid(std::string_view,
    std::span<const CanonicalNumericAggregatePort>,std::string *message=nullptr,std::string *port=nullptr) noexcept;
// Keep the exact same-type i32/u32/i64/u64/f64 intrinsic and IEEE slots.
// Only admitted mixed/narrow/Array signatures require the owned service.
[[nodiscard]] bool canonicalNumericAggregateShouldUse(std::span<const CanonicalNumericAggregatePort>) noexcept;
}
