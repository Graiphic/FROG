#pragma once
// Pure contract metadata for the autonomous Source frontend. No Runtime link.
#include <array>
#include <string_view>
namespace frog::runtime::numeric_aggregate::boolean_tree {
enum class Kind { And, Or, Xor, Not, Nand, Nor, Xnor, Implies, ReduceAnd, ReduceOr };
struct Contract { std::string_view id,catalog;Kind kind;unsigned minimumInputs,maximumInputs; };
inline constexpr std::array<Contract,10> contracts{{
 {"frog.studio.boolean.and.tree.v1","boolean.and",Kind::And,2,16},
 {"frog.studio.boolean.or.tree.v1","boolean.or",Kind::Or,2,16},
 {"frog.studio.boolean.xor.tree.v1","boolean.xor",Kind::Xor,2,16},
 {"frog.studio.boolean.not.tree.v1","boolean.not",Kind::Not,1,1},
 {"frog.studio.boolean.nand.tree.v1","boolean.nand",Kind::Nand,2,16},
 {"frog.studio.boolean.nor.tree.v1","boolean.nor",Kind::Nor,2,16},
 {"frog.studio.boolean.xnor.tree.v1","boolean.xnor",Kind::Xnor,2,16},
 {"frog.studio.boolean.implies.tree.v1","boolean.implies",Kind::Implies,2,2},
 {"frog.studio.boolean.and_array_elements.tree.v1","boolean.and_array_elements",Kind::ReduceAnd,1,1},
 {"frog.studio.boolean.or_array_elements.tree.v1","boolean.or_array_elements",Kind::ReduceOr,1,1}
}};
[[nodiscard]] inline constexpr const Contract *contract(std::string_view id) noexcept {
 for(const auto &c:contracts)if(c.id==id)return &c;return nullptr;
}
[[nodiscard]] inline constexpr const Contract *catalogContract(std::string_view id) noexcept {
 for(const auto &c:contracts)if(c.catalog==id)return &c;return nullptr;
}
[[nodiscard]] inline constexpr bool supports(std::string_view id) noexcept { return contract(id)!=nullptr; }
[[nodiscard]] inline constexpr bool reduction(Kind kind) noexcept { return kind==Kind::ReduceAnd||kind==Kind::ReduceOr; }
}
