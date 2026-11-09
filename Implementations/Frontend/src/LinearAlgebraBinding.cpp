#include "frog_native_engine/LinearAlgebraBinding.hpp"
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
struct FunctionLaw { std::vector<PortLaw> inputs, outputs; std::vector<std::string_view> inputIds, outputIds; std::vector<LinearAlgebraInputDefault> defaults; };
constexpr std::array<LinearAlgebraFunctionDescriptor,37> descriptions{{
    {"math.linear_algebra.special_matrix","frog.studio.math.linear_algebra.special_matrix.owned.v1","Create Special Matrix"},
    {"math.linear_algebra.real_matrix_from_eigenvalues","frog.studio.math.linear_algebra.real_matrix_from_eigenvalues.owned.v1","Create Real Matrix From Eigenvalues"},
    {"math.linear_algebra.solve_linear_equations","frog.studio.math.linear_algebra.solve_linear_equations.owned.v1","Solve Linear Eqs"},
    {"math.linear_algebra.dot_product","frog.studio.math.linear_algebra.dot_product.owned.v1","Dot Product"},
    {"math.linear_algebra.outer_product","frog.studio.math.linear_algebra.outer_product.owned.v1","Outer Product"},
    {"math.linear_algebra.matrix_multiply","frog.studio.math.linear_algebra.matrix_multiply.owned.v1","A x B"},
    {"math.linear_algebra.kronecker_product","frog.studio.math.linear_algebra.kronecker_product.owned.v1","Kronecker Prod"},
    {"math.linear_algebra.subspaces_angle","frog.studio.math.linear_algebra.subspaces_angle.owned.v1","Subspaces Angle"},
    {"math.linear_algebra.determinant","frog.studio.math.linear_algebra.determinant.owned.v1","Determinant"},
    {"math.linear_algebra.vector_norm","frog.studio.math.linear_algebra.vector_norm.owned.v1","Vector Norm"},
    {"math.linear_algebra.matrix_norm","frog.studio.math.linear_algebra.matrix_norm.owned.v1","Matrix Norm"},
    {"math.linear_algebra.matrix_rank","frog.studio.math.linear_algebra.matrix_rank.owned.v1","Matrix Rank"},
    {"math.linear_algebra.trace","frog.studio.math.linear_algebra.trace.owned.v1","Trace"},
    {"math.linear_algebra.test_matrix_type","frog.studio.math.linear_algebra.test_matrix_type.owned.v1","Test Matrix Type"},
    {"math.linear_algebra.condition_number","frog.studio.math.linear_algebra.condition_number.owned.v1","Condition Number"},
    {"math.linear_algebra.inverse_matrix","frog.studio.math.linear_algebra.inverse_matrix.owned.v1","Inverse Matrix"},
    {"math.linear_algebra.pseudoinverse","frog.studio.math.linear_algebra.pseudoinverse.owned.v1","PseudoInv"},
    {"math.linear_algebra.matrix_sqrt","frog.studio.math.linear_algebra.matrix_sqrt.owned.v1","Matrix Sqrt"},
    {"math.linear_algebra.matrix_exp","frog.studio.math.linear_algebra.matrix_exp.owned.v1","Matrix Exp"},
    {"math.linear_algebra.matrix_power","frog.studio.math.linear_algebra.matrix_power.owned.v1","Matrix Power"},
    {"math.linear_algebra.matrix_log","frog.studio.math.linear_algebra.matrix_log.owned.v1","Matrix Log"},
    {"math.linear_algebra.lu","frog.studio.math.linear_algebra.lu.owned.v1","LU"},
    {"math.linear_algebra.cholesky","frog.studio.math.linear_algebra.cholesky.owned.v1","Cholesky"},
    {"math.linear_algebra.cholesky_update","frog.studio.math.linear_algebra.cholesky_update.owned.v1","Cholesky Update"},
    {"math.linear_algebra.qr","frog.studio.math.linear_algebra.qr.owned.v1","QR"},
    {"math.linear_algebra.svd","frog.studio.math.linear_algebra.svd.owned.v1","SVD"},
    {"math.linear_algebra.generalized_svd","frog.studio.math.linear_algebra.generalized_svd.owned.v1","Generalized SVD"},
    {"math.linear_algebra.schur","frog.studio.math.linear_algebra.schur.owned.v1","Schur"},
    {"math.linear_algebra.hessenberg","frog.studio.math.linear_algebra.hessenberg.owned.v1","Hessenberg"},
    {"math.linear_algebra.qz","frog.studio.math.linear_algebra.qz.owned.v1","QZ"},
    {"math.linear_algebra.sylvester_equations","frog.studio.math.linear_algebra.sylvester_equations.owned.v1","Sylvester Eqs"},
    {"math.linear_algebra.lyapunov_equations","frog.studio.math.linear_algebra.lyapunov_equations.owned.v1","Lyapunov Eqs"},
    {"math.linear_algebra.eigenvalues_vectors","frog.studio.math.linear_algebra.eigenvalues_vectors.owned.v1","Eigenvalues and Vectors"},
    {"math.linear_algebra.generalized_eigenvalues_vectors","frog.studio.math.linear_algebra.generalized_eigenvalues_vectors.owned.v1","Generalized Eigenvalues and Vectors"},
    {"math.linear_algebra.matrix_balance","frog.studio.math.linear_algebra.matrix_balance.owned.v1","Matrix Balance"},
    {"math.linear_algebra.back_transform_eigenvectors","frog.studio.math.linear_algebra.back_transform_eigenvectors.owned.v1","Back Transform Eigenvectors"},
    {"math.linear_algebra.characteristic_polynomial","frog.studio.math.linear_algebra.characteristic_polynomial.owned.v1","Matrix Characteristic Polynomial"},
}};
const std::array<FunctionLaw,37> &laws() {
    static const auto value=[] {
        std::array<FunctionLaw,37> result{{
            {{{"x","v",false},{"y","v",false},{"matrix_type","i32",false},{"matrix_size","i32",false}},{{"result","m",false},{"error","i32",false}},{}, {}, {{"x","array<f64>","[]"},{"y","array<f64>","[]"},{"matrix_type","i32","0"},{"matrix_size","i32","1"}}},
            {{{"eigenvalues","cv",true}},{{"result","rm",false},{"error","i32",false}},{}, {}, {}},
            {{{"a","m",true},{"b","vm",true},{"matrix_type","i32",false}},{{"result","solve",false},{"error","i32",false}},{}, {}, {{"matrix_type","i32","0"}}},
            {{{"x","v",true},{"y","v",true}},{{"result","t",false},{"error","i32",false}},{}, {}, {}},
            {{{"x","v",true},{"y","v",true}},{{"result","m",false},{"error","i32",false}},{}, {}, {}},
            {{{"a","vm",true},{"b","vm",true}},{{"result","multiply",false},{"error","i32",false}},{}, {}, {}},
            {{{"a","m",true},{"b","m",true}},{{"result","m",false},{"error","i32",false}},{}, {}, {}},
            {{{"a","rvm",true},{"b","rvm",true}},{{"result","f64",false},{"error","i32",false}},{}, {}, {}},
            {{{"a","m",true},{"matrix_type","i32",false}},{{"result","t",false},{"error","i32",false}},{}, {}, {{"matrix_type","i32","0"}}},
            {{{"x","v",true},{"norm_type","u16",false},{"p","f64",false}},{{"result","f64",false},{"error","i32",false}},{}, {}, {{"norm_type","u16","2"},{"p","f64","-1"}}},
            {{{"a","m",true},{"norm_type","i32",false}},{{"result","f64",false},{"error","i32",false}},{}, {}, {{"norm_type","i32","0"}}},
            {{{"a","m",true},{"tolerance","f64",false}},{{"result","i32",false},{"error","i32",false}},{}, {}, {{"tolerance","f64","-1"}}},
            {{{"a","m",true}},{{"result","t",false},{"error","i32",false}},{}, {}, {}},
            {{{"a","m",true},{"matrix_type","i32",false},{"tolerance","f64",false}},{{"result","bool",false},{"error","i32",false}},{}, {}, {{"matrix_type","i32","0"},{"tolerance","f64","-1"}}},
            {{{"a","m",true},{"norm_type","i32",false}},{{"result","f64",false},{"error","i32",false}},{}, {}, {{"norm_type","i32","0"}}},
            {{{"a","m",true},{"matrix_type","i32",false}},{{"result","m",false},{"error","i32",false}},{}, {}, {{"matrix_type","i32","0"}}},
            {{{"a","m",true},{"tolerance","f64",false}},{{"result","m",false},{"error","i32",false}},{}, {}, {{"tolerance","f64","-1"}}},
            {{{"a","m",true}},{{"result","cm",false},{"error","i32",false}},{}, {}, {}},
            {{{"a","m",true}},{{"result","m",false},{"error","i32",false}},{}, {}, {}},
            {{{"a","m",true},{"n","i32",false}},{{"result","m",false},{"error","i32",false}},{}, {}, {{"n","i32","1"}}},
            {{{"a","m",true},{"logarithm_option","u16",false}},{{"result","cm",false},{"error","i32",false}},{}, {}, {{"logarithm_option","u16","0"}}},
            {{{"a","m",true}},{{"l","m",false},{"u","m",false},{"p","im",false},{"error","i32",false}},{}, {}, {}},
            {{{"a","m",true}},{{"result","m",false},{"error","i32",false}},{}, {}, {}},
            {{{"r","m",true},{"x","v",true},{"method","i32",false}},{{"result","m",false},{"error","i32",false}},{}, {}, {{"method","i32","0"}}},
            {{{"a","m",true},{"pivot","bool",false},{"q_option","u16",false}},{{"q","m",false},{"r","m",false},{"p","im",false},{"error","i32",false}},{}, {}, {{"pivot","bool","false"},{"q_option","u16","0"}}},
            {{{"a","m",true},{"values_only","bool",false},{"svd_option","u16",false}},{{"singular_values","rv",false},{"u","m",false},{"s","m",false},{"v","m",false},{"error","i32",false}},{}, {}, {{"values_only","bool","false"},{"svd_option","u16","0"}}},
            {{{"a","m",true},{"b","m",true},{"values_only","bool",false},{"svd_option","u16",false}},{{"singular_values","rv",false},{"u","m",false},{"v","m",false},{"c","rm",false},{"s","rm",false},{"r","m",false},{"error","i32",false}},{}, {}, {{"values_only","bool","false"},{"svd_option","u16","0"}}},
            {{{"a","m",true},{"compute_vectors","bool",false},{"order","u16",false}},{{"result","m",false},{"vectors","m",false},{"eigenvalues","cv",false},{"error","i32",false}},{}, {}, {{"compute_vectors","bool","false"},{"order","u16","0"}}},
            {{{"a","m",true},{"index_low","i32",false},{"index_high","i32",false}},{{"result","m",false},{"q","m",false},{"error","i32",false}},{}, {}, {{"index_low","i32","-1"},{"index_high","i32","-1"}}},
            {{{"a","m",true},{"b","m",true},{"decomposition_type","i32",false},{"order","u16",false}},{{"eigenvectors","cm",false},{"q","m",false},{"z","m",false},{"alpha","cv",false},{"beta","betav",false},{"s","m",false},{"t","m",false},{"error","i32",false}},{}, {}, {{"decomposition_type","i32","0"},{"order","u16","0"}}},
            {{{"a","m",true},{"b","m",true},{"c","m",true},{"operation_a","u16",false},{"operation_b","u16",false},{"sign","u16",false},{"matrix_type","i32",false}},{{"result","m",false},{"scale","f64",false},{"perturbed","bool",false},{"error","i32",false}},{}, {}, {{"operation_a","u16","0"},{"operation_b","u16","0"},{"sign","u16","0"},{"matrix_type","i32","3"}}},
            {{{"a","m",true},{"b","m",true},{"matrix_type","i32",false},{"equation_type","i32",false}},{{"result","m",false},{"scale","f64",false},{"error","i32",false}},{}, {}, {{"matrix_type","i32","3"},{"equation_type","i32","0"}}},
            {{{"a","m",true},{"matrix_type","i32",false},{"output_option","i32",false}},{{"eigenvalues","cv",false},{"eigenvectors","cm",false},{"error","i32",false}},{}, {}, {{"matrix_type","i32","0"},{"output_option","i32","1"}}},
            {{{"a","m",true},{"b","m",true},{"output_option","i32",false}},{{"eigenvalues","cv",false},{"eigenvectors","cm",false},{"error","i32",false}},{}, {}, {{"output_option","i32","1"}}},
            {{{"a","m",true},{"job","u16",false}},{{"result","m",false},{"index_low","i32",false},{"index_high","i32",false},{"scale","rv",false},{"error","i32",false}},{}, {}, {{"job","u16","3"}}},
            {{{"eigenvectors","m",true},{"scale","rv",true},{"index_low","i32",false},{"index_high","i32",false},{"job","u16",false},{"side","u16",false}},{{"result","m",false},{"error","i32",false}},{}, {}, {{"index_low","i32","-1"},{"index_high","i32","-1"},{"job","u16","3"},{"side","u16","0"}}},
            {{{"a","m",true}},{{"result","v",false},{"error","i32",false}},{}, {}, {}},
        }};
        for(auto &law:result) { for(const auto &p:law.inputs)law.inputIds.push_back(p.id); for(const auto &p:law.outputs)law.outputIds.push_back(p.id); }
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
LinearAlgebraTypeResult failure(std::string_view code,std::string_view message) {
    return {{ResolutionStatus::Incompatible,{},std::string(code),std::string(message)},false};
}
bool dataKind(std::string_view kind) { return kind=="m"||kind=="v"||kind=="vm"||kind=="rvm"||kind=="rm"||kind=="rv"||kind=="cm"||kind=="cv"; }
bool numericLeaf(const TypeRef &type) {
    return type.numeric() && type.kind==TypeKind::Primitive && type.name!="f128" && type.name!="c128";
}
TypeRef arrayType(std::string_view scalar,std::size_t rank) { return TypeRef::array(TypeRef::primitive(scalar),rank); }
} // namespace

std::span<const LinearAlgebraFunctionDescriptor> linearAlgebraFunctions() noexcept { return descriptions; }
const LinearAlgebraFunctionDescriptor *findLinearAlgebraFunction(std::string_view id) noexcept { const auto index=ordinal(id); return index==descriptions.size()?nullptr:&descriptions[index]; }
bool isLinearAlgebraFunction(std::string_view id) noexcept { return findLinearAlgebraFunction(id)!=nullptr; }
std::span<const std::string_view> linearAlgebraInputPortIds(std::string_view id) noexcept { const auto index=ordinal(id); return index==descriptions.size()?std::span<const std::string_view>{}:std::span<const std::string_view>{laws()[index].inputIds}; }
std::span<const std::string_view> linearAlgebraOutputPortIds(std::string_view id) noexcept { const auto index=ordinal(id); return index==descriptions.size()?std::span<const std::string_view>{}:std::span<const std::string_view>{laws()[index].outputIds}; }
std::span<const LinearAlgebraInputDefault> linearAlgebraInputDefaults(std::string_view id) noexcept { const auto index=ordinal(id); return index==descriptions.size()?std::span<const LinearAlgebraInputDefault>{}:std::span<const LinearAlgebraInputDefault>{laws()[index].defaults}; }

LinearAlgebraTypeResult resolveLinearAlgebraPort(std::string_view id,std::string_view port,std::span<const LinearAlgebraTypeEvidence> inputs) {
    const auto index=ordinal(id); if(index==descriptions.size())return failure("math.linear_algebra.unknown_function","The Linear Algebra operation is not registered.");
    const auto &law=laws()[index]; const auto catalog=descriptions[index].catalogId;
    const PortLaw *requested=nullptr; bool requestedInput=false;
    for(const auto &p:law.inputs)if(p.id==port){requested=&p;requestedInput=true;}
    for(const auto &p:law.outputs)if(p.id==port)requested=&p;
    if(!requested)return failure("math.linear_algebra.missing_port","This Linear Algebra function has no such port.");
    std::set<std::string_view> seen; bool complex=false; bool complete=true;
    const auto evidence=[&](std::string_view name)->const TypeRef* {for(const auto &e:inputs)if(e.portId==name)return &e.valueType;return nullptr;};
    for(const auto &e:inputs) {
        if(!seen.insert(e.portId).second)return failure("math.linear_algebra.multiple_sources","A Linear Algebra input has more than one source.");
        const auto it=std::find_if(law.inputs.begin(),law.inputs.end(),[&](const auto &p){return p.id==e.portId;});
        if(it==law.inputs.end())return failure("math.linear_algebra.unexpected_input","The Linear Algebra function has no input with this identity.");
        if(!e.valueType.known()){complete=false;continue;}
        const auto &t=e.valueType; const auto k=it->kind;
        if(dataKind(k)) {
            if(t.kind!=TypeKind::Array || t.children.size()!=1 || !numericLeaf(t.children.front()))return failure("math.linear_algebra.array_type","Linear Algebra requires numeric Arrays; Matrix uses the existing rank-two Array representation.");
            const bool flexible=k=="vm"||k=="rvm"; const std::size_t rank=(k=="v"||k=="rv"||k=="cv")?1:2;
            if((flexible && t.rank!=1 && t.rank!=2)||(!flexible && t.rank!=rank))return failure("math.linear_algebra.rank","The numeric Array rank does not match this Linear Algebra port.");
            const bool c=t.children.front().name.starts_with("c");
            if((k=="rvm"||k=="rm"||k=="rv")&&c)return failure("math.linear_algebra.real_operand","This port requires a real numeric Array.");
            complex=complex||c;
        } else if(k=="bool") {
            if(t!=TypeRef::primitive("bool"))return failure("math.linear_algebra.boolean_option","This Linear Algebra option requires a Boolean scalar.");
        } else {
            const auto &scalar=t.kind==TypeKind::Enum && t.children.size()==1?t.children.front():t;
            if(!numericLeaf(scalar)||scalar.name.starts_with("c"))return failure("math.linear_algebra.numeric_option","This Linear Algebra option requires a real numeric scalar.");
        }
    }
    for(const auto &p:law.inputs)if(p.required){const auto *t=evidence(p.id);if(!t||!t->known())complete=false;}
    const auto *a=evidence("a"), *b=evidence("b");
    if(catalog.ends_with(".matrix_multiply") && a&&b&&a->known()&&b->known()&&a->rank==1&&b->rank==1)return failure("math.linear_algebra.multiply_rank","A x B accepts matrix/matrix, matrix/vector or vector/matrix; use Dot Product for two vectors.");
    const auto kind=requested->kind; const auto scalar=complex?"c64":"f64"; TypeRef result;
    if(dataKind(kind)) {
        const auto *supplied=evidence(port); std::size_t rank=(kind=="v"||kind=="rv"||kind=="cv")?1:2;
        if((kind=="vm"||kind=="rvm") && supplied&&supplied->known())rank=supplied->rank;
        else if(kind=="vm" && catalog.ends_with(".solve_linear_equations") && port=="b")rank=1;
        const auto element=(kind=="cm"||kind=="cv")?"c64":(kind=="rm"||kind=="rv"||kind=="rvm")?"f64":scalar;
        result=arrayType(element,rank);
    } else if(kind=="im")result=arrayType("i32",2);
    else if(kind=="t")result=TypeRef::primitive(scalar);
    else if(kind=="betav")result=arrayType(scalar,1);
    else if(kind=="solve")result=arrayType(scalar,b&&b->known()?b->rank:1);
    else if(kind=="multiply")result=arrayType(scalar,(a&&a->known()&&a->rank==1)||(b&&b->known()&&b->rank==1)?1:2);
    else result=TypeRef::primitive(kind);
    const auto *supplied=requestedInput?evidence(port):nullptr;
    if(supplied && supplied->known() && *supplied!=result)return {{ResolutionStatus::Coercion,std::move(result),"math.linear_algebra.numeric_promotion","Linear Algebra calculates in DBL/CDB; numeric promotion is explicit at this input."},complete};
    return {{ResolutionStatus::Compatible,std::move(result),{},{}},complete};
}
} // namespace frog::native_engine
