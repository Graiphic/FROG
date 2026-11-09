#include "frog_native_engine/DiagramStructureCatalog.hpp"
#include "DiagramStructureCatalog.generated.hpp"

namespace frog::native_engine {
std::span<const DiagramStructureCatalogEntry> diagramStructureCatalog() noexcept {
    return generated::structures::catalog;
}

const DiagramStructureCatalogEntry &diagramStructureCatalogEntry(
    DiagramStructureFamily family) noexcept {
    for (const auto &entry : generated::structures::catalog) {
        if (entry.family == family) {
            return entry;
        }
    }
    return generated::structures::catalog.front();
}

std::optional<DiagramStructureFamily> diagramStructureFamilyForNodeType(
    std::string_view nodeType) noexcept {
    for (const auto &entry : generated::structures::catalog) {
        if (entry.nodeType == nodeType) {
            return entry.family;
        }
    }
    return std::nullopt;
}

std::optional<DiagramStructureFamily> diagramStructureFamilyForCanonicalType(
    std::string_view canonicalType) noexcept {
    for (const auto &entry : generated::structures::catalog) {
        if (entry.canonicalType == canonicalType) {
            return entry.family;
        }
    }
    return std::nullopt;
}

std::string_view diagramStructureNodeType(
    DiagramStructureFamily family) noexcept {
    return diagramStructureCatalogEntry(family).nodeType;
}

std::string_view diagramStructureCanonicalType(
    DiagramStructureFamily family) noexcept {
    return diagramStructureCatalogEntry(family).canonicalType;
}

bool isDiagramStructureNodeType(std::string_view nodeType) noexcept {
    return diagramStructureFamilyForNodeType(nodeType).has_value();
}

} // namespace frog::native_engine
