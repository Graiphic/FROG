#include "frog_native_engine/CalculusNodeBinding.hpp"
#include <array>
#include <map>

namespace frog::native_engine {
namespace {
constexpr std::array<std::string_view,3> referenceOperations{
    "frog.studio.math.calculus.function_reference.1d.owned.v1",
    "frog.studio.math.calculus.function_reference.2d.owned.v1",
    "frog.studio.math.calculus.function_reference.3d.owned.v1"};
constexpr std::array<std::string_view,3> referenceSchemas{
    "ref<connectivity.document.strict,cluster<in_x:array<f64>,in_data:variant,out_value:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>",
    "ref<connectivity.document.strict,cluster<in_x:array<f64>,in_y:array<f64>,in_data:variant,out_value:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>",
    "ref<connectivity.document.strict,cluster<in_x:array<f64>,in_y:array<f64>,in_z:array<f64>,in_data:variant,out_value:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>"};
}

std::string_view calculusFunctionReferenceOperation(unsigned dimensions) noexcept {
    return dimensions>=1&&dimensions<=referenceOperations.size()?referenceOperations[dimensions-1]:std::string_view{};
}
std::string_view calculusFunctionReferenceSchema(unsigned dimensions) noexcept {
    return dimensions>=1&&dimensions<=referenceSchemas.size()?referenceSchemas[dimensions-1]:std::string_view{};
}
unsigned calculusFunctionReferenceDimensions(std::string_view operation) noexcept {
    for(unsigned i=0;i<referenceOperations.size();++i)if(referenceOperations[i]==operation)return i+1;
    return 0;
}

const FunctionBindingDefinition *calculusBindingDefinition(std::string_view id,std::string_view variant) noexcept {
    const auto operation=calculusOperationId(id,variant);
    if(operation.empty())return nullptr;
    static const auto definitions=[] {
        std::map<std::string,FunctionBindingDefinition,std::less<>> result;
        for(const auto &function:calculusFunctions()) {
            const auto *base=findFunctionBindingDefinition(function.catalogId);
            if(!base)continue;
            for(const auto &choice:calculusVariants(function.catalogId)) {
                auto definition=*base;
                definition.implementationId=std::string(choice.operationId);
                definition.ports.clear();
                for(const bool input:{true,false}) {
                    const auto ids=input?calculusInputPortIds(choice.operationId):calculusOutputPortIds(choice.operationId);
                    for(const auto port:ids) {
                        const auto resolved=resolveCalculusPort(choice.operationId,port);
                        if(!resolved.resolution.accepted()||!resolved.resolution.type.known())continue;
                        const auto &type=resolved.resolution.type;
                        FunctionPortBinding portBinding;
                        portBinding.portId=port;portBinding.displayName=calculusPortDisplayName(choice.operationId,port);
                        portBinding.direction=input?FunctionPortDirection::Input:FunctionPortDirection::Output;
                        portBinding.typeExpression=type.canonical();portBinding.defaultValueType=type.canonical();
                        portBinding.requirement=input&&calculusInputRequired(choice.operationId,port)
                            ?FunctionPortRequirement::Required:FunctionPortRequirement::Optional;
                        portBinding.connectionPolicy=type.numeric()?FunctionPortConnectionPolicy::NumericImplicit:
                            type.kind==binding::TypeKind::Array&&type.children.size()==1&&type.children.front().numeric()
                            ?FunctionPortConnectionPolicy::NumericArrayImplicit:FunctionPortConnectionPolicy::Exact;
                        definition.ports.push_back(std::move(portBinding));
                    }
                }
                definition.minimumInputCount=definition.maximumInputCount=calculusInputPortIds(choice.operationId).size();
                result.emplace(choice.operationId,std::move(definition));
            }
        }
        return result;
    }();
    const auto found=definitions.find(operation);return found==definitions.end()?nullptr:&found->second;
}
} // namespace frog::native_engine
