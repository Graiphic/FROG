#pragma once
#include "frog_native_engine/FunctionPolymorphicResolution.hpp"
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog_native_engine/CanonicalScalarNative.hpp"
#include "frog/semantics/boolean_tree_contracts.hpp"
#include <algorithm>
#include <array>

namespace frog::native_engine {
// Source uses the authoritative authoring type fold, not a second logical
// evaluator. The constexpr identities are usable without linking the Runtime.
namespace boolean_source {
namespace contracts=runtime::numeric_aggregate::boolean_tree;
inline const contracts::Contract *definition(std::string_view id) noexcept {
    if(const auto *found=contracts::contract(id))return found;
    if(const auto *scalar=canonicalScalarNativeContract(id))id=scalar->catalogId;
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    return contracts::catalogContract(id);
}
}
inline std::string_view canonicalBooleanTreeMigrationIdentity(std::string_view id) noexcept {
    const auto *definition=boolean_source::definition(id);
    return definition?definition->id:std::string_view{};
}
inline bool canonicalBooleanTreeShouldUse(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports) noexcept {
    const auto *definition=boolean_source::definition(id);
    if(!definition)return false;
    if(boolean_source::contracts::reduction(definition->kind))return true;
    return std::any_of(ports.begin(),ports.end(),[](const auto &p){
        return !binding::fullTypeEqual(p.type,binding::TypeRef::primitive("bool"));
    });
}
inline bool canonicalBooleanTreeSignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports,
    std::string *message=nullptr,std::string *at=nullptr) noexcept {
    try {
        namespace c=boolean_source::contracts;
        const auto refuse=[&](std::string_view why,std::string_view port={}) {
            if(message)*message=why;if(at)*at=port;return false;
        };
        const auto *contract=c::contract(id);
        if(!contract||ports.size()<2||ports.size()>17)
            return refuse("The logical operation needs its exact versioned contract and bounded connector inventory.");
        const CanonicalNumericAggregatePort *output=nullptr;
        std::array<const CanonicalNumericAggregatePort*,16> inputs{};
        unsigned count=0;
        for(const auto &port:ports) {
            if(!port.input) {
                if(port.id!="result"||output)return refuse("A logical function has exactly one result output.",port.id);
                output=&port;continue;
            }
            if(!port.connected)return refuse("Every retained logical operand requires its actual source.",port.id);
            const auto ordinal=contract->kind==c::Kind::Not?(port.id=="in"?std::optional<std::size_t>{0}:std::nullopt):
                c::reduction(contract->kind)?(port.id=="value"?std::optional<std::size_t>{0}:std::nullopt):canonicalBooleanInputOrdinal(port.id);
            if(!ordinal||*ordinal>=contract->maximumInputs||inputs[*ordinal])
                return refuse("Logical input identities must be unique canonical consecutive connectors.",port.id);
            inputs[*ordinal]=&port;++count;
        }
        if(!output||count<contract->minimumInputs||count>contract->maximumInputs||ports.size()!=count+1)
            return refuse("The logical operation lost its actual contract arity or output.");
        std::vector<std::string> wired;
        for(unsigned i=0;i<count;++i) {
            if(!inputs[i])return refuse("Logical input groups cannot contain gaps.");
            wired.push_back(inputs[i]->id);
        }
        const auto *definition=findFunctionBindingDefinition(contract->catalog);
        if(!definition)return refuse("The logical function has no current authoring definition.");
        for(unsigned i=0;i<count;++i) {
            const auto *declared=findFunctionBindingPort(*definition,inputs[i]->id);
            // The catalogue declares the repeated a/b pattern once. Reuse the
            // same bounded ordinal fallback as the ordinary diagram resolver.
            if(!declared&&definition->maximumInputCount>2&&functionBindingInputPortOrdinal(*definition,inputs[i]->id))
                declared=findFunctionBindingPort(*definition,"a");
            if(!declared)return refuse("A logical connector has no current declaration.",inputs[i]->id);
            const auto expression=binding::parseTypeExpr(declared->typeExpression);
            if(!expression.valid()||!binding::constrain(inputs[i]->type,expression.expression).accepted())
                return refuse("A logical source violates its declared complete input constraint.",inputs[i]->id);
        }
        binding::FunctionPolymorphicOptions node;
        const auto resolve=[&](std::string_view port)->binding::BindingResult {
            for(unsigned i=0;i<count;++i)if(inputs[i]->id==port)
                return {{binding::ResolutionStatus::Compatible,inputs[i]->type,{},{}},true,{}};
            return {{binding::ResolutionStatus::Unresolved,{},"binding.input_missing","No retained source for this input."},false,{}};
        };
        const auto resolved=binding::resolveSharedPolymorphicFunction(node,*definition,"result",resolve,wired);
        return (resolved.resolution.accepted()&&resolved.complete&&
            binding::fullTypeEqual(resolved.resolution.type,output->type))||
            refuse(resolved.resolution.message.empty()?"The complete logical result differs from the current authoring contract.":resolved.resolution.message,"result");
    }catch(...) {
        if(message)*message="Logical Source validation failed without admitting a partial schema.";
        return false;
    }
}
}
