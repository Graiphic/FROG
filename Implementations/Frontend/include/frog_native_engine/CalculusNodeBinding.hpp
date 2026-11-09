#pragma once
#include "frog_native_engine/CalculusBinding.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"

namespace frog::native_engine {
// Projects the selected public semantic variant without editing a document.
[[nodiscard]] const FunctionBindingDefinition *calculusBindingDefinition(
    std::string_view catalogId,std::string_view variant = {}) noexcept;

// Explicit paired-point callback profiles. Dimensions outside 1..3 return an
// empty identity/schema; an unrelated strict document reference is not a cast.
[[nodiscard]] std::string_view calculusFunctionReferenceOperation(unsigned dimensions) noexcept;
[[nodiscard]] std::string_view calculusFunctionReferenceSchema(unsigned dimensions) noexcept;
[[nodiscard]] unsigned calculusFunctionReferenceDimensions(std::string_view operation) noexcept;
} // namespace frog::native_engine
