#pragma once
#include "frog_native_engine/BooleanArrayConversion.hpp"
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog/semantics/numeric_boolean_conversion_contracts.hpp"
#include <functional>

namespace frog::native_engine {
namespace numeric_boolean_source {
namespace bc=runtime::numeric_aggregate::boolean_conversion;
inline std::string_view authored(std::string_view id)noexcept{
    if(id.starts_with("frog.studio."))id.remove_prefix(12);return id;
}
inline bool integer(const binding::TypeRef &t){
    return t.kind==binding::TypeKind::Primitive&&
        (t.name=="i4"||t.name=="u4"||t.name=="i8"||t.name=="u8"||t.name=="i16"||t.name=="u16"||t.name=="i32"||t.name=="u32"||t.name=="i64"||t.name=="u64")&&
        binding::fullTypeEqual(t,binding::TypeRef::primitive(t.name));
}
inline bool plain(const binding::TypeRef &t,std::string_view name){return binding::fullTypeEqual(t,binding::TypeRef::primitive(name));}
}
inline std::string_view canonicalNumericBooleanConversionMigrationIdentity(std::string_view id,
    BooleanArraySignExtensionMode mode=BooleanArraySignExtensionMode::IfOutputIsSigned)noexcept{
    namespace s=numeric_boolean_source;if(s::bc::supports(id))return id;id=s::authored(id);
    if(id=="numeric.conversion.number_to_boolean_array"||id=="boolean.number_to_array"||id=="array.conversion.number_to_boolean_array")return s::bc::contracts[0].id;
    if(id=="numeric.conversion.boolean_array_to_number"||id=="boolean.array_to_number"||id=="array.conversion.boolean_array_to_number")
        return s::bc::contracts[mode==BooleanArraySignExtensionMode::Never?1:mode==BooleanArraySignExtensionMode::Always?3:2].id;
    if(id=="numeric.conversion.boolean_to_integer"||id=="boolean.to_integer")return s::bc::contracts[4].id;
    return {};
}
// Only the existing Array-palette accesses use alternate connector IDs. The
// integrator applies this mapping to both node ports and edge endpoints.
inline std::string_view canonicalNumericBooleanConnectorId(std::string_view authored,std::string_view port)noexcept{
    authored=numeric_boolean_source::authored(authored);
    if(authored=="array.conversion.number_to_boolean_array"){if(port=="number")return "value";if(port=="array")return "result";}
    if(authored=="array.conversion.boolean_array_to_number"){if(port=="array")return "value";if(port=="number")return "result";}
    return port;
}
inline bool canonicalNumericBooleanConversionSignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports,std::string *message=nullptr,std::string *at=nullptr)noexcept{
    try{
        namespace s=numeric_boolean_source;using Kind=binding::TypeKind;const auto *contract=s::bc::contract(id);
        const auto fail=[&](std::string_view why,std::string_view port={}){if(message)*message=why;if(at)*at=port;return false;};
        if(!contract||ports.size()!=2)return fail("A Numeric Boolean conversion requires its one exact value/result contract.");
        const CanonicalNumericAggregatePort *in=nullptr,*out=nullptr;
        for(const auto &p:ports){if(p.input&&p.id=="value"&&p.connected&&!in)in=&p;else if(!p.input&&p.id=="result"&&!out)out=&p;else return fail("A Numeric Boolean conversion has an unknown, duplicate or undriven connector.",p.id);}
        if(!in||!out||!in->type.known()||!out->type.known())return fail("A Numeric Boolean conversion lost its actual source, destination or complete type.");
        const auto boolArray=binding::TypeRef::array(binding::TypeRef::primitive("bool"));
        if(contract->kind==s::bc::Kind::NumberToArray)return (s::integer(in->type)&&binding::fullTypeEqual(out->type,boolArray))||fail("Number To Boolean Array requires an exact 4/8/16/32/64-bit integer and a rank-one plain Boolean Array. Extended/fixed representations need their declared carrier.");
        if(contract->kind==s::bc::Kind::ArrayToNumber)return (binding::fullTypeEqual(in->type,boolArray)&&s::integer(out->type))||fail("Boolean Array To Number requires a rank-one plain Boolean Array and the exact configured 4/8/16/32/64-bit destination.");
        if(binding::isErrorCluster(in->type))return s::plain(out->type,"i16")||fail("Boolean To Integer projects an Error Cluster's Boolean status to exact I16.","result");
        std::function<bool(const binding::TypeRef&,const binding::TypeRef&,unsigned)> sameTree;
        sameTree=[&](const auto &a,const auto &b,unsigned depth){
            if(depth>32)return false;
            if(s::plain(a,"bool"))return s::plain(b,"i16");
            if(a.kind==Kind::Array&&b.kind==Kind::Array&&a.rank>=1&&a.rank<=16&&a.rank==b.rank&&a.extent==b.extent&&a.children.size()==1&&b.children.size()==1)
                return a.fieldIds==b.fieldIds&&a.fieldNames==b.fieldNames&&sameTree(a.children[0],b.children[0],depth+1);
            if(a.kind==Kind::Cluster&&b.kind==Kind::Cluster&&!a.children.empty()&&a.children.size()==b.children.size()&&a.fieldIds==b.fieldIds&&a.fieldNames==b.fieldNames){
                for(std::size_t i=0;i<a.children.size();++i)if(!sameTree(a.children[i],b.children[i],depth+1))return false;return true;}
            return false;
        };
        return sameTree(in->type,out->type,0)||fail("Boolean To Integer preserves the exact Array/Cluster structure and maps only Boolean leaves to I16.");
    }catch(...){if(message)*message="Numeric Boolean Source signature validation failed without admitting a partial type.";return false;}
}
}
