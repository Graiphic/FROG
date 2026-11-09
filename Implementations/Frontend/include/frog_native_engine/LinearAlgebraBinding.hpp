#pragma once

#include "frog_native_engine/BindingType.hpp"
#include <span>
#include <string_view>

namespace frog::native_engine {

// Public semantic description only. Algorithms and their target providers do
// not belong to this Frontend. Matrix keeps the existing numeric Array rank 2.
struct LinearAlgebraFunctionDescriptor {
    std::string_view catalogId;
    std::string_view operationId;
    std::string_view displayName;
};
struct LinearAlgebraInputDefault {
    std::string_view portId;
    std::string_view valueType;
    std::string_view jsonValue;
};
struct LinearAlgebraTypeEvidence {
    std::string_view portId;
    binding::TypeRef valueType;
};
struct LinearAlgebraTypeResult {
    binding::TypeResolution resolution;
    bool complete = false;
};

[[nodiscard]] std::span<const LinearAlgebraFunctionDescriptor>
linearAlgebraFunctions() noexcept;
[[nodiscard]] const LinearAlgebraFunctionDescriptor *findLinearAlgebraFunction(
    std::string_view catalogId) noexcept;
[[nodiscard]] bool isLinearAlgebraFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> linearAlgebraInputPortIds(
    std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const std::string_view> linearAlgebraOutputPortIds(
    std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const LinearAlgebraInputDefault> linearAlgebraInputDefaults(
    std::string_view catalogId) noexcept;
[[nodiscard]] LinearAlgebraTypeResult resolveLinearAlgebraPort(
    std::string_view catalogId,
    std::string_view portId,
    std::span<const LinearAlgebraTypeEvidence> inputs = {});

} // namespace frog::native_engine
