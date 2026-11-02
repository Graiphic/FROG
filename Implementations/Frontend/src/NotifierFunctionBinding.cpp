#include "frog_native_engine/NotifierFunctionBinding.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/NotifierReference.hpp"
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
NotifierTypeResult rejected(std::string code,std::string message) {
    return {{ResolutionStatus::Incompatible,{},std::move(code),std::move(message)},false};
}
NotifierTypeResult unresolved(std::string code,std::string message) {
    return {{ResolutionStatus::Unresolved,{},std::move(code),std::move(message)},false};
}
}
bool isNotifierFunction(std::string_view id) noexcept {
    id=identity(id);
    static constexpr std::array<std::string_view,9> ids{
        "synchronization.notifier.obtain","synchronization.notifier.send",
        "synchronization.notifier.cancel","synchronization.notifier.status",
        "synchronization.notifier.release","synchronization.notifier.wait",
        "synchronization.notifier.wait_multiple","synchronization.notifier.wait_history",
        "synchronization.notifier.wait_multiple_history"};
    return std::find(ids.begin(),ids.end(),id)!=ids.end();
}
bool isNotifierMultipleFunction(std::string_view id) noexcept {
    id=identity(id);
    return id=="synchronization.notifier.wait_multiple"||id=="synchronization.notifier.wait_multiple_history";
}
bool isNotifierWaitFunction(std::string_view id) noexcept {
    id=identity(id);
    return id=="synchronization.notifier.wait"||id=="synchronization.notifier.wait_history"||isNotifierMultipleFunction(id);
}
std::optional<std::string> notifierOptionalInputDefault(std::string_view operationId,std::string_view port) {
    if(!isNotifierFunction(operationId))return {};
    const auto *definition=findFunctionBindingDefinition(identity(operationId));
    const auto *descriptor=definition?findFunctionBindingPort(*definition,port):nullptr;
    if(!descriptor||descriptor->direction!=FunctionPortDirection::Input||descriptor->requirement==FunctionPortRequirement::Required)return {};
    if(port=="timeout_ms")return "-1";
    if(port=="create_if_not_found")return "true";
    if(port=="force_destroy"||port=="ignore_previous")return "false";
    if(port=="name")return std::string{};
    return {};
}
NotifierTypeResult resolveNotifierFunctionPort(std::string_view operationId,std::string_view nodeId,
    std::string_view requested,
    std::span<const NotifierTypeEvidence> evidence) {
    const auto id=identity(operationId);
    const auto *definition=findFunctionBindingDefinition(id);
    if(!isNotifierFunction(id)||!definition)return rejected("binding.notifier_unknown","The Notifier operation is not registered.");
    const auto *port=findFunctionBindingPort(*definition,requested);
    if(!port)return rejected("binding.notifier_port","The Notifier terminal does not exist.");
    const auto supplied=[&](std::string_view name)->const TypeRef* {
        const auto found=std::find_if(evidence.begin(),evidence.end(),[&](const auto &item){return item.portId==name;});
        return found==evidence.end()?nullptr:&found->valueType;
    };
    const bool obtain=id=="synchronization.notifier.obtain",multiple=isNotifierMultipleFunction(id);
    TypeRef element=TypeRef::primitive("f64");
    bool hasElement=obtain;
    if(obtain) {
        if(const auto *specimen=supplied("element_type")){element=*specimen;hasElement=element.known();}
    } else if(const auto *reference=supplied(multiple?"notifiers":"notifier")) {
        const TypeRef *single=reference;
        if(multiple) {
            if(reference->kind!=binding::TypeKind::Array||reference->rank!=1||reference->children.size()!=1)
                return rejected("binding.notifier_array_required","Multiple Wait requires a rank-one array of Notifier<T> references.");
            single=&reference->children.front();
        }
        if(!isNotifierReference(*single))return rejected("binding.notifier_reference_required","Notifier operations require Notifier<T>, not Queue, widget or FROG references.");
        const auto *schema=notifierReferenceElementType(*single);
        if(!schema||!schema->known())return unresolved("binding.notifier_element_unresolved","The connected Notifier has no complete notification schema.");
        element=*schema;hasElement=true;
    } else if(const auto *value=supplied("notification")) {
        // Incomplete authoring evidence never supplies the missing live reference.
        element=*value;hasElement=element.known();
    }
    bool complete=true;
    for(const auto &input:definition->ports) {
        if(input.direction!=FunctionPortDirection::Input)continue;
        const auto *actual=supplied(input.portId);
        if(!actual){complete=complete&&input.requirement!=FunctionPortRequirement::Required;continue;}
        if(!actual->known())return unresolved("binding.notifier_input_unresolved","A connected Notifier input has no established type.");
        bool accepted=false;
        if(input.portId=="notifier")accepted=hasElement&&binding::fullTypeEqual(*actual,notifierReferenceType(element));
        else if(input.portId=="notifiers")accepted=hasElement&&binding::fullTypeEqual(*actual,TypeRef::array(notifierReferenceType(element),1));
        else if(input.portId=="notification"||input.portId=="element_type")accepted=hasElement&&binding::fullTypeEqual(*actual,element);
        else {
            const auto expected=binding::parseType(input.typeExpression).type;
            accepted=binding::connect(*actual,expected,input.connectionPolicy==FunctionPortConnectionPolicy::NumericImplicit?
                binding::ConnectionPolicy::NumericImplicit:binding::ConnectionPolicy::Exact).accepted();
        }
        if(!accepted)return rejected("binding.notifier_input_type","Node "+std::string(nodeId)+", terminal "+input.portId+
            ": type "+actual->canonical()+" is incompatible with the Notifier operation's complete schema or fixed input type.");
    }
    TypeRef resolved=binding::parseType(port->defaultValueType).type;
    if(port->portId=="notifier"||port->portId=="notifier_out") {
        if(!hasElement)return unresolved("binding.notifier_element_unresolved","Notifier<T> requires a complete notification schema.");
        resolved=notifierReferenceType(element);
    } else if(port->portId=="notifiers"||port->portId=="notifiers_out") {
        if(!hasElement)return unresolved("binding.notifier_element_unresolved","Notifier<T> requires a complete notification schema.");
        resolved=TypeRef::array(notifierReferenceType(element),1);
    } else if(port->portId=="element_type"||port->portId=="notification"||port->portId=="canceled_notification"||
        port->portId=="latest_notification"||port->portId=="last_notification") {
        if(!hasElement)return unresolved("binding.notifier_element_unresolved","The complete notification type is not established.");
        resolved=element;
    } else if(port->portId=="notifications") {
        if(!hasElement)return unresolved("binding.notifier_element_unresolved","Multiple Wait requires the complete notification schema.");
        resolved=notifierMultipleNotificationType(element);
    }
    return {{ResolutionStatus::Compatible,std::move(resolved),{},{}},complete};
}
NotifierTypeResult resolveNotifierFunctionPort(const DiagramNode &node,
    std::string_view port,std::span<const NotifierTypeEvidence> evidence) {
    return resolveNotifierFunctionPort(node.type,node.id,port,evidence);
}
} // namespace frog::native_engine
