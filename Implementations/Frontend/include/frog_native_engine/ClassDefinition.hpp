// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "frog_native_engine/DraftDependency.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include <map>

namespace frog::native_engine {

inline constexpr std::string_view kClassDefinitionFormat = "frog.class";
inline constexpr std::string_view kClassDefinitionExtension = ".fclass";
inline constexpr std::size_t kMaximumClassClosure = 1024;
inline constexpr std::size_t kMaximumClassDepth = 32;

enum class ClassVisibility { Public, Protected, Private };
enum class ClassDispatch { Static, Dynamic };
enum class ClassPortKind { Data, Object };

struct ClassDependency {
    std::string id;
    std::string path;
    std::uint32_t revision = 0;
    bool operator==(const ClassDependency &) const = default;
};

[[nodiscard]] std::optional<ClassDependency> readClassDependency(
    const source::JsonValue &,std::string_view extension,std::string *error=nullptr);

struct ClassDependencyRoot {
    std::string ownerUri;
    std::string pointer;
    ClassDependency reference;
};

struct ClassField {
    std::string id;
    std::string label;
    binding::TypeRef type;
    source::JsonValue defaultValue;
};

struct ClassPort {
    std::string slot;
    std::string publicPortId;
    bool input = true;
    ClassPortKind kind = ClassPortKind::Data;
    // Object slots refer to the class that owns this declaration. The physical
    // realization of this type is not a part of the member signature.
    binding::TypeRef dataType;
};

struct ClassOverride {
    std::string classId;
    std::string methodId;
    bool operator==(const ClassOverride &) const = default;
};

struct ClassMethod {
    std::string id;
    std::string name;
    ClassDependency implementation;
    ClassVisibility visibility = ClassVisibility::Private;
    ClassDispatch dispatch = ClassDispatch::Static;
    std::optional<ClassOverride> overrides;
    std::vector<ClassPort> ports;
};

struct ClassDefinition {
    std::string id;
    std::string name;
    std::uint32_t revision = 1;
    std::optional<ClassDependency> parent;
    std::vector<ClassField> fields;
    std::vector<ClassMethod> methods;
};

struct ClassIssue {
    std::string code;
    std::string classId;
    std::string pointer;
    std::string message;
    std::string sourceUri;
    std::string relatedSourceUri;
    std::string relatedPointer;
};

// Structural codec: readable but semantically invalid class declarations can
// be edited and written. Invalid syntax and unsupported envelope revisions
// fail without producing a normalized replacement.
[[nodiscard]] std::optional<ClassDefinition> readClassDefinition(std::string_view, std::string *error = nullptr);
[[nodiscard]] std::string writeClassDefinition(const ClassDefinition &);
[[nodiscard]] std::optional<ClassField> prepareClassField(std::string id, std::string label,
    std::string_view canonicalType, std::string_view defaultJson, std::string *error = nullptr);

using ClassDefinitions = std::map<std::string, ClassDefinition, std::less<>>;
struct ClassCallTarget {
    std::string classId;
    std::string methodId;
    bool operator==(const ClassCallTarget &) const = default;
};

// A closed transport layout for the reachable classes of one root. This
// schema is an implementation detail: an authored Cluster is never assignable
// to an object, even if its structure is identical.
struct ClassRecordLayout {
    binding::TypeRef schema;
    std::map<std::string, std::uint32_t, std::less<>> discriminants;
};

struct ClassDispatchArm {
    std::uint32_t discriminant;
    std::string concreteClassId;
    ClassCallTarget implementation;
};
struct ClassDispatchPlan {
    ClassRecordLayout layout;
    CanonicalObjectReferenceDescriptor selector;
    std::vector<ClassDispatchArm> arms;
};

// Receives the captured implementation, already inspected by the ordinary
// sub-FROG owner. This view compares public port identity, direction and type;
// the host must not create it from the class's declared signature.
struct ClassImplementationPort {
    std::string id;
    bool input = true;
    ClassPortKind kind = ClassPortKind::Data;
    binding::TypeRef dataType;
    std::string objectClassId;
};
struct ClassImplementationInterface {
    std::string id;
    std::uint32_t revision = 0;
    std::vector<ClassImplementationPort> ports;
};
[[nodiscard]] std::vector<ClassIssue> validateClassImplementation(
    const ClassDefinition &, std::size_t methodIndex, const ClassImplementationInterface &);

// Frozen, semantically valid declarations. This layer does not open paths,
// select an implementation from a file name, or interpret method FROGs.
class ClassHierarchy {
public:
    [[nodiscard]] static std::optional<ClassHierarchy> validate(ClassDefinitions definitions,
        std::vector<ClassIssue> &issues);
    [[nodiscard]] bool assignable(std::string_view from, std::string_view to) const;
    // Hierarchy-sensitive connections are resolved here, not by the structural
    // type resolver or a Canvas-specific inheritance table.
    [[nodiscard]] binding::TypeResolution connectObjects(const binding::TypeRef &from,const binding::TypeRef &to) const;
    [[nodiscard]] std::optional<binding::TypeRef> commonObjectType(std::span<const binding::TypeRef>) const;
    [[nodiscard]] const ClassDefinition *findClass(std::string_view id) const;
    [[nodiscard]] const ClassMethod *findMethod(const ClassCallTarget &) const;
    // caller must be attributed to an actual, validated member declaration.
    [[nodiscard]] bool canAccess(const std::optional<ClassCallTarget> &caller, const ClassCallTarget &declaration) const;
    [[nodiscard]] bool canAccessField(const ClassCallTarget &caller, std::string_view owner, std::string_view field) const;
    [[nodiscard]] std::optional<ClassRecordLayout> recordLayout(std::string_view wireClass, ClassIssue *issue = nullptr) const;
    [[nodiscard]] std::optional<source::JsonValue> defaultRecord(std::string_view concreteClass,
        const ClassRecordLayout &) const;
    [[nodiscard]] std::optional<std::string> privateRecordSlot(const ClassCallTarget &caller,
        std::string_view wireClass, std::string_view field) const;
    // Compiler-only lowering to the existing Record instructions. Access
    // checks precede descriptor creation; the source never supplies raw paths.
    [[nodiscard]] std::optional<CanonicalObjectReferenceDescriptor> lowerDefaultObject(
        std::string_view concreteClass, std::string_view sourcePointer) const;
    [[nodiscard]] std::optional<CanonicalObjectReferenceDescriptor> lowerPrivateField(
        const ClassCallTarget &caller, std::string_view wireClass, std::string_view field,
        bool update, std::string_view sourcePointer) const;
    [[nodiscard]] std::optional<ClassDispatchPlan> planDynamicCall(std::string_view wireClass,
        const ClassCallTarget &declaration, const std::optional<ClassCallTarget> &caller,
        std::string_view sourcePointer) const;
    // Keyed by concrete class identity. Even a static call retains that class
    // identity on an object result. Ordinary callers have no member privilege.
    [[nodiscard]] std::map<std::string, ClassCallTarget, std::less<>> dispatchTargets(
        std::string_view wireClass, const ClassCallTarget &declaration,
        const std::optional<ClassCallTarget> &caller) const;
private:
    explicit ClassHierarchy(ClassDefinitions definitions) : definitions_(std::move(definitions)) {}
    ClassDefinitions definitions_;
};

struct CapturedClassHierarchy {
    std::optional<ClassHierarchy> hierarchy;
    std::vector<DraftDependencyResult> definitions;
    std::vector<ClassIssue> issues;
};

// The caller lists the concrete classes used by the program. Parents are
// loaded transitively. No discovery by directory, file name or display name.
[[nodiscard]] CapturedClassHierarchy captureClassHierarchy(std::span<const ClassDependencyRoot>,
    const DraftDependencyResolver &);

} // namespace frog::native_engine
