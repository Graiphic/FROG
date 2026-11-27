// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/ClassDefinition.hpp"
#include "frog_native_engine/ClassProgram.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "frog_native_engine/DraftFrontendPreparation.hpp"
#include "frog_native_engine/DraftClassProjection.hpp"
#include "frog_native_engine/ClassAuthoring.hpp"
#include "frog_native_engine/DraftInterfaceInspection.hpp"
#include "frog_native_engine/DiagramBindingResolver.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <set>

namespace {
using namespace frog::native_engine;
using binding::TypeRef;

void check(bool condition, const char *message) {
    if (!condition) throw std::runtime_error(message);
}

source::JsonValue value(std::string_view text) {
    return source::parseJson(text, nullptr, source::JsonNumberPolicy::PreserveLexeme).value();
}

ClassMethod measure(std::string_view owner, std::string id, std::optional<ClassOverride> base = {}) {
    return {std::move(id), "Mesurer", {std::string(owner) + ".measure", "Mesurer.frog", 1},
        ClassVisibility::Public, ClassDispatch::Dynamic, std::move(base),
        {{"receiver", std::string(owner) + ".self", true, ClassPortKind::Object, {}},
         {"result", std::string(owner) + ".reading", false, ClassPortKind::Data, TypeRef::primitive("i64")}}};
}

ClassDefinitions sensors() {
    const ClassDependency parent{"sensor", "Capteur.fclass", 1};
    ClassDefinition sensor{"sensor", "Capteur", 1, {},
        {{"offset", "Offset", TypeRef::primitive("i64"), value("9007199254740993")}}, {measure("sensor", "measure")}};
    sensor.methods.push_back({"set-offset", "Set Offset", {"sensor.offset", "Set Offset.frog", 1},
        ClassVisibility::Public, ClassDispatch::Static,
        {}, {{"receiver", "self-in", true, ClassPortKind::Object, {}},
             {"offset", "offset", true, ClassPortKind::Data, TypeRef::primitive("i64")},
             {"updated", "self-out", false, ClassPortKind::Object, {}}}});
    for (const auto visibility : {ClassVisibility::Private, ClassVisibility::Protected}) {
        auto helper = measure("sensor", visibility == ClassVisibility::Private ? "private" : "protected");
        helper.implementation.id = "sensor." + helper.id;
        helper.implementation.path = helper.id + ".frog";
        helper.dispatch = ClassDispatch::Static;
        helper.visibility = visibility;
        sensor.methods.push_back(std::move(helper));
    }
    ClassDefinition analog{"analog", "Analogique", 1, parent,
        {{"offset", "Calibration", TypeRef::primitive("i64"), value("-3")}},
        {measure("analog", "measure-analog", ClassOverride{"sensor", "measure"})}};
    ClassDefinition digital{"digital", "Numérique", 1, parent,
        {{"resolution", "Résolution", TypeRef::primitive("u32"), value("8")}},
        {measure("digital", "measure-digital", ClassOverride{"sensor", "measure"})}};
    ClassDefinition inherited{"inherited", "Hérité", 1, parent, {}, {}};
    ClassDefinition other{"other", "Sans relation", 1, {}, sensor.fields, {}};
    return {{sensor.id, sensor}, {analog.id, analog}, {digital.id, digital},
        {inherited.id, inherited}, {other.id, other}};
}

void rejects(ClassDefinitions invalid, std::string_view code, std::string_view pointer) {
    std::vector<ClassIssue> issues;
    check(!ClassHierarchy::validate(std::move(invalid), issues), "Invalid hierarchy accepted");
    check(std::any_of(issues.begin(), issues.end(), [&](const auto &issue) {
        return issue.code == code && issue.pointer == pointer;
    }), "Missing located class diagnostic");
}

void fieldPresentationContract(ClassDefinitions definitions) {
    auto &sensor = definitions.at("sensor");
    auto &analog = definitions.at("analog");
    const auto second = prepareClassField("online", "Online", "bool", "true");
    check(second.has_value(), "Class editor refused a Boolean field");
    sensor.fields.push_back(*second);

    const auto legacyBytes = writeClassDefinition(sensor);
    const auto legacy = readClassDefinition(legacyBytes);
    check(legacy && !legacy->fields[0].layout && !legacy->fields[1].layout &&
        writeClassDefinition(*legacy) == legacyBytes, "Reading a legacy class invented field layout");

    std::vector<ClassIssue> issues;
    const auto previousHierarchy = ClassHierarchy::validate(definitions, issues);
    check(previousHierarchy && issues.empty(), "Class layout fixture has invalid semantics");
    const auto previousRecord = previousHierarchy->recordLayout("analog");
    check(previousRecord.has_value(), "Class layout fixture has no Record carrier");

    sensor.fields[0].layout = Rect{36.25, 132.5, 90.25, 24.5};
    sensor.fields[1].layout = Rect{216.5, 27.25, 36.5, 24.25};
    analog.fields[0].layout = Rect{168.25, 62.5, 108.25, 24.5};
    const auto movedBytes = writeClassDefinition(sensor);
    auto moved = readClassDefinition(movedBytes);
    check(moved && writeClassDefinition(*moved) == movedBytes, "Class layout did not round-trip exactly");
    check(moved->fields[0].layout && moved->fields[1].layout &&
        moved->fields[0].id == "offset" && moved->fields[0].layout->x == 36.25 &&
        moved->fields[0].layout->y == 132.5 && moved->fields[0].layout->width == 90.25 &&
        moved->fields[0].layout->height == 24.5 && moved->fields[1].id == "online" &&
        moved->fields[1].layout->x == 216.5 && moved->fields[1].layout->y == 27.25 &&
        moved->fields[1].layout->width == 36.5 && moved->fields[1].layout->height == 24.25,
        "Class field rectangles did not stay associated with the expected identities");
    for (auto &field : moved->fields) field.layout.reset();
    check(writeClassDefinition(*moved) == legacyBytes,
        "Moving fields changed their logical order, defaults, types or class revision");

    const auto movedHierarchy = ClassHierarchy::validate(std::move(definitions), issues);
    check(movedHierarchy && issues.empty(), "Field geometry changed the class hierarchy");
    const auto movedRecord = movedHierarchy->recordLayout("analog");
    const ClassCallTarget accessor{"sensor", "set-offset"};
    const auto oldAccessor = previousHierarchy->lowerPrivateField(accessor, "analog", "offset", true, "");
    const auto newAccessor = movedHierarchy->lowerPrivateField(accessor, "analog", "offset", true, "");
    check(movedRecord && movedRecord->schema == previousRecord->schema &&
        movedRecord->discriminants == previousRecord->discriminants &&
        oldAccessor && newAccessor && *oldAccessor == *newAccessor,
        "Changing presentation modified the existing accessor or Record carrier");

    const auto malformed = [](std::string_view layout) {
        return std::string{R"({"format":"frog.class","version":1,"id":"broken","name":"Broken","revision":1,"parent":null,"fields":[{"id":"field","label":"Field","type":"bool","default":false,"layout":)"}
            + std::string(layout) + R"(}],"methods":[]})";
    };
    std::string error;
    for (const auto layout : {R"({"x":4,"y":5,"width":0,"height":10})",
        R"({"x":4,"y":5,"width":15,"height":-10})",
        R"({"x":1e100,"y":5,"width":15,"height":10})",
        R"({"x":4,"y":5,"width":15})", R"({"x":4,"y":5,"width":15,"height":10,"extra":2})", "null"}) {
        check(!readClassDefinition(malformed(layout), &error) && error.starts_with("/fields/0/layout"),
            "Class reader failed to locate malformed presentation in the field");
    }
}

void run() {
    std::string fieldError;
    const auto preparedField = prepareClassField("offset", "Offset", "i64", "9007199254740993", &fieldError);
    check(preparedField && preparedField->defaultValue.string == "9007199254740993" && fieldError.empty(),
        "Class editor projected an exact field default through binary64");
    check(!prepareClassField("offset", "Offset", "i64", "1.5", &fieldError) && fieldError.starts_with("/default"),
        "Class editor accepted a nonintegral default");
    auto definitions = sensors();
    fieldPresentationContract(definitions);
    const auto memberDraft=makeClassMemberDraft(definitions.at("sensor"),"set-offset",{"sensor","Capteur.fclass",1});
    if(!memberDraft.accepted())throw std::runtime_error(memberDraft.issues.front().message);
    const auto actualBindings=inspectFrogDraftInterfaceBindings(memberDraft.document->declaredInterface,
        memberDraft.document->widgets,memberDraft.document->diagramNodes,memberDraft.document->properties,{});
    if(!actualBindings.empty())throw std::runtime_error("Ordinary native member authoring: "+actualBindings.front().code+": "+actualBindings.front().message);
    const auto memberRoundTrip=readFrogDraftData(serializeFrogDraftData(*memberDraft.document));
    check(memberRoundTrip&&memberRoundTrip->properties.classSource==memberDraft.document->properties.classSource,
        "New native member attribution did not survive Save/Open");
    std::vector<ClassIssue> connectorIssues;
    const auto adopted=adoptClassMemberConnector(definitions.at("sensor"),"set-offset",*memberDraft.document,connectorIssues);
    check(adopted&&connectorIssues.empty()&&writeClassDefinition(*adopted)==writeClassDefinition(definitions.at("sensor")),
        "Explicit actual connector adoption changed stable identities or signature slots");
    auto foreignMember=*memberDraft.document;foreignMember.properties.classSource.member->classId="foreign";
    check(!adoptClassMemberConnector(definitions.at("sensor"),"set-offset",foreignMember,connectorIssues)&&
        !connectorIssues.empty(),"Connector adoption accepted a member of another class");
    for (auto &[id, definition] : definitions) {
        const auto bytes = writeClassDefinition(definition);
        std::string error;
        const auto read = readClassDefinition(bytes, &error);
        if (!read) throw std::runtime_error(error);
        check(read->id == id && writeClassDefinition(*read) == bytes, "Lossy class round trip");
        definition = *read;
    }
    auto renamed = definitions;
    renamed.at("sensor").name = "Capteur renommé";
    renamed.at("sensor").methods[0].name = "Lire";
    renamed.at("sensor").methods[0].implementation.path = "Lire.frog";
    std::vector<ClassIssue> issues;
    const auto hierarchy = ClassHierarchy::validate(std::move(renamed), issues);
    check(hierarchy.has_value() && issues.empty(), "Renaming broke nominal identity");
    const ClassCallTarget measure{"sensor", "measure"}, setOffset{"sensor", "set-offset"};
    const ClassCallTarget member{"analog", "measure-analog"};
    ClassIssue overrideIssue;
    const auto override=makeClassOverride(*hierarchy,definitions.at("inherited"),{"sensor","measure"},
        "inherited.custom",{"inherited.custom.impl","Custom.frog",1},&overrideIssue);
    check(override&&override->overrides==ClassOverride{"sensor","measure"}&&override->ports[0].slot=="receiver"&&
        override->ports[0].publicPortId!="sensor.self","Override authoring did not specialize the inherited connector with stable independent identities");
    auto child=definitions.at("inherited");child.methods.push_back(*override);
    const auto overrideDraft=makeClassMemberDraft(child,override->id,{"inherited","Herite.fclass",1});
    check(overrideDraft.accepted(),"Native authoring cannot realize a specialized object connector");
    check(!makeClassOverride(*hierarchy,definitions.at("inherited"),{"sensor","private"},
        "bad.override",{"bad.override.impl","Bad.frog",1}),"Private/static method became an overridable dynamic slot");
    check(hierarchy->assignable("analog", "sensor"), "No upcast");
    std::vector<DiagramNode> objectNodes(2);
    objectNodes[0].id="child-source";objectNodes[0].kind="frog_call";
    objectNodes[0].callPorts.push_back({"value","value",{},{},TypeRef::object("analog").canonical(),false});
    objectNodes[1].id="parent-target";objectNodes[1].kind="frog_call";
    objectNodes[1].callPorts.push_back({"object","object",{},{},TypeRef::object("sensor").canonical(),true});
    objectNodes[1].callPorts.push_back({"unrelated", "unrelated", {}, {}, TypeRef::object("other").canonical(), true});
    objectNodes[1].callPorts.push_back({"payload", "payload", {}, {}, "variant", true});
    DiagramEdge objectWire;objectWire.id="object-upcast";objectWire.from={"child-source","value"};objectWire.to={"parent-target","object"};
    std::vector<DiagramEdge> objectWires{objectWire};
    FrogWidgetDataView qualified(std::span<const SceneObject>{},{},8,{},nullptr,nullptr,{},&*hierarchy);
    binding::TypeResolver qualifiedResolver(objectNodes,objectWires,qualified);
    check(qualifiedResolver.connection(objectWire).accepted(),"Existing resolver consumers cannot use a borrowed common class context");
    const auto unrelatedWire = qualifiedResolver.connection(objectWire.from, {"parent-target", "unrelated"});
    check(!unrelatedWire.accepted() && unrelatedWire.code == "class.unrelated_types",
        "An unrelated nominal receiver must report the captured hierarchy's rejection");
    const auto boxedWire = qualifiedResolver.connection(objectWire.from, {"parent-target", "payload"});
    check(boxedWire.status == binding::ResolutionStatus::Coercion && boxedWire.code == "binding.variant_box",
        "A Variant receiver must box the nominal source value");
    binding::TypeResolver unqualifiedResolver(objectNodes,objectWires,{});
    check(!unqualifiedResolver.connection(objectWire).accepted(),"A canvas-free structural pass invented inheritance without a qualified context");
    check(!hierarchy->assignable("sensor", "analog") && !hierarchy->assignable("other", "sensor"), "Unsafe nominal conversion");
    check(!hierarchy->canAccess({}, {"sensor", "protected"}) &&
        hierarchy->canAccess(member, {"sensor", "protected"}), "Incorrect protected access");
    check(!hierarchy->canAccess(member, {"sensor", "private"}) &&
        hierarchy->canAccess(setOffset, {"sensor", "private"}), "Incorrect private method access");
    check(hierarchy->dispatchTargets("sensor", measure, {}).size() == 4, "Wrong candidate closure");
    const auto targets = hierarchy->dispatchTargets("sensor", measure, {});
    check(targets.at("analog") == member && targets.at("digital") == ClassCallTarget{"digital", "measure-digital"} &&
        targets.at("sensor") == measure && targets.at("inherited") == measure, "Wrong dynamic override or inheritance");
    check(hierarchy->dispatchTargets("sensor", setOffset, {}).at("analog") == setOffset, "Incorrect static call");
    check(hierarchy->dispatchTargets("sensor", member, {}).empty(), "Invalid call through parent or override ID");
    const auto call = hierarchy->planDynamicCall("sensor", measure, {}, "/diagram/nodes/2");
    check(call && call->arms.size() == 4 && call->arms.front().implementation == member &&
        call->arms.front().concreteClassId == "analog" && call->selector.ports.back().type == "u32" &&
        call->selector.configurationSourcePointer == "/diagram/nodes/2", "Missing runtime discriminator plan");
    check(!hierarchy->planDynamicCall("sensor", setOffset, {}, "/diagram/nodes/3"), "Static member lowered as dynamic");
    const auto parentLayout = hierarchy->recordLayout("sensor");
    const auto childLayout = hierarchy->recordLayout("analog");
    const auto unrelatedLayout = hierarchy->recordLayout("other");
    check(parentLayout && childLayout && unrelatedLayout && parentLayout->schema == childLayout->schema &&
        parentLayout->schema != unrelatedLayout->schema, "Incorrect transport layouts");
    const auto offset = hierarchy->privateRecordSlot(setOffset, "sensor", "offset");
    const auto calibration = hierarchy->privateRecordSlot(member, "analog", "offset");
    check(offset && calibration && offset != calibration && !hierarchy->privateRecordSlot(member, "sensor", "offset") &&
        !hierarchy->privateRecordSlot(member, "analog", "resolution"), "Private field scope or identity is incorrect");
    const auto literal = hierarchy->lowerDefaultObject("analog", "/diagram/nodes/0");
    const auto update = hierarchy->lowerPrivateField(setOffset, "sensor", "offset", true, "/diagram/nodes/1");
    check(literal && update && literal->ports.front().type == update->ports.front().type &&
        update->ports.back().type == update->ports.front().type &&
        update->fieldPaths == std::vector<std::vector<std::string>>{{*offset}}, "Parent write does not keep the exact family carrier");
    check(!hierarchy->lowerPrivateField(member, "sensor", "offset", false, "/diagram/nodes/1"), "Private read lowered outside its owner");
    const auto sealed = canonicalObjectReferenceDescriptorFromAttributes(canonicalObjectReferenceAttributes(*update),
        update->configurationSourcePointer);
    check(sealed && *sealed == *update, "Record lowering did not pass the existing descriptor contract");
    const auto before = hierarchy->defaultRecord("analog", *parentLayout);
    check(before && before->member(*offset) && before->member(*calibration) &&
        before->member(*offset)->string == "9007199254740993" &&
        before->member(*calibration)->string == "-3" &&
        before->member("class:concrete")->string == std::to_string(parentLayout->discriminants.at("analog")),
        "Object literal lost concrete identity or exact defaults");
    auto branch = *before;
    branch.object[*offset] = value("7");
    check(before->member(*offset)->string == "9007199254740993" &&
        branch.member(*calibration)->string == "-3" &&
        branch.member("class:concrete")->string == before->member("class:concrete")->string, "Record copy changed sibling branch or concrete class");

    ClassImplementationInterface actual{"analog.measure", 1,
        {{"analog.self", true, ClassPortKind::Object, {}, "analog"},
         {"analog.reading", false, ClassPortKind::Data, TypeRef::primitive("i64"), {}}}};
    check(validateClassImplementation(definitions.at("analog"), 0, actual).empty(), "Valid implementation interface refused");
    actual.ports[0].objectClassId = "sensor";
    actual.id = "other";
    const auto invalid = validateClassImplementation(definitions.at("analog"), 0, actual);
    check(invalid.size() == 2 && invalid[0].pointer == "/methods/0/implementation" &&
        invalid[1].pointer == "/methods/0/ports/0", "Missing member connector diagnostics");

    auto bad = definitions;
    bad.erase("sensor");
    rejects(bad, "class.parent_dependency", "/parent");
    bad = definitions;
    bad.at("sensor").parent = ClassDependency{"analog", "Analogique.fclass", 1};
    rejects(bad, "class.inheritance_cycle", "/parent");
    bad = definitions;
    bad.at("analog").methods.front().ports.back().dataType = TypeRef::primitive("u64");
    rejects(bad, "class.override_signature", "/methods/0/overrides");
    bad = definitions;
    bad.at("sensor").fields[0].defaultValue = value("9223372036854775808");
    rejects(bad, "class.field_default", "/fields/0/default");
    bad = definitions;
    bad.at("sensor").methods[0].visibility = ClassVisibility::Private;
    rejects(bad, "class.private_dispatch", "/methods/0/visibility");
    check(!readClassDefinition(R"({"format":"frog.class","version":2})"), "Future format silently accepted");

    std::map<std::string, std::string> project{
        {"Analogique.fclass", writeClassDefinition(definitions.at("analog"))},
        {"Capteur.fclass", writeClassDefinition(definitions.at("sensor"))}};
    const ClassDependencyRoot root{"file:///example/main.frog", "/classes/0", {"analog", "Analogique.fclass", 1}};
    const auto resolve = [&](const DraftDependencyRequest &request) {
        DraftDependencyResult result;
        if (request.kind != DraftDependencyKind::ClassDefinition) return result;
        if (request.authoredReference == "Capteur.fclass")
            check(request.ownerUri == "file:///example/Analogique.fclass" && request.sourcePointer == "/parent",
                "Parent reference resolved relative to the wrong owner");
        if (const auto file = project.find(request.authoredReference); file != project.end()) {
            result.state = DraftDependencyState::Present;
            result.canonicalUri = "file:///example/" + file->first;
            result.bytes = std::make_shared<const std::string>(file->second);
        }
        return result;
    };
    const auto captured = captureClassHierarchy({&root, 1}, resolve);
    check(captured.hierarchy && captured.definitions.size() == 2 && captured.definitions[1].identity == "sensor" &&
        captured.definitions[1].sha256.size() == 64, "Capture did not seal the class closure");
    project.erase("Capteur.fclass");
    const auto missing = captureClassHierarchy({&root, 1}, resolve);
    check(!missing.hierarchy && missing.issues.size() == 1 && missing.issues[0].code == "class.dependency_unavailable" &&
        missing.issues[0].sourceUri == "file:///example/Analogique.fclass" && missing.issues[0].pointer == "/parent",
        "Missing parent not attributed to the correct source");
    auto changed = definitions.at("sensor");
    ++changed.revision;
    project.emplace("Capteur.fclass", writeClassDefinition(changed));
    const auto incompatible = captureClassHierarchy({&root, 1}, resolve);
    check(!incompatible.hierarchy && incompatible.issues.size() == 1 &&
        incompatible.issues[0].code == "class.dependency_identity" && incompatible.issues[0].pointer == "/parent",
        "A changed revision was silently substituted");
}

void actualMembers() {
    auto definitions=sensors();
    std::map<std::string,std::string,std::less<>> files;
    for(auto &[id,definition]:definitions) {
        for(auto &method:definition.methods) {
            method.implementation.path=method.implementation.id+".frog";
            FrogDraftData document;document.title=method.name;
            document.properties.classSource.member=ClassMemberIdentity{id,method.id,method.implementation.id,method.implementation.revision};
            document.declaredInterface.emplace();
            for(const auto &port:method.ports)check(document.declaredInterface->declarePort(port.input,port.publicPortId,
                port.kind==ClassPortKind::Object?TypeRef::object(id).canonical():port.dataType.canonical(),port.input?"required":""),"Could not author actual member connector");
            files.emplace(method.implementation.path,serializeFrogDraftData(document));
        }
        files.emplace(id+".fclass",writeClassDefinition(definition));
    }
    // Parent paths are authored in the owning .fclass, independently of names.
    files.emplace("Capteur.fclass",files.at("sensor.fclass"));
    const std::vector<ClassDependencyRoot> roots{{"fixture:/Main.frog","/classes/0",{"analog","analog.fclass",1}},
        {"fixture:/Main.frog","/classes/1",{"digital","digital.fclass",1}},
        {"fixture:/Main.frog","/classes/2",{"inherited","inherited.fclass",1}}};
    const DraftDependencyResolver resolve=[&](const DraftDependencyRequest &request) {
        DraftDependencyResult result;
        if(const auto found=files.find(request.authoredReference);found!=files.end()) {
            result.state=DraftDependencyState::Present;result.canonicalUri="fixture:/"+request.authoredReference;
            result.bytes=std::make_shared<const std::string>(found->second);
        }
        return result;
    };
    const auto capture=captureClassProgram(roots,resolve);
    if(!capture.accepted())throw std::runtime_error(capture.issues.empty()?capture.classes.issues.front().message:capture.issues.front().message);
    check(capture.methods.size()==6,"Actual member closure is incomplete");
    const ClassCallTarget member{"analog","measure-analog"};
    const auto *actual=capture.findMember(member);check(actual&&actual->document,"Missing immutable actual member");
    check(capture.attributeMember(actual->source.canonicalUri,actual->source.sha256)==member,"Actual member was not attributed");
    check(!capture.attributeMember("fixture:/copy.frog",actual->source.sha256),"Copied member annotation gained private privileges");
    check(!capture.attributeMember(actual->source.canonicalUri,hashing::sha256Hex("changed")),"Changed member bytes kept privileges");
    check(!capture.attributeMember(actual->source.canonicalUri,{}),"Unsealed source gained privileges");
    const auto saved=serializeFrogDraftData(*actual->document);
    const auto reopened=readFrogDraftData(saved);
    check(reopened&&reopened->properties.classSource==actual->document->properties.classSource,
        "Save/Open changed semantic member attribution");
    const auto assertRefused=[&](std::string_view code) {
        const auto rejected=captureClassProgram(roots,resolve);
        check(!rejected.accepted(),"Invalid actual member accepted");
        check(std::any_of(rejected.issues.begin(),rejected.issues.end(),[&](const auto &i){return i.code==code&&
            i.sourceUri=="fixture:/analog.fclass"&&!i.relatedSourceUri.empty();}),"Actual member refusal lost its declaration/member locations");
    };
    auto changed=*actual->document;
    changed.properties.classSource.member->implementationId="unrelated.member";
    files["analog.measure.frog"]=serializeFrogDraftData(changed);assertRefused("class.member_identity");
    changed=*actual->document;
    check(changed.declaredInterface->editPortForAuthoring("analog.self",true,TypeRef::object("analog").canonical(),
        true,TypeRef::object("sensor").canonical(),std::nullopt),"Could not edit negative connector fixture");
    files["analog.measure.frog"]=serializeFrogDraftData(changed);assertRefused("class.method_connector");
    files["analog.measure.frog"]=saved;
    check(captureClassProgram(roots,resolve).accepted(),"Restored actual member did not recover");

    const auto &hierarchy=*capture.classes.hierarchy;
    const auto analog=TypeRef::object("analog"),digital=TypeRef::object("digital"),parent=TypeRef::object("sensor");
    check(binding::parseType(analog.canonical()).type==analog,"Nominal object type did not round trip");
    check(hierarchy.connectObjects(analog,parent).accepted(),"Captured hierarchy refused a valid upcast");
    check(!hierarchy.connectObjects(parent,analog).accepted(),"Captured hierarchy admitted an implicit downcast");
    check(!hierarchy.connectObjects(analog,digital).accepted(),"Different child classes became structurally interchangeable");
    check(!binding::connect(analog,parent).accepted(),"Structural resolver invented inheritance without a captured hierarchy");
    const std::array selected{analog,digital};
    check(hierarchy.commonObjectType(selected)==parent,"Select did not find the declared common parent");
    check(!hierarchy.connectObjects(hierarchy.recordLayout("sensor")->schema,parent).accepted(),"A forged Record became an object");

    FrogDraftData source;source.properties.classSource.classes={{"analog","analog.fclass",1}};
    DiagramNode node;node.id="object";node.kind="frog_call";
    node.classOperation=ClassSourceOperation{ClassOperationKind::Default,{"analog","analog.fclass",1},{}};
    source.diagramNodes.push_back(node);
    const auto sourceCopy=readFrogDraftData(serializeFrogDraftData(source));
    check(sourceCopy&&sourceCopy->properties.classSource==source.properties.classSource&&
        sourceCopy->diagramNodes.front().classOperation==node.classOperation,"Object declaration did not survive Save/Open");
}

void checkExample(const std::filesystem::path &directory) {
    const auto load = [&](const DraftDependencyRequest &request) {
        DraftDependencyResult result;
        const auto path = directory / std::filesystem::u8path(request.authoredReference);
        std::ifstream input(path, std::ios::binary);
        if (input && (request.kind == DraftDependencyKind::ClassDefinition||request.kind==DraftDependencyKind::Source)) {
            result.state = DraftDependencyState::Present;
            result.canonicalUri = "fixture:/" + request.authoredReference;
            result.bytes = std::make_shared<const std::string>(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
        }
        return result;
    };
    const std::vector<ClassDependencyRoot> roots{
        {"fixture:/Main.frog", "/classes/0", {"analog", "Analogique.fclass", 1}},
        {"fixture:/Main.frog", "/classes/1", {"digital", "Numerique.fclass", 1}},
        {"fixture:/Main.frog", "/classes/2", {"inherited", "Herite.fclass", 1}}};
    const auto capture = captureClassHierarchy(roots, load);
    check(capture.hierarchy && capture.definitions.size() == 4, "Redistributable sensor definitions rejected");
    const auto plan = capture.hierarchy->planDynamicCall("sensor", {"sensor", "measure"}, {}, "/diagram/nodes/0");
    check(plan && plan->arms.size() == 4 && plan->arms[0].implementation == ClassCallTarget{"analog", "measure-analog"} &&
        plan->arms[1].implementation == ClassCallTarget{"digital", "measure-digital"} &&
        plan->arms[2].implementation == ClassCallTarget{"sensor", "measure"}, "Redistributable sensor dispatch plan is incorrect");
    const auto program=captureClassProgram(roots,load);
    if(!program.accepted())throw std::runtime_error(program.issues.empty()?program.classes.issues.front().message:program.issues.front().message);
    check(program.methods.size()==6,"Actual redistributable member bodies are missing");
    std::ifstream main(directory/"Main.frog",std::ios::binary);
    const std::string bytes(std::istreambuf_iterator<char>(main),{});
    const auto prepared=prepareDraftFrontend(bytes,"fixture:/Main.frog",{},load);
    if(!prepared.accepted()) {
        std::string why;for(const auto &issue:prepared.diagnostics)why+=issue.code+" at "+issue.sourceUri+issue.sourcePath+": "+issue.message+"\n";
        throw std::runtime_error(why);
    }
    check(prepared.originalSourceSha256==hashing::sha256Hex(bytes)&&prepared.sourceRevision==10,
        "Class preparation lost exact authoring provenance/revision");
    check(prepared.subprogramDependencyFingerprint.size()==64&&prepared.dependencies.size()>=10,
        "Class definitions or actual methods escaped the frozen dependency seal");
    std::set<std::string> memberUris;
    for(const auto &identity:prepared.dependencyIdentities)memberUris.insert(identity.sourceUri);
    for(const auto &method:program.methods)check(memberUris.contains(method.source.canonicalUri),"A real member body lost its source correspondence");
    check(std::none_of(prepared.document->diagramNodes.begin(),prepared.document->diagramNodes.end(),[](const auto &n){return n.classOperation.has_value()||n.kind=="frog_call";}),
        "Class preparation left an opaque call or unlowered object operation");
    const auto source=readFrogDraftData(bytes);check(bool(source),"Actual class source is unreadable");
    const auto rejected=[&](std::string input,std::string_view code) {
        const auto before=hashing::sha256Hex(input);
        const auto result=prepareDraftFrontend(input,"fixture:/Main.frog",{},load);
        check(!result.accepted()&&result.originalSourceSha256==before&&*result.originalDraft==input,
            "A class refusal modified or discarded the editable original");
        if(std::none_of(result.diagnostics.begin(),result.diagnostics.end(),[&](const auto &i){return i.code==code&&!i.sourcePath.empty();})) {
            std::string why="Missing located refusal "+std::string(code)+": ";for(const auto &i:result.diagnostics)why+=i.code+":"+i.message+"; ";throw std::runtime_error(why);
        }
    };
    auto privateRead=*source;
    DiagramNode read;read.id="forbidden-private";read.kind="frog_call";read.type="frog.studio.frog_call";
    read.classOperation=ClassSourceOperation{ClassOperationKind::ReadField,{"sensor","Capteur.fclass",1},"offset"};
    read.callPorts=*classOperationPorts(*read.classOperation,program);privateRead.diagramNodes.push_back(read);
    rejected(serializeFrogDraftData(privateRead),"class.private_access");
    auto forged=*source;forged.properties.classSource.member=ClassMemberIdentity{"analog","measure-analog","analog.measure",1};
    rejected(serializeFrogDraftData(forged),"class.member_unattributed");
    auto downcast=*source;
    DiagramNode call;call.id="forbidden-downcast";call.kind="frog_call";call.type="frog.studio.frog_call";call.callDocumentPath="Mesurer Analogique.frog";
    const auto &analogMethod=capture.hierarchy->findClass("analog")->methods.front();
    for(const auto &p:analogMethod.ports)call.callPorts.push_back({p.publicPortId,p.publicPortId,{},p.slot,
        p.kind==ClassPortKind::Object?TypeRef::object("analog").canonical():p.dataType.canonical(),p.input,p.input?"required":"recommended"});
    downcast.diagramNodes.push_back(call);
    DiagramEdge edge;edge.id="parent-to-child";edge.from={"choose","result"};edge.to={call.id,"analog.self"};downcast.diagramEdges.push_back(edge);
    rejected(serializeFrogDraftData(downcast),"class.downcast_forbidden");
    auto bypass=*source;call.id="ordinary-member-bypass";call.callDocumentPath="Get Offset.frog";call.callPorts.clear();
    const auto *get=capture.hierarchy->findMethod({"sensor","get-offset"});
    for(const auto &p:get->ports)call.callPorts.push_back({p.publicPortId,p.publicPortId,{},p.slot,
        p.kind==ClassPortKind::Object?TypeRef::object("sensor").canonical():p.dataType.canonical(),p.input,p.input?"required":"recommended"});
    bypass.diagramNodes.push_back(call);edge.id="bypass-wire";edge.to={call.id,"get.self"};bypass.diagramEdges.push_back(edge);
    rejected(serializeFrogDraftData(bypass),"class.member_call_required");
    auto unknown=bytes;const auto at=unknown.find("\"class_operation\"");check(at!=std::string::npos,"No actual class operation in the fixture");
    unknown.insert(at,"\"undeclared_class_effect\":true,");rejected(std::move(unknown),"class.source_unknown_field");
}
}

int main(int argc, char **argv) {
    try {
        run();
        actualMembers();
        if (argc == 2) checkExample(argv[1]);
        std::cout << "Class codec, nominal hierarchy, member contract and immutable transport: PASS\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
