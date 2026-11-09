#pragma once

#include "frog_native_engine/DiagramShapeCatalog.hpp"

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

enum class FunctionFamily {
    Structures,
    Numeric,
    Boolean,
    StringPath,
    Path,
    Array,
    Cluster,
    Variant,
    Comparison,
    Error,
    Timestamp,
    ApplicationControl,
    Connectivity,
    Mathematics,
    File,
    Synchronization,
    DialogUserInterface,
    Collection
};

enum class FunctionPortDirection {
    Input,
    Output
};

enum class FunctionCatalogEntryKind {
    Operation,
    Folder,
    PaletteOnly
};

enum class FunctionCatalogEntryRole {
    Node,
    StructureContainer
};

using FunctionVisualStandard = DiagramShapeStandard;
using FunctionVisualGeometry = DiagramShapeGeometry;

[[nodiscard]] constexpr FunctionVisualGeometry functionVisualGeometry(
    FunctionVisualStandard standard) noexcept {
    return diagramShapeGeometry(standard);
}

struct FunctionPortDescriptor {
    std::string_view portId;
    FunctionPortDirection direction = FunctionPortDirection::Input;
    double normalizedX = 0.0;
    double normalizedY = 0.5;
};

struct FunctionCatalogEntry {
    FunctionFamily family;
    std::string catalogId;
    std::string displayName;
    std::string iconAssetPath;
    std::string publishedPrimitiveType;
    std::string parentCatalogId{};
    FunctionCatalogEntryKind kind = FunctionCatalogEntryKind::Operation;
    FunctionCatalogEntryRole role = FunctionCatalogEntryRole::Node;
    bool paletteVisible = true; // Legacy nodes stay loadable without new placement entries.
};

using FunctionCatalogView = std::span<const FunctionCatalogEntry *const>;

// Stable authoring-contract family identity, independent from UI labels.
// This is the family serialized by the versioned function manifests.
[[nodiscard]] std::string_view functionFamilyContractId(FunctionFamily family) noexcept;
[[nodiscard]] std::string_view functionFamilyDisplayName(FunctionFamily family) noexcept;
[[nodiscard]] const std::vector<FunctionCatalogEntry> &functionCatalogEntries() noexcept;
[[nodiscard]] FunctionCatalogView functionCatalogEntriesForFamily(FunctionFamily family) noexcept;
[[nodiscard]] FunctionCatalogView functionCatalogEntriesForFolder(
    std::string_view folderCatalogId) noexcept;
[[nodiscard]] const FunctionCatalogEntry *findFunctionCatalogEntry(std::string_view catalogId) noexcept;
[[nodiscard]] std::string_view functionArrayPresetElementType(const FunctionCatalogEntry &entry) noexcept;

[[nodiscard]] bool isFunctionCatalogFolder(const FunctionCatalogEntry &entry) noexcept;
[[nodiscard]] bool isFunctionCatalogPlaceable(const FunctionCatalogEntry &entry) noexcept;
[[nodiscard]] bool isFunctionCatalogStructureContainer(
    const FunctionCatalogEntry &entry) noexcept;
[[nodiscard]] std::string functionDiagramNodeType(const FunctionCatalogEntry &entry);
// Palette creation presets only: never use these to infer a saved node's type.
[[nodiscard]] std::string_view functionNumericConstantInitialValueType(
    const FunctionCatalogEntry &entry) noexcept;
[[nodiscard]] std::string_view functionNumericTerminalInitialRepresentation(
    const FunctionCatalogEntry &entry) noexcept;
[[nodiscard]] const FunctionCatalogEntry *findFunctionCatalogEntryByDiagramNodeType(
    std::string_view nodeType) noexcept;
[[nodiscard]] const FunctionCatalogEntry *findFunctionCatalogEntryByPrimitiveType(
    std::string_view primitiveType) noexcept;
[[nodiscard]] std::span<const FunctionPortDescriptor> functionDiagramPorts(
    const FunctionCatalogEntry &entry) noexcept;
struct DiagramNode;
[[nodiscard]] std::span<const FunctionPortDescriptor> functionDiagramPorts(
    const DiagramNode &, const FunctionCatalogEntry &entry) noexcept;
[[nodiscard]] std::optional<double> functionFixedNumericConstantValue(
    const FunctionCatalogEntry &entry) noexcept;
[[nodiscard]] std::optional<std::string_view> functionFixedStringConstantValue(
    const FunctionCatalogEntry &entry) noexcept;
[[nodiscard]] FunctionVisualStandard functionVisualStandard(
    const FunctionCatalogEntry &entry) noexcept;
[[nodiscard]] bool functionUsesStandardCapsuleNode(
    const FunctionCatalogEntry &entry) noexcept;
[[nodiscard]] bool functionUsesStandardSquareNode(
    const FunctionCatalogEntry &entry) noexcept;
[[nodiscard]] bool functionUsesStandardTriangleNode(
    const FunctionCatalogEntry &entry) noexcept;
[[nodiscard]] bool isNumericScalarValueType(std::string_view valueType) noexcept;
[[nodiscard]] bool numericConnectionRequiresCoercion(
    std::string_view sourceType,
    std::string_view targetType) noexcept;
[[nodiscard]] std::string resolveNumericBinaryValueType(
    std::string_view left,
    std::string_view right);

} // namespace frog::native_engine
