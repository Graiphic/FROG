#include "TrigonometricBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace trigonometric_binding_detail;
bool isTrigonometricFunction(std::string_view id) noexcept { return operation(id) != Operation::Unknown; }

std::span<const std::string_view> trigonometricInputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 1> unary{"x"};
    static constexpr std::array<std::string_view, 2> two{"y", "x"};
    const auto op = operation(id);
    if (op == Operation::Unknown) return {};
    return op == Operation::Atan2 ? std::span<const std::string_view>{two} : std::span<const std::string_view>{unary};
}

std::span<const std::string_view> trigonometricOutputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 1> unary{"result"};
    static constexpr std::array<std::string_view, 2> pair{"sin", "cos"};
    const auto op = operation(id);
    if (op == Operation::Unknown) return {};
    return op == Operation::SinCos ? std::span<const std::string_view>{pair} : std::span<const std::string_view>{unary};
}

TrigonometricTypeResult resolveTrigonometricPort(std::string_view id, std::string_view port,
    std::span<const TrigonometricTypeEvidence> inputs) {
    const auto op = operation(id);
    const auto inputNames = trigonometricInputPortIds(id), outputNames = trigonometricOutputPortIds(id);
    if (op == Operation::Unknown)
        return failure(ResolutionStatus::Incompatible, "math.trigonometric.unknown_function", "The Trigonometric function is not registered.");
    const bool output = std::find(outputNames.begin(), outputNames.end(), port) != outputNames.end();
    if (!output && std::find(inputNames.begin(), inputNames.end(), port) == inputNames.end())
        return failure(ResolutionStatus::Incompatible, "math.trigonometric.missing_port", "The Trigonometric function has no such connector.");
    const TypeRef *x = nullptr, *y = nullptr;
    for (const auto &input : inputs) {
        if (std::find(inputNames.begin(), inputNames.end(), input.portId) == inputNames.end())
            return failure(ResolutionStatus::Incompatible, "math.trigonometric.unexpected_input", "The function has no input with this identity.");
        auto &target = input.portId == "x" ? x : y;
        if (target)
            return failure(ResolutionStatus::Incompatible, "math.trigonometric.multiple_sources", "A calculation operand has more than one source.");
        target = &input.valueType;
        std::size_t nodes = 0;
        const auto reason = validateSchema(input.valueType, 0, nodes, "Trigonometric", op != Operation::Atan2);
        if (!reason.empty())
            return failure(reason == "The operand type is not established."
                ? ResolutionStatus::Unresolved : ResolutionStatus::Incompatible,
                "math.trigonometric.unsupported_type", reason);
    }
    const bool complete = x && (op != Operation::Atan2 || y);
    try {
        const auto result = x && y ? combineTypes(*x, *y, true) : x ? resultTree(*x)
            : y ? resultTree(*y) : TypeRef::primitive("f64");
        const auto *supplied = port == "x" ? x : port == "y" ? y : nullptr;
        return {{ResolutionStatus::Compatible, output ? result
            : supplied ? *supplied : TypeRef::primitive("f64"), {}, {}}, complete};
    } catch (const std::exception &error) {
        return failure(ResolutionStatus::Incompatible, "math.trigonometric.aggregate_shape", error.what());
    }
}
}
