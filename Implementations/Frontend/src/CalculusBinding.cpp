#include "frog_native_engine/CalculusBinding.hpp"
#include <algorithm>
#include <array>
#include <set>
#include <vector>

namespace frog::native_engine {
namespace {
using binding::TypeKind;
using binding::TypeRef;
using binding::ResolutionStatus;
struct PortLaw { std::string_view id,type;bool required;std::string_view displayName; };
struct VariantLaw {
    CalculusVariantDescriptor descriptor;
    std::vector<PortLaw> inputs,outputs;
    std::vector<CalculusInputDefault> defaults;
    std::vector<std::string_view> inputIds,outputIds;
};
struct FunctionLaw {
    std::vector<VariantLaw> variants;
    std::vector<CalculusVariantDescriptor> variantDescriptors;
};

constexpr std::array<CalculusFunctionDescriptor,6> descriptions{{
    {"math.calculus.numeric_integration","frog.studio.math.calculus.numeric_integration.1d.owned.v1","Numeric Integration"},
    {"math.calculus.uneven_numeric_integration","frog.studio.math.calculus.uneven_numeric_integration.owned.v1","Uneven Numeric Integration"},
    {"math.calculus.quadrature","frog.studio.math.calculus.quadrature.formula_1d.owned.v1","Quadrature"},
    {"math.calculus.integral_xt","frog.studio.math.calculus.integral_xt.owned.v1","Integral x(t)"},
    {"math.calculus.derivative_xt","frog.studio.math.calculus.derivative_xt.owned.v1","Derivative x(t)"},
    {"math.calculus.time_domain_math","frog.studio.math.calculus.time_domain_math.waveform_derivative_continuous.owned.v1","Time Domain Math"},
}};
const std::array<FunctionLaw,6> &laws() {
    static const auto value=[] {
        std::array<FunctionLaw,6> result{{
            {{
                {{"1d","frog.studio.math.calculus.numeric_integration.1d.owned.v1","1D"},{{"input_array","array<f64>",true,"Input Array"},{"dt","f64",false,"dt"},{"integration_method","i32",false,"integration method"}},{{"result","f64",false,"result"},{"error","i32",false,"error"}},{{"dt","f64","1"},{"integration_method","i32","0"}},{},{}},
                {{"2d","frog.studio.math.calculus.numeric_integration.2d.owned.v1","2D"},{{"input_array","array<f64,rank=2>",true,"Input Array"},{"interval_size","cluster<dx:f64,dy:f64>",false,"interval size"},{"integration_method","i32",false,"integration method"}},{{"result","f64",false,"result"},{"error","i32",false,"error"}},{{"interval_size","cluster<dx:f64,dy:f64>","{\"dx\":1,\"dy\":1}"},{"integration_method","i32","0"}},{},{}},
                {{"3d","frog.studio.math.calculus.numeric_integration.3d.owned.v1","3D"},{{"input_array","array<f64,rank=3>",true,"Input Array"},{"interval_size","cluster<dx:f64,dy:f64,dz:f64>",false,"interval size"},{"integration_method","i32",false,"integration method"}},{{"result","f64",false,"result"},{"error","i32",false,"error"}},{{"interval_size","cluster<dx:f64,dy:f64,dz:f64>","{\"dx\":1,\"dy\":1,\"dz\":1}"},{"integration_method","i32","0"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.calculus.uneven_numeric_integration.owned.v1","Uneven Numeric Integration"},{{"x","array<f64>",true,"X"},{"t","array<f64>",true,"T"}},{{"result","f64",false,"result"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"formula_1d","frog.studio.math.calculus.quadrature.formula_1d.owned.v1","Formula 1D"},{{"integrand","string",true,"integrand"},{"upper_limit","f64",false,"upper limit"},{"lower_limit","f64",false,"lower limit"},{"tolerance","f64",false,"tolerance"}},{{"result","f64",false,"result"},{"error","i32",false,"error"}},{{"upper_limit","f64","1"},{"lower_limit","f64","0"},{"tolerance","f64","1e-05"}},{},{}},
                {{"function_1d","frog.studio.math.calculus.quadrature.function_1d.owned.v1","Function 1D"},{{"data","variant",false,"data"},{"integrand","ref<connectivity.document.strict,cluster<in_x:array<f64>,in_data:variant,out_value:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>",true,"integrand"},{"upper_limit","f64",false,"upper limit"},{"lower_limit","f64",false,"lower limit"},{"tolerance","f64",false,"tolerance"}},{{"result","f64",false,"result"},{"error","i32",false,"error"}},{{"data","variant","null"},{"upper_limit","f64","1"},{"lower_limit","f64","0"},{"tolerance","f64","1e-05"}},{},{}},
                {{"formula_2d","frog.studio.math.calculus.quadrature.formula_2d.owned.v1","Formula 2D"},{{"integrand","string",true,"integrand"},{"upper_limits","cluster<x_upper_limit:f64,y_upper_limit:f64>",false,"Upper Limits"},{"lower_limits","cluster<x_lower_limit:f64,y_lower_limit:f64>",false,"Lower Limits"},{"tolerance","f64",false,"tolerance"}},{{"result","f64",false,"result"},{"error","i32",false,"error"}},{{"upper_limits","cluster<x_upper_limit:f64,y_upper_limit:f64>","{\"x_upper_limit\":1,\"y_upper_limit\":1}"},{"lower_limits","cluster<x_lower_limit:f64,y_lower_limit:f64>","{\"x_lower_limit\":0,\"y_lower_limit\":0}"},{"tolerance","f64","1e-05"}},{},{}},
                {{"function_2d","frog.studio.math.calculus.quadrature.function_2d.owned.v1","Function 2D"},{{"data","variant",false,"data"},{"integrand","ref<connectivity.document.strict,cluster<in_x:array<f64>,in_y:array<f64>,in_data:variant,out_value:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>",true,"integrand"},{"upper_limits","cluster<x_upper_limit:f64,y_upper_limit:f64>",false,"Upper Limits"},{"lower_limits","cluster<x_lower_limit:f64,y_lower_limit:f64>",false,"Lower Limits"},{"tolerance","f64",false,"tolerance"}},{{"result","f64",false,"result"},{"error","i32",false,"error"}},{{"data","variant","null"},{"upper_limits","cluster<x_upper_limit:f64,y_upper_limit:f64>","{\"x_upper_limit\":1,\"y_upper_limit\":1}"},{"lower_limits","cluster<x_lower_limit:f64,y_lower_limit:f64>","{\"x_lower_limit\":0,\"y_lower_limit\":0}"},{"tolerance","f64","1e-05"}},{},{}},
                {{"formula_3d","frog.studio.math.calculus.quadrature.formula_3d.owned.v1","Formula 3D"},{{"integrand","string",true,"integrand"},{"upper_limits","cluster<x_upper_limit:f64,y_upper_limit:f64,z_upper_limit:f64>",false,"Upper Limits"},{"lower_limits","cluster<x_lower_limit:f64,y_lower_limit:f64,z_lower_limit:f64>",false,"Lower Limits"},{"tolerance","f64",false,"tolerance"}},{{"result","f64",false,"result"},{"error","i32",false,"error"}},{{"upper_limits","cluster<x_upper_limit:f64,y_upper_limit:f64,z_upper_limit:f64>","{\"x_upper_limit\":1,\"y_upper_limit\":1,\"z_upper_limit\":1}"},{"lower_limits","cluster<x_lower_limit:f64,y_lower_limit:f64,z_lower_limit:f64>","{\"x_lower_limit\":0,\"y_lower_limit\":0,\"z_lower_limit\":0}"},{"tolerance","f64","1e-05"}},{},{}},
                {{"function_3d","frog.studio.math.calculus.quadrature.function_3d.owned.v1","Function 3D"},{{"data","variant",false,"data"},{"integrand","ref<connectivity.document.strict,cluster<in_x:array<f64>,in_y:array<f64>,in_z:array<f64>,in_data:variant,out_value:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>",true,"integrand"},{"upper_limits","cluster<x_upper_limit:f64,y_upper_limit:f64,z_upper_limit:f64>",false,"Upper Limits"},{"lower_limits","cluster<x_lower_limit:f64,y_lower_limit:f64,z_lower_limit:f64>",false,"Lower Limits"},{"tolerance","f64",false,"tolerance"}},{{"result","f64",false,"result"},{"error","i32",false,"error"}},{{"data","variant","null"},{"upper_limits","cluster<x_upper_limit:f64,y_upper_limit:f64,z_upper_limit:f64>","{\"x_upper_limit\":1,\"y_upper_limit\":1,\"z_upper_limit\":1}"},{"lower_limits","cluster<x_lower_limit:f64,y_lower_limit:f64,z_lower_limit:f64>","{\"x_lower_limit\":0,\"y_lower_limit\":0,\"z_lower_limit\":0}"},{"tolerance","f64","1e-05"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.calculus.integral_xt.owned.v1","Integral x(t)"},{{"x","array<f64>",true,"X"},{"initial_condition","array<f64>",false,"Initial Condition"},{"final_condition","array<f64>",false,"Final Condition"},{"dt","f64",false,"dt"},{"integration_method","i32",false,"integration method"}},{{"integral_x","array<f64>",false,"Integral X"},{"error","i32",false,"error"}},{{"initial_condition","array<f64>","[0]"},{"final_condition","array<f64>","[0]"},{"dt","f64","1"},{"integration_method","i32","1"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.calculus.derivative_xt.owned.v1","Derivative x(t)"},{{"x","array<f64>",true,"X"},{"initial_condition","array<f64>",false,"Initial Condition"},{"final_condition","array<f64>",false,"Final Condition"},{"dt","f64",false,"dt"},{"method","u16",false,"method"}},{{"dx_dt","array<f64>",false,"dX/dt"},{"error","i32",false,"error"}},{{"initial_condition","array<f64>","[0]"},{"final_condition","array<f64>","[0]"},{"dt","f64","1"},{"method","u16","0"}},{},{}},
            },{}},
            {{
                {{"waveform_derivative_continuous","frog.studio.math.calculus.time_domain_math.waveform_derivative_continuous.owned.v1","Waveform Derivative Continuous"},{{"signals","cluster<t0:timestamp,dt:f64,Y:array<f64>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","cluster<t0:timestamp,dt:f64,Y:array<f64>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveform_derivative_segment","frog.studio.math.calculus.time_domain_math.waveform_derivative_segment.owned.v1","Waveform Derivative Segment"},{{"signals","cluster<t0:timestamp,dt:f64,Y:array<f64>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","cluster<t0:timestamp,dt:f64,Y:array<f64>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveform_difference_continuous","frog.studio.math.calculus.time_domain_math.waveform_difference_continuous.owned.v1","Waveform Difference Continuous"},{{"signals","cluster<t0:timestamp,dt:f64,Y:array<f64>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","cluster<t0:timestamp,dt:f64,Y:array<f64>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveform_difference_segment","frog.studio.math.calculus.time_domain_math.waveform_difference_segment.owned.v1","Waveform Difference Segment"},{{"signals","cluster<t0:timestamp,dt:f64,Y:array<f64>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","cluster<t0:timestamp,dt:f64,Y:array<f64>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveform_integral_continuous","frog.studio.math.calculus.time_domain_math.waveform_integral_continuous.owned.v1","Waveform Integral Continuous"},{{"signals","cluster<t0:timestamp,dt:f64,Y:array<f64>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","cluster<t0:timestamp,dt:f64,Y:array<f64>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveform_integral_segment","frog.studio.math.calculus.time_domain_math.waveform_integral_segment.owned.v1","Waveform Integral Segment"},{{"signals","cluster<t0:timestamp,dt:f64,Y:array<f64>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","cluster<t0:timestamp,dt:f64,Y:array<f64>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveform_sum_continuous","frog.studio.math.calculus.time_domain_math.waveform_sum_continuous.owned.v1","Waveform Sum Continuous"},{{"signals","cluster<t0:timestamp,dt:f64,Y:array<f64>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","cluster<t0:timestamp,dt:f64,Y:array<f64>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveform_sum_segment","frog.studio.math.calculus.time_domain_math.waveform_sum_segment.owned.v1","Waveform Sum Segment"},{{"signals","cluster<t0:timestamp,dt:f64,Y:array<f64>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","cluster<t0:timestamp,dt:f64,Y:array<f64>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveforms_derivative_continuous","frog.studio.math.calculus.time_domain_math.waveforms_derivative_continuous.owned.v1","Waveforms Derivative Continuous"},{{"signals","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveforms_derivative_segment","frog.studio.math.calculus.time_domain_math.waveforms_derivative_segment.owned.v1","Waveforms Derivative Segment"},{{"signals","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveforms_difference_continuous","frog.studio.math.calculus.time_domain_math.waveforms_difference_continuous.owned.v1","Waveforms Difference Continuous"},{{"signals","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveforms_difference_segment","frog.studio.math.calculus.time_domain_math.waveforms_difference_segment.owned.v1","Waveforms Difference Segment"},{{"signals","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveforms_integral_continuous","frog.studio.math.calculus.time_domain_math.waveforms_integral_continuous.owned.v1","Waveforms Integral Continuous"},{{"signals","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveforms_integral_segment","frog.studio.math.calculus.time_domain_math.waveforms_integral_segment.owned.v1","Waveforms Integral Segment"},{{"signals","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveforms_sum_continuous","frog.studio.math.calculus.time_domain_math.waveforms_sum_continuous.owned.v1","Waveforms Sum Continuous"},{{"signals","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveforms_sum_segment","frog.studio.math.calculus.time_domain_math.waveforms_sum_segment.owned.v1","Waveforms Sum Segment"},{{"signals","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"result","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",false,"Result"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
            },{}},
        }};
        for(auto &function:result)for(auto &variant:function.variants) {
            for(const auto &p:variant.inputs)variant.inputIds.push_back(p.id);
            for(const auto &p:variant.outputs)variant.outputIds.push_back(p.id);
            function.variantDescriptors.push_back(variant.descriptor);
        }
        return result;
    }();
    return value;
}
std::string_view canonical(std::string_view id) {
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    if(id.ends_with(".owned.v1"))id.remove_suffix(9);
    return id;
}
struct Match { std::size_t function=6,variant=0;bool explicitVariant=false; };
Match match(std::string_view id) {
    id=canonical(id);
    for(std::size_t i=0;i<descriptions.size();++i) {
        if(descriptions[i].catalogId==id)return {i,0,false};
        for(std::size_t j=0;j<laws()[i].variants.size();++j)
            if(canonical(laws()[i].variants[j].descriptor.operationId)==id)return {i,j,true};
    }
    return {};
}
const VariantLaw *select(std::string_view id,std::string_view variant) {
    const auto found=match(id);if(found.function==descriptions.size())return nullptr;
    const auto &variants=laws()[found.function].variants;
    if(variant.empty())return &variants[found.variant];
    for(std::size_t i=0;i<variants.size();++i)
        if(variants[i].descriptor.variantId==variant)
            return found.explicitVariant&&i!=found.variant?nullptr:&variants[i];
    return nullptr;
}
CalculusTypeResult failure(std::string_view code,std::string_view message) {
    return {{ResolutionStatus::Incompatible,{},std::string(code),std::string(message)},false};
}
bool realNumeric(const TypeRef &type) {
    return type.kind==TypeKind::Primitive&&type.numeric()&&!type.name.starts_with("c")&&type.name!="f128";
}
bool accepts(const TypeRef &source,const TypeRef &target) {
    // Presentation field names are not semantic field identities. Keep the
    // same equality as the other public math families: nominal IDs, order,
    // ranks, representations and Enum constraints remain significant.
    if(source==target)return true;
    if(target.kind==TypeKind::Primitive&&target.numeric()) {
        const auto &scalar=source.kind==TypeKind::Enum&&source.children.size()==1?source.children.front():source;
        return realNumeric(scalar);
    }
    if(target.kind==TypeKind::Array&&target.children.size()==1) {
        if(source.kind!=TypeKind::Array||source.rank!=target.rank||source.children.size()!=1)return false;
        return target.children.front().numeric()&&realNumeric(source.children.front());
    }
    // Timed waveforms, axis/bound records and strict function references keep
    // their exact field identities, order, ranks and representations.
    return false;
}
} // namespace

std::span<const CalculusFunctionDescriptor> calculusFunctions() noexcept { return descriptions; }
const CalculusFunctionDescriptor *findCalculusFunction(std::string_view id) noexcept {
    const auto found=match(id);return found.function==descriptions.size()?nullptr:&descriptions[found.function];
}
bool isCalculusFunction(std::string_view id) noexcept { return findCalculusFunction(id)!=nullptr; }
std::span<const CalculusVariantDescriptor> calculusVariants(std::string_view id) noexcept {
    const auto found=match(id);return found.function==descriptions.size()?std::span<const CalculusVariantDescriptor>{}:std::span<const CalculusVariantDescriptor>{laws()[found.function].variantDescriptors};
}
std::string_view calculusOperationId(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?law->descriptor.operationId:std::string_view{};
}
std::span<const std::string_view> calculusInputPortIds(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const std::string_view>{law->inputIds}:std::span<const std::string_view>{};
}
std::span<const std::string_view> calculusOutputPortIds(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const std::string_view>{law->outputIds}:std::span<const std::string_view>{};
}
std::span<const CalculusInputDefault> calculusInputDefaults(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const CalculusInputDefault>{law->defaults}:std::span<const CalculusInputDefault>{};
}
bool calculusInputRequired(std::string_view id,std::string_view port,std::string_view variant) noexcept {
    const auto *law=select(id,variant);if(!law)return false;
    for(const auto &input:law->inputs)if(input.id==port)return input.required;
    return false;
}
std::string_view calculusPortDisplayName(std::string_view id,std::string_view port,std::string_view variant) noexcept {
    const auto *law=select(id,variant);if(!law)return {};
    for(const auto &input:law->inputs)if(input.id==port)return input.displayName;
    for(const auto &output:law->outputs)if(output.id==port)return output.displayName;
    return {};
}
CalculusTypeResult resolveCalculusPort(std::string_view id,std::string_view port,std::span<const CalculusTypeEvidence> inputs,std::string_view variant) {
    if(!isCalculusFunction(id))return failure("math.calculus.unknown_function","The Calculus function is not registered.");
    const auto *law=select(id,variant);
    if(!law)return failure("math.calculus.unknown_variant","The Calculus variant is unknown or contradicts the native operation identity.");
    const PortLaw *requested=nullptr;bool requestedInput=false;
    for(const auto &p:law->inputs)if(p.id==port){requested=&p;requestedInput=true;}
    for(const auto &p:law->outputs)if(p.id==port)requested=&p;
    if(!requested)return failure("math.calculus.missing_port","The selected Calculus variant has no such port.");
    std::set<std::string_view> seen;bool complete=true;
    const auto evidence=[&](std::string_view name)->const TypeRef* {
        for(const auto &e:inputs)if(e.portId==name)return &e.valueType;return nullptr;
    };
    for(const auto &e:inputs) {
        if(!seen.insert(e.portId).second)return failure("math.calculus.multiple_sources","A Calculus input has more than one source.");
        const auto found=std::find_if(law->inputs.begin(),law->inputs.end(),[&](const auto &p){return p.id==e.portId;});
        if(found==law->inputs.end())return failure("math.calculus.unexpected_input","The selected Calculus variant has no input with this identity.");
        if(!e.valueType.known()){complete=false;continue;}
        const auto expected=binding::parseType(found->type);
        if(!expected.valid())return failure("math.calculus.invalid_contract","The Calculus port contract is not a recognized public type.");
        if(!accepts(e.valueType,expected.type))return failure("math.calculus.input_type","The Calculus input requires its declared real numeric rank or exact structured schema; complex values, timed waveforms, strict references and axis/bound records are never silently converted.");
    }
    for(const auto &p:law->inputs)if(p.required){const auto *t=evidence(p.id);if(!t||!t->known())complete=false;}
    const auto parsed=binding::parseType(requested->type);
    if(!parsed.valid())return failure("math.calculus.invalid_contract","The Calculus port contract is not a recognized public type.");
    auto type=parsed.type;const auto *source=requestedInput?evidence(port):nullptr;
    if(source&&source->known()&&*source!=type)
        return {{ResolutionStatus::Coercion,std::move(type),"math.calculus.numeric_promotion","Calculus calculates in DBL; real numeric promotion is explicit and may round large integers."},complete};
    return {{ResolutionStatus::Compatible,std::move(type),{},{}},complete};
}
} // namespace frog::native_engine
