#pragma once
#include "BindingResult.hpp"
#include "FunctionBindingCatalog.hpp"
#include <functional>

namespace frog::native_engine::binding {
using FunctionInputResolver = std::function<BindingResult(std::string_view)>;
struct FunctionPolymorphicOptions { bool comparisonAggregates=false; };
// Existing Boolean/Comparison/Select resolution is shared by the real Source
// generators and the Studio wrapper. Numeric arithmetic delegates here after
// the wrapper's earlier operation-specific palette branches.
[[nodiscard]] bool hasSharedPolymorphicResolution(std::string_view catalogId) noexcept;
[[nodiscard]] BindingResult resolveSharedPolymorphicFunction(
    const FunctionPolymorphicOptions &,const FunctionBindingDefinition &,
    std::string_view port,const FunctionInputResolver &,
    std::span<const std::string> wiredInputs);
[[nodiscard]] TypeRef mapPolymorphicLeaves(const TypeRef &,const std::function<TypeRef(const TypeRef &)> &);
[[nodiscard]] TypeResolution commonPolymorphicType(const TypeRef &,const TypeRef &,bool lift=false);
}
