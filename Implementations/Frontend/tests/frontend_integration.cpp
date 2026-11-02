// SPDX-License-Identifier: Apache-2.0
// Direct integration with the real API-3 library. JSON helpers below inspect
// its immutable documents and make identified negative fixture mutations only.
#include <frog/frontend.h>
#include "frog_native_engine/FrogJson.hpp"
#include "frog_native_engine/FrogSourceDocument.hpp"
#include "frog_native_engine/CanonicalEventExecutionIr.hpp"
#include "frog_native_engine/CanonicalFrontendMetrics.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <future>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {
namespace fs=std::filesystem;
namespace cj=frog::native_engine::canonical_json;
using Json=frog::native_engine::source::JsonValue;
std::atomic<unsigned> checks{0};
std::string phase;
void need(bool condition,std::string_view why){++checks;if(!condition)throw std::runtime_error(phase+": "+std::string(why));}
frog_frontend_utf8 span(std::string_view v){return {v.data(),v.size()};}
std::string copy(frog_frontend_utf8 v){need(v.data||!v.size,"ABI returned a malformed span");return v.size?std::string(v.data,v.size):std::string{};}
Json parse(std::string_view bytes){std::string error;auto value=frog::native_engine::source::parseJson(bytes,&error,frog::native_engine::source::JsonNumberPolicy::PreserveLexeme);need(bool(value),"Invalid complete result/fixture JSON: "+error);return std::move(*value);}
const Json &field(const Json &j,const char *name){const auto *p=j.member(name);need(p!=nullptr,std::string("Missing actual result/fixture member: ")+name);return *p;}
Json &field(Json &j,const char *name){const auto p=j.object.find(name);need(j.type==Json::Type::Object&&p!=j.object.end(),std::string("Missing mutable fixture member: ")+name);return p->second;}
std::string str(const Json &j,const char *name){const auto &v=field(j,name);need(v.type==Json::Type::String,"Expected a typed JSON string");return v.string;}
std::string read(const fs::path &p){need(fs::file_size(p)<=8U*1024U*1024U,"Fixture exceeded the bounded integration corpus");std::ifstream f(p,std::ios::binary);need(bool(f),"Cannot read the supplied real fixture");return {std::istreambuf_iterator<char>(f),{}};}
bool hasString(const Json &j,std::string_view s){if(j.type==Json::Type::String)return j.string==s;if(j.type==Json::Type::Array){for(const auto &v:j.array)if(hasString(v,s))return true;}
    if(j.type==Json::Type::Object)for(const auto &[k,v]:j.object){(void)k;if(hasString(v,s))return true;}return false;}
bool stringContains(const Json &j,std::string_view s){if(j.type==Json::Type::String)return j.string.find(s)!=j.string.npos;if(j.type==Json::Type::Array){for(const auto &v:j.array)if(stringContains(v,s))return true;}
    if(j.type==Json::Type::Object)for(const auto &[k,v]:j.object){(void)k;if(stringContains(v,s))return true;}return false;}
struct Documents {
    frog_frontend_operation operation{};frog_frontend_outcome outcome{};
    std::string fir,diagnostics,maps,provenance,descriptor,resolved;
    bool operator==(const Documents &)const=default;
};
struct Result {
    frog_frontend_result *value=nullptr;
    Result()=default;Result(const Result &)=delete;Result &operator=(const Result &)=delete;
    Result(Result &&other)noexcept:value(std::exchange(other.value,nullptr)){}
    Result &operator=(Result &&other)noexcept{if(this!=&other){frog_frontend_result_destroy(value);value=std::exchange(other.value,nullptr);}return *this;}
    ~Result(){frog_frontend_result_destroy(value);}
    Documents get()const {frog_frontend_result_view v{};v.struct_size=sizeof(v);need(frog_frontend_result_get(value,&v)==FROG_FRONTEND_OK,"Cannot inspect the actual immutable ABI result");
        return {v.operation,v.outcome,copy(v.fir_json),copy(v.diagnostics_json),copy(v.source_maps_json),copy(v.provenance_json),copy(v.descriptor_json),copy(v.resolved_source_json)};}
};
struct Context {
    frog_frontend_context *value=nullptr;
    explicit Context(frog_frontend_config config={}){config.struct_size=sizeof(config);need(frog_frontend_context_create(FROG_FRONTEND_API_VERSION,&config,&value)==FROG_FRONTEND_OK&&value,"Real API context creation failed");}
    ~Context(){frog_frontend_context_destroy(value);}void close(){frog_frontend_context_destroy(std::exchange(value,nullptr));}
    Context(const Context &)=delete;Context &operator=(const Context &)=delete;
};
struct Input {
    std::string bytes,uri,id,firId,format="frog.source",sourceVersion="0.1",firVersion="0.1",semantic="{}";
    frog_frontend_resolver resolver{};frog_frontend_cancel_fn cancelled=nullptr;void *cancelUser=nullptr;
    frog_frontend_request request()const {frog_frontend_request r{};r.struct_size=sizeof(r);r.source_bytes=span(bytes);r.source_uri=span(uri);r.source_document_id=span(id);r.source_format_id=span(format);
        r.source_format_version=span(sourceVersion);r.fir_format_version=span(firVersion);r.fir_document_id=span(firId);r.semantic_context_json=span(semantic);r.resolver=resolver;r.is_cancelled=cancelled;r.cancel_user=cancelUser;return r;}
};
Input input(const fs::path &directory,const char *name){return {read(directory/name),"frog-integration:/fixtures/"+std::string(name),"source:frontend-integration:"+std::string(name),"fir:frontend-integration:"+std::string(name)};}
Result call(Context &context,const Input &in,bool derive){const auto request=in.request();Result result;const auto status=derive?frog_frontend_derive(context.value,&request,&result.value):frog_frontend_validate(context.value,&request,&result.value);
    need(status==FROG_FRONTEND_OK&&result.value,"Actual validate/derive failed at the C API boundary");return result;}
