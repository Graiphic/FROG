#include "GammaBindingSemanticDetail.hpp"
namespace frog::native_engine {
using namespace gamma_binding_detail;
bool isGammaFunction(std::string_view id) noexcept { return !gammaFunctionKey(id).empty(); }

std::string_view gammaFunctionKey(std::string_view id) noexcept {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    for(const auto &[identity,key]:identities)if(identity==id)return key;
    return {};
}

std::span<const std::string_view> gammaInputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view,1> x{"x"},n{"n"};
    static constexpr std::array<std::string_view,2> xy{"x","y"},xa{"x","a"};
    static constexpr std::array<std::string_view,3> xya{"x","y","a"};
    const auto key=gammaFunctionKey(id);
    if(key.empty())return {};
    if(key=="ln_factorial")return n;
    if(key=="beta")return xy;
    if(key=="beta_p")return xya;
    if(key=="gamma_p" || key=="gamma_q")return xa;
    return x;
}

std::span<const std::string_view> gammaOutputPortIds(std::string_view id) noexcept {
    static constexpr std::array<std::string_view,1> outputs{"value"};
    return isGammaFunction(id)?std::span<const std::string_view>(outputs):std::span<const std::string_view>{};
}

std::optional<double> gammaOptionalInputDefault(std::string_view id,std::string_view port) noexcept {
    if(port!="a")return {};
    const auto key=gammaFunctionKey(id);
    if(key=="gamma_p")return std::numeric_limits<double>::infinity();
    if(key=="beta_p")return 1.;
    return {};
}

GammaTypeResult resolveGammaPort(std::string_view id,std::string_view port,
    std::span<const GammaTypeEvidence> evidence) {
    const auto inputs=gammaInputPortIds(id);
    if(inputs.empty())return rejected(ResolutionStatus::Incompatible,"math.gamma.unknown_function","The Gamma function is not registered.");
    if(port!="value" && std::find(inputs.begin(),inputs.end(),port)==inputs.end())
        return rejected(ResolutionStatus::Incompatible,"math.gamma.missing_port","The selected Gamma instance has no such terminal.");
    std::array<bool,3> present{};
    bool inputPromotion=false;
    for(const auto &input:evidence) {
        const auto found=std::find(inputs.begin(),inputs.end(),input.portId);
        if(found==inputs.end())return rejected(ResolutionStatus::Incompatible,"math.gamma.unexpected_input","The Gamma instance received an unknown input.");
        auto &seen=present[static_cast<std::size_t>(found-inputs.begin())];
        if(seen)return rejected(ResolutionStatus::Incompatible,"math.gamma.multiple_sources","A Gamma terminal has multiple sources.");
        seen=true;
        if(!input.valueType.known())return rejected(ResolutionStatus::Unresolved,"math.gamma.unresolved_type","The Gamma input type is not established.");
        if(input.portId=="n") {
            if(input.valueType!=TypeRef::primitive("i32"))return rejected(ResolutionStatus::Incompatible,
                "math.gamma.integer_required","Ln(n!) requires exact i32; use an explicit conversion for another representation.");
        } else if(!realScalar(input.valueType))return rejected(ResolutionStatus::Incompatible,
            "math.gamma.unsupported_type","This Gamma profile requires real numeric scalars; complex and aggregate operands are not implicitly converted.");
        else if(input.portId==port && input.valueType!=TypeRef::primitive("f64"))inputPromotion=true;
    }
    bool complete=true;
    for(std::size_t i=0;i<inputs.size();++i)if(!present[i] && !gammaOptionalInputDefault(id,inputs[i]))complete=false;
    return {{inputPromotion?ResolutionStatus::Coercion:ResolutionStatus::Compatible,
        TypeRef::primitive(port=="n"?"i32":"f64"),
        inputPromotion?"math.gamma.real_promotion":"",inputPromotion?"The real numeric scalar is promoted to f64.":""},complete};
}
}
