#pragma once
#include "frog_native_engine/ApplicationControlMemberContract.hpp"
#include "frog_native_engine/WidgetInstance.hpp"
#include "frog_native_engine/FrogWidgetData.hpp"
#include <optional>
#include <span>

namespace frog::native_engine {
struct DiagramNode;
struct DiagramEdge;
struct ApplicationControlDiagnostic { std::string code, rowId, message; };

// Declarative member/type contracts only. No object tokens, editor mutation,
// property invocation, window service or execution capability is created here.
[[nodiscard]] std::optional<std::uint32_t> applicationControlRingItemMaximum(
    const binding::TypeRef &valueType) noexcept;
[[nodiscard]] std::vector<std::string> applicationControlReferenceClasses();
[[nodiscard]] std::vector<ApplicationControlMemberDescriptor> applicationControlMembers(
    std::string_view classId,const binding::TypeRef &valueType={},
    const WidgetInstance *presentation=nullptr);
[[nodiscard]] std::optional<ApplicationControlMemberDescriptor> applicationControlMember(
    std::string_view classId,const binding::TypeRef &valueType,std::string_view memberId,
    const WidgetInstance *presentation=nullptr);
[[nodiscard]] binding::TypeRef applicationControlReferenceType(const ApplicationControlNode &);
[[nodiscard]] std::string applicationControlReferenceClass(const WidgetInstance &);
// Read-only provenance follows stable references/fields; it never reads an
// indexed Array cell or guesses identity from a current value or a position.
[[nodiscard]] const WidgetInstance *applicationControlReferencePresentation(
    const DiagramNode &,std::span<const DiagramNode>,std::span<const DiagramEdge>,FrogWidgetDataView);
[[nodiscard]] std::string applicationControlMemberLabel(const ApplicationControlMemberDescriptor &);
[[nodiscard]] std::string applicationControlClassLabel(std::string_view classId,const binding::TypeRef &valueType={});
[[nodiscard]] std::vector<ApplicationControlPort> applicationControlPorts(
    const ApplicationControlNode &,const binding::TypeRef &wiredReferenceType={},
    const WidgetInstance *presentation=nullptr);
[[nodiscard]] std::vector<ApplicationControlDiagnostic> validateApplicationControlNode(
    const ApplicationControlNode &,const binding::TypeRef &wiredReferenceType={},
    const WidgetInstance *presentation=nullptr);
[[nodiscard]] std::string applicationControlPropertyPortId(std::string_view rowId);
}
