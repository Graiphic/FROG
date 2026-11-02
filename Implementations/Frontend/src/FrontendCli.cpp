// SPDX-License-Identifier: Apache-2.0
// File I/O and dependency snapshots belong to this explicit command-line host.
// All Source admission and FIR generation go through the public C ABI.
#include <frog/frontend.h>
#include "frog_native_engine/FrogJson.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <atomic>
#include <charconv>
#include <chrono>
#include <codecvt>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cwctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <initializer_list>
#include <locale>
#include <map>
#include <new>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {
namespace fs=std::filesystem;
namespace cj=frog::native_engine::canonical_json;
using Json=frog::native_engine::source::JsonValue;
struct Failure:std::runtime_error {int code;Failure(int c,std::string text):std::runtime_error(std::move(text)),code(c){}};
volatile std::sig_atomic_t interrupted=0;
void interrupt(int) noexcept {interrupted=1;}
frog_frontend_utf8 span(std::string_view s) noexcept {return {s.data(),s.size()};}
std::string_view text(frog_frontend_utf8 s) {if(s.size&&!s.data)throw Failure(64,"The frontend returned an invalid result span.");return {s.data?s.data:"",s.size};}
fs::path path(std::string_view s) {if(s.empty()||s.find('\0')!=s.npos)throw Failure(64,"An explicit file path is empty or contains NUL.");return fs::u8path(s.begin(),s.end());}
std::string read(const fs::path &p,std::uint64_t maximum=64U*1024U*1024U) {
    std::error_code ec;const auto size=fs::file_size(p,ec);if(ec)throw Failure(64,"Cannot read the explicitly selected file: "+p.generic_string());
    if(size>maximum)throw Failure(5,"The explicitly selected file exceeds the CLI host's bounded read budget.");
    std::ifstream f(p,std::ios::binary);if(!f)throw Failure(64,"Cannot open the explicitly selected file: "+p.generic_string());
    std::string bytes(static_cast<std::size_t>(size),'\0');if(size)f.read(bytes.data(),static_cast<std::streamsize>(size));
    if(!f||f.peek()!=std::char_traits<char>::eof())throw Failure(64,"The explicitly selected file changed or could not be read completely.");return bytes;
}
Json json(std::string_view bytes) {std::string error;auto value=frog::native_engine::source::parseJson(bytes,&error,frog::native_engine::source::JsonNumberPolicy::PreserveLexeme);
    if(!value)throw Failure(64,"Invalid dependency-lock JSON: "+error);return std::move(*value);}
const Json &member(const Json &v,const char *key) {const auto *p=v.member(key);if(v.type!=Json::Type::Object||!p)throw Failure(64,std::string("Dependency lock is missing ")+key);return *p;}
std::string string(const Json &v,const char *key,bool empty=false) {const auto &p=member(v,key);if(p.type!=Json::Type::String||(!empty&&p.string.empty())||p.string.find('\0')!=p.string.npos)throw Failure(64,std::string("Invalid dependency-lock string: ")+key);return p.string;}
void fields(const Json &v,std::initializer_list<std::string_view> allowed) {if(v.type!=Json::Type::Object)throw Failure(64,"A dependency-lock record must be an object.");
    for(const auto &[key,unused]:v.object){(void)unused;bool found=false;for(const auto field:allowed)if(field==key)found=true;if(!found)throw Failure(64,"Unknown dependency-lock member: "+key);}}
std::uint64_t integer(std::string_view value) {std::uint64_t n=0;const auto p=std::from_chars(value.data(),value.data()+value.size(),n);
    if(value.empty()||p.ec!=std::errc{}||p.ptr!=value.data()+value.size())throw Failure(64,"A limit must be an unsigned decimal integer.");return n;}
