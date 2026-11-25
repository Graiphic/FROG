#include "FrogDraftJson.hpp"
#include "CanonicalJsonSupport.hpp"
#include "frog_native_engine/DraftSourcePreparation.hpp"
#include "frog_native_engine/PolymorphicFrog.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include "frog_native_engine/ClusterFunctionBinding.hpp"
#include "frog_native_engine/ContainedWidgetSchemaMetadata.hpp"
#include "frog_native_engine/StructureBindingDefinition.hpp"
#include "frog_native_engine/WidgetDataOperations.hpp"
#include "frog_native_engine/FrogSourceDocument.hpp"
#include "frog_native_engine/DraftInterfaceInspection.hpp"
#include "frog_native_engine/TabControl.hpp"
#include "frog_native_engine/WidgetFamilyEngine.hpp"
#include <algorithm>
#include <map>
#include <set>

namespace frog::native_engine {
namespace {
using Json=source::JsonValue;
constexpr std::size_t maximumDefinitions = 256;
constexpr std::size_t maximumDefinitionDepth = 32;
constexpr std::size_t maximumDefinitionBytes = kMaximumFrogDocumentBytes;
constexpr std::size_t maximumClosureBytes = 64U * 1024U * 1024U;

template<class Widget,class Visitor> void visitWidget(Widget &widget,Visitor &&visitor) {
    visitor(widget);
    if(auto *cluster=clusterContainerState(widget))
        for(auto &field:cluster->elements)if(field.widget)visitWidget(*field.widget,visitor);
    if(auto *array=arrayContainerState(widget);array && array->elementTemplate)
        visitWidget(*array->elementTemplate,visitor);
}
void appendDiagnostic(DraftTypeDefinitionResolution &result,std::string code,std::string message,
    std::string instance={},std::string field={}) {
    if(result.diagnostics.size()>=kMaximumFrogSourceDiagnostics)return;
    result.diagnostics.push_back({std::move(code),std::move(message),std::move(instance),std::move(field),{},{},{}});
}
void stripExecutionLinks(WidgetInstance &widget,unsigned depth=0) {
    if(depth>32)return;
    widget.typeDefinition.reset();
    if(auto *cluster=clusterContainerState(widget))for(auto &field:cluster->elements)
        if(field.widget)stripExecutionLinks(*field.widget,depth+1);
    if(auto *array=arrayContainerState(widget);array&&array->elementTemplate)stripExecutionLinks(*array->elementTemplate,depth+1);
    if(widget.defaultData)if(auto stored=data_operations::decodeStoredDefault(widget,*widget.defaultData)) {
        stripExecutionLinks(*stored,depth+1);widget.defaultData=data_operations::encode(*stored);
    }
}
struct ResolvedDefinition {
    TypeDefinitionDocument definition;
    DraftDependencyResult source;
    std::string key;
    bool accepted=false;
};

FrogDraftData previousSchemaDocument(const FrogDraftData &document) {
    auto previous=document;
    std::vector<std::string> ids;
    for(const auto &object:previous.widgets.objects())if(object.widget)ids.push_back(object.id);
    for(const auto &id:ids)if(auto *object=previous.widgets.objectById(id);object && object->widget)
        visitWidget(*object->widget,[](WidgetInstance &widget) {
            if(!widget.typeDefinition)return;
            if(auto baseline=deserializeTypeDefinitionWidget(widget.typeDefinition->baselineWidgetJson)) {
                baseline->instanceId=widget.instanceId;baseline->role=widget.role;
                baseline->publicInputId=widget.publicInputId;baseline->publicOutputId=widget.publicOutputId;
                widget=std::move(*baseline);
            }
        });
    std::vector<std::pair<std::string,WidgetInstance>> constants;
    for(const auto &node:previous.diagramNodes)if(node.typeDefinition)
        if(auto baseline=deserializeTypeDefinitionWidget(node.typeDefinition->baselineWidgetJson))constants.emplace_back(node.id,std::move(*baseline));
    for(const auto &[id,baseline]:constants)static_cast<void>(installTypeDefinitionConstantWidget(previous.diagramNodes,id,baseline));
    return previous;
}
std::set<std::string> upstreamLinkedInstances(const FrogDraftData &document,std::string nodeId) {
    std::set<std::string> result,visited;std::vector<std::string> pending{std::move(nodeId)};
    while(!pending.empty() && visited.size()<4096) {
        auto id=std::move(pending.back());pending.pop_back();if(!visited.insert(id).second)continue;
        const auto node=std::find_if(document.diagramNodes.begin(),document.diagramNodes.end(),[&](const auto &entry){return entry.id==id;});
        if(node==document.diagramNodes.end())continue;
        if(node->typeDefinition)result.insert(node->id);
        if(node->kind=="widget_value")if(const auto *widget=document.widgets.widgetById(node->widgetId))
            visitWidget(*widget,[&](const WidgetInstance &part){if(part.typeDefinition)result.insert(part.instanceId);});
        for(const auto &edge:document.diagramEdges)if(edge.to.nodeId==id)pending.push_back(edge.from.nodeId);
    }
    return result;
}
void retainPositionalReviewBaseline(FrogDraftData &document,const FrogDraftData &before,
    const std::set<std::string> &instances,const std::string &reason) {
    std::map<std::string,TypeDefinitionLink,std::less<>> links;
    for(const auto &object:before.widgets.objects())if(object.widget)
        visitWidget(*object.widget,[&](const WidgetInstance &part){if(part.typeDefinition)links.emplace(part.instanceId,*part.typeDefinition);});
    for(const auto &node:before.diagramNodes)if(node.typeDefinition)links.emplace(node.id,*node.typeDefinition);
    const auto restore=[&](TypeDefinitionLink &link,const std::string &id) {
        if(!instances.contains(id))return;
        if(const auto found=links.find(id);found!=links.end()) {
            link.baselineWidgetJson=found->second.baselineWidgetJson;
            link.resolvedRevision=found->second.resolvedRevision;
            link.resolvedSchemaFingerprint=found->second.resolvedSchemaFingerprint;
            link.unresolvedReason=reason;
        }
    };
    std::vector<std::string> ids;for(const auto &object:document.widgets.objects())if(object.widget)ids.push_back(object.id);
    for(const auto &id:ids)if(auto *object=document.widgets.objectById(id);object&&object->widget)
        visitWidget(*object->widget,[&](WidgetInstance &part){if(part.typeDefinition)restore(*part.typeDefinition,part.instanceId);});
    for(auto &node:document.diagramNodes)if(node.typeDefinition)restore(*node.typeDefinition,node.id);
}
void reconcileDependentNodes(FrogDraftData &document,const FrogDraftData &before,
    DraftTypeDefinitionResolution &result,bool accept,std::string_view acceptedInstance) {
    const auto previous=previousSchemaDocument(before);
    binding::TypeResolver oldTypes(previous.diagramNodes,previous.diagramEdges,previous.widgets);
    binding::TypeResolver newTypes(document.diagramNodes,document.diagramEdges,document.widgets);
    std::set<std::string> positionalReview;
    std::string positionalReason;
    for(auto &node:document.diagramNodes) {
        if(isClusterRecordFunction(node.type)) {
            const auto wire=std::find_if(document.diagramEdges.begin(),document.diagramEdges.end(),[&](const auto &edge){return edge.to.nodeId==node.id&&edge.to.portId=="cluster";});
            if(wire==document.diagramEdges.end())continue;
            const auto oldSchema=oldTypes.resolveSource(*wire).resolution.type;
            const auto schema=newTypes.resolveSource(*wire).resolution.type;
            if(schema.kind!=binding::TypeKind::Cluster)continue;
            if(isClusterByName(node.type)) {
                const auto prior=std::find_if(before.diagramNodes.begin(),before.diagramNodes.end(),[&](const auto &entry){return entry.id==node.id;});
                if(prior==before.diagramNodes.end())continue;
                const auto rows=clusterFunctionCount(node);
                node.clusterFieldNames.resize((std::max)(node.clusterFieldNames.size(),static_cast<std::size_t>(rows)));
                node.clusterFieldIdPaths.resize((std::max)(node.clusterFieldIdPaths.size(),static_cast<std::size_t>(rows)));
                for(unsigned row=0;row<rows;++row) {
                    auto path=row<prior->clusterFieldIdPaths.size()?prior->clusterFieldIdPaths[row]:std::vector<std::string>{};
                    if(path.empty()&&row<prior->clusterFieldNames.size())path=clusterNamedFieldIdPath(oldSchema,prior->clusterFieldNames[row]);
                    if(path.empty())continue;
                    std::string name;
                    if(!clusterFieldByIdPath(schema,path,&name)) {
                        appendDiagnostic(result,"typedef.by_name.field_removed","The selected Cluster field no longer exists; the existing connection was retained for repair.",node.id,
                            row<prior->clusterFieldNames.size()?prior->clusterFieldNames[row]:path.front());
                        node.clusterFieldIdPaths[row]=std::move(path);
                    } else {
                        if(node.clusterFieldNames[row]!=name || node.clusterFieldIdPaths[row]!=path)result.changed=true;
                        node.clusterFieldNames[row]=std::move(name);node.clusterFieldIdPaths[row]=std::move(path);
                    }
                }
            } else if(oldSchema.kind==binding::TypeKind::Cluster) {
                const auto roots=upstreamLinkedInstances(document,wire->from.nodeId);
                const bool approved=accept && (acceptedInstance.empty() || roots.contains(std::string(acceptedInstance)));
                for(unsigned row=0;row<clusterFunctionCount(node) && row<oldSchema.fieldIds.size();++row) {
                    const bool wired=std::any_of(document.diagramEdges.begin(),document.diagramEdges.end(),[&](const auto &edge){return
                        (edge.from.nodeId==node.id && edge.from.portId==clusterFunctionRowId(row)) ||
                        (edge.to.nodeId==node.id && edge.to.portId==clusterFunctionRowId(row));});
                    if(!wired || (row<schema.fieldIds.size() && oldSchema.fieldIds[row]==schema.fieldIds[row]) || approved)continue;
                    positionalReason="A positional Cluster operation now designates another field. Review this instance before execution; its existing positional semantics and wires were preserved.";
                    appendDiagnostic(result,"typedef.positional_field_changed",positionalReason,node.id,clusterFunctionRowId(row));
                    positionalReview.insert(roots.begin(),roots.end());
                }
            }
        }
        if(binding::isCase(node)) {
            const auto wire=std::find_if(document.diagramEdges.begin(),document.diagramEdges.end(),[&](const auto &edge){return edge.to.nodeId==node.id&&edge.to.portId==binding::caseBindingDefinition().selectorOuterPort;});
            if(wire==document.diagramEdges.end())continue;
            const auto source=newTypes.resolveSource(*wire).resolution;
            const auto &schema=source.type;
            if(!source.accepted() || schema.kind!=binding::TypeKind::Enum || schema.choiceKind!=binding::ChoiceKind::Enum || schema.choices.empty())continue;
            // This is the exact resolved source schema, including its nominal
            // domain, representation and ordered stable item identities. The
            // generic constant-item helper reconstructs only nominal text and
            // cannot equal a complete Enum TypeRef carrying these choices.
            std::vector<RingItem> items;
            for(const auto &choice:schema.choices)items.push_back({choice.label,choice.value,choice.disabled,choice.id,choice.visible});
            for(const auto &region:node.caseStructureRegions)for(const auto &matcher:region.matches) {
                const auto exists=[&](std::string_view id){return std::any_of(items.begin(),items.end(),[&](const auto &item){return item.id==id;});};
                if((matcher.kind=="enum_item"&&!exists(matcher.value)) || (matcher.kind=="enum_range"&&
                    (!exists(matcher.lower.value_or(""))||!exists(matcher.upper.value_or("")))))
                    appendDiagnostic(result,"typedef.enum_case.item_removed","A Case still refers to a removed Enum identity. Its subdiagram and matcher were preserved for repair.",node.id,region.id);
            }
            bool same=node.caseStructureSelectorEnumItems.size()==items.size();
            if(same)for(std::size_t i=0;i<items.size();++i) {
                const auto &a=node.caseStructureSelectorEnumItems[i],&b=items[i];
                if(a.id!=b.id||a.label!=b.label||a.value!=b.value){same=false;break;}
            }
            if(!same){node.caseStructureSelectorEnumItems=std::move(items);result.changed=true;}
            if(node.caseStructureSelectorValueType!=schema.canonical()){node.caseStructureSelectorValueType=schema.canonical();result.changed=true;}
        }
    }
    if(!positionalReview.empty())retainPositionalReviewBaseline(document,before,positionalReview,positionalReason);
}
class Resolver {
public:
    DraftTypeDefinitionResolution result;
    bool acceptProposed=false;
    std::string acceptedInstanceId;
    std::size_t bytes=0;
    unsigned resolvingDefinitionDepth=0;
    std::map<std::string,ResolvedDefinition,std::less<>> definitions;
    std::map<std::string,std::string,std::less<>> identities;
    std::map<std::string,std::size_t,std::less<>> dependencyRecords;
    std::vector<std::string> active;
    DraftDependencyResolver loader;
    std::map<std::string,std::shared_ptr<const std::string>,std::less<>> sourceBytes;
    std::map<std::string,std::map<std::string,std::string,std::less<>>,std::less<>> sourceLocations;
    std::string pointer(std::string_view owner,std::string_view id,std::string_view suffix) {
        if(!sourceLocations.contains(owner)) {
            auto &locations=sourceLocations[std::string(owner)];
            const auto found=sourceBytes.find(owner);
            auto tree=found==sourceBytes.end()?std::optional<Json>{}:readFrogDraftJson(*found->second);
            if(tree) {
                std::vector<std::pair<const Json*,std::string>> work{{&*tree,{}}};
                while(!work.empty()) {
                    auto [value,path]=std::move(work.back());work.pop_back();
                    if(value->type==Json::Type::Object) {
                        for(const auto key:{"id","instance_id"})if(const auto *identity=value->member(key);identity&&identity->type==Json::Type::String) {
                            const auto [entry,inserted]=locations.emplace(identity->string,path);
                            if(!inserted&&entry->second!=path)entry->second.clear();
                        }
                        for(const auto &[key,child]:value->object)work.emplace_back(&child,canonical_json::pointer(path,key));
                    } else if(value->type==Json::Type::Array)for(std::size_t i=0;i<value->array.size();++i)
                        work.emplace_back(&value->array[i],canonical_json::pointer(path,std::to_string(i)));
                }
            }
        }
        const auto &locations=sourceLocations.at(std::string(owner));const auto found=locations.find(id);
        return found==locations.end()||found->second.empty()?std::string{}:found->second+std::string(suffix);
    }

