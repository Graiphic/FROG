#include "FrogDraftJson.hpp"
#include "frog_native_engine/DraftSourcePreparation.hpp"
#include "frog_native_engine/DraftClassProjection.hpp"
#include "CanonicalComposedDraftMigration.hpp"
#include "CanonicalJsonSupport.hpp"
#include "FrogDocumentArrayStorage.hpp"
#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "frog_native_engine/DraftConditionalDisableSpecialization.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include "frog_native_engine/DiagramTunnelIdentity.hpp"
#include "frog_native_engine/DraftInterfaceInspection.hpp"
#include "frog_native_engine/DraftSemanticPreparation.hpp"
#include "frog_native_engine/FrogDocumentFormat.hpp"
#include "frog_native_engine/SequenceStructure.hpp"
#include "frog_native_engine/SequenceFramePassageProjection.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include <iterator>
#include <deque>
#include <mutex>
#include <cmath>
#include <algorithm>
#include <charconv>
#include <map>
#include <set>
#include <stdexcept>

namespace frog::native_engine {
namespace {
using Json=source::JsonValue;
namespace cj=canonical_json;
constexpr std::size_t maximumPrograms=1024, maximumNodes=1024, maximumEdges=4096;
constexpr std::size_t maximumClosureBytes=64U*1024U*1024U, maximumDepth=32;
// Only successful raw-field checks for ordinary callees are reusable across
// edits. They depend solely on these exact bytes. Context-sensitive checks,
// dependency reads, call boundaries and whole-graph admission always run.
struct ValidatedCalleeFields {
    std::mutex mutex;
    std::deque<std::shared_ptr<const std::string>> sources;
    bool contains(const std::string &bytes) {
        std::lock_guard lock(mutex);
        return std::any_of(sources.begin(),sources.end(),[&](const auto &entry){return *entry==bytes;});
    }
    void remember(const std::shared_ptr<const std::string> &bytes) {
        std::lock_guard lock(mutex);
        if(sources.size()==16)sources.pop_front();
        sources.push_back(bytes);
    }
};
ValidatedCalleeFields &validatedCalleeFields() {
    static ValidatedCalleeFields cache;
    return cache;
}
struct Refusal { DraftSubFrogExecutionDiagnostic diagnostic; };
[[noreturn]] void refuse(std::string code,std::string uri,std::string path,std::string message,
    std::string callsite={},bool unsupported=false,std::optional<DocumentDiagnosticDomain> domain=std::nullopt) {
    throw Refusal{{std::move(code),std::move(uri),std::move(path),std::move(message),std::move(callsite),unsupported,
        domain.value_or(unsupported?DocumentDiagnosticDomain::FrontendCapability:DocumentDiagnosticDomain::Semantics)}};
}
void frame(std::string &out,std::string_view value) { out+=std::to_string(value.size())+":";out.append(value); }
std::string text(const Json *value) {return value&&value->type==Json::Type::String?value->string:std::string{};}
const Json &object(const Json *value,const std::string &uri,const std::string &path,const std::string &callsite) {
    if(!value||value->type!=Json::Type::Object)refuse("subfrog.interface.invalid",uri,path,"An explicit call object is required.",callsite);
    return *value;
}
void fields(const Json &value,std::initializer_list<std::string_view> names,
    const std::string &uri,const std::string &path,const std::string &callsite) {
    object(&value,uri,path,callsite);
    for(const auto &[key,_]:value.object)if(std::find(names.begin(),names.end(),key)==names.end())
        refuse("subfrog.source.unknown_field",uri,cj::pointer(path,key),"This field has no Sub-FROG projection contract.",callsite,true,DocumentDiagnosticDomain::Compatibility);
}
// The type spelling of a primitive output may omit display names when its
// field identities already supply those names. A front-panel Cluster saves
// them explicitly. Keep nominal domains, field order and custom labels exact.
bool sameCallBoundaryType(const binding::TypeRef &producer,const binding::TypeRef &port) {
    if(producer!=port)return false;
    std::vector<std::pair<const binding::TypeRef*,const binding::TypeRef*>> pending{{&producer,&port}};
    while(!pending.empty()) {
        const auto [a,b]=pending.back();pending.pop_back();
        if(a->choiceKind!=b->choiceKind||a->choices!=b->choices)return false;
        const auto &left=a->fieldNames.empty()?a->fieldIds:a->fieldNames;
        const auto &right=b->fieldNames.empty()?b->fieldIds:b->fieldNames;
        if(left!=right)return false;
        for(std::size_t i=0;i<a->children.size();++i)pending.emplace_back(&a->children[i],&b->children[i]);
    }
    return true;
}

struct SourceIndex {
    std::map<std::string,std::string> nodes,edges;
    std::map<std::string,const Json*> rawNodes;
};
void indexSource(const Json &scope,const std::string &path,SourceIndex &index,unsigned depth=0) {
    if(depth>maximumDepth)throw std::runtime_error("Source nesting exceeds32.");
    if(const auto *nodes=scope.member("nodes");nodes&&nodes->type==Json::Type::Array)
        for(std::size_t n=0;n<nodes->array.size();++n) {
            const auto &node=nodes->array[n];const auto id=text(node.member("id"));const auto at=path+"/nodes/"+std::to_string(n);
            if(!id.empty()){index.nodes.emplace(id,at);index.rawNodes.emplace(id,&node);}
            if(const auto *regions=node.member("regions");regions&&regions->type==Json::Type::Array)
                for(std::size_t r=0;r<regions->array.size();++r)if(const auto *diagram=regions->array[r].member("diagram"))
                    indexSource(*diagram,at+"/regions/"+std::to_string(r)+"/diagram",index,depth+1);
        }
    if(const auto *edges=scope.member("edges");edges&&edges->type==Json::Type::Array)
        for(std::size_t e=0;e<edges->array.size();++e)index.edges.emplace(text(edges->array[e].member("id")),path+"/edges/"+std::to_string(e));
}
using NominalClusterPorts=std::map<std::string,std::string,std::less<>>;
struct Frozen {
    NominalClusterPorts nominalPorts;
    std::map<std::string,NominalClusterPorts,std::less<>> nominalCalls;
    DraftSubFrogFrozenSource source;
    FrogDraftData active;
    Json raw;
    SourceIndex index;
    std::string identity;
    std::map<std::string,ClassCallTarget,std::less<>> memberCalls;
};
struct Expanded {
    FrogDraftData document;
    std::map<std::string,DraftSubFrogExecutionIdentity> nodes,edges;
    std::vector<DraftSubFrogExecutionIdentity> retired;
};
struct Projection {
    const DraftSubFrogExecutionInput &input;
    DraftSubFrogExecutionProjection &out;
    const Json *normalizedRoot;
    std::map<std::string,Frozen> frozen;
    std::map<std::string,DraftSubFrogFrozenSource> frozenTypes;
    std::map<std::string,DraftSubFrogFrozenSource> frozenFamilies;
    std::optional<CapturedClassProgram> classes;
    std::size_t bytes=0,invocations=0;
    std::set<std::string> reserved;
    std::string rootKey;

