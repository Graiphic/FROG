#pragma once
#include "frog_native_engine/CanonicalNumericAggregate.hpp"
#include "frog/semantics/numeric_calibration_contracts.hpp"
#include "frog/semantics/studio_numeric_promotion.hpp"
#include <algorithm>

namespace frog::native_engine {
inline std::string_view canonicalNumericCalibrationMigrationIdentity(std::string_view id)noexcept{
    namespace cal=runtime::numeric_calibration;if(cal::operationSupported(id))return id;
    if(id.starts_with("frog.studio."))id.remove_prefix(12);
    for(const auto &entry:cal::definitions)if(entry.catalogId==id)return entry.operationId;return {};
}
inline bool canonicalNumericCalibrationSignatureValid(std::string_view id,
    std::span<const CanonicalNumericAggregatePort> ports,std::string *message=nullptr,std::string *at=nullptr)noexcept{
    try{
        namespace cal=runtime::numeric_calibration;using Type=binding::TypeRef;using Kind=binding::TypeKind;
        const auto *spec=cal::contract(id);const auto fail=[&](std::string_view why,std::string_view port={}){if(message)*message=why;if(at)*at=port;return false;};
        if(!spec||!cal::operationSupported(id)||ports.size()!=spec->inputs.size()+1)return fail("Calibration must retain every exact named required/optional connector and its single measurement output.");
        std::array<const CanonicalNumericAggregatePort*,8> inputs{};const CanonicalNumericAggregatePort *output=nullptr;
        for(const auto &p:ports){
            if(!p.type.known())return fail("Calibration has an unresolved complete Source type.",p.id);
            if(!p.input){if(p.id!=spec->output||output)return fail("Calibration has an unknown or duplicate output.",p.id);output=&p;continue;}
            const auto found=std::find(spec->inputs.begin(),spec->inputs.end(),p.id);if(found==spec->inputs.end())return fail("Calibration has an undeclared input.",p.id);
            const auto index=static_cast<std::size_t>(found-spec->inputs.begin());if(inputs[index])return fail("Calibration has a duplicate input.",p.id);inputs[index]=&p;
            if(spec->metadata[index].required&&!p.connected)return fail("Calibration voltage is required and must be connected.",p.id);
            if(index!=0&&!binding::fullTypeEqual(p.type,Type::primitive(spec->metadata[index].leafType)))
                return fail("Calibration parameters must keep their declared scalar F64/I32 types, including when optional and unwired.",p.id);
        }
        if(!output||std::any_of(inputs.begin(),inputs.begin()+spec->inputs.size(),[](const auto *p){return !p;}))return fail("Calibration lost a declared connector.");
        const auto &voltage=inputs[0]->type;const bool array=voltage.kind==Kind::Array;
        if(array&&(voltage.rank!=1||voltage.children.size()!=1))return fail("Sensor voltage supports only a real scalar or one-dimensional real Array.","voltage");
        const auto &leaf=array?voltage.children[0]:voltage;
        if(leaf.kind!=Kind::Primitive||!runtime::studio_numeric::supportedPhysicalType(leaf.name)||
            !binding::fullTypeEqual(leaf,Type::primitive(leaf.name)))
            return fail("Calibration voltage requires an exact plain I8/U8/I16/U16/I32/U32/I64/U64/F32/F64 leaf; nominal, complex, waveform and extended carriers need a separate realization.","voltage");
        auto expected=voltage;
        if(array)expected.children[0]=Type::primitive("f64");else expected=Type::primitive("f64");
        if(!binding::fullTypeEqual(expected,output->type))return fail("Calibration output must retain voltage's exact Array shape schema and produce F64 leaves.",spec->output);
        std::array<std::string_view,8> normalized{};for(std::size_t i=0;i<spec->inputs.size();++i)normalized[i]=spec->metadata[i].leafType;
        return cal::leafSignatureValid(*spec,{normalized.data(),spec->inputs.size()},"f64",message);
    }catch(...){if(message)*message="Calibration Source admission failed without admitting an incomplete signature.";return false;}
}
}
