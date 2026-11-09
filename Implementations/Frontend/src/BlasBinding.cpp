#include "frog_native_engine/BlasBinding.hpp"
#include <algorithm>
#include <array>
#include <set>
#include <vector>

namespace frog::native_engine {
namespace {
using binding::TypeKind;
using binding::TypeRef;
using binding::ResolutionStatus;
struct PortLaw { std::string_view id, kind; bool required; };
struct FunctionLaw {
    std::vector<PortLaw> inputs, outputs;
    std::vector<std::string_view> inputIds, outputIds;
    std::vector<BlasInputDefault> defaults;
};
constexpr std::array<BlasFunctionDescriptor,35> descriptions{{
    {"math.linear_algebra.blas.ddot","frog.studio.math.linear_algebra.blas.ddot.owned.v1","ddot"},
    {"math.linear_algebra.blas.zdotu","frog.studio.math.linear_algebra.blas.zdotu.owned.v1","zdotu"},
    {"math.linear_algebra.blas.zdotc","frog.studio.math.linear_algebra.blas.zdotc.owned.v1","zdotc"},
    {"math.linear_algebra.blas.axpy","frog.studio.math.linear_algebra.blas.axpy.owned.v1","axpy"},
    {"math.linear_algebra.blas.nrm2","frog.studio.math.linear_algebra.blas.nrm2.owned.v1","nrm2"},
    {"math.linear_algebra.blas.asum","frog.studio.math.linear_algebra.blas.asum.owned.v1","asum"},
    {"math.linear_algebra.blas.copy","frog.studio.math.linear_algebra.blas.copy.owned.v1","copy"},
    {"math.linear_algebra.blas.swap","frog.studio.math.linear_algebra.blas.swap.owned.v1","swap"},
    {"math.linear_algebra.blas.drotm","frog.studio.math.linear_algebra.blas.drotm.owned.v1","drotm"},
    {"math.linear_algebra.blas.drotmg","frog.studio.math.linear_algebra.blas.drotmg.owned.v1","drotmg"},
    {"math.linear_algebra.blas.rot","frog.studio.math.linear_algebra.blas.rot.owned.v1","rot"},
    {"math.linear_algebra.blas.rotg","frog.studio.math.linear_algebra.blas.rotg.owned.v1","rotg"},
    {"math.linear_algebra.blas.iamax","frog.studio.math.linear_algebra.blas.iamax.owned.v1","iamax"},
    {"math.linear_algebra.blas.iamin","frog.studio.math.linear_algebra.blas.iamin.owned.v1","iamin"},
    {"math.linear_algebra.blas.gemv","frog.studio.math.linear_algebra.blas.gemv.owned.v1","gemv"},
    {"math.linear_algebra.blas.dsymv","frog.studio.math.linear_algebra.blas.dsymv.owned.v1","dsymv"},
    {"math.linear_algebra.blas.zhemv","frog.studio.math.linear_algebra.blas.zhemv.owned.v1","zhemv"},
    {"math.linear_algebra.blas.trmv","frog.studio.math.linear_algebra.blas.trmv.owned.v1","trmv"},
    {"math.linear_algebra.blas.dsyr","frog.studio.math.linear_algebra.blas.dsyr.owned.v1","dsyr"},
    {"math.linear_algebra.blas.dsyr2","frog.studio.math.linear_algebra.blas.dsyr2.owned.v1","dsyr2"},
    {"math.linear_algebra.blas.zher","frog.studio.math.linear_algebra.blas.zher.owned.v1","zher"},
    {"math.linear_algebra.blas.zher2","frog.studio.math.linear_algebra.blas.zher2.owned.v1","zher2"},
    {"math.linear_algebra.blas.dger","frog.studio.math.linear_algebra.blas.dger.owned.v1","dger"},
    {"math.linear_algebra.blas.zgeru","frog.studio.math.linear_algebra.blas.zgeru.owned.v1","zgeru"},
    {"math.linear_algebra.blas.zgerc","frog.studio.math.linear_algebra.blas.zgerc.owned.v1","zgerc"},
    {"math.linear_algebra.blas.trsv","frog.studio.math.linear_algebra.blas.trsv.owned.v1","trsv"},
    {"math.linear_algebra.blas.gemm","frog.studio.math.linear_algebra.blas.gemm.owned.v1","gemm"},
    {"math.linear_algebra.blas.symm","frog.studio.math.linear_algebra.blas.symm.owned.v1","symm"},
    {"math.linear_algebra.blas.zhemm","frog.studio.math.linear_algebra.blas.zhemm.owned.v1","zhemm"},
    {"math.linear_algebra.blas.trmm","frog.studio.math.linear_algebra.blas.trmm.owned.v1","trmm"},
    {"math.linear_algebra.blas.syrk","frog.studio.math.linear_algebra.blas.syrk.owned.v1","syrk"},
    {"math.linear_algebra.blas.syr2k","frog.studio.math.linear_algebra.blas.syr2k.owned.v1","syr2k"},
    {"math.linear_algebra.blas.zherk","frog.studio.math.linear_algebra.blas.zherk.owned.v1","zherk"},
    {"math.linear_algebra.blas.zher2k","frog.studio.math.linear_algebra.blas.zher2k.owned.v1","zher2k"},
    {"math.linear_algebra.blas.trsm","frog.studio.math.linear_algebra.blas.trsm.owned.v1","trsm"},
}};
constexpr std::array<std::string_view,35> domains{{"f64","c64","c64","t","t","t","t","t","f64","f64","t","t","t","t","t","f64","c64","t","f64","f64","c64","c64","f64","c64","c64","t","t","t","c64","t","t","t","c64","c64","t"}};
const std::array<FunctionLaw,35> &laws() {
    static const auto value=[] {
        std::array<FunctionLaw,35> result{{
            {{{"x","v",true},{"y","v",true},{"x_inc","i32",false},{"y_inc","i32",false}},{{"result","t",false},{"error","i32",false}},{},{},{{"x_inc","i32","1"},{"y_inc","i32","1"}}},
            {{{"x","v",true},{"y","v",true},{"x_inc","i32",false},{"y_inc","i32",false}},{{"result","t",false},{"error","i32",false}},{},{},{{"x_inc","i32","1"},{"y_inc","i32","1"}}},
            {{{"x","v",true},{"y","v",true},{"x_inc","i32",false},{"y_inc","i32",false}},{{"result","t",false},{"error","i32",false}},{},{},{{"x_inc","i32","1"},{"y_inc","i32","1"}}},
            {{{"x","v",true},{"y","v",false},{"x_inc","i32",false},{"y_inc","i32",false},{"alpha","t",false}},{{"result","v",false},{"error","i32",false}},{},{},{{"y","array<f64>","[]"},{"x_inc","i32","1"},{"y_inc","i32","1"},{"alpha","f64","1"}}},
            {{{"x","v",true},{"x_inc","i32",false}},{{"result","f64",false},{"error","i32",false}},{},{},{{"x_inc","i32","1"}}},
            {{{"x","v",true},{"x_inc","i32",false}},{{"result","f64",false},{"error","i32",false}},{},{},{{"x_inc","i32","1"}}},
            {{{"x","v",true},{"y","v",false},{"x_inc","i32",false},{"y_inc","i32",false}},{{"result","v",false},{"error","i32",false}},{},{},{{"y","array<f64>","[]"},{"x_inc","i32","1"},{"y_inc","i32","1"}}},
            {{{"x","v",true},{"y","v",true},{"x_inc","i32",false},{"y_inc","i32",false}},{{"x_out","v",false},{"y_out","v",false},{"error","i32",false}},{},{},{{"x_inc","i32","1"},{"y_inc","i32","1"}}},
            {{{"x","v",true},{"y","v",true},{"rotation_matrix","m",true},{"rotation_matrix_flag","i32",true},{"x_inc","i32",false},{"y_inc","i32",false}},{{"x_out","v",false},{"y_out","v",false},{"error","i32",false}},{},{},{{"x_inc","i32","1"},{"y_inc","i32","1"}}},
            {{{"x","f64",true},{"y","f64",true},{"d1_in","f64",true},{"d2_in","f64",true}},{{"r","f64",false},{"d1_out","f64",false},{"d2_out","f64",false},{"rotation_matrix","m",false},{"rotation_matrix_flag","i32",false},{"error","i32",false}},{},{},{}},
            {{{"x","v",true},{"y","v",true},{"c","f64",true},{"s","f64",true},{"x_inc","i32",false},{"y_inc","i32",false}},{{"x_out","v",false},{"y_out","v",false},{"error","i32",false}},{},{},{{"x_inc","i32","1"},{"y_inc","i32","1"}}},
            {{{"x","t",true},{"y","t",true}},{{"r","t",false},{"c","f64",false},{"s","t",false},{"z","t",false},{"error","i32",false}},{},{},{}},
            {{{"x","v",true},{"x_inc","i32",false}},{{"result","i32",false},{"error","i32",false}},{},{},{{"x_inc","i32","1"}}},
            {{{"x","v",true},{"x_inc","i32",false}},{{"result","i32",false},{"error","i32",false}},{},{},{{"x_inc","i32","1"}}},
            {{{"a","m",true},{"x","v",true},{"y","v",false},{"alpha","t",false},{"beta","t",false},{"operation_a","i32",false}},{{"result","v",false},{"error","i32",false}},{},{},{{"y","array<f64>","[]"},{"alpha","f64","1"},{"beta","f64","1"},{"operation_a","i32","0"}}},
            {{{"a","m",true},{"x","v",true},{"y","v",false},{"alpha","t",false},{"beta","t",false},{"matrix_a_type","i32",false}},{{"result","v",false},{"error","i32",false}},{},{},{{"y","array<f64>","[]"},{"alpha","f64","1"},{"beta","f64","1"},{"matrix_a_type","i32","3"}}},
            {{{"a","m",true},{"x","v",true},{"y","v",false},{"alpha","t",false},{"beta","t",false},{"matrix_a_type","i32",false}},{{"result","v",false},{"error","i32",false}},{},{},{{"y","array<c64>","[]"},{"alpha","c64","1"},{"beta","c64","1"},{"matrix_a_type","i32","3"}}},
            {{{"a","m",true},{"x","v",true},{"operation_a","i32",false},{"matrix_a_type","i32",false},{"diagonal","i32",false}},{{"result","v",false},{"error","i32",false}},{},{},{{"operation_a","i32","0"},{"matrix_a_type","i32","3"},{"diagonal","i32","0"}}},
            {{{"x","v",true},{"a","m",false},{"alpha","t",false},{"matrix_a_type","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"a","array<f64,rank=2>","[]"},{"alpha","f64","1"},{"matrix_a_type","i32","3"}}},
            {{{"x","v",true},{"y","v",true},{"a","m",false},{"alpha","t",false},{"matrix_a_type","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"a","array<f64,rank=2>","[]"},{"alpha","f64","1"},{"matrix_a_type","i32","3"}}},
            {{{"x","v",true},{"a","m",true},{"alpha","f64",false},{"matrix_a_type","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"alpha","f64","1"},{"matrix_a_type","i32","3"}}},
            {{{"x","v",true},{"y","v",true},{"a","m",false},{"alpha","t",false},{"matrix_a_type","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"a","array<c64,rank=2>","[]"},{"alpha","c64","1"},{"matrix_a_type","i32","3"}}},
            {{{"x","v",true},{"y","v",true},{"a","m",false},{"alpha","t",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"a","array<f64,rank=2>","[]"},{"alpha","f64","1"}}},
            {{{"x","v",true},{"y","v",true},{"a","m",false},{"alpha","t",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"a","array<c64,rank=2>","[]"},{"alpha","c64","1"}}},
            {{{"x","v",true},{"y","v",true},{"a","m",false},{"alpha","t",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"a","array<c64,rank=2>","[]"},{"alpha","c64","1"}}},
            {{{"a","m",true},{"x","v",true},{"operation_a","i32",false},{"matrix_a_type","i32",false},{"diagonal","i32",false}},{{"result","v",false},{"error","i32",false}},{},{},{{"operation_a","i32","0"},{"matrix_a_type","i32","3"},{"diagonal","i32","0"}}},
            {{{"a","m",true},{"b","m",true},{"c","m",true},{"alpha","t",false},{"beta","t",false},{"operation_a","i32",false},{"operation_b","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"alpha","f64","1"},{"beta","f64","1"},{"operation_a","i32","0"},{"operation_b","i32","0"}}},
            {{{"a","m",true},{"b","m",true},{"c","m",false},{"alpha","t",false},{"beta","t",false},{"side","i32",false},{"matrix_a_type","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"c","array<f64,rank=2>","[]"},{"alpha","f64","1"},{"beta","f64","1"},{"side","i32","1"},{"matrix_a_type","i32","3"}}},
            {{{"a","m",true},{"b","m",true},{"c","m",false},{"alpha","t",false},{"beta","t",false},{"side","i32",false},{"matrix_a_type","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"c","array<c64,rank=2>","[]"},{"alpha","c64","1"},{"beta","c64","1"},{"side","i32","1"},{"matrix_a_type","i32","3"}}},
            {{{"a","m",true},{"b","m",true},{"alpha","t",false},{"side","i32",false},{"operation_a","i32",false},{"matrix_a_type","i32",false},{"diagonal","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"alpha","f64","1"},{"side","i32","1"},{"operation_a","i32","0"},{"matrix_a_type","i32","3"},{"diagonal","i32","0"}}},
            {{{"a","m",true},{"c","m",true},{"alpha","t",false},{"beta","t",false},{"operation_a","i32",false},{"matrix_c_type","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"alpha","f64","1"},{"beta","f64","1"},{"operation_a","i32","0"},{"matrix_c_type","i32","3"}}},
            {{{"a","m",true},{"b","m",true},{"c","m",true},{"alpha","t",false},{"beta","t",false},{"operation","i32",false},{"matrix_c_type","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"alpha","f64","1"},{"beta","f64","1"},{"operation","i32","0"},{"matrix_c_type","i32","3"}}},
            {{{"a","m",true},{"c","m",false},{"alpha","f64",false},{"beta","f64",false},{"operation_a","i32",false},{"matrix_c_type","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"c","array<c64,rank=2>","[]"},{"alpha","f64","1"},{"beta","f64","1"},{"operation_a","i32","0"},{"matrix_c_type","i32","3"}}},
            {{{"a","m",true},{"b","m",true},{"c","m",false},{"alpha","t",false},{"beta","f64",false},{"operation","i32",false},{"matrix_c_type","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"c","array<c64,rank=2>","[]"},{"alpha","c64","1"},{"beta","f64","1"},{"operation","i32","0"},{"matrix_c_type","i32","3"}}},
            {{{"a","m",true},{"x","m",true},{"alpha","t",false},{"side","i32",false},{"operation_a","i32",false},{"matrix_a_type","i32",false},{"diagonal","i32",false}},{{"result","m",false},{"error","i32",false}},{},{},{{"alpha","f64","1"},{"side","i32","1"},{"operation_a","i32","0"},{"matrix_a_type","i32","3"},{"diagonal","i32","0"}}},
        }};
        for(auto &law:result) {
            for(const auto &p:law.inputs)law.inputIds.push_back(p.id);
            for(const auto &p:law.outputs)law.outputIds.push_back(p.id);
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
std::size_t ordinal(std::string_view id) {
    id=canonical(id);
    for(std::size_t i=0;i<descriptions.size();++i)if(descriptions[i].catalogId==id)return i;
    return descriptions.size();
}
BlasTypeResult failure(std::string_view code,std::string_view message) {
    return {{ResolutionStatus::Incompatible,{},std::string(code),std::string(message)},false};
}
bool arrayKind(std::string_view kind) {
    return kind=="m"||kind=="v"||kind=="rm"||kind=="rv"||kind=="cm"||kind=="cv";
}
bool numericLeaf(const TypeRef &type) {
    return type.numeric() && type.kind==TypeKind::Primitive && type.name!="f128" && type.name!="c128";
}
TypeRef portType(std::string_view kind,bool complex) {
    if(arrayKind(kind)) {
        const auto scalar=(kind=="cm"||kind=="cv")?"c64":(kind=="rm"||kind=="rv")?"f64":complex?"c64":"f64";
        return TypeRef::array(TypeRef::primitive(scalar),(kind=="v"||kind=="rv"||kind=="cv")?1:2);
    }
    return TypeRef::primitive(kind=="t"?(complex?"c64":"f64"):kind);
}
} // namespace

std::span<const BlasFunctionDescriptor> blasFunctions() noexcept { return descriptions; }
const BlasFunctionDescriptor *findBlasFunction(std::string_view id) noexcept {
    const auto index=ordinal(id);return index==descriptions.size()?nullptr:&descriptions[index];
}
bool isBlasFunction(std::string_view id) noexcept { return findBlasFunction(id)!=nullptr; }
std::span<const std::string_view> blasInputPortIds(std::string_view id) noexcept {
    const auto index=ordinal(id);return index==descriptions.size()?std::span<const std::string_view>{}:std::span<const std::string_view>{laws()[index].inputIds};
}
std::span<const std::string_view> blasOutputPortIds(std::string_view id) noexcept {
    const auto index=ordinal(id);return index==descriptions.size()?std::span<const std::string_view>{}:std::span<const std::string_view>{laws()[index].outputIds};
}
std::span<const BlasInputDefault> blasInputDefaults(std::string_view id) noexcept {
    const auto index=ordinal(id);return index==descriptions.size()?std::span<const BlasInputDefault>{}:std::span<const BlasInputDefault>{laws()[index].defaults};
}
BlasTypeResult resolveBlasPort(std::string_view id,std::string_view port,std::span<const BlasTypeEvidence> inputs) {
    const auto index=ordinal(id);
    if(index==descriptions.size())return failure("math.linear_algebra.blas.unknown_function","The BLAS operation is not registered.");
    const auto &law=laws()[index];const PortLaw *requested=nullptr;bool requestedInput=false;
    for(const auto &p:law.inputs)if(p.id==port){requested=&p;requestedInput=true;}
    for(const auto &p:law.outputs)if(p.id==port)requested=&p;
    if(!requested)return failure("math.linear_algebra.blas.missing_port","This BLAS function has no such port.");
    std::set<std::string_view> seen;bool complex=domains[index]=="c64";bool complete=true;
    const auto evidence=[&](std::string_view name)->const TypeRef* {
        for(const auto &e:inputs)if(e.portId==name)return &e.valueType;return nullptr;
    };
    for(const auto &e:inputs) {
        if(!seen.insert(e.portId).second)return failure("math.linear_algebra.blas.multiple_sources","A BLAS input has more than one source.");
        const auto it=std::find_if(law.inputs.begin(),law.inputs.end(),[&](const auto &p){return p.id==e.portId;});
        if(it==law.inputs.end())return failure("math.linear_algebra.blas.unexpected_input","The BLAS function has no input with this identity.");
        if(!e.valueType.known()){complete=false;continue;}
        const auto &t=e.valueType;const auto k=it->kind;const TypeRef *scalar=&t;
        if(arrayKind(k)) {
            if(t.kind!=TypeKind::Array||t.children.size()!=1||!numericLeaf(t.children.front()))
                return failure("math.linear_algebra.blas.array_type","BLAS requires numeric Arrays; matrices use the existing rank-two Array representation.");
            const std::size_t rank=(k=="v"||k=="rv"||k=="cv")?1:2;
            if(t.rank!=rank)return failure("math.linear_algebra.blas.rank","The numeric Array rank does not match this BLAS port.");
            scalar=&t.children.front();
        } else {
            if(t.kind==TypeKind::Enum&&t.children.size()==1)scalar=&t.children.front();
            if(!numericLeaf(*scalar))return failure("math.linear_algebra.blas.scalar_type","This BLAS parameter requires a numeric scalar.");
        }
        const bool c=scalar->name.starts_with("c");
        if(c&&(k=="rm"||k=="rv"||k=="f64"||k=="i32"||((k=="m"||k=="v"||k=="t")&&domains[index]=="f64")))
            return failure("math.linear_algebra.blas.real_operand","This BLAS port requires a real numeric operand; an imaginary part is never discarded.");
        if((k=="m"||k=="v"||k=="t")&&c)complex=true;
    }
    for(const auto &p:law.inputs)if(p.required){const auto *t=evidence(p.id);if(!t||!t->known())complete=false;}
    auto result=portType(requested->kind,complex);
    const auto *supplied=requestedInput?evidence(port):nullptr;
    if(supplied&&supplied->known()&&*supplied!=result)
        return {{ResolutionStatus::Coercion,std::move(result),"math.linear_algebra.blas.numeric_promotion","BLAS calculates in its declared DBL/CDB representation; promotion is explicit at this input."},complete};
    return {{ResolutionStatus::Compatible,std::move(result),{},{}},complete};
}
} // namespace frog::native_engine