fs::path identity(const fs::path &p) {std::error_code ec;auto result=fs::weakly_canonical(fs::absolute(p),ec);if(ec)throw Failure(64,"Cannot resolve an explicitly selected path.");return result;}
bool samePath(const fs::path &a,const fs::path &b) {
    const auto x=identity(a),y=identity(b);std::error_code ec;if(fs::equivalent(x,y,ec))return true;
#ifdef _WIN32
    auto left=x.native(),right=y.native();for(auto &ch:left)ch=static_cast<wchar_t>(std::towlower(ch));for(auto &ch:right)ch=static_cast<wchar_t>(std::towlower(ch));return left==right;
#else
    return x==y;
#endif
}
struct Snapshot {frog_frontend_dependency_kind kind{};std::string reference,ownerUri,uri,id,version,bytes;fs::path file;std::size_t leases=0;};
struct Dependencies {
    std::vector<Snapshot> snapshots;std::size_t resolves=0,releases=0;bool badRelease=false;
    static frog_frontend_resolve_status FROG_FRONTEND_CALL resolve(void *opaque,const frog_frontend_dependency_request *request,frog_frontend_resolved_dependency *out) noexcept {
        auto &self=*static_cast<Dependencies *>(opaque);++self.resolves;
        if(!request||!out||request->struct_size!=sizeof(*request)){if(out)out->error_message=span("Invalid resolver request size.");return FROG_FRONTEND_RESOLVE_ERROR;}
        try {for(auto &s:self.snapshots)if(s.kind==request->kind&&s.reference==text(request->reference)&&(s.ownerUri.empty()||s.ownerUri==text(request->requesting_source_uri))){
                out->source_uri=span(s.uri);out->identity=span(s.id);out->version=span(s.version);out->bytes=span(s.bytes);out->lease=&s;++s.leases;return FROG_FRONTEND_RESOLVED;}
            out->error_message=span("The explicit dependency lock has no matching kind/reference/owner snapshot.");return FROG_FRONTEND_NOT_FOUND;
        } catch(...){out->error_message=span("The dependency resolver could not supply its immutable snapshot.");return FROG_FRONTEND_RESOLVE_ERROR;}
    }
    static void FROG_FRONTEND_CALL release(void *opaque,const frog_frontend_resolved_dependency *value) noexcept {
        auto &self=*static_cast<Dependencies *>(opaque);++self.releases;if(!value){self.badRelease=true;return;}if(!value->lease)return;
        for(auto &s:self.snapshots)if(value->lease==&s){if(!s.leases)self.badRelease=true;else --s.leases;return;}self.badRelease=true;
    }
    void balanced()const {if(resolves!=releases||badRelease)throw Failure(64,"The frontend did not balance dependency resolver/release calls.");for(const auto &s:snapshots)if(s.leases)throw Failure(64,"The frontend retained a dependency lease after returning.");}
};
Dependencies loadDependencies(const fs::path &lock,std::vector<fs::path> &inputs) {
    const auto root=json(read(lock,8U*1024U*1024U));fields(root,{"format","version","dependencies"});
    if(string(root,"format")!="frog.frontend.dependencies"||string(root,"version")!="1")throw Failure(64,"Unsupported dependency-lock format/version.");
    const auto &entries=member(root,"dependencies");if(entries.type!=Json::Type::Array||entries.array.size()>4096)throw Failure(64,"Dependency lock must contain a bounded dependency array.");
    Dependencies result;result.snapshots.reserve(entries.array.size());std::uint64_t total=0;
    const auto base=fs::absolute(lock).parent_path();
    for(const auto &entry:entries.array){fields(entry,{"kind","reference","requesting_source_uri","source_uri","identity","version","path"});const auto &kind=member(entry,"kind");
        if(kind.type!=Json::Type::Number)throw Failure(64,"Dependency kind must be the exact integer 1, 2, 3 or 4.");const auto k=integer(kind.string);if(k<1||k>4)throw Failure(64,"Dependency kind is outside 1..4.");
        Snapshot s;s.kind=static_cast<frog_frontend_dependency_kind>(k);s.reference=string(entry,"reference");s.uri=string(entry,"source_uri");s.id=string(entry,"identity");s.version=string(entry,"version");
        if(entry.member("requesting_source_uri"))s.ownerUri=string(entry,"requesting_source_uri");
        s.file=path(string(entry,"path"));if(s.file.is_relative())s.file=base/s.file;s.file=identity(s.file);
        for(const auto &prior:result.snapshots)if(prior.kind==s.kind&&prior.reference==s.reference&&(prior.ownerUri.empty()||s.ownerUri.empty()||prior.ownerUri==s.ownerUri))throw Failure(64,"Ambiguous kind/reference/owner records in the dependency lock.");
        s.bytes=read(s.file);if(s.bytes.size()>256U*1024U*1024U-total)throw Failure(5,"Dependency lock exceeds the CLI host's bounded snapshot budget.");total+=s.bytes.size();inputs.push_back(s.file);result.snapshots.push_back(std::move(s));
    }return result;
}
// Publish a complete sibling file only after its bytes have been flushed.
// Failed writes and failed derivations leave an existing FIR untouched.
void atomicWrite(const fs::path &destination,std::string_view bytes) {
    static std::atomic<unsigned long> serial{0};fs::path directory;bool reserved=false;
    for(unsigned attempt=0;attempt<100&&!reserved;++attempt){directory=destination;directory+=".frog-front-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())+"-"+std::to_string(++serial)+".tmp";
        std::error_code ec;reserved=fs::create_directory(directory,ec);if(ec)throw Failure(64,"Cannot reserve a sibling output directory.");}
    if(!reserved)throw Failure(64,"Cannot reserve a distinct sibling output directory.");
    struct Pending {fs::path directory,file;~Pending(){std::error_code ec;fs::remove(file,ec);fs::remove(directory,ec);}} pending{directory,directory/"payload"};
    std::ofstream stream(pending.file,std::ios::binary|std::ios::trunc);if(!stream)throw Failure(64,"Cannot create a sibling output payload.");
    stream.write(bytes.data(),static_cast<std::streamsize>(bytes.size()));stream.flush();const bool flushed=bool(stream);stream.close();
    if(!flushed||!stream)throw Failure(64,"Cannot write a complete output payload.");
    // Standard rename owns atomic replacement on the host platform. Never
    // remove the existing destination as a fallback when replacement fails.
    std::error_code ec;fs::rename(pending.file,destination,ec);if(ec)throw Failure(64,"Cannot publish a complete output file.");
}
void usage() {std::cerr<<"Usage: frog-frontend describe [--descriptor-output FILE]\n"
    <<"       frog-frontend validate|derive --source FILE --source-uri URI --document-id SOURCE_ID\n"
    <<"         --fir-document-id FIR_ID\n"
    <<"         --source-format ID --source-version VERSION --fir-version VERSION\n"
    <<"         [--semantic-context FILE] [--dependencies LOCK.json]\n"
    <<"         [--diagnostics-output FILE] [--source-maps-output FILE] [--provenance-output FILE]\n"
    <<"         derive requires --output FILE. Limits use --max-source-bytes, --max-dependency-bytes,\n"
    <<"         --max-total-dependency-bytes, --max-result-bytes, --max-nodes, --max-connections,\n"
    <<"         --max-nesting-depth, --max-dependencies, --max-diagnostics. Ctrl+C cancels cooperatively.\n";}
