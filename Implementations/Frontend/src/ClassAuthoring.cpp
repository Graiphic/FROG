// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/ClassAuthoring.hpp"
#include "frog_native_engine/WidgetCatalog.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/FrogDocumentWidgetValues.hpp"
#include "frog_native_engine/DraftInterfaceInspection.hpp"
#include "frog_native_engine/Sha256.hpp"
#include <algorithm>
#include <stdexcept>

namespace frog::native_engine {
namespace {
using binding::TypeRef;
using binding::TypeKind;
WidgetInstance carrier(const TypeRef &type,const std::string &id,bool input,unsigned depth=0) {
    if(depth>32)throw std::runtime_error("Member connector type nesting exceeds32.");
    const auto classId=type.kind==TypeKind::Object?"frog.widgets.variant_control":type.kind==TypeKind::Cluster?"frog.widgets.cluster_control":
        type.name=="bool"?"frog.widgets.boolean_control":type.name=="string"?"frog.widgets.string_control":type.name=="path"?"frog.widgets.path_control":
        "frog.widgets.numeric_control";
    const auto *entry=findWidgetCatalogEntryByClassId(classId);
    if(!entry)throw std::runtime_error("The native carrier widget is unavailable: "+std::string(classId));
    auto widget=makeWidgetData(id,*entry,{0,0,entry->defaultPreviewSize.width,entry->defaultPreviewSize.height});
    widget.role=input?"control":"indicator";widget.valueType=type.canonical();widget.label=id;
    if(type.kind==TypeKind::Cluster) {
        auto &state=widget.clusterContainer.emplace();
        for(std::size_t i=0;i<type.children.size();++i) {
            auto child=carrier(type.children[i],id+".field."+std::to_string(i),input,depth+1);
            child.label=type.fieldNames.size()==type.children.size()?type.fieldNames[i]:type.fieldIds[i];
            state.elements.push_back({type.fieldIds[i],child.label,static_cast<std::uint32_t>(i),std::make_shared<WidgetInstance>(std::move(child))});
        }
    } else if(type.kind==TypeKind::Primitive) {
        if(type.name=="bool"){widget.booleanValue=false;widget.initialValue="false";}
        else if(type.name=="string"||type.name=="path")widget.initialValue="";
        else if(type.integer()) {
            if(!setWidgetNumericIntegerValueLexeme(widget,"0"))throw std::runtime_error("The exact native connector carrier refused its integer type.");
        } else if(type.name=="f64") {
            if(!setWidgetNumericValue(widget,0))throw std::runtime_error("The native connector carrier refused f64.");
        } else throw std::runtime_error("This method connector type has no admitted native carrier.");
    } else if(type.kind!=TypeKind::Object)throw std::runtime_error("This method connector type has no admitted native carrier.");
    const auto actual=binding::widgetType(widget);
    if(!actual.valid()||actual.type!=type)throw std::runtime_error("The actual native widget carrier differs from the method port type.");
    return widget;
}
}
std::optional<WidgetInstance> makeClassPortCarrier(const TypeRef &type,const std::string &id,bool input,ClassIssue *issue) {
    try{return carrier(type,id,input);}
    catch(const std::exception &failure){if(issue)*issue={"class.carrier_unavailable",{},"/interface",failure.what(),{}};return {};}
}
ClassMemberDraft makeClassMemberDraft(const ClassDefinition &definition,std::string_view methodId,const ClassDependency &ownReference) {
    ClassMemberDraft result;
    const auto found=std::find_if(definition.methods.begin(),definition.methods.end(),[&](const auto &m){return m.id==methodId;});
    const auto issue=[&](std::string message){result.issues.push_back({"class.member_authoring",definition.id,"/methods",std::move(message),{}});};
    if(found==definition.methods.end()||ownReference.id!=definition.id||ownReference.revision!=definition.revision) {
        issue("The requested member or class reference does not match the actual definition.");return result;
    }
    if(!readClassDependency(writeClassSourceOperation({ClassOperationKind::Default,ownReference,{}}).object.at("class"),kClassDefinitionExtension)) {
        issue("The class reference is not a persistent, admitted dependency.");return result;
    }
    try {
        const auto &method=*found;FrogDraftData document;document.title=method.name;
        document.properties.classSource.classes={ownReference};
        document.properties.classSource.member=ClassMemberIdentity{definition.id,method.id,method.implementation.id,method.implementation.revision};
        document.declaredInterface.emplace();
        unsigned inputs=0,outputs=0;
        for(const auto &p:method.ports) {
            const auto type=p.kind==ClassPortKind::Object?TypeRef::object(definition.id):p.dataType;
            const auto id="member-port."+p.publicPortId;
            auto widget=carrier(type,id,p.input);widget.label=p.slot;
            widget.interfaceMapSlotId="slot."+p.publicPortId;
            auto &row=p.input?inputs:outputs;
            widget.bounds.x=p.input?40.0:400.0;widget.bounds.y=60.0+120.0*row++;
            if(p.input){widget.publicInputId=p.publicPortId;widget.interfaceConnectionRequirement="required";}
            else widget.publicOutputId=p.publicPortId;
            widget.defaultData=serializeArrayElementWidget(widget);
            if(!document.declaredInterface->declarePort(p.input,p.publicPortId,type.canonical(),p.input?"required":""))
                throw std::runtime_error("The ordinary interface owner refused the actual method port.");
            DiagramNode terminal;terminal.id="terminal."+p.publicPortId;terminal.kind="widget_value";
            terminal.widgetId=widget.instanceId;terminal.type=diagramTerminalValueType(widget);
            terminal.layout.x=p.input?100.0:700.0;terminal.layout.y=100.0+120.0*(row-1);
            document.widgets.addObject({widget.instanceId,widget.bounds,std::move(widget)});
            document.diagramNodes.push_back(std::move(terminal));
        }
        const auto connector=inspectFrogDraftInterface(*document.declaredInterface);
        ClassImplementationInterface actual{method.implementation.id,method.implementation.revision,{}};
        for(const auto &p:connector.ports)actual.ports.push_back({p.id,p.input,
            p.parsedType.type.kind==TypeKind::Object?ClassPortKind::Object:ClassPortKind::Data,
            p.parsedType.type.kind==TypeKind::Object?TypeRef{}:p.parsedType.type,
            p.parsedType.type.kind==TypeKind::Object?p.parsedType.type.name:std::string{}});
        result.issues=validateClassImplementation(definition,static_cast<std::size_t>(found-definition.methods.begin()),actual);
        if(result.issues.empty())result.document=std::move(document);
    } catch(const std::exception &failure){issue(failure.what());}
    return result;
}
std::optional<ClassMethod> makeClassOverride(const ClassHierarchy &hierarchy,const ClassDefinition &child,
    const ClassCallTarget &original,std::string methodId,ClassDependency implementation,ClassIssue *issue) {
    const auto fail=[&](std::string message)->std::optional<ClassMethod>{
        if(issue)*issue={"class.override_authoring",child.id,"/methods",std::move(message),{}};return {};
    };
    const auto *base=hierarchy.findMethod(original);
    if(!base||base->dispatch!=ClassDispatch::Dynamic||base->overrides||base->visibility==ClassVisibility::Private||
        !child.parent||!hierarchy.assignable(child.parent->id,original.classId))
        return fail("Only an accessible inherited original dynamic slot can be overridden.");
    if(std::any_of(child.methods.begin(),child.methods.end(),[&](const auto &m){return m.id==methodId||
        (m.overrides&&*m.overrides==ClassOverride{original.classId,original.methodId});}))return fail("The child already declares this member or override slot.");
    ClassMethod result=*base;result.id=std::move(methodId);result.implementation=std::move(implementation);
    result.overrides=ClassOverride{original.classId,original.methodId};
    for(auto &p:result.ports)p.publicPortId="port."+hashing::sha256Hex(result.id+":"+p.slot);
    return result;
}
std::optional<ClassDefinition> adoptClassMemberConnector(const ClassDefinition &definition,std::string_view methodId,
    const FrogDraftData &source,std::vector<ClassIssue> &issues) {
    issues.clear();
    const auto fail=[&](std::string message)->std::optional<ClassDefinition>{
        issues.push_back({"class.connector_authoring",definition.id,"/methods",std::move(message),{}});return {};
    };
    auto result=definition;
    const auto method=std::find_if(result.methods.begin(),result.methods.end(),[&](const auto &m){return m.id==methodId;});
    if(method==result.methods.end()||!source.properties.classSource.member||!source.declaredInterface)
        return fail("The actual member and its explicit connector are required.");
    const auto &identity=*source.properties.classSource.member;
    if(identity.classId!=definition.id||identity.methodId!=methodId||identity.implementationId!=method->implementation.id||identity.revision!=method->implementation.revision)
        return fail("The actual source does not carry this member's stable identity/revision.");
    const auto actual=inspectFrogDraftInterface(*source.declaredInterface);
    const auto bindings=inspectFrogDraftInterfaceBindings(source.declaredInterface,source.widgets,source.diagramNodes,source.properties,{});
    if(!actual.contractValid||!actual.inspectable||!bindings.empty())return fail("The actual member connector/bindings are incomplete or invalid.");
    const auto previous=method->ports;method->ports.clear();
    for(const auto &port:actual.ports) {
        if(!port.parsedType.valid()||!port.parsedType.type.known()||(port.input&&port.connection!="required"))
            return fail("Each member input requires its actual complete type and required caller producer.");
        const auto &type=port.parsedType.type;
        if(type.kind==TypeKind::Object&&type.name!=definition.id)return fail("Object member terminals must specialize the declaring class.");
        const auto old=std::find_if(previous.begin(),previous.end(),[&](const auto &p){return p.publicPortId==port.id;});
        method->ports.push_back({old==previous.end()?"arg."+hashing::sha256Hex(port.id):old->slot,port.id,port.input,
            type.kind==TypeKind::Object?ClassPortKind::Object:ClassPortKind::Data,type.kind==TypeKind::Object?TypeRef{}:type});
    }
    std::string error;auto checked=readClassDefinition(writeClassDefinition(result),&error);
    if(!checked)return fail(error);
    return checked;
}
}
