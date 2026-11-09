#pragma once
// Complete local Source/FIR contract, independent of SDK, LLVM and UI.
#include "ApplicationControlTypes.hpp"
#include "FrogSourceDocument.hpp"
namespace frog::native_engine {
inline constexpr std::string_view canonicalObjectReferenceProfile="frog.studio.object-reference.local@1";
inline constexpr std::string_view canonicalObjectReferenceAttribute="frog_studio_object_reference";
enum class CanonicalObjectReferenceKind { Resolve, Property, Invoke, Construct, Update, Select, Close };
struct CanonicalObjectReferencePort {
    std::string id,type,fullTypeJson;
    bool input=false,required=true;
    std::optional<std::string> defaultValueJson;
    bool operator==(const CanonicalObjectReferencePort &) const = default;
};
struct CanonicalObjectReferenceDescriptor {
    std::string profile,configurationJson,configurationSha256,semanticKey,configurationSourcePointer;
    CanonicalObjectReferenceKind kind=CanonicalObjectReferenceKind::Resolve;
    std::vector<CanonicalObjectReferencePort> ports;
    std::vector<std::vector<std::string>> fieldPaths;
    std::vector<std::string> effects;
    bool operator==(const CanonicalObjectReferenceDescriptor &) const = default;
};
struct CanonicalObjectReferenceDiagnostic {
    std::string code,message,location;
    bool unsupported=false;
};
// The full configuration and port schemas are FIR semantics; SHA and Source
// pointers attribute and seal them rather than replacing those semantics.
std::optional<CanonicalObjectReferenceDescriptor> canonicalObjectReferenceDescriptor(
    const source::JsonValue &configuration,std::string_view configurationSourcePointer,
    const binding::TypeRef &wiredReferenceType={},
    CanonicalObjectReferenceDiagnostic *diagnostic=nullptr);
source::JsonValue canonicalObjectReferenceAttributes(const CanonicalObjectReferenceDescriptor &);
std::optional<CanonicalObjectReferenceDescriptor> canonicalObjectReferenceDescriptorFromAttributes(
    const source::JsonValue &,std::string_view configurationSourcePointer,
    CanonicalObjectReferenceDiagnostic *diagnostic=nullptr);
std::optional<CanonicalObjectReferenceDescriptor> canonicalObjectReferenceRecordDescriptor(
    CanonicalObjectReferenceKind,const binding::TypeRef &recordType,
    std::vector<CanonicalObjectReferencePort>,std::vector<std::vector<std::string>> fieldPaths,
    const source::JsonValue *literal,std::string_view configurationSourcePointer,
    CanonicalObjectReferenceDiagnostic *diagnostic=nullptr);
std::optional<binding::TypeRef> canonicalObjectReferenceReadType(const source::JsonValue &,
    CanonicalObjectReferenceDiagnostic *diagnostic=nullptr);
source::JsonValue canonicalObjectReferenceWriteType(const binding::TypeRef &);
bool canonicalObjectReferenceFullTypeEqual(const binding::TypeRef &,const binding::TypeRef &);
// Private physical carrier mapping only. Complete Enum domain/choices/labels
// remain in the descriptor schema; no public SDK Enum contract is implied.
binding::TypeRef canonicalObjectReferencePhysicalType(const binding::TypeRef &);
std::string_view canonicalObjectReferenceOperation(CanonicalObjectReferenceKind) noexcept;
}
