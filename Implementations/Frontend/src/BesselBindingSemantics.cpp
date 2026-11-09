#include "BesselBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace bessel_binding_detail;
bool isBesselFunction(std::string_view id) noexcept { return !besselFunctionKey(id).empty(); }

std::string_view besselFunctionKey(std::string_view id) noexcept {
    constexpr std::string_view prefix = "frog.studio.";
    if (id.starts_with(prefix)) id.remove_prefix(prefix.size());
    constexpr std::string_view family = "math.special.";
    if (!id.starts_with(family)) return {};
    id.remove_prefix(family.size());
    return std::find(keys.begin(), keys.end(), id) != keys.end() ? id : std::string_view{};
}

std::span<const std::string_view> besselInputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 1> x{"x"};
    static constexpr std::array<std::string_view, 2> xv{"x", "v"}, xn{"x", "n"};
    static constexpr std::array<std::string_view, 3> xvt{"x", "v", "type"}, xnt{"x", "n", "type"};
    const auto key = besselFunctionKey(id);
    if (key.empty()) return {};
    if (key == "airy_ai_bi") return x;
    if (key == "hankel_cyl") return xvt;
    if (key == "hankel_sph") return xnt;
    return integerOrder(key) ? std::span<const std::string_view>(xn) : std::span<const std::string_view>(xv);
}

std::span<const std::string_view> besselOutputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 1> value{"value"};
    static constexpr std::array<std::string_view, 2> airy{"ai", "bi"};
    const auto key = besselFunctionKey(id);
    return key.empty() ? std::span<const std::string_view>{} : key == "airy_ai_bi" ?
        std::span<const std::string_view>(airy) : std::span<const std::string_view>(value);
}

BesselTypeResult resolveBesselPort(std::string_view id, std::string_view port,
    std::span<const BesselTypeEvidence> inputs) {
    const auto key = besselFunctionKey(id);
    const auto names = besselInputPortIds(id), outputs = besselOutputPortIds(id);
    if (key.empty()) return failure(ResolutionStatus::Incompatible, "math.bessel.unknown_function", "The Bessel function is not registered.");
    const auto isInput = std::find(names.begin(), names.end(), port) != names.end();
    if (!isInput && std::find(outputs.begin(), outputs.end(), port) == outputs.end())
        return failure(ResolutionStatus::Incompatible, "math.bessel.missing_port", "The Bessel function has no such terminal.");
    std::array<const TypeRef *, 3> evidence{};
    for (const auto &input : inputs) {
        const auto found = std::find(names.begin(), names.end(), input.portId);
        if (found == names.end()) return failure(ResolutionStatus::Incompatible, "math.bessel.unexpected_input", "The Bessel function received an unknown input.");
        auto &slot = evidence[static_cast<std::size_t>(found - names.begin())];
        if (slot) return failure(ResolutionStatus::Incompatible, "math.bessel.multiple_sources", "A Bessel terminal has multiple sources.");
        slot = &input.valueType;
        if (!slot->known()) return failure(ResolutionStatus::Unresolved, "math.bessel.unresolved_type", "The Bessel input type is not established.");
        if (input.portId == "n" || input.portId == "type") {
            if (!integerScalar(*slot) || slot->name != "i32") return failure(ResolutionStatus::Incompatible, "math.bessel.integer_required", "Bessel n and type require i32 terminals; use an explicit conversion for another representation.");
        } else {
            std::size_t count = 0;
            const auto reason = numeric_tree_math::validateSchema(*slot, 0, count, "Bessel", false);
            if (!reason.empty() || (input.portId != "x" && numeric_tree_math::aggregate(*slot)))
                return failure(ResolutionStatus::Incompatible, "math.bessel.unsupported_type", reason.empty() ? "Bessel order v must be a real scalar; only x supports the FROG aggregate extension." : reason);
        }
    }
    const auto x = evidence[0] ? *evidence[0] : TypeRef::primitive("f64");
    TypeRef type;
    if (!isInput) type = resultTree(x, complexResult(key));
    else if (port == "x") type = x;
    else type = TypeRef::primitive(port == "v" ? "f64" : "i32");
    return {{ResolutionStatus::Compatible, std::move(type), {}, {}}, evidence[0] != nullptr};
}
}
