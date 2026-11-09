#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/FunctionPortIdentities.hpp"
#include "frog_native_engine/StringFormatBinding.hpp"
#include <algorithm>
namespace frog::native_engine {
inline std::optional<std::string_view> stringConstantLiteral(const DiagramNode *node) {
    if(node && stringPaletteId(node->type)=="string.constant") return node->text;
    return std::nullopt;
}


inline bool isExpandableStringPalette(const DiagramNode &node) { return stringPaletteId(node.type)=="string.match_regular_expression" || isStringFormat(node.type) || isStringScan(node.type); }
inline unsigned stringPaletteCount(const DiagramNode &node) { return std::clamp(node.stringFunctionDerivedTerminals.value_or(node.stringFunctionTerminals),1U,32U); }


}
