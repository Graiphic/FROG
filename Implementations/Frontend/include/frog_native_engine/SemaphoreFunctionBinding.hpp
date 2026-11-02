#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace frog::native_engine {
struct DiagramNode;
struct SemaphoreTypeEvidence {std::string_view portId;binding::TypeRef valueType;};
struct SemaphoreTypeResult {binding::TypeResolution resolution;bool complete=false;};
[[nodiscard]] bool isSemaphoreFunction(std::string_view catalogId) noexcept;
[[nodiscard]] bool isSemaphoreAcquireFunction(std::string_view catalogId) noexcept;
[[nodiscard]] bool semaphoreExecutesOnInputError(std::string_view catalogId) noexcept;
// Fixed signatures, including the unparameterized nominal reference. This
// validates IR evidence; it is not a new generic type-resolution protocol.
// Neutral contract owner; nodeId is used only for diagnostic attribution.
[[nodiscard]] SemaphoreTypeResult resolveSemaphoreFunctionPort(
    std::string_view operationId,std::string_view nodeId,std::string_view portId,
    std::span<const SemaphoreTypeEvidence> evidence={});
[[nodiscard]] std::optional<std::string> semaphoreOptionalInputDefault(
    std::string_view operationId,std::string_view portId);
// Studio adaptation retains the same contract implementation.
[[nodiscard]] SemaphoreTypeResult resolveSemaphoreFunctionPort(const DiagramNode &,
    std::string_view portId,std::span<const SemaphoreTypeEvidence> evidence={});
[[nodiscard]] std::optional<std::string> semaphoreOptionalInputDefault(const DiagramNode &,std::string_view portId);
} // namespace frog::native_engine
