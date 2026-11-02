#include "frog_native_engine/ApplicationControlMemberContract.hpp"
#include <algorithm>

namespace frog::native_engine {
std::string applicationControlParameterPortId(std::string_view id,ApplicationControlPortDirection direction) {
    return "parameter:"+std::string(id)+(direction==ApplicationControlPortDirection::Input?":in":":out");
}
std::vector<ApplicationControlPort> applicationControlMethodPorts(const ApplicationControlMemberDescriptor &method,
    std::span<const ApplicationControlParameterBinding> bindings) {
    using D=ApplicationControlPortDirection;
    std::vector<ApplicationControlPort> result;
    if(method.kind!=ApplicationControlMemberKind::Method)return result;
    if(method.resultType && method.resultType->known() &&
        !(method.resultType->kind==binding::TypeKind::Primitive && method.resultType->name=="void"))
        result.push_back({"result",method.label,0U,D::Output,*method.resultType,false,{}});
    for(std::size_t i=0;i<method.parameters.size();++i) {
        const auto &param=method.parameters[i];
        const auto enabled=std::find_if(bindings.begin(),bindings.end(),[&](const auto &p){return p.id==param.id;});
        if(!param.required && enabled!=bindings.end() && !enabled->enabled)continue;
        if(param.direction!=ApplicationControlParameterDirection::Out)
            result.push_back({applicationControlParameterPortId(param.id,D::Input),param.label,i+1U,D::Input,param.type,param.required,param.defaultValue});
        if(param.direction!=ApplicationControlParameterDirection::In)
            result.push_back({applicationControlParameterPortId(param.id,D::Output),param.label,i+1U,D::Output,param.type,false,{}});
    }
    return result;
}
} // namespace frog::native_engine
