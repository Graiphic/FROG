// SPDX-License-Identifier: Apache-2.0
#include <frog/frontend.h>
#include "frog_native_engine/DraftFrontendPreparation.hpp"
#include "frog_native_engine/DraftTypedConnections.hpp"
#include "frog_native_engine/FunctionCatalog.hpp"
#include "frog_native_engine/FunctionPortIdentities.hpp"
#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/OccurrenceReference.hpp"
#include "frog_native_engine/SemaphoreReference.hpp"
#include "frog_native_engine/RendezvousReference.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "../src/CanonicalJsonSupport.hpp"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <stdexcept>

using namespace frog::native_engine;
namespace {
using Json=source::JsonValue;
void need(bool value,const char *why) {if(!value)throw std::runtime_error(why);}
frog_frontend_utf8 view(std::string_view value) {return {value.data(),value.size()};}
std::string text(frog_frontend_utf8 value) {return value.size?std::string(value.data,value.size):std::string{};}
Json json(std::string_view value) {auto parsed=source::parseJson(value,nullptr,source::JsonNumberPolicy::PreserveLexeme);need(bool(parsed),"Result JSON is malformed");return std::move(*parsed);}
std::string revision(std::string_view bytes) {const auto root=json(bytes);const auto *v=root.member("draft_revision");need(v&&v->type==Json::Type::Number,"Writer did not identify its actual draft revision");return v->string;}
struct Result {frog_frontend_outcome outcome;std::string fir,diagnostics,provenance;};
struct Context {
    frog_frontend_context *value=nullptr;
    Context() {frog_frontend_config config{};config.struct_size=sizeof(config);need(frog_frontend_context_create(FROG_FRONTEND_API_VERSION,&config,&value)==FROG_FRONTEND_OK,"C API context creation failed");}
    ~Context(){frog_frontend_context_destroy(value);}
};
Result call(Context &context,std::string_view bytes,std::string_view sourceRevision,std::string_view semantic,bool derive,
    std::string_view sourceFormat="frog.document.draft") {
    frog_frontend_request request{};request.struct_size=sizeof(request);
    request.source_bytes=view(bytes);request.source_uri=view("memory:/draft-integration.frog");
    request.source_document_id=view("source:draft-integration");request.fir_document_id=view("fir:draft-integration");
    request.source_format_id=view(sourceFormat);request.source_format_version=view(sourceRevision);request.fir_format_version=view("0.1");request.semantic_context_json=view(semantic);
    frog_frontend_result *result=nullptr;
    const auto status=derive?frog_frontend_derive(context.value,&request,&result):frog_frontend_validate(context.value,&request,&result);
    need(status==FROG_FRONTEND_OK&&result,"C API rejected a well-formed request boundary");
    frog_frontend_result_view found{};found.struct_size=sizeof(found);
    need(frog_frontend_result_get(result,&found)==FROG_FRONTEND_OK,"C API result is unreadable");
    Result output{found.outcome,text(found.fir_json),text(found.diagnostics_json),text(found.provenance_json)};
    frog_frontend_result_destroy(result);return output;
}
Result qualify(Context &context,const FrogDraftData &document,std::string_view semantic="{}") {
    const auto bytes=serializeFrogDraftData(document),hash=hashing::sha256Hex(bytes),version=revision(bytes);
    const auto validated=call(context,bytes,version,semantic,false),derived=call(context,bytes,version,semantic,true);
    if(derived.outcome!=FROG_FRONTEND_SUCCESS)std::cerr<<derived.diagnostics<<'\n';
    need(validated.outcome==FROG_FRONTEND_SUCCESS&&derived.outcome==FROG_FRONTEND_SUCCESS,"Real draft validate/derive failed");
    need(validated.diagnostics==derived.diagnostics&&validated.fir.empty()&&!derived.fir.empty(),"Validate/derive disagree or violate their output contract");
    need(derived.provenance.find(hash)!=std::string::npos,"Provenance lost the original draft hash");
    const auto repeated=call(context,bytes,version,semantic,true);
    need(repeated.fir==derived.fir,"Same exact draft/context produced different FIR");
    need(serializeFrogDraftData(document)==bytes,"C API changed the caller's model or literal values");
    return derived;
}
DiagramNode operation(std::string id,std::string_view catalog) {
    const auto *entry=findFunctionCatalogEntry(catalog);need(entry,"Required real palette operation is absent");
    DiagramNode node;node.id=std::move(id);node.kind="primitive";node.type=functionDiagramNodeType(*entry);node.layout={150,30,48,48};return node;
}
DiagramNode literal(std::string id,std::string type,std::string value) {
    auto node=operation(std::move(id),"numeric.numeric_constant");node.valueType=std::move(type);node.text=std::move(value);return node;
}
void wire(FrogDraftData &document,std::string id,std::string from,std::string output,std::string to,std::string input) {
    document.diagramEdges.push_back({std::move(id),{std::move(from),std::move(output)},{std::move(to),std::move(input)}});
}
void indicator(FrogDraftData &document,std::string type,std::string from,std::string port) {
    const auto *entry=findWidgetCatalogEntryByClassId("frog.widgets.numeric_indicator");need(entry,"Numeric indicator data class is absent");
    auto widget=makeWidgetData("result",*entry,{250,30,100,28});widget.valueType=std::move(type);
    DiagramNode terminal;terminal.id="result-terminal";terminal.kind="widget_value";terminal.widgetId="result";terminal.type=diagramTerminalValueType(widget);
    document.widgets.addObject({"result",widget.bounds,std::move(widget)},true);document.diagramNodes.push_back(std::move(terminal));
    wire(document,"result-wire",std::move(from),std::move(port),"result-terminal","value");
}
FrogDraftData scalar(std::string type) {
    FrogDraftData document;document.title="Public draft scalar";
    document.diagramNodes={literal("left",type,type=="i64"?"9007199254740993":"17"),literal("right",type,"7"),operation("add","numeric.add")};
    wire(document,"left-wire","left","value","add","a");wire(document,"right-wire","right","value","add","b");
    indicator(document,type,"add","result");return document;
}
void legacyStructureType(Context &context) {
    auto original=json(R"frog({
      "format":"frog.document.draft","draft_revision":2,"language":"FROG",
      "title":"Legacy Enum Case identity","frontPanel":{"widgets":[]},
      "diagram":{"zoom_percent":100,"nodes":[
        {"id":"mode","kind":"primitive","type":"frog.studio.numeric.enum_constant",
         "text":"","value_type":"u16","layout":{"x":80,"y":100,"width":160,"height":34},
         "enum_constant":{"domain_id":"frog.studio.constant.mode","selected_value":0,"items":[
           {"id":"off","label":"Off","value":0,"disabled":false,"visible":true},
           {"id":"on","label":"On","value":1,"disabled":false,"visible":true}]}},
        {"id":"case","kind":"structure","structure_type":"case",
         "layout":{"x":280,"y":50,"width":320,"height":240},"boundary":{"inputs":[],"outputs":[]},
         "structure_terminals":{"selector":{"outer_port":"selector","body_port":"selected_case",
           "type":"enum<\"frog.studio.constant.mode\",u16>","enum_items":[
             {"item_id":"off","display_name":"Off","ordinal":0},
             {"item_id":"on","display_name":"On","ordinal":1}]}},
         "regions":[{"id":"off_case","matches":[{"kind":"enum_item","item":"off"}],"default":true,
             "diagram":{"nodes":[],"edges":[],"wire_fragments":[]}},
           {"id":"on_case","matches":[{"kind":"enum_item","item":"on"}],"default":false,
             "diagram":{"nodes":[],"edges":[],"wire_fragments":[]}}]}
        ],"edges":[{"id":"selector-wire","from":{"node":"mode","port":"value"},
          "to":{"node":"case","port":"selector"}}],"wire_fragments":[]}
    })frog");
    auto &nodes=original.object.at("diagram").object.at("nodes").array;
    auto &caseFields=nodes.at(1).object;
    const auto legacy=canonical_json::encode(original);
    const auto checked=call(context,legacy,revision(legacy),"{}",false);
    const auto derived=call(context,legacy,revision(legacy),"{}",true);
    if(checked.outcome!=FROG_FRONTEND_SUCCESS)std::cerr<<checked.diagnostics<<'\n';
    need(checked.outcome==FROG_FRONTEND_SUCCESS&&derived.outcome==FROG_FRONTEND_SUCCESS&&
        checked.diagnostics==derived.diagnostics&&derived.fir.find("selector-wire")!=std::string::npos,
        "Case without its redundant type must preserve its actual selector and canonical identity");

