#include "ReferenceC.hpp"
#include "Profile.hpp"
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#include <array>
#include <iostream>
#include <set>

namespace reference_c {
namespace {
constexpr std::size_t maximumFrame=16U*1024U*1024U;
bool readExact(HANDLE file,char*data,std::size_t bytes,bool first=false){std::size_t at=0;while(at<bytes){DWORD n=0;if(!ReadFile(file,data+at,static_cast<DWORD>(bytes-at),&n,nullptr)){if(first&&at==0&&(GetLastError()==ERROR_BROKEN_PIPE||GetLastError()==ERROR_HANDLE_EOF))return false;fail("reference_c.transport_read","Cannot read bridge frame.");}if(!n){if(first&&at==0)return false;fail("reference_c.transport_truncated","Truncated bridge frame.");}at+=n;}return true;}
std::optional<std::string> frame(){std::array<char,4> prefix{};if(!readExact(GetStdHandle(STD_INPUT_HANDLE),prefix.data(),4,true))return{};std::uint32_t size=0;for(unsigned i=0;i<4;++i)size|=static_cast<std::uint32_t>(static_cast<unsigned char>(prefix[i]))<<(8*i);if(!size||size>maximumFrame)fail("reference_c.frame_limit","Bridge frame is empty or exceeds16MiB.");std::string bytes(size,'\0');readExact(GetStdHandle(STD_INPUT_HANDLE),bytes.data(),bytes.size());return bytes;}
void send(const b::Message&m){b::Error error;if(!b::validatePayload(m,error))fail("reference_c.response_contract",error.message);const auto bytes=b::encodeMessage(m);if(bytes.size()>maximumFrame)fail("reference_c.frame_limit","Response exceeds16MiB.");std::string prefix(4,'\0');for(unsigned i=0;i<4;++i)prefix[i]=static_cast<char>((bytes.size()>>(8*i))&255);for(const auto part:{std::string_view(prefix),std::string_view(bytes)}){std::size_t at=0;while(at<part.size()){DWORD n=0;if(!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),part.data()+at,static_cast<DWORD>(part.size()-at),&n,nullptr)||!n)fail("reference_c.transport_write","Cannot publish bridge frame.");at+=n;}}}
std::string base(std::string_view method){return "{\"schema\":\"frog.bridge.payload\",\"schema_version\":\"0.1\",\"method\":"+encode(method)+",\"phase\":\"response\"";}
std::string handle(std::string_view token){return "{\"token\":"+encode(token)+"}";}
std::string runHandle(std::string_view token,const b::Identity&id){return "{\"token\":"+encode(token)+",\"session_id\":"+encode(id.sessionId)+",\"generation\":"+std::to_string(id.generation)+"}";}
std::string receipt(std::string_view token,const b::GraphRequirements&r){return "{\"token\":"+encode(token)+",\"fir_sha256\":"+encode(r.firSha256)+",\"context_sha256\":"+encode(r.contextSha256)+",\"engine_id\":"+encode(r.engineId)+",\"target_id\":"+encode(r.targetId)+",\"profile_id\":"+encode(r.profileId)+"}";}
bool sameGraph(const b::GraphRequirements&a,const b::GraphRequirements&z){return a.firSha256==z.firSha256&&a.contextSha256==z.contextSha256&&a.engineId==z.engineId&&a.targetId==z.targetId&&a.profileId==z.profileId&&a.documentId==z.documentId&&a.frontendFingerprint==z.frontendFingerprint;}
std::string firBytes(const J&p){const auto value=field(field(p,"fir"),"utf8");if(value.kind()!=j::Kind::String||value.string().empty()||value.string().size()>4U*1024U*1024U)fail("reference_c.fir_limit","Exact FIR text must contain1..4194304 bytes.");return std::string(value.string());}
struct Temporary {
    std::filesystem::path directory;
    Temporary(){wchar_t root[32768]{};const DWORD n=GetTempPathW(32768,root);if(!n||n>=32768)fail("reference_c.temporary","Cannot locate bounded process workspace.");directory=std::filesystem::path(root)/(L"frog-reference-c-"+wide(nonce()));if(!std::filesystem::create_directory(directory))fail("reference_c.temporary","Cannot create unique process workspace.");}
    ~Temporary(){std::error_code ignored;std::filesystem::remove_all(directory,ignored);}
};
struct Graph {
    b::Identity owner;
    Program program;
    std::string token,compilerSha,sourceSha,codeSha,lowered,artifact,exeSha;
    std::vector<std::filesystem::path> files;
    std::optional<Temporary> workspace;
};
std::string exportedArtifacts(const Graph&g){
    constexpr std::size_t maximum=4U*1024U*1024U;std::size_t total=0;std::set<std::filesystem::path> seen;std::string out="[";bool first=true;if(!g.workspace)fail("reference_c.artifacts_unavailable","This graph has no generated artifacts.");
    for(const auto &path:g.files){if(!seen.insert(path).second)continue;if(seen.size()>64)fail("reference_c.artifact_limit","Qualification export exceeds64 owned artifacts.");if(path.parent_path().lexically_normal()!=g.workspace->directory.lexically_normal()||path.filename()!=path.lexically_relative(g.workspace->directory))fail("reference_c.artifact_ownership","An artifact escaped its creating peer workspace.");const auto name=utf8(path.filename());const bool source=name=="kernel.c",executable=name=="kernel.exe",command=name.ends_with(".command.json"),result=name.ends_with(".result.json"),log=name.ends_with(".stdout")||name.ends_with(".stderr");if(!source&&!executable&&!command&&!result&&!log)fail("reference_c.artifact_ownership","A file is outside the finite artifact export set.");const DWORD attrs=GetFileAttributesW(path.c_str());if(attrs==INVALID_FILE_ATTRIBUTES||(attrs&(FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_REPARSE_POINT)))fail("reference_c.artifact_ownership","An owned artifact is no longer a regular local file.");const auto bytes=std::filesystem::file_size(path);if(bytes>maximum-total)fail("reference_c.artifact_limit","Qualification artifact export exceeds4MiB of aggregate original bytes.");const auto data=readFile(path,maximum-total);total+=data.size();if(source&&j::sha256(data)!=g.codeSha)fail("reference_c.artifact_changed","Generated C changed before qualification export.");if(executable&&j::sha256(data)!=g.exeSha)fail("reference_c.artifact_changed","Generated executable changed before qualification export.");if(!first)out+=',';first=false;const bool text=source||command||result;out+="{\"name\":"+encode(name)+",\"sha256\":"+encode(j::sha256(data))+",\"encoding\":"+encode(text?"utf8":"hex")+",\"data\":"+encode(text?data:hex(data))+"}";}
    return out+"]";
}
struct Run {b::Identity owner;std::string token,graph,values;};
struct Session {std::uint64_t generation=0;std::set<std::string> requests;};
class Peer {
    b::Description description_;
    std::filesystem::path compilerConfig_;
    std::optional<Compiler> compiler_;
    std::map<std::string,Session> sessions_;
    std::map<std::string,std::unique_ptr<Graph>> graphs_;
    std::map<std::string,Run> runs_;
    std::size_t requestCount_=0;
    const Compiler&toolchain(){if(!compiler_)compiler_=compiler(compilerConfig_);return *compiler_;}
    b::GraphRequirements requirements(const J&p){b::GraphRequirements r;b::Error error;const auto bytes=firBytes(p);const auto provenance=field(p,"provenance");if(!b::graphRequirements(provenance.originalUtf8(),bytes,r,error))fail(error.code,error.message,{},error.pointer);const auto selection=field(p,"selection");r.engineId=str(selection,"engine_id");r.targetId=str(selection,"target_id");r.profileId=str(selection,"profile_id");return r;}
    void discovery(const b::GraphRequirements&r){b::Error error;if(!b::negotiate(r,description_,error))fail(error.code,error.message,{},error.pointer);}
    void session(const b::Identity&id){
        if(++requestCount_>1024)fail("reference_c.request_limit","Peer lifetime request budget exceeded.");auto found=sessions_.find(id.sessionId);if(found==sessions_.end()){if(sessions_.size()>=16)fail("reference_c.session_limit","Peer session budget exceeded.");found=sessions_.emplace(id.sessionId,Session{id.generation,{}}).first;}
        auto &s=found->second;if(id.generation<s.generation)fail("reference_c.stale_generation","An obsolete generation cannot reuse an admission or handle.");if(id.generation>s.generation){s.generation=id.generation;s.requests.clear();for(auto it=graphs_.begin();it!=graphs_.end();)if(it->second->owner.sessionId==id.sessionId)it=graphs_.erase(it);else ++it;for(auto it=runs_.begin();it!=runs_.end();)if(it->second.owner.sessionId==id.sessionId)it=runs_.erase(it);else ++it;}
        if(!s.requests.insert(id.requestId).second)fail("reference_c.request_replay","Request identity was already consumed in this generation.");
    }
    Graph&existing(const b::Message&m,const J&p,const b::GraphRequirements&r){const auto token=str(field(p,"receipt"),"token");const auto found=graphs_.find(token);if(found==graphs_.end())fail("reference_c.receipt_unknown","No complete-graph admission owns this receipt.");auto &g=*found->second;if(g.owner.sessionId!=m.identity.sessionId||g.owner.generation!=m.identity.generation||!sameGraph(g.program.requirements,r)||g.compilerSha!=toolchain().configSha)fail("reference_c.receipt_identity","Receipt graph, source, context, selection, compiler or session generation changed.");return g;}
    void lower(Graph&g){if(!g.lowered.empty())return;auto code=generate(g.program);g.workspace.emplace();writeFile(g.workspace->directory/L"kernel.c",code);g.files.push_back(g.workspace->directory/L"kernel.c");g.codeSha=j::sha256(code);g.lowered=nonce();}
    void build(Graph&g){if(!g.artifact.empty())return;lower(g);if(fileHash(g.workspace->directory/L"kernel.c")!=g.codeSha)fail("reference_c.code_changed","Generated C changed after lowering.");compile(toolchain(),g.workspace->directory,g.files);g.exeSha=fileHash(g.workspace->directory/L"kernel.exe");g.files.push_back(g.workspace->directory/L"kernel.exe");g.artifact=nonce();}
    Run&run(const b::Message&m,const J&p,Graph&g){if(runs_.size()>=64)fail("reference_c.run_limit","Peer completed-run handle budget exceeded.");const auto input=controlBytes(g.program,p.member("parameters"));build(g);if(fileHash(g.workspace->directory/L"kernel.exe")!=g.exeSha)fail("reference_c.artifact_changed","Compiled C executable changed after Build.");const auto token=nonce();const auto inputPath=g.workspace->directory/(wide(token)+L".inputs");writeFile(inputPath,input);const auto result=process(g.workspace->directory/L"kernel.exe",{inputPath.filename().wstring()},g.workspace->directory,toolchain(),5000);g.files.insert(g.files.end(),result.artifacts.begin(),result.artifacts.end());if(result.exit)fail("reference_c.kernel_failed","Actual C executable returned exit "+std::to_string(result.exit)+": "+excerpt(result.error));checkOutputs(g.program,result.output);Run value{m.identity,token,g.token,result.output};return runs_.emplace(token,std::move(value)).first->second;}
    Run&completed(const b::Message&m,const J&p){const auto token=str(field(p,"run_handle"),"token");const auto found=runs_.find(token);if(found==runs_.end()||found->second.owner.sessionId!=m.identity.sessionId||found->second.owner.generation!=m.identity.generation)fail("reference_c.run_handle","Run handle is unknown or belongs to another generation.");return found->second;}
public:
    explicit Peer(std::filesystem::path config):compilerConfig_(std::move(config)){b::Error error;if(!b::decodeDescription(profileJson,description_,error))fail("reference_c.profile",error.message);}
    std::string invoke(const b::Message&m){session(m.identity);const auto p=parse(m.payloadJson);
        if(m.method=="describe")return b::encodeDescription(description_);
        if(m.method=="probe"){const auto &c=toolchain();return base(m.method)+",\"observations\":{\"compiler_config_sha256\":"+encode(c.configSha)+",\"compiler_version\":"+encode(c.version)+",\"backend_id\":\"msvc-c17-19.44\",\"execution\":\"actual-generated-C17\"}}";}
        if(m.method=="stop"||m.method=="observe"){
            auto &r=completed(m,p);auto out=base(m.method)+",\"run_handle\":"+runHandle(r.token,r.owner);
            if(m.method=="observe"){
                bool include=false;if(const auto params=p.member("parameters")){fields(*params,{}, {"include_artifacts"});if(const auto flag=params->member("include_artifacts")){if(flag->kind()!=j::Kind::Boolean)fail("reference_c.observe_parameters","include_artifacts must be an exact boolean.");include=flag->boolean();}}
                const auto &g=*graphs_.at(r.graph);out+=",\"observations\":{\"state\":\"completed\",\"execution\":\"compiled-c17\",\"kernel_exit_code\":0,\"fir_sha256\":"+encode(g.program.requirements.firSha256)+",\"context_sha256\":"+encode(g.program.requirements.contextSha256)+",\"compiler_config_sha256\":"+encode(g.compilerSha)+",\"generated_c_sha256\":"+encode(g.codeSha)+",\"executable_sha256\":"+encode(g.exeSha)+",\"values_sha256\":"+encode(j::sha256(r.values))+",\"values\":"+r.values;
                if(include)out+=",\"artifacts\":"+exportedArtifacts(g);out+="}";
            }
            return out+"}";
        }
        if(m.method=="run")if(const auto command=p.member("command");command&&command->string()!="start")fail("reference_c.command_unsupported","This finite peer supports start and completed-handle stop/observe only.");
        if(m.method!="validate"&&m.method!="lower"&&m.method!="build"&&m.method!="run")fail("reference_c.method_unsupported","Method is outside this finite peer contract.");
        const auto r=requirements(p);
        if(m.method=="validate"){
            // Detailed type/operation refusal retains the responsible FIR/Source
            // identity; discovery alone cannot replace complete-graph admission.
            auto program=admit(firBytes(p),r);discovery(r);const auto &c=toolchain();if(graphs_.size()>=16)fail("reference_c.graph_limit","Peer immutable graph admission budget exceeded.");
            auto graph=std::make_unique<Graph>();graph->owner=m.identity;graph->program=std::move(program);graph->token=nonce();graph->compilerSha=c.configSha;graph->sourceSha=r.firSha256;const auto token=graph->token;graphs_.emplace(token,std::move(graph));return base(m.method)+",\"receipt\":"+receipt(token,r)+"}";
        }
        auto &g=existing(m,p,r);if(m.method=="lower"){lower(g);return base(m.method)+",\"lowered_handle\":"+handle(g.lowered)+"}";}if(m.method=="build"){build(g);return base(m.method)+",\"artifact_handle\":"+handle(g.artifact)+"}";}auto &rvalue=run(m,p,g);return base(m.method)+",\"run_handle\":"+runHandle(rvalue.token,rvalue.owner)+",\"values\":"+rvalue.values+"}";
    }
};
std::filesystem::path configuration(int argc,wchar_t**argv){if(argc!=3||std::wstring_view(argv[1])!=L"--compiler-config")fail("reference_c.arguments","Usage: frog-reference-c-bridge --compiler-config RELATIVE_CONFIG.json");const std::filesystem::path relative(argv[2]);if(relative.empty()||relative.is_absolute()||relative.has_root_name())fail("reference_c.compiler_config","Compiler configuration must be relative to the installed peer executable.");for(const auto&part:relative)if(part==L"..")fail("reference_c.compiler_config","Compiler configuration cannot escape the peer installation.");return(executablePath().parent_path()/relative).lexically_normal();}
}
int entry(int argc,wchar_t**argv){try{Peer peer(configuration(argc,argv));for(;;){const auto bytes=frame();if(!bytes)return 0;b::Message request;b::Error error;b::Limits limits;limits.maximumMessageBytes=static_cast<std::uint32_t>(maximumFrame);if(!b::decodeMessage(*bytes,request,error,limits)){std::cerr<<error.code<<": "<<error.message<<'\n';return 2;}if(request.kind!="request")fail("reference_c.envelope","Peer accepts request envelopes only.");b::Message response;response.kind="response";response.identity=request.identity;response.method=request.method;response.status="ok";
            try{if(!b::validatePayload(request,error))fail(error.code,error.message,{},error.pointer);response.payloadJson=peer.invoke(request);}
            catch(const Failure&f){response.status="error";response.payloadJson="{}";b::Diagnostic d;d.code=f.code;d.message=f.what();d.objectId=f.object;d.sourcePointer=!f.object.empty() && !f.pointer.empty() && f.pointer[0]=='/' ? f.pointer : std::string{};d.stage=request.method=="build"?"build":request.method=="run"?"run":"admit";try{const auto payload=parse(request.payloadJson);if(const auto provenance=payload.member("provenance"))d.sourceUri=str(field(*provenance,"source"),"uri");}catch(...){}response.diagnostics.push_back(std::move(d));}
            catch(const std::exception&e){response.status="error";response.payloadJson="{}";response.diagnostics.push_back({"reference_c.control_indeterminate","admit","error",std::string("Peer could not complete the bounded request: ")+e.what()});}
            send(response);
        }}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 2;}}
}
int wmain(int argc,wchar_t**argv){return reference_c::entry(argc,argv);}