    ResolvedDefinition *definition(const TypeDefinitionLink &link,std::string_view owner,
        std::string_view instance) {
        DraftDependencyRequest request{DraftDependencyKind::TypeDefinition,std::string(owner),link.sourcePath,
            pointer(owner,instance,"/type_definition/source_path"),link.definitionId,{}, {}};
        auto source=resolveDraftDependency(request,loader);
        const auto key=source.canonicalUri.empty()?std::string(owner)+"::"+link.sourcePath:source.canonicalUri;
        if(!dependencyRecords.contains(key)) {
            if(dependencyRecords.size()>=maximumDefinitions) {
                appendDiagnostic(result,"typedef.dependency.limit","Type Def dependencies exceed the bounded document count.",std::string(instance),link.sourcePath);return nullptr;
            }
            dependencyRecords.emplace(key,result.dependencies.size());
            result.dependencies.push_back({source,link.definitionId,link.resolvedRevision,link.resolvedSchemaFingerprint});
        }
        if(std::find(active.begin(),active.end(),key)!=active.end()) {
            std::string chain;
            for(const auto &entry:active){if(!chain.empty())chain+=" -> ";chain+=definitions.at(entry).source.canonicalUri;}
            chain+=" -> "+source.canonicalUri;
            appendDiagnostic(result,"typedef.dependency.cycle","Type Def composition cycle: "+chain,std::string(instance),link.sourcePath);return nullptr;
        }
        if(source.state!=DraftDependencyState::Present||!source.bytes) {
            const auto code=source.state==DraftDependencyState::Cancelled?"typedef.control.cancelled":
                source.state==DraftDependencyState::LimitExceeded?"typedef.dependency.limit":
                source.diagnosticCode=="dependency.identity_mismatch"?"typedef.identity.mismatch":
                source.diagnosticCode=="dependency.version_mismatch"?"typedef.version.mismatch":
                source.state==DraftDependencyState::Unsupported?"typedef.source.invalid":"typedef.source.missing";
            appendDiagnostic(result,code,source.message.empty()?"The Type Def source is unavailable.":source.message,std::string(instance),link.sourcePath);return nullptr;
        }
        auto found=definitions.find(key);
        if(found!=definitions.end()) {
            if(found->second.source.sha256!=source.sha256) {
                appendDiagnostic(result,"typedef.source.changed","A canonical Type Definition resolved to different immutable bytes.",std::string(instance),link.sourcePath);return nullptr;
            }
            if(found->second.definition.id!=link.definitionId) {
                appendDiagnostic(result,"typedef.identity.mismatch","The located Type Def does not have the expected identity: "+source.canonicalUri,std::string(instance),link.sourcePath);return nullptr;
            }
            return found->second.accepted?&found->second:nullptr;
        }
        if(definitions.size()>=maximumDefinitions || active.size()>=maximumDefinitionDepth) {
            appendDiagnostic(result,"typedef.dependency.limit","Type Def dependencies exceed the bounded depth or document count.",std::string(instance),link.sourcePath);return nullptr;
        }
        if(source.bytes->size()>maximumDefinitionBytes||bytes>maximumClosureBytes||source.bytes->size()>maximumClosureBytes-bytes) {
            appendDiagnostic(result,"typedef.dependency.limit","The Type Def dependency closure exceeds its bounded source budget.",std::string(instance),link.sourcePath);return nullptr;
        }
        bytes+=source.bytes->size();std::string error;
        auto parsed=deserializeTypeDefinition(*source.bytes,&error);
        sourceBytes.insert_or_assign(source.canonicalUri,source.bytes);
        if(!parsed){appendDiagnostic(result,"typedef.source.invalid",source.canonicalUri+": "+error,std::string(instance),link.sourcePath);return nullptr;}
        if(parsed->id!=link.definitionId || (!source.identity.empty()&&source.identity!=parsed->id) ||
           (!source.version.empty()&&source.version!=std::to_string(parsed->revision))) {
            appendDiagnostic(result,"typedef.identity.mismatch","Expected Type Def '"+link.definitionId+"' with its attested version, but found '"+parsed->id+"' at "+source.canonicalUri,std::string(instance),link.sourcePath);return nullptr;
        }
        if(const auto claimed=identities.find(parsed->id);claimed!=identities.end() && claimed->second!=key) {
            appendDiagnostic(result,"typedef.identity.ambiguous","Distinct sources claim the same Type Def identity: "+definitions.at(claimed->second).source.canonicalUri+" and "+source.canonicalUri,std::string(instance),link.sourcePath);return nullptr;
        }
        identities.emplace(parsed->id,key);
        const auto authoredSchemaFingerprint=typeDefinitionSchemaFingerprint(*parsed);
        auto [entry,inserted]=definitions.emplace(key,ResolvedDefinition{std::move(*parsed),source,key,false});
        auto &resolved=entry->second;
        active.push_back(key);
        const auto issues=result.diagnostics.size();
        ++resolvingDefinitionDepth;
        resolveWidget(resolved.definition.root,resolved.source.canonicalUri,0);
        if(!resolved.definition.tabChildren.empty()) {
            auto &model=resolved.definition;
            FrogWidgetData panel;
            panel.addObject({model.root.instanceId,model.root.bounds,model.root},true);
            for(const auto &member:model.tabChildren)panel.addObject(member,true);
            for(const auto &member:model.tabChildren)if(member.widget) {
                auto *object=panel.objectById(member.id);
                const TypeDefinitionDocument *nested=nullptr;
                resolveWidget(*object->widget,resolved.source.canonicalUri,0,false,&nested);
                if(nested&&isTabWidget(nested->root)) {
                    auto children=reconcileTypeDefinitionTabChildren(panel,member.id,*nested,
                        acceptProposed&&(acceptedInstanceId.empty()||acceptedInstanceId==member.id));
                    for(auto &issue:children.diagnostics)
                        if(result.diagnostics.size()<kMaximumFrogSourceDiagnostics)result.diagnostics.push_back(std::move(issue));
                }
            }
            model.tabChildren=typeDefinitionTabChildren(panel,model.root.instanceId);
        }
        --resolvingDefinitionDepth;
        active.pop_back();
        if(result.diagnostics.size()!=issues)return nullptr;
        resolved.accepted=true;
        result.dependencies[dependencyRecords.at(key)]={resolved.source,resolved.definition.id,resolved.definition.revision,
            typeDefinitionSchemaFingerprint(resolved.definition),authoredSchemaFingerprint};
        return &resolved;
    }