    std::string fresh(std::string_view callPath,std::string_view role,std::string_view sourceId) {
        std::string seed;frame(seed,callPath);frame(seed,role);frame(seed,sourceId);
        const auto id="__subfrog_"+hashing::sha256Hex(seed);
        if(!reserved.insert(id).second)refuse("subfrog.dependency.limit",input.sourceUri,"/diagram","A generated execution identity collided; no source identity was reused.");
        return id;
    }
    Frozen &retain(DraftDependencyResult source,bool root=false) {
        const auto uri=source.canonicalUri;
        const auto key=uri;
        if(const auto found=frozen.find(key);found!=frozen.end()) {
            if(found->second.source.source.sha256!=source.sha256)
                refuse("subfrog.source.changed",uri,"/","The same canonical source was resolved to different bytes.",{},false,DocumentDiagnosticDomain::Dependency);
            return found->second;
        }
        if(source.state!=DraftDependencyState::Present||!source.bytes)
            refuse("subfrog.source.unavailable",uri,"/diagram",source.message.empty()?"The referenced source is unavailable.":source.message,{},false,DocumentDiagnosticDomain::Dependency);
        if(frozen.size()+frozenFamilies.size()>=maximumPrograms||source.bytes->size()>maximumClosureBytes-bytes)
            refuse("subfrog.dependency.limit",uri,"/diagram","The frozen dependency closure exceeds1024 programs or64MiB.");
        if(source.bytes->empty())refuse("subfrog.source.invalid",uri,"/","A frozen source requires its exact current bytes.");
        bytes+=source.bytes->size();
        std::string error;
        std::optional<std::string> storage;
        std::optional<FrogDraftData> owner;
        std::optional<Json> parsed;
        std::string_view normalized=*source.bytes;
        if(root && normalizedRoot) {
            parsed=*normalizedRoot;
        } else {
            storage=prepareExecutionStorageSource(*source.bytes,&error);
            if(!storage)refuse("subfrog.source.invalid",uri,"/storage",error);
            normalized=*storage;
            parsed=readFrogDraftJson(normalized,&error);
            owner=readFrogDraftData(normalized,&error);
            if(!parsed||!owner)refuse("subfrog.source.invalid",uri,"/",error.empty()?"The source has no recognized document model.":error);
            if(owner->publicSourceAuthority||owner->privateSourceAwaitingDiagramMigration)
                refuse("subfrog.source.unsupported_profile",uri,"/","Only a recognized authored Diagram is admitted by this projection.",{},true);
        }
        const auto &authority=root?*input.rootDocument:*owner;
        const auto untranslated = std::find_if(authority.diagramNodes.begin(), authority.diagramNodes.end(),
            isUntranslatedLabViewNode);
        if (untranslated != authority.diagramNodes.end()) {
            SourceIndex index;
            if (const auto *diagram = parsed->member("diagram")) indexSource(*diagram, "/diagram", index);
            const auto path = index.nodes.find(untranslated->id);
            refuse(kUntranslatedLabViewNodeDiagnostic, uri,
                path == index.nodes.end() ? "/diagram" : path->second,
                untranslatedLabViewNodeMessage(*untranslated));
        }
        auto selected=specializeConditionalDisableData(authority,input.compilationContext);
        if(!selected.diagnostics.empty()) {
            const auto &issue=selected.diagnostics.front();refuse(issue.code,uri,"/diagram",issue.message,{},false,issue.incomplete?DocumentDiagnosticDomain::Control:DocumentDiagnosticDomain::Semantics);
        }
        DraftResolvedTypeDefinitionSource typeDefinitions;
        if(draftHasTypeDefinitionLinks(authority)) {
            typeDefinitions=prepareDraftTypeDefinitionSource(normalized,uri,input.resolver,false,false);
            if(!typeDefinitions.accepted()) {
                const auto &issue=typeDefinitions.resolution.diagnostics.front();refuse(issue.code,uri,issue.fieldPath,issue.message,{},false,DocumentDiagnosticDomain::Dependency);
            }
        }
        if(!(root&&input.rootSourceFieldsValidated)) {
            const bool reusable=!root && source.bytes->size()<=128U*1024U && !typeDefinitions.linked &&
                !draftHasClasses(authority) && std::none_of(authority.diagramNodes.begin(),authority.diagramNodes.end(),[](const auto &node) {
                    return node.type=="frog.studio.structures.conditional_disable_structure" || node.type=="frog.studio.structures.disabled_structure";
                });
            if(!reusable || !validatedCalleeFields().contains(*source.bytes)) {
                for(const auto &issue:validateCanonicalSubFrogProjectionSource(normalized,selected.document,
                    typeDefinitions.accepted()&&typeDefinitions.linked))if(issue.blocking)
                    refuse(issue.code,uri,issue.path,issue.message,{},issue.domain==DocumentDiagnosticDomain::FrontendCapability,issue.domain);
                if(reusable)validatedCalleeFields().remember(source.bytes);
            }
        }
        if(typeDefinitions.linked) {
            auto execution=readFrogDraftData(typeDefinitions.sourceUtf8,&error);
            if(!execution)refuse("subfrog.source.invalid",uri,"/",error);
            selected=specializeConditionalDisableData(*execution,input.compilationContext);
            if(!selected.diagnostics.empty()) {
                const auto &issue=selected.diagnostics.front();refuse(issue.code,uri,"/diagram",issue.message,{},false,issue.incomplete?DocumentDiagnosticDomain::Control:DocumentDiagnosticDomain::Semantics);
            }
        }
        std::map<std::string,std::string,std::less<>> classOrigins;
        std::map<std::string,ClassCallTarget,std::less<>> memberCalls;
        Frozen entry;
        SourceIndex originalIndex;
        if(draftHasClasses(selected.document)) {
            if(!classes)refuse("class.closure_required",uri,"/classes","The root program must explicitly capture its class closure.");
            if(const auto *diagram=parsed->member("diagram"))indexSource(*diagram,"/diagram",originalIndex);
            if(selected.document.declaredInterface) {
                const auto signature=inspectFrogDraftInterface(*selected.document.declaredInterface);
                for(const auto &p:signature.ports)if(clusterContainsClassValue(p.parsedType.type))
                    entry.nominalPorts.emplace(p.id,p.parsedType.type.canonical());
            }
            for(const auto &n:selected.document.diagramNodes)if(n.kind=="frog_call" && !n.classOperation) {
                for(const auto &p:n.callPorts) {
                    const auto type=binding::parseType(p.valueType);
                    if(type.valid() && clusterContainsClassValue(type.type))
                        entry.nominalCalls[n.id].emplace(p.sourcePortId.empty()?p.id:p.sourcePortId,type.type.canonical());
                }
            }
            auto lowered=projectDraftClassOperations(selected.document,*parsed,*classes,uri,source.sha256,input.resolver,root);
            if(!lowered.accepted()) {
                const auto &issue=lowered.issues.front();refuse(issue.code,issue.sourceUri,issue.pointer,issue.message);
            }
            classOrigins=std::move(lowered.generatedFrom);memberCalls=std::move(lowered.memberCalls);selected.document=std::move(lowered.document);
            parsed=readFrogDraftJson(serializeFrogDraftData(selected.document),&error);
            if(!parsed)refuse("class.projection_invalid",uri,"/diagram",error);
            out.changed=true;
        }
        selected.document.properties.arrayStorageMode=FrogArrayStorageMode::Readable;
        for(const auto &dependency:typeDefinitions.resolution.dependencies) {
            const auto &captured=dependency.source;
            const auto &typeKey=captured.canonicalUri;
            if(const auto previous=frozenTypes.find(typeKey);previous!=frozenTypes.end()) {
                if(previous->second.source.sha256!=captured.sha256)
                    refuse("subfrog.source.changed",typeKey,"/","A Type Definition changed while its schema was being frozen.",{},false,DocumentDiagnosticDomain::Dependency);
                continue;
            }
            const auto definition=captured.bytes?deserializeTypeDefinition(*captured.bytes,&error):std::nullopt;
            if(!definition||definition->id!=dependency.definitionId||definition->revision!=dependency.revision||
               typeDefinitionSchemaFingerprint(*definition)!=dependency.authoredSchemaFingerprint)
                refuse("subfrog.source.invalid",typeKey,"/","The Type Definition's frozen source and resolved schema disagree.");
            if(captured.bytes->size()>maximumClosureBytes-bytes)refuse("subfrog.dependency.limit",uri,"/","Frozen definitions exceed the closure byte budget.");
            bytes+=captured.bytes->size();
            frozenTypes.emplace(typeKey,DraftSubFrogFrozenSource{captured,dependency.schemaFingerprint});
        }
        for(auto &node:selected.document.diagramNodes)if(node.kind=="conditional_default") {
            const auto type=binding::parseType(node.valueType);
            if(!type.valid()||!type.type.known()||type.type.kind!=binding::TypeKind::Primitive||
               (type.type.name!="bool"&&type.type.name!="string"&&!type.type.numeric()))
                refuse("preparation.disabled.default_type_unavailable",uri,"/diagram","The existing compile-time default literal codec does not support this complete type.",{},true);
            node.kind="primitive";
            if(type.type.name=="bool"){node.type="frog.studio.boolean.false_constant";node.text="false";}
            else if(type.type.name=="string"){node.type="frog.studio.string.constant";node.text.clear();}
            else{node.type="frog.studio.numeric.numeric_constant";node.text="0";}
        }
        const auto passages=projectSequenceFramePassagesForExecution(selected.document);
        if(!passages.error.empty())refuse("sequence.passage_provenance_invalid",uri,"/diagram",passages.error);
        selected.document.properties.arrayStorageMode=FrogArrayStorageMode::Readable;
        entry.source={std::move(source),typeDefinitions.resolution.schemaFingerprint};
        entry.memberCalls=std::move(memberCalls);
        entry.active=std::move(selected.document);entry.raw=std::move(*parsed);entry.identity=key;
        auto [it,inserted]=frozen.emplace(key,std::move(entry));
        if(const auto *diagram=it->second.raw.member("diagram"))indexSource(*diagram,"/diagram",it->second.index);
        for(const auto &[id,path]:originalIndex.nodes)if(it->second.index.nodes.contains(id))it->second.index.nodes[id]=path;
        for(const auto &[id,origin]:classOrigins)if(const auto found=originalIndex.nodes.find(origin);found!=originalIndex.nodes.end())
            it->second.index.nodes[id]=found->second;
        return it->second;
    }
    Frozen &load(const std::string &key) {return frozen.at(key);}
    DraftDependencyResult fetch(std::string_view reference,std::string_view owner,
        const std::string &uri,const std::string &at,const std::string &callsite={}) {
        if(reference.empty()||reference.size()>8192||reference.find('\0')!=reference.npos)
            refuse("subfrog.dependency.path_invalid",uri,at,"The authored dependency reference is empty, overlong or contains NUL.",callsite,false,DocumentDiagnosticDomain::Dependency);
        if(classes)for(const auto &member:classes->methods)if(member.source.canonicalUri==reference)return member.source;
        auto result=resolveDraftDependency({DraftDependencyKind::Source,std::string(owner),std::string(reference),at,{}, {}, {}},input.resolver);
        if(result.state!=DraftDependencyState::Present||!result.bytes) {
            const bool control=result.state==DraftDependencyState::Cancelled||result.state==DraftDependencyState::LimitExceeded;
            refuse(result.state==DraftDependencyState::Cancelled?"subfrog.control.cancelled":
                result.state==DraftDependencyState::LimitExceeded?"subfrog.dependency.limit":
                result.diagnosticCode.empty()?"subfrog.source.unavailable":result.diagnosticCode,
                uri,at,result.message.empty()?"The source reference cannot be resolved by its host.":result.message,
                callsite,false,control?DocumentDiagnosticDomain::Control:DocumentDiagnosticDomain::Dependency);
        }
        return result;
    }
    std::string resolve(std::string_view reference,std::string_view owner,
        const std::string &uri,const std::string &at,const std::string &callsite={}) {
        auto result=fetch(reference,owner,uri,at,callsite);
        const auto key=result.canonicalUri;
        try {retain(std::move(result));}
        catch(Refusal &failure) {failure.diagnostic.callsiteId=callsite;throw;}
        return key;
    }
    std::string at(const Frozen &source,const DiagramNode &node) const {
        const auto found=source.index.nodes.find(node.id);return found==source.index.nodes.end()?"/diagram":found->second;
    }
    void callPayload(const Frozen &source,const DiagramNode &call,const std::string &callsite) {
        const auto atNode=at(source,call);const auto found=source.index.rawNodes.find(call.id);
        if(found==source.index.rawNodes.end())refuse("subfrog.interface.invalid",source.source.source.canonicalUri,atNode,"The active call has no exact authored payload.",callsite);
        const auto &node=*found->second;
        const auto *declaredType=node.member("type");
        if(text(node.member("kind"))!="frog_call"||
            (declaredType&&(declaredType->type!=Json::Type::String||declaredType->string!="frog.studio.frog_call")))
            refuse("subfrog.interface.invalid",source.source.source.canonicalUri,atNode,"The authored call owner differs from its actual model.",callsite);
        const auto &payload=object(node.member("call"),source.source.source.canonicalUri,atNode+"/call",callsite);
        fields(payload,{"document_path","document_title","icon_svg","ports","polymorphic"},source.source.source.canonicalUri,atNode+"/call",callsite);
        if ((payload.member("polymorphic") != nullptr) != call.polymorphicFrogSelection.has_value())
            refuse("polymorphic.selection.invalid",source.source.source.canonicalUri,atNode+"/call/polymorphic","The authored polymorphic selection differs from the call.",callsite);
        if(text(payload.member("document_path"))!=call.callDocumentPath)
            refuse("subfrog.interface.stale",source.source.source.canonicalUri,atNode+"/call/document_path","The current call path differs from its exact source payload.",callsite);
        const auto *ports=payload.member("ports");
        if(!ports||ports->type!=Json::Type::Array||ports->array.size()>256||ports->array.size()!=call.callPorts.size())
            refuse("subfrog.interface.invalid",source.source.source.canonicalUri,atNode+"/call/ports","A call requires its complete bounded port declaration without skipped entries.",callsite);
        std::set<std::string> ids;
        for(std::size_t i=0;i<ports->array.size();++i) {
            const auto p=atNode+"/call/ports/"+std::to_string(i);const auto &raw=ports->array[i];const auto &port=call.callPorts[i];
            fields(raw,{"id","source_port_id","interface_map_slot","label","value_type","direction","connection_requirement","normalized_x","normalized_y"},source.source.source.canonicalUri,p,callsite);
            const auto direction=text(raw.member("direction")),policy=text(raw.member("connection_requirement"));
            if(port.id.empty()||!ids.insert(port.id).second||text(raw.member("id"))!=port.id||text(raw.member("source_port_id"))!=port.sourcePortId||
                text(raw.member("value_type"))!=port.valueType||(direction!="input"&&direction!="output")||(direction=="input")!=port.input||
                (policy!="required"&&policy!="recommended"&&policy!="optional")||policy!=port.connectionRequirement)
                refuse("subfrog.interface.invalid",source.source.source.canonicalUri,p,"A call port is malformed, duplicated, normalized or inconsistent with its source identity.",callsite);
            if (call.polymorphicFrogSelection &&
                (port.id!=port.interfaceMapSlotId || text(raw.member("interface_map_slot"))!=port.id))
                refuse("polymorphic.interface.slot_invalid",source.source.source.canonicalUri,p,"A polymorphic port must identify its connector slot.",callsite);
        }
    }

