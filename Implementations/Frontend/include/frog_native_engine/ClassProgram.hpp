// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "frog_native_engine/ClassSource.hpp"

namespace frog::native_engine {
struct FrogDraftData;

// Authored identity, not an access token. Privileges additionally require the
// exact URI and digest captured from the owning .fclass implementation entry.
struct CapturedClassMethod {
    ClassCallTarget declaration;
    DraftDependencyResult source;
    ClassImplementationInterface connector;
    std::shared_ptr<const FrogDraftData> document;
};
struct CapturedClassProgram {
    CapturedClassHierarchy classes;
    std::vector<CapturedClassMethod> methods;
    std::vector<ClassIssue> issues;
    [[nodiscard]] bool accepted() const noexcept {
        return classes.hierarchy.has_value()&&classes.issues.empty()&&issues.empty();
    }
    [[nodiscard]] const CapturedClassMethod *findMember(const ClassCallTarget &) const;
    [[nodiscard]] std::optional<ClassCallTarget> attributeMember(
        std::string_view canonicalUri,std::string_view sha256) const;
};
// Captures actual member bytes and checks their actual declared connectors.
// No host-supplied interface, directory scan or filename-derived identity.
// This is a source contract check; ordinary Frontend admission still checks
// the member body and derives FIR through the existing Sub-FROG projection.
[[nodiscard]] CapturedClassProgram captureClassProgram(
    std::span<const ClassDependencyRoot>,const DraftDependencyResolver &);
}
