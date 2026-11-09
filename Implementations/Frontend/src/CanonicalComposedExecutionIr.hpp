#pragma once
#include "CanonicalStructuredExecutionIr.hpp"
namespace frog::native_engine::structured_ir {
inline constexpr std::string_view composedSourceProfile="frog.studio.composed-program@1";
[[nodiscard]] bool composed(const FrogSourceDocument &) noexcept;
// Another explicitly versioned admission profile of the same open FIR. The
// independent reader still rederives and compares every field against Source.
[[nodiscard]] std::optional<Draft> buildComposed(const FrogSourceDocument &,
    std::string_view,const CanonicalIrReadOptions &,std::vector<CanonicalIrDiagnostic> *);
}