    std::string resolveFamilyCall(DiagramNode &call, const FrogDraftData &document,
        const Frozen &caller, const std::string &pointer, const std::vector<std::string> &ancestry) {
        const auto &uri=caller.source.source.canonicalUri;
        const auto path=pointer+"/call/document_path";
        auto source=fetch(call.callDocumentPath,uri,uri,path,call.id);
        const auto familyUri=source.canonicalUri;
        if (const auto old=frozenFamilies.find(familyUri);old!=frozenFamilies.end()) {
            if (old->second.source.sha256!=source.sha256)
                refuse("subfrog.source.changed",familyUri,"/","The family changed during dependency capture.",call.id,false,DocumentDiagnosticDomain::Dependency);
        } else {
            if (frozenFamilies.size()+frozen.size()>=maximumPrograms || source.bytes->size()>maximumClosureBytes-bytes)
                refuse("subfrog.dependency.limit",familyUri,"/","The family exceeds the frozen dependency budget.",call.id);
            bytes+=source.bytes->size();
            frozenFamilies.emplace(familyUri,DraftSubFrogFrozenSource{source,{}});
        }
        std::string error;
        const auto family=readPolymorphicFrog(*source.bytes,&error);
        if (!family)
            refuse("polymorphic.family.invalid",familyUri,"/",error,call.id,false,DocumentDiagnosticDomain::Dependency);
        for (const auto &issue:validatePolymorphicFrog(*family))
            refuse(issue.code,familyUri,"/members",issue.message,call.id);

        std::set<std::string> uniqueMembers;
        std::vector<std::string> memberUris;
        std::vector<std::optional<PolymorphicFrogInterface>> signatures;
        for (std::size_t i=0;i<family->members.size();++i) {
            const auto &member=family->members[i];
            const auto memberPath="/members/"+std::to_string(i)+"/document_path";
            auto memberSource=fetch(member.documentPath,familyUri,familyUri,memberPath,call.id);
            const auto &memberUri=memberSource.canonicalUri;
            if (memberUri==familyUri || std::find(ancestry.begin(),ancestry.end(),memberUri)!=ancestry.end())
                refuse("polymorphic.member.cycle",familyUri,memberPath,"A member recursively references this family or caller.",call.id,false,DocumentDiagnosticDomain::Dependency);
            if (!uniqueMembers.insert(memberUri).second)
                refuse("polymorphic.member.duplicate",familyUri,memberPath,"Two entries resolve to the same member document.",call.id);
            if (memberSource.bytes->find(kPolymorphicFrogFormat)!=std::string::npos && isPolymorphicFrog(*memberSource.bytes))
                refuse("polymorphic.member.family",familyUri,memberPath,"Only ordinary FROG documents can be instances.",call.id);
            memberUris.push_back(memberUri);
            signatures.push_back(inspectPolymorphicFrogInterface(retain(std::move(memberSource)).active));
        }

        binding::TypeResolver resolver(document.diagramNodes,document.diagramEdges,document.widgets);
        std::map<std::string,binding::TypeRef> inputs;
        std::vector<std::string> wiredSlots;
        for (const auto &edge:document.diagramEdges) {
            if (edge.from.nodeId==call.id) wiredSlots.push_back(edge.from.portId);
            if (edge.to.nodeId!=call.id) continue;
            wiredSlots.push_back(edge.to.portId);
            const auto actual=resolver.resolveSource(edge);
            auto type=actual.resolution.accepted() && actual.complete ? actual.resolution.type : binding::TypeRef{};
            if (!inputs.emplace(edge.to.portId,std::move(type)).second)
                refuse("subfrog.interface.invalid",uri,pointer+"/call/ports","An input has multiple actual producers.",call.id);
        }
        const auto resolution=resolvePolymorphicFrog(*family,*call.polymorphicFrogSelection,signatures,inputs,wiredSlots);
        if (!resolution.issues.empty()) {
            const auto &issue=resolution.issues.front();
            const auto member=std::find_if(family->members.begin(),family->members.end(),
                [&](const auto &entry){return entry.id==issue.memberId;});
            const auto issuePath=member==family->members.end()?pointer+"/call/polymorphic":
                "/members/"+std::to_string(member-family->members.begin());
            refuse(issue.code,member==family->members.end()?uri:familyUri,issuePath,issue.message,call.id);
        }
        const auto index=*resolution.memberIndex;
        const auto &signature=*signatures[index];
        std::vector<DiagramCallPort> ports;
        for (const auto &[slot,port]:signature.slots) {
            const auto existing=std::find_if(call.callPorts.begin(),call.callPorts.end(),
                [&](const auto &entry){return entry.id==slot;});
            DiagramCallPort concrete;
            if (existing!=call.callPorts.end()) concrete=*existing;
            concrete.id=concrete.interfaceMapSlotId=slot;
            concrete.sourcePortId=port.publicId;
            concrete.label=port.label;
            concrete.valueType=port.type.canonical();
            concrete.input=port.input;
            concrete.connectionRequirement=port.requirement;
            ports.push_back(std::move(concrete));
        }
        call.callPorts=std::move(ports);
        return memberUris[index];
    }
    void budget(const FrogDraftData &doc,const Frozen &source,const std::string &callsite) {
        if(doc.diagramNodes.size()>maximumNodes||doc.diagramEdges.size()>maximumEdges||doc.widgets.objects().size()>maximumNodes||invocations>maximumPrograms)
            refuse("subfrog.dependency.limit",source.source.source.canonicalUri,"/diagram","Static call expansion exceeds the existing1024-node/4096-edge/1024-instance budgets.",callsite);
    }
    void rejectLocalOwner(const FrogDraftData &doc,const Frozen &source,const std::string &callsite) {
        if(doc.properties.executionPolicy!=FrogExecutionPolicy{}||doc.properties.executionPolicy!=input.rootDocument->properties.executionPolicy)
            refuse("subfrog.projection.execution_policy_unavailable",source.source.source.canonicalUri,"/execution_policy",
                "This callee execution policy needs an explicit invocation contract; it is not replaced by the parent policy.",callsite,true);
        for(const auto &node:doc.diagramNodes) {
            const auto &t=node.type;
            if(t=="frog.studio.structures.feedback_node"||t=="frog.studio.synchronization.first_call")
                refuse("subfrog.projection.state_unavailable",source.source.source.canonicalUri,at(source,node),
                    "Persistent callee state has no proven sharing/lifetime contract in this static specialization.",callsite,true);
            for(const auto &tunnel:node.caseStructureTunnels)if(!tunnel.shiftRegisterPairId.empty()&&tunnel.side=="left") {
                const auto inputPort=diagram_identity::tunnelInputId(tunnel.id);
                const bool initialized=std::any_of(doc.diagramEdges.begin(),doc.diagramEdges.end(),[&](const auto &edge) {
                    return edge.to.nodeId==node.id&&edge.to.portId==inputPort&&edge.diagramOwnerStructureId==node.diagramOwnerStructureId&&edge.diagramOwnerRegionId==node.diagramOwnerRegionId;
                });
                if(!initialized&&std::count_if(node.caseStructureTunnels.begin(),node.caseStructureTunnels.end(),
                    [&](const auto &face){return face.side=="left"&&face.shiftRegisterPairId==tunnel.shiftRegisterPairId;})!=1)
                    refuse("subfrog.projection.state_unavailable",source.source.source.canonicalUri,at(source,node),
                        "A shared multi-stage register needs atomic history transfer.",callsite,true);
            }
            // NativeCall keeps the authored node's source identity below.
            // Admission and path resolution still belong to the scalar host.
            // A wired Property Node obtains the target instance from its reference
            // operand, including after a call boundary or an indexing tunnel.
            // The normal required-input and type checks below validate that wire.
            // An implicit or explicitly named target still needs the callee's owner.
            const auto &control=node.applicationControl;
            const bool propertyViaReference=control&&control->kind==ApplicationControlKind::Property&&
                control->target.kind==ApplicationControlTargetKind::Unbound&&
                control->target.ownerDocumentId.empty()&&control->target.widgetId.empty();
            // A call shares the application instance. A child panel or widget still
            // requires its own document identity when projected into its caller.
            const bool sameApplication=control && control->expectedClass=="frog.application" &&
                control->target.kind==ApplicationControlTargetKind::ThisApplication &&
                control->target.ownerDocumentId.empty() && control->target.widgetId.empty() &&
                ((control->kind==ApplicationControlKind::Invoke && control->methodId=="application.quit") ||
                 (control->kind==ApplicationControlKind::Property && !control->properties.empty() &&
                  std::all_of(control->properties.begin(),control->properties.end(),[](const auto &row){
                      return row.memberId=="kind" && row.mode==ApplicationControlPropertyMode::Read;
                  })));
            if((control&&!propertyViaReference&&!sameApplication)||t=="frog.studio.structures.event_structure"||
                t=="frog.studio.error.build_cluster"||t=="frog.studio.error.translate_code"||t=="frog.studio.error.write_current_call_chain"||
                t=="frog.studio.path.current_frog_path"||t=="frog.studio.path.default_directory"||
                t=="frog.studio.path.temporary_directory"||t=="frog.studio.path.application_directory"||
                t=="frog.studio.path.get_component_directory"||t=="frog.studio.path.get_system_directory"||t=="frog.studio.path.generate_temporary_file")
                refuse("subfrog.projection.owner_context_unavailable",source.source.source.canonicalUri,at(source,node),
                    "This operation needs the callee's local document/effect authority, which is not replaced by the parent's owner.",callsite,true);
        }
    }
    std::vector<DiagramNode> constant(const WidgetInstance &widget,const std::string &id,const Rect &layout,
        const std::string &owner,const std::string &region,const Frozen &source,const std::string &pointer,const std::string &callsite) {
        DiagramNode node;node.id=id;node.kind="primitive";node.layout=layout;
        node.diagramOwnerStructureId=owner;node.diagramOwnerRegionId=region;
        std::vector<DiagramNode> nodes{std::move(node)};std::string error;
        if(!installTypeDefinitionConstantWidget(nodes,id,widget,&error))
            refuse("subfrog.projection.unsupported",source.source.source.canonicalUri,pointer,
                "The existing typed constant installer cannot freeze this exact current control value: "+error,callsite,true);
        return nodes;
    }

