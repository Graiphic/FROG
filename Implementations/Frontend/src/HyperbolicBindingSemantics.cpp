#include "HyperbolicBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace hyperbolic_binding_detail;
bool isHyperbolicFunction(std::string_view id) noexcept { return operation(id) != Operation::Unknown; }

std::span<const std::string_view> hyperbolicInputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 1> unary{"x"};
    return isHyperbolicFunction(id) ? std::span<const std::string_view>(unary) : std::span<const std::string_view>{};
}

HyperbolicTypeResult resolveHyperbolicPort(std::string_view id, std::string_view port,
    std::span<const HyperbolicTypeEvidence> inputs) {
    if (!isHyperbolicFunction(id))
        return failure(ResolutionStatus::Incompatible, "math.hyperbolic.unknown_function", "The Hyperbolic function is not registered.");
    if (port != "x" && port != "result")
        return failure(ResolutionStatus::Incompatible, "math.hyperbolic.missing_port", "The Hyperbolic function has no such connector.");
    const TypeRef *input = nullptr;
    for (const auto &evidence : inputs) {
        if (evidence.portId != "x")
            return failure(ResolutionStatus::Incompatible, "math.hyperbolic.unexpected_input", "The function has no input with this identity.");
        if (input)
            return failure(ResolutionStatus::Incompatible, "math.hyperbolic.multiple_sources", "The x operand has more than one source.");
        input = &evidence.valueType;
        std::size_t nodes = 0;
        const auto reason = validateSchema(*input, 0, nodes, "Hyperbolic");
        if (!reason.empty()) return failure(reason == "The operand type is not established."
            ? ResolutionStatus::Unresolved : ResolutionStatus::Incompatible, "math.hyperbolic.unsupported_type", reason);
    }
    return {{ResolutionStatus::Compatible, input ? port == "result" ? resultTree(*input) : *input
        : TypeRef::primitive("f64"), {}, {}}, input != nullptr};
}
}
