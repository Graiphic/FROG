// Portable protocol checks using historical wire fixtures and FIR/provenance
// produced by the actual public Frontend API. No substitute language generator.
#include <frog/bridge.hpp>
#include <frog/frontend.h>
#include "frog_native_engine/FrogJson.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "CanonicalJsonSupport.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

namespace {
namespace b=frog::bridge;
namespace cj=frog::native_engine::canonical_json;
using Json=frog::native_engine::source::JsonValue;
unsigned checks=0;
void need(bool value,std::string_view why){++checks;if(!value)throw std::runtime_error(std::string(why));}
std::string read(const std::filesystem::path &path){need(std::filesystem::file_size(path)<=8U*1024U*1024U,"Fixture exceeded the identified bounded corpus");std::ifstream file(path,std::ios::binary);need(bool(file),"Cannot read the existing fixture");return {std::istreambuf_iterator<char>(file),{}};}
Json parse(std::string_view bytes){std::string error;auto v=frog::native_engine::source::parseJson(bytes,&error,frog::native_engine::source::JsonNumberPolicy::PreserveLexeme);need(bool(v),"Actual complete result is not readable JSON: "+error);return std::move(*v);}
frog_frontend_utf8 span(std::string_view value){return {value.data(),value.size()};}
std::string copy(frog_frontend_utf8 value){need(value.data||!value.size,"Malformed real API result span");return value.size?std::string(value.data,value.size):std::string{};}
struct Actual {std::string fir,provenance,maps,source;};
Actual deriveBytes(std::string bytes,std::string_view family="frog.source",std::string_view version="0.1",frog_frontend_resolver resolver={}) {
    Actual actual;actual.source=std::move(bytes);frog_frontend_context *context=nullptr;frog_frontend_config config{};config.struct_size=sizeof(config);
    need(frog_frontend_context_create(FROG_FRONTEND_API_VERSION,&config,&context)==FROG_FRONTEND_OK&&context,"Cannot create the actual API-3 Frontend");
    struct ContextOwner{frog_frontend_context *p;~ContextOwner(){frog_frontend_context_destroy(p);}} owner{context};
    frog_frontend_request request{};request.struct_size=sizeof(request);request.source_bytes=span(actual.source);
    request.source_uri=span("bridge-smoke:/actual/source.frog");request.source_document_id=span("source:bridge-smoke");
    request.source_format_id=span(family);request.source_format_version=span(version);request.fir_format_version=span("0.1");
    request.resolver=resolver;
    request.fir_document_id=span("fir:bridge-smoke");request.semantic_context_json=span("{}");frog_frontend_result *result=nullptr;
    need(frog_frontend_derive(context,&request,&result)==FROG_FRONTEND_OK&&result,"Actual API-3 derive failed");
    struct ResultOwner{frog_frontend_result *p;~ResultOwner(){frog_frontend_result_destroy(p);}} resultOwner{result};
    frog_frontend_result_view view{};view.struct_size=sizeof(view);need(frog_frontend_result_get(result,&view)==FROG_FRONTEND_OK,"Cannot inspect the immutable API result");
    if(view.outcome!=FROG_FRONTEND_SUCCESS)std::cerr<<copy(view.diagnostics_json)<<'\n';
    need(view.outcome==FROG_FRONTEND_SUCCESS,"Real positive source did not derive successfully");
    actual.fir=copy(view.fir_json);actual.provenance=copy(view.provenance_json);actual.maps=copy(view.source_maps_json);
    need(!actual.fir.empty()&&!actual.provenance.empty(),"Actual derive omitted complete documents");return actual;
}
Actual derive(const std::filesystem::path &file) {return deriveBytes(read(file));}

struct DraftSourceCapture {
    std::string bytes,uri="memory:/bridge-child.frog",version;
    unsigned calls=0,releases=0;bool validRequests=true;
    static frog_frontend_resolve_status FROG_FRONTEND_CALL resolve(void *user,
        const frog_frontend_dependency_request *request,frog_frontend_resolved_dependency *out) {
        auto &capture=*static_cast<DraftSourceCapture*>(user);++capture.calls;
        if(!request||!out||request->kind!=FROG_FRONTEND_DEPENDENCY_SOURCE||
            std::string_view(request->reference.data,request->reference.size)!="child.frog") {
            capture.validRequests=false;return FROG_FRONTEND_RESOLVE_ERROR;
        }
        out->struct_size=sizeof(*out);out->source_uri=span(capture.uri);
        out->identity=span(capture.uri);out->version=span(capture.version);out->bytes=span(capture.bytes);
        return FROG_FRONTEND_RESOLVED;
    }
    static void FROG_FRONTEND_CALL release(void *user,const frog_frontend_resolved_dependency *) {
        ++static_cast<DraftSourceCapture*>(user)->releases;
    }
};
frog::native_engine::FrogDraftData passThroughDraft(bool child) {
    using namespace frog::native_engine;
    FrogDraftData document;document.title=child?"Bridge child":"Bridge parent";
    if(child) {
        FrogDraftInterface interface;
        need(interface.declarePort(true,"in","i64","required")&&interface.declarePort(false,"out","i64"),"Cannot declare the genuine child interface");
        document.declaredInterface=std::move(interface);
    }
    for(const bool input:{true,false}) {
        const auto id=input?"source":"sink";
        const auto *entry=findWidgetCatalogEntryByClassId(input?"frog.widgets.numeric_control":"frog.widgets.numeric_indicator");
        need(entry,"The genuine public numeric widget data class is missing");
        auto widget=makeWidgetData(id,*entry,{20,20,96,28});widget.valueType="i64";
        need(setWidgetNumericIntegerValueLexeme(widget,"9007199254740993"),"Exact I64 fixture value was narrowed");
        if(child) {
            if(input){widget.publicInputId="in";widget.interfaceConnectionRequirement="required";}
            else widget.publicOutputId="out";
            widget.interfaceMapSlotId=input?"slot_1":"slot_2";
        }
        DiagramNode node;node.id=std::string(id)+"-terminal";node.kind="widget_value";node.widgetId=id;
        node.type=diagramTerminalValueType(widget);node.layout={20,20,48,24};document.diagramNodes.push_back(std::move(node));
        document.widgets.addObject({id,widget.bounds,std::move(widget)},true);
    }
    document.diagramEdges.push_back({"pass",{"source-terminal","value"},{"sink-terminal","value"}});
    return document;
}
b::Description descriptor(const b::GraphRequirements &requirements);
void actualDraft() {
    using namespace frog::native_engine;
    auto parent=passThroughDraft(false);const auto directBytes=serializeFrogDraftData(parent);
    const auto directRevision=parse(directBytes).object.at("draft_revision").string;
    const auto direct=deriveBytes(directBytes,"frog.document.draft",directRevision);
    b::GraphRequirements requirements;b::Error error;
    need(b::graphRequirements(direct.provenance,direct.fir,requirements,error),"Actual raw draft FIR/provenance failed: "+error.code+" "+error.message);
    auto context=parse(direct.provenance);
    need(context.object.at("source").object.at("family").string=="frog.document.draft"&&
        context.object.at("source").object.at("sha256").string==hashing::sha256Hex(directBytes)&&
        context.member("projected_source_sha256"),"RAW attribution was disguised as public Source");
    auto invalid=context;invalid.object.erase("projected_source_sha256");
    need(!b::graphRequirements(cj::encode(invalid),direct.fir,requirements,error)&&error.code=="bridge.source_projection","A draft lacking its prepared Source hash was admitted");
    invalid=context;invalid.object["projected_source_sha256"]=cj::string(std::string(64,'0'));
    need(!b::graphRequirements(cj::encode(invalid),direct.fir,requirements,error)&&error.code=="bridge.source_projection","A different Source projection inherited the real FIR");
    invalid=context;invalid.object["source"].object["family"]=cj::string("frog.unknown");
    need(!b::graphRequirements(cj::encode(invalid),direct.fir,requirements,error)&&error.code=="bridge.version","An unknown authored source family was admitted");
    invalid=context;invalid.object["source"].object["version"]=cj::string("10");
    need(!b::graphRequirements(cj::encode(invalid),direct.fir,requirements,error)&&error.code=="bridge.version","An unknown draft revision was inferred as current");
    DraftSourceCapture capture;capture.bytes=serializeFrogDraftData(passThroughDraft(true));capture.version="sha256:"+hashing::sha256Hex(capture.bytes);
    parent.diagramEdges.clear();DiagramNode call;call.id="call";call.kind="frog_call";call.type="frog.studio.frog_call";call.callDocumentPath="child.frog";call.layout={120,20,48,48};
    call.callPorts={{"in","in","slot_1","same","i64",true,"required",0.,0.5},{"out","out","slot_2","same","i64",false,"optional",1.,0.5}};
    parent.diagramNodes.push_back(std::move(call));
    parent.diagramEdges.push_back({"argument",{"source-terminal","value"},{"call","in"}});
    parent.diagramEdges.push_back({"result",{"call","out"},{"sink-terminal","value"}});
    const auto bytes=serializeFrogDraftData(parent),childBefore=capture.bytes;
    const auto withDependency=deriveBytes(bytes,"frog.document.draft",parse(bytes).object.at("draft_revision").string,
        {&capture,DraftSourceCapture::resolve,DraftSourceCapture::release});
    need(capture.validRequests&&capture.calls==1&&capture.releases==1&&capture.bytes==childBefore&&serializeFrogDraftData(parent)==bytes,
        "Source capture duplicated resolution, lost ownership or mutated an authored document");
    need(b::graphRequirements(withDependency.provenance,withDependency.fir,requirements,error),"Real captured SubFROG provenance was refused: "+error.code+" "+error.message);
    const auto original=parse(withDependency.provenance);const auto &dependencies=original.object.at("dependencies").array;
    need(dependencies.size()==1&&dependencies[0].object.at("kind").string=="1"&&
        dependencies[0].object.at("sha256").string==hashing::sha256Hex(childBefore)&&
        dependencies[0].object.at("version").string==capture.version,"Exact Source dependency kind/version/hash was lost");
    requirements.engineId="graiphic.llvm.cpu";requirements.targetId="windows-x64";requirements.profileId="native-fast";
    const auto description=descriptor(requirements);
    need(b::negotiate(requirements,description,error),"Draft provenance changed the unchanged FIR/profile negotiation");
    invalid=original;invalid.object["dependencies"].array[0].object["kind"]=parse("3");
    need(!b::graphRequirements(cj::encode(invalid),withDependency.fir,requirements,error)&&error.code=="bridge.version","An unimplemented dependency kind was silently admitted");
    invalid=original;invalid.object["dependencies"].array[0].object["sha256"]=cj::string("not-a-digest");
    need(!b::graphRequirements(cj::encode(invalid),withDependency.fir,requirements,error)&&error.code=="bridge.schema","An invalid dependency digest was admitted");
    invalid=original;invalid.object["dependencies"].array[0].object["version"]=cj::string("");
    need(!b::graphRequirements(cj::encode(invalid),withDependency.fir,requirements,error)&&error.code=="bridge.schema","An absent dependency version was inferred");
}

b::Description descriptor(const b::GraphRequirements &requirements) {
    b::Description d;d.bridgeId="smoke.protocol";d.softwareVersion="0.1.0";d.engineId="graiphic.llvm.cpu";
    d.firFormats={requirements.fir};d.profiles=requirements.profiles;d.operations=requirements.operations;
    d.methods={"describe","probe","validate","lower","build","run","stop","observe"};
    d.targets={{"windows-x64","windows-x64","llvm-22.1.7",{"native-fast","native-debuggable"}}};
    d.typeFamilies={"numeric","record","array"};d.services={"execution"};d.commands={"start","pause","resume","step","controls","events"};return d;
}
std::string selection(){return "{\"engine_id\":\"graiphic.llvm.cpu\",\"target_id\":\"windows-x64\",\"profile_id\":\"native-fast\"}";}
std::string receipt(const b::GraphRequirements &r){return "{\"token\":\"opaque:admitted\",\"fir_sha256\":"+cj::quote(r.firSha256)+",\"context_sha256\":"+cj::quote(r.contextSha256)+",\"engine_id\":\"graiphic.llvm.cpu\",\"target_id\":\"windows-x64\",\"profile_id\":\"native-fast\"}";}
std::string graphPayload(std::string_view method,const Actual &a,const b::GraphRequirements &r) {
    return "{\"schema\":\"frog.bridge.payload\",\"schema_version\":\"0.1\",\"method\":"+cj::quote(method)+",\"phase\":\"request\",\"fir\":{\"family\":"+cj::quote(r.fir.family)+",\"version\":"+cj::quote(r.fir.version)+",\"sha256\":"+cj::quote(r.firSha256)+",\"utf8\":"+cj::quote(a.fir)+"},\"provenance\":"+a.provenance+",\"selection\":"+selection()+(method=="validate"?"":",\"receipt\":"+receipt(r))+"}";
}
void historic(const std::filesystem::path &fixtures) {
    b::Error error;b::Message message;
    for(const auto *name:{"bridge-request.json","bridge-response.json","bridge-progress.json"}) {
        need(b::decodeMessage(read(fixtures/name),message,error),"Historical wire-0.1 fixture was changed: "+error.code);
        const auto encoded=b::encodeMessage(message);b::Message again;need(b::decodeMessage(encoded,again,error)&&again.identity==message.identity&&again.method==message.method,"Historical message no longer round-trips");
    }
    need(!b::validatePayload(message,error),"Opaque historical fixture was mistaken for executable method admission");
    b::Description d;need(b::decodeDescription(read(fixtures/"bridge-describe.json"),d,error),"Historical description is no longer admitted");
    need(d.schemaVersion=="0.1"&&d.operationsExhaustive,"Historical descriptions lost implicit exhaustive operation semantics");
    const auto encoded=b::encodeDescription(d);const auto root=parse(encoded);
    need(!root.member("engine_id")&&!root.member("commands")&&!root.member("operations_exhaustive"),"Encoding 0.1 silently added 0.2 members");
    b::Message describe;describe.identity={"request","session",1};need(b::validatePayload(describe,error),"Legacy describe {} request was refused");
    describe.kind="response";describe.status="ok";describe.payloadJson=encoded;
    need(b::validatePayload(describe,error),"Legacy describe response must remain a direct Description object");
}
void strictWire() {
    b::Message message;message.identity={"request","session",std::numeric_limits<std::uint64_t>::max()};b::Error error;
    auto encoded=b::encodeMessage(message);b::Message decoded;need(b::decodeMessage(encoded,decoded,error)&&decoded.identity==message.identity,"Generation lost exact uint64_t identity");
    auto root=parse(encoded);root.object["generation"].string="9007199254740993";need(b::decodeMessage(cj::encode(root),decoded,error)&&decoded.identity.generation==9007199254740993ULL,"Generation was rounded through binary64");
    root.object["generation"].string="1.00e1";need(b::decodeMessage(cj::encode(root),decoded,error)&&decoded.identity.generation==10,"An exact JSON integer spelling was refused");
    for(const auto *bad:{"1.5","-1","18446744073709551616"}){root.object["generation"].string=bad;need(!b::decodeMessage(cj::encode(root),decoded,error),"An invalid or overflowing generation was admitted");}
    need(!b::decodeMessage("{\"schema\":1,\"schema\":2}",decoded,error)&&error.code=="bridge.json","Duplicate keys bypassed the shared strict reader");
    auto malformed=encoded;malformed.insert(1,1,static_cast<char>(0xff));need(!b::decodeMessage(malformed,decoded,error),"Malformed UTF-8 was accepted");
    root=parse(encoded);root.object["protocol_version"]=cj::string("0.2");need(!b::decodeMessage(cj::encode(root),decoded,error)&&error.code=="bridge.version","Future wire version was reinterpreted as 0.1");
    root=parse(encoded);root.object["future"]=cj::boolean(true);need(!b::decodeMessage(cj::encode(root),decoded,error),"Unknown envelope member was ignored");
    b::Limits limit;limit.maximumMessageBytes=8;need(!b::decodeMessage(encoded,decoded,error,limit)&&error.code=="bridge.message_limit","Message budget was checked after admission");
    root=parse(encoded);root.object["kind"]=cj::string("response");root.object["status"]=cj::string("ok");need(!b::decodeMessage(cj::encode(root),decoded,error),"A response omitted its required diagnostic array");
    message.kind="response";message.status="error";message.diagnostics={{"smoke.error","admit","error","Exact target refusal","source:/actual","/diagram/nodes/1","node1","in1","body"}};
    encoded=b::encodeMessage(message);need(b::decodeMessage(encoded,decoded,error)&&decoded.diagnostics.front().objectId=="node1"&&decoded.diagnostics.front().sourcePointer=="/diagram/nodes/1","Bridge diagnostic attribution was lost");
    root=parse(encoded);root.object["diagnostics"].array[0].object["version"]=cj::string("0.2");need(!b::decodeMessage(cj::encode(root),decoded,error),"Frontend Diagnostic0.2 was silently accepted as bridge Diagnostic0.1");
}
void exchanges() {
    b::Error error;b::Identity identity{"request","session",9007199254740993ULL};b::Limits limits;limits.maximumProgressMessages=1;b::Exchange exchange(identity,"validate",limits);
    b::Message message;message.identity=identity;message.method="validate";message.kind="progress";message.progress=b::Progress{"admit",0,0};
    auto stale=message;stale.identity.generation--;need(!exchange.accept(stale,error)&&error.code=="bridge.stale_message"&&exchange.discarded()==1&&!exchange.finished(),"An obsolete generation affected the active exchange");
    stale=message;stale.identity.requestId="other";need(!exchange.accept(stale,error)&&exchange.discarded()==2,"A foreign request was retargeted");
    stale=message;stale.identity.sessionId="other";need(!exchange.accept(stale,error)&&exchange.discarded()==3,"A foreign session was retargeted");
    need(exchange.accept(message,error),"The current bounded progress message was refused");need(!exchange.accept(message,error)&&error.code=="bridge.progress_limit","Progress count was unbounded");
    message.kind="response";message.status="ok";message.progress.reset();need(exchange.accept(message,error)&&exchange.finished(),"A current final response did not finish the exchange");
    need(!exchange.accept(message,error)&&error.code=="bridge.exchange_finished","A duplicate final response was admitted");
    b::Exchange cancelled(identity,"validate");cancelled.cancel();need(cancelled.finished()&&cancelled.cancelled()&&!cancelled.accept(message,error)&&error.code=="bridge.stale_message","Cancelled output was admitted");
    b::Exchange changed(identity,"validate");message.method="run";need(!changed.accept(message,error)&&error.code=="bridge.exchange_identity","A response changed its request method");
}
void lifecyclePayloads() {
    b::Message m;m.identity={"command","session",7};m.method="run";b::Error error;
    m.payloadJson="{\"schema\":\"frog.bridge.payload\",\"schema_version\":\"0.1\",\"method\":\"run\",\"phase\":\"request\",\"command\":\"pause\",\"parameters\":{},\"run_handle\":{\"token\":\"opaque:run\",\"session_id\":\"session\",\"generation\":7}}";
    need(b::validatePayload(m,error),"An explicitly scoped run command incorrectly required another complete FIR");
    auto payload=parse(m.payloadJson);payload.object["run_handle"].object["generation"].string="6";
    m.payloadJson=cj::encode(payload);need(!b::validatePayload(m,error)&&error.code=="bridge.handle_identity","A command retargeted an obsolete run generation");
    payload.object["run_handle"].object["generation"].string="7";payload.object["command"]=cj::string("start");
    m.payloadJson=cj::encode(payload);need(!b::validatePayload(m,error),"Start bypassed complete FIR/receipt admission using a run handle");
    m.method="stop";payload.object["method"]=cj::string("stop");payload.object.erase("command");payload.object.erase("parameters");
    m.payloadJson=cj::encode(payload);need(b::validatePayload(m,error),"A new stop request could not address the same session/run handle");
    payload.object["artifact_handle"]=cj::object();payload.object["artifact_handle"].object["token"]=cj::string("unrelated");
    m.payloadJson=cj::encode(payload);need(!b::validatePayload(m,error)&&error.code=="bridge.payload_member","A method accepted an unrelated known member");
    m.kind="response";m.status="ok";m.payloadJson="{\"schema\":\"frog.bridge.payload\",\"schema_version\":\"0.1\",\"method\":\"stop\",\"phase\":\"response\"}";
    need(!b::validatePayload(m,error),"A successful stop response omitted the exact affected run handle");
}
void actual(const std::filesystem::path &fixtures) {
    for(const auto *name:{"pure-add.frog","record-exact.frog","event-values.frog"}) {
        const auto a=derive(fixtures/name),before=a;b::GraphRequirements r;b::Error error;
        need(b::graphRequirements(a.provenance,a.fir,r,error),"Real API-3 provenance/FIR did not adapt: "+error.code+" "+error.message);
        need(r.documentId=="fir:bridge-smoke"&&r.sourceUri=="bridge-smoke:/actual/source.frog","Source and FIR identities were conflated");
        need(r.provenanceJson==a.provenance&&r.contextSha256==frog::native_engine::hashing::sha256Hex(a.provenance)&&r.firSha256==frog::native_engine::hashing::sha256Hex(a.fir),"Exact immutable artifact/context bytes lost their hashes");
        need(!r.profiles.empty()&&!r.operations.empty()&&r.profiles.front().version=="1","Actual independently versioned profile/operations were not retained");
        need(a.fir==before.fir&&a.provenance==before.provenance&&a.maps==before.maps&&a.source==before.source,"Protocol adaptation changed the Frontend's immutable documents");
        r.engineId="graiphic.llvm.cpu";r.targetId="windows-x64";r.profileId="native-fast";auto d=descriptor(r);
        need(b::negotiate(r,d,error),"Exact actual graph discovery failed");b::Description round;
        need(b::decodeDescription(b::encodeDescription(d),round,error)&&round.engineId==d.engineId&&round.targets.front().backendId=="llvm-22.1.7","Description0.2 lost independent engine/backend/target identities");
        auto bad=d;bad.profiles.front().version="2";need(!b::negotiate(r,bad,error)&&error.code=="bridge.profile_unsupported","A semantic profile version was inferred from FIR version");
        bad=d;bad.firFormats.front().version="0.2";need(!b::negotiate(r,bad,error)&&error.code=="bridge.fir_unsupported","Future FIR was silently selected");
        bad=d;bad.operations.clear();need(!b::negotiate(r,bad,error)&&error.code=="bridge.operation_unsupported","Exhaustive discovery omitted a required operation");
        bad.operationsExhaustive=false;need(b::negotiate(r,bad,error),"Nonexhaustive discovery prevented mandatory complete-graph validation");
        bad=d;bad.targets.front().profiles={"native-debuggable"};need(!b::negotiate(r,bad,error)&&error.code=="bridge.realization_unsupported","A realization profile silently changed");
        bad=d;bad.methods={"describe","run"};need(!b::negotiate(r,bad,error)&&error.code=="bridge.method_unsupported","Run bypassed required full-graph validate capability");
        auto context=parse(a.provenance);context.object["schema_version"]=cj::string("0.2");b::GraphRequirements rejected;
        need(!b::graphRequirements(cj::encode(context),a.fir,rejected,error)&&error.code=="bridge.version","Historical context was guessed into API-3 provenance");
        context=parse(a.provenance);context.object["closure_complete"]=cj::boolean(false);need(!b::graphRequirements(cj::encode(context),a.fir,rejected,error)&&error.code=="bridge.incomplete_closure","Incomplete dependencies became executable requirements");
        context=parse(a.provenance);context.object["operations"].array.clear();need(!b::graphRequirements(cj::encode(context),a.fir,rejected,error)&&error.code=="bridge.fir_operations","Operation truncation hid actual complete FIR objects");
        context=parse(a.provenance);context.object["fir"].object["document_id"]=cj::string("other");need(!b::graphRequirements(cj::encode(context),a.fir,rejected,error)&&error.code=="bridge.fir_identity","A different FIR identity was admitted");
        b::Message request;request.identity={"request","session",7};request.method="validate";request.payloadJson=graphPayload("validate",a,r);
        need(b::validatePayload(request,error),"Complete actual validate payload failed: "+error.code+" "+error.message);
        b::Message decoded;need(b::decodeMessage(b::encodeMessage(request),decoded,error)&&decoded.payloadJson==request.payloadJson&&b::validatePayload(decoded,error),"Encoding changed exact FIR/provenance payload bytes");
        request.method="build";request.payloadJson=graphPayload("build",a,r);need(b::validatePayload(request,error),"Bound complete-graph receipt failed");
        if(std::string_view(name)=="pure-add.frog") {
            // A newer producer may emit byte-identical FIR. Its provenance is
            // still a different admission authority; the old receipt cannot
            // be reused, while explicit fresh admission remains possible.
            for(const auto *changedField:{"software_version","sha256"}) {
                auto revised=a;auto provenance=parse(a.provenance);
                auto &producer=provenance.object.at("frontend").object;
                const std::string original=producer.at(changedField).string;
                producer[changedField]=cj::string(std::string_view(changedField)=="sha256"?
                    std::string(64,original==std::string(64,'a')?'b':'a'):original+"-explicit-update");
                revised.provenance=cj::encode(provenance);b::GraphRequirements updated;
                need(b::graphRequirements(revised.provenance,revised.fir,updated,error),"An explicit Frontend identity update invalidated compatible FIR syntax");
                updated.engineId=r.engineId;updated.targetId=r.targetId;updated.profileId=r.profileId;
                need(updated.firSha256==r.firSha256&&updated.contextSha256!=r.contextSha256&&updated.frontendFingerprint!=r.frontendFingerprint,
                    "Producer identity did not invalidate provenance admission independently of unchanged FIR bytes");
                need(b::negotiate(updated,d,error),"Compatible explicit producer update silently changed FIR/profile/wire negotiation");
                auto stale=graphPayload("build",revised,updated);
                const auto at=stale.rfind(receipt(updated));need(at!=std::string::npos,"Cannot identify the exact receipt in the owned test payload");
                stale.replace(at,receipt(updated).size(),receipt(r));request.payloadJson=std::move(stale);
                need(!b::validatePayload(request,error)&&error.code=="bridge.receipt_identity",
                    "An old receipt authorized a changed Frontend producer/provenance");
                request.method="validate";request.payloadJson=graphPayload("validate",revised,updated);
                need(b::validatePayload(request,error),"The explicit updated-producer full-graph validation request was refused");
                request.method="build";request.payloadJson=graphPayload("build",revised,updated);
                need(b::validatePayload(request,error),"A freshly bound updated-producer receipt did not satisfy the existing public payload contract");
                need(revised.fir==a.fir&&a.provenance==before.provenance,"Identity regression mutated the original admitted artifact/provenance");
            }
        }
        request.method="build";request.payloadJson=graphPayload("build",a,r);
        auto payload=parse(request.payloadJson);payload.object["receipt"].object["fir_sha256"]=cj::string(std::string(64,'0'));
        request.payloadJson=cj::encode(payload);need(!b::validatePayload(request,error)&&error.code=="bridge.receipt_identity","An admission receipt was reused for different FIR bytes");
        request.kind="response";request.status="ok";request.payloadJson="{\"schema\":\"frog.bridge.payload\",\"schema_version\":\"0.1\",\"method\":\"build\",\"phase\":\"response\",\"artifact_handle\":{\"token\":\"opaque:artifact\"}}";
        need(b::validatePayload(request,error),"A successful build omitted its bounded opaque artifact handle");
        request.status="error";request.payloadJson="{}";need(b::validatePayload(request,error),"A refused bridge response required a fabricated artifact");
    }
}
}
int main(int argc,char **argv) {
    try {need(argc==3,"Usage: bridge-protocol-smoke <ecosystem-fixtures> <real-frontend-fixtures>");historic(argv[1]);strictWire();exchanges();lifecyclePayloads();actual(argv[2]);actualDraft();
        std::cout<<"bridge_protocol_smoke PASS checks="<<checks<<" (historical wire + actual API3 FIR/provenance)\n";return 0;
    }catch(const std::exception &e){std::cerr<<"bridge_protocol_smoke FAIL: "<<e.what()<<'\n';return 1;}
}
