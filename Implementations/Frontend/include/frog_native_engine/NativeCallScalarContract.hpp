#pragma once
// Portable local scalar5 contract. No FFI, loader, Highlight, Runtime or SDK.
#include "NativeCall.hpp"
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>
namespace frog::native_engine {
inline constexpr std::string_view kNativeCallScalarProfile="frog.studio.native_call.scalar5@1";
inline constexpr std::string_view kNativeCallScalarOperationId="frog.studio.native_call.scalar5.v1";
inline constexpr std::string_view kNativeCallScalarErrorType="cluster<\"status\":bool,\"code\":i32,\"source\":string>";
// The active alternative IS the native representation. No carrier promotion,
// narrowing, bool-as-integer, or integer-to-f64 conversion occurs in this API.
using NativeCallScalarValue = std::variant<std::int32_t,std::uint32_t,std::int64_t,std::uint64_t,double>;
using NativeCallScalarInputs = std::vector<std::pair<std::string,NativeCallScalarValue>>;
struct NativeCallError {
    bool status = false;
    std::int32_t code = 0;
    std::string source;
    bool operator==(const NativeCallError &) const = default;
};
struct NativeCallScalarResult {
    std::optional<NativeCallScalarValue> returnValue;
    // All declared parameter mirrors, in ABI order, with exact param.<id>.out IDs.
    NativeCallScalarInputs mirrors;
    NativeCallError errorOut;
    bool bypassed = false;
    // Same fatal/native-memory-fault distinction as the existing Legacy owner.
    std::string fatalError;
};


// Iterative resource/type-text guard for the NEW scalar5 decode boundary.
// Call before nativeCallFromJson. It is not another type/configuration parser.
[[nodiscard]] bool nativeCallScalarConfigurationPreflight(const source::JsonValue &,
    std::string *error=nullptr);
// Does not resolve paths, bind an export, load a provider or claim executable.
[[nodiscard]] NativeCallValidation nativeCallScalarSignatureValidate(const NativeCallConfiguration &);
// Absence is distinct from malformed default; error is cleared before use.
[[nodiscard]] std::optional<NativeCallScalarValue> nativeCallScalarDefaultValue(
    const NativeCallParameter &,std::string *error=nullptr);
namespace native_call_detail {
// Existing native packing budget, shared by Legacy and scalar5. Not a new
// per-program payload or process-memory claim.
inline constexpr std::size_t allocationBudget=64*1024*1024;
struct Fault:std::runtime_error { int code; Fault(int c,std::string m):std::runtime_error(std::move(m)),code(c){} };
using CanonicalNumber=std::variant<std::int64_t,std::uint64_t,double>;
CanonicalNumber decodeCanonicalNumber(const source::JsonValue &,bool unsignedInteger,bool signedInteger);
bool scalarType(NativeCallNumericType) noexcept;
void scalarProfileDiagnostics(const NativeCallConfiguration &,NativeCallValidation &);
// Internal throwing form preserves the invocation's existing 7606/7607 phase.
std::optional<NativeCallScalarValue> scalarCanonicalDefault(const NativeCallParameter &);
}
}
