#include "frog_native_engine/StatisticsBinding.hpp"
#include <algorithm>
#include <array>
#include <set>
#include <vector>
namespace frog::native_engine {
namespace {
using binding::TypeKind;
using binding::TypeRef;
using binding::ResolutionStatus;
struct PortLaw {std::string_view id,type;bool required;std::string_view displayName;};
struct VariantLaw {
    StatisticsVariantDescriptor descriptor;
    std::vector<PortLaw> inputs,outputs;
    std::vector<StatisticsInputDefault> defaults;
    std::vector<std::string_view> inputIds,outputIds;
};
struct FunctionLaw {std::vector<VariantLaw> variants;std::vector<StatisticsVariantDescriptor> variantDescriptors;};
constexpr std::array<StatisticsFunctionDescriptor,39> descriptions{{
    {"math.statistics.mean","frog.studio.math.statistics.mean.real.owned.v1","Mean"},
    {"math.statistics.measures_of_mean","frog.studio.math.statistics.measures_of_mean.owned.v1","Measures of Mean"},
    {"math.statistics.std_variance","frog.studio.math.statistics.std_variance.owned.v1","Std Deviation and Variance"},
    {"math.statistics.skewness_kurtosis","frog.studio.math.statistics.skewness_kurtosis.owned.v1","Skewness and Kurtosis"},
    {"math.statistics.measures_of_spread","frog.studio.math.statistics.measures_of_spread.owned.v1","Measures of Spread"},
    {"math.statistics.percentiles","frog.studio.math.statistics.percentiles.scalar.owned.v1","Percentiles"},
    {"math.statistics.rms","frog.studio.math.statistics.rms.real.owned.v1","RMS"},
    {"math.statistics.mse","frog.studio.math.statistics.mse.owned.v1","MSE"},
    {"math.statistics.moment","frog.studio.math.statistics.moment.owned.v1","Moment about Mean"},
    {"math.statistics.covariance_matrix","frog.studio.math.statistics.covariance_matrix.owned.v1","Covariance Matrix"},
    {"math.statistics.mode","frog.studio.math.statistics.mode.unimodal.owned.v1","Mode"},
    {"math.statistics.median","frog.studio.math.statistics.median.owned.v1","Median"},
    {"math.statistics.histogram","frog.studio.math.statistics.histogram.owned.v1","Histogram"},
    {"math.statistics.general_histogram","frog.studio.math.statistics.general_histogram.owned.v1","General Histogram"},
    {"math.statistics.pearson","frog.studio.math.statistics.pearson.owned.v1","Correlation Coefficient (Pearson)"},
    {"math.statistics.spearman","frog.studio.math.statistics.spearman.owned.v1","Correlation Coefficient (Spearman)"},
    {"math.statistics.kendall","frog.studio.math.statistics.kendall.owned.v1","Correlation Coefficient (Kendall Tau)"},
    {"math.statistics.statistics_express","frog.studio.math.statistics.statistics_express.array.owned.v1","Statistics"},
    {"math.statistics.histogram_express","frog.studio.math.statistics.histogram_express.array.owned.v1","Histogram (Express)"},
    {"math.statistics.probability.continuous_cdf","frog.studio.math.statistics.probability.continuous_cdf.beta.owned.v1","Continuous CDF"},
    {"math.statistics.probability.continuous_inverse_cdf","frog.studio.math.statistics.probability.continuous_inverse_cdf.beta.owned.v1","Continuous Inverse CDF"},
    {"math.statistics.probability.continuous_pdf","frog.studio.math.statistics.probability.continuous_pdf.beta.owned.v1","Continuous PDF"},
    {"math.statistics.probability.continuous_random","frog.studio.math.statistics.probability.continuous_random.beta.owned.v1","Continuous Random"},
    {"math.statistics.probability.continuous_moments","frog.studio.math.statistics.probability.continuous_moments.beta.owned.v1","Continuous Moments"},
    {"math.statistics.probability.discrete_cdf","frog.studio.math.statistics.probability.discrete_cdf.bernoulli.owned.v1","Discrete CDF"},
    {"math.statistics.probability.discrete_inverse_cdf","frog.studio.math.statistics.probability.discrete_inverse_cdf.bernoulli.owned.v1","Discrete Inverse CDF"},
    {"math.statistics.probability.discrete_pf","frog.studio.math.statistics.probability.discrete_pf.bernoulli.owned.v1","Discrete PF"},
    {"math.statistics.probability.discrete_random","frog.studio.math.statistics.probability.discrete_random.bernoulli.owned.v1","Discrete Random"},
    {"math.statistics.probability.discrete_moments","frog.studio.math.statistics.probability.discrete_moments.bernoulli.owned.v1","Discrete Moments"},
    {"math.statistics.hypothesis.t_test","frog.studio.math.statistics.hypothesis.t_test.owned.v1","T Test"},
    {"math.statistics.hypothesis.z_test","frog.studio.math.statistics.hypothesis.z_test.owned.v1","Z Test"},
    {"math.statistics.hypothesis.correlation_test","frog.studio.math.statistics.hypothesis.correlation_test.owned.v1","Correlation Test"},
    {"math.statistics.hypothesis.sign_test","frog.studio.math.statistics.hypothesis.sign_test.median.owned.v1","Sign Test"},
    {"math.statistics.hypothesis.wilcoxon","frog.studio.math.statistics.hypothesis.wilcoxon.median.owned.v1","Wilcoxon Signed Rank Test"},
    {"math.statistics.hypothesis.rank_transformation","frog.studio.math.statistics.hypothesis.rank_transformation.owned.v1","Rank Transformation"},
    {"math.statistics.hypothesis.contingency_table","frog.studio.math.statistics.hypothesis.contingency_table.owned.v1","Contingency Table"},
    {"math.statistics.anova.anova_1d","frog.studio.math.statistics.anova.anova_1d.owned.v1","1D ANOVA"},
    {"math.statistics.anova.anova_2d","frog.studio.math.statistics.anova.anova_2d.owned.v1","2D ANOVA"},
    {"math.statistics.anova.anova_3d","frog.studio.math.statistics.anova.anova_3d.owned.v1","3D ANOVA"},
}};
const std::array<FunctionLaw,39> &laws() {
    static const auto value=[] {
        std::array<FunctionLaw,39> result{{
            {{
                {{"real","frog.studio.math.statistics.mean.real.owned.v1","Real"},{{"x","array<f64>",true,"X"}},{{"mean","f64",false,"mean"},{"error","i32",false,"error"}},{},{},{}},
                {{"complex","frog.studio.math.statistics.mean.complex.owned.v1","Complex"},{{"x","array<c64>",true,"X"}},{{"mean","c64",false,"mean"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.measures_of_mean.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"type","u16",false,"type"},{"percent","f64",false,"percent (trim)"}},{{"mean","f64",false,"mean"},{"error","i32",false,"error"}},{{"type","u16","0"},{"percent","f64","10"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.std_variance.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"weighting","u16",false,"Weighting"}},{{"mean","f64",false,"mean"},{"std_deviation","f64",false,"std deviation"},{"variance","f64",false,"variance"}},{{"weighting","u16","0"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.skewness_kurtosis.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"weighting","u16",false,"Weighting"}},{{"skewness","f64",false,"skewness"},{"kurtosis","f64",false,"kurtosis"},{"error","i32",false,"error"}},{{"weighting","u16","0"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.measures_of_spread.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"type","i32",false,"type"}},{{"spread","f64",false,"spread"},{"error","i32",false,"error"}},{{"type","i32","0"}},{},{}},
            },{}},
            {{
                {{"scalar","frog.studio.math.statistics.percentiles.scalar.owned.v1","Scalar"},{{"x","array<f64>",true,"X"},{"p","f64",true,"p (%)"}},{{"percentile","f64",false,"percentile"},{"error","i32",false,"error"}},{},{},{}},
                {{"array","frog.studio.math.statistics.percentiles.array.owned.v1","Array"},{{"x","array<f64>",true,"X"},{"p","array<f64>",true,"p (%)"}},{{"percentiles","array<f64>",false,"percentiles"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"real","frog.studio.math.statistics.rms.real.owned.v1","Real"},{{"x","array<f64>",true,"X"}},{{"rms","f64",false,"rms"},{"error","i32",false,"error"}},{},{},{}},
                {{"complex","frog.studio.math.statistics.rms.complex.owned.v1","Complex"},{{"x","array<c64>",true,"X"}},{{"rms","f64",false,"rms"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.mse.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"y","array<f64>",true,"Y"}},{{"mse","f64",false,"mse"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.moment.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"order","i32",false,"order"}},{{"moment","f64",false,"moment"},{"error","i32",false,"error"}},{{"order","i32","2"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.covariance_matrix.owned.v1","Default"},{{"x","array<f64,rank=2>",true,"X"}},{{"covariance","array<f64,rank=2>",false,"covariance"},{"means","array<f64>",false,"means"}},{},{},{}},
            },{}},
            {{
                {{"unimodal","frog.studio.math.statistics.mode.unimodal.owned.v1","Unimodal"},{{"x","array<f64>",true,"X"},{"intervals","i32",false,"intervals"}},{{"mode","f64",false,"mode"},{"error","i32",false,"error"}},{{"intervals","i32","100"}},{},{}},
                {{"multimodal","frog.studio.math.statistics.mode.multimodal.owned.v1","Multimodal"},{{"x","array<f64>",true,"X"},{"intervals","i32",false,"intervals"}},{{"modes","array<f64>",false,"modes"},{"error","i32",false,"error"}},{{"intervals","i32","100"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.median.owned.v1","Default"},{{"x","array<f64>",true,"X"}},{{"median","f64",false,"median"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.histogram.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"intervals","i32",false,"intervals"}},{{"graph","cluster<x:array<f64>,y:array<f64>>",false,"Histogram Graph"},{"counts","array<i32>",false,"Histogram: h(x)"},{"centers","array<f64>",false,"X Values"},{"error","i32",false,"error"}},{{"intervals","i32","10"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.general_histogram.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"bins","array<cluster<lower:f64,upper:f64,inclusion:u16>>",false,"Bins"},{"maximum","f64",false,"max"},{"minimum","f64",false,"min"},{"intervals","i32",false,"# bins"},{"inclusion","u16",false,"inclusion"}},{{"graph","cluster<x:array<f64>,y:array<f64>>",false,"Histogram Graph"},{"counts","array<i32>",false,"Histogram"},{"centers","array<f64>",false,"Axis"},{"outside","cluster<total:i32,below:i32,above:i32>",false,"# outside"},{"error","i32",false,"error"}},{{"bins","array<cluster<lower:f64,upper:f64,inclusion:u16>>","[]"},{"intervals","i32","10"},{"inclusion","u16","0"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.pearson.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"y","array<f64>",true,"Y"}},{{"r","f64",false,"r"},{"r_squared","f64",false,"r squared"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.spearman.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"y","array<f64>",true,"Y"}},{{"r","f64",false,"r"},{"r_squared","f64",false,"r squared"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.kendall.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"y","array<f64>",true,"Y"}},{{"r","f64",false,"r"},{"r_squared","f64",false,"r squared"}},{},{},{}},
            },{}},
            {{
                {{"array","frog.studio.math.statistics.statistics_express.array.owned.v1","Array"},{{"signals","array<f64>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in"}},{{"mean","f64",false,"mean"},{"median","f64",false,"median"},{"mode","f64",false,"mode"},{"sum","f64",false,"sum"},{"rms","f64",false,"rms"},{"std_deviation","f64",false,"std deviation"},{"variance","f64",false,"variance"},{"kurtosis","f64",false,"kurtosis"},{"skewness","f64",false,"skewness"},{"maximum","f64",false,"maximum"},{"minimum","f64",false,"minimum"},{"maximum_time","timestamp",false,"maximum time"},{"minimum_time","timestamp",false,"minimum time"},{"maximum_index","i32",false,"maximum index"},{"minimum_index","i32",false,"minimum index"},{"range","f64",false,"range"},{"first_time","timestamp",false,"first time"},{"last_time","timestamp",false,"last time"},{"first_value","f64",false,"first value"},{"last_value","f64",false,"last value"},{"samples","i32",false,"samples"},{"dt","f64",false,"dt"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveform","frog.studio.math.statistics.statistics_express.waveform.owned.v1","Waveform"},{{"signals","cluster<t0:timestamp,dt:f64,Y:array<f64>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in"}},{{"mean","f64",false,"mean"},{"median","f64",false,"median"},{"mode","f64",false,"mode"},{"sum","f64",false,"sum"},{"rms","f64",false,"rms"},{"std_deviation","f64",false,"std deviation"},{"variance","f64",false,"variance"},{"kurtosis","f64",false,"kurtosis"},{"skewness","f64",false,"skewness"},{"maximum","f64",false,"maximum"},{"minimum","f64",false,"minimum"},{"maximum_time","timestamp",false,"maximum time"},{"minimum_time","timestamp",false,"minimum time"},{"maximum_index","i32",false,"maximum index"},{"minimum_index","i32",false,"minimum index"},{"range","f64",false,"range"},{"first_time","timestamp",false,"first time"},{"last_time","timestamp",false,"last time"},{"first_value","f64",false,"first value"},{"last_value","f64",false,"last value"},{"samples","i32",false,"samples"},{"dt","f64",false,"dt"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
                {{"waveforms","frog.studio.math.statistics.statistics_express.waveforms.owned.v1","Waveforms"},{{"signals","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",true,"Signals"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in"}},{{"mean","f64",false,"mean"},{"median","f64",false,"median"},{"mode","f64",false,"mode"},{"sum","f64",false,"sum"},{"rms","f64",false,"rms"},{"std_deviation","f64",false,"std deviation"},{"variance","f64",false,"variance"},{"kurtosis","f64",false,"kurtosis"},{"skewness","f64",false,"skewness"},{"maximum","f64",false,"maximum"},{"minimum","f64",false,"minimum"},{"maximum_time","timestamp",false,"maximum time"},{"minimum_time","timestamp",false,"minimum time"},{"maximum_index","i32",false,"maximum index"},{"minimum_index","i32",false,"minimum index"},{"range","f64",false,"range"},{"first_time","timestamp",false,"first time"},{"last_time","timestamp",false,"last time"},{"first_value","f64",false,"first value"},{"last_value","f64",false,"last value"},{"samples","i32",false,"samples"},{"dt","f64",false,"dt"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"}},{},{}},
            },{}},
            {{
                {{"array","frog.studio.math.statistics.histogram_express.array.owned.v1","Array"},{{"reset","bool",false,"Reset"},{"signal","array<f64>",true,"Signal"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in"},{"enable","bool",false,"Enable"},{"intervals","i32",false,"Bins"},{"auto_range","bool",false,"Automatic range"},{"minimum","f64",false,"Minimum"},{"maximum","f64",false,"Maximum"},{"bins","array<cluster<lower:f64,upper:f64,inclusion:u16>>",false,"Explicit bins"},{"inclusion","u16",false,"inclusion"},{"cumulative","bool",false,"cumulative"},{"normalization","u16",false,"normalization"}},{{"histogram","cluster<x:array<f64>,y:array<f64>>",false,"histogram"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"reset","bool","false"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"},{"enable","bool","true"},{"intervals","i32","10"},{"auto_range","bool","false"},{"minimum","f64","-3"},{"maximum","f64","3"},{"bins","array<cluster<lower:f64,upper:f64,inclusion:u16>>","[]"},{"inclusion","u16","0"},{"cumulative","bool","false"},{"normalization","u16","0"}},{},{}},
                {{"waveform","frog.studio.math.statistics.histogram_express.waveform.owned.v1","Waveform"},{{"reset","bool",false,"Reset"},{"signal","cluster<t0:timestamp,dt:f64,Y:array<f64>>",true,"Signal"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in"},{"enable","bool",false,"Enable"},{"intervals","i32",false,"Bins"},{"auto_range","bool",false,"Automatic range"},{"minimum","f64",false,"Minimum"},{"maximum","f64",false,"Maximum"},{"bins","array<cluster<lower:f64,upper:f64,inclusion:u16>>",false,"Explicit bins"},{"inclusion","u16",false,"inclusion"},{"cumulative","bool",false,"cumulative"},{"normalization","u16",false,"normalization"}},{{"histogram","cluster<x:array<f64>,y:array<f64>>",false,"histogram"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"reset","bool","false"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"},{"enable","bool","true"},{"intervals","i32","10"},{"auto_range","bool","false"},{"minimum","f64","-3"},{"maximum","f64","3"},{"bins","array<cluster<lower:f64,upper:f64,inclusion:u16>>","[]"},{"inclusion","u16","0"},{"cumulative","bool","false"},{"normalization","u16","0"}},{},{}},
                {{"waveforms","frog.studio.math.statistics.histogram_express.waveforms.owned.v1","Waveforms"},{{"reset","bool",false,"Reset"},{"signal","array<cluster<t0:timestamp,dt:f64,Y:array<f64>>>",true,"Signal"},{"error_in","cluster<status:bool,code:i32,source:string>",false,"error in"},{"enable","bool",false,"Enable"},{"intervals","i32",false,"Bins"},{"auto_range","bool",false,"Automatic range"},{"minimum","f64",false,"Minimum"},{"maximum","f64",false,"Maximum"},{"bins","array<cluster<lower:f64,upper:f64,inclusion:u16>>",false,"Explicit bins"},{"inclusion","u16",false,"inclusion"},{"cumulative","bool",false,"cumulative"},{"normalization","u16",false,"normalization"}},{{"histogram","cluster<x:array<f64>,y:array<f64>>",false,"histogram"},{"error_out","cluster<status:bool,code:i32,source:string>",false,"error out"}},{{"reset","bool","false"},{"error_in","cluster<status:bool,code:i32,source:string>","{\"status\":false,\"code\":0,\"source\":\"\"}"},{"enable","bool","true"},{"intervals","i32","10"},{"auto_range","bool","false"},{"minimum","f64","-3"},{"maximum","f64","3"},{"bins","array<cluster<lower:f64,upper:f64,inclusion:u16>>","[]"},{"inclusion","u16","0"},{"cumulative","bool","false"},{"normalization","u16","0"}},{},{}},
            },{}},
            {{
                {{"beta","frog.studio.math.statistics.probability.continuous_cdf.beta.owned.v1","Beta"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"cauchy","frog.studio.math.statistics.probability.continuous_cdf.cauchy.owned.v1","Cauchy"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"chi_squared","frog.studio.math.statistics.probability.continuous_cdf.chi_squared.owned.v1","Chi Squared"},{{"x","f64",true,"x"},{"k","i32",true,"k"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"noncentral_chi_squared","frog.studio.math.statistics.probability.continuous_cdf.noncentral_chi_squared.owned.v1","Chi Squared Non Central"},{{"x","f64",true,"x"},{"k","i32",true,"k"},{"d","f64",true,"d"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"exponential","frog.studio.math.statistics.probability.continuous_cdf.exponential.owned.v1","Exp"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"extreme_value","frog.studio.math.statistics.probability.continuous_cdf.extreme_value.owned.v1","Extreme Value"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"f","frog.studio.math.statistics.probability.continuous_cdf.f.owned.v1","F"},{{"x","f64",true,"x"},{"k1","i32",true,"k1"},{"k2","i32",true,"k2"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"gamma","frog.studio.math.statistics.probability.continuous_cdf.gamma.owned.v1","Gamma"},{{"x","f64",true,"x"},{"b","f64",true,"b"},{"c","f64",true,"c"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"laplace","frog.studio.math.statistics.probability.continuous_cdf.laplace.owned.v1","Laplace"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"logistic","frog.studio.math.statistics.probability.continuous_cdf.logistic.owned.v1","Logistic"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"lognormal","frog.studio.math.statistics.probability.continuous_cdf.lognormal.owned.v1","Lognormal"},{{"x","f64",true,"x"},{"b","f64",true,"b"},{"c","f64",true,"c"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"normal","frog.studio.math.statistics.probability.continuous_cdf.normal.owned.v1","Normal"},{{"x","f64",true,"x"},{"mean","f64",true,"mean"},{"std","f64",true,"std"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"pareto","frog.studio.math.statistics.probability.continuous_cdf.pareto.owned.v1","Pareto"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"c","f64",true,"c"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"rayleigh","frog.studio.math.statistics.probability.continuous_cdf.rayleigh.owned.v1","Rayleigh"},{{"x","f64",true,"x"},{"b","f64",true,"b"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"student_t","frog.studio.math.statistics.probability.continuous_cdf.student_t.owned.v1","Student T"},{{"x","f64",true,"x"},{"k","i32",true,"k degrees of freedom"}},{{"cdf","f64",false,"cdf(x)"},{"sf","f64",false,"1-cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"triangular","frog.studio.math.statistics.probability.continuous_cdf.triangular.owned.v1","Triangular"},{{"x","f64",true,"x"},{"xmin","f64",true,"xmin"},{"xmode","f64",false,"xmode"},{"xmax","f64",true,"xmax"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{{"xmode","f64","\"NaN\""}},{},{}},
                {{"uniform","frog.studio.math.statistics.probability.continuous_cdf.uniform.owned.v1","Uniform"},{{"x","f64",true,"x"},{"xmin","f64",true,"xmin"},{"xmax","f64",true,"xmax"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"weibull","frog.studio.math.statistics.probability.continuous_cdf.weibull.owned.v1","Weibull"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"beta","frog.studio.math.statistics.probability.continuous_inverse_cdf.beta.owned.v1","Beta"},{{"probability","f64",true,"cdf(x)"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"cauchy","frog.studio.math.statistics.probability.continuous_inverse_cdf.cauchy.owned.v1","Cauchy"},{{"probability","f64",true,"cdf(x)"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"chi_squared","frog.studio.math.statistics.probability.continuous_inverse_cdf.chi_squared.owned.v1","Chi Squared"},{{"probability","f64",true,"cdf(x)"},{"k","i32",true,"k"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"noncentral_chi_squared","frog.studio.math.statistics.probability.continuous_inverse_cdf.noncentral_chi_squared.owned.v1","Chi Squared Non Central"},{{"probability","f64",true,"cdf(x)"},{"k","i32",true,"k"},{"d","f64",true,"d"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"exponential","frog.studio.math.statistics.probability.continuous_inverse_cdf.exponential.owned.v1","Exp"},{{"probability","f64",true,"cdf(x)"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"extreme_value","frog.studio.math.statistics.probability.continuous_inverse_cdf.extreme_value.owned.v1","Extreme Value"},{{"probability","f64",true,"cdf(x)"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"f","frog.studio.math.statistics.probability.continuous_inverse_cdf.f.owned.v1","F"},{{"probability","f64",true,"cdf(x)"},{"k1","i32",true,"k1"},{"k2","i32",true,"k2"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"gamma","frog.studio.math.statistics.probability.continuous_inverse_cdf.gamma.owned.v1","Gamma"},{{"probability","f64",true,"cdf(x)"},{"b","f64",true,"b"},{"c","f64",true,"c"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"laplace","frog.studio.math.statistics.probability.continuous_inverse_cdf.laplace.owned.v1","Laplace"},{{"probability","f64",true,"cdf(x)"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"logistic","frog.studio.math.statistics.probability.continuous_inverse_cdf.logistic.owned.v1","Logistic"},{{"probability","f64",true,"cdf(x)"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"lognormal","frog.studio.math.statistics.probability.continuous_inverse_cdf.lognormal.owned.v1","Lognormal"},{{"probability","f64",true,"cdf(x)"},{"b","f64",true,"b"},{"c","f64",true,"c"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"normal","frog.studio.math.statistics.probability.continuous_inverse_cdf.normal.owned.v1","Normal"},{{"probability","f64",true,"cdf(x)"},{"mean","f64",true,"mean"},{"std","f64",true,"std"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"pareto","frog.studio.math.statistics.probability.continuous_inverse_cdf.pareto.owned.v1","Pareto"},{{"probability","f64",true,"cdf(x)"},{"a","f64",true,"a"},{"c","f64",true,"c"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"rayleigh","frog.studio.math.statistics.probability.continuous_inverse_cdf.rayleigh.owned.v1","Rayleigh"},{{"probability","f64",true,"cdf(x)"},{"b","f64",true,"b"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"student_t","frog.studio.math.statistics.probability.continuous_inverse_cdf.student_t.owned.v1","Student T"},{{"probability","f64",true,"cdf(x)"},{"k","i32",true,"k degrees of freedom"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"triangular","frog.studio.math.statistics.probability.continuous_inverse_cdf.triangular.owned.v1","Triangular"},{{"probability","f64",true,"cdf(x)"},{"xmin","f64",true,"xmin"},{"xmode","f64",false,"xmode"},{"xmax","f64",true,"xmax"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{{"xmode","f64","\"NaN\""}},{},{}},
                {{"uniform","frog.studio.math.statistics.probability.continuous_inverse_cdf.uniform.owned.v1","Uniform"},{{"probability","f64",true,"cdf(x)"},{"xmin","f64",true,"xmin"},{"xmax","f64",true,"xmax"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"weibull","frog.studio.math.statistics.probability.continuous_inverse_cdf.weibull.owned.v1","Weibull"},{{"probability","f64",true,"cdf(x)"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"quantile","f64",false,"x"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"beta","frog.studio.math.statistics.probability.continuous_pdf.beta.owned.v1","Beta"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"cauchy","frog.studio.math.statistics.probability.continuous_pdf.cauchy.owned.v1","Cauchy"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"chi_squared","frog.studio.math.statistics.probability.continuous_pdf.chi_squared.owned.v1","Chi Squared"},{{"x","f64",true,"x"},{"k","i32",true,"k"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"noncentral_chi_squared","frog.studio.math.statistics.probability.continuous_pdf.noncentral_chi_squared.owned.v1","Chi Squared Non Central"},{{"x","f64",true,"x"},{"k","i32",true,"k"},{"d","f64",true,"d"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"exponential","frog.studio.math.statistics.probability.continuous_pdf.exponential.owned.v1","Exp"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"extreme_value","frog.studio.math.statistics.probability.continuous_pdf.extreme_value.owned.v1","Extreme Value"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"f","frog.studio.math.statistics.probability.continuous_pdf.f.owned.v1","F"},{{"x","f64",true,"x"},{"k1","i32",true,"k1"},{"k2","i32",true,"k2"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"gamma","frog.studio.math.statistics.probability.continuous_pdf.gamma.owned.v1","Gamma"},{{"x","f64",true,"x"},{"b","f64",true,"b"},{"c","f64",true,"c"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"laplace","frog.studio.math.statistics.probability.continuous_pdf.laplace.owned.v1","Laplace"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"logistic","frog.studio.math.statistics.probability.continuous_pdf.logistic.owned.v1","Logistic"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"lognormal","frog.studio.math.statistics.probability.continuous_pdf.lognormal.owned.v1","Lognormal"},{{"x","f64",true,"x"},{"b","f64",true,"b"},{"c","f64",true,"c"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"normal","frog.studio.math.statistics.probability.continuous_pdf.normal.owned.v1","Normal"},{{"x","f64",true,"x"},{"mean","f64",true,"mean"},{"std","f64",true,"std"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"pareto","frog.studio.math.statistics.probability.continuous_pdf.pareto.owned.v1","Pareto"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"c","f64",true,"c"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"rayleigh","frog.studio.math.statistics.probability.continuous_pdf.rayleigh.owned.v1","Rayleigh"},{{"x","f64",true,"x"},{"b","f64",true,"b"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"student_t","frog.studio.math.statistics.probability.continuous_pdf.student_t.owned.v1","Student T"},{{"x","f64",true,"x"},{"k","i32",true,"k degrees of freedom"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"triangular","frog.studio.math.statistics.probability.continuous_pdf.triangular.owned.v1","Triangular"},{{"x","f64",true,"x"},{"xmin","f64",true,"xmin"},{"xmode","f64",false,"xmode"},{"xmax","f64",true,"xmax"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{{"xmode","f64","\"NaN\""}},{},{}},
                {{"uniform","frog.studio.math.statistics.probability.continuous_pdf.uniform.owned.v1","Uniform"},{{"x","f64",true,"x"},{"xmin","f64",true,"xmin"},{"xmax","f64",true,"xmax"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"weibull","frog.studio.math.statistics.probability.continuous_pdf.weibull.owned.v1","Weibull"},{{"x","f64",true,"x"},{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"pdf","f64",false,"pdf(x)"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"beta","frog.studio.math.statistics.probability.continuous_random.beta.owned.v1","Beta"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"a","f64",true,"a"},{"b","f64",true,"b"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"beta random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"cauchy","frog.studio.math.statistics.probability.continuous_random.cauchy.owned.v1","Cauchy"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"a","f64",true,"a"},{"b","f64",true,"b"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"Cauchy random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"chi_squared","frog.studio.math.statistics.probability.continuous_random.chi_squared.owned.v1","Chi Squared"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"k","i32",true,"k"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"chi-squared random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"noncentral_chi_squared","frog.studio.math.statistics.probability.continuous_random.noncentral_chi_squared.owned.v1","Chi Squared Non Central"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"k","i32",true,"k"},{"d","f64",true,"d"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"chi-squared(nc) random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"exponential","frog.studio.math.statistics.probability.continuous_random.exponential.owned.v1","Exp"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"a","f64",true,"a"},{"b","f64",true,"b"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"exp random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"extreme_value","frog.studio.math.statistics.probability.continuous_random.extreme_value.owned.v1","Extreme Value"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"a","f64",true,"a"},{"b","f64",true,"b"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"extreme value random numbers"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"f","frog.studio.math.statistics.probability.continuous_random.f.owned.v1","F"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"k1","i32",true,"k1"},{"k2","i32",true,"k2"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"f random numbers"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"gamma","frog.studio.math.statistics.probability.continuous_random.gamma.owned.v1","Gamma"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"b","f64",true,"b"},{"c","f64",true,"c"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"gamma random numbers"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"laplace","frog.studio.math.statistics.probability.continuous_random.laplace.owned.v1","Laplace"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"a","f64",true,"a"},{"b","f64",true,"b"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"Laplace random numbers"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"logistic","frog.studio.math.statistics.probability.continuous_random.logistic.owned.v1","Logistic"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"a","f64",true,"a"},{"b","f64",true,"b"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"logistic random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"lognormal","frog.studio.math.statistics.probability.continuous_random.lognormal.owned.v1","Lognormal"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"b","f64",true,"b"},{"c","f64",true,"c"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"Lognormal random numbers"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"normal","frog.studio.math.statistics.probability.continuous_random.normal.owned.v1","Normal"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"mean","f64",true,"mean"},{"std","f64",true,"std"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"normal random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"pareto","frog.studio.math.statistics.probability.continuous_random.pareto.owned.v1","Pareto"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"a","f64",true,"a"},{"c","f64",true,"c"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"Pareto random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"rayleigh","frog.studio.math.statistics.probability.continuous_random.rayleigh.owned.v1","Rayleigh"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"b","f64",true,"b"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"Rayleigh random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"student_t","frog.studio.math.statistics.probability.continuous_random.student_t.owned.v1","Student T"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"k","i32",true,"k degrees of freedom"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"Student t random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"triangular","frog.studio.math.statistics.probability.continuous_random.triangular.owned.v1","Triangular"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"xmin","f64",true,"xmin"},{"xmode","f64",false,"xmode"},{"xmax","f64",true,"xmax"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"triangular random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"xmode","f64","\"NaN\""},{"seed","i32","-1"}},{},{}},
                {{"uniform","frog.studio.math.statistics.probability.continuous_random.uniform.owned.v1","Uniform"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"xmin","f64",true,"xmin"},{"xmax","f64",true,"xmax"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"uniform random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"weibull","frog.studio.math.statistics.probability.continuous_random.weibull.owned.v1","Weibull"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"a","f64",true,"a"},{"b","f64",true,"b"},{"seed","i32",false,"seed"}},{{"data","array<f64>",false,"Weibull random numbers"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
            },{}},
            {{
                {{"beta","frog.studio.math.statistics.probability.continuous_moments.beta.owned.v1","Beta"},{{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"chi_squared","frog.studio.math.statistics.probability.continuous_moments.chi_squared.owned.v1","Chi Squared"},{{"k","i32",true,"k"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"noncentral_chi_squared","frog.studio.math.statistics.probability.continuous_moments.noncentral_chi_squared.owned.v1","Chi Squared Non Central"},{{"k","i32",true,"k"},{"d","f64",true,"d"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"exponential","frog.studio.math.statistics.probability.continuous_moments.exponential.owned.v1","Exp"},{{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"extreme_value","frog.studio.math.statistics.probability.continuous_moments.extreme_value.owned.v1","Extreme Value"},{{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"f","frog.studio.math.statistics.probability.continuous_moments.f.owned.v1","F"},{{"k1","i32",true,"k1"},{"k2","i32",true,"k2"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"gamma","frog.studio.math.statistics.probability.continuous_moments.gamma.owned.v1","Gamma"},{{"b","f64",true,"b"},{"c","f64",true,"c"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"laplace","frog.studio.math.statistics.probability.continuous_moments.laplace.owned.v1","Laplace"},{{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"logistic","frog.studio.math.statistics.probability.continuous_moments.logistic.owned.v1","Logistic"},{{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"lognormal","frog.studio.math.statistics.probability.continuous_moments.lognormal.owned.v1","Lognormal"},{{"b","f64",true,"b"},{"c","f64",true,"c"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"normal","frog.studio.math.statistics.probability.continuous_moments.normal.owned.v1","Normal"},{{"location","f64",true,"mean"},{"std","f64",true,"std"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"pareto","frog.studio.math.statistics.probability.continuous_moments.pareto.owned.v1","Pareto"},{{"a","f64",true,"a"},{"c","f64",true,"c"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"rayleigh","frog.studio.math.statistics.probability.continuous_moments.rayleigh.owned.v1","Rayleigh"},{{"b","f64",true,"b"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"student_t","frog.studio.math.statistics.probability.continuous_moments.student_t.owned.v1","Student T"},{{"k","i32",true,"k degrees of freedom"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"triangular","frog.studio.math.statistics.probability.continuous_moments.triangular.owned.v1","Triangular"},{{"xmin","f64",true,"xmin"},{"xmode","f64",false,"xmode"},{"xmax","f64",true,"xmax"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{{"xmode","f64","\"NaN\""}},{},{}},
                {{"uniform","frog.studio.math.statistics.probability.continuous_moments.uniform.owned.v1","Uniform"},{{"xmin","f64",true,"xmin"},{"xmax","f64",true,"xmax"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"weibull","frog.studio.math.statistics.probability.continuous_moments.weibull.owned.v1","Weibull"},{{"a","f64",true,"a"},{"b","f64",true,"b"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"bernoulli","frog.studio.math.statistics.probability.discrete_cdf.bernoulli.owned.v1","Bernoulli"},{{"x","f64",true,"x"},{"p","f64",true,"p"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"binomial","frog.studio.math.statistics.probability.discrete_cdf.binomial.owned.v1","Binomial"},{{"x","f64",true,"x"},{"n","i32",true,"n"},{"p","f64",true,"p"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"geometric","frog.studio.math.statistics.probability.discrete_cdf.geometric.owned.v1","Geometric"},{{"x","f64",true,"n"},{"p","f64",true,"p"}},{{"cdf","f64",false,"cdf(n)"},{"error","i32",false,"error"}},{},{},{}},
                {{"hypergeometric","frog.studio.math.statistics.probability.discrete_cdf.hypergeometric.owned.v1","Hypergeometric"},{{"x","f64",true,"x"},{"m","i32",true,"M"},{"k","i32",true,"k"},{"n","i32",true,"n"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"negative_binomial","frog.studio.math.statistics.probability.discrete_cdf.negative_binomial.owned.v1","Neg Binomial"},{{"x","f64",true,"y"},{"successes","f64",true,"x"},{"p","f64",true,"p"}},{{"cdf","f64",false,"cdf(y)"},{"error","i32",false,"error"}},{},{},{}},
                {{"poisson","frog.studio.math.statistics.probability.discrete_cdf.poisson.owned.v1","Poisson"},{{"x","f64",true,"x"},{"lambda","f64",true,"lambda"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"uniform_discrete","frog.studio.math.statistics.probability.discrete_cdf.uniform_discrete.owned.v1","Uniform Discrete"},{{"x","f64",true,"x"},{"n","i32",true,"n"}},{{"cdf","f64",false,"cdf(x)"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"bernoulli","frog.studio.math.statistics.probability.discrete_inverse_cdf.bernoulli.owned.v1","Bernoulli"},{{"probability","f64",true,"cdf(x)"},{"p","f64",true,"p"}},{{"quantile","i32",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"binomial","frog.studio.math.statistics.probability.discrete_inverse_cdf.binomial.owned.v1","Binomial"},{{"probability","f64",true,"cdf(x)"},{"n","i32",true,"n"},{"p","f64",true,"p"}},{{"quantile","i32",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"geometric","frog.studio.math.statistics.probability.discrete_inverse_cdf.geometric.owned.v1","Geometric"},{{"probability","f64",true,"cdf(n)"},{"p","f64",true,"p"}},{{"quantile","i32",false,"n"},{"error","i32",false,"error"}},{},{},{}},
                {{"hypergeometric","frog.studio.math.statistics.probability.discrete_inverse_cdf.hypergeometric.owned.v1","Hypergeometric"},{{"probability","f64",true,"cdf(x)"},{"m","i32",true,"M"},{"k","i32",true,"k"},{"n","i32",true,"n"}},{{"quantile","i32",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"negative_binomial","frog.studio.math.statistics.probability.discrete_inverse_cdf.negative_binomial.owned.v1","Neg Binomial"},{{"probability","f64",true,"cdf(y)"},{"successes","f64",true,"x"},{"p","f64",true,"p"}},{{"quantile","i32",false,"y"},{"error","i32",false,"error"}},{},{},{}},
                {{"poisson","frog.studio.math.statistics.probability.discrete_inverse_cdf.poisson.owned.v1","Poisson"},{{"probability","f64",true,"cdf(x)"},{"lambda","f64",true,"lambda"}},{{"quantile","i32",false,"x"},{"error","i32",false,"error"}},{},{},{}},
                {{"uniform_discrete","frog.studio.math.statistics.probability.discrete_inverse_cdf.uniform_discrete.owned.v1","Uniform Discrete"},{{"probability","f64",true,"cdf(x)"},{"n","i32",true,"n"}},{{"quantile","i32",false,"x"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"bernoulli","frog.studio.math.statistics.probability.discrete_pf.bernoulli.owned.v1","Bernoulli"},{{"x","i32",true,"x"},{"p","f64",true,"p"}},{{"pf","f64",false,"prob(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"binomial","frog.studio.math.statistics.probability.discrete_pf.binomial.owned.v1","Binomial"},{{"x","i32",true,"x"},{"n","i32",true,"n"},{"p","f64",true,"p"}},{{"pf","f64",false,"prob(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"geometric","frog.studio.math.statistics.probability.discrete_pf.geometric.owned.v1","Geometric"},{{"x","i32",true,"x"},{"p","f64",true,"p"}},{{"pf","f64",false,"prob(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"hypergeometric","frog.studio.math.statistics.probability.discrete_pf.hypergeometric.owned.v1","Hypergeometric"},{{"x","i32",true,"x"},{"m","i32",true,"M"},{"k","i32",true,"k"},{"n","i32",true,"n"}},{{"pf","f64",false,"prob(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"negative_binomial","frog.studio.math.statistics.probability.discrete_pf.negative_binomial.owned.v1","Neg Binomial"},{{"x","i32",true,"y"},{"successes","f64",true,"x"},{"p","f64",true,"p"}},{{"pf","f64",false,"prob(y)"},{"error","i32",false,"error"}},{},{},{}},
                {{"poisson","frog.studio.math.statistics.probability.discrete_pf.poisson.owned.v1","Poisson"},{{"x","i32",true,"x"},{"lambda","f64",true,"lambda"}},{{"pf","f64",false,"prob(x)"},{"error","i32",false,"error"}},{},{},{}},
                {{"uniform_discrete","frog.studio.math.statistics.probability.discrete_pf.uniform_discrete.owned.v1","Uniform Discrete"},{{"x","i32",true,"x"},{"n","i32",true,"n"}},{{"pf","f64",false,"prob(x)"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"bernoulli","frog.studio.math.statistics.probability.discrete_random.bernoulli.owned.v1","Bernoulli"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"p","f64",true,"p"},{"seed","i32",false,"seed"}},{{"data","array<u32>",false,"Bernoulli random numbers"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"binomial","frog.studio.math.statistics.probability.discrete_random.binomial.owned.v1","Binomial"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"n","i32",true,"n"},{"p","f64",true,"p"},{"seed","i32",false,"seed"}},{{"data","array<u32>",false,"binomial random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"geometric","frog.studio.math.statistics.probability.discrete_random.geometric.owned.v1","Geometric"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"p","f64",true,"p"},{"seed","i32",false,"seed"}},{{"data","array<i32>",false,"geometric random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"hypergeometric","frog.studio.math.statistics.probability.discrete_random.hypergeometric.owned.v1","Hypergeometric"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"m","i32",true,"M"},{"k","i32",true,"k"},{"n","i32",true,"n"},{"seed","i32",false,"seed"}},{{"data","array<i32>",false,"hypergeometric random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"negative_binomial","frog.studio.math.statistics.probability.discrete_random.negative_binomial.owned.v1","Neg Binomial"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"successes","f64",true,"x"},{"p","f64",true,"p"},{"seed","i32",false,"seed"}},{{"data","array<i32>",false,"neg binomial random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"poisson","frog.studio.math.statistics.probability.discrete_random.poisson.owned.v1","Poisson"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"lambda","f64",true,"lambda"},{"seed","i32",false,"seed"}},{{"data","array<i32>",false,"Poisson random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"uniform_discrete","frog.studio.math.statistics.probability.discrete_random.uniform_discrete.owned.v1","Uniform Discrete"},{{"reset","bool",false,"reset"},{"samples","i32",false,"samples"},{"n","i32",true,"n"},{"seed","i32",false,"seed"}},{{"data","array<i32>",false,"uniform (discrete) random data"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"samples","i32","128"},{"seed","i32","-1"}},{},{}},
                {{"permutation","frog.studio.math.statistics.probability.discrete_random.permutation.owned.v1","Permutation"},{{"reset","bool",false,"reset"},{"n","u32",true,"N"},{"start","i32",false,"start"},{"seed","i32",false,"seed"}},{{"data","array<i32>",false,"random permutation"},{"error","i32",false,"error"}},{{"reset","bool","false"},{"start","i32","1"},{"seed","i32","-1"}},{},{}},
            },{}},
            {{
                {{"bernoulli","frog.studio.math.statistics.probability.discrete_moments.bernoulli.owned.v1","Bernoulli"},{{"p","f64",true,"p"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"binomial","frog.studio.math.statistics.probability.discrete_moments.binomial.owned.v1","Binomial"},{{"n","i32",true,"n"},{"p","f64",true,"p"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"geometric","frog.studio.math.statistics.probability.discrete_moments.geometric.owned.v1","Geometric"},{{"p","f64",true,"p"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"hypergeometric","frog.studio.math.statistics.probability.discrete_moments.hypergeometric.owned.v1","Hypergeometric"},{{"m","i32",true,"M"},{"k","i32",true,"k"},{"n","i32",true,"n"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"negative_binomial","frog.studio.math.statistics.probability.discrete_moments.negative_binomial.owned.v1","Neg Binomial"},{{"successes","f64",true,"x"},{"p","f64",true,"p"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"poisson","frog.studio.math.statistics.probability.discrete_moments.poisson.owned.v1","Poisson"},{{"lambda","f64",true,"lambda"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
                {{"uniform_discrete","frog.studio.math.statistics.probability.discrete_moments.uniform_discrete.owned.v1","Uniform Discrete"},{{"n","i32",true,"n"}},{{"mean","f64",false,"mean"},{"variance","f64",false,"variance"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.hypothesis.t_test.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"mean0","f64",false,"mean"},{"alpha","f64",false,"significance level"},{"alternative","i32",false,"alternative hypothesis"}},{{"reject","bool",false,"reject null hypothesis"},{"p_value","f64",false,"p value"},{"confidence_interval","cluster<low:f64,high:f64>",false,"confidence interval"},{"error","i32",false,"error"},{"test_info","cluster<sample_mean:f64,sample_std:f64,sample_se_mean:f64,sample_t_value:f64,t_value:f64>",false,"test info"}},{{"mean0","f64","0"},{"alpha","f64","0.05"},{"alternative","i32","0"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.hypothesis.z_test.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"mean0","f64",false,"mean"},{"alpha","f64",false,"significance level"},{"alternative","i32",false,"alternative hypothesis"},{"std0","f64",true,"std"}},{{"reject","bool",false,"reject null hypothesis"},{"p_value","f64",false,"p value"},{"confidence_interval","cluster<low:f64,high:f64>",false,"confidence interval"},{"error","i32",false,"error"},{"test_info","cluster<sample_mean:f64,sample_std:f64,sample_se_mean:f64,sample_z_value:f64,z_value:f64>",false,"test info"}},{{"mean0","f64","0"},{"alpha","f64","0.05"},{"alternative","i32","0"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.hypothesis.correlation_test.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"y","array<f64>",true,"Y"},{"type","u16",false,"type"},{"alpha","f64",false,"significance level"},{"alternative","i32",false,"alternative hypothesis"}},{{"reject","bool",false,"reject H0?"},{"p_value","f64",false,"p value"},{"critical","f64",false,"rsig"},{"r","f64",false,"r"},{"error","i32",false,"error"}},{{"type","u16","0"},{"alpha","f64","0.05"},{"alternative","i32","0"}},{},{}},
            },{}},
            {{
                {{"median","frog.studio.math.statistics.hypothesis.sign_test.median.owned.v1","Median"},{{"x","array<f64>",true,"X"},{"median0","f64",false,"median"},{"alpha","f64",false,"significance level"},{"alternative","i32",false,"alternative hypothesis"}},{{"reject","bool",false,"reject null hypothesis"},{"p_value","f64",false,"p value"},{"error","i32",false,"error"}},{{"median0","f64","0"},{"alpha","f64","0.05"},{"alternative","i32","0"}},{},{}},
                {{"paired","frog.studio.math.statistics.hypothesis.sign_test.paired.owned.v1","Paired"},{{"x","array<f64>",true,"X"},{"y","array<f64>",true,"Y"},{"alpha","f64",false,"significance level"},{"alternative","i32",false,"alternative hypothesis"}},{{"reject","bool",false,"reject null hypothesis"},{"p_value","f64",false,"p value"},{"error","i32",false,"error"}},{{"alpha","f64","0.05"},{"alternative","i32","0"}},{},{}},
            },{}},
            {{
                {{"median","frog.studio.math.statistics.hypothesis.wilcoxon.median.owned.v1","Median"},{{"x","array<f64>",true,"X"},{"mean0","f64",false,"mean"},{"alpha","f64",false,"significance level"},{"alternative","i32",false,"alternative hypothesis"}},{{"reject","bool",false,"reject null hypothesis"},{"p_value","f64",false,"p value"},{"error","i32",false,"error"}},{{"mean0","f64","0"},{"alpha","f64","0.05"},{"alternative","i32","0"}},{},{}},
                {{"paired","frog.studio.math.statistics.hypothesis.wilcoxon.paired.owned.v1","Paired"},{{"x","array<f64>",true,"X"},{"y","array<f64>",true,"Y"},{"alpha","f64",false,"significance level"},{"alternative","i32",false,"alternative hypothesis"}},{{"reject","bool",false,"reject null hypothesis"},{"p_value","f64",false,"p value"},{"error","i32",false,"error"}},{{"alpha","f64","0.05"},{"alternative","i32","0"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.hypothesis.rank_transformation.owned.v1","Default"},{{"x","array<f64>",true,"X"}},{{"ranks","array<f64>",false,"rank order of X"},{"t_sum","f64",false,"measure of ties"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.hypothesis.contingency_table.owned.v1","Default"},{{"x","array<i32,rank=2>",true,"table"}},{{"chi_square","f64",false,"X"},{"p_value","f64",false,"probability"},{"error","i32",false,"error"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.anova.anova_1d.owned.v1","Default"},{{"x","array<f64>",true,"X"},{"index_a","array<i32>",true,"Index"},{"levels_a","i32",true,"# of levels"}},{{"f","f64",false,"f"},{"ssa","f64",false,"ssa"},{"sse","f64",false,"sse"},{"mse","f64",false,"mse"},{"msa","f64",false,"msa"},{"tss","f64",false,"tss"},{"error","i32",false,"error"},{"sig_a","f64",false,"sig a"}},{},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.anova.anova_2d.owned.v1","Default"},{{"levels_a","i32",true,"A levels"},{"x","array<f64>",true,"X"},{"index_a","array<i32>",true,"Index A"},{"index_b","array<i32>",true,"Index B"},{"observations_per_cell","i32",false,"observations per cell"},{"levels_b","i32",true,"B levels"}},{{"info","array<f64,rank=2>",false,"info"},{"sig_a","f64",false,"sig a"},{"sig_b","f64",false,"sig b"},{"sig_ab","f64",false,"sig ab"},{"error","i32",false,"error"}},{{"observations_per_cell","i32","1"}},{},{}},
            },{}},
            {{
                {{"default","frog.studio.math.statistics.anova.anova_3d.owned.v1","Default"},{{"levels","cluster<level_a:i32,level_b:i32,level_c:i32>",true,"Levels"},{"x","array<f64>",true,"X"},{"index_a","array<i32>",true,"Index A"},{"index_b","array<i32>",true,"Index B"},{"index_c","array<i32>",true,"Index C"},{"observations_per_cell","i32",true,"observations per cell"}},{{"info","array<f64,rank=2>",false,"info"},{"significance","cluster<sig_a:f64,sig_b:f64,sig_c:f64,sig_ab:f64,sig_ac:f64,sig_bc:f64,sig_abc:f64>",false,"significance"},{"error","i32",false,"error"}},{},{},{}},
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
struct Match { std::size_t function=39,variant=0;bool explicitVariant=false; };
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
StatisticsTypeResult failure(std::string_view code,std::string_view message) {
    return {{ResolutionStatus::Incompatible,{},std::string(code),std::string(message)},false};
}
bool realNumeric(const TypeRef &type) {
    return type.kind==TypeKind::Primitive&&type.numeric()&&!type.name.starts_with("c")&&type.name!="f128";
}
bool acceptsScalar(const TypeRef &source,const TypeRef &target) {
    const auto &scalar=source.kind==TypeKind::Enum&&source.children.size()==1?source.children.front():source;
    if(target.kind!=TypeKind::Primitive||!target.numeric()||scalar.kind!=TypeKind::Primitive||!scalar.numeric())return false;
    // A complex consumer may explicitly widen a real/complex physical value;
    // a real consumer never erases even a zero-valued imaginary component.
    return target.name.starts_with("c")?scalar.name!="f128":realNumeric(scalar);
}
bool accepts(const TypeRef &source,const TypeRef &target) {
    if(source==target)return true; // presentation names do not replace IDs
    if(acceptsScalar(source,target))return true;
    if(source.kind==TypeKind::Array&&target.kind==TypeKind::Array&&source.rank==target.rank&&source.children.size()==1&&target.children.size()==1)
        return source.children.front().kind==TypeKind::Primitive&&acceptsScalar(source.children.front(),target.children.front());
    // Records, signals, nominal identities, fields and ranks stay exact.
    return false;
}
} // namespace

std::span<const StatisticsFunctionDescriptor> statisticsFunctions() noexcept { return descriptions; }
const StatisticsFunctionDescriptor *findStatisticsFunction(std::string_view id) noexcept {
    const auto found=match(id);return found.function==descriptions.size()?nullptr:&descriptions[found.function];
}
bool isStatisticsFunction(std::string_view id) noexcept { return findStatisticsFunction(id)!=nullptr; }
bool isStatisticsExpressFunction(std::string_view id) noexcept {const auto *f=findStatisticsFunction(id);return f&&f->catalogId=="math.statistics.statistics_express";}
std::span<const StatisticsVariantDescriptor> statisticsVariants(std::string_view id) noexcept {
    const auto found=match(id);return found.function==descriptions.size()?std::span<const StatisticsVariantDescriptor>{}:std::span<const StatisticsVariantDescriptor>{laws()[found.function].variantDescriptors};
}
std::string_view statisticsOperationId(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?law->descriptor.operationId:std::string_view{};
}
std::span<const std::string_view> statisticsInputPortIds(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const std::string_view>{law->inputIds}:std::span<const std::string_view>{};
}
std::span<const std::string_view> statisticsOutputPortIds(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const std::string_view>{law->outputIds}:std::span<const std::string_view>{};
}
std::span<const StatisticsInputDefault> statisticsInputDefaults(std::string_view id,std::string_view variant) noexcept {
    const auto *law=select(id,variant);return law?std::span<const StatisticsInputDefault>{law->defaults}:std::span<const StatisticsInputDefault>{};
}
bool statisticsInputRequired(std::string_view id,std::string_view port,std::string_view variant) noexcept {
    const auto *law=select(id,variant);if(!law)return false;
    for(const auto &input:law->inputs)if(input.id==port)return input.required;
    return false;
}
std::string_view statisticsPortDisplayName(std::string_view id,std::string_view port,std::string_view variant) noexcept {
    const auto *law=select(id,variant);if(!law)return {};
    for(const auto &input:law->inputs)if(input.id==port)return input.displayName;
    for(const auto &output:law->outputs)if(output.id==port)return output.displayName;
    return {};
}
StatisticsTypeResult resolveStatisticsPort(std::string_view id,std::string_view port,std::span<const StatisticsTypeEvidence> inputs,std::string_view variant) {
    if(!isStatisticsFunction(id))return failure("math.statistics.unknown_function","The Statistics function is not registered.");
    const auto *law=select(id,variant);
    if(!law)return failure("math.statistics.unknown_variant","The Statistics variant is unknown or contradicts the native operation identity.");
    const PortLaw *requested=nullptr;bool requestedInput=false;
    for(const auto &p:law->inputs)if(p.id==port){requested=&p;requestedInput=true;}
    for(const auto &p:law->outputs)if(p.id==port)requested=&p;
    if(!requested)return failure("math.statistics.missing_port","The selected Statistics variant has no such port.");
    std::set<std::string_view> seen;bool complete=true;
    const auto evidence=[&](std::string_view name)->const TypeRef* {
        for(const auto &e:inputs)if(e.portId==name)return &e.valueType;return nullptr;
    };
    for(const auto &e:inputs) {
        if(!seen.insert(e.portId).second)return failure("math.statistics.multiple_sources","A Statistics input has more than one source.");
        const auto found=std::find_if(law->inputs.begin(),law->inputs.end(),[&](const auto &p){return p.id==e.portId;});
        if(found==law->inputs.end())return failure("math.statistics.unexpected_input","The selected Statistics variant has no input with this identity.");
        if(!e.valueType.known()){complete=false;continue;}
        const auto expected=binding::parseType(found->type);
        if(!expected.valid())return failure("math.statistics.invalid_contract","The Statistics port contract is not a recognized public type.");
        if(!accepts(e.valueType,expected.type))return failure("math.statistics.input_type","The Statistics input requires its selected numeric representation/rank or exact structured schema; real consumers never erase complex values, nominal fields or signal timing.");
    }
    for(const auto &p:law->inputs)if(p.required){const auto *t=evidence(p.id);if(!t||!t->known())complete=false;}
    const auto parsed=binding::parseType(requested->type);
    if(!parsed.valid())return failure("math.statistics.invalid_contract","The Statistics port contract is not a recognized public type.");
    auto type=parsed.type;const auto *source=requestedInput?evidence(port):nullptr;
    if(source&&source->known()&&*source!=type)
        return {{ResolutionStatus::Coercion,std::move(type),"math.statistics.numeric_promotion","Statistics applies the selected explicit numeric conversion; large integer-to-floating promotions may round."},complete};
    return {{ResolutionStatus::Compatible,std::move(type),{},{}},complete};
}
} // namespace frog::native_engine
