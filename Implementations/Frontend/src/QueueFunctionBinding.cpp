#include "frog_native_engine/QueueFunctionBinding.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/QueueReference.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include <algorithm>
#include <array>

namespace frog::native_engine {
namespace {
using binding::TypeRef;
using binding::ResolutionStatus;
std::string_view identity(std::string_view id) {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    return id;
}
QueueTypeResult rejected(std::string code,std::string message) {
    return {{ResolutionStatus::Incompatible,{},std::move(code),std::move(message)},false};
}
QueueTypeResult unresolved(std::string code,std::string message) {
    return {{ResolutionStatus::Unresolved,{},std::move(code),std::move(message)},false};
}
}
bool isQueueFunction(std::string_view id) noexcept {
    id=identity(id);
    static constexpr std::array<std::string_view,8> ids{
        "synchronization.queue.obtain","synchronization.queue.enqueue",
        "synchronization.queue.preview","synchronization.queue.status",
        "synchronization.queue.release","synchronization.queue.lossy_enqueue",
        "synchronization.queue.dequeue","synchronization.queue.flush"};
    return std::find(ids.begin(),ids.end(),id)!=ids.end();
}
std::optional<std::string> queueOptionalInputDefault(std::string_view operationId,std::string_view port) {
    if(!isQueueFunction(operationId))return {};
    const auto id=identity(operationId);
    const auto *definition=findFunctionBindingDefinition(id);
    const auto *descriptor=definition?findFunctionBindingPort(*definition,port):nullptr;
    if(!descriptor || descriptor->direction!=FunctionPortDirection::Input ||
        descriptor->requirement==FunctionPortRequirement::Required)return {};
    if(port=="timeout_ms" || port=="max_size")return "-1";
    if(port=="create_if_not_found")return "true";
    if(port=="force_destroy" || port=="return_elements")return "false";
    if(port=="name")return std::string{};
    return {};
}
QueueTypeResult resolveQueueFunctionPort(std::string_view operationId,std::string_view nodeId,
    std::string_view requested,
    std::span<const QueueTypeEvidence> evidence) {
    const auto id=identity(operationId);
    const auto *definition=findFunctionBindingDefinition(id);
    if(!isQueueFunction(id) || !definition)
        return rejected("binding.queue_unknown","The Queue operation is not registered.");
    const auto *port=findFunctionBindingPort(*definition,requested);
    if(!port)return rejected("binding.queue_port","The Queue terminal does not exist.");
    const auto supplied=[&](std::string_view name)->const TypeRef* {
        const auto found=std::find_if(evidence.begin(),evidence.end(),
            [&](const auto &item){return item.portId==name;});
        return found==evidence.end()?nullptr:&found->valueType;
    };
    const bool obtain=id=="synchronization.queue.obtain";
    TypeRef element=TypeRef::primitive("f64");
    bool hasElement=obtain;
    if(obtain) {
        if(const auto *specimen=supplied("element_type")){element=*specimen;hasElement=element.known();}
    } else if(const auto *queue=supplied("queue")) {
        const auto *schema=queueReferenceElementType(*queue);
        if(!isQueueReference(*queue))
            return rejected("binding.queue_reference_required","Queue operations require a Queue<T> resource reference, not an object or File reference.");
        if(!schema || !schema->known())
            return unresolved("binding.queue_element_unresolved","The connected Queue resource has no complete element schema.");
        element=*schema;hasElement=true;
    } else if(const auto *value=supplied("element")) {
        // Authoring evidence may describe an unwired node, but it does not
        // reinterpret an existing Queue or make a required Queue input complete.
        element=*value;hasElement=element.known();
    }
    bool complete=true;
    for(const auto &input:definition->ports) {
        if(input.direction!=FunctionPortDirection::Input)continue;
        const auto *actual=supplied(input.portId);
        if(!actual){complete=complete && input.requirement!=FunctionPortRequirement::Required;continue;}
        if(!actual->known())return unresolved("binding.queue_input_unresolved","A connected Queue input has no established type.");
        bool accepted=false;
        if(input.portId=="queue")accepted=isQueueReference(*actual)&&queueReferenceElementType(*actual)&&
            *queueReferenceElementType(*actual)==element;
        else if(input.portId=="element" || input.portId=="element_type")accepted=hasElement && *actual==element;
        else {
            const auto expected=binding::parseType(input.typeExpression).type;
            accepted=binding::connect(*actual,expected,input.connectionPolicy==FunctionPortConnectionPolicy::NumericImplicit?
                binding::ConnectionPolicy::NumericImplicit:binding::ConnectionPolicy::Exact).accepted();
        }
        if(!accepted)return rejected("binding.queue_input_type","Node "+std::string(nodeId)+", terminal "+input.portId+
            ": type "+actual->canonical()+" is incompatible with the Queue operation's complete element schema or fixed input type.");
    }
    TypeRef resolved=binding::parseType(port->defaultValueType).type;
    if(port->portId=="queue" || port->portId=="queue_out") {
        if(!hasElement)return unresolved("binding.queue_element_unresolved","Queue<T> requires a complete element schema.");
        resolved=queueReferenceType(element);
    } else if(port->portId=="element" || port->portId=="element_type" || port->portId=="overflow_element") {
        if(!hasElement)return unresolved("binding.queue_element_unresolved","The Queue element type is not established.");
        resolved=element;
    } else if(port->portId=="elements" || port->portId=="remaining_elements") {
        if(!hasElement)return unresolved("binding.queue_element_unresolved","The Queue collection requires a complete element schema.");
        resolved=TypeRef::array(element,1);
    }
    return {{ResolutionStatus::Compatible,std::move(resolved),{},{}},complete};
}
QueueTypeResult resolveQueueFunctionPort(const DiagramNode &node,
    std::string_view port,std::span<const QueueTypeEvidence> evidence) {
    return resolveQueueFunctionPort(node.type,node.id,port,evidence);
}
} // namespace frog::native_engine
