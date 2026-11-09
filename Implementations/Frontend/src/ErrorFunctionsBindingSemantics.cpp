#include "ErrorFunctionsBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace errorfunctions_binding_detail;
bool isErrorFunctionsFunction(std::string_view id) noexcept { return !errorFunctionsFunctionKey(id).empty(); }

std::string_view errorFunctionsFunctionKey(std::string_view id) noexcept {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    constexpr std::string_view prefix="math.special.error_functions.";
    if (!id.starts_with(prefix)) return {};
    id.remove_prefix(prefix.size());
    return std::find(keys.begin(),keys.end(),id)==keys.end()?std::string_view{}:id;
}

std::span<const std::string_view> errorFunctionsInputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view,1> unary{"x"};
    return isErrorFunctionsFunction(id)?std::span<const std::string_view>(unary):std::span<const std::string_view>{};
}

std::span<const std::string_view> errorFunctionsOutputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view,1> unary{"result"};
    static constexpr std::array<std::string_view,2> fresnel{"cosine_integral","sine_integral"};
    const auto key=errorFunctionsFunctionKey(id);
    return key.empty()?std::span<const std::string_view>{}:key=="fresnel"?
        std::span<const std::string_view>(fresnel):std::span<const std::string_view>(unary);
}

ErrorFunctionsTypeResult resolveErrorFunctionsPort(std::string_view id,std::string_view port,
    std::span<const ErrorFunctionsTypeEvidence> evidence) {
    const auto outputs=errorFunctionsOutputPortIds(id);
    if(outputs.empty())return failure(ResolutionStatus::Incompatible,"math.error_functions.unknown_function","This Error Functions operation is not registered.");
    if(port!="x"&&std::find(outputs.begin(),outputs.end(),port)==outputs.end())
        return failure(ResolutionStatus::Incompatible,"math.error_functions.missing_port","This operation has no such connector.");
    const TypeRef *input=nullptr;
    for(const auto &item:evidence){
        if(item.portId!="x")return failure(ResolutionStatus::Incompatible,"math.error_functions.unexpected_input","An unknown input was supplied.");
        if(input)return failure(ResolutionStatus::Incompatible,"math.error_functions.multiple_sources","A terminal has multiple sources.");
        input=&item.valueType;
        if(!input->known())return failure(ResolutionStatus::Unresolved,"math.error_functions.unresolved_type","The input type is not established.");
        std::size_t count=0;
        const auto reason=numeric_tree_math::validateSchema(*input,0,count,"Error Functions",false);
        if(!reason.empty())return failure(ResolutionStatus::Incompatible,"math.error_functions.unsupported_type",reason);
    }
    const auto type=input?realTree(*input):TypeRef::primitive("f64");
    const bool promotion=port=="x"&&input&&*input!=type;
    return {{promotion?ResolutionStatus::Coercion:ResolutionStatus::Compatible,type,
        promotion?"math.error_functions.real_promotion":"",promotion?"Real numeric leaves are promoted to f64.":""},input!=nullptr};
}
}
