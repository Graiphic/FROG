#pragma once
#include "frog_native_engine/BindingType.hpp"
#include <optional>
#include <string>
#include <vector>

namespace frog::native_engine {
// Stable semantic identities shared by authored documents and execution plans.
// These types contain no canvas, geometry, editable widget or native resources.
inline constexpr std::string_view kCaseStructureTrueRegionId = "true_case";
inline constexpr std::string_view kCaseStructureFalseRegionId = "false_case";
enum class DiagramTunnelMode { LastValue, Indexing, Concatenating, Invalid };
enum class DiagramTunnelIndexingPolicy { LeadingDimension, AtomicElements, Invalid };

// Matcher lexemes preserve the full integer domain and stable Enum item IDs.
// Region identity is independent of selector values, Default and labels.
struct DiagramCaseStructureMatcher {
    std::string kind = "boolean_exact";
    std::string value = "false";
    std::optional<std::string> lower;
    std::optional<std::string> upper;
};

struct DiagramCaseStructureRegion {
    std::string id;
    std::vector<DiagramCaseStructureMatcher> matches;
    bool isDefault = false;
    std::string title;
    std::optional<std::string> draftText;
};

struct DiagramEdgeEndpoint {
    std::string nodeId;
    std::string portId;
};

struct CaseExecutionDefinition {
    std::string caseStructureSelectorValueType = "bool";
    std::vector<DiagramCaseStructureRegion> caseStructureRegions;
    bool caseStructureManualOrder = false;
    std::vector<binding::ChoiceItemMetadata> caseStructureSelectorEnumItems;
    bool caseStructureStringCaseInsensitive = false;
};
} // namespace frog::native_engine
