#include "frog_native_engine/WidgetFamilyEngine.hpp"

#include "frog_native_engine/WidgetInstance.hpp"
#include "frog_native_engine/TabControl.hpp"

namespace frog::native_engine {
namespace {

[[nodiscard]] std::optional<std::string> defaultLabelSuffix(
    std::string_view label,
    std::string_view baseLabel) {
    if (label == baseLabel) {
        return std::string{};
    }

    if (label.size() <= baseLabel.size() + 1U ||
        label.compare(0, baseLabel.size(), baseLabel) != 0 ||
        label[baseLabel.size()] != ' ') {
        return std::nullopt;
    }

    const auto suffix = label.substr(baseLabel.size());
    int value = 0;
    for (std::size_t index = 1U; index < suffix.size(); ++index) {
        const char ch = suffix[index];
        if (ch < '0' || ch > '9') {
            return std::nullopt;
        }
        value = (value * 10) + (ch - '0');
    }

    if (value < 2) {
        return std::nullopt;
    }
    return std::string(suffix);
}

bool retargetDefaultLabel(
    WidgetInstance &widget,
    std::string_view controlLabel,
    std::string_view indicatorLabel,
    std::string_view nextLabel) {
    auto suffix = defaultLabelSuffix(widget.label, controlLabel);
    if (!suffix.has_value()) {
        suffix = defaultLabelSuffix(widget.label, indicatorLabel);
    }
    if (!suffix.has_value()) {
        return false;
    }

    widget.label = std::string(nextLabel) + suffix.value();
    return true;
}

bool applyRoleVariant(
    WidgetInstance &widget,
    WidgetRole role,
    std::string_view controlClassId,
    std::string_view indicatorClassId,
    std::string_view controlLabel = {},
    std::string_view indicatorLabel = {}) {
    const auto nextClassId = role == WidgetRole::Indicator
        ? indicatorClassId
        : controlClassId;
    const auto nextRole = widgetRoleName(role);
    if (widget.classId == nextClassId && widget.role == nextRole) {
        return false;
    }

    if (!controlLabel.empty() && !indicatorLabel.empty()) {
        static_cast<void>(retargetDefaultLabel(
            widget,
            controlLabel,
            indicatorLabel,
            role == WidgetRole::Indicator ? indicatorLabel : controlLabel));
    }
    widget.classId = nextClassId;
    widget.role = nextRole;
    return true;
}

bool setPictureDisplayRole(WidgetInstance &widget, WidgetRole role) {
    const bool pictureIdentity =
        widget.classId == "frog.widgets.picture_control" ||
        widget.classId == "frog.widgets.picture_indicator";
    if (!pictureIdentity || role != WidgetRole::Indicator) {
        return false;
    }

    bool changed = retargetDefaultLabel(
        widget,
        "Picture Control",
        "Picture Indicator",
        "Picture Display");
    if (widget.classId != "frog.widgets.picture_indicator" ||
        widget.role != "indicator") {
        changed = true;
    }
    if (widget.valueType != "frog.array<u32,rank=2>") {
        changed = true;
    }
    widget.classId = "frog.widgets.picture_indicator";
    widget.role = "indicator";
    widget.valueType = "frog.array<u32,rank=2>";
    if (!widget.pictureDisplay.has_value()) {
        widget.pictureDisplay = PictureDisplayState{};
        changed = true;
    }
    return changed;
}

bool setArrayRole(WidgetInstance &widget, WidgetRole role) {
    const auto nextRole = widgetRoleName(role);
    bool changed = applyRoleVariant(
        widget,
        role,
        "frog.widgets.array_control",
        "frog.widgets.array_indicator");

    auto *state = arrayContainerState(widget);
    if (state == nullptr) {
        return changed;
    }

    if (!state->elementRole.empty() && state->elementRole != nextRole) {
        state->elementRole = nextRole;
        changed = true;
    }
    if (state->elementTemplate == nullptr) {
        return changed;
    }

    auto &element = *state->elementTemplate;
    const bool elementChanged =
        widgetHasCapability(element, WidgetCapability::ArrayElement) &&
        setWidgetRole(element, role);
    state->elementClassId = element.classId;
    state->elementValueType = element.valueType;
    state->elementRole = element.role;
    return changed || elementChanged;
}

bool setClusterRole(WidgetInstance &widget, WidgetRole role) {
    bool changed = applyRoleVariant(
        widget,
        role,
        "frog.widgets.cluster_control",
        "frog.widgets.cluster_indicator",
        "Cluster Control",
        "Cluster Indicator");
    auto *state = clusterContainerState(widget);
    if (state == nullptr) {
        return changed;
    }
    for (auto &field : state->elements) {
        if (field.widget != nullptr) {
            changed = setWidgetRole(*field.widget, role) || changed;
        }
    }
    return changed;
}

} // namespace

const WidgetDefinition *widgetDefinition(
    const WidgetInstance &widget) noexcept {
    return findWidgetDefinitionByClassId(widget.classId);
}

std::optional<WidgetFamily> widgetBehaviorFamily(
    const WidgetInstance &widget) noexcept {
    const auto *definition = widgetDefinition(widget);
    return definition == nullptr
        ? std::optional<WidgetFamily>{}
        : std::optional<WidgetFamily>{definition->family};
}

bool widgetIsFamily(
    const WidgetInstance &widget,
    WidgetFamily family) noexcept {
    return widgetBehaviorFamily(widget) == family;
}

bool widgetHasCapability(
    const WidgetInstance &widget,
    WidgetCapability capability) noexcept {
    const auto *definition = widgetDefinition(widget);
    return definition != nullptr &&
        widgetDefinitionHasCapability(*definition, capability);
}

std::optional<WidgetRole> widgetRole(
    const WidgetInstance &widget) noexcept {
    if (const auto parsed = parseWidgetRole(widget.role); parsed.has_value()) {
        return parsed;
    }
    const auto *definition = widgetDefinition(widget);
    return definition == nullptr
        ? std::optional<WidgetRole>{}
        : std::optional<WidgetRole>{definition->canonicalRole};
}

bool widgetHasRole(
    const WidgetInstance &widget,
    WidgetRole role) noexcept {
    return widgetRole(widget) == role;
}

bool widgetSupportsRole(
    const WidgetInstance &widget,
    WidgetRole role) noexcept {
    const auto *definition = widgetDefinition(widget);
    return definition != nullptr &&
        widgetDefinitionSupportsRole(*definition, role);
}

bool widgetCanConvertRole(
    const WidgetInstance &widget,
    WidgetRole role) noexcept {
    if (role != WidgetRole::Control && role != WidgetRole::Indicator) {
        return false;
    }
    return widgetHasCapability(widget, WidgetCapability::RoleConversion) &&
        widgetSupportsRole(widget, role);
}

static bool applyWidgetRole(WidgetInstance &widget, WidgetRole role) {
    // Picture Control is a legacy identity that is deliberately absent from
    // the catalog. Keep its one-way normalization at this compatibility edge.
    if (widget.classId == "frog.widgets.picture_control" ||
        widget.classId == "frog.widgets.picture_indicator") {
        return setPictureDisplayRole(widget, role);
    }
    if (role != WidgetRole::Control && role != WidgetRole::Indicator) {
        return false;
    }

    const auto *definition = widgetDefinition(widget);
    if (definition == nullptr ||
        !widgetDefinitionHasCapability(
            *definition,
            WidgetCapability::RoleConversion) ||
        !widgetDefinitionSupportsRole(*definition, role)) {
        return false;
    }

    switch (definition->family) {
    case WidgetFamily::Numeric:
        return applyRoleVariant(
            widget,
            role,
            "frog.widgets.numeric_control",
            "frog.widgets.numeric_indicator",
            "Numeric Control",
            "Numeric Indicator");
    case WidgetFamily::Button: {
        const auto nextRole = widgetRoleName(role);
        if (widget.role == nextRole) {
            return false;
        }
        const auto nextLabel = role == WidgetRole::Indicator
            ? std::string_view{ "Text Button Indicator" }
            : std::string_view{ "Text Button Control" };
        const bool retargetedCurrentLabel = retargetDefaultLabel(
            widget,
            "Text Button Control",
            "Text Button Indicator",
            nextLabel);
        if (!retargetedCurrentLabel) {
            static_cast<void>(retargetDefaultLabel(
                widget,
                "Text Button",
                "Text Indicator",
                nextLabel));
        }
        widget.role = nextRole;
        return true;
    }
    case WidgetFamily::Boolean: {
        const auto nextLabel = role == WidgetRole::Indicator
            ? std::string_view{ "Boolean Indicator" }
            : std::string_view{ "Boolean Control" };
        bool labelChanged = retargetDefaultLabel(
            widget,
            "Boolean Control",
            "Boolean Indicator",
            nextLabel);
        if (!labelChanged) {
            labelChanged = retargetDefaultLabel(
                widget,
                "Square LED",
                "Square LED",
                nextLabel);
        }
        if (!labelChanged) {
            labelChanged = retargetDefaultLabel(
                widget,
                "Round LED",
                "Round LED",
                nextLabel);
        }
        const bool roleChanged = applyRoleVariant(
            widget,
            role,
            "frog.widgets.boolean_control",
            "frog.widgets.boolean_indicator");
        return labelChanged || roleChanged;
    }
    case WidgetFamily::StringPath:
        if (widget.classId == "frog.widgets.string_control" ||
            widget.classId == "frog.widgets.string_indicator") {
            return applyRoleVariant(
                widget,
                role,
                "frog.widgets.string_control",
                "frog.widgets.string_indicator",
                "String Control",
                "String Indicator");
        }
        return applyRoleVariant(
            widget,
            role,
            "frog.widgets.path_control",
            "frog.widgets.path_indicator",
            "Path Control",
            "Path Indicator");
    case WidgetFamily::Enum:
        if (isTabWidget(widget)) return applyRoleVariant(widget, role,
            kTabControlWidgetClass, kTabIndicatorWidgetClass);
        return applyRoleVariant(
            widget,
            role,
            "frog.widgets.enum_control",
            "frog.widgets.enum_indicator",
            "Enum Control",
            "Enum Indicator");
    case WidgetFamily::Ring:
        return applyRoleVariant(
            widget,
            role,
            "frog.widgets.ring_control",
            "frog.widgets.ring_indicator",
            "Ring Control",
            "Ring Indicator");
    case WidgetFamily::DataContainers:
        if (widget.valueType == "variant" || widget.classId == kReferenceWidgetClass || widget.classId=="frog.studio.widgets.collection") {
            const auto next = widgetRoleName(role);
            if (widget.role == next) return false;
            widget.role = next;
            return true;
        }
        return isClusterWidget(widget)
            ? setClusterRole(widget, role)
            : setArrayRole(widget, role);
    case WidgetFamily::Visualization:
        if (isXYGraphWidget(widget)) {
            const auto next = widgetRoleName(role);
            if (widget.role == next) return false;
            widget.role = next;
            return true;
        }
        return false;
    case WidgetFamily::Label:
        return false;
    }
    return false;
}

bool setWidgetRole(WidgetInstance &widget, WidgetRole role) {
    const bool changed = applyWidgetRole(widget, role);
    if (!changed || !widget.interfaceMapSlotId || widget.interfaceMapSlotId->empty()) {
        return changed;
    }
    // Connector-pane direction follows the widget role, never the physical
    // slot. Keep the stable public identity when converting a bound widget.
    auto &previous = role == WidgetRole::Indicator ? widget.publicInputId : widget.publicOutputId;
    auto &next = role == WidgetRole::Indicator ? widget.publicOutputId : widget.publicInputId;
    if (previous && !previous->empty() && (!next || next->empty())) {
        next = std::move(previous);
        previous.reset();
    }
    return changed;
}

bool widgetIsNumericOrChoice(
    const WidgetInstance &widget) noexcept {
    const auto family = widgetBehaviorFamily(widget);
    return family == WidgetFamily::Numeric ||
        family == WidgetFamily::Ring ||
        family == WidgetFamily::Enum;
}

WidgetTerminalDirection widgetTerminalDirection(
    const WidgetInstance &widget) noexcept {
    const auto *definition = widgetDefinition(widget);
    if (definition == nullptr) {
        return WidgetTerminalDirection::None;
    }
    const auto role = widgetRole(widget).value_or(definition->canonicalRole);
    return widgetTerminalDirectionForRole(*definition, role);
}

bool widgetSupportsDiagramTerminal(
    const WidgetInstance &widget) noexcept {
    return widgetTerminalDirection(widget) != WidgetTerminalDirection::None;
}

} // namespace frog::native_engine
