#pragma once

#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/FrogJson.hpp"

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

// Private Studio authoring data. These records never contain runtime handles
// and reading/configuring them never loads an assembly, COM server or module.
enum class ConnectivityReferenceFamily { ActiveX, Python, Variant, Document, DataFinder, Database, InputDevice, Registry, Catalog };
enum class ConnectivityNodeKind { Constructor, Property, Invoke, Callback, Python, TypeConversion,
    StaticReference, AutomationOpen, Polymorphic };
enum class ConnectivityMetadataState { Unconfigured, Available, Unavailable };
enum class ConnectivityParameterDirection { Input, Output, InOut };
enum class ConnectivityPropertyMode { Read, Write };

struct ConnectivityParameter {
    std::string id; // authored identity, independent of index and display name
    std::string memberParameterId;
    std::string label;
    ConnectivityParameterDirection direction = ConnectivityParameterDirection::Input;
    binding::TypeRef valueType;
    bool metadataAvailable = false;
    std::string description;
    bool operator==(const ConnectivityParameter &) const = default;
};
struct ConnectivityPropertyRow {
    std::string id;
    std::string memberId;
    std::string label;
    ConnectivityPropertyMode mode = ConnectivityPropertyMode::Read;
    binding::TypeRef valueType;
    bool readable = false;
    bool writable = false;
    bool metadataAvailable = false;
    std::string description;
    bool operator==(const ConnectivityPropertyRow &) const = default;
};
struct ConnectivityCallbackGroup {
    std::string id;
    std::string eventId;
    std::string label;
    binding::TypeRef sourceType;
    binding::TypeRef callbackType;
    binding::TypeRef userParameterType;
    bool metadataAvailable = false;
    bool operator==(const ConnectivityCallbackGroup &) const = default;
};
struct ConnectivityDocumentPort {
    std::string id;
    std::string label;
    bool output = false;
    binding::TypeRef valueType;
    bool operator==(const ConnectivityDocumentPort &) const = default;
};
struct ConnectivityNodeConfiguration {
    std::string catalogId;
    ConnectivityReferenceFamily family = ConnectivityReferenceFamily::Catalog;
    ConnectivityNodeKind kind = ConnectivityNodeKind::Constructor;
    ConnectivityMetadataState metadataState = ConnectivityMetadataState::Unconfigured;
    std::string metadataSource;
    std::string unresolvedReason;
    std::string assemblyIdentity;
    std::string qualifiedTypeId;
    // Concrete nominal class supplied by metadata; no inheritance is guessed.
    std::string referenceClassId;
    std::string memberId;
    std::string memberLabel;
    std::vector<ConnectivityParameter> parameters;
    std::vector<ConnectivityPropertyRow> properties;
    std::vector<ConnectivityCallbackGroup> callbacks;
    binding::TypeRef returnType;
    binding::TypeRef targetType;
    bool hasReturnValue = false;
    std::string instanceId;
    std::string modulePath;
    std::string functionName;
    std::string documentPath;
    std::string documentIdentity;
    bool strictReference = false;
    std::vector<ConnectivityDocumentPort> documentPorts;
    // Monotonic per-node identity source: deleting a row never reuses its ID.
    std::uint64_t nextIdentity = 1;
    bool operator==(const ConnectivityNodeConfiguration &) const = default;
};
struct ConnectivityTypeEvidence {
    std::string portId;
    binding::TypeRef valueType;
};
struct ConnectivityConfiguredPort {
    std::string id;
    std::string name;
    bool output = false;
    binding::TypeRef valueType;
    std::string description;
    std::string unresolvedReason;
};
// Portable metadata must describe actual inspected members. It is not a
// predefined/example class catalog. Import is a JSON read with no activation.
struct ConnectivityMemberMetadata {
    ConnectivityReferenceFamily family = ConnectivityReferenceFamily::Catalog;
    ConnectivityNodeKind kind = ConnectivityNodeKind::Invoke;
    std::string assemblyIdentity;
    std::string qualifiedTypeId;
    std::string referenceClassId;
    std::string memberId;
    std::string label;
    std::vector<ConnectivityParameter> parameters;
    binding::TypeRef returnType;
    bool hasReturnValue = false;
    binding::TypeRef propertyType;
    bool readable = false;
    bool writable = false;
    binding::TypeRef callbackType;
    binding::TypeRef userParameterType;
    std::string description;
};
struct ConnectivityMetadataCatalog {
    std::string sourceIdentity;
    std::vector<ConnectivityMemberMetadata> members;
};


} // namespace frog::native_engine
