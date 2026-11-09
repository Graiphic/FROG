#pragma once
#include <array>
#include <string_view>

// Private compiled-service contracts only. This immutable metadata header can
// be read by Source validation without linking a Runtime implementation.
namespace frog::runtime::numeric_aggregate::boolean_conversion {
enum class Kind {NumberToArray,ArrayToNumber,BooleanToInteger};
enum class Extension {Never,IfOutputSigned,Always};
struct Contract {std::string_view id;Kind kind;Extension extension=Extension::IfOutputSigned;};
inline constexpr std::array<Contract,5> contracts{{
    {"frog.studio.numeric.number_to_boolean_array.exact-width.v1",Kind::NumberToArray},
    {"frog.studio.numeric.boolean_array_to_number.sign-never.v1",Kind::ArrayToNumber,Extension::Never},
    {"frog.studio.numeric.boolean_array_to_number.sign-if-signed.v1",Kind::ArrayToNumber,Extension::IfOutputSigned},
    {"frog.studio.numeric.boolean_array_to_number.sign-always.v1",Kind::ArrayToNumber,Extension::Always},
    {"frog.studio.numeric.boolean_to_integer.i16-tree.v1",Kind::BooleanToInteger}
}};
inline const Contract *contract(std::string_view id)noexcept{
    for(const auto &entry:contracts)if(entry.id==id)return &entry;return nullptr;
}
inline bool supports(std::string_view id)noexcept{return contract(id)!=nullptr;}
}
