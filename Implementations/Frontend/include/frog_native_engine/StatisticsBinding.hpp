#pragma once

#include "frog_native_engine/BindingType.hpp"
#include <span>
#include <string_view>

namespace frog::native_engine {

// Public semantic laws only: no provider, editor, filesystem or runtime state.
struct StatisticsFunctionDescriptor {
    std::string_view catalogId;
    std::string_view operationId;
    std::string_view displayName;
};
struct StatisticsVariantDescriptor {
    std::string_view variantId;
    std::string_view operationId;
    std::string_view displayName;
};
struct StatisticsInputDefault {
    std::string_view portId;
    std::string_view valueType;
    std::string_view jsonValue;
};
struct StatisticsTypeEvidence {
    std::string_view portId;
    binding::TypeRef valueType;
};
struct StatisticsTypeResult {
    binding::TypeResolution resolution;
    bool complete = false;
};

[[nodiscard]] std::span<const StatisticsFunctionDescriptor> statisticsFunctions() noexcept;
[[nodiscard]] const StatisticsFunctionDescriptor *findStatisticsFunction(std::string_view catalogId) noexcept;
[[nodiscard]] bool isStatisticsFunction(std::string_view catalogId) noexcept;
// True only for Statistics Express (the node with selectable measurements).
[[nodiscard]] bool isStatisticsExpressFunction(std::string_view catalogId) noexcept;
[[nodiscard]] std::span<const StatisticsVariantDescriptor> statisticsVariants(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view statisticsOperationId(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const std::string_view> statisticsInputPortIds(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const std::string_view> statisticsOutputPortIds(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::span<const StatisticsInputDefault> statisticsInputDefaults(std::string_view catalogId,std::string_view variant = {}) noexcept;
[[nodiscard]] bool statisticsInputRequired(std::string_view catalogId,std::string_view portId,std::string_view variant = {}) noexcept;
[[nodiscard]] std::string_view statisticsPortDisplayName(std::string_view catalogId,std::string_view portId,std::string_view variant = {}) noexcept;
[[nodiscard]] StatisticsTypeResult resolveStatisticsPort(
    std::string_view catalogId,std::string_view portId,
    std::span<const StatisticsTypeEvidence> inputs = {},std::string_view variant = {});

} // namespace frog::native_engine
