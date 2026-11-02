#include "frog_native_engine/DraftSourcePreparation.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace frog::native_engine;
namespace {
void need(bool value,const char *message) {if(!value)throw std::runtime_error(message);}
DraftDependencyResult content(std::string uri,std::string bytes,std::string id={},std::string version={}) {
    auto shared=std::make_shared<const std::string>(std::move(bytes));
    return {DraftDependencyState::Present,std::move(uri),std::move(id),std::move(version),hashing::sha256Hex(*shared),std::move(shared),{}};
}
WidgetInstance numeric(std::string id,bool input,std::string port={}) {
    const auto *entry=findWidgetCatalogEntryByClassId(input?"frog.widgets.numeric_control":"frog.widgets.numeric_indicator");
    need(entry,"Public widget data catalogue is absent");
    auto widget=makeWidgetData(std::move(id),*entry,{10,10,96,28});
    widget.valueType="i64";
    need(setWidgetNumericIntegerValueLexeme(widget,"9007199254740993"),"Exact I64 initialization failed");
    if(!port.empty()) {
        if(input) {widget.publicInputId=port;widget.interfaceConnectionRequirement="required";}
        else widget.publicOutputId=port;
        widget.interfaceMapSlotId=input?"slot_1":"slot_2";
    }
    return widget;
}
void addWidget(FrogDraftData &document,WidgetInstance widget) {
    const auto id=widget.instanceId;
    DiagramNode node;node.id=id+"-terminal";node.kind="widget_value";node.widgetId=id;node.type=diagramTerminalValueType(widget);
    node.layout={20,20,48,24};document.diagramNodes.push_back(node);
    document.widgets.addObject({id,widget.bounds,std::move(widget)},true);
}
FrogDraftData child() {
    FrogDraftData document;document.title="Public captured child";
    FrogDraftInterface interface;
    need(interface.declarePort(true,"in","i64","required")&&interface.declarePort(false,"out","i64"),"Interface declaration failed");
    document.declaredInterface=std::move(interface);
    addWidget(document,numeric("source",true,"in"));addWidget(document,numeric("sink",false,"out"));
    document.diagramEdges.push_back({"pass",{"source-terminal","value"},{"sink-terminal","value"}});
    return document;
}
FrogDraftData parent() {
    FrogDraftData document;document.title="Public captured parent";
    addWidget(document,numeric("source",true));addWidget(document,numeric("sink",false));
    DiagramNode call;call.id="call";call.kind="frog_call";call.type="frog.studio.frog_call";call.callDocumentPath="child.frog";
    call.layout={120,20,48,48};
    call.callPorts={{"in","in","slot_1","same","i64",true,"required",0.,0.5},
                    {"out","out","slot_2","same","i64",false,"optional",1.,0.5}};
    document.diagramNodes.push_back(std::move(call));
    document.diagramEdges.push_back({"argument",{"source-terminal","value"},{"call","in"}});
    document.diagramEdges.push_back({"result",{"call","out"},{"sink-terminal","value"}});
    return document;
}
}
int main() {
    try {
        const auto source=content("memory:dep","{\"exact\":9007199254740993}","dep","7");
        DraftDependencyRequest request{DraftDependencyKind::Source,"memory:owner","./literal.frog","/diagram/nodes/3/call/document_path","dep","7",source.sha256};
        unsigned calls=0;
        const auto resolver=[&](const DraftDependencyRequest &received) {
            ++calls;need(received.ownerUri==request.ownerUri&&received.authoredReference==request.authoredReference&&
                received.sourcePointer==request.sourcePointer,"Resolver request lost authored identity/location");return source;
        };
        need(resolveDraftDependency(request,resolver).state==DraftDependencyState::Present&&calls==1,"Exact immutable dependency rejected");
        auto corrupt=source;corrupt.sha256=std::string(64,'0');
        need(resolveDraftDependency(request,[&](const auto&){return corrupt;}).diagnosticCode=="dependency.hash_mismatch","Altered dependency admitted");
        corrupt=source;corrupt.identity="another";
        need(resolveDraftDependency(request,[&](const auto&){return corrupt;}).diagnosticCode=="dependency.identity_mismatch","Wrong identity admitted");
        corrupt=source;corrupt.version="8";
        need(resolveDraftDependency(request,[&](const auto&){return corrupt;}).diagnosticCode=="dependency.version_mismatch","Wrong version admitted");
        struct Cancelled {};
        bool cancelled=false;try {static_cast<void>(resolveDraftDependency(request,[](const auto&)->DraftDependencyResult{throw Cancelled{};}));}catch(const Cancelled&){cancelled=true;}
        need(cancelled,"Host cancellation exception was flattened");

        TypeDefinitionDocument definition;definition.id="definition:exact-i64";definition.name="Exact I64";definition.root=numeric("prototype",true);
        const auto definitionSource=content("memory:exact.ftype",serializeTypeDefinition(definition),definition.id,"1");
        auto associated=linkWidgetToTypeDefinition(numeric("instance",true),definition,"exact.ftype");
        need(associated.accepted(),"Actual Type Definition association failed");
        FrogDraftData linked;addWidget(linked,std::move(associated.proposedWidget));
        const auto authored=serializeFrogDraftData(linked);
        auto resolved=prepareDraftTypeDefinitionSource(authored,"memory:linked",[&](const auto &query) {
            need(query.kind==DraftDependencyKind::TypeDefinition&&query.authoredReference=="exact.ftype"&&query.expectedIdentity==definition.id,"Type Definition capture changed reference");return definitionSource;
        });
        need(resolved.accepted()&&resolved.linked&&resolved.resolution.dependencies.size()==1,"Public Type Definition resolution failed");
        auto restored=readFrogDraftData(resolved.sourceUtf8);
        need(restored&&restored->widgets.widgetById("instance")&&
             widgetNumericIntegerValueLexeme(*restored->widgets.widgetById("instance"))=="9007199254740993", "Type Definition resolution lost exact I64");
        need(serializeFrogDraftData(linked)==authored,"Resolution changed authored data");
        need(!prepareDraftTypeDefinitionSource(authored,"memory:linked",{}).accepted(),"Missing Type Definition got a fallback");

        const auto callee=content("memory:child",serializeFrogDraftData(child()));
        auto root=std::make_shared<const FrogDraftData>(parent());const auto rootBytes=serializeFrogDraftData(*root);
        DraftSubFrogExecutionInput input;input.rootDocument=root;input.sourceUtf8=rootBytes;input.sourceUri="memory:parent";
        input.resolver=[&](const auto &query) {
            need(query.kind==DraftDependencyKind::Source&&query.ownerUri=="memory:parent"&&query.authoredReference=="child.frog"&&
                 query.sourcePointer=="/diagram/nodes/2/call/document_path","SubFROG resolver request lost declaration identity");return callee;
        };
        const auto projected=projectDraftSubFrogExecution(input);
        if(!projected.accepted())for(const auto &d:projected.diagnostics)std::cerr<<d.code<<": "<<d.message<<'\n';
        need(projected.accepted()&&projected.changed&&projected.frozenSources.size()==2,"Public captured SubFROG projection failed");
        need(serializeFrogDraftData(*root)==rootBytes,"SubFROG projection changed the input document");
        need(std::none_of(projected.document.diagramNodes.begin(),projected.document.diagramNodes.end(),[](const auto &n){return n.kind=="frog_call";}),"Call body was not expanded");
        need(std::any_of(projected.identities.begin(),projected.identities.end(),[](const auto &i){return i.sourceUri=="memory:child"&&!i.sourcePointer.empty();}),"Callee provenance was lost");
        input.resolver=[](const auto&){return DraftDependencyResult{};};
        const auto missing=projectDraftSubFrogExecution(input);
        need(!missing.accepted()&&missing.diagnostics.front().sourcePath=="/diagram/nodes/2/call/document_path","Missing source has no callsite attribution");
        input.resolver=[](const auto&)->DraftDependencyResult{throw Cancelled{};};cancelled=false;
        try {static_cast<void>(projectDraftSubFrogExecution(input));}catch(const Cancelled&){cancelled=true;}
        need(cancelled,"Projection flattened host cancellation");
        std::cout<<"Public immutable dependency, Type Definition and SubFROG checks passed\n";
        return 0;
    } catch(const std::exception &error) {std::cerr<<error.what()<<'\n';return 1;}
}
