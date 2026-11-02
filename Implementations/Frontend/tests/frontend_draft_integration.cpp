// SPDX-License-Identifier: Apache-2.0
#include <frog/frontend.h>
#include "frog_native_engine/DraftFrontendPreparation.hpp"
#include "frog_native_engine/FunctionCatalog.hpp"
#include "frog_native_engine/FunctionPortIdentities.hpp"
#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/OccurrenceReference.hpp"
#include "frog_native_engine/SemaphoreReference.hpp"
#include "frog_native_engine/RendezvousReference.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "../src/CanonicalJsonSupport.hpp"
#include <algorithm>
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
int main() {
    try {
        Context context;
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
        auto historical=scalar("f64");
        need(revision(serializeFrogDraftData(historical))=="2","The finite historical fixture is no longer a real revision-2 writer case");
        qualify(context,historical);

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
