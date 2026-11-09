#pragma once
#include "frog_native_engine/InterpolationBinding.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"

namespace frog::native_engine {
// Projects the selected public semantic variant, without UI or runtime state.
[[nodiscard]] const FunctionBindingDefinition *interpolationBindingDefinition(
    std::string_view catalogId,std::string_view variant = {}) noexcept;
}
