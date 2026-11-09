#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

// Unlike optional<T>, this retains the difference between an absent declaration
// and an explicit JSON null (an undecided policy). Neither means false or zero.
template<class T>
struct FunctionContractNullable {
    bool declared = false;
    std::optional<T> value;
};

struct FunctionContractOverload {
    std::string policy;
    std::vector<std::string> acceptedOperands;
    std::string result;
    bool implicitCoercion = false;
    std::optional<std::string> description;
};

struct FunctionContractVariadicity {
    bool enabled = false;
    FunctionContractNullable<std::size_t> inputCount;
    std::optional<std::size_t> minimumInputCount;
    std::optional<std::size_t> maximumInputCount;
    std::optional<std::vector<std::string>> portOrder;
    std::optional<std::string> reduction;
    std::optional<std::string> decisionStatus;
    std::optional<std::string> description;
};

struct FunctionContractArrayPolicy {
    FunctionContractNullable<bool> accepted;
    std::optional<std::vector<std::string>> acceptedOperands;
    std::optional<std::size_t> acceptedRank;
    std::optional<std::string> shapeRule;
    std::optional<bool> scalarLift;
    std::optional<bool> broadcasting;
    std::optional<bool> producesArray;
    std::optional<std::size_t> resultRank;
    std::string lifting;
    std::optional<std::string> emptyArrayIdentity;
    std::optional<std::string> description;
};

struct FunctionContractRuntimeAvailability {
    // An authoring declaration is not a target capability. Execution additionally
    // requires the active ProgramSession to support the operation and its types.
    bool available = false;
    std::string implementationKind;
    std::string implementationId;
    std::optional<std::string> availabilityScope;
    std::optional<std::string> providerRequirement;
    std::optional<std::string> notes;
};

struct FunctionContractPresentation {
    std::string shape;
    std::string extension;
};

struct FunctionPortAliases {
    std::string portId;
    std::vector<std::string> aliases;
};

struct FunctionBindingContract {
    std::string catalogId;
    std::vector<FunctionPortAliases> portAliases;
    std::string profileId;
    std::string contractStatus;
    std::string authorityScope;
    FunctionContractOverload overload;
    FunctionContractVariadicity variadicity;
    FunctionContractArrayPolicy arrayPolicy;
    FunctionContractRuntimeAvailability runtimeAvailability;
    std::string behaviorSummary;
    std::vector<std::string> blockers;
    std::optional<std::vector<std::string>> authorityRefs;
    std::optional<FunctionContractPresentation> presentation;
    std::optional<std::string> optionalInputPattern;
    std::optional<std::size_t> optionalInputFirstIndex;
};

// The complete public Add law is generated separately from the pinned FROG
// authority. Studio presentation and target availability remain separate data.
struct PublishedAddContract {
    std::size_t schemaVersion;
    std::string definitionId;
    std::string catalogId;
    std::string maturity;
    std::vector<std::string> requiredInputs;
    std::string optionalInputPattern;
    std::size_t optionalInputFirstIndex;
    std::string output;
    std::size_t minimumInputCount;
    std::size_t maximumInputCount;
    std::string resolverId;
    std::vector<std::string> acceptedOperands;
    std::string numericLeafPromotion;
    bool scalarToArrayLifting;
    std::string arrayShapeRule;
    bool broadcasting;
    std::string resultShape;
    std::vector<std::string> portOrder;
    std::string reduction;
    std::string coercion;
    std::string diagramBodyStandard;
    std::string extension;
    bool geometryIsNotSemantic;
};

[[nodiscard]] const PublishedAddContract &publishedNumericAddContract() noexcept;
[[nodiscard]] std::span<const FunctionBindingContract> builtInFunctionBindingContracts() noexcept;
[[nodiscard]] const FunctionBindingContract *findFunctionBindingContract(std::string_view catalogId) noexcept;

} // namespace frog::native_engine
