#include "frog_native_engine/FittingNodeBinding.hpp"
#include "frog_native_engine/CalculusNodeBinding.hpp"
#include "frog_native_engine/StatisticsNodeBinding.hpp"
#include "frog_native_engine/InterpolationNodeBinding.hpp"
#include "frog_native_engine/GeometryNodeBinding.hpp"
#include <map>
#include <array>

namespace frog::native_engine {
const FunctionBindingDefinition *fittingBindingDefinition(std::string_view id,std::string_view variant) noexcept {
    const auto operation=fittingOperationId(id,variant);
    if(operation.empty())return nullptr;
    static const auto definitions=[] {
        std::map<std::string,FunctionBindingDefinition,std::less<>> result;
        for(const auto &function:fittingFunctions()) {
            const auto *base=findFunctionBindingDefinition(function.catalogId);
            if(!base)continue;
            for(const auto &choice:fittingVariants(function.catalogId)) {
                auto definition=*base;
                definition.implementationId=std::string(choice.operationId);
                definition.ports.clear();
                for(const bool input:{true,false}) {
                    const auto ids=input?fittingInputPortIds(choice.operationId):fittingOutputPortIds(choice.operationId);
                    for(const auto port:ids) {
                        const auto resolved=resolveFittingPort(choice.operationId,port);
                        if(!resolved.resolution.accepted() || !resolved.resolution.type.known())continue;
                        const auto &type=resolved.resolution.type;
                        FunctionPortBinding portBinding;
                        portBinding.portId=port;portBinding.displayName=fittingPortDisplayName(choice.operationId,port);
                        portBinding.direction=input?FunctionPortDirection::Input:FunctionPortDirection::Output;
                        portBinding.typeExpression=type.canonical();portBinding.defaultValueType=type.canonical();
                        portBinding.requirement=input&&fittingInputRequired(choice.operationId,port)
                            ?FunctionPortRequirement::Required:FunctionPortRequirement::Optional;
                        portBinding.connectionPolicy=type.numeric()?FunctionPortConnectionPolicy::NumericImplicit:
                            type.kind==binding::TypeKind::Array && type.children.size()==1 && type.children[0].numeric()
                            ?FunctionPortConnectionPolicy::NumericArrayImplicit:FunctionPortConnectionPolicy::Exact;
                        definition.ports.push_back(std::move(portBinding));
                    }
                }
                definition.minimumInputCount=definition.maximumInputCount=fittingInputPortIds(choice.operationId).size();
                result.emplace(choice.operationId,std::move(definition));
            }
        }
        return result;
    }();
    const auto found=definitions.find(operation);return found==definitions.end()?nullptr:&found->second;
}
const FunctionBindingDefinition *functionBindingDefinitionForNode(const DiagramNode &node) noexcept {
    if(node.kind=="calculus_function_reference")if(const auto d=calculusFunctionReferenceDimensions(node.type);d) {
        static const auto definitions=[] {
            std::array<FunctionBindingDefinition,3> out;
            for(unsigned d=1;d<=3;++d){auto &definition=out[d-1];definition.implementationId=calculusFunctionReferenceOperation(d);
                FunctionPortBinding p;p.portId="reference";p.displayName="Integrand Reference";p.direction=FunctionPortDirection::Output;
                p.typeExpression=calculusFunctionReferenceSchema(d);p.defaultValueType=p.typeExpression;p.connectionPolicy=FunctionPortConnectionPolicy::Exact;
                definition.ports.push_back(std::move(p));}
            return out;
        }();return &definitions[d-1];
    }

    if(node.kind=="fitting_model_reference"&&optimizationReferenceKind(node.type)) {
        static const auto definitions=[] {std::array<FunctionBindingDefinition,3> out;
            for(unsigned k=1;k<=3;++k){auto &d=out[k-1];d.implementationId=optimizationReferenceOperation(k);
                FunctionPortBinding p;p.portId="reference";p.displayName="Objective Reference";p.direction=FunctionPortDirection::Output;
                p.typeExpression=optimizationReferenceSchema(k);p.defaultValueType=p.typeExpression;p.connectionPolicy=FunctionPortConnectionPolicy::Exact;d.ports.push_back(std::move(p));}
            return out;}();return &definitions[optimizationReferenceKind(node.type)-1];
    }
    if(node.kind=="fitting_model_reference"&&node.type==fittingModelReferenceOperation) {
        static const auto model=[] {
            FunctionBindingDefinition result;
            result.implementationId=fittingModelReferenceOperation;
            FunctionPortBinding port;port.portId="reference";port.displayName="Model Reference";
            port.direction=FunctionPortDirection::Output;port.typeExpression=fittingModelReferenceSchema;
            port.defaultValueType=fittingModelReferenceSchema;port.connectionPolicy=FunctionPortConnectionPolicy::Exact;
            result.ports.push_back(std::move(port));return result;
        }();return &model;
    }
    if(isGeometryFunction(node.type))return geometryBindingDefinition(node.type,node.geometryVariant);
    if(isInterpolationFunction(node.type))return interpolationBindingDefinition(node.type,node.interpolationVariant);
    if(isStatisticsFunction(node.type))return statisticsBindingDefinition(node);
    if(isCalculusFunction(node.type))return calculusBindingDefinition(node.type,node.calculusVariant);
    if(isFittingFunction(node.type))return fittingBindingDefinition(node.type,node.fittingVariant);
    const auto *entry=findFunctionCatalogEntryByDiagramNodeType(node.type);
    return entry?findFunctionBindingDefinition(entry->catalogId):nullptr;
}
}
