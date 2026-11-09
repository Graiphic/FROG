#pragma once

#include <optional>
#include <span>
#include <string_view>

namespace frog::native_engine {

enum class DiagramStructureFamily {
    Conditional,
    ForLoop,
    WhileLoop,
    TimedLoop,
    Event,
    Disabled,
    ConditionalDisable,
    Sequence,
    InPlace
};

struct DiagramStructureCatalogEntry {
    DiagramStructureFamily family = DiagramStructureFamily::Conditional;
    std::string_view nodeType;
    std::string_view canonicalType;
    std::string_view displayName;
};

[[nodiscard]] std::span<const DiagramStructureCatalogEntry>
diagramStructureCatalog() noexcept;

[[nodiscard]] const DiagramStructureCatalogEntry &
diagramStructureCatalogEntry(DiagramStructureFamily family) noexcept;

[[nodiscard]] std::optional<DiagramStructureFamily>
diagramStructureFamilyForNodeType(std::string_view nodeType) noexcept;

[[nodiscard]] std::optional<DiagramStructureFamily>
diagramStructureFamilyForCanonicalType(std::string_view canonicalType) noexcept;

[[nodiscard]] std::string_view diagramStructureNodeType(
    DiagramStructureFamily family) noexcept;

[[nodiscard]] std::string_view diagramStructureCanonicalType(
    DiagramStructureFamily family) noexcept;

[[nodiscard]] bool isDiagramStructureNodeType(
    std::string_view nodeType) noexcept;

} // namespace frog::native_engine
