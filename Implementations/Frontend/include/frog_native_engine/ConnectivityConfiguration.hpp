#pragma once
#include "frog_native_engine/ConnectivityData.hpp"
namespace frog::native_engine {
[[nodiscard]] bool connectivityPythonTypeSupported(const binding::TypeRef &);
[[nodiscard]] std::vector<ConnectivityConfiguredPort> connectivityConfiguredPorts(
    const ConnectivityNodeConfiguration &,std::span<const ConnectivityTypeEvidence> evidence = {});
[[nodiscard]] std::string connectivityAppendParameter(ConnectivityNodeConfiguration &);
[[nodiscard]] std::string connectivityAppendProperty(ConnectivityNodeConfiguration &);
[[nodiscard]] std::string connectivityAppendCallback(ConnectivityNodeConfiguration &);
[[nodiscard]] source::JsonValue serializeConnectivityConfiguration(const ConnectivityNodeConfiguration &);
[[nodiscard]] std::optional<ConnectivityNodeConfiguration> deserializeConnectivityConfiguration(
    const source::JsonValue &,std::string *error = nullptr);
}
