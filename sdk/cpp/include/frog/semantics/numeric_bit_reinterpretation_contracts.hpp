#pragma once
// Private immutable metadata, readable by Source without a Runtime link.
#include <array>
#include <string_view>
namespace frog::runtime::numeric_aggregate::bit_reinterpretation {
enum class Kind { Reinterpret, AdaptScalar };
struct Contract {std::string_view id,catalog,input,output,defaultTarget;Kind kind;};
inline constexpr std::array<Contract,2> contracts{{
    {"frog.studio.numeric.reinterpret_number.exact-bits.v1","numeric.data_manipulation.reinterpret_number","data_in","data_out","f64",Kind::Reinterpret},
    {"frog.studio.numeric.adapt_to_type.flat-scalar.v1","numeric.data_manipulation.adapt_to_type","x","result","u32",Kind::AdaptScalar}
}};
inline constexpr const Contract *contract(std::string_view id)noexcept{for(const auto &c:contracts)if(c.id==id)return &c;return nullptr;}
inline constexpr bool supports(std::string_view id)noexcept{return contract(id)!=nullptr;}
inline constexpr unsigned width(std::string_view t)noexcept{
    if(t=="i8"||t=="u8")return 8;if(t=="i16"||t=="u16")return 16;
    if(t=="i32"||t=="u32"||t=="f32")return 32;if(t=="i64"||t=="u64"||t=="f64")return 64;return 0;
}
inline constexpr bool leafSignature(const Contract &c,std::string_view from,std::string_view to)noexcept{
    return width(from)&&width(to)&&(c.kind==Kind::AdaptScalar||width(from)==width(to));
}
inline constexpr bool ieeeOutput(std::string_view id,std::string_view port,std::string_view type)noexcept{
    const auto *c=contract(id);return c&&port==c->output&&(type=="f32"||type=="f64");
}
}
