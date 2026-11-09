#include "frog_native_engine/FittingBinding.hpp"
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
    FittingVariantDescriptor descriptor;
    std::vector<PortLaw> inputs,outputs;
    std::vector<FittingInputDefault> defaults;
    std::vector<std::string_view> inputIds,outputIds;
};
struct FunctionLaw {
    std::vector<VariantLaw> variants;
    std::vector<FittingVariantDescriptor> variantDescriptors;
};
constexpr std::array<FittingFunctionDescriptor,28> descriptions{{
    {"math.fitting.linear_fit","frog.studio.math.fitting.linear_fit.owned.v1","Linear Fit"},
    {"math.fitting.exponential_fit","frog.studio.math.fitting.exponential_fit.owned.v1","Exp Fit"},
    {"math.fitting.power_fit","frog.studio.math.fitting.power_fit.owned.v1","Power Fit"},
    {"math.fitting.gaussian_peak_fit","frog.studio.math.fitting.gaussian_peak_fit.owned.v1","Gauss Peak Fit"},
    {"math.fitting.logarithm_fit","frog.studio.math.fitting.logarithm_fit.owned.v1","Logarithm Fit"},
    {"math.fitting.polynomial_fit","frog.studio.math.fitting.polynomial_fit.owned.v1","Polynomial Fit"},
    {"math.fitting.general_linear_fit","frog.studio.math.fitting.general_linear_fit.owned.v1","Gen. Linear Fit"},
    {"math.fitting.cubic_spline_fit","frog.studio.math.fitting.cubic_spline_fit.owned.v1","Cubic Spline Fit"},
    {"math.fitting.b_spline_fit","frog.studio.math.fitting.b_spline_fit.xy.owned.v1","B-Spline Fit"},
    {"math.fitting.fitting_on_sphere","frog.studio.math.fitting.fitting_on_sphere.owned.v1","Fit on a Sphere"},
    {"math.fitting.nonlinear_curve_fit","frog.studio.math.fitting.nonlinear_curve_fit.function.owned.v1","Nonlinear Curve Fit"},
    {"math.fitting.constrained_nonlinear_curve_fit","frog.studio.math.fitting.constrained_nonlinear_curve_fit.lm.owned.v1","Constrained Nonlinear Curve Fit"},
    {"math.fitting.curve_fit","frog.studio.math.fitting.curve_fit.linear.owned.v1","Curve Fit"},
    {"math.fitting.remove_outliers","frog.studio.math.fitting.remove_outliers.range.owned.v1","Remove Outlier"},
    {"math.fitting.goodness_of_fit","frog.studio.math.fitting.goodness_of_fit.owned.v1","Goodness of Fit"},
    {"math.fitting.nonlinear_curve_fit_intervals","frog.studio.math.fitting.nonlinear_curve_fit_intervals.function.owned.v1","Nonlinear Curve Fit Intervals"},
    {"math.fitting.linear_fit_intervals","frog.studio.math.fitting.linear_fit_intervals.confidence.owned.v1","Linear Fit Intvl"},
    {"math.fitting.exponential_fit_intervals","frog.studio.math.fitting.exponential_fit_intervals.confidence.owned.v1","Exp Fit Intvl"},
    {"math.fitting.power_fit_intervals","frog.studio.math.fitting.power_fit_intervals.confidence.owned.v1","Power Fit Intvl"},
    {"math.fitting.gaussian_fit_intervals","frog.studio.math.fitting.gaussian_fit_intervals.confidence.owned.v1","Gauss Fit Intvl"},
    {"math.fitting.logarithm_fit_intervals","frog.studio.math.fitting.logarithm_fit_intervals.confidence.owned.v1","Log Fit Intvl"},
    {"math.fitting.polynomial_fit_intervals","frog.studio.math.fitting.polynomial_fit_intervals.confidence.owned.v1","Poly Fit Intvl"},
    {"math.fitting.linear_fit_coefficients","frog.studio.math.fitting.linear_fit_coefficients.owned.v1","Linear Fit Coef"},
    {"math.fitting.exponential_fit_coefficients","frog.studio.math.fitting.exponential_fit_coefficients.owned.v1","Exp Fit Coef"},
    {"math.fitting.power_fit_coefficients","frog.studio.math.fitting.power_fit_coefficients.owned.v1","Power Fit Coef"},
    {"math.fitting.gaussian_fit_coefficients","frog.studio.math.fitting.gaussian_fit_coefficients.owned.v1","Gauss Fit Coef"},
    {"math.fitting.logarithm_fit_coefficients","frog.studio.math.fitting.logarithm_fit_coefficients.owned.v1","Log Fit Coef"},
    {"math.fitting.polynomial_fit_coefficients","frog.studio.math.fitting.polynomial_fit_coefficients.owned.v1","Poly Fit Coef"},
}};
const std::array<FunctionLaw,28> &laws() {
    static const auto value=[] {
        std::array<FunctionLaw,28> result{{
            {{
                {{"default","frog.studio.math.fitting.linear_fit.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"parameter_bounds","cluster<slope_min:f64,slope_max:f64,intercept_min:f64,intercept_max:f64>",false,"parameter bounds"}},{{"best_fit","array<f64>",false,"Best Linear Fit"},{"slope","f64",false,"slope"},{"intercept","f64",false,"intercept"},{"error","i32",false,"error"},{"residue","f64",false,"residue"}},{{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"parameter_bounds","cluster<slope_min:f64,slope_max:f64,intercept_min:f64,intercept_max:f64>","{\"slope_min\":\"-Infinity\",\"slope_max\":\"Infinity\",\"intercept_min\":\"-Infinity\",\"intercept_max\":\"Infinity\"}"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.exponential_fit.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"parameter_bounds","cluster<amp_min:f64,amp_max:f64,damping_min:f64,damping_max:f64,offset_min:f64,offset_max:f64>",false,"parameter bounds"}},{{"best_fit","array<f64>",false,"Best Exponential Fit"},{"amplitude","f64",false,"amplitude"},{"damping","f64",false,"damping"},{"offset","f64",false,"offset"},{"error","i32",false,"error"},{"residue","f64",false,"residue"}},{{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"parameter_bounds","cluster<amp_min:f64,amp_max:f64,damping_min:f64,damping_max:f64,offset_min:f64,offset_max:f64>","{\"amp_min\":\"-Infinity\",\"amp_max\":\"Infinity\",\"damping_min\":\"-Infinity\",\"damping_max\":\"Infinity\",\"offset_min\":0,\"offset_max\":0}"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.power_fit.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"parameter_bounds","cluster<amp_min:f64,amp_max:f64,power_min:f64,power_max:f64,offset_min:f64,offset_max:f64>",false,"parameter bounds"}},{{"best_fit","array<f64>",false,"Best Power Fit"},{"amplitude","f64",false,"amplitude"},{"power","f64",false,"power"},{"offset","f64",false,"offset"},{"error","i32",false,"error"},{"residue","f64",false,"residue"}},{{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"parameter_bounds","cluster<amp_min:f64,amp_max:f64,power_min:f64,power_max:f64,offset_min:f64,offset_max:f64>","{\"amp_min\":\"-Infinity\",\"amp_max\":\"Infinity\",\"power_min\":\"-Infinity\",\"power_max\":\"Infinity\",\"offset_min\":0,\"offset_max\":0}"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.gaussian_peak_fit.owned.v1","Default"},{{"initial_guess","cluster<amplitude:f64,center:f64,standard_deviation:f64,offset:f64>",false,"initial guess"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"parameter_bounds","cluster<amp_min:f64,amp_max:f64,center_min:f64,center_max:f64,deviation_min:f64,deviation_max:f64,offset_min:f64,offset_max:f64>",false,"parameter bounds"}},{{"offset","f64",false,"offset"},{"best_fit","array<f64>",false,"Best Gaussian Fit"},{"amplitude","f64",false,"amplitude"},{"center","f64",false,"center"},{"standard_deviation","f64",false,"standard deviation"},{"error","i32",false,"error"},{"residue","f64",false,"residue"}},{{"initial_guess","cluster<amplitude:f64,center:f64,standard_deviation:f64,offset:f64>","{\"amplitude\":\"NaN\",\"center\":\"NaN\",\"standard_deviation\":\"NaN\",\"offset\":\"NaN\"}"},{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"parameter_bounds","cluster<amp_min:f64,amp_max:f64,center_min:f64,center_max:f64,deviation_min:f64,deviation_max:f64,offset_min:f64,offset_max:f64>","{\"amp_min\":\"-Infinity\",\"amp_max\":\"Infinity\",\"center_min\":\"-Infinity\",\"center_max\":\"Infinity\",\"deviation_min\":\"-Infinity\",\"deviation_max\":\"Infinity\",\"offset_min\":0,\"offset_max\":0}"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.logarithm_fit.owned.v1","Default"},{{"base","f64",false,"base"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"parameter_bounds","cluster<amp_min:f64,amp_max:f64,scale_min:f64,scale_max:f64>",false,"parameter bounds"}},{{"best_fit","array<f64>",false,"Best Logarithm Fit"},{"amplitude","f64",false,"amplitude"},{"scale","f64",false,"scale"},{"error","i32",false,"error"},{"residue","f64",false,"residue"}},{{"base","f64","2.718281828459045"},{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"parameter_bounds","cluster<amp_min:f64,amp_max:f64,scale_min:f64,scale_max:f64>","{\"amp_min\":\"-Infinity\",\"amp_max\":\"Infinity\",\"scale_min\":\"-Infinity\",\"scale_max\":\"Infinity\"}"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.polynomial_fit.owned.v1","Default"},{{"coefficient_constraints","array<cluster<order:i32,coefficient:f64>>",false,"Coefficient Constraint"},{"polynomial_order","i32",false,"polynomial order"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"algorithm","u16",false,"algorithm"}},{{"best_fit","array<f64>",false,"Best Polynomial Fit"},{"coefficients","array<f64>",false,"Polynomial Coefficients"},{"error","i32",false,"error"},{"residue","f64",false,"residue"}},{{"coefficient_constraints","array<cluster<order:i32,coefficient:f64>>","[]"},{"polynomial_order","i32","2"},{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"algorithm","u16","0"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.general_linear_fit.owned.v1","Default"},{{"covariance_selector","u16",false,"covariance selector"},{"y","array<f64>",true,"Y"},{"h","array<f64,rank=2>",true,"H"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"algorithm","u16",false,"algorithm"}},{{"best_fit","array<f64>",false,"Best Fit"},{"coefficients","array<f64>",false,"Coefficients"},{"covariance","array<f64,rank=2>",false,"Covariance"},{"weight_out","array<f64>",false,"Weight Out"},{"error","i32",false,"error"},{"residue","f64",false,"residue"}},{{"covariance_selector","u16","0"},{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"algorithm","u16","0"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.cubic_spline_fit.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"balance_parameter","f64",false,"balance parameter"},{"smoothness","array<f64>",false,"Smoothness"}},{{"best_fit","array<f64>",false,"Best Cubic Spline Fit"},{"error","i32",false,"error"}},{{"weight","array<f64>","[]"},{"balance_parameter","f64","-1"},{"smoothness","array<f64>","[]"}},{},{}}
            },{}},
            {{
                {{"xy","frog.studio.math.fitting.b_spline_fit.xy.owned.v1","Xy"},{{"control_points","i32",false,"# of control points"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"degree","i32",false,"degree"},{"parameter_selection","u16",false,"parameter selection"}},{{"best_fit_y","array<f64>",false,"Best BSpline Fit Y"},{"best_fit_x","array<f64>",false,"Best BSpline Fit X"},{"error","i32",false,"error"},{"residue","f64",false,"residue"}},{{"control_points","i32","10"},{"weight","array<f64>","[]"},{"degree","i32","3"},{"parameter_selection","u16","2"}},{},{}},
                {{"nd","frog.studio.math.fitting.b_spline_fit.nd.owned.v1","Nd"},{{"control_points","i32",false,"# of control points"},{"data","array<f64,rank=2>",true,"Data"},{"weight","array<f64>",false,"Weight"},{"degree","i32",false,"degree"},{"parameter_selection","u16",false,"parameter selection"}},{{"best_fit","array<f64,rank=2>",false,"Best BSpline Fit"},{"error","i32",false,"error"},{"residue","f64",false,"residue"}},{{"control_points","i32","10"},{"weight","array<f64>","[]"},{"degree","i32","3"},{"parameter_selection","u16","2"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.fitting_on_sphere.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"y","array<f64>",true,"Y"},{"z","array<f64>",true,"Z"}},{{"center","array<f64>",false,"x0, y0, z0"},{"radius","f64",false,"r"},{"error","i32",false,"error"}},{},{},{}}
            },{}},
            {{
                {{"function","frog.studio.math.fitting.nonlinear_curve_fit.function.owned.v1","Function"},{{"data","variant",false,"data"},{"model_reference","ref<connectivity.document.strict,cluster<in_x:f64,in_parameters:array<f64>,in_data:variant,out_value:f64,out_gradient:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>",true,"f(x,a)"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"initial_parameters","array<f64>",true,"initial parameters"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"},{"termination","cluster<max_iteration:i32,tolerance:f64>",false,"termination"}},{{"number_of_function_calls","i32",false,"number of function calls"},{"best_fit","array<f64>",false,"best nonlinear fit"},{"best_fit_parameters","array<f64>",false,"best fit parameters"},{"covariance","array<f64,rank=2>",false,"covariance"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"residue","f64",false,"residue"}},{{"data","variant","null"},{"weight","array<f64>","[]"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"},{"termination","cluster<max_iteration:i32,tolerance:f64>","{\"max_iteration\":500,\"tolerance\":1e-08}"}},{},{}},
                {{"formula","frog.studio.math.fitting.nonlinear_curve_fit.formula.owned.v1","Formula"},{{"initial_parameters","array<f64>",true,"initial parameters"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"model_description","cluster<model:string,parameters:array<string>,x:string>",true,"model description"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"},{"termination","cluster<max_iteration:i32,tolerance:f64>",false,"termination"}},{{"best_fit","array<f64>",false,"best nonlinear fit"},{"best_fit_parameters","array<f64>",false,"best fit parameters"},{"covariance","array<f64,rank=2>",false,"covariance"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"},{"residue","f64",false,"residue"}},{{"weight","array<f64>","[]"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"},{"termination","cluster<max_iteration:i32,tolerance:f64>","{\"max_iteration\":500,\"tolerance\":1e-08}"}},{},{}}
            },{}},
            {{
                {{"lm","frog.studio.math.fitting.constrained_nonlinear_curve_fit.lm.owned.v1","Lm"},{{"data","variant",false,"data"},{"model_reference","ref<connectivity.document.strict,cluster<in_x:f64,in_parameters:array<f64>,in_data:variant,out_value:f64,out_gradient:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>",true,"f(x,a)"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"initial_parameters","array<f64>",true,"initial parameters"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"},{"termination","cluster<max_iteration:i32,tolerance:f64>",false,"termination"},{"parameter_bounds","cluster<parameter_minimum:array<f64>,parameter_maximum:array<f64>>",false,"parameter bounds"},{"method","u16",false,"method"}},{{"number_of_function_calls","i32",false,"number of function calls"},{"best_fit","array<f64>",false,"best nonlinear fit"},{"best_fit_parameters","array<f64>",false,"best fit parameters"},{"covariance","array<f64,rank=2>",false,"covariance"},{"residue","f64",false,"residue"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"data","variant","null"},{"weight","array<f64>","[]"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"},{"termination","cluster<max_iteration:i32,tolerance:f64>","{\"max_iteration\":500,\"tolerance\":1e-08}"},{"parameter_bounds","cluster<parameter_minimum:array<f64>,parameter_maximum:array<f64>>","{\"parameter_minimum\":[],\"parameter_maximum\":[]}"},{"method","u16","0"}},{},{}},
                {{"trdl","frog.studio.math.fitting.constrained_nonlinear_curve_fit.trdl.owned.v1","Trdl"},{{"data","variant",false,"data"},{"model_reference","ref<connectivity.document.strict,cluster<in_x:f64,in_parameters:array<f64>,in_data:variant,out_value:f64,out_gradient:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>",true,"f(x,a)"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"initial_parameters","array<f64>",true,"initial parameters"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"},{"termination","cluster<max_iteration:i32,tolerance:f64>",false,"termination"},{"parameter_bounds","cluster<parameter_minimum:array<f64>,parameter_maximum:array<f64>>",false,"parameter bounds"},{"method","u16",false,"method"}},{{"number_of_function_calls","i32",false,"number of function calls"},{"best_fit","array<f64>",false,"best nonlinear fit"},{"best_fit_parameters","array<f64>",false,"best fit parameters"},{"covariance","array<f64,rank=2>",false,"covariance"},{"residue","f64",false,"residue"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"data","variant","null"},{"weight","array<f64>","[]"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"},{"termination","cluster<max_iteration:i32,tolerance:f64>","{\"max_iteration\":500,\"tolerance\":1e-08}"},{"parameter_bounds","cluster<parameter_minimum:array<f64>,parameter_maximum:array<f64>>","{\"parameter_minimum\":[],\"parameter_maximum\":[]}"},{"method","u16","0"}},{},{}}
            },{}},
            {{
                {{"linear","frog.studio.math.fitting.curve_fit.linear.owned.v1","Linear"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"parameter_bounds","cluster<slope_min:f64,slope_max:f64,intercept_min:f64,intercept_max:f64>",false,"parameter bounds"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in"}},{{"best_fit","array<f64>",false,"best fit"},{"coefficients","array<f64>",false,"coefficients"},{"residue","f64",false,"residue"},{"residual","array<f64>",false,"residual"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"parameter_bounds","cluster<slope_min:f64,slope_max:f64,intercept_min:f64,intercept_max:f64>","{\"slope_min\":\"-Infinity\",\"slope_max\":\"Infinity\",\"intercept_min\":\"-Infinity\",\"intercept_max\":\"Infinity\"}"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"quadratic","frog.studio.math.fitting.curve_fit.quadratic.owned.v1","Quadratic"},{{"coefficient_constraints","array<cluster<order:i32,coefficient:f64>>",false,"Coefficient Constraint"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"algorithm","u16",false,"algorithm"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in"}},{{"best_fit","array<f64>",false,"best fit"},{"coefficients","array<f64>",false,"coefficients"},{"residue","f64",false,"residue"},{"residual","array<f64>",false,"residual"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"coefficient_constraints","array<cluster<order:i32,coefficient:f64>>","[]"},{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"algorithm","u16","0"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"spline","frog.studio.math.fitting.curve_fit.spline.owned.v1","Spline"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"balance_parameter","f64",false,"balance parameter"},{"smoothness","array<f64>",false,"Smoothness"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in"}},{{"best_fit","array<f64>",false,"best fit"},{"residue","f64",false,"residue"},{"residual","array<f64>",false,"residual"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"weight","array<f64>","[]"},{"balance_parameter","f64","-1"},{"smoothness","array<f64>","[]"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"polynomial","frog.studio.math.fitting.curve_fit.polynomial.owned.v1","Polynomial"},{{"coefficient_constraints","array<cluster<order:i32,coefficient:f64>>",false,"Coefficient Constraint"},{"polynomial_order","i32",false,"polynomial order"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"algorithm","u16",false,"algorithm"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in"}},{{"best_fit","array<f64>",false,"best fit"},{"coefficients","array<f64>",false,"coefficients"},{"residue","f64",false,"residue"},{"residual","array<f64>",false,"residual"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"coefficient_constraints","array<cluster<order:i32,coefficient:f64>>","[]"},{"polynomial_order","i32","5"},{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"algorithm","u16","0"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"general_linear","frog.studio.math.fitting.curve_fit.general_linear.owned.v1","General Linear"},{{"covariance_selector","u16",false,"covariance selector"},{"y","array<f64>",true,"Y"},{"h","array<f64,rank=2>",true,"H"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"algorithm","u16",false,"algorithm"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in"}},{{"best_fit","array<f64>",false,"best fit"},{"coefficients","array<f64>",false,"coefficients"},{"residue","f64",false,"residue"},{"residual","array<f64>",false,"residual"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"covariance_selector","u16","0"},{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"algorithm","u16","0"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"nonlinear","frog.studio.math.fitting.curve_fit.nonlinear.owned.v1","Nonlinear"},{{"initial_parameters","array<f64>",true,"initial parameters"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"model_description","cluster<model:string,parameters:array<string>,x:string>",true,"model description"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"},{"termination","cluster<max_iteration:i32,tolerance:f64>",false,"termination"}},{{"best_fit","array<f64>",false,"best fit"},{"coefficients","array<f64>",false,"coefficients"},{"residue","f64",false,"residue"},{"residual","array<f64>",false,"residual"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"weight","array<f64>","[]"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"},{"termination","cluster<max_iteration:i32,tolerance:f64>","{\"max_iteration\":500,\"tolerance\":1e-08}"}},{},{}}
            },{}},
            {{
                {{"range","frog.studio.math.fitting.remove_outliers.range.owned.v1","Range"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"range_type","u16",true,"range type"},{"range","array<f64>",true,"Range"}},{{"y_out","array<f64>",false,"Y Out"},{"x_out","array<f64>",false,"X Out"},{"weight_out","array<f64>",false,"Weight Out"},{"outlier_indices","array<i32>",false,"Outlier Indexes"},{"error","i32",false,"error"}},{{"weight","array<f64>","[]"}},{},{}},
                {{"index","frog.studio.math.fitting.remove_outliers.index.owned.v1","Index"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"indices","array<i32>",true,"Index"}},{{"y_out","array<f64>",false,"Y Out"},{"x_out","array<f64>",false,"X Out"},{"weight_out","array<f64>",false,"Weight Out"},{"error","i32",false,"error"}},{{"weight","array<f64>","[]"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.goodness_of_fit.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"best_fit","array<f64>",true,"Best Fit"},{"weight","array<f64>",false,"Weight"},{"degree_of_freedom","i32",false,"degree of freedom"}},{{"sse","f64",false,"SSE"},{"r_square","f64",false,"R-square"},{"rmse","f64",false,"RMSE"},{"error","i32",false,"error"}},{{"weight","array<f64>","[]"},{"degree_of_freedom","i32","-1"}},{},{}}
            },{}},
            {{
                {{"function","frog.studio.math.fitting.nonlinear_curve_fit_intervals.function.owned.v1","Function"},{{"data","variant",false,"data"},{"model_reference","ref<connectivity.document.strict,cluster<in_x:f64,in_parameters:array<f64>,in_data:variant,out_value:f64,out_gradient:array<f64>,out_diagnostic:cluster<status:bool,code:i32,source:string>>>",true,"f(x,a)"},{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"best_fit_parameters","array<f64>",true,"best fit parameters"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in (no error)"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"delta_best_fit_parameters","array<f64>",false,"delta best fit parameters"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"data","variant","null"},{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}}
            },{}},
            {{
                {{"confidence","frog.studio.math.fitting.linear_fit_intervals.confidence.owned.v1","Confidence"},{{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"slope","f64",true,"slope"},{"intercept","f64",true,"intercept"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"delta_slope","f64",false,"delta slope"},{"delta_intercept","f64",false,"delta intercept"},{"error","i32",false,"error"}},{{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"}},{},{}},
                {{"prediction","frog.studio.math.fitting.linear_fit_intervals.prediction.owned.v1","Prediction"},{{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"slope","f64",true,"slope"},{"intercept","f64",true,"intercept"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"error","i32",false,"error"}},{{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"}},{},{}}
            },{}},
            {{
                {{"confidence","frog.studio.math.fitting.exponential_fit_intervals.confidence.owned.v1","Confidence"},{{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"amplitude","f64",true,"amplitude"},{"damping","f64",true,"damping"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"delta_amplitude","f64",false,"delta amplitude"},{"delta_damping","f64",false,"delta damping"},{"error","i32",false,"error"}},{{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"}},{},{}},
                {{"prediction","frog.studio.math.fitting.exponential_fit_intervals.prediction.owned.v1","Prediction"},{{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"amplitude","f64",true,"amplitude"},{"damping","f64",true,"damping"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"error","i32",false,"error"}},{{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"}},{},{}}
            },{}},
            {{
                {{"confidence","frog.studio.math.fitting.power_fit_intervals.confidence.owned.v1","Confidence"},{{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"amplitude","f64",true,"amplitude"},{"power","f64",true,"power"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"delta_amplitude","f64",false,"delta amplitude"},{"delta_power","f64",false,"delta power"},{"error","i32",false,"error"}},{{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"}},{},{}},
                {{"prediction","frog.studio.math.fitting.power_fit_intervals.prediction.owned.v1","Prediction"},{{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"amplitude","f64",true,"amplitude"},{"power","f64",true,"power"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"error","i32",false,"error"}},{{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"}},{},{}}
            },{}},
            {{
                {{"confidence","frog.studio.math.fitting.gaussian_fit_intervals.confidence.owned.v1","Confidence"},{{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"amplitude","f64",true,"amplitude"},{"center","f64",true,"center"},{"standard_deviation","f64",true,"standard deviation"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"delta_amplitude","f64",false,"delta amplitude"},{"delta_center","f64",false,"delta center"},{"error","i32",false,"error"},{"delta_standard_deviation","f64",false,"delta standard deviation"}},{{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"}},{},{}},
                {{"prediction","frog.studio.math.fitting.gaussian_fit_intervals.prediction.owned.v1","Prediction"},{{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"amplitude","f64",true,"amplitude"},{"center","f64",true,"center"},{"standard_deviation","f64",true,"standard deviation"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"error","i32",false,"error"}},{{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"}},{},{}}
            },{}},
            {{
                {{"confidence","frog.studio.math.fitting.logarithm_fit_intervals.confidence.owned.v1","Confidence"},{{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"amplitude","f64",true,"amplitude"},{"scale","f64",true,"scale"},{"base","f64",false,"base"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"delta_amplitude","f64",false,"delta amplitude"},{"delta_scale","f64",false,"delta scale"},{"error","i32",false,"error"}},{{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"},{"base","f64","2.718281828459045"}},{},{}},
                {{"prediction","frog.studio.math.fitting.logarithm_fit_intervals.prediction.owned.v1","Prediction"},{{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"amplitude","f64",true,"amplitude"},{"scale","f64",true,"scale"},{"base","f64",false,"base"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"error","i32",false,"error"}},{{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"},{"base","f64","2.718281828459045"}},{},{}}
            },{}},
            {{
                {{"confidence","frog.studio.math.fitting.polynomial_fit_intervals.confidence.owned.v1","Confidence"},{{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"coefficients","array<f64>",true,"Polynomial Coefficients"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"delta_coefficients","array<f64>",false,"Delta Polynomial Coefficients"},{"error","i32",false,"error"}},{{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"}},{},{}},
                {{"prediction","frog.studio.math.fitting.polynomial_fit_intervals.prediction.owned.v1","Prediction"},{{"confidence_level","f64",false,"confidence level"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"coefficients","array<f64>",true,"Polynomial Coefficients"}},{{"upper_bound","array<f64>",false,"Upper Bound"},{"lower_bound","array<f64>",false,"Lower Bound"},{"error","i32",false,"error"}},{{"confidence_level","f64","0.95"},{"weight","array<f64>","[]"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.linear_fit_coefficients.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"}},{{"slope","f64",false,"slope"},{"intercept","f64",false,"intercept"},{"error","i32",false,"error"}},{{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.exponential_fit_coefficients.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"refine","bool",false,"refine?"}},{{"amplitude","f64",false,"amplitude"},{"damping","f64",false,"damping"},{"error","i32",false,"error"}},{{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"refine","bool","false"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.power_fit_coefficients.owned.v1","Default"},{{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"}},{{"amplitude","f64",false,"amplitude"},{"power","f64",false,"power"},{"error","i32",false,"error"}},{{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.gaussian_fit_coefficients.owned.v1","Default"},{{"initial_guess","cluster<amplitude:f64,center:f64,standard_deviation:f64>",false,"initial guess"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"}},{{"amplitude","f64",false,"amplitude"},{"center","f64",false,"center"},{"standard_deviation","f64",false,"standard deviation"},{"error","i32",false,"error"}},{{"initial_guess","cluster<amplitude:f64,center:f64,standard_deviation:f64>","{\"amplitude\":\"NaN\",\"center\":\"NaN\",\"standard_deviation\":\"NaN\"}"},{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.logarithm_fit_coefficients.owned.v1","Default"},{{"base","f64",false,"base"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"}},{{"amplitude","f64",false,"amplitude"},{"scale","f64",false,"scale"},{"error","i32",false,"error"}},{{"base","f64","2.718281828459045"},{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"}},{},{}}
            },{}},
            {{
                {{"default","frog.studio.math.fitting.polynomial_fit_coefficients.owned.v1","Default"},{{"coefficient_constraints","array<cluster<order:i32,coefficient:f64>>",false,"Coefficient Constraint"},{"polynomial_order","i32",false,"polynomial order"},{"y","array<f64>",true,"Y"},{"x","array<f64>",true,"X"},{"weight","array<f64>",false,"Weight"},{"tolerance","f64",false,"tolerance"},{"method","u16",false,"method"},{"algorithm","u16",false,"algorithm"}},{{"coefficients","array<f64>",false,"Polynomial Coefficients"},{"error","i32",false,"error"}},{{"coefficient_constraints","array<cluster<order:i32,coefficient:f64>>","[]"},{"polynomial_order","i32","2"},{"weight","array<f64>","[]"},{"tolerance","f64","0.0001"},{"method","u16","0"},{"algorithm","u16","0"}},{},{}}
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
struct Match { std::size_t function=28,variant=0;bool explicitVariant=false; };
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
FittingTypeResult failure(std::string_view code,std::string_view message) {
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
    // Formula records, callback references, constraints, termination, opaque
    // user data and error records keep their exact schemas and identities.
    return false;
}
} // namespace

std::span<const FittingFunctionDescriptor> fittingFunctions() noexcept { return descriptions; }
const FittingFunctionDescriptor *findFittingFunction(std::string_view id) noexcept {
    const auto found=match(id);return found.function==descriptions.size()?nullptr:&descriptions[found.function];
}
bool isFittingFunction(std::string_view id) noexcept { return findFittingFunction(id)!=nullptr; }
std::span<const FittingVariantDescriptor> fittingVariants(std::string_view id) noexcept {
    const auto found=match(id);return found.function==descriptions.size()?std::span<const FittingVariantDescriptor>{}:std::span<const FittingVariantDescriptor>{laws()[found.function].variantDescriptors};
}
std::string_view fittingOperationId(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?law->descriptor.operationId:std::string_view{};
}
std::span<const std::string_view> fittingInputPortIds(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const std::string_view>{law->inputIds}:std::span<const std::string_view>{};
}
std::span<const std::string_view> fittingOutputPortIds(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const std::string_view>{law->outputIds}:std::span<const std::string_view>{};
}
std::span<const FittingInputDefault> fittingInputDefaults(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const FittingInputDefault>{law->defaults}:std::span<const FittingInputDefault>{};
}
bool fittingInputRequired(std::string_view id,std::string_view port,std::string_view variant) noexcept {
    const auto *law=select(id,variant);if(!law)return false;
    for(const auto &input:law->inputs)if(input.id==port)return input.required;
    return false;
}
std::string_view fittingPortDisplayName(std::string_view id,std::string_view port,std::string_view variant) noexcept {
    const auto *law=select(id,variant);if(!law)return {};
    for(const auto &input:law->inputs)if(input.id==port)return input.displayName;
    for(const auto &output:law->outputs)if(output.id==port)return output.displayName;
    return {};
}
FittingTypeResult resolveFittingPort(std::string_view id,std::string_view port,std::span<const FittingTypeEvidence> inputs,std::string_view variant) {
    if(!isFittingFunction(id))return failure("math.fitting.unknown_function","The Fitting function is not registered.");
    const auto *law=select(id,variant);
    if(!law)return failure("math.fitting.unknown_variant","The Fitting variant is unknown or contradicts the native operation identity.");
    const PortLaw *requested=nullptr;bool requestedInput=false;
    for(const auto &p:law->inputs)if(p.id==port){requested=&p;requestedInput=true;}
    for(const auto &p:law->outputs)if(p.id==port)requested=&p;
    if(!requested)return failure("math.fitting.missing_port","The selected Fitting variant has no such port.");
    std::set<std::string_view> seen;bool complete=true;
    const auto evidence=[&](std::string_view name)->const TypeRef* {
        for(const auto &e:inputs)if(e.portId==name)return &e.valueType;return nullptr;
    };
    for(const auto &e:inputs) {
        if(!seen.insert(e.portId).second)return failure("math.fitting.multiple_sources","A Fitting input has more than one source.");
        const auto found=std::find_if(law->inputs.begin(),law->inputs.end(),[&](const auto &p){return p.id==e.portId;});
        if(found==law->inputs.end())return failure("math.fitting.unexpected_input","The selected Fitting variant has no input with this identity.");
        if(!e.valueType.known()){complete=false;continue;}
        const auto expected=binding::parseType(found->type);
        if(!expected.valid())return failure("math.fitting.invalid_contract","The Fitting port contract is not a recognized public type.");
        if(!accepts(e.valueType,expected.type))return failure("math.fitting.input_type","The Fitting input requires its declared real numeric rank or exact structured schema; complex values and model representations are never silently converted.");
    }
    for(const auto &p:law->inputs)if(p.required){const auto *t=evidence(p.id);if(!t||!t->known())complete=false;}
    const auto parsed=binding::parseType(requested->type);
    if(!parsed.valid())return failure("math.fitting.invalid_contract","The Fitting port contract is not a recognized public type.");
    auto type=parsed.type;const auto *source=requestedInput?evidence(port):nullptr;
    if(source&&source->known()&&*source!=type)
        return {{ResolutionStatus::Coercion,std::move(type),"math.fitting.numeric_promotion","Fitting calculates in DBL; real numeric promotion is explicit and may round large integers."},complete};
    return {{ResolutionStatus::Compatible,std::move(type),{},{}},complete};
}
} // namespace frog::native_engine
