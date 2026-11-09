#pragma once
// Private immutable storage shared ONLY by the ordinary and Event-region FIR
// factories. No SDK, Runtime, generic graph factory or mutable public builder.
#include "frog_native_engine/CanonicalExecutionIr.hpp"

namespace frog::native_engine {
struct CanonicalExecutionIr::Storage {
    explicit Storage(const FrogSourceDocument &source):source(source){}
    FrogSourceDocument source;
    std::string original,documentId,unitId,sourceUri,sourceHash;
    ::frog::native_engine::source::JsonValue tree;
    std::vector<CanonicalIrNode> nodes;
    std::vector<CanonicalIrConnection> edges;
    std::vector<CanonicalIrSourceMap> maps;
    std::vector<CanonicalIrCorrespondence> correspondence;
    std::vector<CanonicalIrRegion> regions;
    std::vector<CanonicalIrStructure> structures;
    std::optional<CanonicalIrEventScope> eventScope;
};
}
