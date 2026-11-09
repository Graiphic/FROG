#pragma once
// Pure signature projection, shared by FIR and the authoring member catalogue.
#include "frog_native_engine/ApplicationControlTypes.hpp"
#include <span>

namespace frog::native_engine {
[[nodiscard]] std::string applicationControlParameterPortId(
    std::string_view parameterId,ApplicationControlPortDirection);
// Accepting a supplied descriptor never registers it as a production member.
[[nodiscard]] std::vector<ApplicationControlPort> applicationControlMethodPorts(
    const ApplicationControlMemberDescriptor &,
    std::span<const ApplicationControlParameterBinding> parameterBindings = {});
} // namespace frog::native_engine
