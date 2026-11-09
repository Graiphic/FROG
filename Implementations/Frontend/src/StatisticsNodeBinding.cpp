#include "frog_native_engine/StatisticsNodeBinding.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include <algorithm>
#include <array>
#include <map>
#include <set>
namespace frog::native_engine {
const FunctionBindingDefinition *statisticsBindingDefinition(std::string_view id,std::string_view variant) noexcept {
    const auto operation=statisticsOperationId(id,variant);
    if(operation.empty())return nullptr;
    static const auto definitions=[] {
        std::map<std::string,FunctionBindingDefinition,std::less<>> result;
        for(const auto &function:statisticsFunctions()) {
            const auto *base=findFunctionBindingDefinition(function.catalogId);
            if(!base)continue;
            for(const auto &choice:statisticsVariants(function.catalogId)) {
                auto definition=*base;
                definition.implementationId=std::string(choice.operationId);
                definition.ports.clear();
                for(const bool input:{true,false}) {
                    const auto ids=input?statisticsInputPortIds(choice.operationId):statisticsOutputPortIds(choice.operationId);
                    for(const auto port:ids) {
                        const auto resolved=resolveStatisticsPort(choice.operationId,port);
                        if(!resolved.resolution.accepted()||!resolved.resolution.type.known())continue;
                        const auto &type=resolved.resolution.type;
                        FunctionPortBinding portBinding;
                        portBinding.portId=port;portBinding.displayName=statisticsPortDisplayName(choice.operationId,port);
                        portBinding.direction=input?FunctionPortDirection::Input:FunctionPortDirection::Output;
                        portBinding.typeExpression=type.canonical();portBinding.defaultValueType=type.canonical();
                        portBinding.requirement=input&&statisticsInputRequired(choice.operationId,port)
                            ?FunctionPortRequirement::Required:FunctionPortRequirement::Optional;
                        portBinding.connectionPolicy=type.numeric()?FunctionPortConnectionPolicy::NumericImplicit:
                            type.kind==binding::TypeKind::Array&&type.children.size()==1&&type.children.front().numeric()
                            ?FunctionPortConnectionPolicy::NumericArrayImplicit:FunctionPortConnectionPolicy::Exact;
                        definition.ports.push_back(std::move(portBinding));
                    }
                }
                definition.minimumInputCount=definition.maximumInputCount=statisticsInputPortIds(choice.operationId).size();
                result.emplace(choice.operationId,std::move(definition));
            }
        }
        return result;
    }();
    const auto found=definitions.find(operation);return found==definitions.end()?nullptr:&found->second;
}
namespace {
std::shared_ptr<const StatisticsNodeProjection> projection(std::string_view operation,
    std::span<const std::string> outputs,bool express) {
    const auto *base=statisticsBindingDefinition(operation);if(!base)return {};
    auto result=std::make_shared<StatisticsNodeProjection>();
    result->operationId=operation;result->selectedOutputs.assign(outputs.begin(),outputs.end());
    result->definition=*base;
    if(express) {
        std::set<std::string_view> selected;
        if(outputs.empty())return {};
        for(const auto &id:outputs) {
            const auto *port=findFunctionBindingPort(*base,id);
            if(!port||port->direction!=FunctionPortDirection::Output||id=="error_out"||!selected.insert(id).second)return {};
        }
        std::erase_if(result->definition.ports,[&](const auto &port){
            return port.direction==FunctionPortDirection::Output&&port.portId!="error_out"&&!selected.contains(port.portId);
        });
    }
    for(const auto direction:{FunctionPortDirection::Input,FunctionPortDirection::Output}) {
        const auto count=std::count_if(result->definition.ports.begin(),result->definition.ports.end(),[&](const auto &p){return p.direction==direction;});
        std::size_t index=0;
        for(const auto &port:result->definition.ports)if(port.direction==direction)
            result->ports.push_back({port.portId,direction,direction==FunctionPortDirection::Input?0.:1.,(++index-.5)/count});
    }
    return result;
}
const StatisticsNodeProjection *defaultProjection(std::string_view operation) {
    static const auto all=[] {
        std::map<std::string,std::shared_ptr<const StatisticsNodeProjection>,std::less<>> values;
        const std::array<std::string,1> mean{"mean"};
        for(const auto &function:statisticsFunctions())for(const auto &variant:statisticsVariants(function.catalogId))
            values.emplace(variant.operationId,projection(variant.operationId,
                isStatisticsExpressFunction(function.catalogId)?std::span<const std::string>{mean}:std::span<const std::string>{},
                isStatisticsExpressFunction(function.catalogId)));
        return values;
    }();
    const auto found=all.find(operation);return found==all.end()?nullptr:found->second.get();
}
const StatisticsNodeProjection *nodeProjection(const DiagramNode &node) {
    const auto operation=statisticsOperationId(node.type,node.statisticsVariant);
    if(operation.empty())return nullptr;
    if(isStatisticsExpressFunction(node.type)&&!node.statisticsOutputs.empty()) {
        const auto &p=node.statisticsProjection;
        return p&&p->operationId==operation&&p->selectedOutputs==node.statisticsOutputs?p.get():nullptr;
    }
    return defaultProjection(operation);
}
}
void refreshStatisticsNodeProjection(DiagramNode &node) {
    node.statisticsProjection.reset();
    if(isStatisticsExpressFunction(node.type)&&!node.statisticsOutputs.empty())
        node.statisticsProjection=projection(statisticsOperationId(node.type,node.statisticsVariant),node.statisticsOutputs,true);
}
const FunctionBindingDefinition *statisticsBindingDefinition(const DiagramNode &node) noexcept {
    const auto *p=nodeProjection(node);return p?&p->definition:nullptr;
}
std::span<const FunctionPortDescriptor> statisticsDiagramPorts(const DiagramNode &node) noexcept {
    const auto *p=nodeProjection(node);return p?std::span<const FunctionPortDescriptor>{p->ports}:std::span<const FunctionPortDescriptor>{};
}
} // namespace frog::native_engine