    struct EnclosingField { WidgetInstance *cluster; std::string_view fieldId; };
    using ClusterPath = std::vector<EnclosingField>;

    // A Make Current Value Default on an ordinary enclosing Cluster takes a
    // separate snapshot. Migrate this field in each such snapshot with the
    // same stable IDs and approval as the actual linked occurrence.
    bool migrateEnclosingDefaults(const ClusterPath &path,const WidgetInstance &previous,
        const WidgetInstance &current,const std::string &instance,bool approve) {
        std::vector<std::pair<WidgetInstance *,std::string>> pending;
        ContainedWidgetPath target;
        for(auto parent=path.rbegin();parent!=path.rend();++parent) {
            target.insert(target.begin(),ContainedWidgetStep{std::string(parent->fieldId),{}});
            auto &owner=*parent->cluster;
            if(!owner.defaultData)continue;
            auto saved=data_operations::decodeStoredDefault(owner,*owner.defaultData);
            if(!saved) {
                appendDiagnostic(result,"typedef.default_invalid","The enclosing Cluster's saved default cannot be decoded.",instance,owner.instanceId);
                return false;
            }
            auto *field=containedWidget(*saved,target);
            // A field introduced after the snapshot has no saved value here.
            if(!field)continue;
            auto migration=migrateTypeDefinitionWidget(*field,previous,current,approve);
            if(!migration.accepted()) {
                for(auto &issue:migration.diagnostics) {
                    issue.instanceId=instance;
                    issue.fieldPath=owner.instanceId+"/<default>/"+field->instanceId+issue.fieldPath;
                    if(result.diagnostics.size()<kMaximumFrogSourceDiagnostics)result.diagnostics.push_back(std::move(issue));
                }
                return false;
            }
            *field=std::move(migration.proposedWidget);
            auto encoded=refreshContainedWidgetSchemaMetadata(*saved)?data_operations::encode(*saved):std::string{};
            if(encoded.empty()) {
                appendDiagnostic(result,"typedef.default_invalid","The migrated enclosing Cluster default cannot be encoded.",instance,owner.instanceId);
                return false;
            }
            pending.emplace_back(&owner,std::move(encoded));
        }
        for(auto &[owner,encoded]:pending)owner->defaultData=std::move(encoded);
        return true;
    }

