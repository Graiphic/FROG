// SPDX-License-Identifier: Apache-2.0
// Request ownership/resolution boundary around the actual common producers.
// This file contains no type inference, interpreter, backend or filesystem I/O.
#include "FrontendOperation.hpp"
#include "FrogDraftJson.hpp"
#include "frog_native_engine/CanonicalExecutionIr.hpp"
#include "frog_native_engine/CanonicalEventExecutionIr.hpp"
#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "frog_native_engine/DraftFrontendPreparation.hpp"
#include "frog_native_engine/FrogDocumentFormat.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cstdint>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifndef FROG_FRONTEND_SOFTWARE_VERSION
#define FROG_FRONTEND_SOFTWARE_VERSION "0.2.0-local"
#endif
#ifndef FROG_FRONTEND_CONTENT_SHA256
#define FROG_FRONTEND_CONTENT_SHA256 ""
#endif
#ifndef FROG_FRONTEND_BASE_REVISION
#define FROG_FRONTEND_BASE_REVISION ""
#endif

namespace frog::frontend {
namespace {
namespace ne=frog::native_engine;
namespace cj=ne::canonical_json;
using Json=ne::source::JsonValue;
constexpr std::uint64_t mib=1024U*1024U;
constexpr std::uint64_t maximumResolutionSteps=4096;
constexpr std::string_view resolutionProfile="frog.frontend.resolution@1";
constexpr std::string_view draftResolutionProfile="frog.frontend.resolution@2";
constexpr std::string_view typeDefinitionsProfile="frog.frontend.type-definitions@1";

struct Failure {
    frog_frontend_outcome outcome;
    std::string code,message,stage,category,uri,pointer;
};
[[noreturn]] void fail(std::string code,std::string message,std::string stage,
    std::string category,std::string uri={},std::string pointer={},
    frog_frontend_outcome outcome=FROG_FRONTEND_REJECTED) {
    throw Failure{outcome,std::move(code),std::move(message),std::move(stage),
        std::move(category),std::move(uri),std::move(pointer)};
}
std::string_view span(frog_frontend_utf8 v) noexcept {return {v.data?v.data:"",v.size};}
frog_frontend_utf8 view(std::string_view v) noexcept {return {v.data(),v.size()};}
Json number(std::uint64_t value) {Json j;j.type=Json::Type::Number;j.string=std::to_string(value);return j;}
Json strings(std::initializer_list<std::string_view> values) {
    auto result=cj::array();for(auto v:values)result.array.push_back(cj::string(std::string(v)));return result;
}
std::uint64_t ceiling(std::uint64_t requested,std::uint64_t hard) noexcept {
    return requested?std::min(requested,hard):hard;
}
frog_frontend_limits normalized(const frog_frontend_limits &v) noexcept {
    // Producer ceilings are deliberately stricter than some API defaults.
    const ne::CanonicalIrLimits producer;
    return {ceiling(v.max_source_bytes,8*mib),ceiling(v.max_dependency_bytes,8*mib),
        ceiling(v.max_total_dependency_bytes,64*mib),ceiling(v.max_result_bytes,32*mib),
        ceiling(v.max_nodes,producer.maximumNodes),ceiling(v.max_connections,producer.maximumEdges),
        ceiling(v.max_nesting_depth,64),ceiling(v.max_dependencies,256),ceiling(v.max_diagnostics,256)};
}
Json limitsJson(const frog_frontend_limits &v) {
    auto out=cj::object();out.object={{"max_source_bytes",number(v.max_source_bytes)},
        {"max_dependency_bytes",number(v.max_dependency_bytes)},
        {"max_total_dependency_bytes",number(v.max_total_dependency_bytes)},
        {"max_result_bytes",number(v.max_result_bytes)},{"max_nodes",number(v.max_nodes)},
        {"max_connections",number(v.max_connections)},{"max_nesting_depth",number(v.max_nesting_depth)},
        {"max_dependencies",number(v.max_dependencies)},{"max_diagnostics",number(v.max_diagnostics)}};return out;
}
Json implementation() {
    auto out=cj::object();out.object={{"software_version",cj::string(FROG_FRONTEND_SOFTWARE_VERSION)},
        {"api_version",cj::string("3")},{"commit",cj::string(FROG_FRONTEND_BASE_REVISION)},
        {"sha256",cj::string(FROG_FRONTEND_CONTENT_SHA256)}};return out;
}
bool digest(std::string_view value) noexcept {
    return value.size()==64&&std::all_of(value.begin(),value.end(),[](char c){return (c>='0'&&c<='9')||(c>='a'&&c<='f');});
}
bool text(std::string_view value) noexcept {return value.size()<=8192&&value.find('\0')==value.npos&&validUtf8(value);}
void apiArguments(frog_frontend_operation operation,const frog_frontend_config &config,
    const frog_frontend_request *request) {
    // Both CABI and the pinned internal Studio adapter cross this same guard.
    if(config.struct_size!=sizeof(config)||config.reserved||
        (operation!=FROG_FRONTEND_DESCRIBE&&operation!=FROG_FRONTEND_VALIDATE&&operation!=FROG_FRONTEND_DERIVE))
        throw ApiFailure{FROG_FRONTEND_INVALID_ARGUMENT};
    if(operation==FROG_FRONTEND_DESCRIBE) {
        if(request)throw ApiFailure{FROG_FRONTEND_INVALID_ARGUMENT};
        return;
    }
    if(!request||request->struct_size!=sizeof(*request)||request->reserved||
        !request->source_bytes.data||!request->source_bytes.size||
        (!request->semantic_context_json.data&&request->semantic_context_json.size)||
        ((request->resolver.resolve==nullptr)!=(request->resolver.release==nullptr)))
        throw ApiFailure{FROG_FRONTEND_INVALID_ARGUMENT};
    for(const auto value:{request->source_uri,request->source_document_id,request->source_format_id,
        request->source_format_version,request->fir_format_version,request->fir_document_id})
        if(!value.data||!value.size||!text(span(value)))throw ApiFailure{FROG_FRONTEND_INVALID_ARGUMENT};
}
void fields(const Json &j,std::initializer_list<std::string_view> allowed,
    std::string_view uri,std::string_view pointer) {
    if(j.type!=Json::Type::Object)fail("frontend.object_required","An explicit JSON object is required.","resolve","dependency",std::string(uri),std::string(pointer));
    for(const auto &[key,value]:j.object) {
        (void)value;
        if(std::find(allowed.begin(),allowed.end(),key)==allowed.end())
            fail("frontend.context_field_unsupported","No semantic contract is declared for field: "+key,
                "resolve","frontend-capability",std::string(uri),cj::pointer(pointer,key),FROG_FRONTEND_UNSUPPORTED);
    }
}
std::string requiredText(const Json &j,std::string_view key,std::string_view uri,std::string_view pointer,bool empty=false) {
    const auto *v=j.member(key);
    if(!v||v->type!=Json::Type::String||(!empty&&v->string.empty())||!text(v->string))
        fail("frontend.identity_invalid","A bounded explicit UTF-8 string is required.","resolve","dependency",std::string(uri),cj::pointer(pointer,key));
    return v->string;
}
std::string token(std::string_view value) {
    std::string out;for(std::size_t i=0;i<value.size();++i) {
        if(value[i]!='~'){out+=value[i];continue;}
        if(++i==value.size()||(value[i]!='0'&&value[i]!='1'))throw std::invalid_argument("Invalid JSON Pointer escape.");
        out+=value[i]=='0'?'~':'/';
    }return out;
}
const Json *at(const Json &root,std::string_view pointer) {
    if(pointer.empty())return &root;if(pointer.front()!='/')return nullptr;
    const Json *v=&root;
    while(!pointer.empty()) {
        pointer.remove_prefix(1);const auto end=pointer.find('/');std::string key;
        try{key=token(pointer.substr(0,end));}catch(...){return nullptr;}
        if(v->type==Json::Type::Object)v=v->member(key);
        else if(v->type==Json::Type::Array) {
            std::size_t index=0;const auto result=std::from_chars(key.data(),key.data()+key.size(),index);
            if(key.empty()||(key.size()>1&&key.front()=='0')||result.ec!=std::errc{}||result.ptr!=key.data()+key.size()||index>=v->array.size())return nullptr;
            v=&v->array[index];
        }else return nullptr;
        if(!v)return nullptr;if(end==pointer.npos)return v;pointer.remove_prefix(end);
    }return v;
}
Json *at(Json &root,std::string_view pointer) {return const_cast<Json *>(at(static_cast<const Json &>(root),pointer));}
// Exact bounded size accounting uses the existing canonical JSON encoder;
// it prevents output allocation before the aggregate result budget is known.
bool add(std::uint64_t &total,std::uint64_t n,std::uint64_t maximum) noexcept {
    if(n>maximum||total>maximum-n)return false;total+=n;return true;
}
bool jsonSize(const Json &v,std::uint64_t &total,std::uint64_t maximum) {
    const auto quoted=[&](std::string_view s) {
        if(!add(total,2,maximum))return false;
        for(unsigned char c:s)if(!add(total,c<32?6:(c=='"'||c=='\\')?2:1,maximum))return false;
        return true;
    };
    switch(v.type) {
    case Json::Type::Null:return add(total,4,maximum);
    case Json::Type::Boolean:return add(total,v.boolean?4:5,maximum);
    case Json::Type::Number:return add(total,v.string.size(),maximum);
    case Json::Type::String:return quoted(v.string);
    case Json::Type::Array:
        if(!add(total,2,maximum))return false;
        for(std::size_t i=0;i<v.array.size();++i)if((i&&!add(total,1,maximum))||!jsonSize(v.array[i],total,maximum))return false;
        return true;
    case Json::Type::Object:
        if(!add(total,2,maximum))return false;
        {bool first=true;for(const auto &[key,value]:v.object) {
            if((!first&&!add(total,1,maximum))||!quoted(key)||!add(total,1,maximum)||!jsonSize(value,total,maximum))return false;first=false;
        }}return true;
    }return false;
}

struct Snapshot {
    std::uint32_t kind=2;
    std::string uri,identity,version,hash;
    std::shared_ptr<const std::string> bytes;
    Json document;
};
struct Release {
    const frog_frontend_resolver &resolver;
    const frog_frontend_resolved_dependency &value;
    bool active=true;
    ~Release(){if(active)try{resolver.release(resolver.user,&value);}catch(...){}}
    void finish() {active=false;resolver.release(resolver.user,&value);}
};
struct Request {
    Request(const frog_frontend_request *value,frog_frontend_limits ceilings)
        :request(value),limits(ceilings),firDocumentId(value?std::string(span(value->fir_document_id)):std::string{}) {}
    const frog_frontend_request *request;
    frog_frontend_limits limits;
    Json diagnostics=cj::array(),maps=cj::object(),provenance=cj::object();
    std::map<std::string,Snapshot> snapshots;
    std::map<std::string,std::string> referenceKeys,uriKeys;
    std::set<std::string> active;
    std::set<std::string> schemaRoots;
    std::map<std::string,std::string> objectPaths;
    std::vector<ne::CanonicalDraftMigrationIdentity> draftMappings;
    Json transformations=cj::array(),semanticContext=cj::object();
    Json selectedProfiles=cj::array(),realizedOperations=cj::array();
    std::string rootHash,projectedHash,firDocumentId,preparedUri;
    std::uint64_t dependencyBytes=0,nodeCount=0,connectionCount=0,steps=0;
    bool closureComplete=false,diagnosticsTruncated=false;

