#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog_native_engine/CanonicalScalarNative.hpp"
#include "frog_native_engine/CanonicalNumericConversionTree.hpp"
#include "frog_native_engine/CanonicalNumericScalarPalette.hpp"
#include "frog_native_engine/CanonicalNumericComplex.hpp"
#include "frog/semantics/studio_numeric_promotion.hpp"
#include "frog/semantics/numeric_aggregate_contracts.hpp"
#include <array>

namespace frog::native_engine {
namespace {
namespace numeric=frog::runtime::studio_numeric;
bool plainScalar(const binding::TypeRef &type) {
    return type.kind==binding::TypeKind::Primitive&&numeric::supportedPhysicalType(type.name)&&
        binding::fullTypeEqual(type,binding::TypeRef::primitive(type.name));
}
const binding::TypeRef *leaf(const binding::TypeRef &type) {
    if(type.choiceKind!=binding::ChoiceKind::None || !type.choices.empty() || !type.fieldNames.empty())return nullptr;
    if(type.kind==binding::TypeKind::Primitive && type.children.empty() && numeric::supportedPhysicalType(type.name))return &type;
    if(type.kind==binding::TypeKind::Array && type.rank>=1 && type.rank<=16 && type.children.size()==1 &&
        type.children[0].kind==binding::TypeKind::Primitive && leaf(type.children[0]))return &type.children[0];
    return nullptr;
}
}
std::string_view canonicalNumericConversionTarget(std::string_view id) noexcept {
    // Source/type validation also links without the execution service. Share
    // its immutable contract table, without introducing a Runtime link edge.
    for(const auto &conversion:frog::runtime::numeric_aggregate::conversionContracts)
        if(conversion.operationId==id)return conversion.destination;
    return {};
}
std::string_view canonicalNumericAggregateMigrationIdentity(std::string_view id) noexcept {
    auto catalog=id;if(catalog.starts_with("frog.studio."))catalog.remove_prefix(12);
    for(const auto &conversion:frog::runtime::numeric_aggregate::conversionContracts)
        if(catalog==conversion.catalogId)return conversion.operationId;
    // The actual palette Add persists its published primitive name even in a
    // Studio draft. This mapper is used only by draft adaptation; explicit
    // public Source keeps its own frog.core.add contract and never comes here.
    if(id=="numeric.add" || id=="frog.studio.numeric.add" || id=="frog.core.add")return canonicalNumericAggregateAddId;
    if(id=="numeric.absolute_value" || id=="frog.studio.numeric.absolute_value" || id=="frog.core.abs")return canonicalNumericExactAbsId;
    return {};
}
bool canonicalNumericScalarCoercionSupported(const binding::TypeRef &source,const binding::TypeRef &target) noexcept {
    try {return plainScalar(source)&&plainScalar(target)&&source!=target;}catch(...){return false;}
}
std::optional<CanonicalNumericCoercionRecipe> canonicalNumericTreeCoercionRecipe(
    const binding::TypeRef &source,const binding::TypeRef &target) noexcept {
    try {
        std::size_t count=0;
        const auto plain=[&](const auto &self,const binding::TypeRef &t,unsigned depth)->bool {
            if(depth>32||++count>4096||t.choiceKind!=binding::ChoiceKind::None||!t.choices.empty())return false;
            if(t.kind==binding::TypeKind::Primitive)return binding::fullTypeEqual(t,binding::TypeRef::primitive(t.name))&&
                (numeric::supportedPhysicalType(t.name)||t.name=="c32"||t.name=="c64");
            if(t.kind==binding::TypeKind::Array)return t.rank>=1&&t.rank<=16&&t.children.size()==1&&self(self,t.children.front(),depth+1);
            if(t.kind!=binding::TypeKind::Cluster||t.children.empty()||t.children.size()>64||t.fieldIds.size()!=t.children.size())return false;
            for(const auto &child:t.children)if(!self(self,child,depth+1))return false;
            return true;
        };
        if(source==target||!plain(plain,source,0))return {};
        count=0;if(!plain(plain,target,0))return {};
        const binding::TypeRef *leaf=&target;
        while(leaf->kind==binding::TypeKind::Array||leaf->kind==binding::TypeKind::Cluster)leaf=&leaf->children.front();
        std::vector<CanonicalNumericAggregatePort> ports{{"value",source,true,true},{"result",target,false,true}};
        for(const auto &conversion:runtime::numeric_aggregate::conversionContracts)
            if(conversion.destination==leaf->name&&canonicalNumericConversionTreeSignatureValid(conversion.operationId,ports))
                return CanonicalNumericCoercionRecipe{conversion.operationId,"value"};
        if(leaf->name=="c32"||leaf->name=="c64") {
            const auto *conversion=canonicalNumericComplexContract(leaf->name=="c32"?"numeric.conversion.to_csg":"numeric.conversion.to_cdb");
            if(conversion&&canonicalNumericComplexSignatureValid(conversion->operationId,ports))
                return CanonicalNumericCoercionRecipe{conversion->operationId,"value"};
        }
    }catch(...){}return {};
}
bool canonicalNumericAggregateOperationSupported(std::string_view id) noexcept {
    return id==canonicalNumericAggregateAddId||id==canonicalNumericCoercionId||
        id==canonicalNumericExactAbsId||!canonicalNumericConversionTarget(id).empty();
}
bool canonicalNumericAggregateSignatureValid(std::string_view id,std::span<const CanonicalNumericAggregatePort> ports,
    std::string *message,std::string *at) noexcept {
    const auto refuse=[&](std::string_view why,std::string_view port={}) {if(message)*message=why;if(at)*at=port;return false;};
    try {
        if(id==canonicalNumericAggregateAddId&&std::any_of(ports.begin(),ports.end(),[](const auto &p){return p.input&&(p.type.name=="c32"||p.type.name=="c64");}))
            return canonicalNumericScalarPaletteSignatureValid(id,ports,message,at);
        const auto target=canonicalNumericConversionTarget(id);
        if(!target.empty())return canonicalNumericConversionTreeSignatureValid(id,ports,message,at);
        if(id==canonicalNumericCoercionId||id==canonicalNumericExactAbsId||!target.empty()) {
            const std::string_view inputId=target.empty()?"in":"value";
            if(ports.size()!=2||ports[0].id!=inputId||!ports[0].input||!ports[0].connected||
                ports[1].id!="result"||ports[1].input||!plainScalar(ports[0].type)||!plainScalar(ports[1].type))
                return refuse("Scalar numeric adaptation requires exactly driven plain numeric in and result connectors.");
            if(id==canonicalNumericExactAbsId&&(!binding::fullTypeEqual(ports[0].type,ports[1].type)||
                (ports[0].type.name!="i8"&&ports[0].type.name!="i16"&&ports[0].type.name!="f32")))
                return refuse("Exact-width Abs requires identical plain i8, i16 or f32 connectors.");
            if(!target.empty() && ports[1].type.name!=target)
                return refuse("An authored numeric conversion must retain its exact destination representation.","result");
            return true;
        }
        if(!canonicalNumericAggregateOperationSupported(id) || ports.size()<3 || ports.size()>17)return refuse("Owned Add requires two through sixteen actual operands and one result.");
        const CanonicalNumericAggregatePort *output=nullptr;std::array<const CanonicalNumericAggregatePort *,16> inputs{};
        std::size_t count=0,rank=0;
        for(const auto &port:ports) {
            if(!leaf(port.type))return refuse("Owned Add admits plain finite numeric primitives or Arrays, with complete nominal schemas prevalidated.",port.id);
            if(!port.input){if(port.id!="result" || output)return refuse("Owned Add has exactly one result connector.",port.id);output=&port;continue;}
            const auto ordinal=canonicalNumericAddInputOrdinal(port.id);
            if(!ordinal || inputs[*ordinal] || !port.connected)return refuse("Owned Add requires unique driven canonical a,b,input_3..16 connectors.",port.id);
            inputs[*ordinal]=&port;++count;
            if(port.type.kind==binding::TypeKind::Array) {if(rank && rank!=port.type.rank)return refuse("Incompatible aggregate shapes or Cluster fields",port.id);rank=port.type.rank;}
        }
        if(!output || count<2 || !inputs[0] || !inputs[1])return refuse("Owned Add requires its actual a,b operands and result.");
        std::string_view result;
        for(const auto *port:inputs)if(port)result=result.empty()?leaf(port->type)->name:numeric::binaryPromotionType(result,leaf(port->type)->name);
        if(!numeric::supportedPhysicalType(result))return refuse("The complete Add type fold requires an unsupported value carrier.");
        auto expected=binding::TypeRef::primitive(result);if(rank)expected=binding::TypeRef::array(std::move(expected),rank);
        if(output->type!=expected)return refuse("Owned Add result must equal the complete ordered promotion of the actual operands.",output->id);
        return true;
    }catch(...){return refuse("Owned Add signature validation failed without admitting a partial schema.");}
}
bool canonicalNumericAggregateShouldUse(std::span<const CanonicalNumericAggregatePort> ports) noexcept {
    if(std::any_of(ports.begin(),ports.end(),[](const auto &p){return p.type.kind==binding::TypeKind::Cluster||(p.type.kind==binding::TypeKind::Array&&!p.type.children.empty()&&p.type.children.front().kind!=binding::TypeKind::Primitive);}))
        return canonicalNumericScalarPaletteSignatureValid(canonicalNumericAggregateAddId,ports);
    if(!canonicalNumericAggregateSignatureValid(canonicalNumericAggregateAddId,ports))return false;
    std::string_view exact;
    for(const auto &p:ports)if(p.input) {
        if(p.type.kind==binding::TypeKind::Array)return true;
        if(p.type.name!="i32" && p.type.name!="u32" && p.type.name!="i64" && p.type.name!="u64" && p.type.name!="f64")return true;
        if(!exact.empty() && exact!=p.type.name)return true;exact=p.type.name;
    }
    return false;
}
}
