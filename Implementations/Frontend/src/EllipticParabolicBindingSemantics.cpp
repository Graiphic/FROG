#include "EllipticParabolicBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace ellipticparabolic_binding_detail;
bool isEllipticParabolicFunction(std::string_view id) noexcept { return !ellipticParabolicFunctionKey(id).empty(); }

std::string_view ellipticParabolicFunctionKey(std::string_view id) noexcept {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    constexpr std::string_view prefix = "math.special.elliptic_parabolic_functions.";
    if (!id.starts_with(prefix)) return {};
    id.remove_prefix(prefix.size());
    return id == "jacobi_elliptic" || id == "parabolic_cylinder" ? id : std::string_view{};
}

std::span<const std::string_view> ellipticParabolicInputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 2> jacobi{"argument", "parameter"}, cylinder{"argument", "order"};
    const auto key = ellipticParabolicFunctionKey(id);
    return key.empty() ? std::span<const std::string_view>{} : key == "jacobi_elliptic" ?
        std::span<const std::string_view>(jacobi) : std::span<const std::string_view>(cylinder);
}

std::span<const std::string_view> ellipticParabolicOutputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 4> jacobi{"cn", "dn", "sn", "amplitude"};
    static constexpr std::array<std::string_view, 1> cylinder{"value"};
    const auto key = ellipticParabolicFunctionKey(id);
    return key.empty() ? std::span<const std::string_view>{} : key == "jacobi_elliptic" ?
        std::span<const std::string_view>(jacobi) : std::span<const std::string_view>(cylinder);
}

EllipticParabolicTypeResult resolveEllipticParabolicPort(std::string_view id, std::string_view port,
    std::span<const EllipticParabolicTypeEvidence> evidence) {
    const auto inputs = ellipticParabolicInputPortIds(id), outputs = ellipticParabolicOutputPortIds(id);
    if (inputs.empty()) return rejected(ResolutionStatus::Incompatible, "unknown_function", "This Elliptic & Parabolic operation is not registered.");
    const auto inputPosition = std::find(inputs.begin(), inputs.end(), port);
    if (inputPosition == inputs.end() && std::find(outputs.begin(), outputs.end(), port) == outputs.end())
        return rejected(ResolutionStatus::Incompatible, "missing_port", "This operation has no such connector.");
    std::array<const TypeRef *, 2> types{};
    for (const auto &item : evidence) {
        const auto found = std::find(inputs.begin(), inputs.end(), item.portId);
        if (found == inputs.end()) return rejected(ResolutionStatus::Incompatible, "unexpected_input", "An input identity is unknown.");
        auto &slot = types[static_cast<std::size_t>(found - inputs.begin())];
        if (slot) return rejected(ResolutionStatus::Incompatible, "multiple_sources", "A terminal has multiple sources.");
        slot = &item.valueType;
        if (!slot->known()) return rejected(ResolutionStatus::Unresolved, "unresolved_type", "The input type is not established.");
        std::size_t count = 0;
        const auto reason = numeric_tree_math::validateSchema(*slot, 0, count, "Elliptic & Parabolic", false);
        if (!reason.empty()) return rejected(ResolutionStatus::Incompatible, "unsupported_type", reason);
    }
    try {
        TypeRef output = TypeRef::primitive("f64");
        for (const auto *type : types) if (type) output = numeric_tree_math::combineTypes(output, realTree(*type), true);
        const auto *input = inputPosition == inputs.end() ? nullptr : types[static_cast<std::size_t>(inputPosition - inputs.begin())];
        const auto type = inputPosition == inputs.end() ? output : input ? realTree(*input) : TypeRef::primitive("f64");
        const bool promotion = input && type != *input;
        return {{promotion ? ResolutionStatus::Coercion : ResolutionStatus::Compatible, type,
            promotion ? "math.elliptic_parabolic.real_promotion" : "", promotion ? "Real numeric leaves are promoted to f64." : ""}, types[0] && types[1]};
    } catch (const std::exception &error) {
        return rejected(ResolutionStatus::Incompatible, "aggregate_shape", error.what());
    }
}
}
