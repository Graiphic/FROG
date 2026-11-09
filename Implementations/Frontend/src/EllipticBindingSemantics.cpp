#include "EllipticBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace elliptic_binding_detail;
bool isEllipticFunction(std::string_view id) noexcept { return !ellipticFunctionKey(id).empty(); }

std::string_view ellipticFunctionKey(std::string_view id) noexcept {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    for (const auto &[identity, key] : identities) if (id == identity) return key;
    return {};
}

std::span<const std::string_view> ellipticInputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 1> complete{"k"};
    static constexpr std::array<std::string_view, 2> incomplete{"k", "a"};
    const auto key = ellipticFunctionKey(id);
    return key.empty() ? std::span<const std::string_view>{} : key.starts_with("incomplete_")
        ? std::span<const std::string_view>(incomplete) : std::span<const std::string_view>(complete);
}

std::span<const std::string_view> ellipticOutputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 1> output{"value"};
    return isEllipticFunction(id) ? std::span<const std::string_view>(output) : std::span<const std::string_view>{};
}

std::optional<double> ellipticOptionalInputDefault(std::string_view id, std::string_view port) noexcept {
    if (port == "a" && ellipticFunctionKey(id).starts_with("incomplete_")) return 0x1.921fb54442d18p+0;
    return {};
}

EllipticTypeResult resolveEllipticPort(std::string_view id, std::string_view port,
    std::span<const EllipticTypeEvidence> evidence) {
    const auto names = ellipticInputPortIds(id);
    if (names.empty()) return rejected(ResolutionStatus::Incompatible, "math.elliptic.unknown_function", "The Elliptic instance is not registered.");
    if (port != "value" && std::find(names.begin(), names.end(), port) == names.end())
        return rejected(ResolutionStatus::Incompatible, "math.elliptic.missing_port", "This Elliptic instance has no such terminal.");
    std::array<const TypeRef *, 2> types{};
    for (const auto &input : evidence) {
        const auto found = std::find(names.begin(), names.end(), input.portId);
        if (found == names.end()) return rejected(ResolutionStatus::Incompatible, "math.elliptic.unexpected_input", "An Elliptic input identity is inactive or unknown.");
        auto &slot = types[static_cast<std::size_t>(found - names.begin())];
        if (slot) return rejected(ResolutionStatus::Incompatible, "math.elliptic.multiple_sources", "An Elliptic terminal has multiple sources.");
        slot = &input.valueType;
        if (!slot->known()) return rejected(ResolutionStatus::Unresolved, "math.elliptic.unresolved_type", "The Elliptic input type is not established.");
        std::size_t count = 0;
        const auto reason = numeric_tree_math::validateSchema(*slot, 0, count, "Elliptic", false);
        if (!reason.empty()) return rejected(ResolutionStatus::Incompatible, "math.elliptic.unsupported_type", reason);
    }
    try {
        TypeRef result = TypeRef::primitive("f64");
        const TypeRef *input = nullptr; bool complete = true;
        for (std::size_t index = 0; index < names.size(); ++index) {
            if (!types[index]) { if (!ellipticOptionalInputDefault(id, names[index])) complete = false; continue; }
            result = numeric_tree_math::combineTypes(result, realTree(*types[index]), true);
            if (names[index] == port) input = types[index];
        }
        const auto type = port == "value" ? result : input ? realTree(*input) : TypeRef::primitive("f64");
        const bool promotion = input && type != *input;
        return {{promotion ? ResolutionStatus::Coercion : ResolutionStatus::Compatible, type,
            promotion ? "math.elliptic.real_promotion" : "", promotion ? "Real numeric leaves are promoted to f64." : ""}, complete};
    } catch (const std::exception &error) { return rejected(ResolutionStatus::Incompatible, "math.elliptic.aggregate_shape", error.what()); }
}
}