    void resolveWidget(WidgetInstance &widget,std::string_view owner,std::size_t depth,bool approvedRoot=false,
        const TypeDefinitionDocument **resolvedRoot=nullptr) {
        ClusterPath path;
        resolveWidget(widget,owner,depth,approvedRoot,resolvedRoot,path);
    }
    void resolveWidget(WidgetInstance &widget,std::string_view owner,std::size_t depth,bool approvedRoot,
        const TypeDefinitionDocument **resolvedRoot,ClusterPath &path) {
        if(depth>maximumDefinitionDepth) {
            appendDiagnostic(result,"typedef.widget.limit","Type Def widget composition exceeds the bounded depth.",widget.instanceId);return;
        }
        approvedRoot=approvedRoot || widget.instanceId==acceptedInstanceId;
        if(widget.typeDefinition) {
            const auto previousLink=*widget.typeDefinition;
            const auto before=result.diagnostics.size();
            auto *resolved=definition(previousLink,owner,widget.instanceId);
            if(!resolved) {
                widget.typeDefinition->unresolvedReason=result.diagnostics.size()>before?result.diagnostics.back().message:"The Type Def source remains unresolved.";
                result.changed=result.changed || widget.typeDefinition->unresolvedReason!=previousLink.unresolvedReason;
                return;
            }
            auto current=resolved->definition.root;
            // Nested links were resolved in their declaring source. Keep that
            // canonical reference when the schema is copied into another owner.
            const auto previous=deserializeTypeDefinitionWidget(previousLink.baselineWidgetJson);
            if(!previous) {
                appendDiagnostic(result,"typedef.baseline.invalid","The linked instance has no complete readable previous schema; locate or replace its definition explicitly.",widget.instanceId);
                widget.typeDefinition->unresolvedReason=result.diagnostics.back().message;return;
            }
            const bool approve=acceptProposed && (acceptedInstanceId.empty() || approvedRoot);
            auto migration=migrateTypeDefinitionWidget(widget,*previous,current,approve);
            if(!migration.accepted()) {
                for(auto &issue:migration.diagnostics) {
                    issue.instanceId=widget.instanceId; // The field path qualifies the local linked instance.
                    if(result.diagnostics.size()<kMaximumFrogSourceDiagnostics)result.diagnostics.push_back(std::move(issue));
                }
                widget.typeDefinition->unresolvedReason=result.diagnostics.back().message;
                result.changed=result.changed || widget.typeDefinition->unresolvedReason!=previousLink.unresolvedReason;return;
            }
            auto link=previousLink;
            if(resolvingDefinitionDepth)link.sourcePath=resolved->source.canonicalUri;
            link.resolvedRevision=resolved->definition.revision;
            link.resolvedSchemaFingerprint=typeDefinitionSchemaFingerprint(resolved->definition);
            link.baselineWidgetJson=serializeArrayElementWidget(current);
            link.unresolvedReason.clear();link.iconSvgText=resolved->definition.iconSvgText;
            const bool changedInCluster=!path.empty() && link.resolvedSchemaFingerprint!=previousLink.resolvedSchemaFingerprint &&
                !typeDefinitionSchemaEquivalent(*previous,current);
            if(changedInCluster && !migrateEnclosingDefaults(path,*previous,current,widget.instanceId,approve)) {
                widget.typeDefinition->unresolvedReason=result.diagnostics.back().message;
                result.changed=result.changed || widget.typeDefinition->unresolvedReason!=previousLink.unresolvedReason;
                return;
            }
            result.changed=result.changed || migration.changed || !(link==previousLink);
            widget=std::move(migration.proposedWidget);widget.typeDefinition=std::move(link);
            if(changedInCluster)static_cast<void>(refreshContainedWidgetSchemaMetadata(*path.front().cluster));
            if(resolvedRoot)*resolvedRoot=&resolved->definition;
        }
        if(auto *cluster=clusterContainerState(widget))
            for(auto &field:cluster->elements)if(field.widget) {
                path.push_back({&widget,field.fieldId});
                resolveWidget(*field.widget,owner,depth+1,approvedRoot,nullptr,path);
                path.pop_back();
            }
        if(auto *array=arrayContainerState(widget);array && array->elementTemplate)
            resolveWidget(*array->elementTemplate,owner,depth+1,approvedRoot);
    }

