#pragma once
// Numeric identities and connector metadata shared by the real frontend and Runtime.
// SourcePort contains no prepared Runtime descriptor or invocation state.
#include "frog/semantics/studio_numeric_promotion.hpp"
#include "frog/semantics/numeric_fixed_constants.hpp"
#include "frog/sdk/runtime_contract.hpp"
#include <array>
#include <span>
#include <string>
#include <vector>

namespace frog::runtime::numeric_aggregate {
namespace rt=sdk::runtime;
inline constexpr std::string_view profile="frog.studio.numeric.aggregate-left-fold@1";
inline constexpr std::string_view addOperationId="frog.studio.numeric.add.owned-promote.v1";
inline constexpr std::string_view coerceOperationId="frog.studio.numeric.coerce.ieee-even-modulo.v1";
inline constexpr std::string_view exactAbsOperationId="frog.studio.numeric.absolute.exact-width.v1";
// Authored conversion nodes are distinct from edge-derived coercion support.
// These private identities fix the destination representation; their source
// remains an exact complete plain numeric schema, never an erased Enum.
struct ConversionContract {std::string_view operationId,catalogId,destination;};
inline constexpr std::string_view conversionProfile="frog.studio.numeric.explicit-conversion.ieee-even-modulo@1";
inline constexpr std::array<ConversionContract,10> conversionContracts{{
    {"frog.studio.numeric.convert.to_i8.ieee-even-modulo.v1","numeric.conversion.to_i8","i8"},
    {"frog.studio.numeric.convert.to_i16.ieee-even-modulo.v1","numeric.conversion.to_i16","i16"},
    {"frog.studio.numeric.convert.to_i32.ieee-even-modulo.v1","numeric.conversion.to_i32","i32"},
    {"frog.studio.numeric.convert.to_i64.ieee-even-modulo.v1","numeric.conversion.to_i64","i64"},
    {"frog.studio.numeric.convert.to_u8.ieee-even-modulo.v1","numeric.conversion.to_u8","u8"},
    {"frog.studio.numeric.convert.to_u16.ieee-even-modulo.v1","numeric.conversion.to_u16","u16"},
    {"frog.studio.numeric.convert.to_u32.ieee-even-modulo.v1","numeric.conversion.to_u32","u32"},
    {"frog.studio.numeric.convert.to_u64.ieee-even-modulo.v1","numeric.conversion.to_u64","u64"},
    {"frog.studio.numeric.convert.to_sgl.ieee-even-modulo.v1","numeric.conversion.to_sgl","f32"},
    {"frog.studio.numeric.convert.to_dbl.ieee-even-modulo.v1","numeric.conversion.to_dbl","f64"},
}};
struct SourcePort {
    std::string id;rt::ValueType type;bool input=false,connected=false;std::string fullTypeJson;
    std::vector<std::string> childTypeSchemas;
};
} // namespace frog::runtime::numeric_aggregate
