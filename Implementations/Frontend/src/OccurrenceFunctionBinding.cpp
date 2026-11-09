#include "frog_native_engine/OccurrenceFunctionBinding.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/OccurrenceReference.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>
#include <utility>

namespace frog::native_engine {
namespace {
std::string_view identity(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    return id;
}
OccurrenceTypeResult rejected(std::string code,std::string message) {
    return {{binding::ResolutionStatus::Incompatible,{},std::move(code),std::move(message)},false};
}
}
bool isOccurrenceFunction(std::string_view id) noexcept {
    id=identity(id);
    static constexpr std::array<std::string_view,3> ids{
        "synchronization.occurrences.generate","synchronization.occurrences.wait","synchronization.occurrences.set"};
    return std::find(ids.begin(),ids.end(),id)!=ids.end();
}
bool isOccurrenceWaitFunction(std::string_view id) noexcept {
    return identity(id)=="synchronization.occurrences.wait";
}
std::optional<std::string> occurrenceOptionalInputDefault(std::string_view operationId,std::string_view port) {
    if(!isOccurrenceWaitFunction(operationId))return {};
    if(port=="timeout_ms")return "-1";
    if(port=="ignore_previous")return "true";
    return {};
}
OccurrenceTypeResult resolveOccurrenceFunctionPort(std::string_view operationId,std::string_view nodeId,
    std::string_view requested,
    std::span<const OccurrenceTypeEvidence> evidence) {
    const auto id=identity(operationId);
    const auto *definition=findFunctionBindingDefinition(id);
    if(!isOccurrenceFunction(id)||!definition)
        return rejected("binding.occurrence_unknown","The Occurrence operation is not registered.");
    const auto *port=findFunctionBindingPort(*definition,requested);
    if(!port)return rejected("binding.occurrence_port","The Occurrence terminal does not exist.");
    for(const auto &item:evidence) {
        const auto *input=findFunctionBindingPort(*definition,item.portId);
        if(!input||input->direction!=FunctionPortDirection::Input)
            return rejected("binding.occurrence_input_port","Evidence targets an absent Occurrence input.");
        if(!item.valueType.known())
            return {{binding::ResolutionStatus::Unresolved,{},"binding.occurrence_input_unresolved",
                "A connected Occurrence input has no established type."},false};
        const auto expected=binding::parseType(input->typeExpression);
        const bool accepted=expected.valid()&&(input->portId=="occurrence"?isOccurrenceReference(item.valueType):
            binding::connect(item.valueType,expected.type,input->connectionPolicy==FunctionPortConnectionPolicy::NumericImplicit?
                binding::ConnectionPolicy::NumericImplicit:binding::ConnectionPolicy::Exact).accepted());
        if(!accepted)return rejected("binding.occurrence_input_type","Node "+std::string(nodeId)+", terminal "+input->portId+
            ": type "+item.valueType.canonical()+" is incompatible with the fixed Occurrence signature.");
    }
    const bool complete=std::none_of(definition->ports.begin(),definition->ports.end(),[&](const auto &input) {
        return input.direction==FunctionPortDirection::Input&&input.requirement==FunctionPortRequirement::Required&&
            std::none_of(evidence.begin(),evidence.end(),[&](const auto &item){return item.portId==input.portId;});
    });
    const auto resolved=binding::parseType(port->typeExpression);
    if(!resolved.valid()||!resolved.type.known()||(port->portId=="occurrence"&&!isOccurrenceReference(resolved.type)))
        return rejected("binding.occurrence_signature","The Occurrence connector has no valid fixed nominal type.");
    return {{binding::ResolutionStatus::Compatible,resolved.type,{},{}},complete};
}
OccurrenceTypeResult resolveOccurrenceFunctionPort(const DiagramNode &node,
    std::string_view port,std::span<const OccurrenceTypeEvidence> evidence) {
    return resolveOccurrenceFunctionPort(node.type,node.id,port,evidence);
}
} // namespace frog::native_engine
