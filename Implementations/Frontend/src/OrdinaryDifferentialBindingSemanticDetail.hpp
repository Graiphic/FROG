#pragma once
#include "frog_native_engine/OrdinaryDifferentialBinding.hpp"
#include "NumericTreeSchema.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>
#include <limits>
#include <map>
#include <stdexcept>
#include <utility>
namespace frog::native_engine::ordinarydifferential_binding_detail {
using binding::TypeRef; using binding::TypeKind; using binding::ResolutionStatus; using T=binding::TypeRef;
using namespace numeric_tree_math;
namespace {
#include "../snapshot/generated/ordinary-differential-runtime.generated.inc"

const FunctionBindingDefinition* ordinaryRuntimeDefinition(std::string_view id){
    static const auto definitions=ordinaryDifferentialRuntimeDefinitions();
    const auto found=std::find_if(definitions.begin(),definitions.end(),[id](const auto& definition){return definition.catalogId==id;});
    return found==definitions.end()?nullptr:&*found;
}
const FunctionPortBinding* ordinaryRuntimePort(const FunctionBindingDefinition& definition,std::string_view id){
    const auto found=std::find_if(definition.ports.begin(),definition.ports.end(),[id](const auto& port){return port.portId==id;});
    return found==definition.ports.end()?nullptr:&*found;
}
T type(std::string_view s){return binding::parseType(s).type;}
bool callableCompatible(const T& actual,const T& expected){if(actual.kind!=binding::TypeKind::Reference||actual.name!=expected.name||actual.children.size()!=1||expected.children.size()!=1)return false;const auto &a=actual.children[0],&e=expected.children[0];if(a.kind!=binding::TypeKind::Cluster||a.fieldIds.size()!=a.children.size()||a.children.size()!=e.children.size())return false;for(std::size_t j=0;j<e.children.size();++j){const auto p=std::find(a.fieldIds.begin(),a.fieldIds.end(),e.fieldIds[j]);if(p==a.fieldIds.end()||a.children[static_cast<std::size_t>(p-a.fieldIds.begin())]!=e.children[j])return false;}return true;}
}
}
