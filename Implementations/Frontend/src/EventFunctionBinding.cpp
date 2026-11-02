#include "frog_native_engine/EventFunctionBinding.hpp"
#include "frog_native_engine/EventStructureSchema.hpp"
#include <algorithm>
#include <array>
#include <set>

namespace frog::native_engine {
namespace {
using Type=binding::TypeRef;
using Kind=binding::TypeKind;
using Status=binding::ResolutionStatus;
constexpr std::string_view prefix="dialog.events.";
std::string_view identity(std::string_view id) {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);return id;
}
EventTypeResult fail(std::string code,std::string message,bool unresolved=false) {
    return {{unresolved?Status::Unresolved:Status::Incompatible,{},std::move(code),std::move(message)},false};
}
bool sourceAllowed(const Type &type,std::string_view eventKind) {
    if(!type.known())return false;
    if(type.kind==Kind::Array)return sourceAllowed(type.children.front(),eventKind);
    if(type.kind==Kind::Cluster)return !type.children.empty() &&
        std::all_of(type.children.begin(),type.children.end(),[&](const auto &child){return sourceAllowed(child,eventKind);});
    if(type.kind!=Kind::Reference)return false;
    if(isUserEventReference(type))return userEventDataType(type) &&
        (eventKind=="user_event" || eventKind=="none");
    EventSourceSpec source;source.id="type-specimen";source.referenceClassId=type.name;
    if(!type.children.empty())source.valueType=type.children.front();
    if(type.name=="frog.application")source.scope=EventSourceScope::Application;
    else if(type.name=="frog.program")source.scope=EventSourceScope::Vi;
    else if(type.name=="frog.front_panel" || type.name=="frog.pane")source.scope=EventSourceScope::Pane;
    else if(binding::referenceClassAssignable(type.name,"frog.control"))source.scope=EventSourceScope::Control;
    else return false;
    if((type.name.find("waveform_graph")!=std::string::npos || type.name.find("xy_graph")!=std::string::npos))source.controlCategory=EventControlCategory::WaveformGraph;
    if(eventKind=="none")return true;
    const auto *event=eventNotificationSpec(eventKind);
    return event && eventNotificationSupportsSource(*event,source);
}
bool criterionAllowed(const Type &type) {
    if(type.kind==Kind::Reference)return sourceAllowed(type,"none");
    if(type.kind==Kind::Enum)return true;
    if(type.kind==Kind::Array)return criterionAllowed(type.children.front());
    if(type.kind==Kind::Cluster)return !type.children.empty() &&
        std::all_of(type.children.begin(),type.children.end(),[](const auto &child){return criterionAllowed(child);});
    return type.kind==Kind::Primitive && type.name=="string"; // stable catalogue kind ID, a FROG selector
}
}
bool isEventFunction(std::string_view id) noexcept {
    id=identity(id);
    constexpr std::array<std::string_view,6> ids{"register_for_events","unregister_for_events","flush_event_queue",
        "create_user_event","generate_user_event","destroy_user_event"};
    return id.starts_with(prefix) && std::find(ids.begin(),ids.end(),id.substr(prefix.size()))!=ids.end();
}
std::vector<DiagramEventRegistrationRow> eventRegistrationRowsForNode(const DiagramNode &node) {
    return node.eventRegistrationRows.empty()?std::vector<DiagramEventRegistrationRow>{{"source_1","user_event"}}:
        node.eventRegistrationRows;
}
std::vector<EventFunctionPort> eventFunctionPorts(const DiagramNode &node) {
    const auto id=identity(node.type);const auto *definition=findFunctionBindingDefinition(id);
    std::vector<EventFunctionPort> result;if(!definition || !isEventFunction(id))return result;
    for(const auto &port:definition->ports) {
        if(id=="dialog.events.register_for_events" && port.portId=="source") {
            for(const auto &row:eventRegistrationRowsForNode(node))result.push_back({"source:"+row.id,row.eventKind,false});
        } else result.push_back({port.portId,port.displayName,port.direction==FunctionPortDirection::Output});
    }
    return result;
}
std::optional<std::string> eventOptionalInputDefault(const DiagramNode &node,std::string_view port) {
    if(identity(node.type)=="dialog.events.generate_user_event" && port=="priority")return "1";
    return {};
}
EventTypeResult resolveEventFunctionPort(const DiagramNode &node,std::string_view requested,
    std::span<const EventTypeEvidence> evidence) {
    const auto id=identity(node.type);const auto *definition=findFunctionBindingDefinition(id);
    if(!definition || !isEventFunction(id))return fail("binding.event_unknown","The Event operation is not registered.");
    const auto ports=eventFunctionPorts(node);
    if(std::none_of(ports.begin(),ports.end(),[&](const auto &p){return p.id==requested;}))
        return fail("binding.event_port","The Event terminal does not exist or its stable row was removed.");
    const auto supplied=[&](std::string_view name)->const Type* {
        const auto found=std::find_if(evidence.begin(),evidence.end(),[&](const auto &item){return item.portId==name;});
        return found==evidence.end()?nullptr:&found->valueType;
    };
    const bool create=id=="dialog.events.create_user_event",generate=id=="dialog.events.generate_user_event";
    const bool registration=id=="dialog.events.register_for_events",flush=id=="dialog.events.flush_event_queue";
    Type data=Type::primitive("f64"),registrationType=eventRegistrationReferenceType(Type::cluster({},{}));
    bool complete=true,hasUser=create,hasRegistration=false;
    if(create) {if(const auto *specimen=supplied("data_type"))data=*specimen;}
    else if(generate || id=="dialog.events.destroy_user_event") {
        if(const auto *reference=supplied("user_event")) {
            const auto *payload=userEventDataType(*reference);
            if(!payload || !payload->known())return fail("binding.user_event_required",
                "This terminal requires UserEvent<T>; data, object references, registrations and Queues are different types.");
            data=*payload;hasUser=true;
        } else complete=false;
    }
    if(const auto *existing=supplied("registration_in")) {
        if(!(flush?isEventRegistrationCollection(*existing):(isEventRegistrationReference(*existing) &&
            eventRegistrationSchema(*existing) && eventRegistrationSchema(*existing)->kind==Kind::Cluster && existing->known())))
            return fail("binding.event_registration_required","This terminal requires an EventRegistration schema, not a Queue, UserEvent, object or file reference.");
        registrationType=*existing;hasRegistration=true;
    }
    if(registration) {
        std::vector<std::string> fields;std::vector<Type> sourceTypes;std::set<std::string> ids;
        const auto previous=eventRegistrationSources(registrationType);
        for(const auto &row:eventRegistrationRowsForNode(node)) {
            if(row.id.empty() || row.id.find_first_of(":/")!=std::string::npos || !ids.insert(row.id).second || row.eventKind.empty())
                return fail("binding.event_registration_rows","Registration rows require unique stable IDs and a selected event kind.");
            auto kind=row.eventKind;const Type *source=supplied("source:"+row.id);
            const auto old=std::find_if(previous.begin(),previous.end(),[&](const auto &p){return p.id==row.id;});
            if(kind=="none" && old==previous.end())return fail("binding.event_none_schema",
                "None suspends an existing typed registration row; an initial None row has no declared event signature.");
            if(!source && old!=previous.end())source=&old->sourceType;
            if(kind=="none" && old!=previous.end())kind=old->eventKind;
            if(!source) {complete=false;sourceTypes.push_back(userEventReferenceType(Type::primitive("f64")));}
            else {
                if(!sourceAllowed(*source,row.eventKind))return fail("binding.event_source_class",
                    "Node "+node.id+", row "+row.id+": the selected event is not supported by the complete source reference class.");
                sourceTypes.push_back(*source);
            }
            fields.push_back(row.id+":"+kind);
        }
        auto schema=Type::cluster(std::move(fields),std::move(sourceTypes));
        if(hasRegistration) {
            const auto &old=*eventRegistrationSchema(registrationType);
            bool same=old.children.size()==schema.children.size();
            for(std::size_t index=0;same && index<schema.fieldIds.size();++index) {
                const auto field=std::find(old.fieldIds.begin(),old.fieldIds.end(),schema.fieldIds[index]);
                same=field!=old.fieldIds.end() && old.children[static_cast<std::size_t>(field-old.fieldIds.begin())]==schema.children[index];
            }
            if(!same)return fail("binding.event_registration_schema",
                "Re-registration may replace compatible source objects but cannot change the compiled registration signature.");
        } else registrationType=eventRegistrationReferenceType(std::move(schema));
        hasRegistration=true;
    } else if(!create && !generate && id!="dialog.events.destroy_user_event" && !hasRegistration)complete=false;
    if(generate && !hasUser)if(const auto *message=supplied("data"))data=*message;
    for(const auto &item:evidence) {
        if(!item.valueType.known())return fail("binding.event_input_unresolved","A connected Event input has an incomplete type.",true);
        if(item.portId.starts_with("source:")) {
            if(std::none_of(ports.begin(),ports.end(),[&](const auto &p){return !p.output && p.id==item.portId;}))
                return fail("binding.event_input_port","A connected registration row was removed; its wire is retained as invalid.");
            continue;
        }
        const auto *descriptor=findFunctionBindingPort(*definition,item.portId);
        if(!descriptor || descriptor->direction!=FunctionPortDirection::Input)
            return fail("binding.event_input_port","A connected Event input does not exist.");
        bool accepted=true;
        if(item.portId=="data")accepted=item.valueType==data;
        else if(item.portId=="data_type")accepted=create;
        else if(item.portId=="event_type_or_object")accepted=criterionAllowed(item.valueType);
        else if(item.portId!="user_event" && item.portId!="registration_in")
            accepted=binding::connect(item.valueType,binding::parseType(descriptor->defaultValueType).type,
                descriptor->connectionPolicy==FunctionPortConnectionPolicy::NumericImplicit?
                    binding::ConnectionPolicy::NumericImplicit:binding::ConnectionPolicy::Exact).accepted();
        if(!accepted)return fail("binding.event_input_type","Node "+node.id+", terminal "+std::string(item.portId)+
            ": the value does not match the complete Event schema or fixed terminal type.");
    }
    for(const auto &p:definition->ports)if(p.direction==FunctionPortDirection::Input &&
        p.requirement==FunctionPortRequirement::Required && !supplied(p.portId))complete=false;
    Type result;
    if(requested=="user_event" || requested=="user_event_out")result=userEventReferenceType(data);
    else if(requested=="data" || requested=="data_type")result=data;
    else if(requested=="registration_in" || requested=="registration_out")result=registrationType;
    else if(requested.starts_with("source:")) {
        const auto sources=eventRegistrationSources(registrationType);
        const auto source=std::find_if(sources.begin(),sources.end(),[&](const auto &p){return p.id==requested.substr(7);});
        if(source==sources.end())return fail("binding.event_source_row","The stable registration row has no schema.");
        result=source->sourceType;
    } else if(requested=="event_type_or_object")result=supplied(requested)?*supplied(requested):Type::primitive("string");
    else if(const auto *p=findFunctionBindingPort(*definition,requested))result=binding::parseType(p->defaultValueType).type;
    if(!result.known())return fail("binding.event_type_unresolved","The Event terminal has no complete type.",true);
    return {{Status::Compatible,std::move(result),{},{}},complete};
}
} // namespace frog::native_engine
