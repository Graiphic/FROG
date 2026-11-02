#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <cstdint>
namespace frog::native_engine::binding {
struct StructureBindingDefinition {
    std::string_view id;
    std::uint32_t version;
    std::string_view selectorOuterPort;
    std::string_view selectorInnerPort;
    TypeExpr selectorConstraint;
    bool requireEveryRegion;
    bool allowDefaultIfUnwired;
    std::string_view resolverId;
    std::string_view loweringId;
};
// Built-in, versioned registry projection. Declaring a lowering ID does not
// install that runtime capability.
[[nodiscard]] const StructureBindingDefinition *findStructureBindingDefinition(std::string_view id);
[[nodiscard]] const StructureBindingDefinition &caseBindingDefinition();
}