void documents(const Documents &d,const Input &in,bool hashKnown=true){
    const auto diagnostics=parse(d.diagnostics);need(diagnostics.type==Json::Type::Array,"Diagnostics must be a complete array of attributed documents");
    for(const auto &diagnostic:diagnostics.array){need(str(diagnostic,"schema")=="frog.diagnostic"&&str(diagnostic,"version")=="0.2"&&!str(diagnostic,"code").empty(),"Diagnostic schema/version/stable code changed");
        const auto &location=field(diagnostic,"source");need(str(location,"uri")==in.uri&&str(location,"document_id")==in.id,"Diagnostic lost its authored source URI or document identity");}
    const auto maps=parse(d.maps);need(str(maps,"schema")=="frog.frontend.source_maps"&&str(maps,"schema_version")=="0.1","Source maps lost their public document family/version");const auto provenance=parse(d.provenance);need(str(provenance,"schema")=="frog.frontend.context"&&str(provenance,"schema_version")=="0.3","Provenance lost its explicit public document family/version");
    const auto &source=field(provenance,"source");need(str(source,"family")==in.format&&str(source,"version")==in.sourceVersion&&str(source,"uri")==in.uri&&str(source,"document_id")==in.id,"Provenance relabeled root format, identity or URI");
    need(str(field(provenance,"fir"),"document_id")==in.firId&&str(field(provenance,"fir"),"version")==in.firVersion&&str(field(provenance,"frontend"),"api_version")=="3","Provenance lost the independent requested FIR identity/version or API 3");
    const auto digest=str(source,"sha256");need(hashKnown?digest==frog::native_engine::hashing::sha256Hex(in.bytes):digest.empty(),"Captured source hash was changed or fabricated before capture");
    const bool resultLimit=std::any_of(diagnostics.array.begin(),diagnostics.array.end(),[](const auto &entry){return str(entry,"code")=="frontend.result_limit";});
    if(resultLimit)need(d.outcome==FROG_FRONTEND_LIMIT_EXCEEDED&&maps.object.size()==2,"An aggregate result refusal changed its bounded minimal source-map document");
    else need(str(maps,"authored_source_sha256")==digest,"Source maps lost the exact authored source hash");
    if(!d.resolved.empty()){need(d.outcome==FROG_FRONTEND_SUCCESS&&d.resolved!=in.bytes,"Resolved Source was returned outside a successful changed projection");static_cast<void>(parse(d.resolved));
        const auto projectionHash=frog::native_engine::hashing::sha256Hex(d.resolved);need(str(provenance,"projected_source_sha256")==projectionHash,"Resolved Source bytes differ from the declared projection hash");
        auto firSourceHash=str(maps,"fir_source_sha256");if(firSourceHash.starts_with("sha256:"))firSourceHash.erase(0,7);need(firSourceHash==projectionHash,"FIR maps refer to different bytes than the returned resolved Source");}
    if(d.outcome!=FROG_FRONTEND_SUCCESS){need(d.fir.empty()&&d.resolved.empty(),"A refused request returned partial executable FIR or resolved Source");need(!diagnostics.array.empty(),"Refusal returned no diagnostic");}
}
void outcome(const Documents &d,frog_frontend_outcome expected){if(d.outcome!=expected)std::cerr<<"FRONTEND_INTEGRATION_DIAGNOSTICS "<<d.diagnostics<<'\n';need(d.outcome==expected,"Unexpected real frontend outcome");}
Documents pair(Context &context,const Input &in,frog_frontend_outcome expected,bool hashKnown=true){const auto before=std::vector<std::string>{in.bytes,in.uri,in.id,in.firId,in.format,in.sourceVersion,in.firVersion,in.semantic};auto validated=call(context,in,false),derived=call(context,in,true);const auto v=validated.get(),d=derived.get();
    outcome(v,expected);outcome(d,expected);documents(v,in,hashKnown);documents(d,in,hashKnown);
    need(v.operation==FROG_FRONTEND_VALIDATE&&v.fir.empty()&&v.descriptor.empty()&&d.operation==FROG_FRONTEND_DERIVE&&d.descriptor.empty(),"Validate and derive changed their declared artifact boundary");
    need(cj::exact(parse(v.diagnostics),parse(d.diagnostics))&&v.resolved==d.resolved,"Validate used different semantic admission/diagnostics or resolved bytes from derive");need(std::vector<std::string>{in.bytes,in.uri,in.id,in.firId,in.format,in.sourceVersion,in.firVersion,in.semantic}==before,"An API operation changed caller-owned source, explicit context or identities");
    if(expected==FROG_FRONTEND_SUCCESS){need(!d.fir.empty(),"Successful derive omitted complete FIR");const auto ir=parse(d.fir);need(str(ir,"schema_family")=="frog.execution_ir"&&str(ir,"schema_version")=="0.1"&&str(ir,"document_id")==in.firId,"Derive silently selected another FIR format or replaced its explicit document identity");
        const auto maps=parse(d.maps);for(const auto &mapping:field(field(ir,"unit"),"source_map").array)need(hasString(maps,str(mapping,"source_ref")),"Separate source-map output omitted an actual FIR mapping");}
    return d;
}
const Json &sourceNode(const Json &source,std::string_view id){for(const auto &n:field(field(source,"diagram"),"nodes").array)if(str(n,"id")==id)return n;throw std::runtime_error("Missing actual source node identity");}
const Json &firNode(const Json &ir,std::string_view id){for(const auto &n:field(field(ir,"unit"),"objects").array)if(const auto *attributes=n.member("attributes"))if(const auto *source=attributes->member("source_node_id");source&&source->string==id)return n;throw std::runtime_error("FIR omitted exact authored node "+std::string(id));}
void fixtureInventory(const fs::path &directory){phase="exact public fixture inventory";const auto manifest=parse(read(directory/"provenance.json"));need(str(manifest,"format")=="frog.frontend.test-fixtures","Wrong integration fixture inventory");
    for(const auto &entry:field(manifest,"fixtures").array){const auto bytes=read(directory/str(entry,"file"));need(frog::native_engine::hashing::sha256Hex(bytes)==str(entry,"sha256"),"An identified production fixture snapshot changed");need(field(parse(bytes),"spec_version").type==Json::Type::String,"Positive corpus contains a Studio draft instead of public Source");}}
