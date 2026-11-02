#pragma once
// Local open Source/FIR contract, independent of SDK, LLVM and native loading.
#include "NativeCallScalarContract.hpp"
namespace frog::native_engine {
struct NativeCallScalarPortContract {
    std::string id,type;
    bool input=false;
    NativeCallInputRequirement requirement=NativeCallInputRequirement::Required;
    std::optional<std::string> defaultCanonicalValue;
};
struct NativeCallScalarArgumentContract {
    std::string id,type;
    NativeCallInputRequirement requirement=NativeCallInputRequirement::Required;
    std::optional<std::string> defaultCanonicalValue;
};
struct CanonicalNativeCallDescriptor {
    std::string profile,configurationJson,configurationSha256,semanticKey,configurationSourcePointer;
    std::vector<NativeCallScalarArgumentContract> arguments;
    std::optional<std::string> returnType;
    std::vector<NativeCallScalarPortContract> ports;
};
struct CanonicalNativeCallDiagnostic {
    std::string code,message,location;
    bool unsupported=false;
};
[[nodiscard]] std::optional<CanonicalNativeCallDescriptor> canonicalNativeCallScalarDescriptor(
    const source::JsonValue &configuration,std::string_view configurationSourcePointer,
    CanonicalNativeCallDiagnostic *diagnostic=nullptr);
// The complete configuration and exact derived signature are carried openly
// in FIR. Source references and SHA only attribute/seal those semantics.
[[nodiscard]] source::JsonValue canonicalNativeCallScalarAttributes(const CanonicalNativeCallDescriptor &);
}
