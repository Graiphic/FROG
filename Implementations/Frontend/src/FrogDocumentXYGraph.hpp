#pragma once
#include "frog_native_engine/graph/XYGraph.hpp"
namespace frog::native_engine::document_internal {
void deserializeXYGraph(const source::JsonValue &,xy::State &);
[[nodiscard]] std::string serializeXYGraph(const xy::State &);
[[nodiscard]] source::JsonValue writeXYGraphDesign(const xy::Design &);
[[nodiscard]] bool readXYGraphDesign(const source::JsonValue &,xy::Design &,std::string *error=nullptr);
}
