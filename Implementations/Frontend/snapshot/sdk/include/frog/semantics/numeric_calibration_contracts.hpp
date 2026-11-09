#pragma once
// Immutable private metadata, usable by the Source-only target without a
// link to Runtime, sensor kernels, numeric_tree, LLVM or an editor.
#include <array>
#include <span>
#include <string>
#include <string_view>

namespace frog::runtime::numeric_calibration {
struct InputContract {std::string_view id,leafType,defaultLexeme;bool required=false;};
struct Contract {std::string_view catalogId,operationId;std::span<const std::string_view> inputs;std::string_view output;std::span<const InputContract> metadata;};
inline constexpr std::array<InputContract,5> rtdMetadata{{
    {"voltage","f64",{},true},{"Ro","f64","100"},{"Iex","f64","0.00000015"},{"A","f64","0.00390802"},{"B","f64","-0.000000580195"}
}};
inline constexpr std::array<InputContract,6> thermistorMetadata{{
    {"voltage","f64",{},true},{"excitation_current","f64","0.0001"},{"R1","f64","5000"},{"voltage_reference","f64","2.5"},
    {"temperature_units","i32","0"},{"type_of_excitation","i32","0"}
}};
inline constexpr std::array<InputContract,6> thermocoupleMetadata{{
    {"voltage","f64",{},true},{"CJC_voltage","f64","0"},{"CJC_sensor","i32","0"},
    {"temperature_units","i32","0"},{"thermocouple_type","i32","2"},{"type_of_excitation","i32","0"}
}};
inline constexpr std::array<InputContract,8> strainMetadata{{
    {"voltage","f64",{},true},{"Rg","f64","120"},{"GF","f64","2"},{"v","f64","0"},{"Vex","f64","3.33"},
    {"Vinit","f64","0"},{"Rl","f64","0"},{"bridge_configuration","i32","3"}
}};
template<std::size_t N>constexpr std::array<std::string_view,N> names(const std::array<InputContract,N> &entries){
    std::array<std::string_view,N> result{};for(std::size_t i=0;i<N;++i)result[i]=entries[i].id;return result;
}
inline constexpr auto rtdInputs=names(rtdMetadata);
inline constexpr auto thermistorInputs=names(thermistorMetadata);
inline constexpr auto strainInputs=names(strainMetadata);
inline constexpr auto thermocoupleInputs=names(thermocoupleMetadata);
inline constexpr std::array<Contract,4> definitions{{
    {"numeric.scaling.convert_rtd_reading","frog.studio.numeric.scaling.convert_rtd_reading.cvd-quadratic.v1",rtdInputs,"temperature",rtdMetadata},
    {"numeric.scaling.convert_thermistor_reading","frog.studio.numeric.scaling.convert_thermistor_reading.scxi-steinhart-hart.v1",thermistorInputs,"temperature",thermistorMetadata},
    {"numeric.scaling.convert_strain_gauge_reading","frog.studio.numeric.scaling.convert_strain_gauge_reading.seven-bridges.v1",strainInputs,"strain",strainMetadata},
    {"numeric.scaling.convert_thermocouple_reading","frog.studio.numeric.scaling.convert_thermocouple_reading.nist-its90-cjc.v1",thermocoupleInputs,"temperature",thermocoupleMetadata}
}};
inline std::span<const Contract> contracts()noexcept{return definitions;}
inline const Contract *contract(std::string_view id)noexcept{
    for(const auto &entry:definitions)if(id==entry.catalogId||id==entry.operationId)return &entry;return nullptr;
}
inline bool operationSupported(std::string_view id)noexcept{
    for(const auto &entry:definitions)if(id==entry.operationId)return true;return false;
}
// This describes the exact mathematical leaf ABI, after the provider's
// explicit numeric promotion of an admitted voltage leaf to F64.
inline bool leafSignatureValid(const Contract &spec,std::span<const std::string_view> inputs,
    std::string_view output,std::string *message=nullptr)noexcept{
    const auto fail=[&](const char *why){if(message)*message=why;return false;};
    if(spec.metadata.size()!=spec.inputs.size()||inputs.size()!=spec.inputs.size())return fail("Calibration must preserve each named required/optional connector.");
    if(output!="f64")return fail("Sensor measurements have exact F64 output leaves.");
    for(std::size_t i=0;i<inputs.size();++i)if(inputs[i]!=spec.metadata[i].leafType||spec.metadata[i].id!=spec.inputs[i])
        return fail("Calibration leaves require F64 measurements/parameters and I32 configuration selectors.");
    return true;
}
}
