#include "HypergeometricBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace hypergeometric_binding_detail;
bool isHypergeometricFunction(std::string_view id) noexcept { return !hypergeometricFunctionKey(id).empty(); }

std::string_view hypergeometricFunctionKey(std::string_view id) noexcept {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    for (const auto &[identity, key] : identities) if (id == identity) return key;
    return {};
}

std::span<const std::string_view> hypergeometricInputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 4> gauss{"x", "a", "b", "c"};
    static constexpr std::array<std::string_view, 3> confluent{"x", "a", "b"};
    const auto key = hypergeometricFunctionKey(id);
    return key.empty() ? std::span<const std::string_view>{} : key == "gauss" ? std::span<const std::string_view>(gauss) : std::span<const std::string_view>(confluent);
}

std::span<const std::string_view> hypergeometricOutputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 1> output{"value"};
    return isHypergeometricFunction(id) ? std::span<const std::string_view>(output) : std::span<const std::string_view>{};
}

HypergeometricTypeResult resolveHypergeometricPort(std::string_view id, std::string_view port,
    std::span<const HypergeometricTypeEvidence> evidence) {
    const auto names = hypergeometricInputPortIds(id);
    if (names.empty()) return rejected(ResolutionStatus::Incompatible, "math.hypergeometric.unknown_function", "The Hypergeometric function is not registered.");
    if (port != "value" && std::find(names.begin(), names.end(), port) == names.end())
        return rejected(ResolutionStatus::Incompatible, "math.hypergeometric.missing_port", "This Hypergeometric function has no such terminal.");
    std::array<const TypeRef *, 4> types{};
    for (const auto &input : evidence) {
        const auto found = std::find(names.begin(), names.end(), input.portId);
        if (found == names.end()) return rejected(ResolutionStatus::Incompatible, "math.hypergeometric.unexpected_input", "A Hypergeometric input identity is unknown.");
        auto &slot = types[static_cast<std::size_t>(found - names.begin())];
        if (slot) return rejected(ResolutionStatus::Incompatible, "math.hypergeometric.multiple_sources", "A Hypergeometric terminal has multiple sources.");
        slot = &input.valueType;
        if (!slot->known()) return rejected(ResolutionStatus::Unresolved, "math.hypergeometric.unresolved_type", "The Hypergeometric input type is not established.");
        std::size_t count = 0;
        const auto reason = numeric_tree_math::validateSchema(*slot, 0, count, "Hypergeometric", false);
        if (!reason.empty()) return rejected(ResolutionStatus::Incompatible, "math.hypergeometric.unsupported_type", reason);
    }
    try {
        TypeRef result = TypeRef::primitive("f64");
        const TypeRef *input = nullptr; bool complete = true;
        for (std::size_t index = 0; index < names.size(); ++index) {
            if (!types[index]) { complete = false; continue; }
            result = numeric_tree_math::combineTypes(result, realTree(*types[index]), true);
            if (names[index] == port) input = types[index];
        }
        const auto type = port == "value" ? result : input ? realTree(*input) : TypeRef::primitive("f64");
        const bool promotion = input && type != *input;
        return {{promotion ? ResolutionStatus::Coercion : ResolutionStatus::Compatible, type,
            promotion ? "math.hypergeometric.real_promotion" : "", promotion ? "Real numeric leaves are promoted to f64." : ""}, complete};
    } catch (const std::exception &error) {
        return rejected(ResolutionStatus::Incompatible, "math.hypergeometric.aggregate_shape", error.what());
    }
}
}