void typedCorpus(const fs::path &directory){phase="production Source corpus through API3";Context context;
    for(const auto *name:{"pure-add.frog","record-exact.frog","nominal-nested.frog","event-values.frog","pane-unbundle.frog","event-product.frog"}){
        phase=std::string("actual fixture ")+name;const auto in=input(directory,name);need(in.id!=in.firId,"The identity oracle must use different Source and FIR identities");const auto d=pair(context,in,FROG_FRONTEND_SUCCESS);need(d.resolved.empty(),"An unchanged actual fixture returned a redundant resolved Source");const auto source=parse(in.bytes),ir=parse(d.fir);
        if(std::string_view(name)=="record-exact.frog"){const auto &record=sourceNode(source,"record"),&actual=field(firNode(ir,"record"),"attributes");
            need(cj::exact(field(record,"value"),field(actual,"literal_value"))&&str(record,"type")==str(actual,"value_type"),"FIR changed exact nested Record literals or schema");
            const auto &value=field(actual,"literal_value");need(field(value,"u").string=="18446744073709551615"&&field(value,"i").string=="9007199254740993"&&field(value,"zero").string=="-0"&&field(field(value,"nested"),"answer").string=="-9007199254740993"&&field(value,"text").string==std::string("A\0B",3),"Exact I64/U64/negative-zero/NUL values were narrowed");}
        if(std::string_view(name)=="nominal-nested.frog"){for(const auto *id:{"operation-0-literal-map","operation-1-literal-map"}){const auto &original=sourceNode(source,id),&actual=field(firNode(ir,id),"attributes");
                need(cj::exact(field(original,"studio_type_schema"),field(actual,"studio_type_schema"))&&cj::exact(field(original,"value"),field(actual,"literal_value")),"Complete nominal/nested Map/Array/Cluster schema or exact data were replaced by physical carriers");}
            need(stringContains(ir,"test.collection.nonzero"),"FIR erased the nominal Enum domain identity");}
        if(std::string_view(name)=="event-values.frog"||std::string_view(name)=="pane-unbundle.frog"||std::string_view(name)=="event-product.frog"){
            const auto &regions=field(field(field(source,"diagram"),"nodes").array.at(1),"regions");need(field(field(ir,"unit"),"regions").array.size()==regions.array.size(),"Event derivation omitted nonvisible owned regions");
            for(std::size_t i=0;i<regions.array.size();++i)need(hasString(field(field(ir,"unit"),"source_map"),in.uri+"#/diagram/nodes/1/regions/"+std::to_string(i)),"A hidden Event region lost its exact authored source mapping");
            if(std::string_view(name)=="pane-unbundle.frog")need(stringContains(ir,"ref<\"frog.front_panel\">"),"Event/reference schemas were reduced to plain strings or numbers");}
    }
}
void refusals(const fs::path &directory){Context context;auto in=input(directory,"pure-add.frog");phase="malformed/unsupported public boundary";
    in.bytes="{";pair(context,in,FROG_FRONTEND_REJECTED);in=input(directory,"pure-add.frog");in.sourceVersion="unsupported-version";pair(context,in,FROG_FRONTEND_UNSUPPORTED);
    in=input(directory,"pure-add.frog");in.format="frog.document.future";in.sourceVersion="2";in.bytes=R"({"format":"frog.document.future","draft_revision":2,"diagram":{"nodes":[],"edges":[]},"frontPanel":{"widgets":[]}})";pair(context,in,FROG_FRONTEND_UNSUPPORTED);
    phase="hidden region remains validated";in=input(directory,"event-values.frog");auto source=parse(in.bytes);auto &regions=field(field(field(source,"diagram"),"nodes").array.at(1),"regions");const auto index=regions.array.size()-1;
    auto &edges=field(field(regions.array.back(),"diagram"),"edges");need(!edges.array.empty(),"Identified hidden-region negative has no actual edge");field(edges.array.front(),"to").object["node"]=cj::string("deliberately-absent-hidden-node");in.bytes=cj::encode(source);
    const auto d=pair(context,in,FROG_FRONTEND_REJECTED);const auto diagnostics=parse(d.diagnostics);const auto prefix="/diagram/nodes/1/regions/"+std::to_string(index)+"/diagram";
    need(std::any_of(diagnostics.array.begin(),diagnostics.array.end(),[&](const auto &entry){return str(field(entry,"source"),"pointer").starts_with(prefix);}),"Hidden-region refusal lost its exact authored location");
    phase="nominal domain mismatch stays invalid";in=input(directory,"nominal-nested.frog");source=parse(in.bytes);auto &literal=field(field(source,"diagram"),"nodes").array.at(2);
    need(str(literal,"id")=="operation-0-literal-default","Nominal negative lost its independently identified input");field(literal,"studio_type_schema").object["canonical"]=cj::string("enum<\"different.nominal.domain\",u32>");in.bytes=cj::encode(source);pair(context,in,FROG_FRONTEND_REJECTED);
}
void limitsAndOwnership(const fs::path &directory){phase="budget before capture and independent contexts";auto in=input(directory,"pure-add.frog");const auto before=in.bytes;frog_frontend_config limited{};limited.limits.max_source_bytes=in.bytes.size()-1;Context low(limited),normal;
    pair(low,in,FROG_FRONTEND_LIMIT_EXCEEDED,false);const auto baseline=pair(normal,in,FROG_FRONTEND_SUCCESS);limited={};limited.limits.max_source_bytes=in.bytes.size();Context exact(limited);pair(exact,in,FROG_FRONTEND_SUCCESS);
    limited={};limited.limits.max_nodes=3;Context nodes(limited);pair(nodes,in,FROG_FRONTEND_LIMIT_EXCEEDED);limited={};limited.limits.max_connections=2;Context connections(limited);pair(connections,in,FROG_FRONTEND_LIMIT_EXCEEDED);
    phase="metadata nodes and edges are not executable graph scopes";
    auto metadataInput=in;auto metadataSource=parse(metadataInput.bytes);auto &metadata=field(metadataSource,"metadata");
    auto metadataNodes=cj::array(),metadataEdges=cj::array();metadataNodes.array.resize(1025);metadataEdges.array.resize(4097);
    metadata.object["nodes"]=std::move(metadataNodes);metadata.object["edges"]=std::move(metadataEdges);metadataInput.bytes=cj::encode(metadataSource);
    std::string inspectionError;const auto inspected=frog::native_engine::FrogSourceDocument::read(metadataInput.bytes,&inspectionError);
    need(bool(inspected)&&inspected->envelopeStatus()==frog::native_engine::FrogSourceEnvelopeStatus::Recognized&&inspected->diagnostics().empty(),
        "The real Source reader did not admit the identified opaque metadata fixture: "+inspectionError);
    need(inspected->diagramScopes().size()==1&&inspected->diagramScopes().front().path=="/diagram",
        "Opaque metadata incorrectly became a declared Source graph scope");
    limited={};limited.limits.max_nodes=4;limited.limits.max_connections=3;Context exactGraph(limited);
    const auto metadataResult=pair(exactGraph,metadataInput,FROG_FRONTEND_SUCCESS);
    need(field(field(parse(metadataResult.fir),"unit"),"objects").array.size()==4&&field(field(parse(metadataResult.fir),"unit"),"connections").array.size()==3,
        "Graph-like metadata altered actual executable object/connection counts");
    need(str(field(parse(metadataResult.provenance),"source"),"sha256")==frog::native_engine::hashing::sha256Hex(metadataInput.bytes),
        "Opaque metadata was erased or excluded from the exact authored Source hash");
    phase="hidden graph scopes still consume the actual source node budget";
    auto hiddenInput=input(directory,"event-values.frog");limited={};limited.limits.max_nodes=2;Context hiddenGraph(limited);
    const auto hiddenResult=pair(hiddenGraph,hiddenInput,FROG_FRONTEND_LIMIT_EXCEEDED);
    const auto hiddenDiagnostics=parse(hiddenResult.diagnostics);
    need(std::any_of(hiddenDiagnostics.array.begin(),hiddenDiagnostics.array.end(),[](const auto &d){return str(d,"code")=="frontend.limit_exceeded"&&
        str(field(d,"source"),"pointer").starts_with("/diagram/nodes/1/regions/");}),
        "A hidden-region budget refusal did not identify the real declared inner graph scope");
    const auto again=pair(normal,in,FROG_FRONTEND_SUCCESS);need(baseline==again&&in.bytes==before,"Another context's resource failure changed deterministic valid results or input");in.semantic.clear();need(pair(normal,in,FROG_FRONTEND_SUCCESS)==baseline,"An empty optional semantic context changed the explicit {} contract");in.semantic="{}";
    phase="independent contexts on concurrent host threads";std::promise<void> start;const auto gate=start.get_future().share();
    const auto concurrent=[&]{gate.wait();Context own;return pair(own,in,FROG_FRONTEND_SUCCESS);};auto first=std::async(std::launch::async,concurrent),second=std::async(std::launch::async,concurrent);start.set_value();
    const auto firstResult=first.get(),secondResult=second.get();need(firstResult==baseline&&secondResult==baseline,"Concurrent independent contexts shared mutable operation state or changed deterministic results");
    phase="caller/context destruction does not destroy result views";Result retained;Documents saved;
    {Context temporary;auto temporaryInput=input(directory,"record-exact.frog");retained=call(temporary,temporaryInput,true);saved=retained.get();outcome(saved,FROG_FRONTEND_SUCCESS);temporaryInput.bytes.assign(temporaryInput.bytes.size(),'!');temporary.close();}
    need(retained.get()==saved&&!saved.fir.empty(),"Result views depend on destroyed caller input or API context");
    phase="cooperative cancellation is not invalid source";unsigned polls=0;in.cancelled=[](void *p)->std::uint32_t{++*static_cast<unsigned *>(p);return 1;};in.cancelUser=&polls;
    pair(normal,in,FROG_FRONTEND_CANCELLED,false);need(polls>=2&&in.bytes==before,"Cancellation skipped its callback or changed immutable bytes");in.cancelled=nullptr;in.cancelUser=nullptr;pair(normal,in,FROG_FRONTEND_SUCCESS);
    phase="explicit unsupported Conditional context cannot be ignored";in.semantic=R"({"profile":"frog.frontend.resolution@1","dependencies":[],"conditional_disable":{"symbols":["EXPLICIT_TEST_SYMBOL"]}})";
    const auto rejected=pair(normal,in,FROG_FRONTEND_UNSUPPORTED);const auto diagnostics=parse(rejected.diagnostics);
    need(std::any_of(diagnostics.array.begin(),diagnostics.array.end(),[](const auto &entry){return str(entry,"code")=="frontend.conditional_context_unsupported"&&str(entry,"stage")=="resolve"&&str(entry,"category")=="frontend-capability"&&str(field(entry,"source"),"pointer")=="/semantic_context/conditional_disable";}),"The unsupported explicit Conditional context was ignored or lost its exact diagnostic");
    in.semantic="{}";need(pair(normal,in,FROG_FRONTEND_SUCCESS)==baseline,"An explicit rejected context leaked into a later independent request");
}