    Expanded expand(Frozen &source,const std::string &callPath,std::vector<std::string> ancestry,
        std::vector<std::string_view> callers={},unsigned depth=0) {
        if(depth>maximumDepth)refuse("subfrog.dependency.limit",source.source.source.canonicalUri,"/diagram","Call nesting exceeds32.");
        ancestry.push_back(source.identity);Expanded result;result.document=source.active;
        callers.push_back(source.active.title);
        for(const auto &node:result.document.diagramNodes) {
            const auto found=source.index.nodes.find(node.id);
            result.nodes.emplace(node.id,DraftSubFrogExecutionIdentity{node.id,source.source.source.canonicalUri,
                found==source.index.nodes.end()?"/diagram":found->second,{},callPath,found==source.index.nodes.end()});
        }
        for(const auto &edge:result.document.diagramEdges) {
            const auto found=source.index.edges.find(edge.id);
            result.edges.emplace(edge.id,DraftSubFrogExecutionIdentity{edge.id,source.source.source.canonicalUri,
                found==source.index.edges.end()?"/diagram":found->second,{},callPath,found==source.index.edges.end()});
        }
        for(const auto &node:source.active.diagramNodes) {
            if(node.type!=kLabViewInvalidMessageNodeType)continue;
            const auto authoredDriver=std::any_of(source.active.diagramEdges.begin(),source.active.diagramEdges.end(),[&](const auto &edge){
                return edge.to.nodeId==node.id&&edge.to.portId=="call_chain";
            });
            if(authoredDriver)refuse("labview_import.invalid_message_call_chain",source.source.source.canonicalUri,at(source,node),
                "The imported Error Ring obtains the call chain from the actual Run, not from an authored wire.");

            // Expand owns the real caller nesting. The compatibility input is
            // compiled into the same region, without appearing in the saved FROG.
            DiagramNode chain;
            chain.id=fresh(callPath,"labview_error_chain",node.id);
            chain.type="frog.studio.string.constant";
            chain.valueType="string";
            chain.labelVisible=false;
            chain.layout=node.layout;
            chain.diagramOwnerStructureId=node.diagramOwnerStructureId;
            chain.diagramOwnerRegionId=node.diagramOwnerRegionId;
            for(auto caller=callers.rbegin();caller!=callers.rend();++caller) {
                auto name=*caller;
                if(name.ends_with(".frog"))name.remove_suffix(5);
                if(name.empty()||name.find_first_of("\r\n")!=name.npos)
                    refuse("labview_import.invalid_message_call_chain",source.source.source.canonicalUri,at(source,node),
                        "The active call has no printable document name for the imported Error Ring.");
                if(!chain.text.empty())chain.text+='\n';
                chain.text+=name;
                if(!name.ends_with(".vi"))chain.text+=".vi";
            }
            DiagramEdge wire;
            wire.id=fresh(callPath,"labview_error_chain_wire",node.id);
            wire.from={chain.id,"value"};wire.to={node.id,"call_chain"};
            wire.diagramOwnerStructureId=node.diagramOwnerStructureId;
            wire.diagramOwnerRegionId=node.diagramOwnerRegionId;
            auto origin=result.nodes.at(node.id);origin.generated=true;
            origin.projectedId=chain.id;result.nodes.emplace(chain.id,origin);
            origin.projectedId=wire.id;result.edges.emplace(wire.id,std::move(origin));
            result.document.diagramNodes.push_back(std::move(chain));
            result.document.diagramEdges.push_back(std::move(wire));
            out.changed=true;
        }
        std::vector<std::string> calls;
        const bool hasFamilies=std::any_of(result.document.diagramNodes.begin(),result.document.diagramNodes.end(),
            [](const auto &node){return node.polymorphicFrogSelection.has_value();});
        if (hasFamilies || classes.has_value()) {
            // Resolve actual producer calls before their consumers, even when
            // their authoring order differs from their dataflow order. Class
            // values also require their real returned Record metadata here;
            // a serialized call-port type is not the complete producer.
            budget(result.document,source,{});
            std::map<std::string,std::vector<std::string_view>> upstream;
            std::map<std::string_view,const DiagramNode *> authoredNodes;
            for (const auto &edge:result.document.diagramEdges) upstream[edge.to.nodeId].push_back(edge.from.nodeId);
            for (const auto &node:source.active.diagramNodes) authoredNodes.emplace(node.id,&node);
            std::set<std::string_view> visited;
            const auto visit=[&](auto &self,std::string_view id) -> void {
                const auto node=authoredNodes.find(id);
                if (node==authoredNodes.end()) return;
                if (!visited.insert(id).second) return;
                if (const auto links=upstream.find(std::string(id));links!=upstream.end())
                    for (const auto producer:links->second) self(self,producer);
                if (node->second->kind=="frog_call") calls.emplace_back(id);
            };
            for (auto &node:result.document.diagramNodes) {
                if (node.kind=="frog_call") visit(visit,node.id);
                // Preserve slot identity while removing any saved type guess
                // from the evidence graph. A concrete projection installs it.
                if (node.polymorphicFrogSelection)
                    for (auto &port:node.callPorts) port.valueType="unknown";
            }
        } else {
            for (const auto &node:result.document.diagramNodes) if (node.kind=="frog_call") calls.push_back(node.id);
        }
        for(const auto &callId:calls) {
            const auto where=std::find_if(result.document.diagramNodes.begin(),result.document.diagramNodes.end(),[&](const auto &n){return n.id==callId;});
            if(where==result.document.diagramNodes.end())continue;
            auto call=*where;const auto pointer=at(source,call);auto site=callPath;frame(site,call.id);
            if (call.polymorphicFrogSelection) {
                const auto original=std::find_if(source.active.diagramNodes.begin(),source.active.diagramNodes.end(),
                    [&](const auto &node){return node.id==callId;});
                if (original!=source.active.diagramNodes.end()) call.callPorts=original->callPorts;
            }
            callPayload(source,call,callId);
            const auto resolution=call.polymorphicFrogSelection?
                resolveFamilyCall(call,result.document,source,pointer,ancestry):
                resolve(call.callDocumentPath,source.source.source.canonicalUri,source.source.source.canonicalUri,pointer+"/call/document_path",callId);
            auto &callee=load(resolution);
            const auto nominal=source.nominalCalls.find(call.id);
            if(nominal==source.nominalCalls.end() ? !callee.nominalPorts.empty() : nominal->second!=callee.nominalPorts)
                refuse("subfrog.interface.type_mismatch",source.source.source.canonicalUri,pointer+"/call/ports",
                    "The call and callee disagree on a nominal class within a Cluster.",callId);
            if(classes) {
                const auto member=classes->attributeMember(callee.source.source.canonicalUri,callee.source.source.sha256);
                const auto requested=source.memberCalls.find(call.id);
                if(member&&(requested==source.memberCalls.end()||requested->second!=*member))
                    refuse("class.member_call_required",source.source.source.canonicalUri,pointer+"/call",
                        "An actual member must be called through its class declaration and dispatch/access contract.",callId);
                if(requested!=source.memberCalls.end()&&(!member||requested->second!=*member))
                    refuse("class.member_identity",source.source.source.canonicalUri,pointer+"/call",
                        "The selected actual member differs from the frozen class target.",callId);
            }
            if(std::find(ancestry.begin(),ancestry.end(),callee.identity)!=ancestry.end())
                refuse("subfrog.dependency.cycle",source.source.source.canonicalUri,pointer+"/call/document_path","Recursive call dependencies require an explicit call-stack contract and are not statically expanded.",callId,true);
            if(++invocations>maximumPrograms)refuse("subfrog.dependency.limit",source.source.source.canonicalUri,pointer,"Static call-instance budget exceeded.",callId);
            rejectLocalOwner(callee.active,callee,callId);
            if(!callee.active.declaredInterface)
                refuse("subfrog.interface.invalid",source.source.source.canonicalUri,pointer+"/call/ports","The callee has no independently declared source interface; widget names or positions cannot create one.",callId,true);
            const auto &rawInterface=object(callee.raw.member("interface"),callee.source.source.canonicalUri,"/interface",callId);
            fields(rawInterface,{"inputs","outputs","map"},callee.source.source.canonicalUri,"/interface",callId);
            for(const auto side:{"inputs","outputs"})if(const auto *entries=rawInterface.member(side);entries&&entries->type==Json::Type::Array)
                for(std::size_t i=0;i<entries->array.size();++i)fields(entries->array[i],{"id","type","name","label","connection"},callee.source.source.canonicalUri,std::string("/interface/")+side+"/"+std::to_string(i),callId);
            const auto declaration=inspectFrogDraftInterface(*callee.active.declaredInterface);
            if(!declaration.inspectable||!declaration.contractValid)
                refuse("subfrog.interface.invalid",callee.source.source.canonicalUri,"/interface","The callee interface declaration is malformed or ambiguous.",callId);
            const auto bindingIssues=inspectFrogDraftInterfaceBindings(callee.active.declaredInterface,callee.active.widgets,
                callee.active.diagramNodes,callee.active.properties,input.compilationContext);
            if(!bindingIssues.empty())refuse("subfrog.interface.binding_missing",callee.source.source.canonicalUri,bindingIssues.front().subjectId,
                bindingIssues.front().message,callId,bindingIssues.front().domain==DocumentDiagnosticDomain::RuntimeTarget);
            if(call.callPorts.size()!=declaration.ports.size())
                refuse("subfrog.interface.stale",source.source.source.canonicalUri,pointer+"/call/ports","The call's port set is stale relative to the frozen callee declaration.",callId);
            struct Boundary {
                const FrogDraftDeclaredPort *declared{};const DiagramCallPort *call{};const WidgetInstance *widget{};
                binding::TypeRef type;std::string tunnel;std::vector<std::string> terminals;
            };
            std::vector<Boundary> boundaries;std::set<std::string> boundWidgets;
            for(const auto &declared:declaration.ports) {
                const auto port=std::find_if(call.callPorts.begin(),call.callPorts.end(),[&](const auto &p){return p.sourcePortId==declared.id;});
                if(port!=call.callPorts.end()&&port->valueType!=declared.parsedType.type.canonical())
                    refuse("subfrog.interface.type_mismatch",source.source.source.canonicalUri,pointer+"/call/ports","The call connector type differs from the frozen callee declaration.",callId);
                if(port==call.callPorts.end()||(!call.polymorphicFrogSelection&&port->id!=declared.id)||port->input!=declared.input||
                    (declared.input&&port->connectionRequirement!=declared.connection)||
                    std::count_if(call.callPorts.begin(),call.callPorts.end(),[&](const auto &p){return p.sourcePortId==declared.id;})!=1)
                    refuse("subfrog.interface.stale",source.source.source.canonicalUri,pointer+"/call/ports","A call connector contradicts the exact source ID, direction, type or input policy.",callId);
                Boundary b;b.declared=&declared;b.call=&*port;b.tunnel=fresh(site,"boundary",declared.id);
                for(const auto &object:callee.active.widgets.objects())if(object.widget) {
                    const auto &widget=*object.widget;const auto &id=declared.input?widget.publicInputId:widget.publicOutputId;
                    if(id&&*id==declared.id) {
                        if(b.widget)refuse("subfrog.interface.invalid",callee.source.source.canonicalUri,declared.sourcePath,"More than one widget claims the same declared interface port.",callId);
                        b.widget=&widget;
                    }
                }
                if(!b.widget)refuse("subfrog.interface.binding_missing",callee.source.source.canonicalUri,declared.sourcePath,"No explicit widget binding implements this declared port.",callId,true);
                const auto schema=binding::widgetType(*b.widget);
                if(!schema.valid()||!schema.type.known()||schema.type.canonical()!=declared.parsedType.type.canonical())
                    refuse("subfrog.interface.type_mismatch",callee.source.source.canonicalUri,declared.sourcePath,"The bound widget's complete type contradicts its declaration.",callId);
                b.type=schema.type;
                if(b.widget->interfaceMapSlotId&&port->interfaceMapSlotId!=*b.widget->interfaceMapSlotId)
                    refuse("subfrog.interface.stale",source.source.source.canonicalUri,pointer+"/call/ports","The call connector slot contradicts the explicitly bound widget slot.",callId);
                if(!boundWidgets.insert(b.widget->instanceId).second)
                    refuse("subfrog.interface.invalid",callee.source.source.canonicalUri,declared.sourcePath,"A widget cannot alias two independently declared call directions.",callId);
                for(const auto &node:callee.active.diagramNodes)if(node.kind=="widget_value"&&node.widgetId==b.widget->instanceId) {
                    if(!node.diagramOwnerStructureId.empty())
                        refuse("subfrog.interface.binding_missing",callee.source.source.canonicalUri,at(callee,node),"The current boundary contract requires an explicit callee-root terminal; it cannot flatten a nested region.",callId,true);
                    b.terminals.push_back(node.id);
                }
                if(b.terminals.empty())refuse("subfrog.interface.binding_missing",callee.source.source.canonicalUri,declared.sourcePath,"The declared widget has no active Diagram terminal.",callId,true);
                boundaries.push_back(std::move(b));
            }
            // The ordinary resolver proves actual parent producers before any
            // endpoint is rewritten. A call boundary never guesses a promotion.
            binding::TypeResolver resolver(result.document.diagramNodes,result.document.diagramEdges,result.document.widgets);
            std::vector<DiagramNode> defaults;std::vector<DiagramEdge> defaultEdges;
            std::map<std::string,std::string> boxesByDriver;
            for(const auto &b:boundaries)if(b.declared->input) {
                std::vector<const DiagramEdge*> drivers;
                for(const auto &edge:result.document.diagramEdges)if(edge.to.nodeId==callId&&edge.to.portId==b.call->id)drivers.push_back(&edge);
                if(drivers.size()>1)refuse("subfrog.interface.invalid",source.source.source.canonicalUri,pointer+"/call/ports","An input has multiple actual producers.",callId);
                if(drivers.empty()) {
                    if(callee.nominalPorts.contains(b.declared->id))
                        refuse("class.object_factory_required",source.source.source.canonicalUri,pointer+"/call/ports",
                            "A class-bearing Cluster requires a producer for its nominal value.",callId);
                    if(b.declared->connection=="required")
                        refuse("subfrog.interface.required_unwired",source.source.source.canonicalUri,pointer+"/call/ports","A required call input has no actual parent producer.",callId);
                    const auto id=fresh(site,"optional_value",b.declared->id);
                    auto nodes=constant(*b.widget,id,call.layout,call.diagramOwnerStructureId,call.diagramOwnerRegionId,callee,b.declared->sourcePath,callId);
                    defaults.insert(defaults.end(),std::make_move_iterator(nodes.begin()),std::make_move_iterator(nodes.end()));
                    DiagramEdge edge;edge.id=fresh(site,"optional_wire",b.declared->id);edge.from={id,"value"};edge.to={callId,diagram_identity::tunnelInputId(b.tunnel)};
                    edge.diagramOwnerStructureId=call.diagramOwnerStructureId;edge.diagramOwnerRegionId=call.diagramOwnerRegionId;defaultEdges.push_back(std::move(edge));
                } else {
                    const auto actual=resolver.resolveSource(*drivers.front());
                    const bool boxes=binding::requiresVariantBoxing(actual.resolution.type,b.type);
                    if(!actual.resolution.accepted()||!actual.complete||!actual.resolution.type.known()||
                        (!sameCallBoundaryType(actual.resolution.type,b.type)&&!boxes))
                        refuse("subfrog.interface.type_mismatch",source.source.source.canonicalUri,pointer+"/call/ports","Input "+b.declared->id+" expects "+b.type.canonical()+"; producer "+drivers.front()->from.nodeId+
                            "/"+drivers.front()->from.portId+" resolves to "+actual.resolution.type.canonical()+
                            (actual.complete?" (complete).":" (incomplete).")+" "+actual.resolution.code,callId);
                    if(boxes)boxesByDriver.emplace(drivers.front()->id,b.declared->id);
                }
            }
            Expanded body;
            // Validate the callee in its OWN identity/region namespace before
            // grafting it into the caller. An invalid local owner must never
            // accidentally resolve to a similarly named parent object.
            try {body=expand(callee,site,ancestry,callers,depth+1);}
            catch(Refusal &failure) {
                // Keep the callee URI/pointer in the diagnostic, but anchor
                // nested failures at this caller until the visible root call.
                failure.diagnostic.callsiteId=callId;throw;
            }
            const auto validity=inspectDraftSubFrogSemantics(body.document,input.compilationContext);
            const auto issue=std::find_if(validity.begin(),validity.end(),[](const auto &d){
                return d.severity==DocumentDiagnosticSeverity::Error&&(d.blocksBuild||d.blocksRun);
            });
            if(issue!=validity.end()) {
                auto issueUri=callee.source.source.canonicalUri;
                auto path=!issue->sourcePath.empty()?issue->sourcePath:
                    issue->subjectId.starts_with('/')?issue->subjectId:std::string("/diagram");
                if(const auto edge=body.edges.find(issue->edgeId);edge!=body.edges.end()) {
                    issueUri=edge->second.sourceUri;path=edge->second.sourcePointer;
                } else if(const auto node=body.nodes.find(issue->nodeId);node!=body.nodes.end()) {
                    issueUri=node->second.sourceUri;path=node->second.sourcePointer;
                }
                refuse(issue->code,std::move(issueUri),std::move(path),issue->message,callId,
                    issue->domain==DocumentDiagnosticDomain::FrontendCapability,issue->domain);
            }
            std::map<std::string,std::pair<const Boundary*,bool>> terminalPorts;
            for(const auto &b:boundaries)for(const auto &terminal:b.terminals)terminalPorts.emplace(terminal,std::pair{&b,b.declared->input});
            // Linked terminals become explicit boundary tokens. Other widgets
            // remain real instance-qualified UI bindings in the specialized
            // runtime snapshot; they are never painted in the parent canvas.
            std::set<std::string> usedWidgets;
            for(const auto &node:body.document.diagramNodes)if(node.kind=="widget_value"&&!terminalPorts.contains(node.id))usedWidgets.insert(node.widgetId);
            std::map<std::string,std::string> nodeIds,edgeIds,widgetIds;
            for(const auto &node:body.document.diagramNodes)if(!terminalPorts.contains(node.id))nodeIds.emplace(node.id,fresh(site,"node",node.id));
            for(const auto &edge:body.document.diagramEdges)edgeIds.emplace(edge.id,fresh(site,"edge",edge.id));
            for(const auto &id:usedWidgets)widgetIds.emplace(id,fresh(site,"widget",id));
            for(const auto &id:usedWidgets) {
                const auto *object=body.document.widgets.objectById(id);
                if(!object||!object->widget||object->widget->instanceId!=id)
                    refuse("subfrog.projection.unsupported",callee.source.source.canonicalUri,"/frontPanel/widgets","An internal widget needs an explicit root runtime binding; contained terminals are not guessed.",callId,true);
                if(object->widget->referenceValue)
                    refuse("subfrog.projection.owner_context_unavailable",callee.source.source.canonicalUri,"/frontPanel/widgets","A local Reference widget requires its original document authority.",callId,true);
                auto clone=*object;clone.id=widgetIds.at(id);clone.widget->instanceId=clone.id;
                const auto contained=[&](const auto &self,WidgetInstance &widget,std::string key,unsigned nesting)->void {
                    if(nesting>32)refuse("subfrog.dependency.limit",callee.source.source.canonicalUri,"/frontPanel/widgets","Contained widget depth exceeds32.",callId);
                    if(widget.clusterContainer)for(auto &field:widget.clusterContainer->elements)if(field.widget) {
                        field.widget=std::make_shared<WidgetInstance>(*field.widget);const auto childKey=key+"/field:"+field.fieldId;
                        field.widget->instanceId=fresh(site,"contained_widget",childKey);self(self,*field.widget,childKey,nesting+1);
                    }
                    if(widget.arrayContainer&&widget.arrayContainer->elementTemplate) {
                        auto &element=widget.arrayContainer->elementTemplate;element=std::make_shared<WidgetInstance>(*element);
                        element->instanceId=fresh(site,"contained_widget",key+"/element");self(self,*element,key+"/element",nesting+1);
                    }
                };
                contained(contained,*clone.widget,id,0);
                if(clone.widget->publicInputId||clone.widget->publicOutputId)
                    refuse("subfrog.interface.invalid",callee.source.source.canonicalUri,"/frontPanel/widgets","An internal clone cannot retain an unprojected public interface binding.",callId);
                if(!clone.parentTabId.empty()) {
                    const auto parent=widgetIds.find(clone.parentTabId);
                    if(parent==widgetIds.end())refuse("subfrog.projection.unsupported",callee.source.source.canonicalUri,"/frontPanel/widgets","The internal widget's actual Tab owner is outside the cloned runtime instance.",callId,true);
                    clone.parentTabId=parent->second;
                }
                result.document.widgets.addObject(std::move(clone),true);
            }
            std::set<std::string> outputWriters;
            const auto endpoint=[&](DiagramEdgeEndpoint value,bool producer) {
                if(const auto port=terminalPorts.find(value.nodeId);port!=terminalPorts.end()) {
                    if(value.portId!="value"||producer!=port->second.second)
                        refuse("subfrog.interface.invalid",callee.source.source.canonicalUri,"/diagram","An interface terminal is used in the wrong direction or through an undeclared port.",callId);
                    if(!producer)outputWriters.insert(port->second.first->declared->id);
                    return DiagramEdgeEndpoint{callId,producer?diagram_identity::tunnelOutputId(port->second.first->tunnel):diagram_identity::tunnelInputId(port->second.first->tunnel)};
                }
                const auto n=nodeIds.find(value.nodeId);
                if(n==nodeIds.end())refuse("subfrog.interface.invalid",callee.source.source.canonicalUri,"/diagram","An edge has no active instance-owned endpoint.",callId);
                value.nodeId=n->second;return value;
            };
            for(auto node:body.document.diagramNodes) {
                if(terminalPorts.contains(node.id)) {
                    auto origin=body.nodes.at(node.id);origin.projectedId=callId;origin.callsiteId=callId;origin.generated=true;result.retired.push_back(std::move(origin));continue;
                }
                const auto old=node.id;node.id=nodeIds.at(old);
                const auto remapNode=[&](std::string &id) {if(!id.empty()) {const auto mapped=nodeIds.find(id);if(mapped!=nodeIds.end())id=mapped->second;}};
                for(auto &face:node.caseStructureTunnels) {
                    using State=DiagramCaseStructureTunnel::InvocationState;
                    if(face.invocationState) {
                        auto state=*face.invocationState;
                        state.frame=nodeIds.at(state.frame);
                        face.invocationState=std::make_shared<const State>(std::move(state));
                    } else if(!face.shiftRegisterPairId.empty()&&face.side=="left"&&body.nodes.at(old).callsiteId.empty()) {
                        const auto port=diagram_identity::tunnelInputId(face.id);
                        const bool initialized=std::any_of(body.document.diagramEdges.begin(),body.document.diagramEdges.end(),
                            [&](const auto &e){return e.to.nodeId==old&&e.to.portId==port&&
                                e.diagramOwnerStructureId==node.diagramOwnerStructureId&&e.diagramOwnerRegionId==node.diagramOwnerRegionId;});
                        if(!initialized) {
                            const auto owner=hashing::sha256Hex(callee.source.source.canonicalUri);
                            std::string identity;frame(identity,owner);frame(identity,old);frame(identity,face.shiftRegisterPairId);
                            face.invocationState=std::make_shared<const State>(State{owner,hashing::sha256Hex(identity),callId});
                        }
                    }
                }
                if(node.diagramOwnerStructureId.empty()){node.diagramOwnerStructureId=callId;node.diagramOwnerRegionId="body";}
                else remapNode(node.diagramOwnerStructureId);
                remapNode(node.constantOwnerNodeId);remapNode(node.feedbackInitializerLoopId);
                if(!node.widgetId.empty())node.widgetId=widgetIds.at(node.widgetId);
                if(node.commentAttachment) {
                    auto &target=node.commentAttachment->targetId;
                    if(node.commentAttachment->kind==DiagramCommentTargetKind::Node)remapNode(target);
                    else if(const auto found=edgeIds.find(target);found!=edgeIds.end())target=found->second;
                }
                for(auto &tunnel:node.caseStructureTunnels)if(!tunnel.sequenceLegacyEdgeId.empty())
                    if(const auto found=edgeIds.find(tunnel.sequenceLegacyEdgeId);found!=edgeIds.end())tunnel.sequenceLegacyEdgeId=found->second;
                auto origin=body.nodes.at(old);origin.projectedId=node.id;origin.callsiteId=callId;
                result.nodes.emplace(node.id,std::move(origin));result.document.diagramNodes.push_back(std::move(node));
            }
            for(auto edge:body.document.diagramEdges) {
                const auto old=edge.id;edge.id=edgeIds.at(old);edge.from=endpoint(std::move(edge.from),true);edge.to=endpoint(std::move(edge.to),false);
                if(edge.diagramOwnerStructureId.empty()){edge.diagramOwnerStructureId=callId;edge.diagramOwnerRegionId="body";}
                else edge.diagramOwnerStructureId=nodeIds.at(edge.diagramOwnerStructureId);
                auto origin=body.edges.at(old);origin.projectedId=edge.id;origin.callsiteId=callId;
                result.edges.emplace(edge.id,std::move(origin));result.document.diagramEdges.push_back(std::move(edge));
            }
            for(const auto &b:boundaries)if(!b.declared->input&&!outputWriters.contains(b.declared->id))
                refuse("subfrog.interface.binding_missing",callee.source.source.canonicalUri,b.declared->sourcePath,"A call output requires a real body writer; an indicator's current/default value is not substituted.",callId,true);
            for(auto origin:body.retired) {
                if(const auto mapped=nodeIds.find(origin.projectedId);mapped!=nodeIds.end())origin.projectedId=mapped->second;
                origin.callsiteId=callId;result.retired.push_back(std::move(origin));
            }
            std::vector<DiagramEdge> boxedBoundaryEdges;
            for(auto &edge:result.document.diagramEdges) {
                if(edge.to.nodeId==callId)for(const auto &b:boundaries)if(b.declared->input&&edge.to.portId==b.call->id)edge.to.portId=diagram_identity::tunnelInputId(b.tunnel);
                if(edge.from.nodeId==callId)for(const auto &b:boundaries)if(!b.declared->input&&edge.from.portId==b.call->id)edge.from.portId=diagram_identity::tunnelOutputId(b.tunnel);
                const auto boxing=boxesByDriver.find(edge.id);
                if(boxing==boxesByDriver.end())continue;
                // A sequence input tunnel infers its type from its parent wire.
                // Box only the branch crossing this declared Variant boundary.
                DiagramNode box;box.id=fresh(site,"variant_input",boxing->second);
                box.kind="primitive";box.type="frog.studio.variant.to_variant";box.paletteCatalogId="variant.to_variant";
                box.layout=call.layout;box.diagramOwnerStructureId=edge.diagramOwnerStructureId;box.diagramOwnerRegionId=edge.diagramOwnerRegionId;
                DiagramEdge boxed;boxed.id=fresh(site,"variant_wire",boxing->second);
                boxed.from={box.id,"variant"};boxed.to=edge.to;edge.to={box.id,"data"};
                boxed.diagramOwnerStructureId=edge.diagramOwnerStructureId;boxed.diagramOwnerRegionId=edge.diagramOwnerRegionId;
                auto origin=result.edges.at(edge.id);
                origin.callsiteId=callId;origin.callPath=site;origin.generated=true;origin.projectedId=box.id;
                result.nodes.emplace(box.id,origin);
                origin.projectedId=boxed.id;result.edges.emplace(boxed.id,std::move(origin));
                result.document.diagramNodes.push_back(std::move(box));
                boxedBoundaryEdges.push_back(std::move(boxed));
            }
            result.document.diagramEdges.insert(result.document.diagramEdges.end(),
                std::make_move_iterator(boxedBoundaryEdges.begin()),std::make_move_iterator(boxedBoundaryEdges.end()));
            DiagramNode wrapper=call;wrapper.kind="primitive";wrapper.type=std::string(kSequenceStructureType);
            wrapper.callDocumentPath.clear();wrapper.callDocumentTitle.clear();wrapper.callIconSvgText.clear();wrapper.callPorts.clear();
            wrapper.polymorphicFrogSelection.reset();
            wrapper.polymorphicFrogSelectorCaption.clear();
            wrapper.polymorphicFrogSelectorShown=false;
            // The synthetic sequence has no authored documentation. The
            // original call retains its help; the execution node uses only
            // the selected member's concrete bindings.
            wrapper.documentationSummary.clear();
            wrapper.documentationUrl.clear();
            wrapper.instanceDescription.reset();
            wrapper.instanceTip.reset();
            wrapper.sequenceSemanticProfile="stacked_sequence";wrapper.sequenceFrames={{"body","",240.0,""}};wrapper.sequenceSelectedFrameId="body";wrapper.caseStructureTunnels.clear();
            for(const auto &b:boundaries) {
                DiagramCaseStructureTunnel tunnel;tunnel.id=b.tunnel;tunnel.valueType=b.type.canonical();tunnel.outputFromStructure=!b.declared->input;
                tunnel.side=b.declared->input?"left":"right";tunnel.sequenceFrameId="body";tunnel.useDefaultIfUnwired=false;
                wrapper.caseStructureTunnels.push_back(std::move(tunnel));
            }
            const auto replace=std::find_if(result.document.diagramNodes.begin(),result.document.diagramNodes.end(),[&](const auto &n){return n.id==callId;});
            *replace=std::move(wrapper);result.nodes.at(callId).callsiteId=callId;
            for(auto &node:defaults) {
                result.nodes.emplace(node.id,DraftSubFrogExecutionIdentity{node.id,callee.source.source.canonicalUri,"/frontPanel/widgets",callId,site,true});
                result.document.diagramNodes.push_back(std::move(node));
            }
            for(auto &edge:defaultEdges) {
                result.edges.emplace(edge.id,DraftSubFrogExecutionIdentity{edge.id,callee.source.source.canonicalUri,"/interface/inputs",callId,site,true});
                result.document.diagramEdges.push_back(std::move(edge));
            }
            out.changed=true;budget(result.document,source,callId);
        }
        return result;
    }
};
} // namespace

std::optional<std::string> prepareExecutionStorageSource(std::string_view bytes,std::string *error) {
    const auto fail=[&](std::string reason)->std::optional<std::string>{if(error)*error=std::move(reason);return {};};
    if(error)error->clear();
    // ArrayStorage reads bounded metadata through JsonValue::number. Match the
    // document reader's policy; its number lexemes remain intact for encoding
    // and exact integer/value owners, including integers above 2^53.
    std::string reason;auto root=readFrogDraftJson(bytes,&reason);
    if(!root||root->type!=Json::Type::Object)return fail(reason.empty()?"A source object is required.":reason);
    if(root->member("spec_version")||text(root->member("format"))!="frog.document.draft")return std::string(bytes);
    const auto *revision=root->member("draft_revision");unsigned rev=0;
    if(!revision||revision->type!=Json::Type::Number)return std::string(bytes);
    const auto number=std::from_chars(revision->string.data(),revision->string.data()+revision->string.size(),rev);
    if(number.ec!=std::errc{}||number.ptr!=revision->string.data()+revision->string.size())return std::string(bytes);
    const auto *storage=root->member("storage");
    // Ordinary readable revisions need no JSON re-encoding. The common
    // document reader still rejects compact payloads under revision2.
    if(!storage&&rev<kFrogArrayStorageDraftRevision)return std::string(bytes);
    if(storage) {
        if(rev<kFrogArrayStorageDraftRevision)return fail("FROG storage requires an explicitly identified revision3 or later writer.");
        if(storage->type!=Json::Type::Object)return fail("The storage options must be an object.");
        for(const auto &[key,_]:storage->object)if(key!="version"&&key!="array_mode")return fail("Unknown storage option: "+key+".");
        const auto *version=storage->member("version"),*mode=storage->member("array_mode");
        if(!version||version->type!=Json::Type::Number||version->string!="1"||!mode||mode->type!=Json::Type::String)
            return fail("Storage requires exact version1 and an explicit array_mode string.");
    }
    const bool mayPreserveBytes=!storage&&rev!=kFrogArrayStorageDraftRevision;
    const auto canonicalBefore=mayPreserveBytes?cj::encode(*root):std::string{};
    FrogArrayStorageMode mode=FrogArrayStorageMode::Readable;
    bool keptPackedArrays=false;
    if(!document_internal::readArrayStorageMode(*root,mode,reason)||
       !document_internal::inflateExecutionArrayStorage(*root,reason,rev>=kFrogArrayStorageDraftRevision,keptPackedArrays))return fail(reason);
    if(mayPreserveBytes&&cj::encode(*root)==canonicalBefore)return std::string(bytes);
    root->object.erase("storage");
    if(rev==kFrogArrayStorageDraftRevision&&!keptPackedArrays) {auto &next=root->object.at("draft_revision");next.string="2";}
    return cj::encode(*root);
}

DraftSubFrogExecutionProjection projectDraftSubFrogExecution(const DraftSubFrogExecutionInput &input) {
    return projectPreparedSubFrogExecution(input,nullptr);
}

DraftSubFrogExecutionProjection projectPreparedSubFrogExecution(const DraftSubFrogExecutionInput &input,const Json *normalizedRoot) {
    DraftSubFrogExecutionProjection out;
    try {
        if(!input.rootDocument||input.rootDocument->publicSourceAuthority||input.sourceUtf8.empty()||input.sourceUri.empty())
            refuse("subfrog.source.invalid",input.sourceUri,"/","Projection requires one immutable private root snapshot and its exact source bytes.");
        out.document=*input.rootDocument;
        // An unchanged root without calls needs no disk reference, closure or
        // extra dialect admission. Ordinary preparation remains its owner.
        if(!draftHasClasses(out.document)&&std::none_of(out.document.diagramNodes.begin(),out.document.diagramNodes.end(),[](const auto &n){
            return n.kind=="frog_call"||n.type==kLabViewInvalidMessageNodeType;
        }))return out;
        Projection projection{input,out,normalizedRoot};
        if(draftHasClasses(out.document)) {
            projection.classes=captureDraftClassProgram(out.document,input.sourceUri,input.resolver);
            if(!projection.classes->accepted()) {
                const auto &issues=projection.classes->classes.issues.empty()?projection.classes->issues:projection.classes->classes.issues;
                if(issues.empty())refuse("class.capture_incomplete",input.sourceUri,"/classes","The class/member capture did not establish a complete closure.");
                const auto &issue=issues.front();refuse(issue.code,issue.sourceUri,issue.pointer,issue.message);
            }
        }
        projection.rootKey=input.sourceUri;
        DraftDependencyResult rootSource{DraftDependencyState::Present,input.sourceUri,{}, {},
            hashing::sha256Hex(input.sourceUtf8),std::make_shared<const std::string>(input.sourceUtf8),{}};
        auto &root=projection.retain(std::move(rootSource),true);
        for(const auto &node:root.active.diagramNodes)projection.reserved.insert(node.id);
        for(const auto &edge:root.active.diagramEdges)projection.reserved.insert(edge.id);
        for(const auto &object:root.active.widgets.objects())projection.reserved.insert(object.id);
        // The same bounded expansion discovers each active authored call and
        // freezes its exact source once. Filesystem root/permission policy is
        // enforced by the supplied resolver, never inferred in this core.
        auto expanded=projection.expand(root,{},{});
        out.document=std::move(expanded.document);projection.budget(out.document,root,{});
        for(auto &[_,origin]:expanded.nodes)out.identities.push_back(std::move(origin));
        for(auto &[_,origin]:expanded.edges)out.identities.push_back(std::move(origin));
        out.identities.insert(out.identities.end(),std::make_move_iterator(expanded.retired.begin()),std::make_move_iterator(expanded.retired.end()));
        std::string digest;frame(digest,"frog.studio.subfrog.static-sequence@1");frame(digest,input.sourceUri);
        for(const auto &[key,entry]:projection.frozen) {
            out.frozenSources.push_back(entry.source);frame(digest,key);frame(digest,entry.source.source.sha256);frame(digest,entry.source.source.identity);frame(digest,entry.source.source.version);frame(digest,entry.source.typeDefinitionSchemaFingerprint);

        }
        for(const auto &[key,entry]:projection.frozenTypes) {
            out.frozenSources.push_back(entry);frame(digest,key);frame(digest,entry.source.sha256);frame(digest,entry.source.identity);frame(digest,entry.source.version);frame(digest,entry.typeDefinitionSchemaFingerprint);
        }
        for(const auto &[key,entry]:projection.frozenFamilies) {
            out.frozenSources.push_back(entry);frame(digest,key);frame(digest,entry.source.sha256);frame(digest,entry.source.identity);frame(digest,entry.source.version);
        }
        if(projection.classes) {
            std::map<std::string,DraftDependencyResult,std::less<>> closure;
            for(const auto &entry:projection.classes->classes.definitions)closure.emplace(entry.canonicalUri,entry);
            for(const auto &entry:projection.classes->methods)closure.emplace(entry.source.canonicalUri,entry.source);
            for(const auto &[key,source]:closure) {
                frame(digest,"class-member-value@1");frame(digest,key);frame(digest,source.sha256);frame(digest,source.identity);frame(digest,source.version);
                if(!projection.frozen.contains(key))out.frozenSources.push_back({source,{}});
            }
            std::map<std::string,std::string,std::less<>> unique;
            std::size_t total=0;for(const auto &entry:out.frozenSources) {
                const auto &[at,inserted]=unique.emplace(entry.source.canonicalUri,entry.source.sha256);
                if(!inserted&&at->second!=entry.source.sha256)refuse("class.dependency_changed",entry.source.canonicalUri,"/","A class source changed during projection.");
                if(inserted) {if(entry.source.bytes->size()>maximumClosureBytes-total)refuse("class.member_limit",entry.source.canonicalUri,"/","The complete frozen program exceeds64MiB.");total+=entry.source.bytes->size();}
            }
        }
        for(const auto &identity:out.identities) {
            frame(digest,identity.projectedId);frame(digest,identity.sourceUri);frame(digest,identity.sourcePointer);frame(digest,identity.callPath);
        }
        out.dependencyFingerprint=hashing::sha256Hex(digest);
    } catch(const Refusal &refusal) {out.diagnostics.push_back(refusal.diagnostic);}
    catch(const std::bad_alloc &) {out.diagnostics.push_back({"subfrog.dependency.limit",input.sourceUri,"/diagram","Not enough memory to prepare the bounded call projection.",{},true,DocumentDiagnosticDomain::Control});}
    if(!out.accepted()) {out.changed=false;if(input.rootDocument)out.document=*input.rootDocument;out.dependencyFingerprint.clear();}
    return out;
}
} // namespace frog::native_engine
