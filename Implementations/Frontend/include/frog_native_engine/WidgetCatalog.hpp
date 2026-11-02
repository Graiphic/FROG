#pragma once

#include "frog_native_engine/Geometry.hpp"

#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace frog::native_engine {

inline constexpr std::string_view kReferenceWidgetClass="frog.studio.widgets.reference";
inline constexpr std::string_view kCollectionWidgetClass="frog.studio.widgets.collection";

enum class WidgetFamily {
    Numeric,
    Button,
    Boolean,
    StringPath,
    Enum,
    Ring,
    DataContainers,
    Visualization,
    Label
};

enum class WidgetRole : std::uint8_t {
    Control,
    Indicator,
    Constant,
    Support
};

using WidgetRoleMask = std::uint8_t;

enum class WidgetCapability : std::uint64_t {
    EditableValue = 1ULL << 0U,
    Label = 1ULL << 1U,
    EnabledState = 1ULL << 2U,
    RoleConversion = 1ULL << 3U,
    DataType = 1ULL << 4U,
    DataEntry = 1ULL << 5U,
    DisplayFormat = 1ULL << 6U,
    Items = 1ULL << 7U,
    Font = 1ULL << 8U,
    Color = 1ULL << 9U,
    IncrementDecrement = 1ULL << 10U,
    Browse = 1ULL << 11U,
    ArrayContainer = 1ULL << 12U,
    PublicInterfaceBinding = 1ULL << 13U,
    MechanicalAction = 1ULL << 14U,
    ArrayElement = 1ULL << 15U,
    ClusterContainer = 1ULL << 16U
};

using WidgetCapabilityMask = std::uint64_t;

enum class WidgetPropertyPage : std::uint16_t {
    Appearance = 1U << 0U,
    DataType = 1U << 1U,
    DataEntry = 1U << 2U,
    DisplayFormat = 1U << 3U,
    EditItems = 1U << 4U,
    Display = 1U << 5U,
    BrowseOptions = 1U << 6U,
    Operation = 1U << 7U,
    Size = 1U << 8U
};

using WidgetPropertyPageMask = std::uint16_t;

enum class WidgetTerminalDirection : std::uint8_t {
    None,
    Input,
    Output,
    RoleDerived
};

enum class WidgetBindingPolicy : std::uint8_t {
    None,
    PublicInput,
    PublicOutput,
    RoleDerived,
    ElementTypeRequired
};

struct WidgetTerminalSchema {
    WidgetTerminalDirection direction = WidgetTerminalDirection::None;
    std::string_view defaultValueType;
};

struct WidgetDefaults {
    bool labelVisible = true;
    bool enabled = true;
};

// Semantic contract for one widget class. This is deliberately separate from
// WidgetCatalogEntry: several visual navigator entries may realize the same
// class, while every class must have exactly one behavioral definition.
struct WidgetDefinition {
    std::string_view classId;
    WidgetFamily family = WidgetFamily::Label;
    WidgetRole canonicalRole = WidgetRole::Support;
    WidgetRoleMask supportedRoles = 0U;
    WidgetCapabilityMask capabilities = 0U;
    WidgetPropertyPageMask propertyPages = 0U;
    WidgetTerminalSchema terminal;
    WidgetBindingPolicy bindingPolicy = WidgetBindingPolicy::None;
    WidgetDefaults defaults;
};

// Studio-private placement preset; independent of the widget semantic class.
enum class WidgetAuthoringPreset : std::uint8_t { None, StringDropdown };

struct WidgetCatalogEntry {
    WidgetFamily family;
    std::string displayName;
    std::string classId;
    std::string role;
    std::string valueType;
    std::string sourceExamplePath;
    std::string wfrogPackagePath;
    std::string realizationPackageRef;
    std::string assetRef;
    std::string popupAssetRef;
    std::string defaultSvgAssetPath;
    Size defaultPreviewSize;
    WidgetAuthoringPreset authoringPreset = WidgetAuthoringPreset::None;
};

using WidgetCatalogView = std::span<const WidgetCatalogEntry *const>;
using WidgetDefinitionView = std::span<const WidgetDefinition>;

[[nodiscard]] constexpr WidgetRoleMask widgetRoleMask(WidgetRole role) noexcept {
    return static_cast<WidgetRoleMask>(1U << static_cast<unsigned>(role));
}

[[nodiscard]] constexpr WidgetCapabilityMask widgetCapabilityMask(
    WidgetCapability capability) noexcept {
    return static_cast<WidgetCapabilityMask>(capability);
}

[[nodiscard]] constexpr WidgetPropertyPageMask widgetPropertyPageMask(
    WidgetPropertyPage page) noexcept {
    return static_cast<WidgetPropertyPageMask>(page);
}

[[nodiscard]] std::string_view widgetFamilyDisplayName(WidgetFamily family) noexcept;
[[nodiscard]] std::string_view widgetRoleName(WidgetRole role) noexcept;
[[nodiscard]] std::optional<WidgetRole> parseWidgetRole(std::string_view role) noexcept;
[[nodiscard]] const std::vector<WidgetCatalogEntry> &widgetCatalogEntries() noexcept;
[[nodiscard]] WidgetCatalogView widgetCatalogEntriesForFamily(WidgetFamily family) noexcept;
[[nodiscard]] WidgetCatalogView widgetNavigatorEntriesForFamily(WidgetFamily family) noexcept;
[[nodiscard]] const WidgetCatalogEntry *findWidgetCatalogEntryByClassId(std::string_view classId) noexcept;
[[nodiscard]] WidgetDefinitionView widgetDefinitions() noexcept;
[[nodiscard]] const WidgetDefinition *findWidgetDefinitionByClassId(
    std::string_view classId) noexcept;
[[nodiscard]] bool widgetDefinitionSupportsRole(
    const WidgetDefinition &definition,
    WidgetRole role) noexcept;
[[nodiscard]] bool widgetDefinitionHasCapability(
    const WidgetDefinition &definition,
    WidgetCapability capability) noexcept;
[[nodiscard]] bool widgetDefinitionHasPropertyPage(
    const WidgetDefinition &definition,
    WidgetPropertyPage page) noexcept;
[[nodiscard]] WidgetTerminalDirection widgetTerminalDirectionForRole(
    const WidgetDefinition &definition,
    WidgetRole role) noexcept;

} // namespace frog::native_engine
