#pragma once
#include "frog_native_engine/CanonicalExecutionIr.hpp"

namespace frog::native_engine::structured_ir {
struct Draft {
    std::string bytes,documentId,unitId,hash;
    std::vector<CanonicalIrNode> nodes;
    std::vector<CanonicalIrConnection> connections;
    std::vector<CanonicalIrSourceMap> maps;
    std::vector<CanonicalIrCorrespondence> correspondence;
    std::vector<CanonicalIrRegion> regions;
    std::vector<CanonicalIrStructure> structures;
};
[[nodiscard]] bool required(const FrogSourceDocument &)noexcept;
[[nodiscard]] std::optional<Draft> build(const FrogSourceDocument &,std::string_view documentId,
    const CanonicalIrReadOptions &,std::vector<CanonicalIrDiagnostic> *);
// Semantic comparison ignores object member order but preserves exact numeric
// lexemes. It never repairs an independently supplied artifact.
[[nodiscard]] bool matches(std::string_view,const Draft &,std::vector<CanonicalIrDiagnostic> *);
}
