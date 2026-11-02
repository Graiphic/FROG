#include "DiscreteMathBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace discretemath_binding_detail;
bool isDiscreteMathFunction(std::string_view id) noexcept {return operation(id)!=Operation::Unknown;}

std::span<const std::string_view> discreteMathInputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view,2> xy{"x","y"},nk{"n","k"},ab{"A","B"};
    static constexpr std::array<std::string_view,1> n{"n"};
    switch(operation(id)){case Operation::Gcd:case Operation::Lcm:return xy;case Operation::Factorial:case Operation::PrimeFactor:case Operation::CheckPrime:return n;
    case Operation::Permute:case Operation::Binomial:return nk;case Operation::ContinuedFraction:return ab;default:return {};}
}

std::span<const std::string_view> discreteMathOutputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view,1> gcd{"gcd"},lcm{"lcm"},result{"result"},factors{"factors"},primePort{"prime"};
    static constexpr std::array<std::string_view,2> binomial{"coefficient","error"},fraction{"result","error"};
    switch(operation(id)){case Operation::Gcd:return gcd;case Operation::Lcm:return lcm;case Operation::Factorial:case Operation::Permute:return result;
    case Operation::PrimeFactor:return factors;case Operation::CheckPrime:return primePort;case Operation::Binomial:return binomial;
    case Operation::ContinuedFraction:return fraction;default:return {};}
}

DiscreteMathTypeResult resolveDiscreteMathPort(std::string_view id,std::string_view port,std::span<const DiscreteMathTypeEvidence> inputs) {
    const auto op=operation(id);const auto names=discreteMathInputPortIds(id),outputs=discreteMathOutputPortIds(id);
    if(op==Operation::Unknown)return typeFailure(ResolutionStatus::Incompatible,"math.discrete.unknown_function","The Discrete Math function is not registered.");
    if(std::find(names.begin(),names.end(),port)==names.end()&&std::find(outputs.begin(),outputs.end(),port)==outputs.end())
        return typeFailure(ResolutionStatus::Incompatible,"math.discrete.missing_port","The Discrete Math function has no such terminal.");
    std::array<const TypeRef *,2> supplied{};
    for(const auto &evidence:inputs){
        const auto found=std::find(names.begin(),names.end(),evidence.portId);
        if(found==names.end())return typeFailure(ResolutionStatus::Incompatible,"math.discrete.unexpected_input","The Discrete Math function has no input with this identity.");
        auto &source=supplied[static_cast<std::size_t>(found-names.begin())];
        if(source)return typeFailure(ResolutionStatus::Incompatible,"math.discrete.multiple_sources","A required discrete input has multiple sources.");
        source=&evidence.valueType;
        if(source->kind==TypeKind::Unknown)return typeFailure(ResolutionStatus::Unresolved,"math.discrete.unresolved_type","The discrete input type is not established.");
        const auto expected=declaredType(op,evidence.portId);
        if(!inputMatches(*source,expected))return typeFailure(ResolutionStatus::Incompatible,"math.discrete.unsupported_type",
            "Terminal "+std::string(evidence.portId)+" requires exactly "+expected.canonical()+"; fractional coercion, lifting and alternate ranks are unavailable.");
    }
    bool complete=true;for(std::size_t i=0;i<names.size();++i)complete=complete&&supplied[i];
    const auto found=std::find(names.begin(),names.end(),port);
    const auto *actual=found==names.end()?nullptr:supplied[static_cast<std::size_t>(found-names.begin())];
    return {{ResolutionStatus::Compatible,actual?*actual:declaredType(op,port),{},{}},complete};
}
}
