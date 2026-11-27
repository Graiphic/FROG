#include "frog_native_engine/DraftSourcePreparation.hpp"
#include "frog_native_engine/DraftFrontendPreparation.hpp"
#include "frog_native_engine/FrogDocumentFormat.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/graph/WaveformGraph.hpp"
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

FrogDraftData polymorphicMember(std::string_view inputId, std::string_view outputId,
    std::string_view type, std::string_view pattern = "pattern_06") {
    auto document = child();
    FrogDraftInterface declaration;
    need(declaration.declarePort(true, inputId, type, "required") &&
        declaration.declarePort(false, outputId, type), "Instance interface declaration failed");
    document.interfaceBindingMapProfileId = "square_40x40_v1";
    document.interfaceMapLayoutPatternId = pattern;
    need(declaration.setMapSettings(document.interfaceBindingMapProfileId, pattern, {}), "Map settings failed");
    document.declaredInterface = std::move(declaration);
    auto *source = document.widgets.widgetById("source");
    auto *sink = document.widgets.widgetById("sink");
    source->publicInputId = inputId;
    sink->publicOutputId = outputId;
    source->valueType = sink->valueType = type;
    return document;
}

void polymorphicFrogChecks() {
    PolymorphicFrog family;
    family.id = "test-family";
    family.title = "Opérations";
    family.description = "Famille\nDescription";
    family.members = {{"integer", "sous dossier/entier.frog", "Numeric:Integer", "Entier\nI64"},
        {"real", "real.frog", "Numeric:Real", "Réel"}, {"same", "same.frog", "", ""}};
    family.showSelectorByDefault = true;
    const auto familyBytes = writePolymorphicFrog(family);
    need(isPolymorphicFrog(familyBytes) && readPolymorphicFrog(familyBytes) == family, "Family round trip lost data");
    auto malformed = familyBytes;
    malformed.replace(malformed.find("\"revision\":1"), 12, "\"revision\":2");
    need(!readPolymorphicFrog(malformed), "Unsupported family revision was accepted");
    auto invalid = family;
    invalid.members[1].menuName = "Numeric::Real";
    need(validatePolymorphicFrog(invalid).front().code == "polymorphic.member.menu_invalid" &&
        readPolymorphicFrog(writePolymorphicFrog(invalid)) == invalid, "Editable invalid member was lost");
    const auto integer = polymorphicMember("integer-input", "integer-output", "i64");
    const auto real = polymorphicMember("real-input", "real-output", "f64");
    const std::vector signatures{inspectPolymorphicFrogInterface(integer),
        inspectPolymorphicFrogInterface(real), inspectPolymorphicFrogInterface(real)};
    need(signatures[0].has_value() && signatures[1].has_value(), "Independent mapped connectors could not be inspected");
    const std::vector<std::string> slots{"slot_1", "slot_2"};
    const auto selected = [&](std::string_view type, std::optional<std::string> manual = {}) {
        return resolvePolymorphicFrog(family, {std::move(manual), {}}, signatures,
            {{"slot_1", binding::TypeRef::primitive(type)}}, slots);
    };
    need(selected("f64").memberIndex == 1 && selected("f64").issues.empty(), "First matching exact DBL signature should win");
    need(selected("i64").memberIndex == 0 && selected("i64").issues.empty(), "I64 was coerced to DBL");
    need(!selected("i32").memberIndex && selected("i32").issues.front().code == "polymorphic.selection.no_match", "An incompatible representation was coerced");
    need(selected("f64", "same").memberIndex == 2 && selected("f64", "same").issues.empty(), "Manual identity did not distinguish identical signatures");
    need(selected("i64", "same").memberIndex == 2 && selected("i64", "same").issues.front().code == "polymorphic.selection.type_mismatch",
        "Incompatible manual selection was dropped or ignored");
    need(selected("i64", "deleted").issues.front().code == "polymorphic.selection.member_missing", "Removed manual selection silently fell back");
    const std::vector<std::string> outputOnly{"slot_2"};
    need(resolvePolymorphicFrog(family, {}, signatures, {}, {}).memberIndex == 0 &&
        resolvePolymorphicFrog(family, {}, signatures, {}, outputOnly).memberIndex == 0,
        "An unwired call or an output-only wire did not retain the default candidate");
    auto reordered=family;
    auto reorderedSignatures=signatures;
    std::swap(reordered.members[1],reordered.members[2]);
    std::swap(reorderedSignatures[1],reorderedSignatures[2]);
    const auto reorderedChoice=resolvePolymorphicFrog(reordered,{},reorderedSignatures,
        {{"slot_1",binding::TypeRef::primitive("f64")}},slots);
    need(reorderedChoice.memberIndex && reordered.members[*reorderedChoice.memberIndex].id=="same",
        "Reordering did not update priority among equally exact candidates");
    auto changed = signatures;
    changed[1]->slots.at("slot_1").input=false;
    need(resolvePolymorphicFrog(family,{},changed,{},{}).issues.front().code ==
        "polymorphic.member.slot_mismatch", "Conflicting slot directions were accepted");
    changed=signatures;
    changed[1]->slots.erase("slot_2");
    need(resolvePolymorphicFrog(family, {"real", {}}, changed, {}, {}).issues.empty(), "Unused connector slot rejected");
    need(resolvePolymorphicFrog(family, {"real", {}}, changed, {}, slots).issues.front().code ==
        "polymorphic.selection.slot_missing", "Wired unavailable slot accepted");
    changed[1]->patternId = "other";
    need(resolvePolymorphicFrog(family, {}, changed, {}, {}).issues.front().code ==
        "polymorphic.member.connector_mismatch", "Incompatible pattern was accepted");
    auto disabled = family;
    disabled.adaptToType = false;
    need(resolvePolymorphicFrog(disabled, {}, signatures, {}, {}).issues.front().code ==
        "polymorphic.selection.manual_required", "Disabling adaptation still selected automatically");
    const PolymorphicFrogSelection fixed{{}, false, "same"};
    const auto frozen = resolvePolymorphicFrog(disabled, fixed, signatures,
        {{"slot_1", binding::TypeRef::primitive("f64")}}, slots);
    need(frozen.memberIndex == 2 && frozen.issues.empty(),
        "Turning off adaptation did not preserve the last valid concrete instance");
    const auto incompatible = resolvePolymorphicFrog(disabled, fixed, signatures,
        {{"slot_1", binding::TypeRef::primitive("i64")}}, slots);
    need(incompatible.memberIndex == 2 && incompatible.issues.front().code == "polymorphic.selection.type_mismatch",
        "A fixed instance still changed with input types");
    need(resolvePolymorphicFrog(family, fixed, signatures,
        {{"slot_1", binding::TypeRef::primitive("i64")}}, slots).memberIndex == 0,
        "Restoring adaptation did not re-enable exact input selection");

    auto owner = parent();
    auto &call = owner.diagramNodes.back();
    call.callDocumentPath = "family.frog";
    call.documentationSummary = "Help belongs to the authored call.";
    call.polymorphicFrogSelection = fixed;
    // A stale presentation cache deliberately disagrees with the actual input.
    call.callPorts[0].id = "slot_1";
    call.callPorts[1].id = "slot_2";
    call.callPorts[0].valueType = call.callPorts[1].valueType = "f64";
    owner.diagramEdges[0].to.portId = "slot_1";
    owner.diagramEdges[1].from.portId = "slot_2";
    const auto bytes = serializeFrogDraftData(owner);
    auto read = readFrogDraftData(bytes);
    need(read && read->diagramNodes.back().polymorphicFrogSelection == call.polymorphicFrogSelection, "Call intent or explicit hidden selector was lost");
    DraftSubFrogExecutionInput input;
    input.rootDocument = std::make_shared<const FrogDraftData>(std::move(*read));
    input.sourceUri = "memory:parent";
    input.sourceUtf8 = bytes;
    input.resolver = [&](const DraftDependencyRequest &request) {
        if (request.authoredReference == "family.frog") {
            need(request.ownerUri == "memory:parent", "The family reference did not use the caller as owner");
            return content("memory:family", familyBytes);
        }
        need(request.ownerUri == "memory:family", "A member reference did not use the family as owner");
        if (request.authoredReference == family.members[0].documentPath) return content("memory:integer", serializeFrogDraftData(integer));
        if (request.authoredReference == family.members[1].documentPath) return content("memory:real", serializeFrogDraftData(real));
        if (request.authoredReference == family.members[2].documentPath) return content("memory:same", serializeFrogDraftData(real));
        return DraftDependencyResult{};
    };
    const auto projected = projectDraftSubFrogExecution(input);
    if (!projected.accepted()) for (const auto &d : projected.diagnostics) std::cerr << d.code << ": " << d.message << '\n';
    need(projected.accepted() && projected.changed && projected.frozenSources.size() == 5, "Family and instance projection did not freeze its closure");
    need(std::any_of(projected.identities.begin(), projected.identities.end(),
        [](const auto &identity) { return identity.sourceUri == "memory:integer"; }), "Exact input did not choose the integer member");
    need(std::none_of(projected.document.diagramNodes.begin(), projected.document.diagramNodes.end(),
        [](const auto &node) { return node.polymorphicFrogSelection.has_value(); }), "Polymorphic authoring state escaped lowering");
    need(serializeFrogDraftData(projected.document).find(call.documentationSummary)==std::string::npos &&
        input.rootDocument->diagramNodes.back().documentationSummary==call.documentationSummary,
        "Presentation-only documentation leaked into the synthetic execution node or was lost from the authored call");
    const auto docField = bytes.find("\"summary\": \"Help belongs to the authored call.\"");
    need(docField != std::string::npos, "The fixture must exercise an explicit authored documentation field");
    auto unexpectedDoc = bytes;
    unexpectedDoc.replace(docField, std::string_view("\"summary\"").size(), "\"unexpected\"");
    input.sourceUtf8 = unexpectedDoc;
    const auto refusedDoc = projectDraftSubFrogExecution(input);
    need(!refusedDoc.accepted() && refusedDoc.diagnostics.front().code=="migration.composed_field_unavailable",
        "An undeclared documentation field bypassed exact source validation");
    input.sourceUtf8 = bytes;
    need(widgetNumericIntegerValueLexeme(*projected.document.widgets.widgetById("source")) ==
        "9007199254740993", "Projection changed the caller's exact I64");
    const auto executable=prepareDraftFrontend(bytes,input.sourceUri,{},input.resolver);
    if (!executable.accepted()) for (const auto &d:executable.diagnostics) std::cerr<<d.code<<": "<<d.message<<'\n';
    need(executable.accepted(),"Frontend admission used persisted presentation ports before automatic selection");
    const auto rejectedMember=[&](DraftDependencyResult member,std::string_view expected) {
        const auto original=input.resolver;
        const auto rejected=prepareDraftFrontend(bytes,input.sourceUri,{},[&](const DraftDependencyRequest &request) {
            if(request.ownerUri=="memory:family"&&request.authoredReference==family.members.front().documentPath)
                return member;
            return original(request);
        });
        need(!rejected.accepted()&&std::any_of(rejected.diagnostics.begin(),rejected.diagnostics.end(),
            [&](const auto &issue){return issue.code==expected;}),"An invalid family member lost its precise diagnostic");
    };
    rejectedMember({},"subfrog.source.unavailable");
    rejectedMember(content("memory:wrong-kind","{}"),"subfrog.source.invalid");
    rejectedMember(content("memory:family",familyBytes),"polymorphic.member.cycle");
    rejectedMember(content("memory:parent",bytes),"polymorphic.member.cycle");
    rejectedMember(content("memory:nested-family",familyBytes),"polymorphic.member.family");
    auto manual=owner;
    manual.diagramNodes.back().polymorphicFrogSelection->manualMemberId="real";
    const auto wrongMember=prepareDraftFrontend(serializeFrogDraftData(manual),input.sourceUri,{},input.resolver);
    need(!wrongMember.accepted()&&std::any_of(wrongMember.diagnostics.begin(),wrongMember.diagnostics.end(),[](const auto &issue) {
        return issue.code=="polymorphic.selection.type_mismatch"&&issue.callsiteId=="call";
    }),"Frontend admission did not diagnose the incompatible requested member");
    auto producer=owner.diagramNodes.back();
    producer.id="producer";
    owner.diagramNodes.push_back(std::move(producer));
    owner.diagramEdges.front().to.nodeId="producer";
    owner.diagramEdges.push_back({"producer-result",{"producer","slot_2"},{"call","slot_1"}});
    const auto chainBytes=serializeFrogDraftData(owner);
    input.rootDocument=std::make_shared<const FrogDraftData>(owner);
    input.sourceUtf8=chainBytes;
    const auto chain=projectDraftSubFrogExecution(input);
    if (!chain.accepted()) for (const auto &d : chain.diagnostics) std::cerr << d.code << ": " << d.message << '\n';
    need(chain.accepted()&&chain.changed, "Reversed authoring order used a stale type instead of the concrete upstream call");
    std::cout << "Polymorphic family format, connector resolution, call persistence and immutable lowering passed\n";
}
}
int main(int argc, char **argv) {
    try {
        if (argc == 2 && std::string_view(argv[1]) == "--polymorphic") {
            polymorphicFrogChecks();
            return 0;
        }
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

        auto extendedChild = child();
        auto childLink = linkWidgetToTypeDefinition(*extendedChild.widgets.widgetById("source"), definition, "exact.ftype");
        need(childLink.accepted(), "The revision 14 child must have an actual linked widget");
        *extendedChild.widgets.widgetById("source") = std::move(childLink.proposedWidget);
        const auto *graphEntry = findWidgetCatalogEntryByClassId(graph::classId);
        need(graphEntry != nullptr, "Missing Waveform Graph catalogue entry");
        auto graphWidget = makeWidgetData("scope", *graphEntry, {250, 20, 260, 160});
        need(bool(graphWidget.waveformGraph), "Missing Waveform Graph design");
        graphWidget.waveformGraph->design.showKindBadge = false;
        extendedChild.widgets.addObject({"scope", graphWidget.bounds, std::move(graphWidget)}, true);
        const auto extendedBytes = serializeFrogDraftData(extendedChild);
        const auto resolveDefinition = [&](const auto &query) {
            return query.kind == DraftDependencyKind::TypeDefinition &&
                query.authoredReference == "exact.ftype" && query.expectedIdentity == definition.id
                ? definitionSource : DraftDependencyResult{};
        };
        const auto prepared = prepareDraftFrontend(extendedBytes, "memory:/extended.frog", {}, resolveDefinition);
        if (!prepared.accepted()) for (const auto &d : prepared.diagnostics) std::cerr << d.code << ": " << d.message << '\n';
        need(prepared.accepted() && prepared.sourceRevision == kFrogExtendedAuthoringDraftRevision &&
            !prepared.typeDefinitionSchemaFingerprint.empty(),
            "Revision 14 must derive with its required exact Type Definition resolution");
        const auto missingDefinition = prepareDraftFrontend(extendedBytes, "memory:/extended.frog", {}, {});
        need(!missingDefinition.accepted() && std::any_of(missingDefinition.diagnostics.begin(), missingDefinition.diagnostics.end(),
            [](const auto &d) { return d.domain == DocumentDiagnosticDomain::Dependency; }),
            "Revision 14 must still refuse a missing linked Type Definition");

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
        const auto extendedCallee = content("memory:child", extendedBytes);
        input.resolver = [&](const auto &query) {
            return query.kind == DraftDependencyKind::Source && query.authoredReference == "child.frog"
                ? extendedCallee : resolveDefinition(query);
        };
        const auto extendedProjection = projectDraftSubFrogExecution(input);
        if (!extendedProjection.accepted()) for (const auto &d : extendedProjection.diagnostics) std::cerr << d.code << ": " << d.message << '\n';
        const auto froze = [&](std::string_view uri, std::string_view sha256) {
            return std::any_of(extendedProjection.frozenSources.begin(), extendedProjection.frozenSources.end(),
                [&](const auto &entry) { return entry.source.canonicalUri == uri && entry.source.sha256 == sha256; });
        };
        need(extendedProjection.accepted() && extendedProjection.changed && extendedProjection.frozenSources.size() == 3 &&
            froze(input.sourceUri, hashing::sha256Hex(rootBytes)) &&
            froze(extendedCallee.canonicalUri, extendedCallee.sha256) &&
            froze(definitionSource.canonicalUri, definitionSource.sha256),
            "The revision 14 Sub-FROG must freeze the caller, the callee and its exact linked Type Definition");
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
