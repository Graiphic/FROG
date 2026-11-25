// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "frog_native_engine/ClassProgram.hpp"
#include "frog_native_engine/FrogDraftData.hpp"

namespace frog::native_engine {
struct DraftClassProjection {
    FrogDraftData document;
    std::vector<ClassIssue> issues;
    std::map<std::string,std::string,std::less<>> generatedFrom;
    std::map<std::string,ClassCallTarget,std::less<>> memberCalls;
    bool changed=false;
    [[nodiscard]] bool accepted() const noexcept {return issues.empty();}
};
[[nodiscard]] bool clusterContainsClassValue(const binding::TypeRef &type);
[[nodiscard]] bool draftHasClasses(const FrogDraftData &);
[[nodiscard]] CapturedClassProgram captureDraftClassProgram(const FrogDraftData &,std::string_view uri,
    const DraftDependencyResolver &);
// The same signature owner serves authoring, help and validation. Display names
// never select a class or method. A failure leaves the document editable.
[[nodiscard]] std::optional<std::vector<DiagramCallPort>> classOperationPorts(
    const ClassSourceOperation &,const CapturedClassProgram &,ClassIssue *issue=nullptr);
[[nodiscard]] DraftClassProjection projectDraftClassOperations(const FrogDraftData &,
    const source::JsonValue &original,const CapturedClassProgram &,std::string_view uri,
    std::string_view sha256,const DraftDependencyResolver &,bool root=false);
}
