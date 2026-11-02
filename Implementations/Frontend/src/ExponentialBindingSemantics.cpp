#include "ExponentialBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace exponential_binding_detail;
bool isExponentialFunction(std::string_view id) noexcept { return operation(id) != Operation::Unknown; }

std::span<const std::string_view> exponentialInputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view, 1> unary{"x"};
    static constexpr std::array<std::string_view, 2> two{"y", "x"};
    const auto op = operation(id);
    if (op == Operation::Unknown) return {};
    return binary(op) ? std::span<const std::string_view>(two) : std::span<const std::string_view>(unary);
}

ExponentialTypeResult resolveExponentialPort(std::string_view id, std::string_view port,
    std::span<const ExponentialTypeEvidence> inputs) {
    const auto op = operation(id);
    const auto names = exponentialInputPortIds(id);
    if (op == Operation::Unknown) return failure(ResolutionStatus::Incompatible, "math.exponential.unknown_function", "The Exponential function is not registered.");
    if (port != "result" && std::find(names.begin(), names.end(), port) == names.end())
        return failure(ResolutionStatus::Incompatible, "math.exponential.missing_port", "The Exponential function has no such connector.");
    const TypeRef *x = nullptr, *y = nullptr;
    bool complete = true;
    for (const auto &input : inputs) {
        if (std::find(names.begin(), names.end(), input.portId) == names.end())
            return failure(ResolutionStatus::Incompatible, "math.exponential.unexpected_input", "The function has no input with this identity.");
        auto &target = input.portId == "x" ? x : y;
        if (target) return failure(ResolutionStatus::Incompatible, "math.exponential.multiple_sources", "A calculation operand has more than one source.");
        target = &input.valueType;
        if (input.valueType.kind == TypeKind::Matrix) {
            if (input.portId != "x" || (op != Operation::Exp && op != Operation::Ln && op != Operation::Power) ||
                !input.valueType.known())
                return failure(ResolutionStatus::Incompatible, "math.exponential.matrix_operand",
                    "Only the x operand of Exponential, Natural Logarithm and Power accepts the concrete matrix<f64> profile.");
            continue;
        }
        std::size_t nodes = 0;
        const auto reason = validateSchema(input.valueType, 0, nodes);
        if (!reason.empty()) return failure(reason == "The operand type is not established." ?
            ResolutionStatus::Unresolved : ResolutionStatus::Incompatible, "math.exponential.unsupported_type", reason);
    }
    complete = x && (!binary(op) || y);
    if (x && x->kind == TypeKind::Matrix) {
        if (y && (!supportedLeaf(*y) || complexType(*y) || y->rank != 0 || y->extent ||
                  !y->children.empty() || !y->fieldIds.empty() || !y->fieldNames.empty()))
            return failure(ResolutionStatus::Incompatible, "math.exponential.matrix_exponent",
                "Matrix Power requires a real numeric scalar exponent; it never lifts an Array or Cluster exponent.");
        const auto matrix = TypeRef::matrix(TypeRef::primitive("f64"));
        return {{ResolutionStatus::Compatible, port == "result" || port == "x" ? matrix :
            y ? *y : TypeRef::primitive("f64"), {}, {}}, complete};
    }
    try {
        const auto result = x && y ? combineTypes(*x, *y, true) :
            x ? resultTree(*x) : y ? resultTree(*y) : TypeRef::primitive("f64");
        const auto *supplied = port == "x" ? x : port == "y" ? y : nullptr;
        return {{ResolutionStatus::Compatible, port == "result" ? result :
            supplied ? *supplied : TypeRef::primitive("f64"), {}, {}}, complete};
    } catch (const std::exception &error) {
        return failure(ResolutionStatus::Incompatible, "math.exponential.aggregate_shape", error.what());
    }
}
}
