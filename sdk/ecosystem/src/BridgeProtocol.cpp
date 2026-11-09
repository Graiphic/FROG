// Public protocol implementation. JSON grammar, Unicode, resource ceilings and
// exact-byte hashing are shared with the production Frontend, not reimplemented.
#include <frog/bridge.hpp>
#include "frog_native_engine/FrogJson.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <charconv>
#include <cstddef>
#include <initializer_list>
#include <limits>
#include <set>
#include <stdexcept>
#include <utility>

namespace frog::bridge {
namespace {
namespace cj=frog::native_engine::canonical_json;
namespace json=frog::native_engine::source;
using Json=json::JsonValue;
constexpr std::size_t ceiling=16U*1024U*1024U;
struct Refusal final:std::runtime_error {
    Error error;
    explicit Refusal(Error value):std::runtime_error(value.message),error(std::move(value)){}
};
[[noreturn]] void reject(std::string code,std::string message,std::string pointer={}) {
    throw Refusal{{std::move(code),std::move(message),std::move(pointer)}};
}
bool oneOf(std::string_view value,std::initializer_list<std::string_view> values) {
    return std::find(values.begin(),values.end(),value)!=values.end();
}
std::string at(std::string_view parent,std::string_view key) {return cj::pointer(parent,key);}
void object(const Json &v,std::string_view pointer) {
    if(v.type!=Json::Type::Object)reject("bridge.schema","An object is required.",std::string(pointer));
}
void fields(const Json &v,std::initializer_list<std::string_view> required,
    std::initializer_list<std::string_view> optional,std::string_view pointer) {
    object(v,pointer);
    for(const auto key:required)if(!v.member(key))reject("bridge.schema","A required member is missing.",at(pointer,key));
    for(const auto &[key,value]:v.object) {
        (void)value;
        if(!oneOf(key,required)&&!oneOf(key,optional))reject("bridge.schema","An unknown member is not admitted.",at(pointer,key));
    }
}
const Json &member(const Json &v,std::string_view key,std::string_view pointer) {
    object(v,pointer);const auto *value=v.member(key);
    if(!value)reject("bridge.schema","A required member is missing.",at(pointer,key));
    return *value;
}
std::string text(const Json &v,std::string_view pointer,std::size_t maximum=2048,bool empty=false) {
    if(v.type!=Json::Type::String)reject("bridge.schema","A string is required.",std::string(pointer));
    // The shared parser already established UTF-8 validity. Schema lengths are
    // Unicode scalar counts, not encoded byte counts.
    std::size_t count=0;for(const unsigned char c:v.string)if((c&0xc0)!=0x80)++count;
    if((!empty&&count==0)||count>maximum)reject("bridge.schema","A string exceeds its declared bounds.",std::string(pointer));
    return v.string;
}
std::string str(const Json &v,std::string_view key,std::string_view parent,std::size_t maximum=2048,bool empty=false) {
    return text(member(v,key,parent),at(parent,key),maximum,empty);
}
void constant(const Json &v,std::string_view key,std::string_view expected,std::string_view parent={}) {
    if(str(v,key,parent,512)!=expected)reject("bridge.version","The document family or version is not supported.",at(parent,key));
}
// Exact JSON integer projection: 1, 1.0 and 1e0 mean the same integer. Never
// convert generation through binary64 (which aliases values above 2^53).
std::uint64_t uint(const Json &v,std::string_view pointer,std::uint64_t maximum=std::numeric_limits<std::uint64_t>::max()) {
    if(v.type!=Json::Type::Number)reject("bridge.schema","A nonnegative integer is required.",std::string(pointer));
    std::string_view lexeme=v.string;bool negative=false;
    if(!lexeme.empty()&&lexeme.front()=='-'){negative=true;lexeme.remove_prefix(1);}
    const auto exponentAt=lexeme.find_first_of("eE");
    const auto mantissa=lexeme.substr(0,exponentAt);std::string digits;digits.reserve(mantissa.size());
    std::size_t fraction=0;bool afterPoint=false;
    for(char c:mantissa){if(c=='.')afterPoint=true;else {digits+=c;if(afterPoint)++fraction;}}
    const auto nonzero=digits.find_first_not_of('0');
    if(nonzero==std::string::npos)return 0;
    if(negative)reject("bridge.schema","A nonnegative integer is required.",std::string(pointer));
    digits.erase(0,nonzero);
    std::int64_t exponent=0;
    if(exponentAt!=std::string_view::npos) {
        auto exp=lexeme.substr(exponentAt+1);if(!exp.empty()&&exp.front()=='+')exp.remove_prefix(1);
        const auto converted=std::from_chars(exp.data(),exp.data()+exp.size(),exponent);
        if(converted.ec!=std::errc{}||converted.ptr!=exp.data()+exp.size())reject("bridge.schema","An integer is outside its declared range.",std::string(pointer));
    }
    if(exponent>64||exponent< -static_cast<std::int64_t>(ceiling))reject("bridge.schema","An integer is outside its declared range.",std::string(pointer));
    const auto scale=exponent-static_cast<std::int64_t>(fraction);
    if(scale<0) {
        const auto remove=static_cast<std::uint64_t>(-scale);
        if(remove>=digits.size()||digits.find_first_not_of('0',digits.size()-static_cast<std::size_t>(remove))!=std::string::npos)
            reject("bridge.schema","A fractional value is not an integer.",std::string(pointer));
        digits.resize(digits.size()-static_cast<std::size_t>(remove));
    } else {
        if(digits.size()+static_cast<std::uint64_t>(scale)>20)reject("bridge.schema","An integer is outside its declared range.",std::string(pointer));
        digits.append(static_cast<std::size_t>(scale),'0');
    }
    std::uint64_t result=0;const auto converted=std::from_chars(digits.data(),digits.data()+digits.size(),result);
    if(converted.ec!=std::errc{}||converted.ptr!=digits.data()+digits.size()||result>maximum)
        reject("bridge.schema","An integer is outside its declared range.",std::string(pointer));
    return result;
}
std::uint64_t integer(const Json &v,std::string_view key,std::string_view parent,std::uint64_t maximum=std::numeric_limits<std::uint64_t>::max()) {
    return uint(member(v,key,parent),at(parent,key),maximum);
}
Json number(std::uint64_t n){Json v;v.type=Json::Type::Number;v.string=std::to_string(n);return v;}
Json parse(std::string_view bytes,std::size_t maximum=ceiling) {
    if(bytes.empty()||bytes.size()>std::min(maximum,ceiling))reject("bridge.message_limit","JSON bytes exceed the effective message budget.");
    std::string error;std::size_t offset=0;
    auto parsed=json::parseJson(bytes,&error,json::JsonNumberPolicy::PreserveLexeme,offset);
    if(!parsed)reject("bridge.json",error,"@byte:"+std::to_string(offset));
    return std::move(*parsed);
}
std::string raw(std::string_view bytes,const Json &value) {
    if(value.sourceBegin==std::string_view::npos||value.sourceEnd>bytes.size())return cj::encode(value);
    return std::string(bytes.substr(value.sourceBegin,value.sourceEnd-value.sourceBegin));
}
std::vector<std::string> strings(const Json &v,std::string_view pointer,std::size_t minimum=0,std::size_t maximumLength=512) {
    if(v.type!=Json::Type::Array||v.array.size()<minimum||v.array.size()>4096)reject("bridge.schema","An array exceeds its declared bounds.",std::string(pointer));
    std::vector<std::string> result;std::set<std::string> seen;
    for(std::size_t i=0;i<v.array.size();++i){auto value=text(v.array[i],at(pointer,std::to_string(i)),maximumLength);
        if(!seen.insert(value).second)reject("bridge.schema","A list member is duplicated.",at(pointer,std::to_string(i)));
        result.push_back(std::move(value));}
    return result;
}
Json stringArray(const std::vector<std::string> &values){auto a=cj::array();for(const auto &v:values)a.array.push_back(cj::string(v));return a;}
bool method(std::string_view s){return oneOf(s,{"describe","probe","validate","lower","build","deploy","run","stop","observe"});}
bool digest(std::string_view s){return s.size()==64&&std::all_of(s.begin(),s.end(),[](char c){return (c>='0'&&c<='9')||(c>='a'&&c<='f');});}
bool pointer(std::string_view s) {
    if(!s.empty()&&s.front()!='/')return false;
    for(std::size_t i=0;i<s.size();++i)if(s[i]=='~'&&(++i==s.size()||(s[i]!='0'&&s[i]!='1')))return false;
    return true;
}
void hash(const Json &v,std::string_view key,std::string_view parent){if(!digest(str(v,key,parent,64)))reject("bridge.schema","A lowercase SHA-256 digest is required.",at(parent,key));}
bool technicalId(std::string_view s){return !s.empty()&&s.size()<=128&&((s[0]>='A'&&s[0]<='Z')||(s[0]>='a'&&s[0]<='z'))&&
    std::all_of(s.begin()+1,s.end(),[](char c){return (c>='A'&&c<='Z')||(c>='a'&&c<='z')||(c>='0'&&c<='9')||c=='_'||c=='.'||c=='-';});}
Diagnostic diagnostic(const Json &v,std::string_view pointer) {
    fields(v,{"schema","version","code","stage","severity","message","source"},{},pointer);
    constant(v,"schema","frog.diagnostic",pointer);constant(v,"version","0.1",pointer);Diagnostic d;
    d.code=str(v,"code",pointer);d.stage=str(v,"stage",pointer);d.severity=str(v,"severity",pointer);d.message=str(v,"message",pointer);
    if(!oneOf(d.stage,{"read","resolve","validate","derive","admit","build","deploy","run","inspect","install"})||!oneOf(d.severity,{"info","warning","error"}))
        reject("bridge.schema","A diagnostic stage or severity is unknown.",std::string(pointer));
    const auto p=at(pointer,"source");const auto &source=member(v,"source",pointer);fields(source,{"uri","pointer"},{"object_id","port_id","region_id"},p);
    d.sourceUri=str(source,"uri",p);d.sourcePointer=str(source,"pointer",p,ceiling,true);
    if(!d.sourcePointer.empty()&&d.sourcePointer.front()!='/')reject("bridge.schema","A diagnostic pointer must be empty or start with '/'.",at(p,"pointer"));
    if(source.member("object_id"))d.objectId=str(source,"object_id",p);
    if(source.member("port_id"))d.portId=str(source,"port_id",p);
    if(source.member("region_id"))d.regionId=str(source,"region_id",p);
    return d;
}
Json diagnosticJson(const Diagnostic &d) {
    auto value=cj::object(),source=cj::object();source.object={{"uri",cj::string(d.sourceUri)},{"pointer",cj::string(d.sourcePointer)}};
    if(!d.objectId.empty())source.object["object_id"]=cj::string(d.objectId);
    if(!d.portId.empty())source.object["port_id"]=cj::string(d.portId);
    if(!d.regionId.empty())source.object["region_id"]=cj::string(d.regionId);
    value.object={{"schema",cj::string("frog.diagnostic")},{"version",cj::string("0.1")},{"code",cj::string(d.code)},
        {"stage",cj::string(d.stage)},{"severity",cj::string(d.severity)},{"message",cj::string(d.message)},{"source",std::move(source)}};
    return value;
}
template<class F> bool checked(Error &error,F &&f) {
    error={};try {f();return true;}catch(const Refusal &r){error=r.error;return false;}
    catch(const std::bad_alloc &){error={"bridge.resource_limit","Insufficient memory for the bounded protocol document.",{}};return false;}
    catch(const std::length_error &){error={"bridge.resource_limit","The bounded protocol document exceeds storage limits.",{}};return false;}
}
Message readMessage(std::string_view bytes,const Limits &limits) {
    const auto root=parse(bytes,limits.maximumMessageBytes);
    fields(root,{"schema","protocol_version","request_id","session_id","generation","kind","method","payload"},{"status","diagnostics","progress"},"");
    constant(root,"schema","frog.bridge.message");constant(root,"protocol_version","0.1");Message m;
    m.identity={str(root,"request_id",""),str(root,"session_id",""),integer(root,"generation","")};
    m.kind=str(root,"kind","");m.method=str(root,"method","");
    if(!oneOf(m.kind,{"request","response","progress"})||!method(m.method))reject("bridge.schema","An envelope kind or method is unknown.");
    const auto &payload=member(root,"payload","");object(payload,"/payload");m.payloadJson=raw(bytes,payload);
    if(root.member("status")){m.status=str(root,"status","");if(!oneOf(m.status,{"ok","error","cancelled"}))reject("bridge.schema","An envelope status is unknown.","/status");}
    if(const auto *diags=root.member("diagnostics")) {
        if(diags->type!=Json::Type::Array||diags->array.size()>4096)reject("bridge.schema","Diagnostics exceed their declared bounds.","/diagnostics");
        for(std::size_t i=0;i<diags->array.size();++i)m.diagnostics.push_back(diagnostic(diags->array[i],at("/diagnostics",std::to_string(i))));
    }
    if(m.kind=="response"&&(!root.member("status")||!root.member("diagnostics")))reject("bridge.schema","A response requires status and diagnostics.");
    if(const auto *progress=root.member("progress")) {
        fields(*progress,{"stage","completed","total"},{},"/progress");
        m.progress=Progress{str(*progress,"stage","/progress"),integer(*progress,"completed","/progress"),integer(*progress,"total","/progress")};
        if(m.progress->completed>m.progress->total)reject("bridge.progress","Completed progress exceeds its total.","/progress/completed");
    }
    if(m.kind=="progress"&&!m.progress)reject("bridge.schema","A progress envelope requires progress.","/progress");
    return m;
}
Description readDescription(std::string_view bytes) {
    const auto root=parse(bytes);const auto version=str(root,"schema_version","");
    if(version=="0.1")fields(root,{"schema","schema_version","bridge_id","protocol_version","fir_formats","profiles","operations","methods","limits"},{},"");
    else if(version=="0.2")fields(root,{"schema","schema_version","bridge_id","protocol_version","software_version","engine_id","fir_formats","profiles","operations","operations_exhaustive","methods","targets","type_families","services","commands","constraints","limits"},{},"");
    else reject("bridge.version","The bridge description version is unsupported.","/schema_version");
    constant(root,"schema","frog.bridge.describe");constant(root,"protocol_version","0.1");Description d;d.schemaVersion=version;d.bridgeId=str(root,"bridge_id","");
    if(!technicalId(d.bridgeId))reject("bridge.schema","The bridge ID is not a bounded technical identifier.","/bridge_id");
    const auto readPairs=[&](std::string_view key,bool formats){const auto &array=member(root,key,"");
        if(array.type!=Json::Type::Array||array.array.empty()||array.array.size()>4096)reject("bridge.schema","Formats and profiles require a bounded nonempty list.",at("",key));
        for(std::size_t i=0;i<array.array.size();++i){const auto p=at(at("",key),std::to_string(i));const auto &v=array.array[i];
            if(formats){fields(v,{"family","version"},{},p);d.firFormats.push_back({str(v,"family",p,512),str(v,"version",p,512)});}
            else {fields(v,{"id","version"},{},p);d.profiles.push_back({str(v,"id",p,512),str(v,"version",p,512)});}}
    };
    readPairs("fir_formats",true);readPairs("profiles",false);
    d.operations=strings(member(root,"operations",""),"/operations");d.methods=strings(member(root,"methods",""),"/methods",1);
    for(const auto &m:d.methods)if(!method(m))reject("bridge.schema","An advertised method is unknown.","/methods");
    const auto &limits=member(root,"limits","");
    if(version=="0.1")fields(limits,{"maximum_message_bytes","request_timeout_ms"},{},"/limits");
    else fields(limits,{"maximum_message_bytes","request_timeout_ms","maximum_progress_messages","maximum_stderr_bytes"},{},"/limits");
    d.limits.maximumMessageBytes=static_cast<std::uint32_t>(integer(limits,"maximum_message_bytes","/limits",ceiling));
    d.limits.requestTimeoutMs=static_cast<std::uint32_t>(integer(limits,"request_timeout_ms","/limits",300000));
    if(d.limits.maximumMessageBytes<1024||!d.limits.requestTimeoutMs)reject("bridge.schema","Advertised limits are outside the public bounds.","/limits");
    if(version=="0.2") {
        const auto &exhaustive=member(root,"operations_exhaustive","");
        if(exhaustive.type!=Json::Type::Boolean)reject("bridge.schema","Operation exhaustiveness requires a boolean.","/operations_exhaustive");
        d.operationsExhaustive=exhaustive.boolean;
        d.softwareVersion=str(root,"software_version","",512);d.engineId=str(root,"engine_id","",512);
        d.typeFamilies=strings(member(root,"type_families",""),"/type_families");d.services=strings(member(root,"services",""),"/services");d.commands=strings(member(root,"commands",""),"/commands");
        const auto &constraints=member(root,"constraints","");object(constraints,"/constraints");d.constraintsJson=raw(bytes,constraints);
        d.limits.maximumProgressMessages=static_cast<std::uint32_t>(integer(limits,"maximum_progress_messages","/limits",65536));
        d.limits.maximumStderrBytes=static_cast<std::uint32_t>(integer(limits,"maximum_stderr_bytes","/limits",1048576));
        if(!d.limits.maximumProgressMessages)reject("bridge.schema","At least one progress message must be budgeted.","/limits/maximum_progress_messages");
        const auto &targets=member(root,"targets","");
        if(targets.type!=Json::Type::Array||targets.array.size()>4096)reject("bridge.schema","Targets exceed their declared bounds.","/targets");
        std::set<std::string> ids;
        for(std::size_t i=0;i<targets.array.size();++i){const auto p=at("/targets",std::to_string(i));const auto &v=targets.array[i];
            fields(v,{"id","hardware_id","backend_id","profiles"},{},p);Target t{str(v,"id",p,512),str(v,"hardware_id",p,512),str(v,"backend_id",p,512),strings(member(v,"profiles",p),at(p,"profiles"))};
            if(!ids.insert(t.id).second)reject("bridge.schema","A target ID is duplicated.",at(p,"id"));d.targets.push_back(std::move(t));}
    }
    return d;
}
Profile profile(std::string value,std::string_view pointer) {
    auto delimiter=value.rfind('@');std::size_t suffix=delimiter==value.npos?value.npos:delimiter+1;
    if(suffix==value.npos){delimiter=value.rfind(".v");if(delimiter!=value.npos)suffix=delimiter+2;}
    if(suffix==value.npos||suffix==value.size()||!std::all_of(value.begin()+static_cast<std::ptrdiff_t>(suffix),value.end(),[](char c){return c>='0'&&c<='9';}))
        reject("bridge.profile_version","A Frontend profile must carry its own explicit .vN or @N version.",std::string(pointer));
    const auto version=value.substr(suffix);return {std::move(value),version};
}
GraphRequirements requirements(std::string_view provenanceBytes,std::string_view firBytes) {
    const auto provenance=parse(provenanceBytes),fir=parse(firBytes);object(provenance,"");object(fir,"/fir");
    fields(provenance,{"schema","schema_version","source","fir","frontend","profiles","operations","dependencies","closure_complete","semantic_context","limits","transformations"},{"projected_source_sha256"},"");
    constant(provenance,"schema","frog.frontend.context");constant(provenance,"schema_version","0.3");
    const auto &frontend=member(provenance,"frontend","");fields(frontend,{"software_version","api_version","commit","sha256"},{},"/frontend");
    constant(frontend,"api_version","3","/frontend");hash(frontend,"sha256","/frontend");
    str(frontend,"software_version","/frontend",512);const auto commit=str(frontend,"commit","/frontend",40);
    if(commit.size()!=40||!std::all_of(commit.begin(),commit.end(),[](char c){return (c>='0'&&c<='9')||(c>='a'&&c<='f');}))reject("bridge.schema","A captured Frontend commit must be a lowercase 40-digit digest.","/frontend/commit");
    const auto &complete=member(provenance,"closure_complete","");if(complete.type!=Json::Type::Boolean||!complete.boolean)reject("bridge.incomplete_closure","A complete Frontend dependency closure is required.","/closure_complete");
    const auto &source=member(provenance,"source","");fields(source,{"family","version","uri","document_id","sha256"},{},"/source");
    const auto family=str(source,"family","/source",512),version=str(source,"version","/source",512);
    const bool draft=family=="frog.document.draft";
    // These are admitted provenance families/versions, not a source reader.
    // The bridge consumes only the complete FIR produced by that Frontend.
    if((draft&&!oneOf(version,{"0","1","2","3","4","5","6","7","8","9"}))||
        (!draft&&(family!="frog.source"||version!="0.1")))
        reject("bridge.version","The authored source family or version is not supported.","/source/version");
    hash(source,"sha256","/source");str(source,"document_id","/source",8192);
    if(draft&&!provenance.member("projected_source_sha256"))
        reject("bridge.source_projection","Draft provenance requires the exact derived Source hash.","/projected_source_sha256");
    if(provenance.member("projected_source_sha256"))hash(provenance,"projected_source_sha256","");
    const auto sourceHash="sha256:"+(provenance.member("projected_source_sha256")?
        str(provenance,"projected_source_sha256","",64):str(source,"sha256","/source",64));
    // The existing FIR producers encode their exact input hash either as the
    // composed metadata field or as the canonical scalar/Event identity note.
    // Neither representation permits a different projection to inherit FIR.
    const auto &metadata=member(fir,"metadata","");object(metadata,"/metadata");
    std::size_t hashIdentities=0;
    if(metadata.member("source_sha256")) {
        ++hashIdentities;
        if(str(metadata,"source_sha256","/metadata",71)!=sourceHash)
            reject("bridge.source_projection","FIR does not belong to the captured Source projection.","/metadata/source_sha256");
    }
    if(const auto *notes=metadata.member("notes")) {
        if(notes->type!=Json::Type::Array)reject("bridge.schema","FIR metadata notes must be an array.","/metadata/notes");
        for(const auto &note:notes->array) {
            const auto value=text(note,"/metadata/notes",ceiling);
            if(value.starts_with("Exact source bytes: ")) {
                ++hashIdentities;
                if(value!="Exact source bytes: "+sourceHash)
                    reject("bridge.source_projection","FIR does not belong to the captured Source projection.","/metadata/notes");
            }
        }
    }
    if(hashIdentities!=1)reject("bridge.source_projection","FIR must retain one exact Source projection hash.","/metadata");
    object(member(provenance,"semantic_context",""),"/semantic_context");
    const auto &limits=member(provenance,"limits","");
    fields(limits,{"max_source_bytes","max_dependency_bytes","max_total_dependency_bytes","max_result_bytes","max_nodes","max_connections","max_nesting_depth","max_dependencies","max_diagnostics"},{},"/limits");
    for(const auto &[key,value]:limits.object)if(!uint(value,at("/limits",key)))reject("bridge.schema","Captured Frontend limits must be positive.",at("/limits",key));
    const auto &dependencies=member(provenance,"dependencies","");
    if(dependencies.type!=Json::Type::Array||dependencies.array.size()>4096)reject("bridge.schema","Captured dependencies exceed the bridge metadata budget.","/dependencies");
    for(std::size_t i=0;i<dependencies.array.size();++i) {
        const auto p=at("/dependencies",std::to_string(i));const auto &dependency=dependencies.array[i];
        fields(dependency,{"kind","identity","version","sha256","source_uri","origin"},{},p);
        const auto kind=integer(dependency,"kind",p);
        if(kind!=1&&kind!=2)reject("bridge.version","This API-3 dependency kind is not supported.",at(p,"kind"));
        for(const auto key:{"identity","version","source_uri"})str(dependency,key,p,8192);
        constant(dependency,"origin","explicit-resolver",p);hash(dependency,"sha256",p);
    }
    const auto &transformations=member(provenance,"transformations","");
    if(transformations.type!=Json::Type::Array||transformations.array.size()>4096)reject("bridge.schema","Captured transformations exceed the bridge metadata budget.","/transformations");
    for(std::size_t i=0;i<transformations.array.size();++i) {
        const auto p=at("/transformations",std::to_string(i));const auto &transformation=transformations.array[i];
        fields(transformation,{"profile","source_uri","source_pointer","dependency_uri","dependency_pointer","dependency_sha256"},{},p);
        str(transformation,"profile",p,512);
        for(const auto key:{"source_uri","dependency_uri"})str(transformation,key,p,8192,true);
        for(const auto key:{"source_pointer","dependency_pointer"})if(!pointer(str(transformation,key,p,ceiling,true)))reject("bridge.schema","A provenance pointer is malformed.",at(p,key));
        hash(transformation,"dependency_sha256",p);
    }
    const auto &declared=member(provenance,"fir","");fields(declared,{"family","version","document_id"},{},"/fir");
    GraphRequirements r;r.fir={str(declared,"family","/fir",512),str(declared,"version","/fir",512)};
    r.documentId=str(declared,"document_id","/fir",8192);r.sourceUri=str(source,"uri","/source",8192);
    if(str(fir,"schema_family","",512)!=r.fir.family||str(fir,"schema_version","",512)!=r.fir.version||str(fir,"document_id","",8192)!=r.documentId)
        reject("bridge.fir_identity","The complete FIR and provenance identify different artifacts.","/fir");
    const auto selected=strings(member(provenance,"profiles",""),"/profiles",1);
    for(std::size_t i=0;i<selected.size();++i)r.profiles.push_back(profile(selected[i],at("/profiles",std::to_string(i))));
    r.operations=strings(member(provenance,"operations",""),"/operations");
    const auto &unit=member(fir,"unit",""),&objects=member(unit,"objects","/unit");
    if(objects.type!=Json::Type::Array)reject("bridge.schema","The complete FIR object list is required.","/unit/objects");
    std::set<std::string> actual,declaredOps(r.operations.begin(),r.operations.end());
    for(std::size_t i=0;i<objects.array.size();++i)actual.insert(str(objects.array[i],"kind",at("/unit/objects",std::to_string(i)),512));
    if(actual!=declaredOps)reject("bridge.fir_operations","Provenance operations differ from the complete FIR object list.","/operations");
    r.firSha256=frog::native_engine::hashing::sha256Hex(firBytes);
    r.contextSha256=frog::native_engine::hashing::sha256Hex(provenanceBytes);
    r.frontendFingerprint=frog::native_engine::hashing::sha256Hex(cj::encode(frontend));r.provenanceJson=provenanceBytes;
    return r;
}
void selection(const Json &v,std::string_view pointer) {
    fields(v,{"engine_id","target_id","profile_id"},{},pointer);
    for(const auto key:{"engine_id","target_id","profile_id"})str(v,key,pointer,512);
}
void receipt(const Json &v,std::string_view pointer) {
    fields(v,{"token","fir_sha256","context_sha256","engine_id","target_id","profile_id"},{},pointer);
    str(v,"token",pointer);hash(v,"fir_sha256",pointer);hash(v,"context_sha256",pointer);
    for(const auto key:{"engine_id","target_id","profile_id"})str(v,key,pointer,512);
}
void handle(const Json &v,std::string_view pointer,bool run) {
    if(run){fields(v,{"token","session_id","generation"},{},pointer);str(v,"session_id",pointer);integer(v,"generation",pointer);}
    else fields(v,{"token"},{},pointer);
    str(v,"token",pointer);
}
void payload(const Message &m) {
    const auto p=parse(m.payloadJson);object(p,"/payload");
    if((m.method=="describe"||m.method=="probe")&&m.kind=="request"&&p.object.empty())return;
    if(m.method=="describe"&&m.kind=="response"&&m.status=="ok"){(void)readDescription(m.payloadJson);return;}
    if(m.kind=="response"&&m.status!="ok"&&p.object.empty())return;
    fields(p,{"schema","schema_version","method","phase"},{"fir","provenance","selection","receipt","lowered_handle","artifact_handle","deployment_handle","run_handle","command","parameters","values","observations"},"/payload");
    constant(p,"schema","frog.bridge.payload","/payload");constant(p,"schema_version","0.1","/payload");
    if(str(p,"method","/payload")!=m.method||str(p,"phase","/payload")!=(m.kind=="request"?"request":m.kind=="response"?"response":"progress"))
        reject("bridge.payload_identity","Payload method or phase differs from its envelope.","/payload");
    if(!method(m.method))reject("bridge.schema","An envelope method is unknown.","/method");
    const auto *command=p.member("command");
    const bool runCommand=m.method=="run"&&command&&text(*command,"/payload/command",512)!="start";
    const bool graph=m.kind=="request"&&oneOf(m.method,{"validate","lower","build","deploy","run"})&&!runCommand;
    std::set<std::string_view> allowed{"schema","schema_version","method","phase"};
    const auto allow=[&](std::initializer_list<std::string_view> keys){allowed.insert(keys.begin(),keys.end());};
    if(m.kind=="request") {
        if(graph){allow({"fir","provenance","selection"});if(m.method!="validate")allow({"receipt"});if(m.method=="run")allow({"command","parameters"});}
        else if(runCommand)allow({"run_handle","command","parameters"});
        else if(m.method=="stop")allow({"run_handle"});
        else if(m.method=="observe")allow({"run_handle","parameters"});
        else if(m.method=="probe")allow({"parameters"});
    } else if(m.kind=="response"&&m.status=="ok") {
        if(m.method=="validate")allow({"receipt"});
        else if(m.method=="lower")allow({"lowered_handle"});
        else if(m.method=="build")allow({"artifact_handle"});
        else if(m.method=="deploy")allow({"deployment_handle"});
        else if(m.method=="run")allow({"run_handle","values"});
        else if(m.method=="stop")allow({"run_handle"});
        else if(m.method=="observe")allow({"run_handle","observations"});
        else if(m.method=="probe")allow({"observations","run_handle"});
    }
    for(const auto &[key,value]:p.object){(void)value;if(!allowed.contains(key))reject("bridge.payload_member","This member does not belong to the declared method and phase.",at("/payload",key));}
    if(graph) {
        const auto &f=member(p,"fir","/payload");fields(f,{"family","version","sha256","utf8"},{},"/payload/fir");
        hash(f,"sha256","/payload/fir");const auto bytes=str(f,"utf8","/payload/fir",ceiling);
        const auto &provenance=member(p,"provenance","/payload");const auto r=requirements(raw(m.payloadJson,provenance),bytes);
        if(r.fir.family!=str(f,"family","/payload/fir",512)||r.fir.version!=str(f,"version","/payload/fir",512)||r.firSha256!=str(f,"sha256","/payload/fir",64))
            reject("bridge.fir_digest","FIR bytes differ from the declared artifact identity.","/payload/fir");
        const auto &s=member(p,"selection","/payload");selection(s,"/payload/selection");
        if(m.method!="validate") {
            const auto &ticket=member(p,"receipt","/payload");receipt(ticket,"/payload/receipt");
            if(str(ticket,"fir_sha256","/payload/receipt",64)!=r.firSha256||str(ticket,"context_sha256","/payload/receipt",64)!=r.contextSha256)
                reject("bridge.receipt_identity","The admission receipt names different FIR or provenance bytes.","/payload/receipt");
            for(const auto key:{"engine_id","target_id","profile_id"})if(str(ticket,key,"/payload/receipt",512)!=str(s,key,"/payload/selection",512))
                reject("bridge.receipt_identity","The admission receipt names a different target selection.",at("/payload/receipt",key));
        }
    }
    if(const auto *v=p.member("receipt"))receipt(*v,"/payload/receipt");
    if(const auto *v=p.member("selection"))selection(*v,"/payload/selection");
    for(const auto key:{"lowered_handle","artifact_handle","deployment_handle","run_handle"})if(const auto *v=p.member(key))handle(*v,at("/payload",key),std::string_view(key)=="run_handle");
    if(m.kind=="request"&&(oneOf(m.method,{"stop","observe"})||runCommand))(void)member(p,"run_handle","/payload");
    if(const auto *run=p.member("run_handle")) {
        handle(*run,"/payload/run_handle",true);
        if(str(*run,"session_id","/payload/run_handle")!=m.identity.sessionId||integer(*run,"generation","/payload/run_handle")!=m.identity.generation)
            reject("bridge.handle_identity","The run handle belongs to another session generation.","/payload/run_handle");
    }
    for(const auto key:{"parameters","values","observations"})if(const auto *v=p.member(key))object(*v,at("/payload",key));
    if(m.kind=="response"&&m.status=="ok") {
        const std::string_view required=m.method=="validate"?"receipt":m.method=="lower"?"lowered_handle":m.method=="build"?"artifact_handle":m.method=="deploy"?"deployment_handle":oneOf(m.method,{"run","stop","observe"})?"run_handle":"";
        if(!required.empty())(void)member(p,required,"/payload");
        if(m.method=="probe")(void)member(p,"observations","/payload");
    }
}
}

bool decodeMessage(std::string_view bytes,Message &out,Error &error,const Limits &limits) {
    return checked(error,[&]{auto value=readMessage(bytes,limits);out=std::move(value);});
}
std::string encodeMessage(const Message &m) {
    const auto p=parse(m.payloadJson);object(p,"/payload");auto root=cj::object();
    root.object={{"schema",cj::string("frog.bridge.message")},{"protocol_version",cj::string(m.protocolVersion)},
        {"request_id",cj::string(m.identity.requestId)},{"session_id",cj::string(m.identity.sessionId)},
        {"generation",number(m.identity.generation)},{"kind",cj::string(m.kind)},{"method",cj::string(m.method)}};
    if(!m.status.empty())root.object["status"]=cj::string(m.status);
    if(m.kind=="response"||!m.diagnostics.empty()){auto a=cj::array();for(const auto &d:m.diagnostics)a.array.push_back(diagnosticJson(d));root.object["diagnostics"]=std::move(a);}
    if(m.progress){auto progress=cj::object();progress.object={{"stage",cj::string(m.progress->stage)},{"completed",number(m.progress->completed)},{"total",number(m.progress->total)}};root.object["progress"]=std::move(progress);}
    auto encoded=cj::encode(root);encoded.pop_back();encoded+=",\"payload\":";encoded+=m.payloadJson;encoded+='}';
    Limits limits;limits.maximumMessageBytes=static_cast<std::uint32_t>(ceiling);(void)readMessage(encoded,limits);return encoded;
}
bool validatePayload(const Message &m,Error &error){return checked(error,[&]{payload(m);});}
bool decodeDescription(std::string_view bytes,Description &out,Error &error){return checked(error,[&]{auto d=readDescription(bytes);out=std::move(d);});}
std::string encodeDescription(const Description &d) {
    auto root=cj::object(),formats=cj::array(),profiles=cj::array(),limits=cj::object();
    for(const auto &f:d.firFormats){auto v=cj::object();v.object={{"family",cj::string(f.family)},{"version",cj::string(f.version)}};formats.array.push_back(std::move(v));}
    for(const auto &p:d.profiles){auto v=cj::object();v.object={{"id",cj::string(p.id)},{"version",cj::string(p.version)}};profiles.array.push_back(std::move(v));}
    limits.object={{"maximum_message_bytes",number(d.limits.maximumMessageBytes)},{"request_timeout_ms",number(d.limits.requestTimeoutMs)}};
    root.object={{"schema",cj::string("frog.bridge.describe")},{"schema_version",cj::string(d.schemaVersion)},{"bridge_id",cj::string(d.bridgeId)},
        {"protocol_version",cj::string(d.protocolVersion)},{"fir_formats",std::move(formats)},{"profiles",std::move(profiles)},
        {"operations",stringArray(d.operations)},{"methods",stringArray(d.methods)}};
    if(d.schemaVersion=="0.2") {
        root.object["operations_exhaustive"]=cj::boolean(d.operationsExhaustive);
        root.object["software_version"]=cj::string(d.softwareVersion);root.object["engine_id"]=cj::string(d.engineId);
        root.object["type_families"]=stringArray(d.typeFamilies);root.object["services"]=stringArray(d.services);root.object["commands"]=stringArray(d.commands);
        auto targets=cj::array();for(const auto &t:d.targets){auto v=cj::object();v.object={{"id",cj::string(t.id)},{"hardware_id",cj::string(t.hardwareId)},{"backend_id",cj::string(t.backendId)},{"profiles",stringArray(t.profiles)}};targets.array.push_back(std::move(v));}root.object["targets"]=std::move(targets);
        root.object["constraints"]=parse(d.constraintsJson);object(root.object["constraints"],"/constraints");
        limits.object["maximum_progress_messages"]=number(d.limits.maximumProgressMessages);limits.object["maximum_stderr_bytes"]=number(d.limits.maximumStderrBytes);
    }
    root.object["limits"]=std::move(limits);auto bytes=cj::encode(root);(void)readDescription(bytes);return bytes;
}
bool graphRequirements(std::string_view provenance,std::string_view fir,GraphRequirements &out,Error &error) {
    return checked(error,[&]{auto r=requirements(provenance,fir);out=std::move(r);});
}
bool negotiate(const GraphRequirements &r,const Description &d,Error &error) {
    return checked(error,[&]{
        if(d.protocolVersion!="0.1"||!oneOf(d.schemaVersion,{"0.1","0.2"}))reject("bridge.version","The bridge wire or description version is unsupported.");
        for(const auto m:{"describe","validate"})if(std::find(d.methods.begin(),d.methods.end(),m)==d.methods.end())reject("bridge.method_unsupported","The bridge must describe itself and validate the complete FIR.","/methods");
        if(std::find(d.firFormats.begin(),d.firFormats.end(),r.fir)==d.firFormats.end())reject("bridge.fir_unsupported","The bridge does not admit the exact FIR family and version.","/fir_formats");
        for(const auto &p:r.profiles)if(std::find(d.profiles.begin(),d.profiles.end(),p)==d.profiles.end())reject("bridge.profile_unsupported","A required independently versioned semantic profile is unavailable: "+p.id,"/profiles");
        for(const auto &op:r.operations)if(d.operationsExhaustive&&std::find(d.operations.begin(),d.operations.end(),op)==d.operations.end())reject("bridge.operation_unsupported","A required complete-graph operation is unavailable: "+op,"/operations");
        if(!r.engineId.empty()&&r.engineId!=d.engineId)reject("bridge.engine_unsupported","The selected engine is unavailable.","/engine_id");
        if(!r.targetId.empty()||!r.profileId.empty()) {
            const auto target=std::find_if(d.targets.begin(),d.targets.end(),[&](const Target &t){return t.id==r.targetId;});
            if(target==d.targets.end())reject("bridge.target_unsupported","The selected target is unavailable.","/targets");
            if(!r.profileId.empty()&&std::find(target->profiles.begin(),target->profiles.end(),r.profileId)==target->profiles.end())reject("bridge.realization_unsupported","The target does not offer the selected realization profile.","/targets/profiles");
        }
    });
}
Exchange::Exchange(Identity identity,std::string method,Limits limits):identity_(std::move(identity)),method_(std::move(method)),limits_(limits){}
bool Exchange::accept(const Message &m,Error &error) {
    return checked(error,[&]{
        if(m.identity!=identity_||cancelled_){if(discarded_!=std::numeric_limits<std::uint32_t>::max())++discarded_;reject("bridge.stale_message","An obsolete or cancelled request/session generation was discarded.");}
        if(finished_)reject("bridge.exchange_finished","A message followed the final response.");
        if(m.protocolVersion!="0.1"||m.method!=method_)reject("bridge.exchange_identity","A message changed the wire version or request method.");
        if(m.kind=="progress") {
            if(!m.progress||m.progress->stage.empty()||m.progress->completed>m.progress->total)reject("bridge.progress","Progress is malformed or exceeds its total.");
            if(progress_>=limits_.maximumProgressMessages)reject("bridge.progress_limit","The request exhausted its progress-message budget.");++progress_;
        } else if(m.kind=="response") {
            if(!oneOf(m.status,{"ok","error","cancelled"}))reject("bridge.schema","A final response requires a known status.");finished_=true;
        } else reject("bridge.exchange_kind","Only progress or a final response is admitted from a bridge.");
    });
}
void Exchange::cancel()noexcept{cancelled_=true;finished_=true;}
bool Exchange::finished()const noexcept{return finished_;}
bool Exchange::cancelled()const noexcept{return cancelled_;}
std::uint32_t Exchange::discarded()const noexcept{return discarded_;}
}
