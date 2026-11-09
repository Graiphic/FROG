#pragma once
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/DocumentDiagnostic.hpp"
#include <unordered_set>
#include <vector>
namespace frog::native_engine {
// Extracted from DocumentReadiness.cpp required-port validation. Call after
// conditional specialization; the optional existing connector set preserves
// the readiness owner's prior edge validation without a second graph walk.
[[nodiscard]] std::vector<DocumentDiagnostic> inspectDraftRequiredInputs(
    FrogDraftDataView,const std::unordered_set<std::string> *connectedPorts=nullptr);
}
