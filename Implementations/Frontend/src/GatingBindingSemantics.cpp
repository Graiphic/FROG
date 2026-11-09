#include "GatingBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace gating_binding_detail;
bool isGatingFunction(std::string_view id) noexcept {return operation(id)!=Operation::Unknown;}

std::span<const std::string_view> gatingInputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view,1> unary{"x"};
    return isGatingFunction(id)?std::span<const std::string_view>(unary):std::span<const std::string_view>{};
}

GatingTypeResult resolveGatingPort(std::string_view id,std::string_view port,
    std::span<const GatingTypeEvidence> inputs) {
    if(!isGatingFunction(id))return failure(ResolutionStatus::Incompatible,"math.gating.unknown_function","The Gating function is not registered.");
    if(port!="x"&&port!="result")return failure(ResolutionStatus::Incompatible,"math.gating.missing_port","The Gating function has no such connector.");
    const TypeRef *input=nullptr;
    for(const auto &evidence:inputs){
        if(evidence.portId!="x")return failure(ResolutionStatus::Incompatible,"math.gating.unexpected_input","The function has no input with this identity.");
        if(input)return failure(ResolutionStatus::Incompatible,"math.gating.multiple_sources","The x operand has more than one source.");
        input=&evidence.valueType;
        std::size_t nodes=0;
        const auto reason=validateSchema(*input,0,nodes,"Gating",false);
        if(!reason.empty())return failure(reason=="The operand type is not established."?ResolutionStatus::Unresolved:
            ResolutionStatus::Incompatible,"math.gating.unsupported_type",reason);
    }
    return {{ResolutionStatus::Compatible,input?port=="result"?resultTree(*input):*input:TypeRef::primitive("f64"),{},{}},input!=nullptr};
}
}
