// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/DraftClassProjection.hpp"
#include "frog_native_engine/ClassAuthoring.hpp"
#include "frog_native_engine/ClusterFunctionBinding.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include "frog_native_engine/DiagramTunnelIdentity.hpp"
#include "frog_native_engine/DiagramStructureCatalog.hpp"
#include "frog_native_engine/DraftInterfaceInspection.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <set>
#include <stdexcept>

namespace frog::native_engine {
namespace {
using Json=source::JsonValue;
namespace cj=canonical_json;
using binding::TypeRef;
using binding::TypeKind;
std::string classType(std::string_view id){return TypeRef::object(std::string(id)).canonical();}
std::string nodePath(const Json &scope,std::string_view id,std::string prefix="/diagram",bool edge=false) {
    if(edge)if(const auto *edges=scope.member("edges");edges&&edges->type==Json::Type::Array)
        for(std::size_t e=0;e<edges->array.size();++e)if(const auto *found=edges->array[e].member("id");found&&found->type==Json::Type::String&&found->string==id)
            return prefix+"/edges/"+std::to_string(e);
    if(const auto *nodes=scope.member("nodes");nodes&&nodes->type==Json::Type::Array)
        for(std::size_t n=0;n<nodes->array.size();++n) {
            const auto &node=nodes->array[n];const auto at=prefix+"/nodes/"+std::to_string(n);
            if(const auto *found=node.member("id");!edge&&found&&found->type==Json::Type::String&&found->string==id)return at;
            if(const auto *regions=node.member("regions");regions&&regions->type==Json::Type::Array)
                for(std::size_t r=0;r<regions->array.size();++r)if(const auto *diagram=regions->array[r].member("diagram"))
                    if(auto result=nodePath(*diagram,id,at+"/regions/"+std::to_string(r)+"/diagram",edge);!result.empty())return result;
        }
    return {};
}
const ClassField *field(const ClassDefinition &owner,std::string_view id) {
    const auto found=std::find_if(owner.fields.begin(),owner.fields.end(),[&](const auto &f){return f.id==id;});
    return found==owner.fields.end()?nullptr:&*found;
}
const Json *rawNode(const Json &scope,std::string_view id) {
    if(const auto *nodes=scope.member("nodes");nodes&&nodes->type==Json::Type::Array)
        for(const auto &node:nodes->array) {
            if(const auto *found=node.member("id");found&&found->type==Json::Type::String&&found->string==id)return &node;
            if(const auto *regions=node.member("regions");regions&&regions->type==Json::Type::Array)
                for(const auto &region:regions->array)if(const auto *diagram=region.member("diagram"))
                    if(const auto *found=rawNode(*diagram,id))return found;
        }
    return nullptr;
}
DiagramCallPort port(std::string id,std::string type,bool input) {
    DiagramCallPort p;p.id=id;p.sourcePortId=std::move(id);p.valueType=std::move(type);p.input=input;
    p.connectionRequirement=input?"required":"recommended";p.normalizedX=input?0.0:1.0;return p;
}
struct Invalid {ClassIssue issue;};
}
bool clusterContainsClassValue(const binding::TypeRef &type) {
    if(type.kind!=binding::TypeKind::Cluster)return false;
    return std::any_of(type.children.begin(),type.children.end(),[](const auto &child) {
        return child.kind==binding::TypeKind::Object || clusterContainsClassValue(child);
    });
}
bool draftHasClasses(const FrogDraftData &document) {
    if(!document.properties.classSource.classes.empty()||document.properties.classSource.member)return true;
    return std::any_of(document.diagramNodes.begin(),document.diagramNodes.end(),[](const auto &n){return n.classOperation.has_value();});
}
CapturedClassProgram captureDraftClassProgram(const FrogDraftData &document,std::string_view uri,const DraftDependencyResolver &resolver) {
    std::vector<ClassDependencyRoot> roots;
    for(std::size_t i=0;i<document.properties.classSource.classes.size();++i)
        roots.push_back({std::string(uri),"/classes/"+std::to_string(i),document.properties.classSource.classes[i]});
    for(const auto &node:document.diagramNodes)if(node.classOperation)
        roots.push_back({std::string(uri),"/diagram/"+node.id+"/call/class_operation/class",node.classOperation->owner});
    return captureClassProgram(roots,resolver);
}
std::optional<std::vector<DiagramCallPort>> classOperationPorts(const ClassSourceOperation &operation,
    const CapturedClassProgram &program,ClassIssue *issue) {
    const auto fail=[&](std::string code,std::string message)->std::optional<std::vector<DiagramCallPort>> {
        if(issue)*issue={std::move(code),operation.owner.id,"/call/class_operation",std::move(message),{}};return {};
    };
    if(!program.accepted())return fail("class.program_unavailable","The class/member closure has not been admitted.");
    const auto *owner=program.classes.hierarchy->findClass(operation.owner.id);
    if(!owner||owner->revision!=operation.owner.revision)return fail("class.dependency_identity","The class definition identity or revision differs.");
    std::vector<DiagramCallPort> ports;
    switch(operation.kind) {
    case ClassOperationKind::Default:ports.push_back(port("value",classType(owner->id),false));break;
    case ClassOperationKind::ReadField:
    case ClassOperationKind::WriteField: {
        const auto *value=field(*owner,operation.memberId);
        if(!value)return fail("class.field_missing","The selected declaring class has no such private field.");
        const bool update=operation.kind==ClassOperationKind::WriteField;
        ports={port("object",classType(owner->id),true),port("value",value->type.canonical(),update)};
        if(update)ports.push_back(port("updated",classType(owner->id),false));break;
    }
    case ClassOperationKind::StaticCall:
    case ClassOperationKind::DynamicCall: {
        const auto *method=program.classes.hierarchy->findMethod({owner->id,operation.memberId});
        if(!method)return fail("class.method_missing","The selected class has no such method declaration.");
        const auto expected=operation.kind==ClassOperationKind::DynamicCall?ClassDispatch::Dynamic:ClassDispatch::Static;
        if(method->dispatch!=expected||(expected==ClassDispatch::Dynamic&&method->overrides))
            return fail("class.dispatch_mode","The call must name the original slot and its declared dispatch mode.");
        for(const auto &p:method->ports) {
            auto boundary=port(p.publicPortId,p.kind==ClassPortKind::Object?classType(owner->id):p.dataType.canonical(),p.input);
            boundary.label=p.slot;ports.push_back(std::move(boundary));
        }
        break;
    }
    }
    std::size_t inputs=0,outputs=0;for(const auto &p:ports)(p.input?inputs:outputs)++;
    std::size_t in=0,out=0;for(auto &p:ports)p.normalizedY=double(p.input?++in:++out)/double(1+(p.input?inputs:outputs));
    return ports;
}

DraftClassProjection projectDraftClassOperations(const FrogDraftData &authored,const Json &original,
    const CapturedClassProgram &program,std::string_view uri,std::string_view digest,const DraftDependencyResolver &dependencies,bool root) {
    DraftClassProjection result;result.document=authored;
    if(!program.accepted()) {
        result.issues=program.classes.issues;result.issues.insert(result.issues.end(),program.issues.begin(),program.issues.end());return result;
    }
    const auto &hierarchy=*program.classes.hierarchy;
    const auto caller=program.attributeMember(uri,digest);
    const auto path=[&](std::string_view id){const auto *diagram=original.member("diagram");return diagram?nodePath(*diagram,id):std::string{};};
    const auto wirePath=[&](std::string_view id){const auto *diagram=original.member("diagram");return diagram?nodePath(*diagram,id,"/diagram",true):std::string{};};
    const auto refuse=[&](std::string code,std::string message,std::string pointer,std::string classId={}) {
        throw Invalid{{std::move(code),std::move(classId),std::move(pointer),std::move(message),std::string(uri)}};
    };
    try {
        // Every authored location still resolves to the same frozen definition.
        // An ID copied beside a different path is not dependency attestation.
        std::vector<ClassDependencyRoot> roots;
        for(std::size_t i=0;i<authored.properties.classSource.classes.size();++i)
            roots.push_back({std::string(uri),"/classes/"+std::to_string(i),authored.properties.classSource.classes[i]});
        for(const auto &node:authored.diagramNodes)if(node.classOperation)
            roots.push_back({std::string(uri),path(node.id)+"/call/class_operation/class",node.classOperation->owner});
        if(!roots.empty()) {
            const auto local=captureClassHierarchy(roots,dependencies);
            if(!local.hierarchy||!local.issues.empty())throw Invalid{local.issues.empty()?ClassIssue{"class.closure_required",{},"/classes","Missing declared class closure.",std::string(uri)}:local.issues.front()};
            for(const auto &definition:local.definitions) {
                const auto found=std::find_if(program.classes.definitions.begin(),program.classes.definitions.end(),[&](const auto &d){return d.identity==definition.identity;});
                if(found==program.classes.definitions.end()||found->canonicalUri!=definition.canonicalUri||found->sha256!=definition.sha256)
                    refuse("class.dependency_changed","A member references a different class definition than the root's frozen closure.","/classes",definition.identity);
            }
        }
        if(authored.properties.classSource.member) {
            const auto &m=*authored.properties.classSource.member;
            if(!caller||*caller!=ClassCallTarget{m.classId,m.methodId})
                refuse("class.member_unattributed","Private access requires the actual member URI and bytes captured from its .fclass declaration.","/class_member",m.classId);
        }
        const auto declaration=authored.declaredInterface?inspectFrogDraftInterface(*authored.declaredInterface):FrogDraftInterfaceInspection{};
        for(const auto &object:authored.widgets.objects())if(object.widget) {
            const auto schema=binding::widgetType(*object.widget);
            if(schema.type.kind!=TypeKind::Object && !clusterContainsClassValue(schema.type))continue;
            // An authored Variant payload cannot manufacture an object. The
            // first profile accepts object interface carriers only at actual
            // call boundaries; entry objects come from checked defaults.
            const auto &widget=*object.widget;
            const auto bound=std::find_if(declaration.ports.begin(),declaration.ports.end(),[&](const auto &p){
                const auto &id=p.input?widget.publicInputId:widget.publicOutputId;
                // Interface type expressions identify Enum domains and representations.
                // Their labels and values come from the actual bound widget.
                return id && *id==p.id && schema.valid() && schema.type.known() &&
                    schema.type.canonical()==p.parsedType.type.canonical();
            });
            if(root||bound==declaration.ports.end())refuse("class.object_factory_required",
                "Entry objects require a checked default-object operation; an object interface carrier requires an actual caller.","/frontPanel/"+object.id,schema.type.name);
        }
        for(const auto &node:authored.diagramNodes)if(node.classOperation) {
            const auto &op=*node.classOperation;const auto at=path(node.id);
            const auto *diagram=original.member("diagram");const auto *raw=diagram?rawNode(*diagram,node.id):nullptr;
            const auto *call=raw?raw->member("call"):nullptr;
            if(!call||call->type!=Json::Type::Object)refuse("class.source_invalid","The exact class operation source is absent.",at);
            for(const auto &[key,_]:call->object)if(key!="document_path"&&key!="document_title"&&key!="icon_svg"&&key!="ports"&&key!="class_operation")
                refuse("class.source_unknown_field","This call field has no class-operation contract.",at+"/call/"+key);
            const auto *rawPorts=call->member("ports");
            if(!rawPorts||rawPorts->type!=Json::Type::Array||rawPorts->array.size()!=node.callPorts.size())
                refuse("class.connector_invalid","The raw connector contains invalid or duplicated ports.",at+"/call/ports");
            std::set<std::string> rawIds;
            for(const auto &p:rawPorts->array) {
                if(p.type!=Json::Type::Object)refuse("class.connector_invalid","A connector entry must be an object.",at+"/call/ports");
                for(const auto &[key,_]:p.object)if(key!="id"&&key!="source_port_id"&&key!="interface_map_slot"&&key!="label"&&key!="value_type"&&key!="direction"&&key!="connection_requirement"&&key!="normalized_x"&&key!="normalized_y")
                    refuse("class.source_unknown_field","This connector field has no class-operation contract.",at+"/call/ports/"+key);
                const auto *id=p.member("id"),*direction=p.member("direction");
                if(!id||id->type!=Json::Type::String||!rawIds.insert(id->string).second||!direction||direction->type!=Json::Type::String||
                    (direction->string!="input"&&direction->string!="output"))
                    refuse("class.connector_invalid","Stable connector IDs and exact directions are required.",at+"/call/ports");
            }
            if(node.kind!="frog_call"||node.polymorphicFrogSelection||!node.callDocumentPath.empty())
                refuse("class.operation_owner","A class operation is distinct from a file call or signature-polymorphic family.",at,op.owner.id);
            ClassIssue issue;const auto expected=classOperationPorts(op,program,&issue);
            if(!expected)refuse(issue.code,issue.message,at+"/call/class_operation",op.owner.id);
            if(expected->size()!=node.callPorts.size())refuse("class.connector_stale","Class operation connector differs from its actual declaration.",at+"/call/ports",op.owner.id);
            for(const auto &p:*expected) {
                const auto found=std::find_if(node.callPorts.begin(),node.callPorts.end(),[&](const auto &q){return q.id==p.id;});
                if(found==node.callPorts.end()||found->sourcePortId!=p.sourcePortId||found->input!=p.input||
                    found->valueType!=p.valueType||(p.input&&found->connectionRequirement!="required"))
                    refuse("class.connector_stale","Stable port identity, direction or nominal type has changed.",at+"/call/ports",op.owner.id);
            }
            if(op.kind==ClassOperationKind::ReadField||op.kind==ClassOperationKind::WriteField) {
                if(!caller||!hierarchy.canAccessField(*caller,op.owner.id,op.memberId))
                    refuse("class.private_access","Only an actual method of the declaring class may directly access its private field.",at+"/call/class_operation/member_id",op.owner.id);
            } else if(op.kind==ClassOperationKind::StaticCall||op.kind==ClassOperationKind::DynamicCall) {
                if(!hierarchy.canAccess(caller,{op.owner.id,op.memberId}))
                    refuse("class.method_access","The caller is outside the method's public/protected/private scope.",at+"/call/class_operation/member_id",op.owner.id);
            }
        }
        binding::TypeResolver resolver(authored.diagramNodes,authored.diagramEdges,authored.widgets,{},&hierarchy);
        const auto permissible=[&](const DiagramEdgeEndpoint &endpoint,const TypeRef &nominal) {
            const auto n=std::find_if(authored.diagramNodes.begin(),authored.diagramNodes.end(),[&](const auto &v){return v.id==endpoint.nodeId;});
            if(n==authored.diagramNodes.end())return false;
            if(n->kind=="frog_call"||n->kind=="widget_value"||n->type=="frog.studio.structures.select")return true;
            if(clusterContainsClassValue(nominal)) {
                if(n->type=="frog.studio.cluster.constant" && endpoint.portId=="value")return true;
                if((n->type=="frog.studio.cluster.bundle_by_name" || n->type=="frog.studio.cluster.unbundle_by_name") &&
                   (endpoint.portId=="cluster" || endpoint.portId=="result")) {
                    // Resolve each selection before lowering. A private field inside
                    // an Object cannot become a selectable field of its carrier.
                    for(unsigned row=0;row<clusterFunctionCount(*n);++row)
                        if(!clusterSelectedField(*n,nominal,row))return false;
                    return true;
                }
            }
            return isDiagramStructureNodeType(n->type)&&endpoint.portId.starts_with("tunnel:");
        };
        for(const auto &edge:authored.diagramEdges) {
            const auto from=resolver.resolveSource(edge),to=resolver.resolveInRegion(edge.to,edge.diagramOwnerStructureId,edge.diagramOwnerRegionId);
            const bool object=from.resolution.type.kind==TypeKind::Object||to.resolution.type.kind==TypeKind::Object;
            if(!object && !clusterContainsClassValue(from.resolution.type) && !clusterContainsClassValue(to.resolution.type))continue;
            if(!object && (!from.complete || !from.resolution.accepted()))
                refuse("class.object_connection","A Cluster's nominal fields require a complete producer.",wirePath(edge.id));
            if(!permissible(edge.from,from.resolution.type)||!permissible(edge.to,to.resolution.type))refuse("class.object_operation_required",
                "An object's private transport cannot be inspected or forged by a Cluster or reference operation.",wirePath(edge.id));
            // File-call type expressions omit Enum labels and values. The composed
            // call validator checks the producer against the frozen callee widget.
            const auto target=std::find_if(authored.diagramNodes.begin(),authored.diagramNodes.end(),
                [&](const auto &node){return node.id==edge.to.nodeId;});
            if(!object && target!=authored.diagramNodes.end() && target->kind=="frog_call" &&
               !target->classOperation && !target->callDocumentPath.empty()) {
                if(!to.resolution.accepted() || from.resolution.type.canonical()!=to.resolution.type.canonical())
                    refuse("subfrog.interface.type_mismatch","The file-call connector and its producer have different nominal types.",wirePath(edge.id));
                continue;
            }
            const auto connection=resolver.connection(edge);
            if(!connection.accepted())refuse(connection.code.empty()?"class.object_connection":connection.code,connection.message,wirePath(edge.id));
        }
        std::set<std::string> ids;
        for(const auto &n:authored.diagramNodes)ids.insert(n.id);
        for(const auto &e:authored.diagramEdges)ids.insert(e.id);
        for(const auto &o:authored.widgets.objects())ids.insert(o.id);
        const auto fresh=[&](std::string_view origin,std::string_view role,std::string_view identity) {
            const auto id="__class_"+hashing::sha256Hex(std::string(origin)+":"+std::string(role)+":"+std::string(identity));
            if(!ids.insert(id).second)refuse("class.generated_identity","A compiler-generated identity collided with source state.",path(origin));
            result.generatedFrom.emplace(id,origin);return id;
        };
        const auto lowerValue=[&](auto &self,TypeRef &type) -> void {
            if(type.kind==TypeKind::Object) {
                const auto layout=hierarchy.recordLayout(type.name);
                if(!layout)refuse("class.layout_unavailable","The nominal object has no admitted closed value layout.","/diagram",type.name);
                type=layout->schema;
            } else if(type.kind==TypeKind::Cluster) {
                for(auto &field:type.children)self(self,field);
            }
        };
        const auto physical=[&](std::string &expression) {
            auto type=binding::parseType(expression);
            if(type.valid()&&(type.type.kind==TypeKind::Object||clusterContainsClassValue(type.type))) {
                lowerValue(lowerValue,type.type);
                expression=type.type.canonical();
            }
        };
        // Resolve inferred Select/tunnel types before replacing nominal rails.
        for(auto &node:result.document.diagramNodes) {
            if(node.type=="frog.studio.structures.select") {
                const auto type=resolver.resolve({node.id,"result"});
                if(type.resolution.type.kind==TypeKind::Object)node.valueType=type.resolution.type.canonical();
            }
            physical(node.valueType);for(auto &p:node.callPorts)physical(p.valueType);
            for(auto &t:node.caseStructureTunnels)physical(t.valueType);
        }
        const auto lowerWidget=[&](auto &self,WidgetInstance &widget) -> void {
            const auto nominal=binding::widgetType(widget);
            if(nominal.valid()&&nominal.type.kind==TypeKind::Object) {
                const auto &old=widget;const auto layout=hierarchy.recordLayout(nominal.type.name);
                ClassIssue issue;auto value=layout?makeClassPortCarrier(layout->schema,old.instanceId,old.role=="control",&issue):std::nullopt;
                if(!value)refuse("class.carrier_unavailable",issue.message,"/frontPanel/"+widget.instanceId,nominal.type.name);
                value->bounds=old.bounds;value->label=old.label;value->publicInputId=old.publicInputId;value->publicOutputId=old.publicOutputId;
                value->interfaceMapSlotId=old.interfaceMapSlotId;value->interfaceConnectionRequirement=old.interfaceConnectionRequirement;
                value->defaultData=serializeArrayElementWidget(*value);widget=std::move(*value);
            } else if(nominal.valid() && clusterContainsClassValue(nominal.type)) {
                auto *container=clusterContainerState(widget);
                if(!container)refuse("class.carrier_unavailable","Missing Cluster field model.","/frontPanel/"+widget.instanceId);
                for(auto &field:container->elements)if(field.widget)self(self,*field.widget);
                widget.valueType=binding::widgetType(widget).type.canonical();
                widget.defaultData=serializeArrayElementWidget(widget);
            }
        };
        for(auto &o:result.document.widgets.mutableObjects())if(o.widget) {
            const auto nominal=binding::widgetType(*o.widget);
            if(!nominal.valid() || (nominal.type.kind!=TypeKind::Object && !clusterContainsClassValue(nominal.type)))continue;
            lowerWidget(lowerWidget,*o.widget);
            for(auto &node:result.document.diagramNodes)if(node.kind=="widget_value"&&node.widgetId==o.id) {
                node.type=o.widget->valueType;node.valueType=o.widget->valueType;
            }
        }
        if(result.document.declaredInterface) {
            const auto declaration=inspectFrogDraftInterface(*result.document.declaredInterface);
            for(const auto &p:declaration.ports)if(p.parsedType.type.kind==TypeKind::Object || clusterContainsClassValue(p.parsedType.type)) {
                auto type=p.typeExpression;physical(type);std::string why;
                if(!result.document.declaredInterface->editPortForAuthoring(p.id,p.input,p.typeExpression,p.input,type,std::nullopt,&why))
                    refuse("class.connector_projection",why,p.sourcePath);
            }
        }
        std::vector<DiagramNode> extra;std::vector<DiagramEdge> wires;
        const auto remap=[&](const std::string &node,std::string_view old,std::string_view next,bool input) {
            for(auto &edge:result.document.diagramEdges) {
                auto &endpoint=input?edge.to:edge.from;
                if(endpoint.nodeId==node&&endpoint.portId==old)endpoint.portId=next;
            }
        };
        const auto literal=[&](const auto &self,DiagramNode node,const TypeRef &type,const Json &value,const std::string &origin)->void {
            node.kind="primitive";node.classOperation.reset();node.callPorts.clear();node.callDocumentPath.clear();node.valueType=type.canonical();
            if(type.kind==TypeKind::Cluster) {
                node.type="frog.studio.cluster.constant";node.clusterConstantCollapsed=true;
                const auto parent=node.id;extra.push_back(std::move(node));
                for(std::size_t i=0;i<type.children.size();++i) {
                    const auto *v=value.member(type.fieldIds[i]);if(!v)refuse("class.default_invalid","A private default field is absent.",path(origin));
                    DiagramNode child;child.id=fresh(origin,"default_field",parent+":"+type.fieldIds[i]);
                    child.constantOwnerNodeId=parent;child.constantFieldId=type.fieldIds[i];child.constantFieldOrder=static_cast<std::uint32_t>(i);
                    child.label=type.fieldIds[i];
                    self(self,std::move(child),type.children[i],*v,origin);
                }
            } else {
                if(type.name=="bool"){node.type=value.boolean?"frog.studio.boolean.true_constant":"frog.studio.boolean.false_constant";node.text=value.boolean?"true":"false";}
                else if(type.name=="string"){node.type="frog.studio.string.constant";node.text=value.string;}
                else if(type.name=="path"){node.type="frog.studio.path.constant";node.text=value.string;}
                else{node.type="frog.studio.numeric.numeric_constant";node.text=value.string;}
                extra.push_back(std::move(node));
            }
        };
        std::set<std::string> replaced;
        for(auto &node:result.document.diagramNodes)if(node.classOperation) {
            const auto op=*node.classOperation;const auto origin=node.id;
            node.classOperation.reset();
            if(op.kind==ClassOperationKind::Default) {
                const auto layout=hierarchy.recordLayout(op.owner.id);const auto value=layout?hierarchy.defaultRecord(op.owner.id,*layout):std::nullopt;
                if(!layout||!value)refuse("class.default_invalid","The concrete object's declared defaults cannot be realized.",path(origin));
                literal(literal,node,layout->schema,*value,origin);replaced.insert(origin);continue;
            }
            if(op.kind==ClassOperationKind::ReadField||op.kind==ClassOperationKind::WriteField) {
                const bool update=op.kind==ClassOperationKind::WriteField;
                const auto descriptor=hierarchy.lowerPrivateField(*caller,op.owner.id,op.memberId,update,path(origin));
                if(!descriptor)refuse("class.private_access","Private field lowering was refused.",path(origin));
                node.kind="primitive";node.type=update?"frog.studio.cluster.bundle_by_name":"frog.studio.cluster.unbundle_by_name";
                node.callPorts.clear();node.clusterFunctionTerminals=1;node.clusterFieldIdPaths=descriptor->fieldPaths;
                node.clusterFieldNames={op.memberId};remap(origin,"object","cluster",true);
                remap(origin,"value","element1",update);if(update)remap(origin,"updated","result",false);continue;
            }
            const ClassCallTarget declaration{op.owner.id,op.memberId};const auto *declared=hierarchy.findMethod(declaration);
            const auto configureCall=[&](DiagramNode &call,const ClassCallTarget &target) {
                const auto *actual=program.findMember(target);const auto *method=hierarchy.findMethod(target);
                if(!actual||!method)refuse("class.member_unavailable","The selected implementation has no actual captured member.",path(origin));
                call.kind="frog_call";call.type="frog.studio.frog_call";call.callDocumentPath=actual->source.canonicalUri;
                call.callDocumentTitle=method->name;call.callPorts.clear();call.polymorphicFrogSelection.reset();call.classOperation.reset();
                result.memberCalls.emplace(call.id,target);
                for(const auto &p:method->ports) {
                    auto type=p.kind==ClassPortKind::Object?classType(target.classId):p.dataType.canonical();physical(type);
                    auto boundary=port(p.publicPortId,std::move(type),p.input);
                    for(const auto &object:actual->document->widgets.objects())if(object.widget) {
                        const auto &widget=*object.widget;const auto &id=p.input?widget.publicInputId:widget.publicOutputId;
                        if(id&&*id==p.publicPortId&&widget.interfaceMapSlotId)boundary.interfaceMapSlotId=*widget.interfaceMapSlotId;
                    }
                    call.callPorts.push_back(std::move(boundary));
                }
            };
            if(op.kind==ClassOperationKind::StaticCall){configureCall(node,declaration);continue;}
            const auto plan=hierarchy.planDynamicCall(op.owner.id,declaration,caller,path(origin));
            if(!plan)refuse("class.dynamic_dispatch","The original dynamic slot has no valid concrete implementations.",path(origin));
            const auto receiver=std::find_if(declared->ports.begin(),declared->ports.end(),[](const auto &p){return p.kind==ClassPortKind::Object&&p.input;});
            DiagramNode selector;selector.id=fresh(origin,"discriminant",op.owner.id);selector.kind="primitive";
            selector.type="frog.studio.cluster.unbundle_by_name";selector.clusterFunctionTerminals=1;
            selector.clusterFieldIdPaths=plan->selector.fieldPaths;selector.clusterFieldNames={"Concrete type"};
            selector.diagramOwnerStructureId=node.diagramOwnerStructureId;selector.diagramOwnerRegionId=node.diagramOwnerRegionId;
            const auto driver=std::find_if(result.document.diagramEdges.begin(),result.document.diagramEdges.end(),[&](const auto &e){return e.to.nodeId==origin&&e.to.portId==receiver->publicPortId;});
            if(driver==result.document.diagramEdges.end())refuse("class.receiver_required","Dynamic dispatch requires a real object producer.",path(origin));
            auto edge=*driver;edge.id=fresh(origin,"receiver",receiver->publicPortId);edge.to={selector.id,"cluster"};edge.bends.clear();wires.push_back(std::move(edge));
            edge={};edge.id=fresh(origin,"selector",op.owner.id);edge.from={selector.id,"element1"};edge.to={origin,"selector"};
            edge.diagramOwnerStructureId=node.diagramOwnerStructureId;edge.diagramOwnerRegionId=node.diagramOwnerRegionId;wires.push_back(std::move(edge));
            extra.push_back(std::move(selector));
            node.kind="primitive";node.type="frog.studio.structures.case_structure";node.callDocumentPath.clear();node.callPorts.clear();
            node.caseStructureSelectorValueType="u32";node.caseStructureRegions.clear();node.caseStructureTunnels.clear();
            for(const auto &p:declared->ports) {
                DiagramCaseStructureTunnel tunnel;tunnel.id=p.publicPortId;tunnel.valueType=p.kind==ClassPortKind::Object?classType(op.owner.id):p.dataType.canonical();physical(tunnel.valueType);
                tunnel.outputFromStructure=!p.input;tunnel.side=p.input?"left":"right";node.caseStructureTunnels.push_back(tunnel);
                remap(origin,p.publicPortId,p.input?diagram_identity::tunnelInputId(tunnel.id):diagram_identity::tunnelOutputId(tunnel.id),p.input);
            }
            for(std::size_t armIndex=0;armIndex<plan->arms.size();++armIndex) {
                const auto &arm=plan->arms[armIndex];
                const auto region="concrete_"+std::to_string(arm.discriminant);
                DiagramCaseStructureRegion r;r.id=region;r.title=arm.concreteClassId;
                // Object construction and field writes seal the finite tag
                // domain. The existing integer Case requires a default; its
                // remaining valid tag is this real member, not a witness or
                // an invented result. No authored U32/Record can enter here.
                r.isDefault=armIndex==0;
                if(!r.isDefault)r.matches.push_back({"integer_exact",std::to_string(arm.discriminant),{}, {}});
                node.caseStructureRegions.push_back(std::move(r));
                DiagramNode call;call.id=fresh(origin,"implementation",arm.concreteClassId);configureCall(call,arm.implementation);
                call.diagramOwnerStructureId=origin;call.diagramOwnerRegionId=region;
                const auto *implementation=hierarchy.findMethod(arm.implementation);
                for(const auto &p:declared->ports) {
                    const auto corresponding=std::find_if(implementation->ports.begin(),implementation->ports.end(),[&](const auto &q){return q.slot==p.slot;});
                    if(corresponding==implementation->ports.end())refuse("class.override_signature","An actual override lost its stable connector slot.",path(origin));
                    DiagramEdge wire;wire.id=fresh(origin,"argument",arm.concreteClassId+":"+p.slot);
                    if(p.input){wire.from={origin,diagram_identity::tunnelOutputId(p.publicPortId)};wire.to={call.id,corresponding->publicPortId};}
                    else{wire.from={call.id,corresponding->publicPortId};wire.to={origin,diagram_identity::tunnelInputId(p.publicPortId)};}
                    wire.diagramOwnerStructureId=origin;wire.diagramOwnerRegionId=region;wires.push_back(std::move(wire));
                }
                extra.push_back(std::move(call));
            }
        }
        std::erase_if(result.document.diagramNodes,[&](const auto &node){return replaced.contains(node.id);});
        result.document.diagramNodes.insert(result.document.diagramNodes.end(),std::make_move_iterator(extra.begin()),std::make_move_iterator(extra.end()));
        result.document.diagramEdges.insert(result.document.diagramEdges.end(),std::make_move_iterator(wires.begin()),std::make_move_iterator(wires.end()));
        result.document.properties.classSource={};result.changed=true;
    } catch(const Invalid &failure){result.issues.push_back(failure.issue);result.document=authored;result.generatedFrom.clear();result.memberCalls.clear();result.changed=false;}
    return result;
}
}
