#pragma once
// Source-only contract table; no Runtime library linkage or schema parsing.
#include <array>
#include <string_view>
namespace frog::runtime::numeric_aggregate::comparison_tree {
enum class Kind {Equal,NotEqual,Greater,Less,GreaterEqual,LessEqual,EqualZero,NotEqualZero,GreaterZero,LessZero,GreaterEqualZero,LessEqualZero,NotANumberPathRefnum,EmptyArray,EmptyStringPath,DecimalDigit,HexDigit,OctalDigit,WhiteSpace,NotEmptyPath,Select};
struct Contract {
 std::string_view id,catalog;Kind kind;bool aggregate=false;
 std::array<std::string_view,3> inputs;std::array<std::string_view,1> outputs;
};
inline constexpr std::array<Contract,27> contracts{{
 {"frog.studio.comparison.equal.elements.owned.v1","comparison.equal",Kind::Equal,false,{"x","y"},{"result"}},
 {"frog.studio.comparison.equal.aggregates.owned.v1","comparison.equal",Kind::Equal,true,{"x","y"},{"result"}},
 {"frog.studio.comparison.not_equal.elements.owned.v1","comparison.not_equal",Kind::NotEqual,false,{"x","y"},{"result"}},
 {"frog.studio.comparison.not_equal.aggregates.owned.v1","comparison.not_equal",Kind::NotEqual,true,{"x","y"},{"result"}},
 {"frog.studio.comparison.greater.elements.owned.v1","comparison.greater",Kind::Greater,false,{"x","y"},{"result"}},
 {"frog.studio.comparison.greater.aggregates.owned.v1","comparison.greater",Kind::Greater,true,{"x","y"},{"result"}},
 {"frog.studio.comparison.less.elements.owned.v1","comparison.less",Kind::Less,false,{"x","y"},{"result"}},
 {"frog.studio.comparison.less.aggregates.owned.v1","comparison.less",Kind::Less,true,{"x","y"},{"result"}},
 {"frog.studio.comparison.greater_or_equal.elements.owned.v1","comparison.greater_or_equal",Kind::GreaterEqual,false,{"x","y"},{"result"}},
 {"frog.studio.comparison.greater_or_equal.aggregates.owned.v1","comparison.greater_or_equal",Kind::GreaterEqual,true,{"x","y"},{"result"}},
 {"frog.studio.comparison.less_or_equal.elements.owned.v1","comparison.less_or_equal",Kind::LessEqual,false,{"x","y"},{"result"}},
 {"frog.studio.comparison.less_or_equal.aggregates.owned.v1","comparison.less_or_equal",Kind::LessEqual,true,{"x","y"},{"result"}},
 {"frog.studio.comparison.equal_zero.owned.v1","comparison.equal_zero",Kind::EqualZero,false,{"x"},{"result"}},
 {"frog.studio.comparison.not_equal_zero.owned.v1","comparison.not_equal_zero",Kind::NotEqualZero,false,{"x"},{"result"}},
 {"frog.studio.comparison.greater_zero.owned.v1","comparison.greater_zero",Kind::GreaterZero,false,{"x"},{"result"}},
 {"frog.studio.comparison.less_zero.owned.v1","comparison.less_zero",Kind::LessZero,false,{"x"},{"result"}},
 {"frog.studio.comparison.greater_or_equal_zero.owned.v1","comparison.greater_or_equal_zero",Kind::GreaterEqualZero,false,{"x"},{"result"}},
 {"frog.studio.comparison.less_or_equal_zero.owned.v1","comparison.less_or_equal_zero",Kind::LessEqualZero,false,{"x"},{"result"}},
 {"frog.studio.comparison.not_a_number_path_refnum.owned.v1","comparison.not_a_number_path_refnum",Kind::NotANumberPathRefnum,false,{"x"},{"result"}},
 {"frog.studio.comparison.empty_array.owned.v1","comparison.empty_array",Kind::EmptyArray,false,{"x"},{"result"}},
 {"frog.studio.comparison.empty_string_or_path.owned.v1","comparison.empty_string_or_path",Kind::EmptyStringPath,false,{"x"},{"result"}},
 {"frog.studio.comparison.decimal_digit.owned.v1","comparison.decimal_digit",Kind::DecimalDigit,false,{"x"},{"result"}},
 {"frog.studio.comparison.hex_digit.owned.v1","comparison.hex_digit",Kind::HexDigit,false,{"x"},{"result"}},
 {"frog.studio.comparison.octal_digit.owned.v1","comparison.octal_digit",Kind::OctalDigit,false,{"x"},{"result"}},
 {"frog.studio.comparison.white_space.owned.v1","comparison.white_space",Kind::WhiteSpace,false,{"x"},{"result"}},
 {"frog.studio.path.is_not_empty.owned.v1","path.is_not_empty",Kind::NotEmptyPath,false,{"path"},{"result"}},
 {"frog.studio.comparison.select.owned.v1","structures.select",Kind::Select,false,{"select","true","false"},{"result"}}
}};
[[nodiscard]] inline constexpr const Contract *contract(std::string_view id) noexcept {for(const auto &c:contracts)if(c.id==id)return &c;return nullptr;}
[[nodiscard]] inline constexpr const Contract *catalogContract(std::string_view id,bool aggregate=false) noexcept {for(const auto &c:contracts)if(c.catalog==id&&c.aggregate==aggregate)return &c;return nullptr;}
[[nodiscard]] inline constexpr bool ieeeInput(std::string_view id,std::string_view port,std::string_view type) noexcept {
 const auto *c=contract(id);if(!c||(type!="f32"&&type!="f64"))return false;
 if(c->kind==Kind::Select)return port=="true"||port=="false";
 for(const auto input:c->inputs)if(!input.empty()&&port==input)return true;return false;
}
[[nodiscard]] inline constexpr bool ieeeOutput(std::string_view id,std::string_view port,std::string_view type) noexcept {
 const auto *c=contract(id);return c&&c->kind==Kind::Select&&port=="result"&&(type=="f32"||type=="f64");
}
[[nodiscard]] inline constexpr bool supports(std::string_view id) noexcept {return contract(id)!=nullptr;}
[[nodiscard]] inline constexpr bool binary(Kind kind) noexcept {return kind==Kind::Equal||kind==Kind::NotEqual||kind==Kind::Greater||kind==Kind::Less||kind==Kind::GreaterEqual||kind==Kind::LessEqual;}
[[nodiscard]] inline constexpr bool equality(Kind kind) noexcept {return kind==Kind::Equal||kind==Kind::NotEqual||kind==Kind::EqualZero||kind==Kind::NotEqualZero;}
}
