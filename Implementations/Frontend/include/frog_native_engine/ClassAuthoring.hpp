// SPDX-License-Identifier: Apache-2.0
#pragma once
#include "frog_native_engine/ClassProgram.hpp"
#include "frog_native_engine/FrogDraftData.hpp"

namespace frog::native_engine {
[[nodiscard]] std::optional<WidgetInstance> makeClassPortCarrier(const binding::TypeRef &,
    const std::string &id,bool input,ClassIssue *issue=nullptr);
struct ClassMemberDraft {
    std::optional<FrogDraftData> document;
    std::vector<ClassIssue> issues;
    [[nodiscard]] bool accepted() const noexcept {return document.has_value()&&issues.empty();}
};
// Creates an ordinary editable member with the real declared connector and
// native widget carriers. Its body is intentionally incomplete until authored;
// a default indicator value never substitutes for an unwired method result.
[[nodiscard]] ClassMemberDraft makeClassMemberDraft(const ClassDefinition &,
    std::string_view methodId,const ClassDependency &ownReference);
// Signature specialization retains inherited slot identities while allocating
// distinct public port identities for the new member. No source body is copied.
[[nodiscard]] std::optional<ClassMethod> makeClassOverride(const ClassHierarchy &,
    const ClassDefinition &child,const ClassCallTarget &original,std::string methodId,
    ClassDependency implementation,ClassIssue *issue=nullptr);
// Explicit author action after editing an ordinary member connector. Retained
// ports keep their slots; new ports receive deterministic independent slots.
// Hierarchy/override compatibility remains a separate common validation step.
[[nodiscard]] std::optional<ClassDefinition> adoptClassMemberConnector(
    const ClassDefinition &,std::string_view methodId,const FrogDraftData &,
    std::vector<ClassIssue> &);
}
