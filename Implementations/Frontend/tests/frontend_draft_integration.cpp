// SPDX-License-Identifier: Apache-2.0
#include <frog/frontend.h>
#include "frog_native_engine/DraftFrontendPreparation.hpp"
#include "frog_native_engine/CanonicalExecutionIr.hpp"
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"
#include "frog_native_engine/FrogDocumentFormat.hpp"
#include "frog_native_engine/DraftTypedConnections.hpp"
#include "frog_native_engine/FunctionCatalog.hpp"
#include "frog_native_engine/FunctionPortIdentities.hpp"
#include "frog_native_engine/ApplicationControlSourceCodec.hpp"
#include "frog_native_engine/ApplicationControlSchema.hpp"
#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/OccurrenceReference.hpp"
#include "frog_native_engine/SemaphoreReference.hpp"
#include "frog_native_engine/RendezvousReference.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "frog_native_engine/graph/WaveformGraph.hpp"
#include "frog_native_engine/graph/XYGraph.hpp"
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
    explicit Context(std::uint64_t maximumSourceBytes=0) {frog_frontend_config config{};config.struct_size=sizeof(config);config.limits.max_source_bytes=maximumSourceBytes;need(frog_frontend_context_create(FROG_FRONTEND_API_VERSION,&config,&value)==FROG_FRONTEND_OK,"C API context creation failed");}
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
void studioForAndFixedQuotient(Context &context) {
    FrogDraftData draft;draft.title="Studio For with fixed I32 Quotient & Remainder";
    auto loop=operation("loop","structures.for_loop");
    loop.kind="structure";
    draft.diagramNodes={literal("n","i64","-5"),loop};
    wire(draft,"count","n","value","loop","count");
    indicator(draft,"i64","loop","iteration");
    const auto insideLoop=[](auto &item) {
        item.diagramOwnerStructureId="loop";
        item.diagramOwnerRegionId="body";
    };
    insideLoop(draft.diagramNodes.back());
    insideLoop(draft.diagramEdges.back());
    const auto verifyPolicy=[&](const FrogDraftData &sourceDraft) {
        const auto result=qualify(context,sourceDraft);
        need(result.fir.find(canonicalStudioForCountProfile)!=std::string::npos,
            "Studio For count policy is absent from the real FIR");
        const auto prepared=prepareDraftFrontend(serializeFrogDraftData(sourceDraft),"memory:/studio-for.frog",{},{});
        need(prepared.accepted(),"The authored For did not prepare Source");
        const CanonicalIrReadOptions options{"memory:/for-source.frog",{}};
        const auto fir=CanonicalExecutionIr::derive(*prepared.source,"fir:studio-count-policy",options);
        need(fir&&fir->structures().size()==1&&fir->structures().front().clampNegativeCount,
            "The Studio For policy was lost in its executable projection");
        auto strictSource=prepared.source->root();
        auto &nodes=strictSource.object.at("diagram").object.at("nodes").array;
        const auto owner=std::find_if(nodes.begin(),nodes.end(),[](const auto &node){
            const auto *id=node.member("id");return id&&id->string=="loop";
        });
        need(owner!=nodes.end(),"Studio Source lost the For owner");
        auto &fields=owner->object;
        need(fields.erase("for_loop_count_profile")==1,"Studio Source lacks its explicit For policy");
        auto publicSource=FrogSourceDocument::read(canonical_json::encode(strictSource));
        need(bool(publicSource),"The Source envelope could not be reopened");
        const auto strict=CanonicalExecutionIr::derive(*publicSource,"fir:strict",options);
        need(strict&&strict->structures().size()==1&&!strict->structures().front().clampNegativeCount,
            "A For without the explicit policy must keep the strict public rule");
        fields["for_loop_count_profile"]=canonical_json::string("frog.studio.for.count-zero-floor@999");
        publicSource=FrogSourceDocument::read(canonical_json::encode(strictSource));
        need(bool(publicSource),"The Source envelope could not be reopened with an unknown policy");
        std::vector<CanonicalIrDiagnostic> diagnostics;
        need(!CanonicalExecutionIr::derive(*publicSource,"fir:unknown-count-policy",options,&diagnostics)&&
            std::any_of(diagnostics.begin(),diagnostics.end(),[](const auto &d){return d.code=="IR_FOR_COUNT_PROFILE";}),
            "An unknown count policy must never enable execution");
        return *fir;
    };
    verifyPolicy(draft);

    auto liveCount=draft;
    const auto *control=findWidgetCatalogEntryByClassId("frog.widgets.numeric_control");
    need(control,"Numeric control data class is absent");
    auto input=makeWidgetData("count-control",*control,{40,30,100,28});
    input.valueType="i32";
    DiagramNode terminal;terminal.id="n";terminal.kind="widget_value";terminal.widgetId=input.instanceId;
    terminal.type=diagramTerminalValueType(input);
    liveCount.widgets.addObject({input.instanceId,input.bounds,std::move(input)},true);
    liveCount.diagramNodes.front()=std::move(terminal);
    const auto liveFir=verifyPolicy(liveCount);
    for(const auto *id:{"n","loop"}) {
        const auto portId=std::string_view(id)=="n"?"value":"count";
        const auto node=std::find_if(liveFir.nodes().begin(),liveFir.nodes().end(),[&](const auto &n){
            return n.sourceNodeId==id;
        });
        need(node!=liveFir.nodes().end()&&std::any_of(node->ports.begin(),node->ports.end(),[&](const auto &port){
            return port.id==portId&&port.typeRef=="i32";
        }),"An exact live I32 counter must keep its type on both source and receiver");
    }
    need(std::none_of(liveFir.nodes().begin(),liveFir.nodes().end(),[](const auto &node){
        return node.kind==canonicalNumericCoercionId;
    }),"An exact live I32 For count unexpectedly acquired a coercion");

    auto divisor=literal("divisor","i32","16");
    auto remainder=operation("division","numeric.quotient_remainder");
    insideLoop(divisor);insideLoop(remainder);
    remainder.valueType="i32";
    draft.diagramNodes.push_back(std::move(divisor));
    draft.diagramNodes.push_back(std::move(remainder));
    const auto resultWire=draft.diagramEdges.back();
    draft.diagramEdges.pop_back();
    wire(draft,"iteration","loop","iteration","division","x");
    insideLoop(draft.diagramEdges.back());
    wire(draft,"divisor-value","divisor","value","division","y");
    insideLoop(draft.diagramEdges.back());
    // Original I64 output remains as a representation witness downstream.
    draft.diagramEdges.push_back(resultWire);

    const auto bytes=serializeFrogDraftData(draft);
    const auto reopened=readFrogDraftData(bytes);
    need(bool(reopened),"The authored Quotient & Remainder could not be reopened");
    need(std::any_of(reopened->diagramNodes.begin(),reopened->diagramNodes.end(),[](const auto &node){
        return node.id=="division"&&node.valueType=="i32";
    }),"Save/Open lost the imported operation's representation");
    binding::TypeResolver resolver(reopened->diagramNodes,reopened->diagramEdges,reopened->widgets);
    const auto type=[](std::string_view name){return binding::TypeRef::primitive(name);};
    const auto sourceType=resolver.resolve({"loop","iteration"}).resolution;
    const auto inputType=resolver.resolve({"division","x"}).resolution;
    const auto sameType=resolver.resolve({"division","y"}).resolution;
    const auto outputType=resolver.resolve({"division","quotient"}).resolution;
    need(sourceType.type==type("i64")&&inputType.type==type("i32")&&
        inputType.status==binding::ResolutionStatus::Coercion&&sameType.type==type("i32")&&
        sameType.status==binding::ResolutionStatus::Compatible&&outputType.type==type("i32"),
        "Fixed Quotient & Remainder must keep I32 while the original For index stays I64");
    verifyPolicy(*reopened);
    const auto prepared=prepareDraftFrontend(bytes,"memory:/quotient.frog",{},{});
    const auto fir=CanonicalExecutionIr::derive(*prepared.source,"fir:quotient",{"memory:/quotient.frog",{}});
    need(fir&&std::count_if(fir->nodes().begin(),fir->nodes().end(),[](const auto &node){
        return node.kind==canonicalNumericCoercionId&&node.sourceNodeId=="division"&&
            node.ports.size()==2&&node.ports.front().typeRef=="i64"&&node.ports.back().typeRef=="i32";
    })==1,"The authored mismatch must materialize one I64 to I32 conversion, after the For terminal");

    draft.diagramNodes.back().valueType.clear();
    binding::TypeResolver promoted(draft.diagramNodes,draft.diagramEdges,draft.widgets);
    need(promoted.resolve({"division","quotient"}).resolution.type==type("i64"),
        "An ordinary Quotient & Remainder lost its normal promotion");
    draft.diagramNodes.back().valueType="i32";
    auto boolean=operation("nonnumeric","boolean.false_constant");
    insideLoop(boolean);
    draft.diagramNodes.push_back(std::move(boolean));
    draft.diagramEdges.at(1).from={"nonnumeric","value"};
    const auto refused=prepareDraftFrontend(serializeFrogDraftData(draft),"memory:/invalid-quotient.frog",{},{});
    need(!refused.accepted()&&std::any_of(refused.diagnostics.begin(),refused.diagnostics.end(),[](const auto &d){
        return d.code=="incompatible_wire_types"&&d.edgeId=="iteration";
    }),"A fixed operation must explicitly reject a Boolean wire");
}
void rootSourceBudget(Context &standard) {
    constexpr std::size_t eightMiB=8U*1024U*1024U;
    const auto small=serializeFrogDraftData(scalar("i32")),version=revision(small);
    std::string padded=small;
    padded.resize(eightMiB+1,' ');
    Context expanded(padded.size());
    need(call(standard,padded,version,"{}",true).outcome==FROG_FRONTEND_LIMIT_EXCEEDED,
        "The public root input exceeded its default eight MiB budget");
    const auto result=call(expanded,padded,version,"{}",true);
    if(result.outcome!=FROG_FRONTEND_SUCCESS)std::cerr<<result.diagnostics<<'\n';
    need(result.outcome==FROG_FRONTEND_SUCCESS && !result.fir.empty(),
        "A valid draft exactly at the explicit root budget did not produce executable IR");
    padded.push_back(' ');
    need(call(expanded,padded,version,"{}",true).outcome==FROG_FRONTEND_LIMIT_EXCEEDED,
        "The caller's explicit root budget must remain effective");
    std::string context="{}";context.resize(eightMiB+1,' ');
    const auto largeContext=call(expanded,small,version,context,true);
    need(largeContext.outcome==FROG_FRONTEND_LIMIT_EXCEEDED && largeContext.diagnostics.find("Semantic-context")!=std::string::npos,
        "A larger root budget also widened the semantic-context budget");
    padded.pop_back();padded.front()='!';
    const auto malformed=call(expanded,padded,version,"{}",false);
    need(malformed.outcome==FROG_FRONTEND_REJECTED && !malformed.diagnostics.empty(),
        "Malformed source within the explicit budget must remain a source error");
    Context oversized(kMaximumFrogDocumentBytes+1);
    padded.resize(kMaximumFrogDocumentBytes+1,' ');
    need(call(oversized,padded,version,"{}",false).outcome==FROG_FRONTEND_LIMIT_EXCEEDED,
        "The explicit root budget exceeded the format's hard ceiling");
}
void runtimePropertyCatalogue() {
    using Type=binding::TypeRef;
    const auto u16=Type::primitive("u16"),variant=Type::primitive("variant");
    const auto checkValue=[](std::string_view classId,const Type &schema,const Type &expected) {
        const auto menu=applicationControlMembers(classId,schema);
        const auto inMenu=std::find_if(menu.begin(),menu.end(),[](const auto &member){return member.id=="value";});
        const auto direct=applicationControlMember(classId,schema,"value");
        need(inMenu!=menu.end() && inMenu->runtimeAvailable && inMenu->readable && inMenu->writable &&
            inMenu->type==expected && direct && direct->runtimeAvailable && direct->type==expected,
            "Browse Properties and direct member lookup must advertise typed runtime Value");
    };
    checkValue("frog.control",{},variant);
    checkValue("frog.widgets.ring_control",u16,u16);
    const auto controls=applicationControlMember("frog.widgets.cluster_control",{},"controls");
    need(controls && controls->runtimeAvailable && controls->readable && !controls->writable &&
        controls->type==Type::array(Type::reference("frog.control")) && controls->help.find("unavailable")==std::string::npos,
        "Cluster Controls[] must describe its runtime child-reference array");
    const auto element=applicationControlMember("frog.widgets.array_control",Type::array(u16),"array_element.value");
    need(element && !element->runtimeAvailable,"Array Element is a presentation template and cannot exchange Value");
}
void pictureRingRevision() {
    const auto *entry = findWidgetCatalogEntryByClassId("frog.widgets.ring_control");
    need(entry, "Picture Ring requires the ordinary Ring widget class");
    auto ring = makeWidgetData("ring", *entry, {10, 10, 80, 40});
    FrogDraftData draft;
    draft.title = "Embedded pictures";
    draft.widgets.addObject({"ring", ring.bounds, ring}, true);
    need(revision(serializeFrogDraftData(draft)) == "3", "Ordinary Ring revision changed");
    ring.pictureRing = true;
    ring.valueType = "u16";
    RingItem picture;
    picture.id = "picture_1";
    picture.label = "Green";
    picture.imageDataUri = std::make_shared<const std::string>(
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR4nGPQOuL9HwAEjgI52cuZrgAAAABJRU5ErkJggg==");
    ring.ringItems = {picture};
    const std::string nested = serializeArrayElementWidget(ring);
    const auto reopened = deserializeArrayElementWidget(nested);
    need(revision(nested) == "11" && reopened && reopened->pictureRing &&
             reopened->ringItems.size() == 1 && reopened->ringItems.front().id == picture.id &&
             reopened->ringItems.front().imageDataUri &&
             *reopened->ringItems.front().imageDataUri == *picture.imageDataUri,
         "Nested Picture Ring lost its presentation or revision");
    FrogDraftData pictures;
    pictures.title = draft.title;
    pictures.widgets.addObject({"ring", ring.bounds, ring}, true);
    const auto current = serializeFrogDraftData(pictures);
    need(revision(current) == "11", "Picture Ring document requires revision 11");
    auto old = json(current);
    old.object["draft_revision"] = json("3");
    const auto migrated = readFrogDraftData(canonical_json::encode(old));
    need(migrated && revision(serializeFrogDraftData(*migrated)) == "11" &&
             serializeFrogDraftData(*migrated).find(*picture.imageDataUri) != std::string::npos,
         "Reading a legacy Picture Ring must retain the catalogue and save the new revision");
    old.object["draft_revision"] = json(std::to_string(kMaximumSupportedFrogDraftRevision+1U));
    need(!readFrogDraftData(canonical_json::encode(old)), "Unknown future draft revision was admitted");
}
void graphPresentationRevision() {
    const auto *xyEntry = findWidgetCatalogEntryByClassId("frog.widgets.xy_graph");
    const auto *waveformEntry = findWidgetCatalogEntryByClassId("frog.widgets.waveform_graph");
    need(xyEntry && waveformEntry, "Missing graph widget catalogue entries");
    auto xyWidget = makeWidgetData("xy", *xyEntry, {20, 30, 320, 200});
    auto waveformWidget = makeWidgetData("waveform", *waveformEntry, {360, 30, 320, 200});
    need(xyWidget.xyGraph && waveformWidget.waveformGraph, "Missing graph design state");
    const auto write = [](const WidgetInstance &widget) {
        FrogDraftData draft;
        draft.title = "Graph presentation compatibility";
        draft.widgets.addObject({widget.instanceId, widget.bounds, widget}, true);
        return serializeFrogDraftData(draft);
    };
    need(revision(write(xyWidget)) == "3" && revision(write(waveformWidget)) == "3",
        "Default graphs must retain the ordinary draft revision");

    xyWidget.xyGraph->design.legendWidth = 208;
    need(revision(write(xyWidget)) == "14", "An explicit XY legend width requires draft 14");
    xyWidget.xyGraph->design.legendWidth.reset();
    xyWidget.xyGraph->design.showKindBadge = false;
    need(revision(write(xyWidget)) == "14", "Hiding the XY kind badge requires draft 14");

    auto &waveform = waveformWidget.waveformGraph->design;
    const auto defaultPlots = waveform.plots;
    graph::Plot plot;
    plot.id = "plot-0";
    plot.name = "Plot 0";
    waveform.plots = {plot};
    need(revision(write(waveformWidget)) == "3", "A default Waveform trace must retain draft 3");
    graph::setPlotPresentation(waveform.plots.front(), graph::Presentation::Bars);
    need(revision(write(waveformWidget)) == "14", "Waveform bars require draft 14");
    waveform.plots = {plot};
    waveform.plots.front().lineStyle = graph::LineStyle::DashDot;
    need(revision(write(waveformWidget)) == "14", "A Waveform line style requires draft 14");
    waveform.plots = {plot};
    waveform.plots.front().interpolation = graph::Interpolation::Staircase;
    need(revision(write(waveformWidget)) == "14", "Waveform interpolation requires draft 14");
    waveform.plots = {plot};
    waveform.plots.front().symbol = graph::Symbol::Square;
    need(revision(write(waveformWidget)) == "14", "A Waveform point symbol requires draft 14");
    waveform.plots = defaultPlots;
    waveform.legendWidth = 209;
    need(revision(write(waveformWidget)) == "14", "An explicit Waveform legend width requires draft 14");
    waveform.legendWidth.reset();
    waveform.showKindBadge = false;
    need(revision(write(waveformWidget)) == "14", "Hiding the Waveform kind badge requires draft 14");

    xyWidget.xyGraph->design.legendWidth = 208;
    waveform.legendWidth = 209;
    graph::setPlotPresentation(plot, graph::Presentation::Bars);
    plot.lineStyle = graph::LineStyle::DashDot;
    plot.interpolation = graph::Interpolation::Staircase;
    plot.symbol = graph::Symbol::Square;
    waveform.plots = {plot};
    FrogDraftData draft;
    draft.widgets.addObject({xyWidget.instanceId, xyWidget.bounds, xyWidget}, true);
    draft.widgets.addObject({waveformWidget.instanceId, waveformWidget.bounds, waveformWidget}, true);
    auto stored = json(serializeFrogDraftData(draft));
    for (const auto *declaredRevision : {"3", "14"}) {
        stored.object["draft_revision"] = json(declaredRevision);
        const auto reopened = readFrogDraftData(canonical_json::encode(stored));
        need(bool(reopened), "Opening an existing graph file failed");
        const auto &objects = reopened->widgets.objects();
        need(objects.size() == 2 && objects[0].widget && objects[0].widget->xyGraph &&
                objects[0].widget->xyGraph->design == xyWidget.xyGraph->design &&
                objects[1].widget && objects[1].widget->waveformGraph &&
                objects[1].widget->waveformGraph->design == waveform,
            "Graph presentation changed during migration or save/open");
        need(revision(serializeFrogDraftData(*reopened)) == "14", "A styled graph must save as draft 14");
    }
    const auto nested = serializeArrayElementWidget(waveformWidget);
    need(revision(nested) == "14", "A styled graph in a standalone widget payload requires revision 14");
    const auto reopenedNested = deserializeArrayElementWidget(nested);
    need(reopenedNested && reopenedNested->waveformGraph && reopenedNested->waveformGraph->design == waveform,
        "Reopening a revision 14 widget payload changed its graph settings");
    auto ordinaryWaveform = makeWidgetData("default-waveform", *waveformEntry, {0, 0, 320, 200});
    const auto nestedObject = json(nested);
    ordinaryWaveform.defaultData = canonical_json::encode(nestedObject.object.at("widget"));
    need(revision(write(ordinaryWaveform)) == "14", "A retained legacy graph default requires revision 14");
    stored.object["draft_revision"] = json("15");
    need(!readFrogDraftData(canonical_json::encode(stored)), "Draft 15 must remain unsupported");
}
void nativeCallAuthoringRevision() {
    auto node = operation("native-call", "connectivity.libraries.call_library_function");
    NativeCallConfiguration ordinary;
    ordinary.symbol = "sum";
    ordinary.library.value = "example.dll";
    NativeCallParameter input;
    input.id = "input";
    ordinary.arguments = {input};
    ordinary.presentation.visualParameterOrder = {input.id};
    const auto write = [&](const NativeCallConfiguration &configuration) {
        FrogDraftData draft;
        node.nativeCall = configuration;
        draft.diagramNodes = {node};
        return serializeFrogDraftData(draft);
    };
    need(revision(write(ordinary)) == "7", "Ordinary native calls still use draft 7");

    auto extended = ordinary;
    extended.arguments.front().type.fromWire = true;
    need(revision(write(extended)) == "14", "An explicit Native Call wire type requires draft 14");
    extended = ordinary;
    extended.returnDefinition.type.kind = NativeCallKind::Numeric;
    extended.returnDefinition.type.wideBoolean = true;
    need(revision(write(extended)) == "14", "A native Win32 Boolean return requires draft 14");

    NativeCallStructField arrayField;
    arrayField.fieldId = "values";
    arrayField.storage = NativeCallFieldStorage::InlineArray;
    arrayField.extent.source = NativeCallCapacitySource::Constant;
    arrayField.extent.constant = 4;
    extended = ordinary;
    auto &argument = extended.arguments.front().type;
    argument.arrayFieldContracts = {arrayField};
    need(revision(write(extended)) == "14", "Remembered ABI array field contracts require draft 14");
    argument.arrayFieldContracts.clear();
    argument.kind = NativeCallKind::Cluster;
    argument.adaptedType = binding::parseType("cluster<\"values\":array<i32>>").type;
    argument.structLayout = {"Samples", 16, 4, 0, {arrayField}};
    const auto bytes = write(extended);
    need(revision(bytes) == "14", "A native inline array layout requires draft 14");
    auto stored = json(bytes);
    stored.object["draft_revision"] = json("7");
    std::string error;
    const auto reopened = readFrogDraftData(canonical_json::encode(stored), &error);
    if (!reopened) throw std::runtime_error("Legacy Native Call failed to open: " + error);
    need(reopened->diagramNodes.size() == 1 && reopened->diagramNodes.front().nativeCall == extended,
        "Previously saved native struct settings changed during migration");
    need(revision(serializeFrogDraftData(*reopened)) == "14", "A migrated native struct must save as draft 14");
}
void composedControlTypeMetadata() {
    namespace codec=application_control_source;
    using Type=binding::TypeRef;
    const auto ring=[](unsigned count) {
        auto type=Type::primitive("u16");type.choiceKind=binding::ChoiceKind::Ring;
        for(unsigned i=0;i<count;++i)type.choices.push_back({"choice-"+std::to_string(i),
            "Serialized colour for component "+std::to_string(i),i,false,true});
        return type;
    };
    const auto cluster=[](std::vector<Type> fields) {
        std::vector<std::string> ids,names;
        for(std::size_t i=0;i<fields.size();++i){ids.push_back("field-"+std::to_string(i));names.push_back("Field "+std::to_string(i));}
        auto type=Type::cluster(std::move(ids),std::move(fields));type.fieldNames=std::move(names);return type;
    };
    const auto pins=cluster(std::vector<Type>(4,ring(3)));
    const auto row=cluster({ring(10),ring(10),ring(10),ring(10),pins});
    const auto board=cluster(std::vector<Type>(12,row));
    const auto schema=codec::writeType(board);
    need(canonical_json::encode(schema).size()>64U*1024U && board.canonical().size()<64U*1024U,
        "The composite fixture must exceed 64 KiB in full, with bounded individual spellings");
    const auto decoded=codec::readType(schema);
    need(decoded && canonical_json::exact(schema,codec::writeType(*decoded)),
        "Strict read/write lost a field or a choice in the 12-row control type");
    auto broken=schema;
    auto &lastRing=broken.object.at("children").array.back().object.at("children").array.back().object.at("children").array.back();
    lastRing.object.at("choices").array.back().object.at("value")=canonical_json::string("invalid");
    need(!codec::readType(broken),"An invalid choice in the last nested Ring escaped type validation");
    broken=schema;broken.object.at("children").array.back().object.at("canonical")=canonical_json::string("u16");
    need(!codec::readType(broken),"A mismatched final child escaped canonical type validation");
    auto deep=Type::primitive("u16");for(unsigned i=0;i<33;++i)deep=Type::array(std::move(deep));
    need(!codec::readType(codec::writeType(deep)),"The type spelling's depth ceiling must remain effective");
    auto limit=codec::writeType(ring(4095));
    need(bool(codec::readType(limit)),"One type with 4095 choices is within the structural budget");
    limit.object.at("choices").array.push_back(schema.object.at("children").array.front().object.at("children").array.front().object.at("choices").array.front());
    need(!codec::readType(limit),"One type with 4096 choices exceeds the structural budget");

    ApplicationControlNode symbol;symbol.kind=ApplicationControlKind::Reference;
    symbol.target={ApplicationControlTargetKind::Unbound,{},{}};symbol.expectedClass="frog.widgets.cluster_control";symbol.valueType=board;
    auto config=codec::writeConfiguration(symbol);
    auto unrelated=canonical_json::array();unrelated.array.resize(4096);
    config.object["extension"]=std::move(unrelated);
    need(!codec::preflight(config),"Unrelated metadata incorrectly received the serialized type allowance");
    auto model=operation("board-model","application_control.reference");model.applicationControl=symbol;
    auto members=operation("board-controls","application_control.property_node");
    symbol.kind=ApplicationControlKind::Property;symbol.properties={{"children","controls",ApplicationControlPropertyMode::Read}};
    members.applicationControl=symbol;
    FrogDraftData draft;draft.title="Twelve rows of controls";draft.diagramNodes={std::move(model),std::move(members)};
    // A fixed text literal selects the same composed adapter as the complete game.
    draft.diagramNodes.push_back(operation("separator","string.other_constants.space"));
    wire(draft,"board-reference","board-model","reference_out","board-controls","reference_in");
    const auto bytes=serializeFrogDraftData(draft);
    const auto prepared=prepareDraftFrontend(bytes,"memory:/twelve-rows.frog",{},{});
    if(!prepared.accepted())for(const auto &d:prepared.diagnostics)std::cerr<<d.code<<' '<<d.message<<'\n';
    need(prepared.accepted(),"The .frog with the full composite control-reference type did not prepare");
    const auto &diagram=prepared.source->root().object.at("diagram");
    need(diagram.object.at("profile").string=="frog.studio.composed-program@1",
        "The full control type must travel through the composed Source adapter");
}
void typedControlCast(Context &context) {
    using Type=binding::TypeRef;
    ApplicationControlNode node;
    node.kind=ApplicationControlKind::ToMoreSpecificClass;
    node.expectedClass="frog.control";
    node.target={ApplicationControlTargetKind::Unbound,{},{}};
    const auto config=application_control_source::writeConfiguration(node);
    need(application_control_source::readConfiguration(config)==node,"Control cast changed during source round trip");
    const auto source=Type::reference("frog.control");
    const auto ring=Type::reference("frog.widgets.ring_control",Type::primitive("u16"));
    const auto cluster=Type::reference("frog.widgets.cluster_control");
    constexpr auto pointer="/diagram/nodes/2/application_control";
    for(const auto &target:{ring,cluster}) {
        const auto descriptor=canonicalObjectReferenceDescriptor(config,pointer,source,nullptr,target);
        need(descriptor && descriptor->kind==CanonicalObjectReferenceKind::ToMoreSpecificClass,
            "A typed Ring or class-only Cluster model was rejected");
        const auto &ports=descriptor->ports;
        need(ports.size()==5 && ports[0].id=="reference_in" && ports[0].type==source.canonical() && ports[0].required &&
            ports[1].id=="target_class" && ports[1].type==target.canonical() && ports[1].required &&
            ports[2].id=="reference_out" && ports[2].fullTypeJson==ports[1].fullTypeJson &&
            !ports[2].input && ports[3].id=="error_in" && ports[4].id=="error_out",
            "Control cast did not preserve separate source, template, strict return and error ports");
        auto attrs=canonicalObjectReferenceAttributes(*descriptor);
        need(canonicalObjectReferenceDescriptorFromAttributes(attrs,pointer)==descriptor,
            "Sealed control cast did not round trip");
        auto &outputs=attrs.object.at("ports").array;
        outputs.at(2).object.at("schema")=outputs.at(0).object.at("schema");
        need(!canonicalObjectReferenceDescriptorFromAttributes(attrs,pointer),
            "A cast with a forged source schema on its result was accepted");
    }
    need(!canonicalObjectReferenceDescriptor(config,pointer,source,nullptr,Type::primitive("u16")) &&
        !canonicalObjectReferenceDescriptor(config,pointer,Type::primitive("u16"),nullptr,ring),
        "The value behind a control was allowed to substitute for a reference");
    node.kind=ApplicationControlKind::Reference;
    node.expectedClass=cluster.name;
    const auto unbound=canonicalObjectReferenceDescriptor(application_control_source::writeConfiguration(node),pointer);
    need(unbound && unbound->effects==std::vector<std::string>{"transport_typed_null"} &&
        unbound->ports.front().type==cluster.canonical(),"An unbound Cluster class constant required a live target");
    FrogDraftData draft;
    draft.title="Separate source and strict cast template";
    auto sourceNode=operation("source","application_control.reference");
    node.expectedClass=source.name;
    sourceNode.applicationControl=node;
    auto templateNode=operation("ring-model","application_control.reference");
    node.expectedClass=ring.name;
    node.valueType=ring.children.front();
    templateNode.applicationControl=node;
    auto cast=operation("cast","application_control.to_more_specific_class");
    node.kind=ApplicationControlKind::ToMoreSpecificClass;
    cast.applicationControl=node;
    draft.diagramNodes={std::move(sourceNode),std::move(templateNode),std::move(cast)};
    wire(draft,"source-channel","source","reference_out","cast","reference_in");
    wire(draft,"model-channel","ring-model","reference_out","cast","target_class");
    const auto bytes = serializeFrogDraftData(draft);
    need(revision(bytes) == "14", "A reference cast requires draft 14");
    auto legacy = json(bytes);
    legacy.object["draft_revision"] = json("6");
    const auto migrated = readFrogDraftData(canonical_json::encode(legacy));
    need(migrated && migrated->diagramNodes.size() == 3 && migrated->diagramNodes.back().applicationControl == node &&
            revision(serializeFrogDraftData(*migrated)) == "14",
        "Previously saved control casts must retain their configuration on migration");
    const auto result=qualify(context,draft);
    need(result.fir.find("frog.studio.object_reference.to_more_specific_class.v1")!=std::string::npos &&
        result.fir.find("target_class")!=std::string::npos,
        "Public draft derivation did not emit the native control cast and its independent model input");
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
void packedIntegralArrayLiteral(Context &context) {
    for(const auto *type:{"i8","u16","i64","u64"}) {
        FrogDraftData document;document.title="Exact packed integral Array through canonical FIR";
        document.properties.arrayStorageMode=FrogArrayStorageMode::CompactStandalone;
        auto array=literal("packed",type,"0");
        array.type="frog.studio.numeric.numeric_array_constant";
        array.numericArrayRank=2;array.numericArrayShape={2,3};
        array.numericArrayFirstIndices={0,0};array.numericArrayVisibleCounts={2,3};
        need(array.numericArrayValues.resizeNumeric(6,type),"Packed integral fixture could not allocate typed pages");
        const auto lexeme=std::string_view(type)=="i8"?"-128":std::string_view(type)=="u16"?"65535":
            std::string_view(type)=="i64"?"-9223372036854775807":"18446744073709551615";
        array.numericArrayValues.replaceAt(0,lexeme);
        array.numericArrayValues.replaceAt(5,std::string{"\""}+lexeme+'"');
        document.diagramNodes.push_back(std::move(array));
        document.diagramNodes.push_back(operation("size","array.size"));
        wire(document,"data","packed","value","size","array");
        const auto bytes=serializeFrogDraftData(document);
        const auto result=call(context,bytes,revision(bytes),"{}",true);
        if(result.outcome!=FROG_FRONTEND_SUCCESS)std::cerr<<result.diagnostics<<'\n';
        need(result.outcome==FROG_FRONTEND_SUCCESS&&result.fir.find("studio_numeric_words")!=std::string::npos,
            "The canonical FIR expanded or rejected a rank-2 packed integral Array");
        const auto copy=readFrogDraftData(bytes,nullptr,nullptr);
        need(copy&&copy->diagramNodes.front().numericArrayValues==document.diagramNodes.front().numericArrayValues,
            "Packed integral Save/Open lost exact words or authoring lexemes");
    }
}
void numericArrayAndScalarMigration(Context &context) {
    for(const auto *catalog:{"numeric.multiply","numeric.subtract"})for(const bool scalarFirst:{false,true}) {
        FrogDraftData document;document.title="I32 numeric Array and scalar admission";
        document.diagramNodes={literal("left","i32","17"),literal("right","i32","29"),
            operation("build","array.build"),literal("scalar","i32","2"),operation("binary",catalog)};
        wire(document,"build-a","left","value","build",buildArrayInputId(0));
        wire(document,"build-b","right","value","build",buildArrayInputId(1));
        wire(document,"array-wire","build","result","binary",scalarFirst?"b":"a");
        wire(document,"scalar-wire","scalar","value","binary",scalarFirst?"a":"b");
        const auto result=qualify(context,document);
        need(result.fir.find("frog.studio."+std::string(catalog)+".owned.v1")!=std::string::npos,
            "The admitted I32 Array signature selected a legacy scalar lane");
    }
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
void incompleteWireDiagnostics(Context &context) {
    auto document=scalar("f64");
    DiagramWireFragment fragment;fragment.id="loose-wire";
    fragment.source=DiagramEdgeEndpoint{"left","value"};
    fragment.points={{0,0},{20,0}};fragment.typeHint="f64";fragment.typeHintAuthoritative=true;
    fragment.routeIntent = DiagramWireRouteIntent::Manual;
    document.diagramWireFragments.push_back(fragment);
    const auto bytes=serializeFrogDraftData(document);
    const auto loaded=readFrogDraftData(bytes);
    need(loaded && loaded->diagramWireFragments.size() == 1 &&
         loaded->diagramWireFragments.front().routeIntent == DiagramWireRouteIntent::Manual,
         "Read discarded an intentionally incomplete wire or its saved routing policy");
    const auto prepared=prepareDraftFrontend(bytes,"memory:/broken-wire.frog",{},{});
    need(std::any_of(prepared.diagnostics.begin(),prepared.diagnostics.end(),[](const auto &d) {
        return d.code=="incomplete_wire_fragment"&&d.domain==DocumentDiagnosticDomain::Topology&&
            d.stage==DocumentDiagnosticStage::Validation&&d.subjectId=="loose-wire"&&
            d.sourcePath=="/diagram/wire_fragments/0"&&(d.blocksBuild||d.blocksRun);
    }),"Broken wire was not a localized, editable topology error before migration");
    for(bool derive:{false,true}) {
        const auto refused=call(context,bytes,revision(bytes),"{}",derive);
        need(refused.outcome!=FROG_FRONTEND_SUCCESS&&refused.fir.empty()&&
            refused.diagnostics.find("incomplete_wire_fragment")!=std::string::npos&&
            refused.diagnostics.find("migration.unsupported")==std::string::npos,
            "Validation/derive misclassified a broken wire as an unknown format or admitted FIR");
    }
    need(serializeFrogDraftData(document)==bytes,"Inspection mutated the authored broken wire");
    std::cout<<"INCOMPLETE_WIRE_FRONTEND_PASS localized_topology=1 read_retained=1 validate_derive_refused=1 source_immutable=1\n";
}
void wireRouteIntentPresentation(Context &context) {
    FrogDraftData document;
    document.title = "Route intent on a composed String conversion";
    auto input = operation("text", "string.constant");
    input.valueType = "string";
    input.text = "12.5tail";
    auto decimal = operation("decimal-selector", "boolean.false_constant");
    decimal.valueType = "bool";
    decimal.text = "false";
    document.diagramNodes = {std::move(input), std::move(decimal), literal("default", "f64", "0"),
        operation("conversion", "string.conversion.floating_string_to_number")};
    wire(document, "value-wire", "text", "value", "conversion", "value");
    wire(document, "decimal-wire", "decimal-selector", "value", "conversion", "use_system_decimal_point");
    wire(document, "default-wire", "default", "value", "conversion", "default_value");
    indicator(document, "f64", "conversion", "result");
    const auto originalBytes = serializeFrogDraftData(document);
    const auto original = prepareDraftFrontend(originalBytes, "memory:/route-intent.frog", {}, {});
    need(original.accepted(), "The baseline route-intent fixture did not prepare");
    const auto &baselineDiagram = *original.source->root().member("diagram");
    need(baselineDiagram.member("profile") && baselineDiagram.member("profile")->string == "frog.studio.composed-program@1",
         "The route-intent fixture must use the composed Source profile");
    const auto baselineFir = json(qualify(context, document).fir);

    document.diagramEdges[0].routeIntent = DiagramWireRouteIntent::Automatic;
    document.diagramEdges[1].routeIntent = DiagramWireRouteIntent::Manual;
    const auto saved = serializeFrogDraftData(document);
    const auto reopened = readFrogDraftData(saved);
    need(reopened && reopened->diagramEdges.size() == document.diagramEdges.size() &&
         reopened->diagramEdges[0].routeIntent == DiagramWireRouteIntent::Automatic &&
         reopened->diagramEdges[1].routeIntent == DiagramWireRouteIntent::Manual,
         "Save and Open did not retain the automatic and manual routing policies");
    const auto prepared = prepareDraftFrontend(saved, "memory:/route-intent.frog", {}, {});
    need(prepared.accepted() && prepared.originalDraft && *prepared.originalDraft == saved,
         "Saved wire policies blocked preparation or lost their authoring source");
    need(canonical_json::exact(baselineDiagram, *prepared.source->root().member("diagram")),
         "Private wire editing policies changed the composed Source diagram");
    const auto routedFir = json(qualify(context, *reopened).fir);
    need(canonical_json::exact(*baselineFir.member("unit")->member("connections"),
                               *routedFir.member("unit")->member("connections")),
         "Private wire editing policies changed executable connections");
    std::cout << "WIRE_ROUTE_INTENT_FRONTEND_PASS saved_and_reopened=1 composed_source=1 same_fir_connections=1\n";
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
        const auto diagnostics = json(rejected.diagnostics);
        const auto output = std::find_if(diagnostics.array.begin(), diagnostics.array.end(), [](const auto &entry) {
            const auto *code = entry.member("code");
            return code && code->string == "structure.output_unwired";
        });
        need(output != diagnostics.array.end(), "The public output diagnostic has no record");
        const auto *location = output->member("source");
        const auto *pointer = location ? location->member("pointer") : nullptr;
        const auto *object = location ? location->member("object_id") : nullptr;
        const auto *region = location ? location->member("region_id") : nullptr;
        need(pointer && pointer->string.ends_with("/regions/0") && object && object->string == "event" &&
            region && region->string == "change",
            "The public diagnostic must point to the actual region of the boundary owner");
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
        rootSourceBudget(context);
        studioForAndFixedQuotient(context);
        runtimePropertyCatalogue();
        pictureRingRevision();
        graphPresentationRevision();
        nativeCallAuthoringRevision();
        composedControlTypeMetadata();
        typedControlCast(context);
        incompleteWireDiagnostics(context);
        independentStructureDiagnostics(context);
        wireRouteIntentPresentation(context);
        packedArrayLiteral(context);
        packedIntegralArrayLiteral(context);
        numericArrayAndScalarMigration(context);
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
        // Attribution indexing must preserve escaped/stable identities and
        // the previous refusal to attribute a duplicate retained-source ID.
        auto indexed=scalar("i64");
        indexed.diagramNodes[0].id="left:~part/one";
        for(auto &edge:indexed.diagramEdges)
            if(edge.from.nodeId=="left")edge.from.nodeId="left:~part/one";
        const auto indexedBytes=serializeFrogDraftData(indexed);
        const auto indexedPreparation=prepareDraftFrontend(indexedBytes,"memory:/identity-index.frog",{},{});
        need(indexedPreparation.accepted(),"Indexed attribution changed semantic admission");
        const auto mapped=std::find_if(indexedPreparation.identities.begin(),indexedPreparation.identities.end(),
            [](const auto &entry){return entry.id=="left:~part/one";});
        need(mapped!=indexedPreparation.identities.end()&&mapped->originalPointer=="/diagram/nodes/0",
            "Indexed attribution lost an exact authored node pointer");
        auto duplicated=json(indexedBytes);
        auto metadata=canonical_json::object();
        metadata.object.emplace("id",canonical_json::string("left:~part/one"));
        duplicated.object.insert_or_assign("metadata",std::move(metadata));
        DocumentDiagnostic duplicateDiagnostic;duplicateDiagnostic.subjectId="left:~part/one";
        attributeDraftSourceDiagnostic(duplicateDiagnostic,indexed,duplicated);
        need(duplicateDiagnostic.sourcePath.empty(),"Ambiguous retained IDs acquired a coincidental source pointer");
        need(serializeFrogDraftData(indexed)==indexedBytes,"Attribution indexing changed source values or identities");
        std::cout<<"Draft C API: exact I64, aggregate, revision 2, Conditional, diagnostics and immutable repeatability passed\n";
        return 0;
    } catch(const std::exception &error) {std::cerr<<error.what()<<'\n';return 1;}
}
