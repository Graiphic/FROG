#include "ExponentialIntegralsBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace exponentialintegrals_binding_detail;
bool isExponentialIntegralsFunction(std::string_view id) noexcept { return !exponentialIntegralsFunctionKey(id).empty(); }

std::string_view exponentialIntegralsFunctionKey(std::string_view id) noexcept {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    constexpr std::string_view prefix = "math.special.exponential_integrals.";
    if (!id.starts_with(prefix)) return {};
    id.remove_prefix(prefix.size());
    return std::find(keys.begin(), keys.end(), id) == keys.end() ? std::string_view{} : id;
}

std::span<const std::string_view> exponentialIntegralsInputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 1> unary{"x"};
    static constexpr std::array<std::string_view, 2> expn{"x", "n"};
    const auto key = exponentialIntegralsFunctionKey(id);
    return key.empty() ? std::span<const std::string_view>{} : key == "expn"
        ? std::span<const std::string_view>(expn) : std::span<const std::string_view>(unary);
}

std::span<const std::string_view> exponentialIntegralsOutputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 1> result{"result"};
    return isExponentialIntegralsFunction(id) ? std::span<const std::string_view>(result) : std::span<const std::string_view>{};
}

ExponentialIntegralsTypeResult resolveExponentialIntegralsPort(std::string_view id, std::string_view port,
    std::span<const ExponentialIntegralsTypeEvidence> evidence) {
    const auto names = exponentialIntegralsInputPortIds(id);
    if (names.empty()) return failure(ResolutionStatus::Incompatible, "math.exponential_integrals.unknown_function", "This Exponential Integrals operation is not registered.");
    if (port != "result" && std::find(names.begin(), names.end(), port) == names.end())
        return failure(ResolutionStatus::Incompatible, "math.exponential_integrals.missing_port", "This operation has no such connector.");
    std::array<const TypeRef *, 2> types{};
    for (const auto &input : evidence) {
        const auto found = std::find(names.begin(), names.end(), input.portId);
        if (found == names.end()) return failure(ResolutionStatus::Incompatible, "math.exponential_integrals.unexpected_input", "An unknown or inactive input was supplied.");
        auto &slot = types[static_cast<std::size_t>(found - names.begin())];
        if (slot) return failure(ResolutionStatus::Incompatible, "math.exponential_integrals.multiple_sources", "A terminal has multiple sources.");
        slot = &input.valueType;
        if (!slot->known()) return failure(ResolutionStatus::Unresolved, "math.exponential_integrals.unresolved_type", "The input type is not established.");
        if (input.portId == "n") {
            if (!exactOrderType(*slot)) return failure(ResolutionStatus::Incompatible, "math.exponential_integrals.integer_required", "En requires a scalar i32 order. Use an explicit conversion for another representation; real orders are never truncated.");
        } else {
            std::size_t count = 0;
            const auto reason = numeric_tree_math::validateSchema(*slot, 0, count, "Exponential Integrals", false);
            if (!reason.empty()) return failure(ResolutionStatus::Incompatible, "math.exponential_integrals.unsupported_type", reason);
        }
    }
    const auto xType = types[0] ? realTree(*types[0]) : TypeRef::primitive("f64");
    const auto type = port == "n" ? TypeRef::primitive("i32") : xType;
    const bool promotion = port == "x" && types[0] && *types[0] != type;
    const bool complete = types[0] && (names.size() == 1 || types[1]);
    return {{promotion ? ResolutionStatus::Coercion : ResolutionStatus::Compatible, type,
        promotion ? "math.exponential_integrals.real_promotion" : "",
        promotion ? "Real numeric leaves are promoted to f64." : ""}, complete};
}
}
