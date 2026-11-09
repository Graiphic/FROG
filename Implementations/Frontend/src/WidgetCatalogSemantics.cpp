// SPDX-License-Identifier: Apache-2.0
// Extracted from the existing Studio data owner; no editor or runtime.
#include "frog_native_engine/WidgetCatalog.hpp"
#include "WidgetCatalog.generated.hpp"
#include <algorithm>
namespace frog::native_engine {

const WidgetDefinition &collectionWidgetDefinition(){static const WidgetDefinition definition{
    "frog.studio.widgets.collection",WidgetFamily::DataContainers,WidgetRole::Control,
    static_cast<WidgetRoleMask>(widgetRoleMask(WidgetRole::Control)|widgetRoleMask(WidgetRole::Indicator)),
    widgetCapabilityMask(WidgetCapability::Label)|widgetCapabilityMask(WidgetCapability::EnabledState)|widgetCapabilityMask(WidgetCapability::RoleConversion)|widgetCapabilityMask(WidgetCapability::DataType)|widgetCapabilityMask(WidgetCapability::PublicInterfaceBinding)|widgetCapabilityMask(WidgetCapability::Font)|widgetCapabilityMask(WidgetCapability::Color)|widgetCapabilityMask(WidgetCapability::ArrayElement),
    widgetPropertyPageMask(WidgetPropertyPage::Appearance)|widgetPropertyPageMask(WidgetPropertyPage::DataType),
    {WidgetTerminalDirection::RoleDerived,"unknown"},WidgetBindingPolicy::RoleDerived,{true,true}};return definition;}

const WidgetDefinition &referenceWidgetDefinition(){static const WidgetDefinition definition{
    kReferenceWidgetClass,WidgetFamily::DataContainers,WidgetRole::Control,
    static_cast<WidgetRoleMask>(widgetRoleMask(WidgetRole::Control)|widgetRoleMask(WidgetRole::Indicator)),
    widgetCapabilityMask(WidgetCapability::Label)|widgetCapabilityMask(WidgetCapability::EnabledState)|widgetCapabilityMask(WidgetCapability::RoleConversion)|widgetCapabilityMask(WidgetCapability::DataType)|widgetCapabilityMask(WidgetCapability::PublicInterfaceBinding)|widgetCapabilityMask(WidgetCapability::Font)|widgetCapabilityMask(WidgetCapability::Color)|widgetCapabilityMask(WidgetCapability::ArrayElement),
    widgetPropertyPageMask(WidgetPropertyPage::Appearance)|widgetPropertyPageMask(WidgetPropertyPage::DataType),
    {WidgetTerminalDirection::RoleDerived,"ref<frog.object>"},WidgetBindingPolicy::RoleDerived,{true,true}};return definition;}

std::string_view widgetRoleName(WidgetRole role) noexcept {
    switch (role) {
    case WidgetRole::Control:
        return "control";
    case WidgetRole::Indicator:
        return "indicator";
    case WidgetRole::Constant:
        return "constant";
    case WidgetRole::Support:
        return "support";
    }
    return "support";
}

std::optional<WidgetRole> parseWidgetRole(std::string_view role) noexcept {
    if (role == "control") {
        return WidgetRole::Control;
    }
    if (role == "indicator" || role == "output") {
        return WidgetRole::Indicator;
    }
    if (role == "constant") {
        return WidgetRole::Constant;
    }
    if (role == "support") {
        return WidgetRole::Support;
    }
    return std::nullopt;
}

WidgetDefinitionView widgetDefinitions() noexcept {
    static const auto definitions=[] {std::vector<WidgetDefinition> list(generated::widgets::definitions.begin(),generated::widgets::definitions.end());list.push_back(referenceWidgetDefinition());list.push_back(collectionWidgetDefinition());return list;}();
    return definitions;
}

const WidgetDefinition *findWidgetDefinitionByClassId(std::string_view classId) noexcept {
    if(classId=="frog.studio.widgets.collection")return &collectionWidgetDefinition();
    if(classId==kReferenceWidgetClass)return &referenceWidgetDefinition();
    for (const auto &[alias, target] : generated::widgets::aliases) {
        if (classId == alias) {
            classId = target;
            break;
        }
    }
    const auto found = std::find_if(
        generated::widgets::definitions.begin(),
        generated::widgets::definitions.end(),
        [classId](const WidgetDefinition &definition) {
            return definition.classId == classId;
        });
    return found == generated::widgets::definitions.end() ? nullptr : &*found;
}

bool widgetDefinitionSupportsRole(
    const WidgetDefinition &definition,
    WidgetRole role) noexcept {
    return (definition.supportedRoles & widgetRoleMask(role)) != 0U;
}

bool widgetDefinitionHasCapability(
    const WidgetDefinition &definition,
    WidgetCapability capability) noexcept {
    return (definition.capabilities & widgetCapabilityMask(capability)) != 0U;
}

bool widgetDefinitionHasPropertyPage(
    const WidgetDefinition &definition,
    WidgetPropertyPage page) noexcept {
    return (definition.propertyPages & widgetPropertyPageMask(page)) != 0U;
}

WidgetTerminalDirection widgetTerminalDirectionForRole(
    const WidgetDefinition &definition,
    WidgetRole role) noexcept {
    if (definition.terminal.direction != WidgetTerminalDirection::RoleDerived) {
        return definition.terminal.direction;
    }
    if (role == WidgetRole::Control) {
        return WidgetTerminalDirection::Output;
    }
    if (role == WidgetRole::Indicator) {
        return WidgetTerminalDirection::Input;
    }
    return WidgetTerminalDirection::None;
}


}
