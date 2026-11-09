#include "PartialDifferentialBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace partialdifferential_binding_detail;
bool isPartialDifferentialFunction(std::string_view id) noexcept { try { return definition(id) != nullptr; } catch (...) { return false; } }

T partialDifferentialProblemType(std::string_view family) { const auto* d = definition("math.pde.define_equation." + std::string(family) + ".numeric"); if (!d) return {}; return parse(port(*d, "pde_out")->defaultValueType); }

T partialDifferentialCallableType(std::string_view family) { const auto* d = definition("math.pde.define_equation." + std::string(family) + ".reference"); if (!d) return {}; return parse(port(*d, "function")->defaultValueType); }

PartialDifferentialTypeResult resolvePartialDifferentialPort(std::string_view id, std::string_view requested, std::span<const PartialDifferentialTypeEvidence> evidence) {
    const auto* d = definition(id); if (!d || !port(*d, requested)) return {{binding::ResolutionStatus::Incompatible, {}, "PDE_FAMILY_MISMATCH", "Unknown PDE function or connector."}, false};
    bool complete = true; std::map<std::string_view, T> known;
    for (const auto& item : evidence) { const auto* expected = port(*d, item.portId); if (!expected || expected->direction != FunctionPortDirection::Input || !known.emplace(item.portId, item.valueType).second) return {{binding::ResolutionStatus::Incompatible, {}, "PDE_SHAPE_MISMATCH", "Unknown or duplicate PDE input."}, false};
        const auto type = parse(expected->defaultValueType); const auto c = binding::connect(item.valueType, type, type.numeric() ? binding::ConnectionPolicy::NumericImplicit : binding::ConnectionPolicy::Exact);
        if (!c.accepted()) return {{binding::ResolutionStatus::Incompatible, {}, item.portId == "pde_in" ? "PDE_FAMILY_MISMATCH" : item.portId == "function" ? "PDE_CALLBACK_TYPE_MISMATCH" : "PDE_SHAPE_MISMATCH", "PDE inputs preserve exact field rank, family and strict callable types."}, false}; }
    for (const auto& p : d->ports) if (p.direction == FunctionPortDirection::Input && p.requirement == FunctionPortRequirement::Required && !known.contains(p.portId)) complete = false;
    return {{binding::ResolutionStatus::Compatible, parse(port(*d, requested)->defaultValueType), {}, {}}, complete};
}
}
