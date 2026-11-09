#pragma once
// Pure Source schema validation only. Values always use the existing one
// numeric_tree walker and complex_numeric leaves after typed lowering.
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog/semantics/complex_numeric_contracts.hpp"
#include <functional>
namespace frog::native_engine {
inline const runtime::complex_numeric::Contract *canonicalNumericComplexContract(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    return runtime::complex_numeric::contract(id);
}
inline bool canonicalNumericComplexSignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports,std::string *message=nullptr) noexcept {
    const auto reject=[&](std::string_view why){if(message)*message=why;return false;};
    try {
        const auto *contract=canonicalNumericComplexContract(id);if(!contract)return reject("Unknown Numeric complex operation identity.");
        if(ports.size()!=contract->inputs.size()+contract->outputs.size())return reject("Complex operation lost its actual semantic connector count.");
        using Kind=binding::TypeKind;
        std::size_t visited=0;std::function<bool(std::span<const CanonicalNumericAggregatePort>,unsigned)> check;
        check=[&](std::span<const CanonicalNumericAggregatePort> current,unsigned depth) {
            if(depth>32||++visited>4096)return reject("Complex Source schema exceeds its bounded depth or node count.");
            const binding::TypeRef *shape=nullptr;
            for(const auto &p:current)if(p.input&&p.type.kind==Kind::Array){shape=&p.type;break;}
            if(!shape)for(const auto &p:current)if(p.input&&p.type.kind==Kind::Cluster){shape=&p.type;break;}
            for(const auto &p:current)if(!p.type.known()||(p.input&&!p.connected))return reject("Complex Source requires complete actual types and connected operands.");
            if(!shape) {
                std::vector<sdk::runtime::ValueType> inputs,outputs;
                const auto collect=[&](auto ids,bool input,auto &target){for(const auto name:ids){
                    const auto found=std::find_if(current.begin(),current.end(),[&](const auto &p){return p.id==name&&p.input==input;});
                    if(found==current.end()||found->type.kind!=Kind::Primitive||
                       !binding::fullTypeEqual(found->type,binding::TypeRef::primitive(found->type.name)))return false;
                    target.push_back(sdk::runtime::valueTypeFromCanonicalName(found->type.name));
                }return true;};
                if(!collect(contract->inputs,true,inputs)||!collect(contract->outputs,false,outputs))return reject("Complex leaves require their exact semantic IDs and complete plain schemas.");
                return runtime::complex_numeric::signatureValid(contract->operationId,inputs,outputs,message);
            }
            if(shape->children.empty()||(shape->kind==Kind::Array&&(shape->rank<1||shape->rank>16||shape->children.size()!=1||shape->children.front().kind==Kind::Array)))
                return reject("Complex trees require bounded-rank Arrays or nonempty Clusters admitted by current authoring.");
            for(const auto &p:current) {
                if(!p.input) {
                    if(p.type.kind!=shape->kind)return reject("Every complex output must retain its actual input aggregate structure.");
                    if(shape->kind==Kind::Array&&(p.type.rank!=shape->rank||p.type.children.size()!=1))return reject("Complex output Array rank differs.");
                    if(shape->kind==Kind::Cluster&&(p.type.fieldIds!=shape->fieldIds||p.type.children.size()!=shape->children.size()||p.type.fieldNames!=shape->fieldNames))
                        return reject("Complex output Cluster must retain stable fields, order and display names.");
                }else if(p.type.kind==shape->kind) {
                    if(shape->kind==Kind::Array&&(p.type.rank!=shape->rank||p.type.children.size()!=1))return reject("Complex input Array ranks differ.");
                    if(shape->kind==Kind::Cluster&&(p.type.fieldIds!=shape->fieldIds||p.type.children.size()!=shape->children.size()))return reject("Complex input Cluster stable fields differ.");
                }else if(shape->kind==Kind::Cluster&&p.type.kind!=Kind::Primitive)return reject("Only a primitive scalar can broadcast over a complex Cluster.");
            }
            const auto count=shape->kind==Kind::Array?std::size_t{1}:shape->children.size();
            for(std::size_t i=0;i<count;++i){std::vector<CanonicalNumericAggregatePort> children;children.reserve(current.size());
                for(const auto &p:current){auto child=p;if(p.type.kind==shape->kind)child.type=p.type.children.at(i);children.push_back(std::move(child));}
                if(!check(children,depth+1))return false;
            }
            return true;
        };
        return check(ports,0);
    }catch(...){return reject("Complex Source validation failed without admitting an erased schema.");}
}
}
