#include "frog_native_engine/DraftTypedConnections.hpp"
#include "FrogDraftJson.hpp"
#include "frog_native_engine/DraftFrontendPreparation.hpp"
#include "frog_native_engine/DraftSemanticPreparation.hpp"
#include "frog_native_engine/DraftRequiredInputs.hpp"
#include "frog_native_engine/DraftConditionalDisableSpecialization.hpp"
#include "frog_native_engine/SequenceFramePassageProjection.hpp"
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/Sha256.hpp"
#include <charconv>
#include "CanonicalComposedDraftMigration.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <set>
#include <iterator>

namespace frog::native_engine {
namespace {
using Json=source::JsonValue;
std::string identityPath(const Json &root,std::string_view id) {
    std::vector<std::pair<const Json*,std::string>> pending{{&root,{}}};std::string found;bool seen=false;
    while(!pending.empty()) {
        auto [node,path]=std::move(pending.back());pending.pop_back();
        if(node->type==Json::Type::Object) {
            if(const auto *value=node->member("id");value&&value->type==Json::Type::String&&value->string==id) {
                if(seen)return {};seen=true;found=path;
            }
            for(const auto &[key,value]:node->object)pending.emplace_back(&value,canonical_json::pointer(path,key));
        } else if(node->type==Json::Type::Array)
            for(std::size_t i=0;i<node->array.size();++i)pending.emplace_back(&node->array[i],canonical_json::pointer(path,std::to_string(i)));
    }
    return found;
}
std::string authoredPath(std::string_view path,const Json &projected,const Json &original) {
    if(path.empty()||!path.starts_with('/'))return std::string(path);
    std::vector<std::pair<const Json*,std::string>> work{{&projected,{}}};
    std::string mapped;std::size_t prefix=0;
    while(!work.empty()) {
        auto [node,at]=std::move(work.back());work.pop_back();
        if(node->type==Json::Type::Object) {
            if((path==at||path.starts_with(at+"/"))&&at.size()>prefix)
                if(const auto *id=node->member("id");id&&id->type==Json::Type::String)
                    if(const auto origin=identityPath(original,id->string);!origin.empty()) {
                        mapped=origin+std::string(path.substr(at.size()));prefix=at.size();
                    }
            for(const auto &[key,child]:node->object)if(const auto next=canonical_json::pointer(at,key);path==next||path.starts_with(next+"/"))
                work.emplace_back(&child,next);
        } else if(node->type==Json::Type::Array)for(std::size_t i=0;i<node->array.size();++i) {
            const auto next=canonical_json::pointer(at,std::to_string(i));
            if(path==next||path.starts_with(next+"/"))work.emplace_back(&node->array[i],next);
        }
    }
    // Root object fields such as icon/metadata keep their location when their
    // exact value is unchanged. Array-index paths always require an identity.
    if(mapped.empty()) {
        const Json *left=&projected,*right=&original;
        for(std::size_t first=1;left&&right&&first<=path.size();) {
            const auto slash=path.find('/',first),end=slash==path.npos?path.size():slash;
            std::string token;for(std::size_t i=first;i<end;++i) {
                if(path[i]=='~'&&i+1<end&&(path[i+1]=='0'||path[i+1]=='1'))token+=path[++i]=='0'?'~':'/';
                else token+=path[i];
            }
            if(left->type!=Json::Type::Object||right->type!=Json::Type::Object)break;
            left=left->member(token);right=right->member(token);
            if(slash==path.npos) {
                if(left&&right&&canonical_json::exact(*left,*right))return std::string(path);
                break;
            }
            first=slash+1;
        }
    }
    // Unknown identity never becomes a coincidental array index in the
    // original tree after a conditional/call projection changed its order.
    return mapped;
}
}
void attributeDraftSourceDiagnostic(DocumentDiagnostic &diagnostic,FrogDraftDataView document,const Json &root) {
    // A diagnostic subject is not automatically a JSON Pointer. Preserve the
    // established node:port convention using actual IDs (which may contain ':').
    std::string subject=diagnostic.subjectId;
    if(!diagnostic.sourcePath.empty()&&!diagnostic.sourcePath.starts_with('/')) {
        if(subject.empty())subject=diagnostic.sourcePath;
        diagnostic.sourcePath.clear();
    }
    if(diagnostic.subjectId.empty())diagnostic.subjectId=subject;
    const DiagramNode *node=nullptr;
    bool ambiguous=false;
    const auto choose=[&](const DiagramNode &candidate,bool exact) {
        if(exact) {
            if(node&&node->id==candidate.id)ambiguous=true;
            else if(!node||node->id.size()<candidate.id.size())node=&candidate;
        } else if(!node||candidate.id.size()>node->id.size()) {node=&candidate;ambiguous=false;}
        else if(candidate.id.size()==node->id.size())ambiguous=true;
    };
    const auto requestedNode=diagnostic.nodeId.empty()?std::string_view(subject):std::string_view(diagnostic.nodeId);
    for(const auto &candidate:document.diagramNodes)if(candidate.id==requestedNode)choose(candidate,true);
    if(!node&&!subject.empty())for(const auto &candidate:document.diagramNodes)
        if(!candidate.id.empty()&&subject.size()>candidate.id.size()+1&&subject.starts_with(candidate.id)&&subject[candidate.id.size()]==':')choose(candidate,false);
    if(node&&!ambiguous) {
        diagnostic.nodeId=node->id;diagnostic.subjectId=node->id;
        if(diagnostic.portId.empty()&&subject.size()>node->id.size()+1&&subject.starts_with(node->id)&&subject[node->id.size()]==':')
            diagnostic.portId=subject.substr(node->id.size()+1);
        diagnostic.ownerStructureId=node->diagramOwnerStructureId;diagnostic.regionId=node->diagramOwnerRegionId;
        if(!node->widgetId.empty())diagnostic.widgetId=node->widgetId;
        if(diagnostic.sourcePath.empty()) {
            diagnostic.sourcePath=identityPath(root,node->id);
            if(node->kind=="frog_call"&&!diagnostic.portId.empty()&&!diagnostic.sourcePath.empty()) {
                std::optional<std::size_t> slot;bool unique=true;
                for(std::size_t i=0;i<node->callPorts.size();++i)if(node->callPorts[i].id==diagnostic.portId) {
                    if(slot)unique=false;else slot=i;
                }
                if(slot&&unique)diagnostic.sourcePath+="/call/ports/"+std::to_string(*slot);
            }
        }
        if(diagnostic.edgeId.empty()&&!diagnostic.portId.empty()) {
            const DiagramEdge *driver=nullptr;bool unique=true;
            for(const auto &edge:document.diagramEdges)if(edge.to.nodeId==node->id&&edge.to.portId==diagnostic.portId) {
                if(driver)unique=false;else driver=&edge;
            }
            if(unique&&driver)diagnostic.edgeId=driver->id;
        }
    } else if(!subject.empty()&&diagnostic.sourcePath.empty())diagnostic.sourcePath=identityPath(root,subject);
    // Same explicit JSON-pointer walk as the historical preparation owner.
    // It records nested region ownership without using geometry or labels.
    const Json *value=&root;std::string container,owner;
    const auto path=std::string_view(diagnostic.sourcePath);
    if(path.empty()||path.front()!='/')return;
    for(std::size_t first=1;first<=path.size();) {
        const auto slash=path.find('/',first);const auto encoded=path.substr(first,slash==path.npos?path.size()-first:slash-first);
        std::string token;for(std::size_t i=0;i<encoded.size();++i) {
            if(encoded[i]=='~'&&i+1<encoded.size()&&(encoded[i+1]=='0'||encoded[i+1]=='1'))token+=encoded[++i]=='0'?'~':'/';
            else token+=encoded[i];
        }
        if(value->type==Json::Type::Object) {container=token;value=value->member(token);}
        else if(value->type==Json::Type::Array) {
            std::size_t index=0;const auto parsed=std::from_chars(token.data(),token.data()+token.size(),index);
            if(parsed.ec!=std::errc{}||parsed.ptr!=token.data()+token.size()||index>=value->array.size())return;
            value=&value->array[index];
            if(value->type==Json::Type::Object)if(const auto *id=value->member("id");id&&id->type==Json::Type::String) {
                if(container=="nodes") {owner=id->string;diagnostic.nodeId=owner;}
                else if(container=="edges"||container=="wire_fragments") {diagnostic.edgeId=id->string;diagnostic.nodeId.clear();}
                else if(container=="widgets")diagnostic.widgetId=id->string;
                else if(container=="regions") {diagnostic.ownerStructureId=owner;diagnostic.regionId=id->string;}
            }
        } else return;
        if(!value)return;
        if(slash==path.npos)break;first=slash+1;
    }
    if(diagnostic.subjectId.empty())diagnostic.subjectId=!diagnostic.edgeId.empty()?diagnostic.edgeId:
        !diagnostic.nodeId.empty()?diagnostic.nodeId:diagnostic.widgetId;
}
DraftFrontendPreparation prepareDraftFrontend(std::string_view bytes,std::string_view uri,
    const ConditionalDisableCompilationContext &context,const DraftDependencyResolver &resolver,
    const std::function<void()> &checkpoint) {
    DraftFrontendPreparation out;
    out.originalDraft=std::make_shared<const std::string>(bytes);
    out.originalSourceUri=uri;out.originalSourceSha256=hashing::sha256Hex(bytes);
    const auto poll=[&]{if(checkpoint)checkpoint();};
    const auto error=[&](std::string code,std::string message,std::string path,
        DocumentDiagnosticDomain domain=DocumentDiagnosticDomain::Semantics,
        DocumentDiagnosticStage stage=DocumentDiagnosticStage::Validation,bool blocking=true) {
        DocumentDiagnostic d{blocking?DocumentDiagnosticSeverity::Error:DocumentDiagnosticSeverity::Information,
            domain,std::move(code),{},std::move(message),blocking,blocking};
        d.stage=stage;d.sourceUri=uri;d.sourcePath=std::move(path);out.diagnostics.push_back(std::move(d));
    };
    poll();std::string why;
    // Syntax is a read failure, independent of the storage encoding. Preserve
    // the reader's exact diagnostic instead of reclassifying malformed JSON
    // as an invalid ArrayStorage envelope.
    auto raw=readFrogDraftJson(bytes,&why);
    if(!raw||raw->type!=Json::Type::Object) {
        error("preparation.json.invalid",why.empty()?"Source must be a JSON object.":why,{},
            DocumentDiagnosticDomain::Integrity,DocumentDiagnosticStage::Read);return out;
    }
    if(mixedFrogSourceAuthorities(*raw)) {
        error("preparation.authority.mixed","Public source and Studio draft envelopes cannot share one execution authority.",{},
            DocumentDiagnosticDomain::Compatibility,DocumentDiagnosticStage::Format);return out;
    }
    auto normalized=prepareExecutionStorageSource(bytes,&why);
    if(!normalized){error("preparation.storage.invalid",why,"/storage",DocumentDiagnosticDomain::Integrity,DocumentDiagnosticStage::Read);return out;}
    // Storage expansion may change revision 3 to readable revision 2. Retain
    // the original envelope by moving its tree; do not copy large literals.
    std::optional<Json> authoredEnvelope;
    if(*normalized!=bytes) {
        authoredEnvelope=std::move(raw);raw=readFrogDraftJson(*normalized,&why);
        if(!raw){error("preparation.storage.invalid",why,"/storage",DocumentDiagnosticDomain::Integrity,DocumentDiagnosticStage::Read);return out;}
    }
    const auto readFailure=[&](const FrogDocumentReadDiagnostic &diagnostic) {
        if(diagnostic.kind==FrogDocumentReadErrorKind::InvalidPresentation)
            error("migration.presentation_invalid",diagnostic.message,diagnostic.path,
                DocumentDiagnosticDomain::Semantics,DocumentDiagnosticStage::Validation);
        else error("preparation.source.unreadable",why,{},DocumentDiagnosticDomain::Integrity,DocumentDiagnosticStage::Read);
    };
    FrogDocumentReadDiagnostic readDiagnostic;
    auto document=readFrogDraftData(*normalized,&why,&readDiagnostic);
    if(!document){readFailure(readDiagnostic);return out;}
    const auto &revisionOwner=authoredEnvelope?*authoredEnvelope:*raw;
    if(const auto *revision=revisionOwner.member("draft_revision");revision&&revision->type==Json::Type::Number) {
        const auto result=std::from_chars(revision->string.data(),revision->string.data()+revision->string.size(),out.sourceRevision);
        if(result.ec!=std::errc{}||result.ptr!=revision->string.data()+revision->string.size())out.sourceRevision=0;
    }
    if(document->publicSourceAuthority||document->privateSourceAwaitingDiagramMigration) {
        error("private_source.diagram_migration_required","The retained format has no authored Diagram; an explicit document migration is required.","/diagram",DocumentDiagnosticDomain::Compatibility);return out;
    }
    poll();auto definitions=prepareDraftTypeDefinitionSource(*normalized,uri,resolver,false,false);
    if(!definitions.accepted()) {
        for(const auto &issue:definitions.resolution.diagnostics) {
            error(issue.code,issue.message,issue.fieldPath.empty()?identityPath(*raw,issue.instanceId):issue.fieldPath,DocumentDiagnosticDomain::Dependency,DocumentDiagnosticStage::Resolve);
            out.diagnostics.back().subjectId=issue.instanceId;
            attributeDraftSourceDiagnostic(out.diagnostics.back(),*document,*raw);
        }
        return out;
    }
    auto selected=specializeConditionalDisableData(*document,context);
    if(!selected.diagnostics.empty()) {
        for(const auto &issue:selected.diagnostics) {
            error(issue.code,issue.message,{},issue.incomplete?DocumentDiagnosticDomain::Control:DocumentDiagnosticDomain::Semantics);
            out.diagnostics.back().subjectId=issue.subjectId;
            attributeDraftSourceDiagnostic(out.diagnostics.back(),*document,*raw);
        }
        return out;
    }
    for(const auto &issue:validateCanonicalSubFrogProjectionSource(*normalized,selected.document,definitions.linked))if(issue.blocking) {
        error(issue.code,issue.message,issue.path,issue.domain,
            issue.domain==DocumentDiagnosticDomain::Semantics?DocumentDiagnosticStage::Validation:DocumentDiagnosticStage::Adaptation);
        attributeDraftSourceDiagnostic(out.diagnostics.back(),*document,*raw);
    }
    if(!out.diagnostics.empty())return out;
    out.typeDefinitionSchemaFingerprint=definitions.resolution.schemaFingerprint;
    if(definitions.linked) {
        readDiagnostic={};document=readFrogDraftData(definitions.sourceUtf8,&why,&readDiagnostic);
        if(!document){readFailure(readDiagnostic);return out;}
    }
    poll();const auto passages=projectSequenceFramePassagesForExecution(*document);
    if(!passages.error.empty()){error("sequence.passage_provenance_invalid",passages.error,"/diagram");return out;}
    std::set<std::string> generated;
    const bool conditional=std::any_of(document->diagramNodes.begin(),document->diagramNodes.end(),[](const auto &node){return
        node.type=="frog.studio.structures.disabled_structure"||node.type=="frog.studio.structures.conditional_disable_structure";});
    if(conditional) {
        std::set<std::string> original;for(const auto &node:document->diagramNodes)original.insert(node.id);
        auto projected=specializeConditionalDisableData(*document,context);
        for(const auto &issue:projected.diagnostics) {
            error(issue.code,issue.message,{},issue.incomplete?DocumentDiagnosticDomain::Control:DocumentDiagnosticDomain::Semantics);
            out.diagnostics.back().subjectId=issue.subjectId;
            attributeDraftSourceDiagnostic(out.diagnostics.back(),*document,*raw);
        }
        if(!out.diagnostics.empty())return out;
        document=std::move(projected.document);
        for(auto &node:document->diagramNodes)if(!original.contains(node.id)) {
            generated.insert(node.id);if(node.kind!="conditional_default")continue;
            const auto type=binding::parseType(node.valueType);
            if(!type.valid()||type.type.kind!=binding::TypeKind::Primitive||
                (type.type.name!="bool"&&type.type.name!="string"&&!type.type.numeric())) {
                error("preparation.disabled.default_type_unavailable","The compile-time default has no admitted literal codec.","/diagram",DocumentDiagnosticDomain::FrontendCapability);return out;
            }
            node.kind="primitive";
            if(type.type.name=="bool"){node.type="frog.studio.boolean.false_constant";node.text="false";}
            else if(type.type.name=="string"){node.type="frog.studio.string.constant";node.text.clear();}
            else{node.type="frog.studio.numeric.numeric_constant";node.text="0";}
        }
    }
    auto connections=inspectDraftTypedConnections(*document);
    auto required=inspectDraftRequiredInputs(*document,&connections.connectedPorts);
    connections.diagnostics.insert(connections.diagnostics.end(),std::make_move_iterator(required.begin()),std::make_move_iterator(required.end()));
    for(auto &issue:connections.diagnostics) {
        issue.sourceUri=uri;attributeDraftSourceDiagnostic(issue,*document,*raw);
        out.diagnostics.push_back(std::move(issue));
    }
    if(!out.diagnostics.empty())return out;
    std::string projected=definitions.linked||passages.changed||conditional?serializeFrogDraftData(*document):*normalized;
    poll();
    if(std::any_of(document->diagramNodes.begin(),document->diagramNodes.end(),[](const auto &node){return node.kind=="frog_call";})) {
        auto calls=projectDraftSubFrogExecution({std::make_shared<const FrogDraftData>(*document),projected,std::string(uri),resolver,context,true});
        for(const auto &issue:calls.diagnostics){error(issue.code,issue.message,issue.sourcePath,issue.domain);out.diagnostics.back().sourceUri=issue.sourceUri;out.diagnostics.back().subjectId=issue.callsiteId;out.diagnostics.back().callsiteId=issue.callsiteId;if(!issue.callsiteId.empty()){out.diagnostics.back().relatedSourceUri=uri;out.diagnostics.back().relatedSourcePath=identityPath(*raw,issue.callsiteId);}}
        if(!out.diagnostics.empty())return out;
        out.subprogramDependencyFingerprint=std::move(calls.dependencyFingerprint);out.dependencies=std::move(calls.frozenSources);
        for(auto &dependency:out.dependencies)if(dependency.source.canonicalUri==uri) {
            dependency.source.bytes=out.originalDraft;
            dependency.source.sha256=out.originalSourceSha256;
        }
        out.dependencyIdentities=std::move(calls.identities);document=std::move(calls.document);projected=serializeFrogDraftData(*document);
    }
    if(!generated.empty()) {
        auto tree=source::parseJson(projected,&why,source::JsonNumberPolicy::PreserveLexeme);
        const auto omit=[&](const auto &self,Json &node)->void {
            if(node.type==Json::Type::Object){if(const auto *id=node.member("id");id&&id->type==Json::Type::String&&generated.contains(id->string))node.object.erase("layout");for(auto &[_,child]:node.object)self(self,child);}
            else if(node.type==Json::Type::Array)for(auto &child:node.array)self(self,child);
        };
        if(tree){omit(omit,*tree);projected=canonical_json::encode(*tree);}
    }
    poll();auto migration=prepareDraftSemanticSource(*document,projected,uri);
    std::optional<Json> projectedTree;
    for(const auto &issue:migration.diagnostics) {
        std::string path=issue.path;
        if(path.starts_with('/')) {
            if(!projectedTree)projectedTree=source::parseJson(projected,nullptr,source::JsonNumberPolicy::PreserveLexeme);
            if(projectedTree)path=authoredPath(path,*projectedTree,*raw);
        }
        error(issue.code,issue.message,std::move(path),issue.domain,
            issue.domain==DocumentDiagnosticDomain::Semantics||issue.domain==DocumentDiagnosticDomain::Topology?
                DocumentDiagnosticStage::Validation:DocumentDiagnosticStage::Adaptation,issue.blocking);
        attributeDraftSourceDiagnostic(out.diagnostics.back(),*document,*raw);
    }
    if(!migration.accepted()) {
        if(out.diagnostics.empty())error("frontend.draft_validation_indeterminate",
            "Draft preparation did not produce a validated source or an attributable failure.","/diagram",DocumentDiagnosticDomain::Control);
        return out;
    }
    for(auto &mapping:migration.identities) {
        const auto dependency=std::find_if(out.dependencyIdentities.begin(),out.dependencyIdentities.end(),[&](const auto &entry){return entry.projectedId==mapping.id;});
        if(dependency!=out.dependencyIdentities.end()) {mapping.originalPointer=identityPath(*raw,dependency->callsiteId);mapping.generated=dependency->generated;}
        else if(generated.contains(mapping.id)||generated.contains(mapping.id.substr(0,mapping.id.find(':')))){mapping.generated=true;mapping.originalPointer="/diagram";}
        else if(const auto original=identityPath(*raw,mapping.id);!original.empty())mapping.originalPointer=original;
    }
    out.source=std::move(migration.publicSource);out.identities=std::move(migration.identities);out.controls=std::move(migration.controls);
    out.projectedDraft=std::move(projected);out.document=std::make_shared<const FrogDraftData>(std::move(*document));
    return out;
}
}
