#pragma once
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/FunctionPolymorphicResolution.hpp"

namespace frog::native_engine::binding {
inline constexpr std::string_view kStudioPolymorphicResolver = "frog.studio.polymorphic.type-resolution@1";
// Shared by graph validation, wire colours, terminal creation and Context Help.
// Input evidence comes from the current graph, never a persisted output cache.
[[nodiscard]] BindingResult resolvePolymorphicFunction(const DiagramNode &node,
    const FunctionBindingDefinition &definition, std::string_view port,
    const FunctionInputResolver &input, std::span<const std::string> wiredInputs,
    std::optional<std::string_view> formatLiteral = std::nullopt);
}
