#include "frog_native_engine/EventStructureSchema.hpp"
#include "frog_native_engine/DiagramStructureShelfCatalog.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/WidgetCatalog.hpp"
#include "frog_native_engine/EventReference.hpp"
#include <algorithm>
#include <array>
#include <initializer_list>
namespace frog::native_engine {
namespace {
constexpr std::uint32_t scopeBit(EventSourceScope scope) noexcept {
    return 1U << static_cast<unsigned>(scope);
}
bool sameEvent(const DiagramEventBinding &a, const DiagramEventBinding &b) noexcept {
    return a.kind == b.kind && a.source == b.source &&
        eventBindingSourceScope(a) == eventBindingSourceScope(b);
}
// Versioned Studio source-profile types, not LabVIEW binary layouts or a runtime ABI.
// Coords use owning-pane logical units; window bounds use window logical
// limits. These are FROG authoring decisions, not NI integer widths/layouts.
// Char is a Unicode scalar; virtual/scan key codes are opaque unsigned codes.
constexpr DiagramStructureShelfFieldSpec payloadFields[]{
    kDiagramEventValueChangeDataFields[0], kDiagramEventValueChangeDataFields[1],
    {"coords","Coords","cluster<x:f64,y:f64>"},
    {"button","Button","u32"},
    {"mods","Mods","cluster<menu_key:bool,keypad:bool,double_click:bool>"},
    {"platform_mods","PlatMods","cluster<shift:bool,control:bool,alt:bool,windows:bool>"},
    {"char","Char","u32"}, {"virtual_key","VKey","u32"}, {"scan_code","ScanCode","u32"},
    {"old_bounds","OldBnds","cluster<left:f64,top:f64,right:f64,bottom:f64>"},
    {"new_bounds","NewBnds","cluster<left:f64,top:f64,right:f64,bottom:f64>"},
    {"control_ref","CtlRef","unknown"}, {"program_ref","FROGRef","unknown"},
    {"pane_ref","PaneRef","unknown"}, {"application_ref","AppRef","unknown"},
    {"program_name","FROGName","string"}, {"program_window","FROGWindow","unknown"},
    {"program_application_ref","FROGAppRef","ref<frog.application>"},
    {"focus_object","FocusObj","unknown"}, {"sub_object","SubObj","unknown"},
    {"delta","Delta","f64"}, {"orientation","Orientation","unknown"},
    {"button_mods","ButtonMods","cluster<left:bool,right:bool,middle:bool>"},
    {"window_action","Act","unknown"}, {"previous_window_state","PrevState","unknown"},
    {"available_data_names","AvailableDataNames","array<string,rank=1>"},
    {"drag_result","Result","unknown"}, {"menu_ref","MenuRef","ref<frog.menu>"},
    {"item_tag","ItemTag","string"}, {"item_path","ItemPath","string"},
    {"cursor_index","CursIdx","i32"}, {"graph_cursor_ref","CursRef","ref<frog.graph.cursor>"},
    {"plot_index","PlotIdx","i32"}, {"point_index","PointIdx","i32"},
    {"cursor_location","CursLoc","cluster<x:f64,y:f64>"},
    {"scale_kind","Kind","unknown"}, {"scale_index","Index","i32"},
    {"scale_ref","ScaleRef","ref<frog.graph.scale>"},
    {"new_range","NewRange","cluster<minimum:f64,maximum:f64,increment:f64,minimum_increment:f64,start:f64>"},
    {"plot_ref","PlotRef","ref<frog.graph.plot>"},
    // Exact private graph::Plot state; no binary/field-layout parity with NI.
    {"old_attributes","OldAttrib","cluster<id:string,name:string,rgb:u32,visible:bool,lines:bool,points:bool,width:f64>"},
    {"new_attributes","NewAttrib","cluster<id:string,name:string,rgb:u32,visible:bool,lines:bool,points:bool,width:f64>"},
    {"target_id","Target ID","string"}, {"graph_update","Graph Update","string"},
    {"discard","Discard?","bool"}, {"silent_shutdown","SilentShutdown","bool"},
    {"accepted","Accepted?","bool"}, {"drag_cursor","Cursor","ref<frog.cursor>"}};

std::vector<std::string_view> fieldIds(std::string_view list) {
    std::vector<std::string_view> result;
    while (!list.empty()) {
        const auto end=list.find(' ');
        result.push_back(list.substr(0,end));
        if(end==std::string_view::npos)break;
        list.remove_prefix(end+1);
    }
    return result;
}
bool hasField(std::string_view list,std::string_view id) noexcept {
    // No allocation in the admission path.
    while(!list.empty()) {
        const auto end=list.find(' ');
        if(list.substr(0,end)==id)return true;
        if(end==std::string_view::npos)break;
        list.remove_prefix(end+1);
    }
    return false;
}
std::string_view sourceReferenceId(EventSourceScope scope) noexcept {
    switch(scope) {
    case EventSourceScope::Control:return "control_ref";
    case EventSourceScope::Vi:return "program_ref";
    case EventSourceScope::Pane:return "pane_ref";
    case EventSourceScope::Application:return "application_ref";
    default:return {};
    }
}
std::string_view eventFilterSchemaFamily(std::string_view kind) noexcept {
    // Equal partial field lists cannot prove that unknown specialized payloads
    // are interchangeable. Only the explicitly shared keyboard family aliases.
    if(kind=="key_down_filter" || kind=="key_repeat_filter")return "keyboard_filter";
    return kind;
}
bool eventHasDataField(const DiagramEventBinding &binding,std::string_view id) noexcept {
    if(binding.kind=="user_event" && (id=="user_data" || id.starts_with("user_data/")))return true;
    if(diagramStructureShelfFieldById(diagramEventStructureDataFields(),id))return true;
    const auto *spec=eventNotificationSpec(binding.kind);
    if(binding.kind=="vi_activation" &&
        eventBindingSourceScope(binding)!=EventSourceScope::Application)return false;
    return spec && (hasField(spec->dataFields,id) ||
        (spec->sourceReference && id==sourceReferenceId(eventBindingSourceScope(binding))));
}
void addLimitation(EventDataAuthoringSchema &schema,std::string text) {
    if(std::find(schema.limitations.begin(),schema.limitations.end(),text)==schema.limitations.end())
        schema.limitations.push_back(std::move(text));
}
binding::TypeRef eventEnum(std::string_view domain,std::initializer_list<std::string_view> labels) {
    // Versioned authoring domains with FROG-local values, never NI API codes.
    auto type=binding::TypeRef::enumeration("frog.studio.event."+std::string(domain),binding::TypeRef::primitive("u32"));
    type.choiceKind=binding::ChoiceKind::Enum;
    for(const auto label:labels)type.choices.push_back({std::string(label),std::string(label),
        static_cast<std::uint32_t>(type.choices.size())});
    return type;
}
binding::TypeRef fieldType(std::string_view id,const EventSourceSpec &source) {
    if(id=="old_value" || id=="new_value")return source.valueType;
    if(id==sourceReferenceId(source.scope)) {
        const auto classId=source.referenceClassId.empty()?
            (source.scope==EventSourceScope::Control?"frog.control":source.scope==EventSourceScope::Vi?"frog.program":
                source.scope==EventSourceScope::Pane?"frog.front_panel":"frog.application"):source.referenceClassId;
        return binding::TypeRef::reference(classId,
            source.scope==EventSourceScope::Control && source.valueType.known()?std::optional{source.valueType}:std::nullopt);
    }
    if(id=="focus_object" || id=="sub_object")return binding::TypeRef::reference("frog.object");
    if(id=="program_window")return eventEnum("program_window.v1",{"front_panel","block_diagram"});
    if(id=="orientation")return eventEnum("wheel_orientation.v1",{"vertical","horizontal"});
    if(id=="window_action")return eventEnum("window_action.v1",{"resized","minimized","maximized"});
    if(id=="previous_window_state")return eventEnum("window_state.v1",{"standard","minimized","maximized"});
    if(id=="drag_result")return eventEnum("drag_result.v1",{"cancel","move","copy"});
    if(id=="scale_kind")return eventEnum("scale_kind.v1",{"x","y","z"});
    if(const auto *field=diagramStructureShelfFieldById(payloadFields,id))return binding::parseType(field->valueType).type;
    return {};
}
std::optional<binding::TypeRef> commonReferenceType(const binding::TypeRef &a,const binding::TypeRef &b) {
    if(a.kind!=binding::TypeKind::Reference || b.kind!=binding::TypeKind::Reference)return {};
    std::string common;
    if(binding::referenceClassAssignable(a.name,b.name))common=b.name;
    else if(binding::referenceClassAssignable(b.name,a.name))common=a.name;
    else for(const auto candidate:{"frog.control","frog.object"})
        if(binding::referenceClassAssignable(a.name,candidate) && binding::referenceClassAssignable(b.name,candidate)) {
            common=candidate;break;
        }
    if(common.empty())return {};
    const bool strict=a.children.size()==1 && b.children.size()==1 && a.children.front()==b.children.front();
    return binding::TypeRef::reference(std::move(common),strict?std::optional{a.children.front()}:std::nullopt);
}
}
bool isEventAuthoringDataField(std::string_view id) noexcept {
    if(id=="user_data" || id.starts_with("user_data/"))return true;
    if(diagramStructureShelfFieldById(diagramEventStructureDataFields(),id))return true;
    for(const auto &event:eventNotificationCatalog())
        if(hasField(event.dataFields,id))return true;
    return id=="control_ref" || id=="program_ref" || id=="pane_ref" || id=="application_ref";
}
bool isEventAuthoringResponseField(std::string_view id) noexcept {
    for(const auto &event:eventNotificationCatalog())
        if(hasField(event.responseFields,id))return true;
    return false;
}
bool eventNotificationAllowsNewBinding(const EventNotificationSpec &event) noexcept {
    return event.contractDiagnostic.empty();
}
bool eventNotificationAllowsNewBinding(const EventNotificationSpec &event,EventSourceScope scope) noexcept {
    if(scope==EventSourceScope::Dynamic)return eventNotificationAllowsNewBinding(event);
    if(event.kind=="vi_activation")return eventNotificationAllowsNewBinding(event) &&
        scope==EventSourceScope::Application;
    // NI Pane/notify Mouse Down, Mouse Up and Pane/filter Mouse Down?
    // explicitly declare PaneRef plus Coords/Button/Mods/PlatMods (and
    // Discard? for the filter). The private FROG logical coordinate/type
    // mappings remain explicit; no NI integer width or binary ABI is inferred.
    // Catalogue membership is wider than the verified source-specific payload.
    // Preserve historical adaptations, but do not author another incomplete
    // contract merely because this event has a verified Control definition.
    return eventNotificationAllowsNewBinding(event) &&
        !(event.sourceReference && ((scope==EventSourceScope::Pane &&
            event.kind!="pane_resize" && event.kind!="mouse_move" &&
            event.kind!="mouse_down" && event.kind!="mouse_down_filter" && event.kind!="mouse_up") ||
            (scope==EventSourceScope::Vi && event.kind.starts_with("mouse_"))));
}
bool eventNotificationAllowsNewBinding(const EventNotificationSpec &event,const EventSourceSpec &source) noexcept {
    return eventNotificationAllowsNewBinding(event,source.scope==EventSourceScope::Dynamic?
        source.registeredSourceScope:source.scope);
}
bool eventNotificationSupportsSource(const EventNotificationSpec &event, const EventSourceSpec &source) noexcept {
    if(event.kind.starts_with("xy_") && source.referenceClassId!=xy::classId)return false;
    if(source.scope==EventSourceScope::Dynamic && !source.registeredKind.empty())
        return event.kind==source.registeredKind &&
            (event.sourceScopes & scopeBit(source.registeredSourceScope))!=0 &&
            (event.controlCategory==EventControlCategory::Any || event.controlCategory==source.controlCategory);
    return (event.sourceScopes & scopeBit(source.scope)) != 0 &&
        (event.controlCategory == EventControlCategory::Any || event.controlCategory == source.controlCategory);
}
std::vector<EventSourceSpec> eventSources(FrogWidgetDataView canvas, bool resolveTypes) {
    // Source identities are independent of event names and translated captions.
    // The legacy "this_vi" binding still denotes the current .frog document.
    std::vector<EventSourceSpec> sources{
        {"", "Application", EventSourceScope::Application},
        {"this_vi", "This FROG", EventSourceScope::Vi},
        {"main_pane", "Main Pane", EventSourceScope::Pane}};
    // Same classes as ObjectReferenceRecord. Main Pane is currently the sole
    // FrontPanel object; no separate Pane/Splitter resource is fabricated.
    sources[0].referenceClassId="frog.application";
    sources[1].referenceClassId="frog.program";
    sources[2].referenceClassId="frog.front_panel";
    std::vector<const WidgetInstance *> ancestry;
    const auto append = [&](const auto &self, const WidgetInstance &widget,
            const std::string &parentId, const std::string &parentPath) -> void {
        // Ownership cycles cannot make opening the editor recurse forever.
        // Array prototypes and cells are not separate event sources.
        if (std::find(ancestry.begin(), ancestry.end(), &widget) != ancestry.end()) return;
        const auto *entry = findWidgetCatalogEntryByClassId(widget.classId);
        const auto label = !widget.label.empty() ? widget.label :
            entry ? entry->displayName : widget.role == "indicator" ? "Indicator" : "Control";
        const auto path = parentPath.empty() ? label : parentPath + " / " + label;
        const auto *cluster = clusterContainerState(widget);
        sources.push_back({widget.instanceId, label, EventSourceScope::Control,
            (isWaveformGraphWidget(widget)||isXYGraphWidget(widget)) ? EventControlCategory::WaveformGraph : EventControlCategory::Any,
            true, parentId, path, cluster != nullptr});
        if (resolveTypes) {
            const auto schema=binding::widgetType(widget);
            sources.back().valueType=schema.type;
            sources.back().valueTypeError=schema.error;
        }
        sources.back().referenceClassId=widget.classId;
        if (!cluster) return;
        ancestry.push_back(&widget);
        for (const auto &field : cluster->elements)
            if (field.widget) self(self, *field.widget, widget.instanceId, path);
        ancestry.pop_back();
    };
    for (const auto &object : canvas.objects()) {
        if (!object.widget || (object.widget->role != "control" && object.widget->role != "indicator")) continue;
        append(append, *object.widget, {}, {});
    }
    return sources;
}
std::vector<EventSourceSpec> eventStructureSources(const DiagramNode &,
    const binding::TypeRef &registrationType,FrogWidgetDataView canvas,bool resolveTypes) {
    auto sources=eventSources(canvas,resolveTypes);
    unsigned dynamicIndex=0;
    for(const auto &registered:eventRegistrationSources(registrationType)) {
        std::vector<const binding::TypeRef*> leaves;
        const auto gather=[&](const auto &self,const binding::TypeRef &type)->void {
            if(type.kind==binding::TypeKind::Array || type.kind==binding::TypeKind::Cluster)
                for(const auto &child:type.children)self(self,child);
            else leaves.push_back(&type);
        };
        gather(gather,registered.sourceType);
        if(leaves.empty())continue;
        const auto *type=leaves.front();
        EventSourceSpec source;
        source.id="dynamic:"+registered.id;
        source.label="Event Source "+std::to_string(++dynamicIndex);
        source.path=source.label;
        source.scope=EventSourceScope::Dynamic;
        source.registeredKind=registered.eventKind;
        source.registeredSourceScope=EventSourceScope::Dynamic;
        if(const auto *data=userEventDataType(*type)) {
            source.valueType=*data;
            source.label="User Event "+std::to_string(dynamicIndex);
        } else if(type->kind==binding::TypeKind::Reference) {
            source.referenceClassId=type->name;
            if(!type->children.empty())source.valueType=type->children.front();
            if(type->name=="frog.application")source.registeredSourceScope=EventSourceScope::Application;
            else if(type->name=="frog.program")source.registeredSourceScope=EventSourceScope::Vi;
            else if(type->name=="frog.front_panel" || type->name=="frog.pane")source.registeredSourceScope=EventSourceScope::Pane;
            else if(binding::referenceClassAssignable(type->name,"frog.control"))source.registeredSourceScope=EventSourceScope::Control;
            else source.available=false;
            if((type->name.find("waveform_graph")!=std::string::npos || type->name.find("xy_graph")!=std::string::npos))source.controlCategory=EventControlCategory::WaveformGraph;
        } else source.available=false;
        for(std::size_t index=1;index<leaves.size();++index) {
            const auto *other=leaves[index];
            if(const auto *data=userEventDataType(*other)) {
                if(source.registeredSourceScope!=EventSourceScope::Dynamic || !isUserEventReference(*type))source.available=false;
                if(source.valueType!=*data)source.valueType=binding::TypeRef::primitive("variant");
            } else if(other->kind==binding::TypeKind::Reference && type->kind==binding::TypeKind::Reference) {
                const auto common=commonReferenceType(*type,*other);
                if(!common)source.available=false;
                else source.referenceClassId=common->name;
                const auto otherData=other->children.empty()?binding::TypeRef{}:other->children.front();
                if(source.valueType!=otherData)source.valueType=binding::TypeRef::primitive("variant");
                if((other->name.find("waveform_graph")==std::string::npos && other->name.find("xy_graph")==std::string::npos))source.controlCategory=EventControlCategory::Any;
            } else source.available=false;
        }
        // Row IDs, not source labels, are the durable binding identity.
        source.path=source.label;
        sources.push_back(std::move(source));
    }
    return sources;
}
std::string validateEventBindingSources(const std::vector<DiagramEventBinding> &bindings,
    std::span<const EventSourceSpec> sources) {
    if (const auto error = validateEventBindings(bindings); !error.empty()) return error;
    for (const auto &binding : bindings) {
        const auto found = std::find_if(sources.begin(), sources.end(), [&](const auto &source) {
            return source.id == binding.source && source.scope == eventBindingSourceScope(binding);
        });
        const auto identity=std::string(eventSourceScopeName(eventBindingSourceScope(binding)))+":"+binding.source;
        if (found == sources.end() || !found->available)
            return "Event '"+binding.kind+"' source '"+identity+"' is unavailable.";
        if (std::count_if(sources.begin(),sources.end(),[&](const auto &source) {
                return source.id==binding.source && source.scope==eventBindingSourceScope(binding);}) != 1)
            return "Event '"+binding.kind+"' source '"+identity+"' has an ambiguous identity.";
        const auto *spec = eventNotificationSpec(binding.kind);
        if (!spec || !eventNotificationSupportsSource(*spec, *found))
            return "Event '"+binding.kind+"' is not available for source '"+identity+"'.";
    }
    return {};
}
bool isPublicEventDescriptorKind(std::string_view kind) noexcept {
    return kind == "timeout" || kind == "value_change" || kind == "user";
}
std::string validateEventBindings(const std::vector<DiagramEventBinding> &bindings) {
    if (bindings.empty()) return "Choose at least one event.";
    if (bindings.size() > 64) return "A case supports at most 64 event specifiers.";
    std::optional<bool> filter;
    for (std::size_t i = 0; i < bindings.size(); ++i) {
        const auto &binding = bindings[i];
        if (binding.kind.empty()) return "Choose a source and an event for every row.";
        if (!binding.sourceScope.empty() && binding.sourceScope != eventSourceScopeName(eventBindingSourceScope(binding)))
            return "Unknown event source scope.";
        const auto *spec = eventNotificationSpec(binding.kind);
        if (!spec) return "This event is not in the authoring catalogue.";
        if (filter && *filter!=spec->filter)
            return "Notifications and filter events cannot share the same Event case.";
        filter=spec->filter;
        const auto scope = eventBindingSourceScope(binding);
        if (scope!=EventSourceScope::Dynamic && (spec->sourceScopes & scopeBit(scope)) == 0) return "This event does not belong to the selected source.";
        if (scope == EventSourceScope::Application && !binding.source.empty()) return "Application events do not take a control source.";
        if (scope != EventSourceScope::Application && binding.source.empty()) return "This event requires a source.";
        if (std::any_of(bindings.begin(), bindings.begin() + static_cast<std::ptrdiff_t>(i),
                [&](const auto &other) { return sameEvent(other, binding); }))
            return "The same event is listed more than once.";
    }
    return {};
}
EventDataAuthoringSchema eventCaseDataSchema(const DiagramNode &node,
    std::string_view regionId, std::span<const EventSourceSpec> sources) {
    EventDataAuthoringSchema result;
    const auto fail=[&](const std::string &message) {
        result.fields.clear();result.responses.clear();result.complete=false;
        result.error="Event '"+node.id+"' region '"+std::string(regionId)+"': "+message;
        return result;
    };
    if(node.type!="frog.studio.structures.event_structure")return fail("The owner is not an Event Structure.");
    const auto found=std::find_if(node.eventStructureCases.begin(),node.eventStructureCases.end(),
        [&](const auto &candidate) {return candidate.id==regionId;});
    if(found==node.eventStructureCases.end())return fail("The region does not exist.");
    if(std::count_if(node.eventStructureCases.begin(),node.eventStructureCases.end(),
            [&](const auto &candidate) {return candidate.id==regionId;})!=1)
        return fail("The region identity is ambiguous.");
    const auto bindings=eventCaseBindings(*found);
    if(const auto error=validateEventBindingSources(bindings,sources);!error.empty())return fail(error);
    bool first=true;
    std::optional<EventSourceScope> firstScope;
    const auto contextual=[&](const DiagramEventBinding &binding,std::string_view detail) {
        return "Event '"+node.id+"' region '"+std::string(regionId)+"' source '"+
            std::string(eventSourceScopeName(eventBindingSourceScope(binding)))+":"+binding.source+
            "' kind '"+binding.kind+"': "+std::string(detail);
    };
    for(const auto &binding:bindings) {
        const auto source=std::find_if(sources.begin(),sources.end(),[&](const auto &candidate) {
            return candidate.id==binding.source && candidate.scope==eventBindingSourceScope(binding);});
        const auto *spec=eventNotificationSpec(binding.kind);
        auto effectiveSource=*source;
        if(source->scope==EventSourceScope::Dynamic)
            effectiveSource.scope=source->registeredSourceScope;
        std::vector<EventDataAuthoringField> fields,responses;
        for(const auto &field:diagramEventStructureDataFields())
            fields.push_back({std::string(field.id),std::string(field.label),binding::parseType(field.valueType).type});
        if(binding.kind=="value_change" && (!source->valueTypeError.empty() || !source->valueType.known()))
            return fail("Value Change source '"+binding.source+"' has no valid value schema"+
                (source->valueTypeError.empty()?std::string{"."}:std::string{": "}+source->valueTypeError));
        auto append=[&](auto &target,std::string_view id,bool response) {
            const auto *field=diagramStructureShelfFieldById(payloadFields,id);
            if(!field)return;
            auto type=fieldType(id,effectiveSource);
            if(!type.known()) {
                result.complete=false;
                addLimitation(result,contextual(binding,std::string(field->label)+
                    ": schema unavailable; its specialized type/reference is absent from the common FROG system."));
            }
            auto policy=EventResponseUnwiredPolicy::PreserveIncoming;
            // FROG v1: no explicit drop acceptance means refusal. A missing
            // SilentShutdown response preserves the occurrence's incoming
            // request, like every other rewritable field; it never guesses a
            // native application's confirmation or save policy.
            if(response && (id=="discard" || id=="accepted"))policy=EventResponseUnwiredPolicy::False;
            else if(response && id=="drag_cursor")policy=EventResponseUnwiredPolicy::NativeFeedback;
            if(response && policy==EventResponseUnwiredPolicy::Undeclared) {
                result.complete=false;
                addLimitation(result,contextual(binding,std::string(field->label)+
                    ": schema unavailable for the unwired response policy; no default or native action is inferred."));
            }
            target.push_back({std::string(id),std::string(field->label),std::move(type),policy});
        };
        // Preserve the historical VI-scoped subset without manufacturing
        // Application occurrences or changing its existing field identities.
        const bool legacyActivation=binding.kind=="vi_activation" &&
            effectiveSource.scope!=EventSourceScope::Application;
        if(!legacyActivation) {
            if(spec->sourceReference)append(fields,sourceReferenceId(effectiveSource.scope),false);
            for(const auto id:fieldIds(spec->dataFields))append(fields,id,false);
            for(const auto id:fieldIds(spec->responseFields))append(responses,id,true);
        }
        if(binding.kind=="user_event") {
            if(!source->valueType.known())return fail("User Event source '"+binding.source+"' has no complete payload schema.");
            fields.push_back({"user_data","Data",source->valueType});
            const auto appendChildren=[&](const auto &self,const binding::TypeRef &type,
                    const std::string &path,const std::string &caption)->void {
                if(type.kind!=binding::TypeKind::Cluster)return;
                for(std::size_t index=0;index<type.children.size();++index) {
                    const auto childPath=path+"/"+eventPayloadFieldComponent(type.fieldIds[index]);
                    const auto name=index<type.fieldNames.size() && !type.fieldNames[index].empty()?
                        type.fieldNames[index]:"Element "+std::to_string(index+1);
                    const auto label=caption.empty()?name:caption+" / "+name;
                    fields.push_back({childPath,label,type.children[index]});
                    self(self,type.children[index],childPath,label);
                }
            };
            appendChildren(appendChildren,source->valueType,"user_data",{});
        }
        if(!spec->contractDiagnostic.empty()) {
            result.complete=false;
            addLimitation(result,contextual(binding,spec->contractDiagnostic));
        }
        // The historical UI offers wider scope families than the supplied NI
        // control contracts. Preserve those bindings without calling their
        // source-specific substitution a verified or executable contract.
        if(eventNotificationAllowsNewBinding(*spec) && !eventNotificationAllowsNewBinding(*spec,effectiveSource.scope)) {
            result.complete=false;
            addLimitation(result,contextual(binding,"schema unavailable: this source-class payload is an unverified authoring adaptation; execution is unavailable."));
        }
        if(first) {
            result.fields=fields;firstScope=source->scope;
            result.responses=responses;
            first=false;
        } else {
            if((!result.responses.empty() || !responses.empty()) &&
                (result.responses.size()!=responses.size() ||
                !std::equal(result.responses.begin(),result.responses.end(),responses.begin(),responses.end(),
                    [](const auto &a,const auto &b) {return a.id==b.id && a.type==b.type && a.unwiredPolicy==b.unwiredPolicy;})))
                return fail("Events with response protocols must expose identical response fields and unwired policies.");
            // Filters require the same input schema; no intersection silently
            // removes a field that another filter occurrence needs to return.
            if(spec->filter && (!result.complete || eventFilterSchemaFamily(bindings.front().kind)!=eventFilterSchemaFamily(binding.kind) ||
                firstScope!=source->scope || result.fields.size()!=fields.size() ||
                !std::equal(result.fields.begin(),result.fields.end(),fields.begin(),fields.end(),
                    [](const auto &a,const auto &b) {return a.id==b.id && a.type==b.type;}) ||
                result.responses.size()!=responses.size() ||
                !std::equal(result.responses.begin(),result.responses.end(),responses.begin(),responses.end(),
                    [](const auto &a,const auto &b) {return a.id==b.id && a.type==b.type && a.unwiredPolicy==b.unwiredPolicy;})))
                return fail("Filter events must share a declared schema family, identical data types and source scopes.");
            for(auto current=result.fields.begin();current!=result.fields.end();) {
                auto &field=*current;
                const auto other=std::find_if(fields.begin(),fields.end(),[&](const auto &item) {return item.id==field.id;});
                if(other==fields.end()) {current=result.fields.erase(current);continue;}
                if(field.type==other->type) {++current;continue;}
                if(const auto reference=commonReferenceType(field.type,other->type)) {
                    field.type=*reference;++current;continue;
                }
                // The only heterogeneous-value rule is Value Change. Other
                // data must share both identity and exact type to survive.
                if(field.id=="old_value" || field.id=="new_value") {
                    field.type=binding::TypeRef::primitive("variant");++current;continue;
                }
                current=result.fields.erase(current);
            }
        }
    }
    result.runtimeAvailable=std::all_of(bindings.begin(),bindings.end(),[](const auto &binding) {
        return binding.kind=="timeout" || (binding.kind=="user_event" && eventBindingSourceScope(binding)==EventSourceScope::Dynamic);
    });
    for(const auto &binding:bindings)if(binding.kind!="user_event" && binding.kind!="timeout")
        addLimitation(result,contextual(binding,"Configured authoring schema; occurrence capture, filter dispatch and source/FIR execution are unavailable."));
    return result;
}
std::string validateEventCaseConfiguration(const DiagramNode &node, std::string_view replacedRegion,
    const DiagramEventCase &configured) {
    const auto bindings = eventCaseBindings(configured);
    if (const auto error = validateEventBindings(bindings); !error.empty()) return error;
    const auto original=std::find_if(node.eventStructureCases.begin(),node.eventStructureCases.end(),
        [&](const auto &candidate){return candidate.id==replacedRegion;});
    const auto previous=original==node.eventStructureCases.end()?std::vector<DiagramEventBinding>{}:eventCaseBindings(*original);
    for(const auto &binding:bindings) {
        const auto *spec=eventNotificationSpec(binding.kind);
        if(spec && !eventNotificationAllowsNewBinding(*spec,eventBindingSourceScope(binding)) &&
            std::none_of(previous.begin(),previous.end(),[&](const auto &old){return sameEvent(old,binding);}))
            return "New associations require a verified authoring payload/response contract. Existing bindings are preserved.";
    }
    // Use the same private catalogue as regional type resolution and the menu.
    for (const auto &field : configured.eventDataFields)
        if (!std::all_of(bindings.begin(),bindings.end(),[&](const auto &binding) {
                return eventHasDataField(binding,field);}))
            return "This field is not available for the configured event intersection.";
    if (configured.eventResponseFields && configured.eventResponseFields->size()>16)
        return "The response shelf supports at most sixteen authoring rows.";
    if (configured.eventResponseFields) for (std::size_t index = 0;
            index < configured.eventResponseFields->size(); ++index) {
        const auto &id = (*configured.eventResponseFields)[index];
        // Repeated authoring rows have independent port occurrences. Actual
        // event dispatch and arbitration still require their runtime contract.
        if (!std::all_of(bindings.begin(), bindings.end(), [&](const auto &binding) {
                const auto *spec = eventNotificationSpec(binding.kind);
                return spec && hasField(spec->responseFields,id); }))
            return "This response is not available for the configured event intersection.";
    }
    for (const auto &other : node.eventStructureCases) if (other.id != replacedRegion) {
        const auto occupied = eventCaseBindings(other);
        for (const auto &binding : bindings) if (std::any_of(occupied.begin(), occupied.end(),
                [&](const auto &otherBinding) { return sameEvent(otherBinding, binding); }))
            return "This event is already handled by another case in this structure.";
    }
    return {};
}
}
