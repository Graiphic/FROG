#pragma once
// Exact Graph XY reference identities/signatures; no graph state, font cache,
// image service, widget presentation, document or rendering dependency.
#include "frog_native_engine/ApplicationControlMemberContract.hpp"

namespace frog::native_engine::xy {
inline constexpr char classId[]="frog.widgets.xy_graph";
}
namespace frog::native_engine::xy_reference {
inline constexpr std::string_view presentationKind="frog.graph.xy.presentation@1";
[[nodiscard]] std::vector<ApplicationControlMemberDescriptor> members();
[[nodiscard]] std::optional<ApplicationControlMemberDescriptor> member(std::string_view);
[[nodiscard]] bool supported(std::string_view);
}
