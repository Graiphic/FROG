#pragma once
#include "FrogDocumentJsonValue.hpp"
#include "frog_native_engine/graph/WaveformGraph.hpp"
namespace frog::native_engine::document_internal {
void deserializeWaveformGraph(const JsonValue &value, graph::State &state);
[[nodiscard]] std::string serializeWaveformGraph(const graph::State &state);
}
