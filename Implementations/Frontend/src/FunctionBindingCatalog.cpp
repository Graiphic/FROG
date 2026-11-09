#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/FunctionPortIdentities.hpp"
#include "frog_native_engine/FunctionBindingContract.hpp"
#include "frog_native_engine/BindingType.hpp"

#include <algorithm>
#include <array>
#include <exception>
#include <unordered_set>
#include <utility>

namespace frog::native_engine {

namespace {

FunctionBindingDefinition makePublishedNumericAddDefinition() {
#include "../manifests/function-bindings/frog.core.add.v1.inc"
}

#include "../manifests/function-bindings/studio.function-bindings.v2.inc"

FunctionBindingRegistry makeBuiltInRegistry() {
    FunctionBindingRegistry registry;
    for (auto &definition : generatedFunctionBindingDefinitions()) {
        const auto *entry = findFunctionCatalogEntry(definition.catalogId);
        if (entry == nullptr || !isFunctionCatalogPlaceable(*entry) ||
            (!entry->publishedPrimitiveType.empty() &&
             entry->publishedPrimitiveType != definition.definitionId)) {
            std::terminate();
        }

        std::vector<FunctionBindingValidationIssue> issues;
        const bool registered = registry.registerDefinition(
            std::move(definition),
            &issues);
        const bool hasError = std::any_of(
            issues.begin(),
            issues.end(),
            [](const FunctionBindingValidationIssue &issue) {
                return issue.severity == FunctionBindingIssueSeverity::Error;
            });
        if (!registered || hasError) {
            std::terminate();
        }
    }
    const auto catalogCount = std::count_if(
        functionCatalogEntries().begin(), functionCatalogEntries().end(),
        [](const FunctionCatalogEntry &entry) { return isFunctionCatalogPlaceable(entry); });
    if (static_cast<std::size_t>(catalogCount) != registry.definitions().size()) {
        std::terminate();
    }
    return registry;
}

} // namespace

#include "../manifests/function-bindings/frog.core.add.authority.v1.inc"

std::span<const FunctionBindingContract> builtInFunctionBindingContracts() noexcept {
    static const auto contracts = generatedFunctionBindingContracts();
    return contracts;
}

const FunctionBindingContract *findFunctionBindingContract(std::string_view catalogId) noexcept {
    // The generated storage is immutable for process lifetime. Index only its
    // addresses: preserve the public order and avoid owning duplicate ID text.
    static const auto contractsById = [] {
        const auto contracts = builtInFunctionBindingContracts();
        std::vector<const FunctionBindingContract *> index;
        index.reserve(contracts.size());
        for (const auto &contract : contracts) index.push_back(&contract);
        std::sort(index.begin(), index.end(), [](const auto *first, const auto *second) {
            const int order = first->catalogId.compare(second->catalogId);
            // Preserve the first-match rule even if a generated ID is repeated.
            return order < 0 || (order == 0 && first < second);
        });
        return index;
    }();
    const auto found = std::lower_bound(contractsById.begin(), contractsById.end(), catalogId,
        [](const auto *contract, std::string_view id) { return contract->catalogId < id; });
    return found != contractsById.end() && (*found)->catalogId == catalogId ? *found : nullptr;
}

std::vector<FunctionBindingValidationIssue> validateFunctionBindingDefinition(
    const FunctionBindingDefinition &definition) {
    std::vector<FunctionBindingValidationIssue> issues;
    const auto addError = [&issues, &definition](
        std::string code,
        std::string portId,
        std::string message) {
        issues.push_back(FunctionBindingValidationIssue{
            FunctionBindingIssueSeverity::Error,
            std::move(code),
            definition.catalogId,
            std::move(portId),
            std::move(message)
        });
    };

    if (definition.catalogId.empty()) {
        addError("missing_catalog_id", {}, "A function definition requires a catalog ID.");
    }
    if (definition.definitionId.empty()) {
        addError("missing_definition_id", {}, "A function definition requires a stable definition ID.");
    }
    if (definition.schemaVersion == 0U) {
        addError("invalid_schema_version", {}, "Schema version zero is not valid.");
    }
    if (definition.typeResolution == FunctionTypeResolutionPolicy::CustomResolver &&
        definition.resolverId.empty()) {
        addError("missing_resolver", {}, "A custom type policy requires a resolver ID.");
    }
    if ((definition.implementationKind == FunctionImplementationKind::BuiltInProvider ||
         definition.implementationKind == FunctionImplementationKind::ExternalProvider) &&
        definition.implementationId.empty()) {
        addError("missing_implementation", {}, "A provider-backed function requires an implementation ID.");
    }
    if (!definition.variadicInputPrefix.empty() &&
        (definition.minimumInputCount == 0U ||
         definition.maximumInputCount < definition.minimumInputCount)) {
        addError("invalid_variadic_range", {}, "The variadic input range is invalid.");
    }
    if (definition.arrayLiftingPolicy != FunctionArrayLiftingPolicy::None &&
        definition.typeResolution != FunctionTypeResolutionPolicy::CustomResolver) {
        addError(
            "array_lifting_requires_resolver",
            {},
            "Array lifting requires an explicit versioned type resolver.");
    }
    if (definition.reductionPolicy != FunctionReductionPolicy::Unspecified &&
        definition.evaluationOrder == FunctionEvaluationOrder::Unspecified) {
        addError(
            "reduction_requires_order",
            {},
            "A reduction policy requires an explicit evaluation order.");
    }

    std::unordered_set<std::string> portIds;
    for (const auto &candidate : definition.ports) {
        if (candidate.portId.empty()) {
            addError("missing_port_id", {}, "Every port requires a stable ID.");
        } else if (!portIds.insert(candidate.portId).second) {
            addError("duplicate_port_id", candidate.portId, "Port IDs must be unique within a function.");
        }
        if (candidate.typeExpression.empty()) {
            addError("missing_type_expression", candidate.portId, "Every declared port requires a type expression.");
        }
        if (candidate.defaultValueType.empty()) {
            addError("missing_default_type", candidate.portId, "Every declared port requires an editor fallback type.");
        }
    }
    return issues;
}

bool FunctionBindingRegistry::registerDefinition(
    FunctionBindingDefinition definition,
    std::vector<FunctionBindingValidationIssue> *issues) {
    auto validation = validateFunctionBindingDefinition(definition);
    if (indexByCatalogId_.contains(definition.catalogId)) {
        validation.push_back(FunctionBindingValidationIssue{
            FunctionBindingIssueSeverity::Error,
            "duplicate_catalog_id",
            definition.catalogId,
            {},
            "A binding definition with this catalog ID is already registered."
        });
    }
    const bool valid = std::none_of(
        validation.begin(),
        validation.end(),
        [](const FunctionBindingValidationIssue &issue) {
            return issue.severity == FunctionBindingIssueSeverity::Error;
        });
    if (issues != nullptr) {
        issues->insert(issues->end(), validation.begin(), validation.end());
    }
    if (!valid) {
        return false;
    }

    const std::size_t index = definitions_.size();
    indexByCatalogId_.emplace(definition.catalogId, index);
    definitions_.push_back(std::move(definition));
    return true;
}

const FunctionBindingDefinition *FunctionBindingRegistry::find(
    std::string_view catalogId) const noexcept {
    const auto found = indexByCatalogId_.find(catalogId);
    return found == indexByCatalogId_.end()
        ? nullptr
        : &definitions_[found->second];
}

std::span<const FunctionBindingDefinition> FunctionBindingRegistry::definitions() const noexcept {
    return definitions_;
}

const FunctionBindingRegistry &builtInFunctionBindingRegistry() noexcept {
    static const FunctionBindingRegistry registry = makeBuiltInRegistry();
    return registry;
}

const FunctionBindingDefinition *findFunctionBindingDefinition(
    std::string_view catalogId) noexcept {
    return builtInFunctionBindingRegistry().find(catalogId);
}

const FunctionPortBinding *findFunctionBindingPort(
    const FunctionBindingDefinition &definition,
    std::string_view portId) noexcept {
    portId = canonicalFunctionBindingPortId(definition.catalogId, portId);
    if(definition.catalogId=="dialog.events.register_for_events" && portId.starts_with("source:"))portId="source";
    if(isClusterFunction(definition.catalogId) && clusterFunctionRow(portId)) portId="element1";
    if (stringPaletteIndex(definition.catalogId,portId)) portId=stringPaletteRepeatedBase(definition.catalogId,portId);
    if (arrayPaletteKind(definition.catalogId)!=ArrayPaletteKind::None) {
        const auto dynamic=arrayPaletteDeclaredPort(definition.catalogId,portId);
        if(!dynamic) return nullptr;
        portId=dynamic->base;
    }
    if (definition.catalogId == "array.build" && buildArrayInputIndex(portId)) portId = "array_a";
    if (definition.catalogId == "array.reshape" && reshapeArrayDimensionIndex(portId)) portId = "dimension_size";
    if (definition.catalogId == "array.index") {
        if (const auto dynamic = parseIndexArrayPort(portId))
            portId = dynamic->isOutput ? "element" : "index";
    }
    if (const auto kind = arrayEditKind(definition.catalogId); kind != ArrayEditKind::None) {
        const auto dynamic = parseArrayEditPort(portId);
        if (!dynamic || !arrayEditPortAllowed(kind, *dynamic)) return nullptr;
        portId = arrayEditBasePort(dynamic->kind);
    }
    const auto found = std::find_if(
        definition.ports.begin(),
        definition.ports.end(),
        [portId](const FunctionPortBinding &candidate) {
            return candidate.portId == portId;
        });
    return found == definition.ports.end() ? nullptr : &*found;
}

std::string_view canonicalFunctionBindingPortId(
    std::string_view catalogId,
    std::string_view portId) noexcept {
    if (catalogId == "array.build" && portId == "input_2") return "array_b";
    for (const auto &alias : kGeneratedFunctionPortAliases) {
        if (alias.catalogId == catalogId && alias.alias == portId) {
            return alias.portId;
        }
    }
    return portId;
}

std::optional<std::string_view> functionDefaultPortValueType(
    std::string_view catalogId,
    std::string_view portId) noexcept {
    const auto *definition = findFunctionBindingDefinition(catalogId);
    if (definition == nullptr) {
        return std::nullopt;
    }
    const auto *binding = findFunctionBindingPort(*definition, portId);
    if (binding == nullptr || binding->defaultValueType.empty()) {
        return std::nullopt;
    }
    return binding->defaultValueType;
}

bool functionPortAcceptsValueType(
    const FunctionPortBinding &binding,
    std::string_view valueType) {
    if (binding.direction != FunctionPortDirection::Input || valueType.empty()) {
        return false;
    }
    const auto source = frog::native_engine::binding::parseType(valueType);
    if (!source.valid() || !source.type.known()) return false;
    const auto numericArray = [&] {
        const auto *leaf = &source.type;
        if (leaf->kind != frog::native_engine::binding::TypeKind::Array) return false;
        while (leaf->kind == frog::native_engine::binding::TypeKind::Array) leaf = &leaf->children.front();
        return leaf->numeric();
    };
    switch (binding.connectionPolicy) {
    case FunctionPortConnectionPolicy::Exact:
        return frog::native_engine::binding::connect(source.type,
            frog::native_engine::binding::parseType(binding.defaultValueType).type).accepted();
    case FunctionPortConnectionPolicy::NumericImplicit:
    case FunctionPortConnectionPolicy::NumericExplicit:
        return source.type.numeric();
    case FunctionPortConnectionPolicy::NumericArrayImplicit: {
        return numericArray();
    }
    case FunctionPortConnectionPolicy::NumericScalarOrArrayImplicit: {
        return source.type.numeric() || numericArray();
    }
    case FunctionPortConnectionPolicy::IntegerExplicit:
        return source.type.integer();
    case FunctionPortConnectionPolicy::AnyExplicit: {
        const auto expression = frog::native_engine::binding::parseTypeExpr(binding.typeExpression);
        // Concrete array/cluster constraints are not permission to accept every
        // scalar. same(port) still needs the graph's cross-input resolver.
        if (expression.valid() && expression.expression.kind !=
                frog::native_engine::binding::ConstraintKind::SameAs)
            return frog::native_engine::binding::constrain(source.type, expression.expression).accepted();
        return true; // Preserve legacy, separately resolved expression forms.
    }
    }
    return false;
}

bool functionBindingHasUsablePorts(
    const FunctionBindingDefinition &definition) noexcept {
    return definition.maturity != FunctionBindingMaturity::CatalogOnly &&
        !definition.ports.empty();
}

std::string_view functionBindingMaturityName(
    FunctionBindingMaturity maturity) noexcept {
    switch (maturity) {
    case FunctionBindingMaturity::CatalogOnly:
        return "catalog_only";
    case FunctionBindingMaturity::EditorManaged:
        return "editor_managed";
    case FunctionBindingMaturity::Draft:
        return "draft";
    case FunctionBindingMaturity::ReferenceVerified:
        return "reference_verified";
    case FunctionBindingMaturity::PublishedFrog:
        return "published_frog";
    }
    return "unknown";
}

std::string_view functionTypeResolutionPolicyName(
    FunctionTypeResolutionPolicy policy) noexcept {
    switch (policy) {
    case FunctionTypeResolutionPolicy::Fixed:
        return "fixed";
    case FunctionTypeResolutionPolicy::NumericPromotion:
        return "numeric_promotion";
    case FunctionTypeResolutionPolicy::PassThrough:
        return "pass_through";
    case FunctionTypeResolutionPolicy::ExplicitConversion:
        return "explicit_conversion";
    case FunctionTypeResolutionPolicy::CustomResolver:
        return "custom_resolver";
    }
    return "unknown";
}

std::string_view functionImplementationKindName(
    FunctionImplementationKind kind) noexcept {
    switch (kind) {
    case FunctionImplementationKind::None:
        return "none";
    case FunctionImplementationKind::EditorManaged:
        return "editor_managed";
    case FunctionImplementationKind::BuiltInProvider:
        return "built_in_provider";
    case FunctionImplementationKind::ExternalProvider:
        return "external_provider";
    }
    return "unknown";
}

std::string_view functionEvaluationOrderName(
    FunctionEvaluationOrder order) noexcept {
    switch (order) {
    case FunctionEvaluationOrder::Unspecified:
        return "unspecified";
    case FunctionEvaluationOrder::CanonicalPortOrder:
        return "canonical_port_order";
    }
    return "unknown";
}

std::string_view functionReductionPolicyName(
    FunctionReductionPolicy policy) noexcept {
    switch (policy) {
    case FunctionReductionPolicy::Unspecified:
        return "unspecified";
    case FunctionReductionPolicy::CanonicalLeftFold:
        return "canonical_left_fold";
    }
    return "unknown";
}

std::string_view functionArrayLiftingPolicyName(
    FunctionArrayLiftingPolicy policy) noexcept {
    switch (policy) {
    case FunctionArrayLiftingPolicy::None:
        return "none";
    case FunctionArrayLiftingPolicy::ElementWiseSameShapeWithScalarLift:
        return "element_wise_same_shape_with_scalar_lift";
    }
    return "unknown";
}

} // namespace frog::native_engine
