// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/ClassDefinition.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

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

void run() {
    std::string fieldError;
    const auto preparedField = prepareClassField("offset", "Offset", "i64", "9007199254740993", &fieldError);
    check(preparedField && preparedField->defaultValue.string == "9007199254740993" && fieldError.empty(),
        "Class editor projected an exact field default through binary64");
    check(!prepareClassField("offset", "Offset", "i64", "1.5", &fieldError) && fieldError.starts_with("/default"),
        "Class editor accepted a nonintegral default");
    auto definitions = sensors();
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
    check(hierarchy->assignable("analog", "sensor"), "No upcast");
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

void checkExample(const std::filesystem::path &directory) {
    const auto load = [&](const DraftDependencyRequest &request) {
        DraftDependencyResult result;
        const auto path = directory / request.authoredReference;
        std::ifstream input(path, std::ios::binary);
        if (input && request.kind == DraftDependencyKind::ClassDefinition) {
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
}
}

int main(int argc, char **argv) {
    try {
        run();
        if (argc == 2) checkExample(argv[1]);
        std::cout << "Class codec, nominal hierarchy, member contract and immutable transport: PASS\n";
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
