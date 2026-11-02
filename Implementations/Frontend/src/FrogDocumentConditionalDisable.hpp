#pragma once
#include "FrogDocumentJsonValue.hpp"
namespace frog::native_engine::document_internal {
// Runs before flattening/deserialization so no comparison or region can be
// dropped by a permissive reader. Empty/default-only structures are distinct.
[[nodiscard]] std::string validateConditionalDisableJson(const JsonValue &root);
void readConditionalDisableCases(const JsonValue &nodeValue, DiagramNode &node);
[[nodiscard]] std::string readCompilationSymbols(const JsonValue &root, FrogSourceProperties &properties);
[[nodiscard]] std::string serializeCompilationSymbols(const FrogSourceProperties &properties);
}
