#include "frog_native_engine/RendezvousFunctionBinding.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/RendezvousReference.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>

namespace frog::native_engine {
namespace {
std::string_view identity(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    return id;
}
RendezvousTypeResult rejected(std::string code,std::string message) {
    return {{binding::ResolutionStatus::Incompatible,{},std::move(code),std::move(message)},false};
}
}
bool isRendezvousFunction(std::string_view id) noexcept {
    id=identity(id);
    static constexpr std::array<std::string_view,6> ids{
        "synchronization.rendezvous.create","synchronization.rendezvous.wait",
        "synchronization.rendezvous.resize","synchronization.rendezvous.destroy",
        "synchronization.rendezvous.status","synchronization.rendezvous.not_a_rendezvous"};
    return std::find(ids.begin(),ids.end(),id)!=ids.end();
}
bool isRendezvousWaitFunction(std::string_view id) noexcept {
    return identity(id)=="synchronization.rendezvous.wait";
}
bool rendezvousExecutesOnInputError(std::string_view id) noexcept {
    return identity(id)=="synchronization.rendezvous.destroy";
}
std::optional<std::string> rendezvousOptionalInputDefault(std::string_view operationId,std::string_view port) {
    if(!isRendezvousFunction(operationId))return {};
    const auto *definition=findFunctionBindingDefinition(identity(operationId));
    const auto *input=definition?findFunctionBindingPort(*definition,port):nullptr;
    if(!input || input->direction!=FunctionPortDirection::Input || input->requirement==FunctionPortRequirement::Required)return {};
    if(port=="timeout_ms")return "-1";
    if(port=="size")return "2";
    if(port=="size_change")return "0";
    if(port=="return_existing")return "false";
    if(port=="name")return std::string{};
    // error_in uses the common canonical no-error cluster path.
    return {};
}
RendezvousTypeResult resolveRendezvousFunctionPort(std::string_view operationId,std::string_view nodeId,
    std::string_view requested,
    std::span<const RendezvousTypeEvidence> evidence) {
    const auto id=identity(operationId);
    const auto *definition=findFunctionBindingDefinition(id);
    if(!isRendezvousFunction(id) || !definition)
        return rejected("binding.rendezvous_unknown","The Rendezvous operation is not registered.");
    const auto *port=findFunctionBindingPort(*definition,requested);
    if(!port)return rejected("binding.rendezvous_port","The Rendezvous terminal does not exist.");
    bool complete=true;
    for(const auto &input:definition->ports) {
        if(input.direction!=FunctionPortDirection::Input)continue;
        const auto found=std::find_if(evidence.begin(),evidence.end(),[&](const auto &item){return item.portId==input.portId;});
        if(found==evidence.end()) {complete=complete && input.requirement!=FunctionPortRequirement::Required;continue;}
        if(!found->valueType.known())
            return {{binding::ResolutionStatus::Unresolved,{},"binding.rendezvous_input_unresolved",
                "A connected Rendezvous input has no established type."},false};
        const auto expected=binding::parseType(input.typeExpression);
        const bool accepted=expected.valid() && (input.portId=="rendezvous"?
            isRendezvousReference(found->valueType):binding::connect(found->valueType,expected.type,
                input.connectionPolicy==FunctionPortConnectionPolicy::NumericImplicit?
                    binding::ConnectionPolicy::NumericImplicit:binding::ConnectionPolicy::Exact).accepted());
        if(!accepted)return rejected("binding.rendezvous_input_type","Node "+std::string(nodeId)+", terminal "+input.portId+
            ": type "+found->valueType.canonical()+" is incompatible with the fixed Rendezvous signature.");
    }
    const auto resolved=binding::parseType(port->typeExpression);
    if(!resolved.valid() || !resolved.type.known())
        return rejected("binding.rendezvous_signature","The Rendezvous connector has no concrete declared type.");
    if((port->portId=="rendezvous" || port->portId=="rendezvous_out") && !isRendezvousReference(resolved.type))
        return rejected("binding.rendezvous_signature","Rendezvous references must not declare a message type.");
    return {{binding::ResolutionStatus::Compatible,resolved.type,{},{}},complete};
}
RendezvousTypeResult resolveRendezvousFunctionPort(const DiagramNode &node,
    std::string_view port,std::span<const RendezvousTypeEvidence> evidence) {
    return resolveRendezvousFunctionPort(node.type,node.id,port,evidence);
}
} // namespace frog::native_engine
