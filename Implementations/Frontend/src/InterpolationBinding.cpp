#include "frog_native_engine/InterpolationBinding.hpp"
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
    InterpolationVariantDescriptor descriptor;
    std::vector<PortLaw> inputs,outputs;
    std::vector<InterpolationInputDefault> defaults;
    std::vector<std::string_view> inputIds,outputIds;
};
struct FunctionLaw {
    std::vector<VariantLaw> variants;
    std::vector<InterpolationVariantDescriptor> variantDescriptors;
};

constexpr std::array<InterpolationFunctionDescriptor,14> descriptions{{
    {"math.interpolation.interpolate_1d","frog.studio.math.interpolation.interpolate_1d.owned.v1","Interpolate 1D"},
    {"math.interpolation.interpolate_2d","frog.studio.math.interpolation.interpolate_2d.grid_axes.owned.v1","Interpolate 2D"},
    {"math.interpolation.interpolate_2d_scattered","frog.studio.math.interpolation.interpolate_2d_scattered.points2d.owned.v1","Interpolate 2D Scattered"},
    {"math.interpolation.create_mesh_grid_2d","frog.studio.math.interpolation.create_mesh_grid_2d.xy.owned.v1","Create Mesh Grid (2D)"},
    {"math.interpolation.spline_interpolation_1d","frog.studio.math.interpolation.spline_interpolation_1d.owned.v1","Spline Interpolation 1D"},
    {"math.interpolation.hermite_interpolation_1d","frog.studio.math.interpolation.hermite_interpolation_1d.owned.v1","Hermite Interpolation 1D"},
    {"math.interpolation.create_interpolating_polynomial","frog.studio.math.interpolation.create_interpolating_polynomial.owned.v1","Create Interpolating Polynomial"},
    {"math.interpolation.evaluate_interpolating_polynomial","frog.studio.math.interpolation.evaluate_interpolating_polynomial.owned.v1","Evaluate Interpolating Polynomial"},
    {"math.interpolation.interpolate_1d_fourier","frog.studio.math.interpolation.interpolate_1d_fourier.owned.v1","Interpolate 1D Fourier"},
    {"math.interpolation.search_ordered_table","frog.studio.math.interpolation.search_ordered_table.owned.v1","Search Ordered Table"},
    {"math.interpolation.polynomial_interpolation","frog.studio.math.interpolation.polynomial_interpolation.owned.v1","Polynomial Interpolation"},
    {"math.interpolation.rational_interpolation","frog.studio.math.interpolation.rational_interpolation.owned.v1","Rational Interpolation"},
    {"math.interpolation.spline_interpolant","frog.studio.math.interpolation.spline_interpolant.owned.v1","Spline Interpolant"},
    {"math.interpolation.spline_interpolation","frog.studio.math.interpolation.spline_interpolation.owned.v1","Spline Interpolation"},
}};
const std::array<FunctionLaw,14> &laws() {
    static const auto value=[] {
        std::array<FunctionLaw,14> result{{
            {{
                {{"default","frog.studio.math.interpolation.interpolate_1d.owned.v1","Default"},{{"method","i32",false,"method"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"xi","array<f64>",false,"xi"},{"x_is_monotonic","bool",false,"X is monotonic"},{"ntimes","i32",false,"ntimes"}},{{"yi","array<f64>",false,"yi"},{"xi_used","array<f64>",false,"xi used"},{"error","i32",false,"error"}},{{"method","i32","1"},{"xi","array<f64>","[]"},{"x_is_monotonic","bool","false"},{"ntimes","i32","1"}},{},{}},
            },{}},
            {{
                {{"grid_axes","frog.studio.math.interpolation.interpolate_2d.grid_axes.owned.v1","Grid Axes"},{{"method","i32",false,"method"},{"z","array<f64,rank=2>",true,"Z"},{"x","array<f64>",false,"X"},{"y","array<f64>",false,"Y"},{"xi","array<f64,rank=2>",false,"xi"},{"yi","array<f64,rank=2>",false,"yi"},{"ntimes","i32",false,"ntimes"}},{{"zi","array<f64,rank=2>",false,"zi"},{"xi_used","array<f64,rank=2>",false,"xi used"},{"yi_used","array<f64,rank=2>",false,"yi used"},{"error","i32",false,"error"}},{{"method","i32","1"},{"x","array<f64>","[]"},{"y","array<f64>","[]"},{"xi","array<f64,rank=2>","[]"},{"yi","array<f64,rank=2>","[]"},{"ntimes","i32","1"}},{},{}},
                {{"grid_matrices","frog.studio.math.interpolation.interpolate_2d.grid_matrices.owned.v1","Grid Matrices"},{{"method","i32",false,"method"},{"z","array<f64,rank=2>",true,"Z"},{"x","array<f64,rank=2>",false,"X"},{"y","array<f64,rank=2>",false,"Y"},{"xi","array<f64,rank=2>",false,"xi"},{"yi","array<f64,rank=2>",false,"yi"},{"ntimes","i32",false,"ntimes"}},{{"zi","array<f64,rank=2>",false,"zi"},{"xi_used","array<f64,rank=2>",false,"xi used"},{"yi_used","array<f64,rank=2>",false,"yi used"},{"error","i32",false,"error"}},{{"method","i32","1"},{"x","array<f64,rank=2>","[]"},{"y","array<f64,rank=2>","[]"},{"xi","array<f64,rank=2>","[]"},{"yi","array<f64,rank=2>","[]"},{"ntimes","i32","1"}},{},{}},
                {{"query_axes","frog.studio.math.interpolation.interpolate_2d.query_axes.owned.v1","Query Axes"},{{"method","i32",false,"method"},{"z","array<f64,rank=2>",true,"Z"},{"x","array<f64>",false,"X"},{"y","array<f64>",false,"Y"},{"xi","array<f64>",false,"xi"},{"yi","array<f64>",false,"yi"},{"ntimes","i32",false,"ntimes"}},{{"zi","array<f64,rank=2>",false,"zi"},{"xi_used","array<f64,rank=2>",false,"xi used"},{"yi_used","array<f64,rank=2>",false,"yi used"},{"error","i32",false,"error"}},{{"method","i32","1"},{"x","array<f64>","[]"},{"y","array<f64>","[]"},{"xi","array<f64>","[]"},{"yi","array<f64>","[]"},{"ntimes","i32","1"}},{},{}},
            },{}},
            {{
                {{"points2d","frog.studio.math.interpolation.interpolate_2d_scattered.points2d.owned.v1","Points2D"},{{"method","i32",false,"method"},{"z","array<f64>",true,"Z"},{"x","array<f64>",true,"X"},{"y","array<f64>",true,"Y"},{"xi","array<f64,rank=2>",true,"Xi"},{"yi","array<f64,rank=2>",true,"Yi"}},{{"zi","array<f64,rank=2>",false,"Zi"},{"error","i32",false,"error"}},{{"method","i32","1"}},{},{}},
                {{"points1d","frog.studio.math.interpolation.interpolate_2d_scattered.points1d.owned.v1","Points1D"},{{"method","i32",false,"method"},{"z","array<f64>",true,"Z"},{"x","array<f64>",true,"X"},{"y","array<f64>",true,"Y"},{"xi","array<f64>",true,"Xi"},{"yi","array<f64>",true,"Yi"}},{{"zi","array<f64>",false,"Zi"},{"error","i32",false,"error"}},{{"method","i32","1"}},{},{}},
            },{}},
            {{
                {{"xy","frog.studio.math.interpolation.create_mesh_grid_2d.xy.owned.v1","Xy"},{{"x_domain","array<f64>",true,"x domain"},{"y_domain","array<f64>",true,"y domain"}},{{"x","array<f64,rank=2>",false,"X"},{"y","array<f64,rank=2>",false,"Y"},{"error","i32",false,"error"}},{},{},{}},
                {{"x","frog.studio.math.interpolation.create_mesh_grid_2d.x.owned.v1","X"},{{"domain","array<f64>",true,"domain"}},{{"x","array<f64,rank=2>",false,"X"},{"y","array<f64,rank=2>",false,"Y"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.interpolation.spline_interpolation_1d.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"xi","array<f64>",true,"xi"},{"initial_boundary","cluster<boundary:u16,derivative_value:f64>",false,"initial boundary"},{"final_boundary","cluster<boundary:u16,derivative_value:f64>",false,"final boundary"}},{{"yi","array<f64>",false,"yi"},{"piecewise_polynomial","cluster<x_locations:array<f64>,coefficients:array<f64,rank=2>>",false,"piecewise polynomial"},{"error","i32",false,"error"}},{{"initial_boundary","cluster<boundary:u16,derivative_value:f64>","{\"boundary\":0,\"derivative_value\":0}"},{"final_boundary","cluster<boundary:u16,derivative_value:f64>","{\"boundary\":0,\"derivative_value\":0}"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.interpolation.hermite_interpolation_1d.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"xi","array<f64>",true,"xi"}},{{"yi","array<f64>",false,"yi"},{"piecewise_polynomial","cluster<x_locations:array<f64>,coefficients:array<f64,rank=2>>",false,"piecewise polynomial"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.interpolation.create_interpolating_polynomial.owned.v1","Default"},{{"x_locations","array<f64>",true,"x locations"},{"coefficients","array<f64,rank=2>",true,"coefficients"}},{{"piecewise_polynomial","cluster<x_locations:array<f64>,coefficients:array<f64,rank=2>>",false,"piecewise polynomial"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.interpolation.evaluate_interpolating_polynomial.owned.v1","Default"},{{"piecewise_polynomial","cluster<x_locations:array<f64>,coefficients:array<f64,rank=2>>",true,"piecewise polynomial"},{"xi","array<f64>",true,"xi"}},{{"yi","array<f64>",false,"yi"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.interpolation.interpolate_1d_fourier.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"n","i32",true,"n"},{"dt_in","f64",false,"dt in"},{"type","u16",false,"type"}},{{"y","array<f64>",false,"Y"},{"interpolation_size","i32",false,"interpolation size"},{"dt_out","f64",false,"dt out"},{"error","i32",false,"error"}},{{"dt_in","f64","1"},{"type","u16","0"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.interpolation.search_ordered_table.owned.v1","Default"},{{"ordered_x_array","array<f64>",true,"ordered x array"},{"x_value","f64",true,"x value"},{"starting_index","i32",false,"starting index"}},{{"fractional_index","f64",false,"fractional index"}},{{"starting_index","i32","0"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.interpolation.polynomial_interpolation.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"x_value","f64",true,"x value"}},{{"interpolation_value","f64",false,"interpolation value"},{"interpolation_error","f64",false,"interpolation error"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.interpolation.rational_interpolation.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"x_value","f64",true,"x value"}},{{"interpolation_value","f64",false,"interpolation value"},{"interpolation_error","f64",false,"interpolation error"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.interpolation.spline_interpolant.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"initial_boundary","f64",false,"initial boundary"},{"final_boundary","f64",false,"final boundary"}},{{"interpolant","array<f64>",false,"Interpolant"},{"error","i32",false,"error"}},{{"initial_boundary","f64","1e+30"},{"final_boundary","f64","1e+30"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.interpolation.spline_interpolation.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"interpolant","array<f64>",true,"Interpolant"},{"x_value","f64",true,"x value"}},{{"interpolation_value","f64",false,"interpolation value"},{"error","i32",false,"error"}},{},{},{}},
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
struct Match { std::size_t function=14,variant=0;bool explicitVariant=false; };
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
InterpolationTypeResult failure(std::string_view code,std::string_view message) {
    return {{ResolutionStatus::Incompatible,{},std::string(code),std::string(message)},false};
}
bool realNumeric(const TypeRef &type) {
    return type.kind==TypeKind::Primitive&&type.numeric()&&!type.name.starts_with("c")&&type.name!="f128";
}
bool accepts(const TypeRef &source,const TypeRef &target) {
    if(source==target)return true;
    if(target.kind==TypeKind::Primitive&&target.numeric()) {
        const auto &scalar=source.kind==TypeKind::Enum&&source.children.size()==1?source.children.front():source;
        return realNumeric(scalar);
    }
    if(target.kind==TypeKind::Array&&target.children.size()==1) {
        if(source.kind!=TypeKind::Array||source.rank!=target.rank||source.children.size()!=1)return false;
        return target.children.front().numeric()?realNumeric(source.children.front()):source.children.front()==target.children.front();
    }
    // Piecewise polynomials and endpoint-condition records keep their exact
    // field identities, order, ranks and representations.
    return false;
}
} // namespace

std::span<const InterpolationFunctionDescriptor> interpolationFunctions() noexcept { return descriptions; }
const InterpolationFunctionDescriptor *findInterpolationFunction(std::string_view id) noexcept {
    const auto found=match(id);return found.function==descriptions.size()?nullptr:&descriptions[found.function];
}
bool isInterpolationFunction(std::string_view id) noexcept { return findInterpolationFunction(id)!=nullptr; }
std::span<const InterpolationVariantDescriptor> interpolationVariants(std::string_view id) noexcept {
    const auto found=match(id);return found.function==descriptions.size()?std::span<const InterpolationVariantDescriptor>{}:std::span<const InterpolationVariantDescriptor>{laws()[found.function].variantDescriptors};
}
std::string_view interpolationOperationId(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?law->descriptor.operationId:std::string_view{};
}
std::span<const std::string_view> interpolationInputPortIds(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const std::string_view>{law->inputIds}:std::span<const std::string_view>{};
}
std::span<const std::string_view> interpolationOutputPortIds(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const std::string_view>{law->outputIds}:std::span<const std::string_view>{};
}
std::span<const InterpolationInputDefault> interpolationInputDefaults(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const InterpolationInputDefault>{law->defaults}:std::span<const InterpolationInputDefault>{};
}
bool interpolationInputRequired(std::string_view id,std::string_view port,std::string_view variant) noexcept {
    const auto *law=select(id,variant);if(!law)return false;
    for(const auto &input:law->inputs)if(input.id==port)return input.required;
    return false;
}
std::string_view interpolationPortDisplayName(std::string_view id,std::string_view port,std::string_view variant) noexcept {
    const auto *law=select(id,variant);if(!law)return {};
    for(const auto &input:law->inputs)if(input.id==port)return input.displayName;
    for(const auto &output:law->outputs)if(output.id==port)return output.displayName;
    return {};
}
InterpolationTypeResult resolveInterpolationPort(std::string_view id,std::string_view port,std::span<const InterpolationTypeEvidence> inputs,std::string_view variant) {
    if(!isInterpolationFunction(id))return failure("math.interpolation.unknown_function","The Interpolation function is not registered.");
    const auto *law=select(id,variant);
    if(!law)return failure("math.interpolation.unknown_variant","The Interpolation variant is unknown or contradicts the native operation identity.");
    const PortLaw *requested=nullptr;bool requestedInput=false;
    for(const auto &p:law->inputs)if(p.id==port){requested=&p;requestedInput=true;}
    for(const auto &p:law->outputs)if(p.id==port)requested=&p;
    if(!requested)return failure("math.interpolation.missing_port","The selected Interpolation variant has no such port.");
    std::set<std::string_view> seen;bool complete=true;
    const auto evidence=[&](std::string_view name)->const TypeRef* {
        for(const auto &e:inputs)if(e.portId==name)return &e.valueType;return nullptr;
    };
    for(const auto &e:inputs) {
        if(!seen.insert(e.portId).second)return failure("math.interpolation.multiple_sources","A Interpolation input has more than one source.");
        const auto found=std::find_if(law->inputs.begin(),law->inputs.end(),[&](const auto &p){return p.id==e.portId;});
        if(found==law->inputs.end())return failure("math.interpolation.unexpected_input","The selected Interpolation variant has no input with this identity.");
        if(!e.valueType.known()){complete=false;continue;}
        const auto expected=binding::parseType(found->type);
        if(!expected.valid())return failure("math.interpolation.invalid_contract","The Interpolation port contract is not a recognized public type.");
        if(!accepts(e.valueType,expected.type))return failure("math.interpolation.input_type","The Interpolation input requires its declared real numeric rank or exact structured schema; complex values and polynomial/boundary records are never silently converted.");
    }
    for(const auto &p:law->inputs)if(p.required){const auto *t=evidence(p.id);if(!t||!t->known())complete=false;}
    const auto parsed=binding::parseType(requested->type);
    if(!parsed.valid())return failure("math.interpolation.invalid_contract","The Interpolation port contract is not a recognized public type.");
    auto type=parsed.type;const auto *source=requestedInput?evidence(port):nullptr;
    if(source&&source->known()&&*source!=type)
        return {{ResolutionStatus::Coercion,std::move(type),"math.interpolation.numeric_promotion","Interpolation calculates in DBL; real numeric promotion is explicit and may round large integers."},complete};
    return {{ResolutionStatus::Compatible,std::move(type),{},{}},complete};
}
} // namespace frog::native_engine
