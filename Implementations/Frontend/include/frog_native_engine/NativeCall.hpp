#pragma once

#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/FrogJson.hpp"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

inline constexpr std::string_view kNativeCallCatalogId = "connectivity.libraries.call_library_function";
inline constexpr std::string_view kNativeCallDiagramNodeType = "frog.studio.connectivity.libraries.call_library_function";
inline constexpr std::string_view kNativeCallProviderRequirement = "frog.studio.native_call.libffi@1";

enum class NativeCallCallingConvention { Cdecl, Stdcall };
enum class NativeCallThread { Ui, Any };
enum class NativeCallErrorChecking { Default, Maximum, Disabled };
enum class NativeCallPathMode { Absolute, DocumentRelative, ProjectRelative, ApplicationRelative, LibraryName };
enum class NativeCallKind { Void, Numeric, Pointer, Array, String, AdaptToType, Cluster,
    InstanceDataPointer, LabViewHandle, Waveform, DigitalWaveform, DigitalData, ActiveX };
enum class NativeCallNumericType { I8, U8, I16, U16, I32, U32, I64, U64, F32, F64,
    IntPtr, UIntPtr, CBool, Win32Bool };
enum class NativeCallPass { Value, Pointer };
enum class NativeCallAccess { ReadOnly, ReadWrite, WriteOnly };
enum class NativeCallInputRequirement { Required, Recommended, Optional };
enum class NativeCallArrayFormat { DataPointer, LabViewHandle };
enum class NativeCallStringFormat { CString, PascalString, LabViewHandle };
enum class NativeCallStringEncoding { Utf8, Utf16, Ansi };
enum class NativeCallOwnership { Borrowed, CallerOwned, LibraryOwned };
enum class NativeCallInitialization { Zero, DefaultValue };
enum class NativeCallCapacitySource { Unspecified, Constant, Parameter, InputLength };
enum class NativeCallCapacityUnit { Elements, Bytes, Utf16CodeUnits };
enum class NativeCallDiagnosticCategory { InvalidConfiguration, Incomplete, Unavailable, Migration };
enum class NativeCallDiagnosticSeverity { Error, Warning, Information };

struct NativeCallCapacity {
    NativeCallCapacitySource source = NativeCallCapacitySource::Unspecified;
    std::uint64_t constant = 0;
    std::string parameterId;
    NativeCallCapacityUnit unit = NativeCallCapacityUnit::Elements;
    bool operator==(const NativeCallCapacity &) const = default;
};

struct NativeCallStructField {
    std::string fieldId;
    NativeCallNumericType numericType = NativeCallNumericType::I32;
    std::uint64_t offset = 0;
    bool operator==(const NativeCallStructField &) const = default;
};

struct NativeCallStructLayout {
    std::string cTypeName;
    std::uint64_t byteSize = 0;
    std::uint64_t alignment = 0;
    std::uint64_t packing = 0;
    std::vector<NativeCallStructField> fields;
    bool operator==(const NativeCallStructLayout &) const = default;
};

struct NativeCallType {
    NativeCallKind kind = NativeCallKind::Numeric;
    NativeCallNumericType numericType = NativeCallNumericType::I32;
    NativeCallNumericType elementType = NativeCallNumericType::F64;
    std::size_t rank = 1;
    NativeCallArrayFormat arrayFormat = NativeCallArrayFormat::DataPointer;
    NativeCallStringFormat stringFormat = NativeCallStringFormat::CString;
    NativeCallStringEncoding stringEncoding = NativeCallStringEncoding::Utf8;
    // ANSI is only meaningful with an explicit code page; zero never means host default.
    std::uint32_t ansiCodePage = 0;
    // Adapt/Cluster carry a real FROG type and an explicit native layout, never an any->pointer cast.
    binding::TypeRef adaptedType;
    NativeCallStructLayout structLayout;
    bool operator==(const NativeCallType &) const = default;
};

struct NativeCallParameter {
    std::string id;
    std::string displayName;
    NativeCallType type;
    NativeCallPass pass = NativeCallPass::Value;
    NativeCallAccess access = NativeCallAccess::ReadOnly;
    NativeCallInputRequirement inputRequirement = NativeCallInputRequirement::Required;
    NativeCallInitialization initialization = NativeCallInitialization::Zero;
    // Canonical Source value text, not a double projection of an integer default.
    std::optional<std::string> defaultCanonicalValue;
    bool nullable = false;
    NativeCallCapacity minimumCapacity;
    // Rank > 1 needs explicit dimensions, in row-major order.
    std::vector<NativeCallCapacity> dimensions;
    std::optional<std::string> validLengthParameterId;
    bool operator==(const NativeCallParameter &) const = default;
};

struct NativeCallReturnDefinition {
    std::string displayName = "return";
    NativeCallType type = [] { NativeCallType value; value.kind = NativeCallKind::Void; return value; }();
    NativeCallOwnership ownership = NativeCallOwnership::Borrowed;
    // Returned string pointers require a finite read bound and an explicit lifetime.
    NativeCallCapacity maximumRead;
    std::string lifetime;
    bool operator==(const NativeCallReturnDefinition &) const = default;
};

struct NativeCallLibrary {
    NativeCallPathMode mode = NativeCallPathMode::DocumentRelative;
    std::string value;
    bool specifyPathOnDiagram = false;
    NativeCallPathMode dynamicRelativeBase = NativeCallPathMode::DocumentRelative;
    bool operator==(const NativeCallLibrary &) const = default;
};

struct NativeCallCallbacks {
    // Studio C lifecycle ABI v1: int32_t callback(void **instance_slot).
    std::uint32_t profileVersion = 1;
    std::string reserve;
    std::string unreserve;
    std::string abort;
    bool operator==(const NativeCallCallbacks &) const = default;
};