// A deliberately hostile lifetime tap: every returned dependency buffer is
// overwritten and destroyed on release. The real core must capture it first.
struct Resolver {
    enum class Mode {Found,Missing,BadIdentity,BadDigest,Cancelled,RootUri};Mode mode=Mode::Found;
    std::string bytes,expectedDigest,rootUri;std::size_t calls=0,releases=0,active=0;bool validRequest=true,busyChecked=false;frog_frontend_context *context=nullptr;
    struct Lease {std::string bytes,uri="frog-integration:/types/nominal.json",id="integration-nominal-types",version="1";};
    static frog_frontend_resolve_status FROG_FRONTEND_CALL resolve(void *opaque,const frog_frontend_dependency_request *r,frog_frontend_resolved_dependency *out)noexcept {
        auto &s=*static_cast<Resolver *>(opaque);++s.calls;
        try{const auto pointer=r?copy(r->source_pointer):std::string{};s.validRequest=s.validRequest&&r&&r->struct_size==sizeof(*r)&&r->kind==FROG_FRONTEND_DEPENDENCY_TYPE_DEFINITION&&copy(r->requesting_source_uri)==s.rootUri&&(pointer=="/diagram/nodes/0/studio_ports/0/studio_type_schema"||pointer=="/diagram/nodes/0/studio_type_schema")&&copy(r->reference)=="dep:nominal-types"&&copy(r->expected_identity)=="integration-nominal-types"&&copy(r->expected_version)=="1"&&copy(r->expected_sha256)==s.expectedDigest;
            if(s.context){frog_frontend_result *nested=nullptr;s.busyChecked=frog_frontend_describe(s.context,&nested)==FROG_FRONTEND_BUSY&&!nested;frog_frontend_result_destroy(nested);}
            if(s.mode==Mode::Missing){out->error_message=span("Explicit test resolver has no snapshot.");return FROG_FRONTEND_NOT_FOUND;}
            if(s.mode==Mode::Cancelled)return FROG_FRONTEND_RESOLVE_CANCELLED;
            auto lease=std::make_unique<Lease>();lease->bytes=s.bytes;if(s.mode==Mode::BadIdentity)lease->id="wrong-actual-snapshot-identity";if(s.mode==Mode::BadDigest)lease->bytes+=' ';if(s.mode==Mode::RootUri)lease->uri=s.rootUri;
            out->source_uri=span(lease->uri);out->identity=span(lease->id);out->version=span(lease->version);out->bytes=span(lease->bytes);out->lease=lease.release();++s.active;return FROG_FRONTEND_RESOLVED;
        }catch(...){out->error_message=span("Host resolver could not return its test snapshot.");return FROG_FRONTEND_RESOLVE_ERROR;}}
    static void FROG_FRONTEND_CALL release(void *opaque,const frog_frontend_resolved_dependency *value)noexcept {auto &s=*static_cast<Resolver *>(opaque);++s.releases;if(value&&value->lease){auto *lease=static_cast<Lease *>(value->lease);std::fill(lease->bytes.begin(),lease->bytes.end(),'!');delete lease;--s.active;}}
    void reset(Mode next){need(active==0,"A dependency lease escaped the preceding operation");mode=next;calls=releases=0;validRequest=true;busyChecked=false;}
    void balanced(std::size_t expected=1)const {need(calls==expected&&releases==expected&&active==0&&validRequest,"Exact resolver requests/release count or snapshot lifetime changed");}
};
void dependencyFragments(const fs::path &directory){phase="real TypeDef fragment resolution through ABI3";Context context;auto in=input(directory,"nominal-nested.frog");auto source=parse(in.bytes);auto &first=field(field(source,"diagram"),"nodes").array.front();const auto originalSchema=field(first,"studio_type_schema");
    auto envelope=cj::object(),definitions=cj::object();definitions.object["T"]=originalSchema;envelope.object={{"profile",cj::string("frog.frontend.type-definitions@1")},{"identity",cj::string("integration-nominal-types")},{"version",cj::string("1")},{"definitions",std::move(definitions)}};
    Resolver resolver;resolver.bytes=cj::encode(envelope);resolver.expectedDigest=frog::native_engine::hashing::sha256Hex(resolver.bytes);resolver.rootUri=in.uri;resolver.context=context.value;
    auto reference=cj::object();reference.object={{"$ref",cj::string("dep:nominal-types")},{"identity",cj::string("integration-nominal-types")},{"version",cj::string("1")},{"sha256",cj::string(resolver.expectedDigest)},{"pointer",cj::string("/definitions/T")}};
    field(first,"studio_type_schema")=reference;field(field(first,"studio_ports").array.front(),"studio_type_schema")=reference;in.bytes=cj::encode(source);
    auto settings=cj::object(),records=cj::array();for(const auto *pointer:{"/diagram/nodes/0/studio_type_schema","/diagram/nodes/0/studio_ports/0/studio_type_schema"}){auto record=cj::object();record.object={{"kind",parse("2")},{"source_pointer",cj::string(pointer)}};records.array.push_back(std::move(record));}
    settings.object={{"profile",cj::string("frog.frontend.resolution@1")},{"dependencies",std::move(records)}};in.semantic=cj::encode(settings);in.resolver={&resolver,Resolver::resolve,Resolver::release};
    const auto authoredBytes=in.bytes,authoredContext=in.semantic,authoredId=in.id,authoredFirId=in.firId,authoredUri=in.uri;Documents projected;std::string validatedProjection;
    auto expectedProjection=source;auto &expectedFirst=field(field(expectedProjection,"diagram"),"nodes").array.front();
    field(expectedFirst,"studio_type_schema")=originalSchema;field(field(expectedFirst,"studio_ports").array.front(),"studio_type_schema")=originalSchema;
    for(const bool derive:{false,true}){resolver.reset(Resolver::Mode::Found);const auto result=call(context,in,derive);const auto d=result.get();outcome(d,FROG_FRONTEND_SUCCESS);documents(d,in);resolver.balanced();need(resolver.busyChecked,"Reentrant use of one busy API context was admitted");
        need(!d.resolved.empty()&&cj::exact(parse(d.resolved),expectedProjection),"Resolved Source changed fields outside the two declared schema projections or omitted its complete document");
        const auto provenance=parse(d.provenance);const auto &dependencies=field(provenance,"dependencies");need(dependencies.array.size()==1&&str(dependencies.array.front(),"identity")=="integration-nominal-types"&&str(dependencies.array.front(),"sha256")==resolver.expectedDigest,"Dependency closure was duplicated, relabeled or not bound to exact bytes");
        if(derive){const auto ir=parse(d.fir);need(str(ir,"document_id")==in.firId&&cj::exact(field(field(firNode(ir,"operation-0-literal-map"),"attributes"),"studio_type_schema"),originalSchema),"Resolved type fragment or explicit FIR identity did not enter the actual semantic/FIR producer");projected=d;need(d.resolved==validatedProjection,"Validate and derive returned different complete resolved Source bytes");}
        else {validatedProjection=d.resolved;need(d.fir.empty(),"Validation returned executable FIR instead of only the resolved Source");}}
    phase="returned resolved Source is the actual FIR input";auto resolvedInput=in;resolvedInput.bytes=projected.resolved;resolvedInput.semantic="{}";resolvedInput.resolver={};
    const auto rederived=pair(context,resolvedInput,FROG_FRONTEND_SUCCESS);need(rederived.fir==projected.fir&&rederived.resolved.empty(),"The returned projection cannot reproduce exact FIR without dependency lookup or caused a redundant projection");
    const auto reprovenance=parse(rederived.provenance);need(field(reprovenance,"dependencies").array.empty()&&field(reprovenance,"transformations").array.empty(),"The resolved standalone Source retained another request's resolution closure");
    phase="resolved Source participates in the complete result budget";need(projected.resolved.size()>256,"The identified TypeDef projection is too small for its discriminating result budget");
    const auto previousPayloadBytes=projected.fir.size()+projected.diagnostics.size()+projected.maps.size()+projected.provenance.size()+projected.descriptor.size();
    frog_frontend_config resultBudget{};resultBudget.limits.max_result_bytes=previousPayloadBytes+projected.resolved.size()/2;Context boundedResult(resultBudget);resolver.context=boundedResult.value;resolver.reset(Resolver::Mode::Found);
    const auto tooLarge=call(boundedResult,in,true).get();outcome(tooLarge,FROG_FRONTEND_LIMIT_EXCEEDED);documents(tooLarge,in);resolver.balanced();
    const auto budgetDiagnostics=parse(tooLarge.diagnostics);need(std::any_of(budgetDiagnostics.array.begin(),budgetDiagnostics.array.end(),[](const auto &d){return str(d,"code")=="frontend.result_limit";}),"The new projection bytes were not counted at the aggregate result boundary");
    phase="resolved Source result survives input and context destruction";Result retained;Documents retainedDocuments;
    {Context temporary;resolver.context=temporary.value;resolver.reset(Resolver::Mode::Found);auto temporaryInput=in;retained=call(temporary,temporaryInput,true);retainedDocuments=retained.get();outcome(retainedDocuments,FROG_FRONTEND_SUCCESS);resolver.balanced();
        temporaryInput.bytes.assign(temporaryInput.bytes.size(),'!');temporaryInput.semantic.assign(temporaryInput.semantic.size(),'!');temporary.close();}
    need(retained.get()==retainedDocuments&&retainedDocuments.resolved==projected.resolved&&!retainedDocuments.resolved.empty(),"The resolved Source view depended on a released host snapshot, input or context");resolver.context=context.value;
    for(const auto mode:{Resolver::Mode::Missing,Resolver::Mode::BadIdentity,Resolver::Mode::BadDigest,Resolver::Mode::Cancelled}){resolver.reset(mode);const auto d=call(context,in,true).get();outcome(d,mode==Resolver::Mode::Cancelled?FROG_FRONTEND_CANCELLED:FROG_FRONTEND_REJECTED);documents(d,in);resolver.balanced();}
    phase="TypeDef URI cannot alias the authoritative root Source";resolver.reset(Resolver::Mode::RootUri);
    const auto rootAlias=call(context,in,true).get();outcome(rootAlias,FROG_FRONTEND_REJECTED);documents(rootAlias,in);resolver.balanced();
    const auto aliasDiagnostics=parse(rootAlias.diagnostics);
    need(std::any_of(aliasDiagnostics.array.begin(),aliasDiagnostics.array.end(),[&](const auto &d){return str(d,"code")=="frontend.dependency_uri_conflict"&&
        str(d,"category")=="dependency"&&str(field(d,"source"),"uri")==in.uri&&
        str(field(d,"source"),"pointer").starts_with("/diagram/nodes/0/");}),
        "A root/dependency URI collision lost its exact attributable dependency diagnostic");
    resolver.reset(Resolver::Mode::Found);frog_frontend_config config{};config.limits.max_dependency_bytes=resolver.bytes.size()-1;Context limited(config);resolver.context=limited.value;const auto d=call(limited,in,true).get();outcome(d,FROG_FRONTEND_LIMIT_EXCEEDED);documents(d,in);resolver.balanced();
    resolver.reset(Resolver::Mode::Found);in.resolver={};const auto missing=call(context,in,true).get();outcome(missing,FROG_FRONTEND_REJECTED);documents(missing,in);resolver.balanced(0);
    need(in.bytes==authoredBytes&&in.semantic==authoredContext&&in.id==authoredId&&in.firId==authoredFirId&&in.uri==authoredUri,"Type fragment projection or rejection mutated authored Source/context bytes or explicit identities");
}
void preparedEventProjection(const fs::path &directory) {
    phase="immutable admitted Event projection without source rederivation";
    const auto bytes=read(directory/"event-product.frog");std::string error;
    auto source=frog::native_engine::FrogSourceDocument::read(bytes,&error);
    need(bool(source),"Cannot read the real Event fixture");
    frog::native_engine::CanonicalFrontendMetrics metrics;
    frog::native_engine::CanonicalIrReadOptions options;options.sourceUri="fixture:/event-product.frog";options.metrics=&metrics;
    std::vector<frog::native_engine::CanonicalIrDiagnostic> diagnostics;
    auto event=frog::native_engine::CanonicalEventExecutionIr::derive(*source,"ir:projection",options,&diagnostics);
    need(bool(event),"Real Event producer refused fixture");
    const auto semantics=metrics.semanticCalls,construction=metrics.firConstructionCalls;
    const auto exactEvent=std::string(event->originalUtf8());
    for(const auto &region:event->regions()) {
        auto body=frog::native_engine::CanonicalExecutionIr::deriveEventRegion(*event,region.sourceRegionId,options,&diagnostics);
        need(bool(body),"Admitted Event body projection failed");
        need(body->sourceHash()==event->sourceHash(),"Body changed its source identity");
    }
    need(metrics.semanticCalls==semantics&&metrics.firConstructionCalls==construction&&metrics.sealingCalls==0,
        "Admitted Event was reparsed or rederived once per body");
    need(event->originalUtf8()==exactEvent,"Projection mutated its immutable Event owner");
    options.limits.maximumNodes=1;
    auto refused=frog::native_engine::CanonicalExecutionIr::deriveEventRegion(*event,event->regions().front().sourceRegionId,options,&diagnostics);
    need(!refused&&!diagnostics.empty(),"A reduced budget bypassed complete Event admission");
    options.limits={};options.sourceUri="fixture:/different.frog";
    need(!frog::native_engine::CanonicalExecutionIr::deriveEventRegion(*event,event->regions().front().sourceRegionId,options,&diagnostics),
        "A projection accepted a different Source identity");
}
void benchmark(const fs::path &directory,unsigned iterations){phase="total API-call benchmark";const auto in=input(directory,"pure-add.frog");Context context;std::int64_t validateNs=0,deriveNs=0;
    for(unsigned i=0;i<iterations;++i)for(const bool derive:{false,true}){const auto start=std::chrono::steady_clock::now();auto result=call(context,in,derive);const auto stop=std::chrono::steady_clock::now();outcome(result.get(),FROG_FRONTEND_SUCCESS);const auto ns=std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start).count();(derive?deriveNs:validateNs)+=ns;}
    // The ABI exposes complete calls, not independent internal phase clocks.
    // In particular validate uses the same FIR admission as derive.
    std::cout<<"FRONTEND_BENCH {\"iterations\":"<<iterations<<",\"source_bytes\":"<<in.bytes.size()<<",\"validate_total_ns\":"<<validateNs<<",\"derive_total_ns\":"<<deriveNs<<",\"internal_phase_split\":false,\"timing_scope\":\"complete_api_call\"}\n";
}
}
int main(int argc,char **argv){try{
#ifdef FROG_FRONTEND_FIXTURE_ROOT
    fs::path directory=fs::u8path(FROG_FRONTEND_FIXTURE_ROOT);
#else
    fs::path directory=fs::path(__FILE__).parent_path()/"fixtures";
#endif
    unsigned bench=0;int i=1;if(i<argc&&std::string_view(argv[i])!="--bench")directory=fs::u8path(argv[i++]);
    if(i<argc){need(i+2==argc&&std::string_view(argv[i])=="--bench","Usage: frontend-integration [FIXTURE_DIRECTORY] [--bench 1..100]");bench=static_cast<unsigned>(std::stoul(argv[i+1]));need(bench>0&&bench<=100,"Benchmark iteration count is outside its bounded range");}
    need(frog_frontend_api_version()==3&&FROG_FRONTEND_API_VERSION==3,"Integration must use the real API3 library");fixtureInventory(directory);typedCorpus(directory);refusals(directory);limitsAndOwnership(directory);dependencyFragments(directory);preparedEventProjection(directory);if(bench)benchmark(directory,bench);
    std::cout<<"FRONTEND_INTEGRATION_PASS checks="<<checks.load()<<" api=3 production_public_fixtures=6 exact_i64_u64=1 nested_nominal=1 hidden_event_regions=1 real_dependency_fragment=1 resolved_source_lifetime=1 resolved_source_budget=1 distinct_source_fir_ids=1 lease_lifetime=1 independent_contexts=1 concurrent_contexts=2 compiler_api_calls=0 runtime_api_calls=0\n";return 0;
}catch(const std::exception &error){std::cerr<<"FRONTEND_INTEGRATION_FAIL "<<error.what()<<" checks="<<checks.load()<<'\n';return 1;}}
