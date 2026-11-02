#include "frog_native_engine/SemaphoreFunctionBinding.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/SemaphoreReference.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>

namespace frog::native_engine {
namespace {
std::string_view identity(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    return id;
}
SemaphoreTypeResult rejected(std::string code,std::string message) {
    return {{binding::ResolutionStatus::Incompatible,{},std::move(code),std::move(message)},false};
}
}
bool isSemaphoreFunction(std::string_view id) noexcept {
    id=identity(id);
    static constexpr std::array<std::string_view,6> ids{
        "synchronization.semaphore.obtain","synchronization.semaphore.acquire",
        "synchronization.semaphore.release","synchronization.semaphore.release_reference",
        "synchronization.semaphore.status","synchronization.semaphore.not_a_semaphore"};
    return std::find(ids.begin(),ids.end(),id)!=ids.end();
}
bool isSemaphoreAcquireFunction(std::string_view id) noexcept {
    return identity(id)=="synchronization.semaphore.acquire";
}
bool semaphoreExecutesOnInputError(std::string_view id) noexcept {
    id=identity(id);
    return id=="synchronization.semaphore.acquire" || id=="synchronization.semaphore.release" ||
        id=="synchronization.semaphore.release_reference";
}
std::optional<std::string> semaphoreOptionalInputDefault(std::string_view operationId,std::string_view port) {
    if(!isSemaphoreFunction(operationId))return {};
    const auto *definition=findFunctionBindingDefinition(identity(operationId));
    const auto *input=definition?findFunctionBindingPort(*definition,port):nullptr;
    if(!input || input->direction!=FunctionPortDirection::Input || input->requirement==FunctionPortRequirement::Required)return {};
    if(port=="timeout_ms")return "-1";
    if(port=="size")return "1";
    if(port=="create_if_not_found")return "true";
    if(port=="force_destroy")return "false";
    if(port=="name")return std::string{};
    // error_in is supplied by the existing canonical no-error cluster path.
    return {};
}
SemaphoreTypeResult resolveSemaphoreFunctionPort(std::string_view operationId,std::string_view nodeId,
    std::string_view requested,
    std::span<const SemaphoreTypeEvidence> evidence) {
    const auto id=identity(operationId);
    const auto *definition=findFunctionBindingDefinition(id);
    if(!isSemaphoreFunction(id) || !definition)
        return rejected("binding.semaphore_unknown","The Semaphore operation is not registered.");
    const auto *port=findFunctionBindingPort(*definition,requested);
    if(!port)return rejected("binding.semaphore_port","The Semaphore terminal does not exist.");
    bool complete=true;
    for(const auto &input:definition->ports) {
        if(input.direction!=FunctionPortDirection::Input)continue;
        const auto found=std::find_if(evidence.begin(),evidence.end(),[&](const auto &item){return item.portId==input.portId;});
        if(found==evidence.end()) {complete=complete && input.requirement!=FunctionPortRequirement::Required;continue;}
        if(!found->valueType.known())
            return {{binding::ResolutionStatus::Unresolved,{},"binding.semaphore_input_unresolved",
                "A connected Semaphore input has no established type."},false};
        const auto expected=binding::parseType(input.typeExpression);
        const bool accepted=expected.valid() && (input.portId=="semaphore"?
            isSemaphoreReference(found->valueType):binding::connect(found->valueType,expected.type,
                input.connectionPolicy==FunctionPortConnectionPolicy::NumericImplicit?
                    binding::ConnectionPolicy::NumericImplicit:binding::ConnectionPolicy::Exact).accepted());
        if(!accepted)return rejected("binding.semaphore_input_type","Node "+std::string(nodeId)+", terminal "+input.portId+
            ": type "+found->valueType.canonical()+" is incompatible with the fixed Semaphore signature.");
    }
    const auto resolved=binding::parseType(port->typeExpression);
    if(!resolved.valid() || !resolved.type.known())
        return rejected("binding.semaphore_signature","The Semaphore connector has no concrete declared type.");
    if((port->portId=="semaphore" || port->portId=="semaphore_out") && !isSemaphoreReference(resolved.type))
        return rejected("binding.semaphore_signature","Semaphore references must not declare a payload type.");
    return {{binding::ResolutionStatus::Compatible,resolved.type,{},{}},complete};
}
SemaphoreTypeResult resolveSemaphoreFunctionPort(const DiagramNode &node,
    std::string_view port,std::span<const SemaphoreTypeEvidence> evidence) {
    return resolveSemaphoreFunctionPort(node.type,node.id,port,evidence);
}
} // namespace frog::native_engine