    std::string uri()const {return request?std::string(span(request->source_uri)):std::string{};}
    void poll()const {
        if(!request||!request->is_cancelled)return;
        std::uint32_t result;
        try{result=request->is_cancelled(request->cancel_user);}catch(...){fail("frontend.cancel_callback_failed","The cancellation callback threw an exception.","resolve","control",uri());}
        if(result==1)fail("frontend.cancelled","The request was cancelled.","resolve","control",uri(),{},FROG_FRONTEND_CANCELLED);
        if(result!=0)fail("frontend.cancel_callback_failed","The cancellation callback returned an unknown status.","resolve","control",uri());
    }
    void limit(std::string message,std::string_view origin,std::string_view pointer={})const {
        fail("frontend.limit_exceeded",std::move(message),"validate","control",std::string(origin),std::string(pointer),FROG_FRONTEND_LIMIT_EXCEEDED);
    }
    void diagnostic(const Failure &failure) {
        if(diagnosticsTruncated)return;
        if(diagnostics.array.size()>=limits.max_diagnostics) {
            diagnostics.array.back()=entry({FROG_FRONTEND_LIMIT_EXCEEDED,"frontend.diagnostic_limit","Additional diagnostics exceed the effective diagnostic budget.","validate","control",uri(),{}});
            diagnosticsTruncated=true;return;
        }
        diagnostics.array.push_back(entry(failure));
    }
    Json entry(const Failure &failure)const {
        const bool derived=failure.pointer=="/unit"||failure.pointer.starts_with("/unit/");
        auto source=cj::object();source.object={{"uri",cj::string(failure.uri.empty()?uri():failure.uri)},
            {"pointer",cj::string(derived?std::string{}:failure.pointer)},{"document_id",cj::string(request?std::string(span(request->source_document_id)):std::string{})}};
        auto out=cj::object();out.object={{"schema",cj::string("frog.diagnostic")},{"version",cj::string("0.2")},
            {"code",cj::string(failure.code)},{"stage",cj::string(failure.stage)},
            {"severity",cj::string("error")},{"category",cj::string(failure.category)},
            {"message",cj::string(failure.message)},{"source",std::move(source)}};
        if(failure.category=="frontend-capability")out.object["capability_id"]=cj::string(failure.code);
        if(derived)out.object["derived_pointer"]=cj::string(failure.pointer);
        return out;
    }
    void countSourceGraphs(const ne::FrogSourceDocument &source) {
        // Source owns the list of root/hidden graph scopes. A field merely
        // named nodes/edges inside metadata, a literal or a TypeDef is not one.
        std::set<std::string> counted;
        for(const auto &scope:source.diagramScopes()) {
            if(!counted.insert(scope.path).second)continue;
            const auto *nodes=scope.diagram->member("nodes"),*edges=scope.diagram->member("edges");
            if(nodes&&nodes->type==Json::Type::Array&&!add(nodeCount,nodes->array.size(),limits.max_nodes))
                limit("Declared Source graph nodes exceed the effective producer ceiling.",uri(),scope.path+"/nodes");
            if(edges&&edges->type==Json::Type::Array&&!add(connectionCount,edges->array.size(),limits.max_connections))
                limit("Declared Source graph connections exceed the effective producer ceiling.",uri(),scope.path+"/edges");
            poll();
        }
    }
    void walk(const Json &root,std::string_view origin,bool count=true) {
        struct Item{const Json *v;std::string path;std::uint64_t depth;};
        std::vector<Item> work{{&root,{},0}};
        while(!work.empty()) {
            auto item=std::move(work.back());work.pop_back();
            if(item.depth>limits.max_nesting_depth)limit("Semantic nesting exceeds its effective ceiling.",origin,item.path);
            if(item.v->type==Json::Type::Object)for(const auto &[key,value]:item.v->object) {
                const auto path=cj::pointer(item.path,key);
                if(count&&origin==uri()&&key=="id"&&value.type==Json::Type::String) {
                    const auto [position,inserted]=objectPaths.emplace(value.string,item.path);
                    if(!inserted&&position->second!=item.path)position->second.clear();
                }
                work.push_back({&value,path,item.depth+((key=="regions"||key=="region"||key=="children")?1U:0U)});
            }else if(item.v->type==Json::Type::Array)for(std::size_t i=0;i<item.v->array.size();++i)
                work.push_back({&item.v->array[i],cj::pointer(item.path,std::to_string(i)),item.depth});
            if((work.size()&127U)==0)poll();
        }
    }
    const Snapshot &captureDocument(std::uint32_t kind,std::string_view reference,
        std::string_view identity,std::string_view version,std::string_view expected,
        std::string_view ownerUri,std::string_view pointer) {
        if(!expected.empty()&&!digest(expected))fail("frontend.dependency_digest_invalid","An authored dependency digest must be 64 lowercase hexadecimal characters.","resolve","dependency",std::string(ownerUri),std::string(pointer));
        const auto key=std::to_string(kind)+"\n"+std::string(ownerUri)+"\n"+std::string(reference);
        const auto verify=[&](const Snapshot &s)->const Snapshot& {
            if((!identity.empty()&&s.identity!=identity)||(!version.empty()&&s.version!=version)||(!expected.empty()&&s.hash!=expected))
                fail("frontend.dependency_conflict","A reference contradicts its captured dependency identity/version/content.","resolve","dependency",std::string(ownerUri),std::string(pointer));
            return s;
        };
        if(const auto found=referenceKeys.find(key);found!=referenceKeys.end())return verify(snapshots.at(found->second));
        poll();
        if(!request->resolver.resolve)fail("frontend.dependency_resolver_required","This authored type dependency requires an explicit resolver.","resolve","dependency",std::string(ownerUri),std::string(pointer));
        frog_frontend_dependency_request wanted{};wanted.struct_size=sizeof(wanted);wanted.kind=kind;
        wanted.requesting_source_uri=view(ownerUri);wanted.source_pointer=view(pointer);wanted.reference=view(reference);
        wanted.expected_identity=view(identity);wanted.expected_version=view(version);wanted.expected_sha256=view(expected);
        frog_frontend_resolved_dependency resolved{};resolved.struct_size=sizeof(resolved);
        frog_frontend_resolve_status status;
        try{status=request->resolver.resolve(request->resolver.user,&wanted,&resolved);}catch(...){fail("frontend.resolve_callback_failed","The resolver threw instead of returning a declared status.","resolve","dependency",std::string(ownerUri),std::string(pointer));}
        Release release{request->resolver,resolved};
        const auto close=[&] {try{release.finish();}catch(...){fail("frontend.release_callback_failed","The release callback threw an exception.","resolve","dependency",std::string(ownerUri),std::string(pointer));}};
        if(status!=FROG_FRONTEND_RESOLVED) {
            std::string message="The explicit resolver refused the authored dependency.";
            if(resolved.error_message.data&&resolved.error_message.size<=8192&&text(span(resolved.error_message)))message+=" "+std::string(span(resolved.error_message));
            close();
            if(status==FROG_FRONTEND_RESOLVE_CANCELLED)fail("frontend.cancelled",message,"resolve","control",std::string(ownerUri),std::string(pointer),FROG_FRONTEND_CANCELLED);
            const auto code=status==FROG_FRONTEND_NOT_FOUND?"frontend.dependency_not_found":status==FROG_FRONTEND_ACCESS_DENIED?"frontend.dependency_access_denied":status==FROG_FRONTEND_RESOLVE_ERROR?"frontend.dependency_resolver_error":"frontend.dependency_status_invalid";
            fail(code,message,"resolve","dependency",std::string(ownerUri),std::string(pointer));
        }
        if(resolved.struct_size!=sizeof(resolved)||resolved.reserved||!resolved.source_uri.data||!resolved.identity.data||!resolved.version.data||
            !resolved.bytes.data||resolved.source_uri.size==0||resolved.identity.size==0||resolved.version.size==0||
            !text(span(resolved.source_uri))||!text(span(resolved.identity))||!text(span(resolved.version))) {
            close();fail("frontend.dependency_metadata_invalid","Resolver metadata violates the exact API contract.","resolve","dependency",std::string(ownerUri),std::string(pointer));
        }
        if(resolved.bytes.size>limits.max_dependency_bytes){close();limit("Dependency bytes exceed the effective ceiling.",ownerUri,pointer);}
        if(span(resolved.source_uri)==uri()) {
            close();fail("frontend.dependency_uri_conflict","A TypeDef dependency cannot claim the authoritative root Source URI.",
                "resolve","dependency",std::string(ownerUri),std::string(pointer));
        }
        // Check aggregate/unique ceilings before allocating a new large
        // snapshot. Alias requests for an existing identity reuse its owner.
        Snapshot snapshot;snapshot.kind=kind;snapshot.uri=span(resolved.source_uri);snapshot.identity=span(resolved.identity);snapshot.version=span(resolved.version);
        const auto snapshotKey=std::to_string(kind)+":"+snapshot.identity;
        if(const auto previous=snapshots.find(snapshotKey);previous!=snapshots.end()) {
            snapshot.hash=ne::hashing::sha256Hex(span(resolved.bytes));
            close();poll();verify(snapshot);
            if(previous->second.uri!=snapshot.uri||previous->second.version!=snapshot.version||previous->second.hash!=snapshot.hash)
                fail("frontend.dependency_conflict","One dependency identity resolved to contradictory URI/version/content snapshots.","resolve","dependency",snapshot.uri);
            referenceKeys.emplace(key,snapshotKey);return previous->second;
        }
        if(snapshots.size()>=limits.max_dependencies){close();limit("Unique dependencies exceed the effective ceiling.",snapshot.uri);}
        if(dependencyBytes>limits.max_total_dependency_bytes||resolved.bytes.size>limits.max_total_dependency_bytes-dependencyBytes) {
            close();limit("Total unique dependency bytes exceed the effective ceiling.",snapshot.uri);
        }
        // Capture exactly once before releasing any host-owned views.
        snapshot.bytes=std::make_shared<const std::string>(resolved.bytes.data,resolved.bytes.size);
        close();poll();
        if(kind!=FROG_FRONTEND_DEPENDENCY_SOURCE&&!validUtf8(*snapshot.bytes))fail("frontend.dependency_utf8_invalid","The captured dependency is not strict UTF-8.","resolve","dependency",snapshot.uri);
        snapshot.hash=ne::hashing::sha256Hex(*snapshot.bytes);verify(snapshot);
        if(const auto previous=uriKeys.find(snapshot.uri);previous!=uriKeys.end()&&previous->second!=snapshotKey)
            fail("frontend.dependency_alias_ambiguous","One canonical URI claims several dependency identities.","resolve","dependency",snapshot.uri);
        if(!add(dependencyBytes,snapshot.bytes->size(),limits.max_total_dependency_bytes))limit("Total unique dependency bytes exceed the effective ceiling.",snapshot.uri);
        std::string error;auto parsed=ne::source::parseJson(*snapshot.bytes,&error,ne::source::JsonNumberPolicy::PreserveLexeme);
        if(!parsed&&kind!=FROG_FRONTEND_DEPENDENCY_SOURCE)fail("frontend.dependency_json_invalid",error,"resolve","dependency",snapshot.uri);
        if(parsed){snapshot.document=std::move(*parsed);walk(snapshot.document,snapshot.uri);}
        uriKeys.emplace(snapshot.uri,snapshotKey);referenceKeys.emplace(key,snapshotKey);
        return snapshots.emplace(snapshotKey,std::move(snapshot)).first->second;
    }
    const Snapshot &capture(const Json &ref,std::string_view ownerUri,std::string_view pointer) {
        fields(ref,{"$ref","identity","version","sha256","pointer"},ownerUri,pointer);
        const auto reference=requiredText(ref,"$ref",ownerUri,pointer);
        const auto identity=requiredText(ref,"identity",ownerUri,pointer);
        const auto version=requiredText(ref,"version",ownerUri,pointer);
        const auto expected=requiredText(ref,"sha256",ownerUri,pointer);
        if(!digest(expected))fail("frontend.dependency_digest_invalid","An authored dependency digest must be 64 lowercase hexadecimal characters.","resolve","dependency",std::string(ownerUri),std::string(pointer));
        const auto &snapshot=captureDocument(FROG_FRONTEND_DEPENDENCY_TYPE_DEFINITION,reference,identity,version,expected,ownerUri,pointer);
        fields(snapshot.document,{"profile","identity","version","definitions"},snapshot.uri,{});
        if(requiredText(snapshot.document,"profile",snapshot.uri,{})!=typeDefinitionsProfile||
            requiredText(snapshot.document,"identity",snapshot.uri,{})!=snapshot.identity||
            requiredText(snapshot.document,"version",snapshot.uri,{})!=snapshot.version||
            !snapshot.document.member("definitions")||snapshot.document.member("definitions")->type!=Json::Type::Object)
            fail("frontend.dependency_envelope_invalid","The dependency's own type-definition profile/identity/version must match its captured metadata.","resolve","dependency",snapshot.uri);
        return snapshot;
    }
    Json resolve(const Json &value,std::string_view ownerUri,std::string_view pointer,std::uint64_t depth=0) {
        poll();if(depth>limits.max_nesting_depth||++steps>maximumResolutionSteps)limit("Type-reference expansion exceeds its bounded nesting/work ceiling.",ownerUri,pointer);
        if(value.type==Json::Type::Object&&value.member("$ref")) {
            const auto &snapshot=capture(value,ownerUri,pointer);
            const auto fragment=requiredText(value,"pointer",ownerUri,pointer,true);
            if(!fragment.starts_with("/definitions/"))fail("frontend.dependency_fragment_invalid","A type fragment must select an explicit /definitions/... value.","resolve","dependency",std::string(ownerUri),std::string(pointer));
            const auto *selected=at(snapshot.document,fragment);
            if(!selected)fail("frontend.dependency_fragment_missing","The selected authored type fragment does not exist.","resolve","dependency",snapshot.uri,fragment);
            const auto cycle=snapshot.uri+"#"+fragment;
            if(!active.insert(cycle).second)fail("frontend.dependency_cycle","An authored type-reference cycle was detected.","resolve","dependency",snapshot.uri,fragment);
            auto result=resolve(*selected,snapshot.uri,fragment,depth+1);active.erase(cycle);
            ne::CanonicalObjectReferenceDiagnostic why;
            if(!ne::canonicalObjectReferenceReadType(result,&why))fail("frontend.type_fragment_invalid",why.message,"resolve","semantics",snapshot.uri,fragment);
            auto transformation=cj::object();transformation.object={{"profile",cj::string(std::string(resolutionProfile))},
                {"source_uri",cj::string(std::string(ownerUri))},{"source_pointer",cj::string(std::string(pointer))},
                {"dependency_uri",cj::string(snapshot.uri)},{"dependency_pointer",cj::string(fragment)},
                {"dependency_sha256",cj::string(snapshot.hash)}};
            transformations.array.push_back(std::move(transformation));return result;
        }
        auto result=value;
        if(result.type==Json::Type::Object)for(auto &[key,child]:result.object)child=resolve(child,ownerUri,cj::pointer(pointer,key),depth+1);
        else if(result.type==Json::Type::Array)for(std::size_t i=0;i<result.array.size();++i)result.array[i]=resolve(result.array[i],ownerUri,cj::pointer(pointer,std::to_string(i)),depth+1);
        return result;
    }
    bool project(Json &root) {
        if(semanticContext.object.empty())return false;
        fields(semanticContext,{"profile","dependencies","conditional_disable"},uri(),"/semantic_context");
        if(const auto profile=requiredText(semanticContext,"profile",uri(),"/semantic_context");profile!=resolutionProfile&&profile!=draftResolutionProfile)
            fail("frontend.context_profile_unsupported","The semantic context requires the exact local resolution profile.","resolve","frontend-capability",uri(),"/semantic_context/profile",FROG_FRONTEND_UNSUPPORTED);
        const auto *dependencies=semanticContext.member("dependencies");
        if(!dependencies||dependencies->type!=Json::Type::Array)fail("frontend.dependencies_invalid","The resolution profile requires its explicit dependencies array.","resolve","dependency",uri(),"/semantic_context/dependencies");
        if(dependencies->array.size()>limits.max_dependencies)limit("Requested dependency bindings exceed their effective ceiling.",uri(),"/semantic_context/dependencies");
        std::set<std::string> targets;
        for(std::size_t i=0;i<dependencies->array.size();++i) {
            const auto &binding=dependencies->array[i];const auto contextPath="/semantic_context/dependencies/"+std::to_string(i);
            fields(binding,{"kind","source_pointer"},uri(),contextPath);
            const auto *kind=binding.member("kind");
            if(!kind||kind->type!=Json::Type::Number||kind->string!="2")
                fail("frontend.dependency_kind_unsupported","This explicit resolution profile currently binds only type_definition (kind2) fragments.","resolve","frontend-capability",uri(),contextPath+"/kind",FROG_FRONTEND_UNSUPPORTED);
            const auto path=requiredText(binding,"source_pointer",uri(),contextPath);
            if(std::none_of(schemaRoots.begin(),schemaRoots.end(),[&](const auto &root){return path==root||path.starts_with(root+"/");}))
                fail("frontend.dependency_target_unsupported","A type dependency must select an actual node/port/widget studio_type_schema or its child schema.","resolve","frontend-capability",uri(),path,FROG_FRONTEND_UNSUPPORTED);
            for(const auto &other:targets)if(path==other||path.starts_with(other+"/")||other.starts_with(path+"/"))
                fail("frontend.dependency_target_conflict","Dependency projection targets overlap.","resolve","dependency",uri(),path);
            targets.insert(path);
        }
        for(const auto &path:targets) {
            auto *destination=at(root,path);
            if(!destination||destination->type!=Json::Type::Object||!destination->member("$ref"))
                fail("frontend.dependency_target_invalid","The target must be an actual authored $ref object, not a host-supplied replacement.","resolve","dependency",uri(),path);
            *destination=resolve(*destination,uri(),path);
        }
        return !targets.empty();
    }
    ne::ConditionalDisableCompilationContext conditionalContext() {
        ne::ConditionalDisableCompilationContext out;
        if(semanticContext.object.empty())return out;
        fields(semanticContext,{"profile","dependencies","conditional_disable"},uri(),"/semantic_context");
        if(const auto profile=requiredText(semanticContext,"profile",uri(),"/semantic_context");profile!=resolutionProfile&&profile!=draftResolutionProfile)
            fail("frontend.context_profile_unsupported","The semantic context requires its exact resolution profile.","resolve","frontend-capability",uri(),"/semantic_context/profile",FROG_FRONTEND_UNSUPPORTED);
        const auto *conditional=semanticContext.member("conditional_disable");
        if(!conditional)return out;
        if(requiredText(semanticContext,"profile",uri(),"/semantic_context")==resolutionProfile&&
            (conditional->type!=Json::Type::Object||!conditional->object.empty()))
            fail("frontend.conditional_context_unsupported","Conditional symbols require frog.frontend.resolution@2.","resolve","frontend-capability",uri(),"/semantic_context/conditional_disable",FROG_FRONTEND_UNSUPPORTED);
        fields(*conditional,{"built_in","project","target"},uri(),"/semantic_context/conditional_disable");
        for(const auto &[name,destination]:std::initializer_list<std::pair<const char*,ne::ConditionalDisableSymbolMap*>>{
            {"built_in",&out.builtInSymbols},{"project",&out.projectSymbols},{"target",&out.targetSymbols}}) {
            const auto *symbols=conditional->member(name);if(!symbols)continue;
            if(symbols->type!=Json::Type::Object)fail("frontend.conditional_context_invalid","Symbols require an explicit string map.","resolve","semantics",uri(),"/semantic_context/conditional_disable");
            for(const auto &[key,value]:symbols->object) {
                if(key.empty()||!text(key)||value.type!=Json::Type::String||!text(value.string))
                    fail("frontend.conditional_symbol_invalid","Conditional symbols require bounded names and string values.","resolve","semantics",uri(),"/semantic_context/conditional_disable");
                destination->emplace(key,value.string);
            }
        }
        return out;
    }
    ne::DraftDependencyResult draftDependency(const ne::DraftDependencyRequest &wanted) {
        const auto &value=captureDocument(static_cast<std::uint32_t>(wanted.kind),wanted.authoredReference,
            wanted.expectedIdentity,wanted.expectedVersion,wanted.expectedSha256,wanted.ownerUri,wanted.sourcePointer);
        return {ne::DraftDependencyState::Present,value.uri,value.identity,value.version,value.hash,value.bytes,{}};
    }
    void draftDiagnostic(const ne::DocumentDiagnostic &d) {
        const auto category=std::string(ne::documentDiagnosticDomainId(d.domain));
        const frog_frontend_outcome outcome=d.domain==ne::DocumentDiagnosticDomain::FrontendCapability?FROG_FRONTEND_UNSUPPORTED:FROG_FRONTEND_REJECTED;
        const char *stage="validate";
        switch(d.stage) {
        case ne::DocumentDiagnosticStage::Read:
        case ne::DocumentDiagnosticStage::Format:stage="read";break;
        case ne::DocumentDiagnosticStage::Resolve:stage="resolve";break;
        case ne::DocumentDiagnosticStage::Adaptation:stage="adaptation";break;
        case ne::DocumentDiagnosticStage::Fir:stage="derive";break;
        default:break;
        }
        diagnostic({outcome,d.code,d.message,stage,category,d.sourceUri,d.sourcePath});
        auto &entry=diagnostics.array.back();
        entry.object["severity"]=cj::string(d.severity==ne::DocumentDiagnosticSeverity::Error?"error":d.severity==ne::DocumentDiagnosticSeverity::Warning?"warning":"info");
        auto &origin=entry.object["source"];
        const auto object=!d.nodeId.empty()?d.nodeId:!d.widgetId.empty()?d.widgetId:d.subjectId;
        if(!object.empty())origin.object["object_id"]=cj::string(object);
        if(!d.portId.empty())origin.object["port_id"]=cj::string(d.portId);
        if(!d.edgeId.empty())origin.object["connection_id"]=cj::string(d.edgeId);
        if(!d.regionId.empty())origin.object["region_id"]=cj::string(d.regionId);
        if(!d.callsiteId.empty()) {
            auto related=cj::object();related.object={{"uri",cj::string(d.relatedSourceUri.empty()?uri():d.relatedSourceUri)},
                {"pointer",cj::string(d.relatedSourcePath)},{"object_id",cj::string(d.callsiteId)}};
            auto locations=cj::array();locations.array.push_back(std::move(related));entry.object["related_locations"]=std::move(locations);
        }
    }
    void sourceDiagnostic(const ne::CanonicalIrDiagnostic &d) {
        std::string pointer=d.location,origin=uri();
        const auto prefix=(preparedUri.empty()?origin:preparedUri)+"#";
        if(pointer.starts_with(prefix))pointer.erase(0,prefix.size());
        if(!pointer.empty()&&pointer.front()=='/') {
            const ne::CanonicalDraftMigrationIdentity *best=nullptr;
            for(const auto &mapping:draftMappings)if(!mapping.generated&&!mapping.publicPointer.empty()&&
                (pointer==mapping.publicPointer||pointer.starts_with(mapping.publicPointer+"/"))&&
                (!best||mapping.publicPointer.size()>best->publicPointer.size()))best=&mapping;
            if(best)pointer=best->originalPointer+pointer.substr(best->publicPointer.size());
        }
        if(!pointer.empty()&&pointer.front()!='/') {
            const auto found=objectPaths.find(pointer);pointer=found==objectPaths.end()?std::string{}:found->second;
        }
        const bool limited=d.code=="IR_RESOURCE_LIMIT"||d.code=="IR_RESOURCE_POLICY"||d.code=="EVENT_IR_LIMIT";
        diagnostic({static_cast<frog_frontend_outcome>(limited?FROG_FRONTEND_LIMIT_EXCEEDED:d.unsupported?FROG_FRONTEND_UNSUPPORTED:FROG_FRONTEND_REJECTED),
            d.code,d.message,"derive",limited?"control":d.unsupported?"frontend-capability":"semantics",origin,pointer});
    }
    Json contextProvenance()const {
        auto out=cj::object(),source=cj::object(),fir=cj::object(),dependencies=cj::array();
        source.object={{"family",cj::string(request?std::string(span(request->source_format_id)):std::string{})},
            {"version",cj::string(request?std::string(span(request->source_format_version)):std::string{})},{"uri",cj::string(uri())},
            {"document_id",cj::string(request?std::string(span(request->source_document_id)):std::string{})},{"sha256",cj::string(rootHash)}};
        fir.object={{"family",cj::string("frog.execution_ir")},{"version",cj::string(request?std::string(span(request->fir_format_version)):std::string{})},
            {"document_id",cj::string(firDocumentId)}};
        for(const auto &[key,snapshot]:snapshots) {
            (void)key;auto dependency=cj::object();dependency.object={{"kind",number(snapshot.kind)},{"identity",cj::string(snapshot.identity)},
                {"version",cj::string(snapshot.version)},{"sha256",cj::string(snapshot.hash)},{"source_uri",cj::string(snapshot.uri)},
                {"origin",cj::string("explicit-resolver")}};dependencies.array.push_back(std::move(dependency));
        }
        out.object={{"schema",cj::string("frog.frontend.context")},{"schema_version",cj::string("0.3")},
            {"source",std::move(source)},{"fir",std::move(fir)},{"frontend",implementation()},
            {"profiles",selectedProfiles},{"operations",realizedOperations},
            {"dependencies",std::move(dependencies)},{"closure_complete",cj::boolean(closureComplete)},
            {"semantic_context",semanticContext},{"limits",limitsJson(limits)},{"transformations",transformations}};
        if(!projectedHash.empty())out.object["projected_source_sha256"]=cj::string(projectedHash);
        return out;
    }
    OperationResult finish(frog_frontend_outcome outcome,std::string fir={},Json descriptor=Json{},std::string_view resolvedSource={}) {
        if(outcome!=FROG_FRONTEND_SUCCESS){fir.clear();resolvedSource={};}
        maps.object["schema"]=cj::string("frog.frontend.source_maps");maps.object["schema_version"]=cj::string("0.1");
        maps.object["authored_source_sha256"]=cj::string(rootHash);maps.object["transformations"]=transformations;
        provenance=contextProvenance();
        std::uint64_t total=0;
        const auto fits=[&]{total=0;return add(total,fir.size(),limits.max_result_bytes)&&add(total,resolvedSource.size(),limits.max_result_bytes)&&jsonSize(diagnostics,total,limits.max_result_bytes)&&
            jsonSize(maps,total,limits.max_result_bytes)&&jsonSize(provenance,total,limits.max_result_bytes)&&
            (descriptor.type==Json::Type::Null||jsonSize(descriptor,total,limits.max_result_bytes));};
        if(!fits()) {
            outcome=FROG_FRONTEND_LIMIT_EXCEEDED;fir.clear();resolvedSource={};descriptor=Json{};maps=cj::object();
            maps.object={{"schema",cj::string("frog.frontend.source_maps")},{"schema_version",cj::string("0.1")}};
            diagnostics=cj::array();diagnosticsTruncated=false;
            diagnostic({outcome,"frontend.result_limit","Complete result documents exceed the effective aggregate result budget.","derive","control",uri(),{}});
            if(!fits())throw ApiFailure{FROG_FRONTEND_OUT_OF_MEMORY};
        }
        OperationResult result;result.outcome=outcome;result.firJson=std::move(fir);
        // The borrowed complete projection is copied only after the aggregate
        // output budget admitted it. No result retains this temporary view.
        if(!resolvedSource.empty())result.resolvedSourceJson.assign(resolvedSource);
        result.diagnosticsJson=cj::encode(diagnostics);result.sourceMapsJson=cj::encode(maps);result.provenanceJson=cj::encode(provenance);
        if(descriptor.type!=Json::Type::Null)result.descriptorJson=cj::encode(descriptor);
        return result;
    }
};

Json describe(const frog_frontend_limits &limits) {
    auto result=cj::object(),source=cj::object(),fir=cj::object();
    source.object={{"family",cj::string("frog.source")},{"version",cj::string("0.1")}};
    fir.object={{"family",cj::string("frog.execution_ir")},{"version",cj::string("0.1")}};
    auto sources=cj::array(),firs=cj::array();sources.array.push_back(std::move(source));firs.array.push_back(std::move(fir));
    for(unsigned revision=0;revision<=ne::kMaximumSupportedFrogDraftRevision;++revision) {auto entry=cj::object();entry.object={{"family",cj::string(std::string(ne::kFrogDraftDocumentFormat))},{"version",cj::string(std::to_string(revision))}};sources.array.push_back(std::move(entry));}
    result.object={{"schema",cj::string("frog.frontend.capabilities")},{"schema_version",cj::string("0.2")},
        {"source_formats",std::move(sources)},{"fir_formats",std::move(firs)},{"frontend_apis",strings({"3"})},
        {"frontend",implementation()},{"api_operations",strings({"validate","derive","describe"})},
        {"profiles",strings({"frog.canonical.widget-f64-add.v1","frog.canonical.typed-structures.v1","frog.studio.composed-program@1","frog.canonical.event-scalar.v1","frog.canonical.event-captured.v1",resolutionProfile,draftResolutionProfile})},
        {"profiles_exhaustive",cj::boolean(false)},
        {"default_limits",limitsJson(normalized({}))},{"effective_limits",limitsJson(limits)},
        {"max_fir_bytes",number(ne::CanonicalIrLimits{}.maximumIrBytes)},
        {"max_resolution_steps",number(maximumResolutionSteps)},
        {"dependency_kinds",strings({"source","type_definition"})},{"dissemination",cj::string("local")},
        {"native_execution_claim",cj::boolean(false)}};
    return result;
}
bool eventSource(const ne::FrogSourceDocument &source) {
    const auto &root=source.root();
    const auto *diagram=root.member("diagram");if(!diagram)return false;
    if(const auto *profile=diagram->member("profile");profile&&profile->type==Json::Type::String&&profile->string=="frog.studio.composed-program@1")return false;
    // The public source owner enumerates root and hidden diagrams, without
    // discovering structures by names or graphical positions.
    for(const auto &scope:source.diagramScopes())if(const auto *nodes=scope.diagram->member("nodes");nodes&&nodes->type==Json::Type::Array)
        for(const auto &node:nodes->array) {
            const auto *kind=node.member("kind"),*structure=node.member("structure_type");
            if(kind&&kind->type==Json::Type::String&&kind->string=="structure"&&structure&&
                structure->type==Json::Type::String&&structure->string=="event_structure")return true;
        }
    return false;
}
} // namespace

bool validUtf8(std::string_view text) noexcept {
    // Same Unicode scalar/overlong policy as the production JSON reader. This
    // is an ABI byte-span check, not an alternative JSON/source parser.
    for(std::size_t i=0;i<text.size();) {
        const auto lead=static_cast<unsigned char>(text[i++]);
        std::uint32_t cp=lead,minimum=0;unsigned continuation=0;
        if(lead==0)return false;
        if(lead<0x80)continue;
        if(lead>=0xc2&&lead<=0xdf){continuation=1;minimum=0x80;cp&=31;}
        else if(lead>=0xe0&&lead<=0xef){continuation=2;minimum=0x800;cp&=15;}
        else if(lead>=0xf0&&lead<=0xf4){continuation=3;minimum=0x10000;cp&=7;}
        else return false;
        if(continuation>text.size()-i)return false;
        while(continuation--) {const auto c=static_cast<unsigned char>(text[i++]);if((c&0xc0)!=0x80)return false;cp=(cp<<6)|(c&63);}
        if(cp<minimum||cp>0x10ffff||(cp>=0xd800&&cp<=0xdfff))return false;
    }return true;
}

OperationResult runOperation(frog_frontend_operation operation,const frog_frontend_config &config,
    const frog_frontend_request *request,std::shared_ptr<const ne::FrogSourceDocument> capturedSource,
    PreparedSourceCallback preparedSource) {
    apiArguments(operation,config,request);
    if(operation==FROG_FRONTEND_DESCRIBE&&capturedSource)throw ApiFailure{FROG_FRONTEND_INVALID_ARGUMENT};
    Request work{request,normalized(config.limits)};
    if(operation==FROG_FRONTEND_DESCRIBE)return work.finish(FROG_FRONTEND_SUCCESS,{},describe(work.limits));
    if(!request)throw ApiFailure{FROG_FRONTEND_INVALID_ARGUMENT};
    try {
        work.poll();
        if(request->source_bytes.size>work.limits.max_source_bytes)work.limit("Source bytes exceed their effective ceiling.",work.uri());
        if(request->semantic_context_json.size>work.limits.max_source_bytes)work.limit("Semantic-context bytes exceed their effective ceiling.",work.uri(),"/semantic_context");
        if(capturedSource) {
            const auto captured=capturedSource->originalUtf8(),requested=span(request->source_bytes);
            if(captured.size()!=requested.size()||(captured.data()!=requested.data()&&captured!=requested))
                throw ApiFailure{FROG_FRONTEND_INVALID_ARGUMENT};
        }
        // A verified immutable FrogSourceDocument has already crossed the
        // strict public UTF-8/JSON reader. Semantic admission is still rerun.
        const bool draft=span(request->source_format_id)==ne::kFrogDraftDocumentFormat;
        if((!draft&&!capturedSource&&!validUtf8(span(request->source_bytes)))||!validUtf8(span(request->semantic_context_json)))throw ApiFailure{FROG_FRONTEND_INVALID_ARGUMENT};
        work.rootHash=ne::hashing::sha256Hex(span(request->source_bytes));
        // Capture valid context metadata even when the source format is later
        // refused. Do not resolve dependencies or change refusal precedence.
        std::string contextError;
        std::optional<Json> capturedContext;
        if(request->semantic_context_json.size) {
            capturedContext=ne::source::parseJson(span(request->semantic_context_json),&contextError,ne::source::JsonNumberPolicy::PreserveLexeme);
            if(capturedContext&&capturedContext->type==Json::Type::Object)work.semanticContext=std::move(*capturedContext);
        }
        unsigned draftRevision=0;const auto version=span(request->source_format_version);
        const auto parsedRevision=std::from_chars(version.data(),version.data()+version.size(),draftRevision);
        const bool draftVersion=parsedRevision.ec==std::errc{}&&parsedRevision.ptr==version.data()+version.size()&&draftRevision<=ne::kMaximumSupportedFrogDraftRevision&&std::to_string(draftRevision)==version;
        if((draft?!draftVersion:(span(request->source_format_id)!="frog.source"||version!="0.1"))||span(request->fir_format_version)!="0.1")
            fail("frontend.format_unsupported","An exact supported Source or Studio draft revision and FIR0.1 are required; API/software versions do not substitute for formats.","read","compatibility",work.uri(),{},FROG_FRONTEND_UNSUPPORTED);
        if(request->semantic_context_json.size) {
            if(!capturedContext||capturedContext->type!=Json::Type::Object)fail("frontend.context_invalid",contextError.empty()?"The semantic context must be an explicit JSON object.":contextError,"resolve","dependency",work.uri(),"/semantic_context");
        }
        auto source=std::move(capturedSource);std::string error;
        std::shared_ptr<const ne::DraftFrontendPreparation> draftPrepared;
        if(draft) {
            if(source)throw ApiFailure{FROG_FRONTEND_INVALID_ARGUMENT};
            const auto context=work.conditionalContext();
            auto prepared=ne::prepareDraftFrontend(span(request->source_bytes),work.uri(),context,
                [&](const ne::DraftDependencyRequest &wanted){return work.draftDependency(wanted);},[&]{work.poll();});
            for(const auto &diagnostic:prepared.diagnostics)work.draftDiagnostic(diagnostic);
            if(!prepared.accepted()) {
                if(prepared.diagnostics.empty())work.diagnostic({FROG_FRONTEND_REJECTED,"frontend.draft_preparation_failed","Draft preparation is incomplete.","validate","control",work.uri(),{}});
                return work.finish(FROG_FRONTEND_REJECTED);
            }
            if(prepared.sourceRevision!=draftRevision)
                fail("frontend.source_revision_mismatch","The declared draft revision differs from its actual envelope.","read","compatibility",work.uri(),"/draft_revision");
            source=prepared.source;work.projectedHash=ne::hashing::sha256Hex(source->originalUtf8());
            work.draftMappings=prepared.identities;
            for(const auto &mapping:prepared.identities) {
                auto record=cj::object();record.object={{"id",cj::string(mapping.id)},{"kind",cj::string(mapping.kind)},
                    {"source_pointer",cj::string(mapping.originalPointer)},{"public_pointer",cj::string(mapping.publicPointer)},{"generated",cj::boolean(mapping.generated)}};
                work.maps.object["draft_identities"].type=Json::Type::Array;
                work.maps.object["draft_identities"].array.push_back(std::move(record));
                if(!mapping.generated)work.objectPaths[mapping.id]=mapping.originalPointer;
            }
            draftPrepared=std::make_shared<const ne::DraftFrontendPreparation>(std::move(prepared));
        } else if(!source) {
            auto owner=std::make_shared<const std::string>(request->source_bytes.data,request->source_bytes.size);
            auto parsed=ne::FrogSourceDocument::readOwned(std::move(owner),&error);
            if(parsed)source=std::make_shared<const ne::FrogSourceDocument>(std::move(*parsed));
        }
        if(!source)fail("frontend.source_json_invalid",error,"read","format",work.uri());
        if(ne::mixedFrogSourceAuthorities(source->root()))fail("preparation.authority.mixed",
            "Public source and Studio draft envelopes cannot share one execution authority.",
            "read","compatibility",work.uri());
        if(source->root().member("format"))fail("frontend.source_authority_unsupported",
            "The declared public Source format does not match this draft envelope.",
            "read","compatibility",work.uri(),"/format",FROG_FRONTEND_UNSUPPORTED);
        if(source->envelopeStatus()!=ne::FrogSourceEnvelopeStatus::Recognized||!source->diagnostics().empty()) {
            frog_frontend_outcome outcome=source->envelopeStatus()==ne::FrogSourceEnvelopeStatus::UnsupportedVersion?FROG_FRONTEND_UNSUPPORTED:FROG_FRONTEND_REJECTED;
            for(const auto &d:source->diagnostics()) {
                std::string code="frontend.source_invalid",category="structure";
                if(d.code==ne::FrogSourceDiagnosticCode::UnsupportedVersion){code="frontend.source_version_unsupported";category="compatibility";outcome=FROG_FRONTEND_UNSUPPORTED;}
                else if(d.code==ne::FrogSourceDiagnosticCode::InvalidEnvelope){code="frontend.source_envelope_invalid";category="format";}
                else if(d.code==ne::FrogSourceDiagnosticCode::InvalidInterface){code="frontend.interface_invalid";category="interface";}
                else if(d.code==ne::FrogSourceDiagnosticCode::DuplicateIdentity){code="frontend.duplicate_identity";category="integrity";}
                else if(d.code==ne::FrogSourceDiagnosticCode::InspectionLimit){code="frontend.inspection_limit";category="control";outcome=FROG_FRONTEND_LIMIT_EXCEEDED;}
                work.diagnostic({outcome,std::move(code),d.message,"read",std::move(category),work.uri(),d.path});
            }
            if(source->diagnostics().empty())work.diagnostic({outcome,"frontend.source_envelope_invalid","The public source envelope is not recognized.","read","format",work.uri(),{}});
            return work.finish(outcome);
        }
        work.countSourceGraphs(*source);work.walk(source->root(),work.uri(),!draft);work.poll();
        // Only real declaration slots may acquire a type dependency. A
        // metadata/documentation field with the same spelling has no authority.
        for(const auto &scope:source->diagramScopes())if(const auto *nodes=scope.diagram->member("nodes");nodes&&nodes->type==Json::Type::Array)
            for(std::size_t i=0;i<nodes->array.size();++i) {
                const auto &node=nodes->array[i];const auto path=scope.path+"/nodes/"+std::to_string(i);
                if(node.member("studio_type_schema"))work.schemaRoots.insert(path+"/studio_type_schema");
                if(const auto *ports=node.member("studio_ports");ports&&ports->type==Json::Type::Array)
                    for(std::size_t p=0;p<ports->array.size();++p)if(ports->array[p].member("studio_type_schema"))
                        work.schemaRoots.insert(path+"/studio_ports/"+std::to_string(p)+"/studio_type_schema");
            }
        if(const auto *panel=source->section("front_panel"))if(const auto *widgets=panel->member("widgets");widgets&&widgets->type==Json::Type::Array)
            for(std::size_t i=0;i<widgets->array.size();++i)if(widgets->array[i].member("studio_type_schema"))
                work.schemaRoots.insert("/front_panel/widgets/"+std::to_string(i)+"/studio_type_schema");
        if(!draft&&!work.semanticContext.object.empty()) {
            (void)work.conditionalContext();
            auto projected=source->root();
            if(work.project(projected)) {
                std::uint64_t bytes=0;if(!jsonSize(projected,bytes,work.limits.max_source_bytes))work.limit("Resolved source projection exceeds the source byte ceiling.",work.uri());
                auto projectedOwner=std::make_shared<const std::string>(cj::encode(projected));
                work.projectedHash=ne::hashing::sha256Hex(*projectedOwner);
                auto parsed=ne::FrogSourceDocument::readOwned(std::move(projectedOwner),&error);
                if(!parsed)fail("frontend.projection_invalid",error,"resolve","structure",work.uri());
                source=std::make_shared<const ne::FrogSourceDocument>(std::move(*parsed));
                work.walk(source->root(),work.uri(),false);
            }
        }
        work.closureComplete=true;work.poll();
        work.preparedUri=draft?"frog-prepared:/"+work.rootHash+".frog":work.uri();
        if(preparedSource) {
            work.firDocumentId=preparedSource(source,draftPrepared,work.preparedUri);
            if(work.firDocumentId.empty()||!text(work.firDocumentId))throw ApiFailure{FROG_FRONTEND_INVALID_ARGUMENT};
        }
        ne::CanonicalIrReadOptions options;options.sourceUri=work.preparedUri;
        options.limits.maximumSourceBytes=static_cast<std::size_t>(work.limits.max_source_bytes);
        options.limits.maximumIrBytes=static_cast<std::size_t>(std::min<std::uint64_t>(options.limits.maximumIrBytes,work.limits.max_result_bytes));
        options.limits.maximumNodes=static_cast<std::size_t>(work.limits.max_nodes);
        options.limits.maximumEdges=static_cast<std::size_t>(work.limits.max_connections);
        std::vector<ne::CanonicalIrDiagnostic> diagnostics;
        std::optional<ne::CanonicalExecutionIr> regular;
        std::optional<ne::CanonicalEventExecutionIr> event;
        if(eventSource(*source))event=ne::CanonicalEventExecutionIr::derive(*source,work.firDocumentId,options,&diagnostics);
        else regular=ne::CanonicalExecutionIr::derive(*source,work.firDocumentId,options,&diagnostics);
        work.poll();
        if(!regular&&!event) {
            frog_frontend_outcome outcome=FROG_FRONTEND_REJECTED;
            for(const auto &d:diagnostics) {work.sourceDiagnostic(d);if(d.unsupported)outcome=FROG_FRONTEND_UNSUPPORTED;
                if(d.code=="IR_RESOURCE_LIMIT"||d.code=="IR_RESOURCE_POLICY"||d.code=="EVENT_IR_LIMIT")outcome=FROG_FRONTEND_LIMIT_EXCEEDED;}
            if(diagnostics.empty())work.diagnostic({outcome,"frontend.derivation_failed","The production frontend refused the source without emitting an artifact.","derive","frontend-capability",work.uri(),{}});
            return work.finish(outcome);
        }
        const auto &root=regular?regular->root():event->root();
        std::string profile;
        if(event)profile=event->requiresCapturedValues()?"frog.canonical.event-captured.v1":"frog.canonical.event-scalar.v1";
        else if(const auto *metadata=root.member("metadata")) {
            if(const auto *explicitProfile=metadata->member("profile");explicitProfile&&explicitProfile->type==Json::Type::String)profile=explicitProfile->string;
            else if(const auto *producer=metadata->member("producer")) {
                if(producer->string=="frog.native.canonical-execution-ir")profile="frog.canonical.widget-f64-add.v1";
                else if(producer->string=="frog.native.canonical-structured-ir")profile="frog.canonical.typed-structures.v1";
            }
        }
        if(!profile.empty())work.selectedProfiles.array.push_back(cj::string(std::move(profile)));
        if(!work.transformations.array.empty())work.selectedProfiles.array.push_back(cj::string(std::string(resolutionProfile)));
        if(const auto *unit=root.member("unit")) {
            for(const auto key:{"source_map","correspondence"})if(const auto *entries=unit->member(key))work.maps.object[key]=*entries;
            std::set<std::string> operations;
            if(const auto *objects=unit->member("objects"))for(const auto &object:objects->array)
                if(const auto *kind=object.member("kind");kind&&kind->type==Json::Type::String)operations.insert(kind->string);
            for(const auto &kind:operations)work.realizedOperations.array.push_back(cj::string(kind));
        }
        work.maps.object["fir_source_sha256"]=cj::string(regular?std::string(regular->sourceHash()):std::string(event->sourceHash()));
        // validate deliberately uses precisely the same producer and sealing
        // checks. It simply withholds executable FIR from its successful result.
        auto fir=operation==FROG_FRONTEND_DERIVE?std::string(regular?regular->originalUtf8():event->originalUtf8()):std::string{};
        const auto resolved=work.projectedHash.empty()?std::string_view{}:source->originalUtf8();
        auto result=work.finish(FROG_FRONTEND_SUCCESS,std::move(fir),Json{},resolved);
        if(result.outcome==FROG_FRONTEND_SUCCESS) {
            result.canonicalSource=std::move(source);
            result.draftPreparation=std::move(draftPrepared);
            if(operation==FROG_FRONTEND_DERIVE) {
                if(regular)result.typedFir=std::make_shared<const ne::CanonicalExecutionIr>(std::move(*regular));
                else result.typedEvent=std::make_shared<const ne::CanonicalEventExecutionIr>(std::move(*event));
            }
        }
        return result;
    }catch(const Failure &failure) {
        work.diagnostic(failure);return work.finish(failure.outcome);
    }
}
} // namespace frog::frontend
