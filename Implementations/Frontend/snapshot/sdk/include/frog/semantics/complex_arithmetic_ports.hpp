#pragma once
// Pure connector/schema validation, shared by Source and the existing service.
// No invocation, WorkBudget, Source parser or external Runtime symbol is used.
#include "frog/semantics/complex_arithmetic_contracts.hpp"
#include <algorithm>
#include <optional>
#include <string>
#include <vector>

namespace frog::runtime::complex_numeric::arithmetic {
[[nodiscard]] inline std::string plainSchema(const rt::ValueType &type){
    if(type!=rt::valueTypeFromCanonicalName(type.canonicalName)||(!complexType(type.canonicalName)&&!studio_numeric::supportedPhysicalType(type.canonicalName)))return {};
    return "{\"canonical\":\""+type.canonicalName+"\",\"children\":[],\"choice_kind\":\"none\",\"choices\":[],\"field_names\":[]}";
}
template<class Port> struct PortSignatureFor {const Spec *operation=nullptr;std::vector<const Port*> inputs;const Port *output=nullptr;};
template<class Port>
[[nodiscard]] inline std::optional<PortSignatureFor<Port>> describePorts(std::string_view id,
    std::span<const Port> ports,std::string *message=nullptr)noexcept{
    const auto reject=[&](std::string_view why)->std::optional<PortSignatureFor<Port>>{if(message)*message=why;return {};};
    try{PortSignatureFor<Port> result;result.operation=spec(id);if(!result.operation||ports.size()<2||ports.size()>17)return reject("Complex arithmetic connector count is outside its actual bounded contract.");
        const auto count=ports.size()-1;if(result.operation->minimumInputs?(count<result.operation->minimumInputs||count>result.operation->inputs.size()):count!=result.operation->inputs.size())return reject("Complex arithmetic lost or acquired a required actual operand.");std::vector<const Port*> ordered(result.operation->inputs.size());
        for(const auto &port:ports){const auto schema=plainSchema(port.type);if(schema.empty()||port.fullTypeJson!=schema)return reject("Complex arithmetic requires complete plain numeric/complex leaves, never erased nominal metadata.");
            if(!port.input){if(result.output||port.id!="result")return reject("Complex arithmetic output identity is not its actual result terminal.");result.output=&port;continue;}
            if(!port.connected)return reject("Complex arithmetic requires every declared operand to be driven.");const auto found=std::find(result.operation->inputs.begin(),result.operation->inputs.end(),port.id);if(found==result.operation->inputs.end())return reject("Complex arithmetic received an undeclared actual connector.");const auto index=static_cast<std::size_t>(found-result.operation->inputs.begin());if(ordered[index])return reject("Complex arithmetic received a duplicated actual connector.");ordered[index]=&port;
        }
        for(std::size_t i=0;i<ordered.size();++i){if(!ordered[i]){if(!result.operation->minimumInputs||i<result.operation->minimumInputs)return reject("Complex arithmetic lost a required operand.");}else result.inputs.push_back(ordered[i]);}
        if(!result.output||result.inputs.size()!=count)return reject("Complex arithmetic lost an actual input/output connector.");
        std::vector<rt::ValueType> inputTypes;inputTypes.reserve(count);for(const auto *port:result.inputs)inputTypes.push_back(port->type);const std::array<rt::ValueType,1> outputTypes{{result.output->type}};if(!signatureValid(id,inputTypes,outputTypes,message))return {};return result;
    }catch(...){return reject("Complex arithmetic connector validation failed without admitting a partial signature.");}
}
}
