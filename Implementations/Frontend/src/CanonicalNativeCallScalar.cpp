#include "frog_native_engine/CanonicalNativeCallScalar.hpp"
#include "frog_native_engine/Sha256.hpp"
#include "CanonicalJsonSupport.hpp"
#include <algorithm>
#include <stdexcept>
namespace frog::native_engine {
namespace {
namespace cj=canonical_json;
std::string requirement(NativeCallInputRequirement value) {
    switch(value){case NativeCallInputRequirement::Required:return "required";
    case NativeCallInputRequirement::Recommended:return "recommended";
    case NativeCallInputRequirement::Optional:return "optional";}return {};
}
}
std::optional<CanonicalNativeCallDescriptor> canonicalNativeCallScalarDescriptor(
    const source::JsonValue &raw,std::string_view pointer,CanonicalNativeCallDiagnostic *diagnostic) {
    if(diagnostic)*diagnostic={};
    const auto refuse=[&](std::string code,std::string message,bool unsupported=false)->std::optional<CanonicalNativeCallDescriptor>{
        if(diagnostic)*diagnostic={std::move(code),std::move(message),std::string(pointer),unsupported};return {};
    };
    std::string error;
    if(!nativeCallScalarConfigurationPreflight(raw,&error))return refuse("native_call.configuration_shape_invalid",error);
    const auto decoded=nativeCallFromJson(raw,&error);
    if(!decoded)return refuse("native_call.configuration_invalid",error.empty()?"A complete NativeCall codec object is required.":error);
    const auto encoded=nativeCallToJson(*decoded);
    if(!cj::exact(raw,encoded))return refuse("native_call.configuration_shape_invalid","The complete canonical NativeCall codec must round-trip exactly; unknown, omitted or normalized fields are not erased.");
    const auto validation=nativeCallScalarSignatureValidate(*decoded);
    // Invalid/incomplete source has priority over a merely unavailable target.
    for(const auto &d:validation.diagnostics)if(d.severity==NativeCallDiagnosticSeverity::Error&&d.category!=NativeCallDiagnosticCategory::Unavailable)
        return refuse("native_call.configuration_invalid",d.code+": "+d.message);
    for(const auto &d:validation.diagnostics)if(d.severity==NativeCallDiagnosticSeverity::Error)
        return refuse("native_call.signature_unavailable",d.code+": "+d.message,true);
    for(const auto &parameter:decoded->arguments)if(parameter.defaultCanonicalValue) {
        std::string reason;const auto value=nativeCallScalarDefaultValue(parameter,&reason);
        if(!value)return refuse("native_call.default_lexeme_unavailable","The actual scalar service cannot decode this unchanged default token for "+parameter.id+": "+reason,true);
    }
    CanonicalNativeCallDescriptor result;result.profile=kNativeCallScalarProfile;
    result.configurationJson=cj::encode(encoded);result.configurationSha256=hashing::sha256Hex(result.configurationJson);
    result.semanticKey=nativeCallSemanticKey(*decoded);result.configurationSourcePointer=pointer;
    for(const auto &parameter:decoded->arguments)result.arguments.push_back({parameter.id,nativeCallTypeRef(parameter.type,64).canonical(),parameter.inputRequirement,parameter.defaultCanonicalValue});
    if(decoded->returnDefinition.type.kind!=NativeCallKind::Void)result.returnType=nativeCallTypeRef(decoded->returnDefinition.type,64).canonical();
    for(const auto &port:nativeCallPorts(*decoded,64)) {
        if(!port.valueType.known())return refuse("native_call.signature_unavailable","A real NativeCall connector has no exact scalar5/error type.",true);
        result.ports.push_back({port.id,port.valueType.canonical(),!port.output,port.requirement,port.defaultCanonicalValue});
    }
    return result;
}
source::JsonValue canonicalNativeCallScalarAttributes(const CanonicalNativeCallDescriptor &descriptor) {
    namespace cj=canonical_json;
    const auto configuration=source::parseJson(descriptor.configurationJson,nullptr,source::JsonNumberPolicy::PreserveLexeme);
    if(!configuration||configuration->type!=source::JsonValue::Type::Object)
        throw std::invalid_argument("NativeCall FIR attributes require a complete valid codec JSON object.");
    auto result=cj::object(),arguments=cj::array(),ports=cj::array();
    for(const auto &argument:descriptor.arguments) {
        auto value=cj::object();value.object={{"id",cj::string(argument.id)},{"type",cj::string(argument.type)},
            {"requirement",cj::string(requirement(argument.requirement))},{"default_present",cj::boolean(argument.defaultCanonicalValue.has_value())}};
        if(argument.defaultCanonicalValue)value.object["default_canonical_value"]=cj::string(*argument.defaultCanonicalValue);
        arguments.array.push_back(std::move(value));
    }
    for(const auto &port:descriptor.ports) {
        auto value=cj::object();value.object={{"id",cj::string(port.id)},{"direction",cj::string(port.input?"in":"out")},
            {"type",cj::string(port.type)},{"requirement",cj::string(requirement(port.requirement))},
            {"default_present",cj::boolean(port.defaultCanonicalValue.has_value())}};
        if(port.defaultCanonicalValue)value.object["default_canonical_value"]=cj::string(*port.defaultCanonicalValue);
        ports.array.push_back(std::move(value));
    }
    result.object={{"profile",cj::string(descriptor.profile)},{"version",*source::parseJson("1")},
        {"configuration",*configuration},
        {"configuration_sha256",cj::string(descriptor.configurationSha256)},
        {"configuration_ref",cj::string(descriptor.configurationSourcePointer)},
        {"semantic_key",cj::string(descriptor.semanticKey)},{"arguments",std::move(arguments)},
        {"return_type",descriptor.returnType?cj::string(*descriptor.returnType):source::JsonValue{}},{"ports",std::move(ports)}};
    return result;
}
}
