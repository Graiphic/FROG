#pragma once

#include "frog_native_engine/FunctionCatalog.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace frog::native_engine {

enum class FunctionBindingMaturity {
    CatalogOnly,
    EditorManaged,
    Draft,
    ReferenceVerified,
    PublishedFrog
};

enum class FunctionPortRequirement {
    Required,
    Recommended,
    Optional
};

enum class FunctionPortCardinality {
    Single,
    Variadic
};

enum class FunctionPortConnectionPolicy {
    Exact,
    NumericImplicit,
    NumericArrayImplicit,
    NumericScalarOrArrayImplicit,
    NumericExplicit,
    IntegerExplicit,
    AnyExplicit
};

enum class FunctionTypeResolutionPolicy {
    Fixed,
    NumericPromotion,
    PassThrough,
    ExplicitConversion,
    CustomResolver
};

enum class FunctionImplementationKind {
    None,
    EditorManaged,
    BuiltInProvider,
    ExternalProvider
};

enum class FunctionEvaluationOrder {
    Unspecified,
    CanonicalPortOrder
};

enum class FunctionReductionPolicy {
    Unspecified,
    CanonicalLeftFold
};

enum class FunctionArrayLiftingPolicy {
    None,
    ElementWiseSameShapeWithScalarLift
};

struct FunctionPortBinding {
    std::string portId;
    std::string displayName;
    FunctionPortDirection direction = FunctionPortDirection::Input;
    std::string typeExpression;
    std::string defaultValueType;
    FunctionPortRequirement requirement = FunctionPortRequirement::Required;
    FunctionPortCardinality cardinality = FunctionPortCardinality::Single;
    FunctionPortConnectionPolicy connectionPolicy =
        FunctionPortConnectionPolicy::Exact;
};

struct FunctionBindingDefinition {
    std::string catalogId;
    std::string definitionId;
    std::uint32_t schemaVersion = 1U;
    FunctionBindingMaturity maturity = FunctionBindingMaturity::CatalogOnly;
    FunctionTypeResolutionPolicy typeResolution =
        FunctionTypeResolutionPolicy::Fixed;
    std::string resolverId;
    FunctionImplementationKind implementationKind =
        FunctionImplementationKind::None;
    std::string implementationId;
    std::string providerRequirement;
    std::string documentationUrl;
    std::string variadicInputPrefix;
    std::size_t minimumInputCount = 0U;
    std::size_t maximumInputCount = 0U;
    FunctionEvaluationOrder evaluationOrder =
        FunctionEvaluationOrder::Unspecified;
    FunctionReductionPolicy reductionPolicy =
        FunctionReductionPolicy::Unspecified;
    FunctionArrayLiftingPolicy arrayLiftingPolicy =
        FunctionArrayLiftingPolicy::None;
    std::vector<FunctionPortBinding> ports;
};

enum class FunctionTypeResolutionStatus {
    Resolved,
    NoOperands,
    InvalidOperand,
    IncompatibleArrayShape
};

struct FunctionTypeResolutionResult {
    FunctionTypeResolutionStatus status =
        FunctionTypeResolutionStatus::NoOperands;
    std::string resolvedValueType;
    std::size_t operandIndex = 0U;

    [[nodiscard]] bool resolved() const noexcept {
        return status == FunctionTypeResolutionStatus::Resolved;
    }
};

enum class FunctionBindingIssueSeverity {
    Warning,
    Error
};

struct FunctionBindingValidationIssue {
    FunctionBindingIssueSeverity severity = FunctionBindingIssueSeverity::Error;
    std::string code;
    std::string catalogId;
    std::string portId;
    std::string message;
};

class FunctionBindingRegistry final {
public:
    [[nodiscard]] bool registerDefinition(
        FunctionBindingDefinition definition,
        std::vector<FunctionBindingValidationIssue> *issues = nullptr);

    [[nodiscard]] const FunctionBindingDefinition *find(
        std::string_view catalogId) const noexcept;

    [[nodiscard]] std::span<const FunctionBindingDefinition> definitions() const noexcept;

private:
    struct CatalogIdHash {
        using is_transparent = void;

        [[nodiscard]] std::size_t operator()(std::string_view value) const noexcept {
            return std::hash<std::string_view>{}(value);
        }
    };

    std::vector<FunctionBindingDefinition> definitions_;
    // Own the keys so registry copies and definition-vector growth remain safe.
    std::unordered_map<std::string, std::size_t, CatalogIdHash, std::equal_to<>>
        indexByCatalogId_;
};

[[nodiscard]] std::vector<FunctionBindingValidationIssue>
validateFunctionBindingDefinition(const FunctionBindingDefinition &definition);

[[nodiscard]] const FunctionBindingRegistry &builtInFunctionBindingRegistry() noexcept;

[[nodiscard]] const FunctionBindingDefinition *findFunctionBindingDefinition(
    std::string_view catalogId) noexcept;

[[nodiscard]] const FunctionPortBinding *findFunctionBindingPort(
    const FunctionBindingDefinition &definition,
    std::string_view portId) noexcept;

// Returns the stable port id written by current documents. Legacy aliases are
// accepted only to migrate already-authored diagrams; they are never emitted.
[[nodiscard]] std::string_view canonicalFunctionBindingPortId(
    std::string_view catalogId,
    std::string_view portId) noexcept;

[[nodiscard]] std::optional<std::string_view> functionDefaultPortValueType(
    std::string_view catalogId,
    std::string_view portId) noexcept;

[[nodiscard]] bool functionPortAcceptsValueType(
    const FunctionPortBinding &port,
    std::string_view valueType);

[[nodiscard]] bool functionBindingHasUsablePorts(
    const FunctionBindingDefinition &definition) noexcept;

[[nodiscard]] FunctionTypeResolutionResult resolveNumericAddValueTypes(
    std::span<const std::string_view> operandTypes);

[[nodiscard]] std::optional<std::size_t> functionBindingInputPortOrdinal(
    const FunctionBindingDefinition &definition,
    std::string_view portId) noexcept;

[[nodiscard]] std::string_view functionBindingMaturityName(
    FunctionBindingMaturity maturity) noexcept;

[[nodiscard]] std::string_view functionTypeResolutionPolicyName(
    FunctionTypeResolutionPolicy policy) noexcept;

[[nodiscard]] std::string_view functionImplementationKindName(
    FunctionImplementationKind kind) noexcept;

[[nodiscard]] std::string_view functionEvaluationOrderName(
    FunctionEvaluationOrder order) noexcept;

[[nodiscard]] std::string_view functionReductionPolicyName(
    FunctionReductionPolicy policy) noexcept;

[[nodiscard]] std::string_view functionArrayLiftingPolicyName(
    FunctionArrayLiftingPolicy policy) noexcept;

} // namespace frog::native_engine
