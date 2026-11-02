#pragma once
#include "frog_native_engine/FunctionPolymorphicResolution.hpp"
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog_native_engine/CanonicalScalarNative.hpp"
#include "frog_native_engine/CanonicalScalarSelection.hpp"
#include "frog/semantics/comparison_tree_contracts.hpp"
#include <algorithm>
#include <array>
#include <set>
#include <span>
#include <vector>

namespace frog::native_engine {
// Source-only validation reuses the authoring resolver and immutable contract
// table. Runtime values and their single owned walker enter only after lowering.
namespace comparison_source {
namespace contracts=runtime::numeric_aggregate::comparison_tree;
inline const contracts::Contract *definition(std::string_view id,bool aggregate=false) noexcept {
    if(const auto *found=contracts::contract(id))return found;
    if(const auto *scalar=canonicalScalarNativeContract(id))id=scalar->catalogId;
    if(const auto *selection=canonicalScalarSelectionContract(id))id=selection->catalogId;
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    if(id=="string.empty_string_or_path")id="comparison.empty_string_or_path";
    return contracts::catalogContract(id,aggregate);
}
inline bool hasLegacyCorridor(std::string_view authored) noexcept {
    try {
        const auto *definition=comparison_source::definition(authored);
        if(!definition)return false;
        const auto privateId=std::string("frog.studio.")+std::string(definition->catalog);
        return canonicalScalarSelectionAuthoredContract(privateId)||
            !canonicalScalarMigrationIdentity(privateId).empty();
    }catch(...) {return false;}
}
inline bool plain(const binding::TypeRef &type) {
    return type.kind==binding::TypeKind::Primitive&&
        binding::fullTypeEqual(type,binding::TypeRef::primitive(type.name));
}
}
inline std::string_view canonicalComparisonTreeMigrationIdentity(std::string_view id,bool aggregate=false) noexcept {
    const auto *definition=comparison_source::definition(id,aggregate);
    return definition?definition->id:std::string_view{};
}
// Return an existing exact scalar identity only when EVERY real data input
// belongs to that same complete corridor. This is a routing check, not proof
// of signature validity; full Source/FIR checks follow before execution.
inline std::string_view canonicalComparisonTreeLegacyIdentity(std::string_view authored,
    std::span<const CanonicalNumericAggregatePort> ports,bool aggregate=false) noexcept {
    try {
        namespace c=comparison_source::contracts;
        if(c::contract(authored))return {}; // An explicit new identity stays exact.
        const auto *definition=comparison_source::definition(authored,aggregate);
        if(!definition||definition->aggregate)return {};
        const CanonicalNumericAggregatePort *first=nullptr,*selector=nullptr;
        unsigned count=0;
        for(const auto &port:ports)if(port.input) {
            if(definition->kind==c::Kind::Select&&port.id=="select") {selector=&port;continue;}
            if(!port.connected||!comparison_source::plain(port.type))return {};
            if(first&&!binding::fullTypeEqual(first->type,port.type))return {};
            if(!first)first=&port;
            ++count;
        }
        if(!first)return {};
        const auto privateId=std::string("frog.studio.")+std::string(definition->catalog);
        if(definition->kind==c::Kind::Select) {
            const auto *selection=canonicalScalarSelectionAuthoredContract(privateId);
            if(count!=2||!selector||!selector->connected||
                !binding::fullTypeEqual(selector->type,binding::TypeRef::primitive("bool"))||!selection||
                !canonicalScalarSelectionTypeAccepted(*selection,first->type.name))return {};
            return selection->operationId;
        }
        const auto id=canonicalScalarMigrationIdentityForType(privateId,first->type.name);
        const auto *scalar=canonicalScalarNativeContract(id);
        if(!scalar||count!=scalar->inputCount||!canonicalScalarNativeOperandAccepted(*scalar,first->type.name))return {};
        return id;
    }catch(...) {return {};}
}
inline bool canonicalComparisonTreeShouldUse(std::string_view authored,
    std::span<const CanonicalNumericAggregatePort> ports,bool aggregate=false) noexcept {
    return comparison_source::definition(authored,aggregate)&&
        canonicalComparisonTreeLegacyIdentity(authored,ports,aggregate).empty();
}
inline bool canonicalComparisonTreeSignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports,
    std::string *message=nullptr,std::string *at=nullptr) noexcept {
    const auto refuse=[&](std::string_view why,std::string_view port={}) {
        if(message)*message=why;if(at)*at=port;return false;
    };
    try {
        namespace c=comparison_source::contracts;
        const auto *contract=c::contract(id);
        if(!contract)return refuse("Comparison requires its exact versioned native contract.");
        std::vector<CanonicalNumericAggregatePort> adapted;
        if(id=="frog.studio.comparison.empty_string_or_path.owned.v1"&&std::any_of(ports.begin(),ports.end(),[](const auto &p){return p.input&&p.id=="value";})){
            adapted.assign(ports.begin(),ports.end());for(auto &p:adapted)if(p.input&&p.id=="value")p.id="x";ports=adapted;
        }
        const auto inputCount=std::count_if(contract->inputs.begin(),contract->inputs.end(),[](auto id){return !id.empty();});
        if(ports.size()!=static_cast<std::size_t>(inputCount)+1)
            return refuse("Comparison requires every actual input and its single result connector.");
        std::array<const CanonicalNumericAggregatePort*,3> inputs{};
        const CanonicalNumericAggregatePort *output=nullptr;
        for(const auto &port:ports) {
            if(!port.type.known())return refuse("Comparison lost a complete declared type.",port.id);
            if(!port.input) {
                if(port.id!="result"||output)return refuse("Comparison has exactly one result output.",port.id);
                output=&port;continue;
            }
            const auto found=std::find(contract->inputs.begin(),contract->inputs.end(),port.id);
            if(found==contract->inputs.end()||port.id.empty()||!port.connected)
                return refuse("Comparison requires each declared input with its actual driver.",port.id);
            const auto ordinal=static_cast<std::size_t>(found-contract->inputs.begin());
            if(inputs[ordinal])return refuse("Comparison input identities must be unique.",port.id);
            inputs[ordinal]=&port;
        }
        if(!output)return refuse("Comparison lost its actual result connector.","result");
        const auto *definition=findFunctionBindingDefinition(contract->catalog);
        if(!definition)return refuse("Comparison has no current authoring declaration.");
        std::vector<std::string> wired;
        for(std::size_t i=0;i<contract->inputs.size();++i)if(!contract->inputs[i].empty()) {
            if(!inputs[i])return refuse("Comparison lost a required input.",contract->inputs[i]);
            const auto *declared=findFunctionBindingPort(*definition,contract->inputs[i]);
            const auto expression=declared?binding::parseTypeExpr(declared->typeExpression):binding::TypeExprParseResult{};
            if(!declared||!expression.valid()||!binding::constrain(inputs[i]->type,expression.expression).accepted())
                return refuse("Comparison input violates its current complete type constraint.",contract->inputs[i]);
            wired.emplace_back(contract->inputs[i]);
        }
        if(definition->typeResolution==FunctionTypeResolutionPolicy::Fixed) {
            // Path.is_not_empty has a real fixed Path -> Bool declaration; it
            // must not be sent to a polymorphic resolver that does not own it.
            const auto *declared=findFunctionBindingPort(*definition,"result");
            const auto expected=declared?binding::parseType(declared->defaultValueType):binding::TypeParseResult{};
            return (declared&&expected.valid()&&binding::fullTypeEqual(expected.type,output->type))||
                refuse("The fixed Comparison result differs from its declared complete output type.","result");
        }
        binding::FunctionPolymorphicOptions node{contract->aggregate};
        const auto resolve=[&](std::string_view port)->binding::BindingResult {
            for(const auto *input:inputs)if(input&&input->id==port)
                return {{binding::ResolutionStatus::Compatible,input->type,{},{}},true,{}};
            return {{binding::ResolutionStatus::Unresolved,{},"binding.input_missing","No actual retained source for this connector."},false,{}};
        };
        const auto resolved=binding::resolveSharedPolymorphicFunction(node,*definition,"result",resolve,wired);
        if(!resolved.resolution.accepted()||!resolved.complete)
            return refuse(resolved.resolution.message.empty()?"Comparison did not resolve a complete current result type.":resolved.resolution.message,"result");
        return binding::fullTypeEqual(resolved.resolution.type,output->type)||
            refuse("Comparison result differs from its current mode, complete type, rank or stable fields.","result");
    }catch(...) {return refuse("Comparison Source validation failed without admitting a partial schema.");}
}
}
