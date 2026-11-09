#include "frog_native_engine/PolynomialBinding.hpp"
#include <algorithm>
#include <array>
#include <set>
#include <vector>
namespace frog::native_engine {
namespace {
using binding::TypeRef;using binding::TypeKind;using binding::ResolutionStatus;
struct Port {std::string_view id,kind;bool required;};
struct Law {
    std::string_view catalog,operation,name;
    std::vector<Port> inputs,outputs;
    std::vector<std::string_view> inputIds,outputIds;
};
const auto &laws(){static const auto value=[] {
    std::array<Law,41> result{{
#include "PolynomialLaws.inc"
    }};
    for(auto &l:result){for(const auto &p:l.inputs)l.inputIds.push_back(p.id);for(const auto &p:l.outputs)l.outputIds.push_back(p.id);}return result;
}();return value;}
std::size_t ordinal(std::string_view id){for(std::size_t i=0;i<laws().size();++i)if(id==laws()[i].catalog||id==laws()[i].operation||
    (id.starts_with("frog.studio.")&&id.substr(12)==laws()[i].catalog))return i;return laws().size();}
PolynomialTypeResult fail(std::string_view code,std::string_view message){return {{ResolutionStatus::Incompatible,{},std::string(code),std::string(message)},false};}
bool numeric(const TypeRef &t){return t.kind==TypeKind::Primitive&&t.numeric()&&t.name!="f128"&&t.name!="c128";}
bool data(std::string_view k){return k=="v"||k=="rv"||k=="cv"||k=="m"||k=="cm"||k=="vm"||k=="tensor"||k=="t"||k=="query";}
}
std::span<const PolynomialFunctionDescriptor> polynomialFunctions() noexcept {
    static const auto value=[] {std::array<PolynomialFunctionDescriptor,41> out;for(std::size_t i=0;i<out.size();++i)out[i]={laws()[i].catalog,laws()[i].operation,laws()[i].name};return out;}();return value;
}
const PolynomialFunctionDescriptor *findPolynomialFunction(std::string_view id) noexcept {const auto i=ordinal(id);return i==laws().size()?nullptr:&polynomialFunctions()[i];}
bool isPolynomialFunction(std::string_view id) noexcept{return findPolynomialFunction(id)!=nullptr;}
std::span<const std::string_view> polynomialInputPortIds(std::string_view id) noexcept {const auto i=ordinal(id);return i==laws().size()?std::span<const std::string_view>{}:std::span<const std::string_view>{laws()[i].inputIds};}
std::span<const std::string_view> polynomialOutputPortIds(std::string_view id) noexcept {const auto i=ordinal(id);return i==laws().size()?std::span<const std::string_view>{}:std::span<const std::string_view>{laws()[i].outputIds};}
PolynomialTypeResult resolvePolynomialPort(std::string_view id,std::string_view port,std::span<const PolynomialTypeEvidence> evidence){
    const auto index=ordinal(id);if(index==laws().size())return fail("math.polynomial.unknown_function","Unknown Polynomial operation.");
    const auto &law=laws()[index];const Port *requested=nullptr;bool input=false;
    for(const auto &p:law.inputs)if(p.id==port){requested=&p;input=true;}
    for(const auto &p:law.outputs)if(p.id==port)requested=&p;
    if(!requested)return fail("math.polynomial.missing_port","No such Polynomial connector.");
    const auto supplied=[&](std::string_view name)->const TypeRef*{for(const auto &e:evidence)if(e.portId==name)return &e.valueType;return nullptr;};
    bool complex=false,complete=true;std::set<std::string_view> seen;
    for(const auto &e:evidence){
        if(!seen.insert(e.portId).second)return fail("math.polynomial.multiple_sources","A Polynomial input has multiple sources.");
        const auto p=std::find_if(law.inputs.begin(),law.inputs.end(),[&](const auto &p){return p.id==e.portId;});
        if(p==law.inputs.end())return fail("math.polynomial.unexpected_input","An undeclared Polynomial input is connected.");
        if(!e.valueType.known()){complete=false;continue;}const auto &t=e.valueType;const auto k=p->kind;
        if(data(k)){
            const TypeRef *leaf=&t;std::size_t rank=0;
            if(t.kind==TypeKind::Array&&t.children.size()==1){leaf=&t.children[0];rank=t.rank;}
            if(!numeric(*leaf))return fail("math.polynomial.numeric_type","Polynomial data must have a supported real or complex numeric leaf.");
            const bool validRank=k=="query"?rank<=2:k=="t"?rank==0:k=="tensor"?rank==3:k=="vm"?(rank==1||rank==2):(k=="m"||k=="cm")?rank==2:rank==1;
            if(!validRank)return fail("math.polynomial.rank","The Polynomial input has the wrong rank; coefficients, points and matrix coefficients are distinct.");
            const bool c=leaf->name.starts_with("c");if(k=="rv"&&c)return fail("math.polynomial.real_input","This Polynomial connector requires a real array.");complex=complex||c;
        }else if(k=="iv"){
            if(t.kind!=TypeKind::Array||t.rank!=1||t.children.size()!=1||!numeric(t.children[0])||!t.children[0].integer())return fail("math.polynomial.multiplicity_type","Multiplicity requires a rank-one integer array.");
        }else if(k=="bool"){
            if(t!=TypeRef::primitive("bool"))return fail("math.polynomial.boolean_option","This Polynomial option requires a Boolean.");
        }else {
            const auto &s=t.kind==TypeKind::Enum&&t.children.size()==1?t.children[0]:t;
            if(!numeric(s)||s.name.starts_with("c"))return fail("math.polynomial.option_type","A Polynomial option requires a real numeric scalar.");
        }
    }
    for(const auto &p:law.inputs)if(p.required){const auto *t=supplied(p.id);if(!t||!t->known())complete=false;}
    const auto k=requested->kind;const auto leaf=complex?"c64":"f64";TypeRef result;
    if(k=="xy")result=binding::parseType("cluster<X:array<f64>,Y:array<f64>>").type;
    else if(k=="like_x"||k=="query"){
        const auto *x=supplied("x");const auto rank=x&&x->kind==TypeKind::Array?x->rank:law.catalog.ends_with(".linear_evaluate")?1u:0u;result=rank?TypeRef::array(TypeRef::primitive(leaf),rank):TypeRef::primitive(leaf);
    }else if(k=="v"||k=="rv"||k=="cv"||k=="m"||k=="cm"||k=="vm"||k=="tensor"||k=="iv"){
        auto rank=k=="m"||k=="cm"?2u:k=="tensor"?3u:1u;
        if(k=="vm"){const auto *t=supplied(port);if(t&&t->kind==TypeKind::Array)rank=t->rank;}
        result=TypeRef::array(TypeRef::primitive(k=="iv"?"i32":k=="rv"?"f64":k=="cv"||k=="cm"?"c64":leaf),rank);
    }else result=TypeRef::primitive(k=="t"?leaf:k);
    const auto *actual=input?supplied(port):nullptr;
    if(actual&&actual->known()&&*actual!=result)return {{ResolutionStatus::Coercion,std::move(result),"math.polynomial.numeric_promotion","Explicit DBL/CDB or option representation conversion at this connector."},complete};
    return {{ResolutionStatus::Compatible,std::move(result),{},{}},complete};
}
}
