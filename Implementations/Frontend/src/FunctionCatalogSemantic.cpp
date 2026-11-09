#include "frog_native_engine/FunctionCatalog.hpp"
#include <algorithm>
#include <array>
#include <functional>
#include <cmath>
#include <limits>
#include <numbers>
#include <unordered_map>
namespace frog::native_engine {
const std::vector<FunctionCatalogEntry> &functionCatalogEntries() noexcept {
    // The catalog owns Studio discovery only. A primitive type is populated
    // solely when the public FROG specification already publishes it.
    static const std::vector<FunctionCatalogEntry> entries{
#define FROG_FUNCTION_ENTRY(...) {__VA_ARGS__},
#include "FunctionCatalogEntries.inc"
#include "../manifests/connectivity/entries.generated.inc"
#include "../manifests/math/entries.inc"
#include "../manifests/file/entries.inc"
#undef FROG_FUNCTION_ENTRY
    };
    return entries;
}



const FunctionCatalogEntry *findFunctionCatalogEntry(std::string_view catalogId) noexcept {
    static const auto entriesById = [] {
        std::unordered_map<std::string_view, const FunctionCatalogEntry *> lookup;
        const auto &entries = functionCatalogEntries();
        lookup.reserve(entries.size() + 8U);
        for (const auto &entry : entries) {
            lookup.emplace(entry.catalogId, &entry);
#define FROG_FUNCTION_ALIAS(alias, canonical) if (entry.catalogId == canonical) lookup.emplace(alias, &entry);
#include "FunctionCatalogLegacyAliases.inc"
#undef FROG_FUNCTION_ALIAS
        }
        return lookup;
    }();
    const auto found = entriesById.find(catalogId);
    return found == entriesById.end() ? nullptr : found->second;
}

bool isFunctionCatalogFolder(const FunctionCatalogEntry &entry) noexcept {
    return entry.kind == FunctionCatalogEntryKind::Folder;
}

bool isFunctionCatalogPlaceable(const FunctionCatalogEntry &entry) noexcept {
    return entry.kind == FunctionCatalogEntryKind::Operation;
}

bool isFunctionCatalogStructureContainer(
    const FunctionCatalogEntry &entry) noexcept {
    return entry.role == FunctionCatalogEntryRole::StructureContainer;
}

std::string functionDiagramNodeType(const FunctionCatalogEntry &entry) {
    if (!isFunctionCatalogPlaceable(entry)) {
        return {};
    }
    if (!entry.publishedPrimitiveType.empty()) {
        return entry.publishedPrimitiveType;
    }
    if (entry.catalogId == "collection.map.to_array" || entry.catalogId == "collection.set.to_array")
        return "frog.studio.structures.for_loop";
    if (entry.catalogId == "collection.map.in_place_access")
        return "frog.studio.structures.in_place_element_structure";
    // INT and Float are creation presets of the existing numeric objects.
    if (entry.catalogId == "numeric.int_constant" ||
        entry.catalogId == "numeric.color_box_constant") {
        return "frog.studio.numeric.numeric_constant";
    }
    if (entry.catalogId == "numeric.int_terminal") {
        return "frog.studio.numeric.numeric_terminal";
    }
    return "frog.studio." + entry.catalogId;
}

const FunctionCatalogEntry *findFunctionCatalogEntryByDiagramNodeType(
    std::string_view nodeType) noexcept {
    if (nodeType.empty()) {
        return nullptr;
    }
    static const auto entriesByNodeType = [] {
        struct NodeTypeHash {
            using is_transparent = void;

            std::size_t operator()(std::string_view value) const noexcept {
                return std::hash<std::string_view>{}(value);
            }
        };
        // Keep owned keys for synthesized and legacy node types, but do not
        // allocate a temporary string on every render/type-resolution lookup.
        std::unordered_map<std::string, const FunctionCatalogEntry *,
            NodeTypeHash, std::equal_to<>> lookup;
        const auto &entries = functionCatalogEntries();
        lookup.reserve(entries.size() + 2U);
        for (const auto &entry : entries) {
            const auto nodeType = functionDiagramNodeType(entry);
            if (!nodeType.empty()) {
                lookup.emplace(nodeType, &entry);
            }
#define FROG_FUNCTION_ALIAS(alias, canonical) if (entry.catalogId == canonical) lookup.emplace("frog.studio." alias, &entry);
#include "FunctionCatalogLegacyAliases.inc"
#undef FROG_FUNCTION_ALIAS
            if (entry.catalogId == "array.constant") {
                lookup.emplace(
                    "frog.studio.numeric.numeric_array_constant",
                    &entry);
                lookup.emplace(
                    "frog.studio.numeric.enum_array_constant",
                    &entry);
                lookup.emplace(
                    "frog.studio.numeric.ring_array_constant",
                    &entry);
                lookup.emplace(
                    "frog.studio.string.string_array_constant",
                    &entry);
                lookup.emplace(
                    "frog.studio.path.path_array_constant",
                    &entry);
            }
        }
        return lookup;
    }();
    const auto found = entriesByNodeType.find(nodeType);
    return found == entriesByNodeType.end() ? nullptr : found->second;
}

const FunctionCatalogEntry *findFunctionCatalogEntryByPrimitiveType(
    std::string_view primitiveType) noexcept {
    if (primitiveType.empty()) {
        return nullptr;
    }
    static const auto entriesByPrimitiveType = [] {
        std::unordered_map<std::string_view, const FunctionCatalogEntry *> lookup;
        const auto &entries = functionCatalogEntries();
        lookup.reserve(entries.size());
        for (const auto &entry : entries) {
            if (!entry.publishedPrimitiveType.empty()) {
                lookup.emplace(entry.publishedPrimitiveType, &entry);
            }
        }
        return lookup;
    }();
    const auto found = entriesByPrimitiveType.find(primitiveType);
    return found == entriesByPrimitiveType.end() ? nullptr : found->second;
}

std::string_view functionArrayPresetElementType(const FunctionCatalogEntry &entry) noexcept {
    if (entry.parentCatalogId != "array.other_constants" &&
        entry.parentCatalogId != "array.other_terminals") return {};
    const auto suffix = std::string_view(entry.catalogId).substr(entry.catalogId.rfind('.') + 1U);
    if (suffix == "double_precision") return "f64";
    if (suffix == "integer") return "i32";
    if (suffix == "boolean") return "bool";
    if (suffix == "string") return "string";
    if (suffix == "waveform") return "cluster<t0:timestamp,dt:f64,Y:array<f64,rank=1>>";
    if (suffix == "timestamp") return "timestamp";
    if (suffix == "path") return "path";
    if (suffix == "cluster") return "cluster<>";
    if (suffix == "error") return "cluster<status:bool,code:i32,source:string>";
    return {};
}










std::optional<double> functionFixedNumericConstantValue(
    const FunctionCatalogEntry &entry) noexcept {
    if (entry.parentCatalogId != "numeric.other_constants") {
        return std::nullopt;
    }

    struct FixedNumericConstant {
        std::string_view catalogId;
        double value;
    };
    static const std::array<FixedNumericConstant, 20> constants{ {
        { "numeric.other_constants.positive_infinity",
          std::numeric_limits<double>::infinity() },
        { "numeric.other_constants.negative_infinity",
          -std::numeric_limits<double>::infinity() },
        { "numeric.other_constants.pi", std::numbers::pi_v<double> },
        { "numeric.other_constants.pi_over_two", std::numbers::pi_v<double> / 2.0 },
        { "numeric.other_constants.two_pi", 2.0 * std::numbers::pi_v<double> },
        { "numeric.other_constants.one_over_pi", 1.0 / std::numbers::pi_v<double> },
        { "numeric.other_constants.e", std::numbers::e_v<double> },
        { "numeric.other_constants.one_over_e", 1.0 / std::numbers::e_v<double> },
        { "numeric.other_constants.machine_epsilon",
          std::numeric_limits<double>::epsilon() },
        { "numeric.other_constants.ln_ten", std::numbers::ln10_v<double> },
        { "numeric.other_constants.ln_two", std::numbers::ln2_v<double> },
        { "numeric.other_constants.ln_pi", std::log(std::numbers::pi_v<double>) },
        { "numeric.other_constants.log10_e", std::numbers::log10e_v<double> },
        { "numeric.other_constants.ideal_gas", 8.31446261815324 },
        { "numeric.other_constants.elementary_charge", 1.602176634e-19 },
        { "numeric.other_constants.avogadro", 6.02214076e23 },
        { "numeric.other_constants.rydberg", 10973731.568157 },
        { "numeric.other_constants.gravitational", 6.67430e-11 },
        { "numeric.other_constants.planck", 6.62607015e-34 },
        { "numeric.other_constants.speed_of_light", 299792458.0 }
    } };
    const auto found = std::find_if(
        constants.begin(),
        constants.end(),
        [&entry](const FixedNumericConstant &constant) {
            return constant.catalogId == entry.catalogId;
        });
    return found == constants.end()
        ? std::nullopt
        : std::optional<double>{ found->value };
}

std::optional<std::string_view> functionFixedStringConstantValue(
    const FunctionCatalogEntry &entry) noexcept {
    if (entry.parentCatalogId != "string.other_constants") {
        return std::nullopt;
    }

    struct FixedStringConstant {
        std::string_view catalogId;
        std::string_view value;
    };
    static constexpr std::array<FixedStringConstant, 6> constants{ {
        { "string.other_constants.empty_string", "" },
        { "string.other_constants.carriage_return", "\r" },
        { "string.other_constants.line_feed", "\n" },
        { "string.other_constants.end_of_line", "\r\n" },
        { "string.other_constants.tab", "\t" },
        { "string.other_constants.space", " " }
    } };
    const auto found = std::find_if(
        constants.begin(),
        constants.end(),
        [&entry](const FixedStringConstant &constant) {
            return constant.catalogId == entry.catalogId;
        });
    return found == constants.end()
        ? std::nullopt
        : std::optional<std::string_view>{ found->value };
}

}