    void seal() {
        if(result.dependencies.empty())return;
        std::vector<std::pair<std::string,std::string>> schemas;
        for(const auto &dependency:result.dependencies)schemas.emplace_back(dependency.definitionId,dependency.schemaFingerprint);
        std::sort(schemas.begin(),schemas.end());
        std::string framed;
        for(const auto &[identity,schema]:schemas)framed+=std::to_string(identity.size())+":"+identity+std::to_string(schema.size())+":"+schema;
        result.schemaFingerprint=hashing::sha256Hex(framed);
    }
};
}

bool draftHasTypeDefinitionLinks(const FrogDraftDataView &document) {
    bool linked=false;
    for(const auto &object:document.canvas.objects())if(object.widget)
        visitWidget(*object.widget,[&](const WidgetInstance &widget){linked=linked || bool(widget.typeDefinition);});
    for(const auto &node:document.diagramNodes) {
        linked=linked || bool(node.typeDefinition);
        if(node.arrayElementTemplate)visitWidget(*node.arrayElementTemplate,[&](const WidgetInstance &widget){linked=linked || bool(widget.typeDefinition);});
    }
    return linked;
}

DraftTypeDefinitionResolution reconcileTypeDefinitionTabChildren(FrogWidgetData &scene,std::string_view rootId,
    const TypeDefinitionDocument &definition,bool acceptProposed) {
    DraftTypeDefinitionResolution result;
    const std::string root(rootId); // Adding a child can reallocate the original root record.
    const auto *rootRecord=scene.objectById(root);
    if(!rootRecord||!rootRecord->widget||!rootRecord->widget->typeDefinition||
        rootRecord->widget->typeDefinition->definitionId!=definition.id) {
        appendDiagnostic(result,"typedef.tab_owner","The Tab has no matching Type Definition identity.",root);return result;
    }
    const auto &oldBindings=rootRecord->widget->typeDefinition->tabChildBindings;
    if(definition.tabChildren.empty()&&oldBindings.empty()&&!isTabWidget(*rootRecord->widget))return result;
    const auto localChildren=typeDefinitionTabChildren(scene,root);
    if(definition.tabChildren.empty()&&oldBindings.empty()&&localChildren.empty())return result;
    std::set<std::string,std::less<>> descendants,used;
    for(const auto &object:localChildren)descendants.insert(object.id);
    for(const auto &object:scene.objects()) {
        used.insert(object.id);
        if(object.widget)visitWidget(*object.widget,[&](const auto &w){used.insert(w.instanceId);});
    }
    for(const auto &[member,local]:oldBindings)if(used.contains(local)&&!descendants.contains(local)) {
        appendDiagnostic(result,"typedef.tab_member_identity",
            "A linked Tab member belongs to another object. Its identity cannot be reassigned.",root,member);return result;
    }
    std::map<std::string,std::string,std::less<>> bindings;
    std::map<std::string,std::string,std::less<>> identities{{definition.root.instanceId,root}};
    std::size_t serial=0;
    const auto allocate=[&](){
        std::string id;
        do{id=root+".member-"+std::to_string(++serial);}while(!used.insert(id).second);
        return id;
    };
    for(const auto &source:definition.tabChildren) {
        const auto known=oldBindings.find(source.id);
        auto id=known!=oldBindings.end()&&descendants.contains(known->second)?known->second:allocate();
        bindings.emplace(source.id,id);identities.emplace(source.id,std::move(id));
    }
    // Prepare all new identities first. References between two page controls
    // can then be projected even when the target is later in the scene order.
    for(const auto &source:definition.tabChildren)if(source.widget) {
        const auto *local=scene.widgetById(bindings.at(source.id));
        const auto mapMembers=[&](auto &&self,const WidgetInstance &widget,const WidgetInstance *existing)->void {
            if(const auto *fields=clusterContainerState(widget))for(const auto &field:fields->elements)if(field.widget) {
                const WidgetInstance *known=nullptr;
                if(existing)if(const auto *current=clusterContainerState(*existing))
                    for(const auto &entry:current->elements)if(entry.fieldId==field.fieldId){known=entry.widget.get();break;}
                identities.emplace(field.widget->instanceId,known?known->instanceId:allocate());
                self(self,*field.widget,known);
            }
            if(const auto *array=arrayContainerState(widget);array&&array->elementTemplate) {
                const auto *current=existing?arrayContainerState(*existing):nullptr;
                auto *known=current?current->elementTemplate.get():nullptr;
                identities.emplace(array->elementTemplate->instanceId,known?known->instanceId:allocate());
                self(self,*array->elementTemplate,known);
            }
        };
        mapMembers(mapMembers,*source.widget,local);
    }
    const auto remap=[&](auto &&self,WidgetInstance &widget)->bool {
        bool valid=true;
        visitWidget(widget,[&](WidgetInstance &part) {
            const auto original=part.instanceId;
            if(const auto found=identities.find(original);found!=identities.end())part.instanceId=found->second;
            part.publicInputId.reset();part.publicOutputId.reset();part.interfaceMapSlotId.reset();
            part.interfaceConnectionRequirement.reset();part.detachedInterfaceBinding.reset();
            if(part.referenceValue) {
                auto &target=part.referenceValue->target;
                if(target.kind==ApplicationControlTargetKind::Widget&&target.ownerDocumentId.empty())
                    if(const auto found=identities.find(target.widgetId);found!=identities.end())target.widgetId=found->second;
            }
            if(part.typeDefinition)
                for(auto &[key,value]:part.typeDefinition->tabChildBindings)
                    if(const auto found=identities.find(value);found!=identities.end())value=found->second;
            if(part.defaultData) {
                auto saved=data_operations::decodeStoredDefault(part,*part.defaultData);
                if(!saved||saved->instanceId!=original||!self(self,*saved)) {valid=false;return;}
                auto encoded=data_operations::encode(*saved);
                if(encoded.empty()){valid=false;return;}
                part.defaultData=std::move(encoded);
            }
        });
        return valid;
    };
    // A failed member migration must leave the entire occurrence intact,
    // including independently wired children and their existing identities.
    auto prepared=scene;
    const auto offsetX=rootRecord->bounds.x-definition.root.bounds.x,offsetY=rootRecord->bounds.y-definition.root.bounds.y;
    for(const auto &source:definition.tabChildren) {
        const auto parent=identities.find(source.parentTabId);
        if(parent==identities.end()) {
            appendDiagnostic(result,"typedef.tab_owner","The definition is missing a member's parent Tab.",root,source.id);return result;
        }
        const auto &id=bindings.at(source.id);
        SceneObject model=source;model.id=id;model.parentTabId=parent->second;
        model.bounds.x+=offsetX;model.bounds.y+=offsetY;
        if(model.widget) {
            if(!remap(remap,*model.widget)) {
                appendDiagnostic(result,"typedef.tab_member_default","A Tab member's saved default could not be assigned to its new identity.",root,source.id);return result;
            }
            model.widget->bounds=model.bounds;
        }
        auto *existing=prepared.objectById(id);
        if(!existing){prepared.addObject(std::move(model),true);result.changed=true;continue;}
        if(bool(existing->widget)!=bool(model.widget)) {
            appendDiagnostic(result,"typedef.tab_member_kind","A linked Tab member's identity changed between decoration and control.",root,source.id);return result;
        }
        if(model.widget) {
            const auto role=widgetRole(*model.widget);
            const bool changedDirection=role&&widgetRole(*existing->widget)!=role;
            if(changedDirection&&!acceptProposed) {
                appendDiagnostic(result,"typedef.tab_member_direction","A Tab member's terminal direction changed. Review its existing wires before accepting the new role.",root,source.id);return result;
            }
            auto outcome=migrateTypeDefinitionWidget(*existing->widget,*existing->widget,*model.widget,acceptProposed);
            if(!outcome.accepted()) {
                for(auto &issue:outcome.diagnostics)appendDiagnostic(result,issue.code,issue.message,root,source.id+issue.fieldPath);
                return result;
            }
            const auto &oldLink=outcome.proposedWidget.typeDefinition,&newLink=model.widget->typeDefinition;
            const bool linkChanged=bool(oldLink)!=bool(newLink)||(oldLink&&*oldLink!=*newLink);
            if(changedDirection) {
                static_cast<void>(setWidgetRole(outcome.proposedWidget,*role));
                if(widgetRole(outcome.proposedWidget)!=role) {
                    appendDiagnostic(result,"typedef.tab_member_direction","The updated Tab member cannot provide its declared role.",root,source.id);return result;
                }
            }
            result.changed=result.changed||outcome.changed||linkChanged||changedDirection;
            outcome.proposedWidget.typeDefinition=model.widget->typeDefinition;
            existing->widget=std::move(outcome.proposedWidget);existing->bounds=existing->widget->bounds;
        } else if(model.staticVisual&&existing->staticVisual) {
            const auto &a=*model.staticVisual,&b=*existing->staticVisual;
            if(a.kind!=b.kind||a.svgText!=b.svgText||a.mediaRef!=b.mediaRef||a.sourceName!=b.sourceName||
                a.naturalWidth!=b.naturalWidth||a.naturalHeight!=b.naturalHeight||a.flipHorizontal!=b.flipHorizontal||
                a.flipVertical!=b.flipVertical||a.rotationQuarterTurns!=b.rotationQuarterTurns||
                a.preserveAspectRatio!=b.preserveAspectRatio||(!staticVisualUsesEncodedRasterSource(a)&&a.dibBytes!=b.dibBytes)) {
                existing->staticVisual=model.staticVisual;result.changed=true;
            }
        }
        result.changed=result.changed||existing->parentTabId!=model.parentTabId||existing->parentTabPageId!=model.parentTabPageId;
        existing->parentTabId=model.parentTabId;existing->parentTabPageId=model.parentTabPageId;
    }
    for(const auto &[member,id]:oldBindings)if(!bindings.contains(member))if(auto *object=prepared.objectById(id)) {
        if(!acceptProposed) {
            appendDiagnostic(result,"typedef.tab_member_removed","A linked Tab member was removed. Approve detaching its local control to retain its data and diagram connections.",root,member);return result;
        }
        object->parentTabId.clear();object->parentTabPageId.clear();result.changed=true;
    }
    // Local controls added on an occurrence are independent. When their
    // page is removed they also require review before they can be detached.
    for(const auto &local:localChildren) {
        auto *object=prepared.objectById(local.id);
        if(!object||object->parentTabId.empty())continue;
        const auto *parent=prepared.widgetById(object->parentTabId);
        if(parent&&tabPageIndex(*parent,object->parentTabPageId))continue;
        if(!acceptProposed) {
            appendDiagnostic(result,"typedef.tab_page_removed","A local control is on a removed page. Approve detaching it to keep its existing terminal and data.",root,local.id);return result;
        }
        object->parentTabId.clear();object->parentTabPageId.clear();result.changed=true;
    }
    result.changed=result.changed||bindings!=oldBindings;
    if(result.changed) {
        prepared.objectById(root)->widget->typeDefinition->tabChildBindings=std::move(bindings);
        scene=std::move(prepared);
    }
    return result;
}

void resolveTypeDefinitionDocument(FrogDraftData &document,std::string_view owner,
    Resolver &resolver,bool acceptProposed,std::string_view acceptedInstanceId) {
    if(!draftHasTypeDefinitionLinks(document))return;
    if(!resolver.sourceBytes.contains(owner))resolver.sourceBytes.emplace(std::string(owner),
        std::make_shared<const std::string>(serializeFrogDraftData(document)));
    const auto before=document;
    // Snapshot the IDs before a linked Tab materializes new page children.
    std::vector<std::string> ids;
    for(const auto &object:document.widgets.objects())if(object.widget)ids.push_back(object.id);
    for(const auto &id:ids)if(auto *object=document.widgets.objectById(id);object && object->widget) {
        const auto prior=object->widget->typeDefinition?object->widget:std::nullopt;
        const TypeDefinitionDocument *definition=nullptr;
        resolver.resolveWidget(*object->widget,owner,0,false,&definition);
        if(definition && prior && (isTabWidget(*prior)||isTabWidget(definition->root))) {
            auto members=reconcileTypeDefinitionTabChildren(document.widgets,id,*definition,
                acceptProposed&&(acceptedInstanceId.empty()||acceptedInstanceId==id));
            object=document.widgets.objectById(id);
            if(!members.accepted()) {
                object->widget=prior;object->widget->typeDefinition->unresolvedReason=members.diagnostics.front().message;
                for(auto &issue:members.diagnostics)
                    if(resolver.result.diagnostics.size()<kMaximumFrogSourceDiagnostics)resolver.result.diagnostics.push_back(std::move(issue));
            } else resolver.result.changed=resolver.result.changed||members.changed;
        }
        const auto bounds=object->widget->bounds;
        if(object->bounds.x!=bounds.x || object->bounds.y!=bounds.y || object->bounds.width!=bounds.width || object->bounds.height!=bounds.height)
            object->bounds=bounds;
    }
    std::vector<std::string> constants;
    for(const auto &node:document.diagramNodes)if(node.typeDefinition)constants.push_back(node.id);
    for(const auto &id:constants) {
        auto target=std::find_if(document.diagramNodes.begin(),document.diagramNodes.end(),[&](const auto &node){return node.id==id;});
        if(target==document.diagramNodes.end())continue; // A deliberately approved parent-field removal.
        std::string error;
        auto value=typeDefinitionConstantWidget(document.diagramNodes,id,&error);
        if(!value) {
            target->typeDefinition->unresolvedReason=error;
            appendDiagnostic(resolver.result,"typedef.constant.invalid",error,id);continue;
        }
        const auto before=resolver.result.diagnostics.size();
        resolver.resolveWidget(*value,owner,0);
        if(resolver.result.diagnostics.size()!=before) {
            target->typeDefinition=value->typeDefinition;continue;
        }
        if(!installTypeDefinitionConstantWidget(document.diagramNodes,id,*value,&error))
            appendDiagnostic(resolver.result,"typedef.constant.install_failed",std::move(error),id);
    }
    for(auto &node:document.diagramNodes)if(node.arrayElementTemplate)
        resolver.resolveWidget(*node.arrayElementTemplate,owner,0);
    // Widget terminal schemas come from the exact linked widget, never a label
    // or a nominal Type Def name. The ordinary resolver still checks all wires.
    for(auto &node:document.diagramNodes)if(node.kind=="widget_value")
        if(const auto *widget=document.widgets.widgetById(node.widgetId)) {
            const auto type=diagramTerminalValueType(*widget);
            if(node.type!=type){node.type=type;resolver.result.changed=true;}
        }
    reconcileDependentNodes(document,before,resolver.result,acceptProposed,acceptedInstanceId);
    if(document.declaredInterface) {
        const auto inspection=inspectFrogDraftInterface(*document.declaredInterface);
        for(const auto &object:document.widgets.objects())if(object.widget) {
            const auto &widget=*object.widget;
            const auto type=binding::widgetType(widget);
            if(!type.valid()||!type.type.known())continue;
            for(const bool input:{true,false}) {
                const auto &binding=input?widget.publicInputId:widget.publicOutputId;
                if(!binding)continue;
                const auto port=std::find_if(inspection.ports.begin(),inspection.ports.end(),[&](const auto &entry){return entry.id==*binding && entry.input==input;});
                if(port==inspection.ports.end()||port->typeExpression==type.type.canonical())continue;
                std::string error;
                if(document.declaredInterface->editPortForAuthoring(port->id,input,port->typeExpression,input,type.type.canonical(),
                    input?std::optional<std::string_view>{port->connection}:std::nullopt,&error))resolver.result.changed=true;
                else appendDiagnostic(resolver.result,"typedef.interface.update_failed",std::move(error),widget.instanceId,*binding);
            }
        }
    }
}

DraftTypeDefinitionResolution resolveDraftTypeDefinitionLinks(FrogDraftData &document,
    std::string_view owner,const DraftDependencyResolver &loader,bool acceptProposed,std::string_view acceptedInstanceId) {
    Resolver resolver;resolver.acceptProposed=acceptProposed;resolver.acceptedInstanceId=acceptedInstanceId;resolver.loader=loader;
    resolveTypeDefinitionDocument(document,owner,resolver,acceptProposed,acceptedInstanceId);
    resolver.seal();return std::move(resolver.result);
}

DraftTypeDefinitionResolution inspectTypeDefinitionClosure(std::string_view,std::string_view,FrogDraftData *,bool,bool = true,
    const DraftDependencyResolver & = {});

DraftResolvedTypeDefinitionSource prepareDraftTypeDefinitionSource(std::string_view source,
    std::string_view uri,const DraftDependencyResolver &loader,bool acceptProposed,bool inspectSubprograms) {
    DraftResolvedTypeDefinitionSource out;out.sourceUtf8=source;
    std::string error;auto program=readFrogDraftData(source,&error);
    // The ordinary reader owns malformed/unsupported envelopes and public
    // Source diagnostics; this adapter does not admit or repair those cases.
    if(!program || program->publicSourceAuthority || program->privateSourceAwaitingDiagramMigration)return out;
    auto document=*program;
    out.resolution=inspectTypeDefinitionClosure(source,uri,&document,acceptProposed,inspectSubprograms,loader);
    if(!out.accepted())return out;
    if(!draftHasTypeDefinitionLinks(*program))return out;
    out.linked=true;
    if(out.accepted()) {
        std::vector<std::string> ids;
        for(const auto &object:document.widgets.objects())if(object.widget)ids.push_back(object.id);
        for(const auto &id:ids)if(auto *object=document.widgets.objectById(id);object && object->widget)
            stripExecutionLinks(*object->widget);
        for(auto &node:document.diagramNodes) {
            node.typeDefinition.reset();
            if(node.arrayElementTemplate)stripExecutionLinks(*node.arrayElementTemplate);
        }
        out.sourceUtf8=serializeFrogDraftData(document);
    }
    return out;
}

DraftTypeDefinitionResolution inspectTypeDefinitionClosure(std::string_view source,std::string_view uri,
    FrogDraftData *resolvedRoot,bool acceptProposed,bool inspectSubprograms,const DraftDependencyResolver &loader) {
    Resolver resolver;resolver.acceptProposed=acceptProposed;resolver.loader=loader;
    struct Pending {std::shared_ptr<const std::string> bytes;std::string uri;bool root=false;};
    std::vector<Pending> pending{{std::make_shared<const std::string>(source),std::string(uri),true}};
    std::set<std::string> visited;
    std::size_t bytes=source.size(),programs=0;
    while(!pending.empty()) {
        auto current=std::move(pending.back());pending.pop_back();
        if(!visited.insert(current.uri).second)continue;
        if(++programs>maximumDefinitions){appendDiagnostic(resolver.result,"typedef.program_closure.limit","Type Def source inspection exceeds its bounded sub-FROG count.");break;}
        const auto enqueue=[&](std::string_view path,const std::string &pointer) {
            const auto callee=resolveDraftDependency({DraftDependencyKind::Source,current.uri,std::string(path),pointer,{}, {}, {}},loader);
            if(callee.state!=DraftDependencyState::Present||!callee.bytes)return true;
            if(bytes>maximumClosureBytes||callee.bytes->size()>maximumClosureBytes-bytes) {
                appendDiagnostic(resolver.result,"typedef.program_closure.limit","Type Def source inspection exceeds its bounded byte count.");
                return false;
            }
            bytes+=callee.bytes->size();
            pending.push_back({callee.bytes,callee.canonicalUri,false});
            return true;
        };
        if(inspectSubprograms && current.bytes->find(kPolymorphicFrogFormat)!=std::string::npos) {
            if(const auto family=readPolymorphicFrog(*current.bytes)) {
                for(std::size_t index=0;index<family->members.size();++index)
                    if(!enqueue(family->members[index].documentPath,"/members/"+std::to_string(index)+"/document_path"))break;
                continue;
            }
        }
        auto program=readFrogDraftData(*current.bytes);
        if(!program || program->publicSourceAuthority || program->privateSourceAwaitingDiagramMigration)continue;
        auto document=std::move(*program);
        resolver.sourceBytes.insert_or_assign(current.uri,current.bytes);
        resolveTypeDefinitionDocument(document,current.uri,resolver,acceptProposed,{});
        if(inspectSubprograms)for(const auto &node:document.diagramNodes)if(node.kind=="frog_call" && !node.callDocumentPath.empty()) {
            if(!enqueue(node.callDocumentPath,resolver.pointer(current.uri,node.id,"/call/document_path")))break;
        }
        if(current.root&&resolvedRoot)*resolvedRoot=std::move(document);
    }
    resolver.seal();return std::move(resolver.result);
}
DraftTypeDefinitionResolution inspectDraftTypeDefinitionDependencies(std::string_view source,std::string_view uri,const DraftDependencyResolver &loader) {
    return inspectTypeDefinitionClosure(source,uri,nullptr,false,true,loader);
}

} // namespace frog::native_engine