struct Context {frog_frontend_context *value=nullptr;~Context(){frog_frontend_context_destroy(value);}};
struct Result {frog_frontend_result *value=nullptr;~Result(){frog_frontend_result_destroy(value);}};
int run(const std::vector<std::string> &args) {
    if(args.size()==2&&(args[1]=="--help"||args[1]=="-h")){usage();return 0;}
    if(args.size()<2)throw Failure(64,"An explicit operation is required.");const auto &operation=args[1];
    if(operation!="describe"&&operation!="validate"&&operation!="derive")throw Failure(64,"Unknown frontend operation.");
    frog_frontend_config config{};config.struct_size=sizeof(config);std::map<std::string,std::uint64_t *> limits{
        {"--max-source-bytes",&config.limits.max_source_bytes},{"--max-dependency-bytes",&config.limits.max_dependency_bytes},
        {"--max-total-dependency-bytes",&config.limits.max_total_dependency_bytes},{"--max-result-bytes",&config.limits.max_result_bytes},
        {"--max-nodes",&config.limits.max_nodes},{"--max-connections",&config.limits.max_connections},{"--max-nesting-depth",&config.limits.max_nesting_depth},
        {"--max-dependencies",&config.limits.max_dependencies},{"--max-diagnostics",&config.limits.max_diagnostics}};
    const std::vector<std::string> names{"--source","--source-uri","--document-id","--fir-document-id","--source-format","--source-version","--fir-version","--output","--semantic-context","--dependencies","--diagnostics-output","--source-maps-output","--provenance-output","--descriptor-output"};
    std::map<std::string,std::string> values;for(std::size_t i=2;i<args.size();++i){const auto &key=args[i];if(i+1>=args.size()||values.contains(key))throw Failure(64,"Missing value or duplicate option: "+key);
        if(const auto found=limits.find(key);found!=limits.end()){*found->second=integer(args[++i]);values.emplace(key,args[i]);continue;}
        bool known=false;for(const auto &name:names)if(key==name)known=true;if(!known)throw Failure(64,"Unknown frontend option: "+key);
        const auto &value=args[++i];if(value.empty())throw Failure(64,"An option value is empty: "+key);values.emplace(key,value);}
    const auto has=[&](const char *key){return values.contains(key);};
    if(operation=="describe"){for(const auto *key:{"--source","--source-uri","--document-id","--fir-document-id","--source-format","--source-version","--fir-version","--output","--semantic-context","--dependencies"})if(has(key))throw Failure(64,"Describe does not consume Source or dependencies.");}
    else {for(const auto *key:{"--source","--source-uri","--document-id","--fir-document-id","--source-format","--source-version","--fir-version"})if(!has(key))throw Failure(64,std::string("Required option missing: ")+key);
        if(has("--descriptor-output"))throw Failure(64,"Only describe can emit a descriptor.");if((operation=="derive")!=has("--output"))throw Failure(64,"Derive requires --output; validate does not emit FIR.");}
    std::vector<fs::path> inputs;std::string source,semantic;Dependencies dependencies;
    if(operation!="describe"){const auto sourcePath=path(values.at("--source"));inputs.push_back(sourcePath);source=read(sourcePath);
        if(has("--semantic-context")){const auto p=path(values.at("--semantic-context"));inputs.push_back(p);semantic=read(p,8U*1024U*1024U);}
        if(has("--dependencies")){const auto p=path(values.at("--dependencies"));inputs.push_back(p);dependencies=loadDependencies(p,inputs);}}
    std::vector<fs::path> outputs;for(const auto *key:{"--output","--diagnostics-output","--source-maps-output","--provenance-output","--descriptor-output"})if(has(key)){const auto p=path(values.at(key));
        for(const auto &input:inputs)if(samePath(p,input))throw Failure(64,"An output aliases an immutable input file.");for(const auto &prior:outputs)if(samePath(p,prior))throw Failure(64,"Output files must have distinct paths.");outputs.push_back(p);}
    Context context;const auto version=frog_frontend_api_version();if(version!=FROG_FRONTEND_API_VERSION)throw Failure(64,"The linked frontend API version differs from this CLI.");
    const auto created=frog_frontend_context_create(version,&config,&context.value);if(created!=FROG_FRONTEND_OK||!context.value)throw Failure(created==FROG_FRONTEND_OUT_OF_MEMORY?5:64,"Cannot create the actual frontend context.");
    std::signal(SIGINT,interrupt);Result result;frog_frontend_status status;
    if(operation=="describe")status=frog_frontend_describe(context.value,&result.value);
    else {frog_frontend_request request{};request.struct_size=sizeof(request);request.source_bytes=span(source);request.source_uri=span(values.at("--source-uri"));request.source_document_id=span(values.at("--document-id"));request.fir_document_id=span(values.at("--fir-document-id"));
        request.source_format_id=span(values.at("--source-format"));request.source_format_version=span(values.at("--source-version"));request.fir_format_version=span(values.at("--fir-version"));request.semantic_context_json=span(semantic);
        request.is_cancelled=[](void *)noexcept->std::uint32_t{return interrupted?1U:0U;};if(has("--dependencies"))request.resolver={&dependencies,Dependencies::resolve,Dependencies::release};
        status=operation=="derive"?frog_frontend_derive(context.value,&request,&result.value):frog_frontend_validate(context.value,&request,&result.value);}
    dependencies.balanced();if(status!=FROG_FRONTEND_OK||!result.value)throw Failure(status==FROG_FRONTEND_OUT_OF_MEMORY?5:64,"The frontend API operation failed (status "+std::to_string(status)+").");
    frog_frontend_result_view view{};view.struct_size=sizeof(view);if(frog_frontend_result_get(result.value,&view)!=FROG_FRONTEND_OK)throw Failure(64,"Cannot read the immutable frontend result.");
    const auto fir=text(view.fir_json),diagnostics=text(view.diagnostics_json),maps=text(view.source_maps_json),provenance=text(view.provenance_json),descriptor=text(view.descriptor_json),resolved=text(view.resolved_source_json);
    const auto expectedOperation=operation=="describe"?FROG_FRONTEND_DESCRIBE:operation=="derive"?FROG_FRONTEND_DERIVE:FROG_FRONTEND_VALIDATE;
    if(view.operation!=expectedOperation||diagnostics.empty()||maps.empty()||provenance.empty()||
        ((view.outcome!=FROG_FRONTEND_SUCCESS||operation!="derive")&&!fir.empty())||((view.outcome!=FROG_FRONTEND_SUCCESS||operation!="describe")&&!descriptor.empty())||
        ((view.outcome!=FROG_FRONTEND_SUCCESS||operation=="describe")&&!resolved.empty()))
        throw Failure(64,"The frontend returned incomplete result documents, a wrong operation or an undeclared artifact.");
    const auto publish=[&](const char *key,std::string_view bytes){if(has(key)){if(bytes.empty())throw Failure(64,"The frontend omitted a requested result document.");atomicWrite(path(values.at(key)),bytes);}};
    publish("--diagnostics-output",diagnostics);publish("--source-maps-output",maps);publish("--provenance-output",provenance);
    if(view.outcome==FROG_FRONTEND_SUCCESS){if(operation=="derive"){if(fir.empty())throw Failure(64,"Successful derive returned no FIR.");publish("--output",fir);}
        else if(operation=="describe"){if(descriptor.empty())throw Failure(64,"Successful describe returned no descriptor.");publish("--descriptor-output",descriptor);std::cout<<descriptor<<'\n';}
        if(operation!="describe")std::cout<<diagnostics<<'\n';return 0;}
    std::cout<<diagnostics<<'\n';switch(view.outcome){case FROG_FRONTEND_REJECTED:return 2;case FROG_FRONTEND_UNSUPPORTED:return 3;case FROG_FRONTEND_CANCELLED:return 4;case FROG_FRONTEND_LIMIT_EXCEEDED:return 5;default:throw Failure(64,"The frontend returned an unknown outcome.");}
}
int entry(const std::vector<std::string> &args) {try{return run(args);}catch(const Failure &e){std::cerr<<"frog-frontend: "<<e.what()<<'\n';if(e.code==64)usage();return e.code;}catch(const std::bad_alloc &){std::cerr<<"frog-frontend: host allocation limit\n";return 5;}catch(const std::exception &e){std::cerr<<"frog-frontend: "<<e.what()<<'\n';return 64;}catch(...){std::cerr<<"frog-frontend: internal host failure\n";return 64;}}
}
#ifdef _WIN32
int wmain(int argc,wchar_t **argv) {try{std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> utf8;std::vector<std::string> args;for(int i=0;i<argc;++i)args.push_back(utf8.to_bytes(argv[i]));return entry(args);}catch(...){return 64;}}
#else
int main(int argc,char **argv) {std::vector<std::string> args;for(int i=0;i<argc;++i)args.emplace_back(argv[i]);return entry(args);}
#endif
