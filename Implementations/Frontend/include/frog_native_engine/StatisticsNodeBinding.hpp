#pragma once
#include "frog_native_engine/StatisticsBinding.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"
#include "frog_native_engine/FunctionCatalog.hpp"
#include <memory>
namespace frog::native_engine {
struct DiagramNode;
// Immutable semantic/connector projection. The editor selects identities; the
// common Frontend derives the same exact contract for every consumer.
struct StatisticsNodeProjection {
    std::string operationId;
    std::vector<std::string> selectedOutputs;
    FunctionBindingDefinition definition;
    std::vector<FunctionPortDescriptor> ports;
};
[[nodiscard]] const FunctionBindingDefinition *statisticsBindingDefinition(
    std::string_view catalogId,std::string_view variant = {}) noexcept;
void refreshStatisticsNodeProjection(DiagramNode &);
[[nodiscard]] const FunctionBindingDefinition *statisticsBindingDefinition(const DiagramNode &) noexcept;
[[nodiscard]] std::span<const FunctionPortDescriptor> statisticsDiagramPorts(const DiagramNode &) noexcept;
} // namespace frog::native_engine
