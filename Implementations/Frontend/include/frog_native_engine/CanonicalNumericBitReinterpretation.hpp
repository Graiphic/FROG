#pragma once
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog/semantics/numeric_bit_reinterpretation_contracts.hpp"
#include <algorithm>
namespace frog::native_engine {
inline std::string_view canonicalNumericBitReinterpretationMigrationIdentity(std::string_view id)noexcept{
    namespace b=runtime::numeric_aggregate::bit_reinterpretation;if(b::supports(id))return id;
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    for(const auto &c:b::contracts)if(c.catalog==id)return c.id;return {};
}
inline bool canonicalNumericBitReinterpretationSignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports,std::string *message=nullptr,std::string *at=nullptr)noexcept{
    try{namespace b=runtime::numeric_aggregate::bit_reinterpretation;using Type=binding::TypeRef;using Kind=binding::TypeKind;
        const auto fail=[&](std::string_view why,std::string_view port={}){if(message)*message=why;if(at)*at=port;return false;};
        const auto *c=b::contract(id);if(!c||ports.size()!=3)return fail("Bit reinterpretation retains its exact data/type/result connectors, including optional type.");
        const CanonicalNumericAggregatePort *data=nullptr,*target=nullptr,*out=nullptr;
        for(const auto &p:ports){if(p.input&&p.id==c->input&&p.connected&&!data)data=&p;else if(p.input&&p.id=="type"&&!target)target=&p;
            else if(!p.input&&p.id==c->output&&!out)out=&p;else return fail("Bit reinterpretation has an unknown, duplicate or undriven connector.",p.id);}
        if(!data||!target||!out)return fail("Bit reinterpretation lost a required descriptor.");
        const auto plain=[](const Type &t){return t.kind==Kind::Primitive&&b::width(t.name)&&binding::fullTypeEqual(t,Type::primitive(t.name));};
        if(!plain(target->type)||(!target->connected&&target->type.name!=c->defaultTarget))return fail("The type model must be its exact plain numeric scalar, or the declared default when unwired.","type");
        const auto &source=data->type;const bool array=source.kind==Kind::Array;
        if(array&&(c->kind!=b::Kind::Reinterpret||source.rank!=1||source.children.size()!=1))return fail("This realization covers Reinterpret's rank-one numeric Array; Adapt's aggregate layout remains a distinct unfinished contract.",c->input);
        const auto &leaf=array?source.children.front():source;
        if(!plain(leaf)||!b::leafSignature(*c,leaf.name,target->type.name))return fail("Reinterpret requires equal 8/16/32/64-bit plain numeric widths; Adapt scalar uses only established byte-sized primitive layouts.",c->input);
        auto expected=array?source:target->type;if(array)expected.children.front()=target->type;
        return binding::fullTypeEqual(out->type,expected)||fail("Bit reinterpretation lost its exact target representation or rank/extent schema.",c->output);
    }catch(...){if(message)*message="Bit reinterpretation Source admission failed without admitting a partial schema.";return false;}
}
}