    caseFields["type"]=json("null");
    const auto malformed=canonical_json::encode(original);
    const auto rejected=call(context,malformed,revision(malformed),"{}",false);
    if(std::string_view(rejected.diagnostics).find("/diagram/nodes/1/type")==std::string::npos)
        std::cerr<<rejected.diagnostics<<'\n';
    need(rejected.outcome==FROG_FRONTEND_REJECTED&&rejected.diagnostics.find("/diagram/nodes/1/type")!=std::string::npos,
        "A malformed explicit Case type must produce an attributed diagnostic");
}
FrogDraftData resourceReference(const binding::TypeRef &type) {
    FrogDraftData document;document.title="Exact typed-null resource reference";
    for(const bool output:{false,true}) {
        const std::string id=output?"reference-result":"reference-input";
        auto widget=referenceWidget(id,type,output);
        DiagramNode terminal;terminal.id=id+"-terminal";terminal.kind="widget_value";
        terminal.widgetId=id;terminal.type=diagramTerminalValueType(widget);
        document.diagramNodes.push_back(std::move(terminal));
        document.widgets.addObject({id,widget.bounds,std::move(widget)},true);
    }
    wire(document,"reference-wire","reference-input-terminal","value","reference-result-terminal","value");
    return document;
}
FrogDraftData aggregate() {
    FrogDraftData document;document.title="Public draft Build and Index";
    document.diagramNodes={literal("left","f64","17"),literal("right","f64","29"),operation("build","array.build"),
        operation("index","array.index"),literal("position","i32","1")};
    wire(document,"build-a","left","value","build",buildArrayInputId(0));wire(document,"build-b","right","value","build",buildArrayInputId(1));
    wire(document,"array-wire","build","result","index","array");wire(document,"index-wire","position","value","index","index");
    indicator(document,"f64","index","element");return document;
}
}
namespace {
void packedArrayLiteral(Context &context) {
    const std::uint32_t count=std::getenv("FROG_TEST_PACKED_LITERAL_10M")?10'000'000U:32'768U;
    FrogDraftData document;document.title="Packed real literal through canonical FIR";
    document.properties.arrayStorageMode=FrogArrayStorageMode::CompactStandalone;
    auto array=literal("packed","f64","0");
    array.type="frog.studio.numeric.numeric_array_constant";
    array.numericArrayRank=1;array.numericArrayShape={count};
    need(array.numericArrayValues.resizeNumeric(count,"f64"),"Packed Array fixture could not allocate typed pages");
    array.numericArrayValues.replaceAt(0,"-0");
    array.numericArrayValues.replaceAt(count-1,"1.2500");
    document.diagramNodes.push_back(std::move(array));
    document.diagramNodes.push_back(operation("size","array.size"));
    wire(document,"data","packed","value","size","array");
    indicator(document,"i32","size","size");
    const auto bytes=serializeFrogDraftData(document);
    if(const auto *fixture=std::getenv("FROG_TEST_PACKED_LITERAL_FIXTURE")) {
        std::ofstream output(std::filesystem::u8path(fixture),std::ios::binary);
        output.write(bytes.data(),static_cast<std::streamsize>(bytes.size()));
        need(bool(output),"Could not write the exact packed literal fixture used by the test");
    }
    const auto start=std::chrono::steady_clock::now();
    const auto result=call(context,bytes,revision(bytes),"{}",true);
    const auto elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
    std::cout<<"PACKED_LITERAL elements="<<count<<" draft_bytes="<<bytes.size()<<" fir_bytes="<<result.fir.size()
        <<" derive_ms="<<elapsed<<" outcome="<<result.outcome<<'\n';
    if(result.outcome!=FROG_FRONTEND_SUCCESS)std::cerr<<result.diagnostics<<'\n';
    need(result.outcome==FROG_FRONTEND_SUCCESS,"Compact F64 draft did not derive FIR");
    need(result.fir.find("studio_numeric_words")!=std::string::npos&&result.fir.find("numeric-cell-bits-le-v1")!=std::string::npos,
        "The composed FIR expanded its packed Array literal");
    need(result.fir.size()<bytes.size()*2+4096,"The packed literal acquired an unbounded FIR representation");
    need(result.provenance.find(hashing::sha256Hex(bytes))!=std::string::npos,"Packed literal provenance no longer identifies the original file");
    document.diagramNodes.front().numericArrayValues.replaceAt(count-1,"2.5000");
    const auto modified=serializeFrogDraftData(document);
    const auto rerun=call(context,modified,revision(modified),"{}",true);
    need(rerun.outcome==FROG_FRONTEND_SUCCESS&&rerun.fir!=result.fir&&
        rerun.provenance.find(hashing::sha256Hex(modified))!=std::string::npos,"Edited words reused stale FIR or provenance");
    document.diagramNodes.front().numericArrayValues.replaceAt(count-1,"NaN");
    const auto invalid=serializeFrogDraftData(document);
    const auto refused=call(context,invalid,revision(invalid),"{}",true);
    need(refused.outcome!=FROG_FRONTEND_SUCCESS&&refused.fir.empty()&&
        refused.diagnostics.find("IR_CONSTANT_PACKED_ARRAY")!=std::string::npos,"A non-finite authored literal became an executable packed value");
}
void colorBoxArrayPresentation(Context &context) {
    FrogDraftData document;document.title="Typed U32 Color Box cells";
    auto colors=literal("colors","u32","0");
    colors.type="frog.studio.numeric.numeric_array_constant";colors.numericColorBox=true;
    colors.numericArrayRank=2;colors.numericArrayShape={2,3};
    colors.numericArrayFirstIndices={0,0};colors.numericArrayVisibleCounts={2,3};
    colors.numericArrayValues={"0","16711680","65280","255","16777215","4294967295"};
    document.diagramNodes={colors};
    const auto result=qualify(context,document);
    const auto bytes=serializeFrogDraftData(document);
    std::string error;const auto reopened=readFrogDraftData(bytes,&error,nullptr);
    need(reopened&&reopened->diagramNodes.size()==1,"Color Box Array cannot reopen");
    const auto &copy=reopened->diagramNodes.front();
    need(copy.numericColorBox&&copy.valueType=="u32"&&copy.numericArrayRank==2&&
        copy.numericArrayShape==colors.numericArrayShape&&copy.numericArrayValues==colors.numericArrayValues,
        "Color Box Array changed its U32 values, rank, shape or presentation");
    const auto fir=json(result.fir);
    const auto &objects=fir.object.at("unit").object.at("objects").array;
    need(objects.size()==1,"Color Box presentation introduced executable objects");
    const auto &object=objects.front();const auto &attributes=object.object.at("attributes").object;
    need(object.object.at("kind").string=="frog.core.constant"&&
        attributes.at("value_type").string=="array<u32,rank=2>"&&
        object.object.at("ports").array.front().object.at("type_ref").string=="array<u32,rank=2>",
        "Color Box presentation changed the actual Array<U32> operation or port");
    const auto &payload=attributes.at("literal_value").object;
    need(payload.at("shape").array.size()==2&&payload.at("shape").array[0].string=="2"&&
        payload.at("shape").array[1].string=="3"&&payload.at("elements").array.size()==6,
        "Derived Color Box Array lost its actual 2D shape");
    for(std::size_t i=0;i<6;++i)need(payload.at("elements").array[i].type==Json::Type::Number&&
        payload.at("elements").array[i].string==colors.numericArrayValues[i],
        "Color Box Array payload became text or changed an exact U32 value/order");
    auto plain=document;plain.diagramNodes.front().numericColorBox=false;
    static_cast<void>(qualify(context,plain));
    FrogDraftData scalarColor;scalarColor.title="Existing scalar Color Box neighbor";
    scalarColor.diagramNodes={literal("scalar-color","u32","4294967295")};
    scalarColor.diagramNodes.front().numericColorBox=true;
    static_cast<void>(qualify(context,scalarColor));
    unsigned refused=0;
    for(unsigned malformed=0;malformed<3;++malformed) {
        auto bad=json(bytes);auto &node=bad.object.at("diagram").object.at("nodes").array.front();
        if(malformed==0)node.object["value_type"]=canonical_json::string("f64");
        if(malformed==1)node.object["numeric_color_box"]=canonical_json::string("true");
        if(malformed==2)node.object["type"]=canonical_json::string("frog.studio.numeric.add");
        const auto encoded=canonical_json::encode(bad);
        for(bool derive:{false,true}) {
            const auto invalid=call(context,encoded,revision(bytes),"{}",derive);
            need(invalid.outcome!=FROG_FRONTEND_SUCCESS&&invalid.diagnostics.find("migration.presentation_invalid")!=std::string::npos,
                "Invalid Color Box flag, type or operation concealed executable state");
            ++refused;
        }
    }
    std::cout<<"COLOR_BOX_ARRAY_FRONTEND_PASS rank=2 exact_u32_values=6 validate_derive=1 save_reopen=1 negative_results="<<refused<<'\n';
}
void independentStructureDiagnostics(Context &context) {
    auto document=scalar("f64");
    auto event=operation("event","structures.event_structure");event.kind="structure";
    document.diagramNodes.push_back(event);
    need(inspectDraftTypedConnections(document).diagnostics.empty(),
        "A well-formed Timeout case changed the independent connection inspection");
    auto &authored=document.diagramNodes.back();
    authored.eventStructureCases.front().id="change";
    authored.eventStructureCases.front().eventKind="value_change";
    authored.eventStructureCases.front().eventSource="missing-control";
    authored.eventStructureCases.front().eventBindings={{"value_change","missing-control","control"}};
    DiagramCaseStructureTunnel tunnel;tunnel.id="result";tunnel.side="right";
    tunnel.outputFromStructure=true;tunnel.valueType="unknown";
    authored.caseStructureTunnels.push_back(tunnel);
    document.diagramNodes.front()=literal("left","bool","true");
    const auto bytes=serializeFrogDraftData(document);
    const auto prepared=prepareDraftFrontend(bytes,"memory:/independent-errors.frog",{},{});
    for(const auto code:{"event.invalid_configuration","structure.output_unwired","structure.unresolved_tunnel"})
        need(std::any_of(prepared.diagnostics.begin(),prepared.diagnostics.end(),[&](const auto &d){
            return (d.blocksBuild || d.blocksRun) && d.stage==DocumentDiagnosticStage::Validation && d.code==code &&
                d.nodeId=="event" && d.sourcePath.find("/diagram/nodes/")==0;
        }),"An unrelated type error hid or misattributed an independent Event/tunnel error");
    need(std::any_of(prepared.diagnostics.begin(),prepared.diagnostics.end(),[](const auto &d){
        return d.code=="event.invalid_configuration" && d.regionId=="change" &&
            d.message.find("control:missing-control")!=std::string::npos;
    }),"The Event error lost its actual missing source or stable case identity");
    for(bool derive:{false,true}) {
        const auto rejected=call(context,bytes,revision(bytes),"{}",derive);
        need(rejected.outcome!=FROG_FRONTEND_SUCCESS && rejected.fir.empty() &&
            rejected.diagnostics.find("event.invalid_configuration")!=std::string::npos &&
            rejected.diagnostics.find("structure.output_unwired")!=std::string::npos &&
            rejected.diagnostics.find("structure.unresolved_tunnel")!=std::string::npos,
            "Public validate/derive suppressed an independent structure diagnostic");
    }
    need(serializeFrogDraftData(document)==bytes,"Validation repaired or mutated the incomplete Event");
    // Repair only the actual sources and wires, then use the same inspector.
    document.diagramNodes.front()=literal("left","f64","17");
    const auto *definition=findWidgetCatalogEntryByClassId("frog.widgets.boolean_control");
    need(definition,"The real Boolean source definition is absent");
    auto widget=makeWidgetData("source",*definition,{0,0,100,30});
    document.widgets.addObject({"source",widget.bounds,std::move(widget)},true);
    auto &fixed=document.diagramNodes.back();
    fixed.eventStructureCases.front().eventSource="source";
    fixed.eventStructureCases.front().eventBindings={{"value_change","source","control"}};
    fixed.caseStructureTunnels.front().valueType="bool";
    auto value=literal("event-result","bool","true");value.diagramOwnerStructureId="event";value.diagramOwnerRegionId="change";
    document.diagramNodes.push_back(std::move(value));
    DiagramEdge edge{"event-output",{"event-result","value"},{"event","tunnel:result:input"}};
    edge.diagramOwnerStructureId="event";edge.diagramOwnerRegionId="change";document.diagramEdges.push_back(std::move(edge));
    need(inspectDraftTypedConnections(document).diagnostics.empty(),
        "The actual restored control and typed producer did not clear the endpoint errors");
    std::cout<<"INDEPENDENT_STRUCTURE_DIAGNOSTICS_PASS simultaneous_errors=4 public_validate_derive=1 source_immutable=1 restored_bindings=1\n";
}

}
int main() {
    try {
        Context context;
        independentStructureDiagnostics(context);
        packedArrayLiteral(context);
        colorBoxArrayPresentation(context);
        const auto exact=qualify(context,scalar("i64"));
        need(exact.fir.find("9007199254740993")!=std::string::npos&&exact.fir.find("i64")!=std::string::npos,"Exact I64 literal/type was narrowed");
        const auto arrays=qualify(context,aggregate());
        need(arrays.fir.find("array")!=std::string::npos&&arrays.fir.find("17")!=std::string::npos&&arrays.fir.find("29")!=std::string::npos,"Aggregate draft lost its type or distinct values");
        // Root drafts and closed callees use the same typed-wire/required-input
        // owner before any migration. Exact source pointers must survive both
        // public operations; a genuine type error is not format incompatibility.
        for(const bool typeConflict:{false,true}) {
            auto invalid=scalar("f64");
            if(typeConflict) {
                invalid.diagramNodes.front()=operation("left","boolean.false_constant");
                invalid.diagramNodes.front().valueType="bool";invalid.diagramNodes.front().text="false";
            } else std::erase_if(invalid.diagramEdges,[](const auto &edge){return edge.id=="left-wire";});
            const auto bytes=serializeFrogDraftData(invalid);
            const auto prepared=prepareDraftFrontend(bytes,"memory:/wire-invalid.frog",{},{});
            const std::string code=typeConflict?"incompatible_wire_types":"required_input_not_connected";
            const auto found=std::find_if(prepared.diagnostics.begin(),prepared.diagnostics.end(),[&](const auto &d){return d.code==code;});
            need(!prepared.accepted()&&found!=prepared.diagnostics.end()&&found->stage==DocumentDiagnosticStage::Validation&&
                found->domain==(typeConflict?DocumentDiagnosticDomain::Semantics:DocumentDiagnosticDomain::Topology)&&
                (typeConflict?(found->edgeId=="left-wire"&&found->sourcePath=="/diagram/edges/0"):
                    (found->nodeId=="add"&&found->portId=="a"&&found->sourcePath=="/diagram/nodes/2")),
                "The common root validation lost the exact typed-wire/required-input rejection");
            const auto checked=call(context,bytes,revision(bytes),"{}",false),derived=call(context,bytes,revision(bytes),"{}",true);
            const auto diagnostics=json(checked.diagnostics);
            need(checked.outcome==FROG_FRONTEND_REJECTED&&derived.outcome==FROG_FRONTEND_REJECTED&&
                checked.fir.empty()&&derived.fir.empty()&&checked.diagnostics==derived.diagnostics&&
                std::any_of(diagnostics.array.begin(),diagnostics.array.end(),[&](const auto &d){
                    const auto *actual=d.member("code"),*stage=d.member("stage"),*origin=d.member("source");
                    const auto *pointer=origin?origin->member("pointer"):nullptr;
                    return actual&&actual->string==code&&stage&&stage->string=="validate"&&pointer&&
                        pointer->string==(typeConflict?"/diagram/edges/0":"/diagram/nodes/2");
                }),"C validate/derive changed a genuine type/topology error into migration incompatibility");
            need(serializeFrogDraftData(invalid)==bytes,"Rejected semantic validation changed the invalid authored document");
        }
        // Exact public example: three schema-derived outputs with an authored
        // historical row count of one. Neither validation nor derivation may
        // rewrite that presentation count or accept an invented fourth output.
        {
            const auto fixture=std::filesystem::u8path(__FILE__).parent_path()/"fixtures"/"draft-unbundle-schema-arity.frog";
            std::ifstream input(fixture,std::ios::binary);need(bool(input),"The real Unbundle schema-arity source fixture is absent");
            const std::string bytes{std::istreambuf_iterator<char>{input},std::istreambuf_iterator<char>{}};
            auto authored=readFrogDraftData(bytes);need(bool(authored),"The real Unbundle schema-arity source did not read");
            const auto operation=std::find_if(authored->diagramNodes.begin(),authored->diagramNodes.end(),[](const auto &node){return node.id=="operation";});
            need(operation!=authored->diagramNodes.end()&&operation->clusterFunctionTerminals==1,"The fixture must retain its historical authored row count");
            const auto prepared=prepareDraftFrontend(bytes,"memory:/unbundle-schema-arity.frog",{},{});
            if(!prepared.accepted())for(const auto &issue:prepared.diagnostics)std::cerr<<issue.code<<": "<<issue.message<<'\n';
            need(prepared.accepted()&&prepared.originalDraft&&*prepared.originalDraft==bytes&&prepared.document&&
                prepared.document->diagramNodes.front().clusterFunctionTerminals==1,
                "Read-only typed connection validation rejected schema-derived Unbundle outputs or changed the authored row count");
            const auto checked=call(context,bytes,revision(bytes),"{}",false),derived=call(context,bytes,revision(bytes),"{}",true);
            need(checked.outcome==FROG_FRONTEND_SUCCESS&&derived.outcome==FROG_FRONTEND_SUCCESS&&checked.diagnostics==derived.diagnostics,
                "The real Unbundle schema-arity source did not validate/derive through C API");
            const auto ir=json(derived.fir);std::map<std::string,std::string> outputs;
            for(const auto &node:ir.object.at("unit").object.at("objects").array) {
                const auto *attributes=node.member("attributes");const auto *id=attributes?attributes->member("source_node_id"):nullptr;
                if(!id||id->string!="operation")continue;
                for(const auto &port:node.object.at("ports").array)if(port.object.at("direction").string=="out")
                    outputs.emplace(port.object.at("metadata").object.at("source_port_id").string,port.object.at("type_ref").string);
            }
            need(outputs==std::map<std::string,std::string>{{"element1","i64"},{"element2","string"},{"element3","bool"}}&&
                derived.fir.find("9007199254740993")!=std::string::npos&&derived.fir.find("kept")!=std::string::npos,
                "Schema-derived Unbundle lost one of its three exact output identities, types or source values");
            const auto wire=std::find_if(authored->diagramEdges.begin(),authored->diagramEdges.end(),[](const auto &edge){return edge.from.nodeId=="operation"&&edge.from.portId=="element3";});
            need(wire!=authored->diagramEdges.end(),"The actual third Unbundle output wire is absent");wire->from.portId="element99";
            const auto badBytes=serializeFrogDraftData(*authored);
            const auto rejected=prepareDraftFrontend(badBytes,"memory:/unbundle-unknown-port.frog",{},{});
            need(!rejected.accepted()&&std::any_of(rejected.diagnostics.begin(),rejected.diagnostics.end(),[&](const auto &issue){
                return issue.code=="invalid_diagram_edge_ports"&&issue.stage==DocumentDiagnosticStage::Validation&&issue.edgeId==wire->id;
            }),"Semantic Unbundle specialization accepted an invented output beyond the exact incoming schema");
            need(authored->diagramNodes.front().clusterFunctionTerminals==1,"Unknown-port validation rewrote the authored row count");
        }
        auto historical=json(serializeFrogDraftData(scalar("f64")));
        historical.object.erase("storage");
        historical.object["draft_revision"]=json("2");
        const auto historicalBytes=canonical_json::encode(historical);
        const auto historicalValidation=call(context,historicalBytes,"2","{}",false);
        const auto historicalFir=call(context,historicalBytes,"2","{}",true);
        need(historicalValidation.outcome==FROG_FRONTEND_SUCCESS&&historicalFir.outcome==FROG_FRONTEND_SUCCESS&&
             historicalFir.provenance.find(hashing::sha256Hex(historicalBytes))!=std::string::npos,
             "The readable revision-2 fixture lost validation, derivation or its original identity");

        auto conditional=scalar("f64");DiagramNode selector;selector.id="conditional";selector.kind="primitive";selector.type="frog.studio.structures.conditional_disable_structure";
        selector.conditionalDisableCases={{"release",false,{{{{"MODE","==","release"}}}}},{"default",true,{}}};
        auto hidden=operation("unsupported-inactive","numeric.add");hidden.type="frog.unavailable";
        hidden.diagramOwnerStructureId=selector.id;hidden.diagramOwnerRegionId="default";
        conditional.diagramNodes.push_back(std::move(selector));conditional.diagramNodes.push_back(std::move(hidden));
        constexpr std::string_view release=R"({"profile":"frog.frontend.resolution@2","conditional_disable":{"target":{"MODE":"release"}}})";
        qualify(context,conditional,release);
        const auto conditionalBytes=serializeFrogDraftData(conditional);
        const auto unconfigured=call(context,conditionalBytes,revision(conditionalBytes),"{}",true);
        need(unconfigured.outcome!=FROG_FRONTEND_SUCCESS&&!unconfigured.diagnostics.empty(),"Unresolved Conditional symbols silently selected a case");
        legacyStructureType(context);
        need(call(context,"{","2","{}",false).outcome==FROG_FRONTEND_REJECTED,"Malformed draft was admitted");
        for(const auto field:{"format","draft_revision","frontPanel"}) {
            auto mixed=canonical_json::object();mixed.object["spec_version"]=canonical_json::string("0.1");
            mixed.object[field]=std::string_view(field)=="format"?canonical_json::string("frog.document.draft"):
                std::string_view(field)=="frontPanel"?canonical_json::object():json("2");
            const auto bytes=canonical_json::encode(mixed);
            for(const auto format:{"frog.source","frog.document.draft"}) {
                const auto version=std::string_view(format)=="frog.source"?"0.1":"2";
                const auto checked=call(context,bytes,version,"{}",false,format),derived=call(context,bytes,version,"{}",true,format);
                const auto diagnostics=json(checked.diagnostics);
                need(checked.outcome==FROG_FRONTEND_REJECTED&&derived.outcome==FROG_FRONTEND_REJECTED&&
                    checked.fir.empty()&&derived.fir.empty()&&checked.diagnostics==derived.diagnostics&&
                    diagnostics.array.size()==1&&diagnostics.array.front().object.at("code").string=="preparation.authority.mixed"&&
                    diagnostics.array.front().object.at("stage").string=="read"&&diagnostics.array.front().object.at("category").string=="compatibility",
                    "Mixed public/draft source authorities did not retain their exact common envelope refusal");
            }
        }
        const auto malformed=prepareDraftFrontend("{","memory:/malformed.frog",{},{});
        need(!malformed.accepted()&&malformed.diagnostics.size()==1&&
            malformed.diagnostics.front().code=="preparation.json.invalid"&&
            malformed.diagnostics.front().stage==DocumentDiagnosticStage::Read&&
            malformed.diagnostics.front().domain==DocumentDiagnosticDomain::Integrity,
            "Malformed JSON was misclassified as storage or semantic validation");
        const auto malformedApi=call(context,"{","2","{}",false);
        const auto malformedDiagnostics=json(malformedApi.diagnostics);
        need(malformedDiagnostics.array.size()==1&&
            malformedDiagnostics.array.front().member("stage")->string=="read",
            "The public C API lost the malformed-source Read stage");
        auto caption=json(serializeFrogDraftData(scalar("f64")));
        caption.object.at("frontPanel").object.at("widgets").array.front().object.at("props").object["caption.independent"]=
            canonical_json::string("invalid");
        const auto badCaption=prepareDraftFrontend(canonical_json::encode(caption),"memory:/caption.frog",{},{});
        need(!badCaption.accepted()&&std::any_of(badCaption.diagnostics.begin(),badCaption.diagnostics.end(),[](const auto &d){
            return d.stage==DocumentDiagnosticStage::Adaptation&&d.sourcePath.ends_with("/props/caption.independent");
        }),"Malformed Caption lost its exact adaptation stage or property path");
        const auto captionBytes=canonical_json::encode(caption);
        const auto captionApi=call(context,captionBytes,revision(captionBytes),"{}",true);
        const auto captionDiagnostics=json(captionApi.diagnostics);
        need(captionApi.outcome==FROG_FRONTEND_REJECTED&&captionApi.fir.empty()&&
            std::any_of(captionDiagnostics.array.begin(),captionDiagnostics.array.end(),[](const auto &d){
                const auto *stage=d.member("stage"),*origin=d.member("source");
                const auto *pointer=origin?origin->member("pointer"):nullptr;
                return stage&&stage->string=="adaptation"&&pointer&&pointer->string.ends_with("/props/caption.independent");
            }),"The public C API lost the exact Caption adaptation diagnostic");
        for(const auto &type:{occurrenceReferenceType(),semaphoreReferenceType(),rendezvousReferenceType()}) {
            const auto reference=qualify(context,resourceReference(type));
            need(reference.fir.find(type.name)!=std::string::npos,
                "A typed-null resource reference lost its nominal type during common preparation");
        }

        auto attribution=scalar("f64");attribution.diagramNodes[2].id="add:part";
        auto tree=json(serializeFrogDraftData(attribution));
        DocumentDiagnostic diagnostic;diagnostic.subjectId="add:part:a";
        attributeDraftSourceDiagnostic(diagnostic,attribution,tree);
        need(diagnostic.nodeId=="add:part"&&diagnostic.portId=="a"&&diagnostic.sourcePath=="/diagram/nodes/2","Attribution split an actual node ID or lost its source path");
        std::cout<<"Draft C API: exact I64, aggregate, revision 2, Conditional, diagnostics and immutable repeatability passed\n";
        return 0;
    } catch(const std::exception &error) {std::cerr<<error.what()<<'\n';return 1;}
}
