#pragma once
#include "frog_native_engine/FunctionCatalog.hpp"
#include "frog_native_engine/ConnectivityConfiguration.hpp"
#include <span>
#include <string>
#include <vector>
#include <utility>

namespace frog::native_engine {
struct DiagramNode;
struct ConnectivityPortMetadata {
    std::string id, name, type, description, requirement, defaultValue, unit, source;
    bool output=false;
};
struct ConnectivityInstanceMetadata {
    std::string id, name;
    std::vector<ConnectivityPortMetadata> ports;
};
struct ConnectivityConversionRule {
    int sourceRank=0;
    std::string sourceType;
    std::vector<std::string> targetShapes;
    std::string rule;
};
struct ConnectivityFunctionMetadata {
    std::string key, catalogId, name, description, documentationUrl, signatureStatus, configuration;
    std::string parentCatalogId;
    int row=1, column=1;
    std::vector<ConnectivityPortMetadata> inputs, outputs;
    std::vector<ConnectivityInstanceMetadata> instances;
    std::vector<std::string> limitations;
    std::vector<ConnectivityConversionRule> conversionMatrix;
};
struct ConnectivityPaletteMetadata {
    std::string id, name, parent, description, icon;
    int columns=1, row=1, column=1;
};
[[nodiscard]] std::span<const ConnectivityFunctionMetadata> connectivityFunctions();
[[nodiscard]] std::span<const ConnectivityPaletteMetadata> connectivityPalettes();
[[nodiscard]] const ConnectivityFunctionMetadata *findConnectivityFunction(std::string_view);
[[nodiscard]] const ConnectivityPaletteMetadata *findConnectivityPalette(std::string_view);
// Shared signature selection for the binder, live terminals and Context Help.
// Unknown or ambiguous evidence never selects an arbitrary overload.
[[nodiscard]] const ConnectivityInstanceMetadata *connectivitySelectedInstance(
    const ConnectivityFunctionMetadata &, const DiagramNode &,
    std::span<const ConnectivityTypeEvidence> evidence = {});
[[nodiscard]] std::vector<ConnectivityConfiguredPort> connectivityNodePorts(
    const DiagramNode &, std::span<const ConnectivityTypeEvidence> evidence = {});
// Checks the reviewed source/target shape matrix without converting a value.
// An unwired source uses the selected/default connector shape, not its data.
[[nodiscard]] binding::TypeResolution connectivityDatabaseConversion(
    const DiagramNode &, std::span<const ConnectivityTypeEvidence> evidence = {});
}