struct NativeCallPresentation {
    bool showParameterNames = true;
    bool headerTitleVisible = true;
    // Named rows have priority over compact rendering. UI may clear compact when showing names.
    bool compact = false;
    double requestedWidth = 0.0; // zero = fit content
    std::vector<std::string> visualParameterOrder;
    bool operator==(const NativeCallPresentation &) const = default;
};

struct NativeCallConfiguration {
    std::uint32_t schemaVersion = 1;
    NativeCallLibrary library;
    std::string symbol;
    NativeCallCallingConvention callingConvention = NativeCallCallingConvention::Cdecl;
    NativeCallThread thread = NativeCallThread::Ui;
    NativeCallErrorChecking errorChecking = NativeCallErrorChecking::Default;
    NativeCallReturnDefinition returnDefinition;
    // This vector is THE ABI order. Presentation never participates in native argument packing.
    std::vector<NativeCallParameter> arguments;
    NativeCallCallbacks callbacks;
    NativeCallPresentation presentation;
    std::uint64_t nextIdentity = 1;
    bool variadic = false;
    bool operator==(const NativeCallConfiguration &) const = default;
};

struct NativeCallDiagnostic {
    std::string code;
    std::string message;
    std::string parameterId;
    NativeCallDiagnosticCategory category = NativeCallDiagnosticCategory::InvalidConfiguration;
    NativeCallDiagnosticSeverity severity = NativeCallDiagnosticSeverity::Error;
};

struct NativeCallTargetCapabilities {
    // Zero means unknown target width, never sizeof(void*) on the authoring host.
    std::size_t pointerBits = 0;
    bool providerAvailable = false;
    bool scalar = false;
    bool pointers = false;
    bool arrays = false;
    bool strings = false;
    bool utf16 = false;
    bool ansi = false;
    bool pascalStrings = false;
    bool multidimensionalArrays = false;
    bool structs = false;
    bool adaptToType = false;
    bool labViewHandles = false;
    bool waveforms = false;
    bool digitalData = false;
    bool activeX = false;
    bool instanceData = false;
    bool lifecycleCallbacks = false;
    bool uiThreadDispatcher = false;
    bool anyThread = false;
    bool stdcall = false;
    // Binding evidence is separate from the provider's ability to marshal a signature.
    bool libraryResolved = false;
    bool symbolResolved = false;
};

struct NativeCallValidation {
    std::vector<NativeCallDiagnostic> diagnostics;
    bool structurallyValid = true;
    bool complete = true;
    bool executable = false;
};

struct NativeCallPort {
    std::string id;
    std::string name;
    bool output = false;
    binding::TypeRef valueType;
    NativeCallInputRequirement requirement = NativeCallInputRequirement::Required;
    std::optional<std::string> defaultCanonicalValue;
    std::string description;
    std::string unresolvedReason;
};

struct NativeCallPrototype {
    std::string text;
    bool valid = false;
    std::vector<NativeCallDiagnostic> diagnostics;
};

[[nodiscard]] const NativeCallParameter *nativeCallParameter(const NativeCallConfiguration &, std::string_view id);
[[nodiscard]] binding::TypeRef nativeCallTypeRef(const NativeCallType &, std::size_t pointerBits = 0);
[[nodiscard]] std::vector<NativeCallPort> nativeCallPorts(const NativeCallConfiguration &, std::size_t pointerBits = 0);
[[nodiscard]] NativeCallValidation nativeCallValidate(const NativeCallConfiguration &,
    const NativeCallTargetCapabilities *target = nullptr);
[[nodiscard]] NativeCallPrototype nativeCallPrototype(const NativeCallConfiguration &);
// Exact semantic cache key: excludes display names and presentation, includes ABI order and policies.
[[nodiscard]] std::string nativeCallSemanticKey(const NativeCallConfiguration &);

// Mutations are atomic on error; edits may intentionally leave incomplete/unavailable configuration.
[[nodiscard]] std::optional<std::string> nativeCallAddParameter(NativeCallConfiguration &, NativeCallParameter = {},
    std::optional<std::size_t> abiPosition = {}, std::vector<NativeCallDiagnostic> *diagnostics = nullptr);
bool nativeCallRemoveParameter(NativeCallConfiguration &, std::string_view id,
    std::vector<NativeCallDiagnostic> *diagnostics = nullptr);
bool nativeCallUpdateParameter(NativeCallConfiguration &, std::string_view id, NativeCallParameter,
    std::vector<NativeCallDiagnostic> *diagnostics = nullptr);
bool nativeCallReorderAbi(NativeCallConfiguration &, std::span<const std::string> ids,
    std::vector<NativeCallDiagnostic> *diagnostics = nullptr);
bool nativeCallSwapVisual(NativeCallConfiguration &, std::string_view firstId, std::string_view secondId,
    std::vector<NativeCallDiagnostic> *diagnostics = nullptr);
bool nativeCallMoveVisual(NativeCallConfiguration &, std::string_view id, std::size_t visualIndex,
    std::vector<NativeCallDiagnostic> *diagnostics = nullptr);
bool nativeCallResetVisual(NativeCallConfiguration &, std::vector<NativeCallDiagnostic> *diagnostics = nullptr);
// Only presentation is repaired. Invalid ABI identities remain untouched and diagnosed.
[[nodiscard]] std::vector<NativeCallDiagnostic> nativeCallMigrateVisual(NativeCallConfiguration &);

[[nodiscard]] source::JsonValue nativeCallToJson(const NativeCallConfiguration &);
[[nodiscard]] std::optional<NativeCallConfiguration> nativeCallFromJson(const source::JsonValue &,
    std::string *error = nullptr, std::vector<NativeCallDiagnostic> *diagnostics = nullptr);

} // namespace frog::native_engine
