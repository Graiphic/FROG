#include "frog_native_engine/InterpolationNodeBinding.hpp"
#include <map>

namespace frog::native_engine {
const FunctionBindingDefinition *interpolationBindingDefinition(std::string_view id,std::string_view variant) noexcept {
    const auto operation=interpolationOperationId(id,variant);
    if(operation.empty())return nullptr;
    static const auto definitions=[] {
        std::map<std::string,FunctionBindingDefinition,std::less<>> result;
        for(const auto &function:interpolationFunctions()) {
            const auto *base=findFunctionBindingDefinition(function.catalogId);
            if(!base)continue;
            for(const auto &choice:interpolationVariants(function.catalogId)) {
                auto definition=*base;
                definition.implementationId=std::string(choice.operationId);
                definition.ports.clear();
                for(const bool input:{true,false}) {
                    const auto ids=input?interpolationInputPortIds(choice.operationId):interpolationOutputPortIds(choice.operationId);
                    for(const auto port:ids) {
                        const auto resolved=resolveInterpolationPort(choice.operationId,port);
                        if(!resolved.resolution.accepted() || !resolved.resolution.type.known())continue;
                        const auto &type=resolved.resolution.type;
                        FunctionPortBinding portBinding;
                        portBinding.portId=port;portBinding.displayName=interpolationPortDisplayName(choice.operationId,port);
                        portBinding.direction=input?FunctionPortDirection::Input:FunctionPortDirection::Output;
                        portBinding.typeExpression=type.canonical();portBinding.defaultValueType=type.canonical();
                        portBinding.requirement=input&&interpolationInputRequired(choice.operationId,port)
                            ?FunctionPortRequirement::Required:FunctionPortRequirement::Optional;
                        portBinding.connectionPolicy=type.numeric()?FunctionPortConnectionPolicy::NumericImplicit:
                            type.kind==binding::TypeKind::Array && type.children.size()==1 && type.children[0].numeric()
                            ?FunctionPortConnectionPolicy::NumericArrayImplicit:FunctionPortConnectionPolicy::Exact;
                        definition.ports.push_back(std::move(portBinding));
                    }
                }
                definition.minimumInputCount=definition.maximumInputCount=interpolationInputPortIds(choice.operationId).size();
                result.emplace(choice.operationId,std::move(definition));
            }
        }
        return result;
    }();
    const auto found=definitions.find(operation);return found==definitions.end()?nullptr:&found->second;
}
}
