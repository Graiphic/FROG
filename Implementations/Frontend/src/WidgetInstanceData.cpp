// SPDX-License-Identifier: Apache-2.0
// Extracted from the existing Studio data owner; no editor or runtime.
#include "frog_native_engine/WidgetInstance.hpp"
#include "frog_native_engine/WidgetPictureData.hpp"
#include "frog_native_engine/NumericColorBox.hpp"
#include "frog_native_engine/WidgetFamilyEngine.hpp"
#include "frog_native_engine/WidgetDataCompatibility.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/TabControl.hpp"
#include "frog_native_engine/NumericValueLexeme.hpp"
#include "frog_native_engine/PreparedWidgetValueAssignment.hpp"
#include "frog_native_engine/RingItemIdentity.hpp"
#include "frog_native_engine/WidgetChoiceProperties.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <utility>
namespace frog::native_engine {

[[nodiscard]] bool ringItemListsEqual(const std::vector<RingItem> &left, const std::vector<RingItem> &right) noexcept {
    if (left.size() != right.size()) {
        return false;
    }
    for (std::size_t index = 0; index < left.size(); ++index) {
        if (left[index].label != right[index].label ||
            left[index].value != right[index].value ||
            left[index].disabled != right[index].disabled ||
            left[index].id != right[index].id ||
            left[index].visible != right[index].visible) {
            return false;
        }
    }
    return true;
}

bool isBooleanWidget(const WidgetInstance &widget) noexcept {
    return widgetIsFamily(widget, WidgetFamily::Boolean);
}

bool setBooleanWidgetIndicatorRole(WidgetInstance &widget, bool indicator) {
    return setWidgetRole(
        widget,
        indicator ? WidgetRole::Indicator : WidgetRole::Control);
}

bool isButtonWidget(const WidgetInstance &widget) noexcept {
    return widgetIsFamily(widget, WidgetFamily::Button);
}

bool isButtonIndicatorWidget(const WidgetInstance &widget) noexcept {
    return isButtonWidget(widget) &&
        widgetHasRole(widget, WidgetRole::Indicator);
}

std::string_view buttonMechanicalAction(const WidgetInstance &widget) noexcept {
    if (!isButtonWidget(widget) && !isBooleanWidget(widget)) {
        return {};
    }

    if (widget.buttonMechanicalAction.has_value()) {
        const auto &action = widget.buttonMechanicalAction.value();
        const std::string_view actionView(action.data(), action.size());
        if (isSupportedButtonMechanicalAction(actionView)) {
            return actionView;
        }
    }
    return kDefaultButtonMechanicalAction;
}

bool setButtonMechanicalAction(WidgetInstance &widget, std::string_view action) {
    if ((!isButtonWidget(widget) && !isBooleanWidget(widget)) ||
        !isSupportedButtonMechanicalAction(action)) {
        return false;
    }

    if (widget.buttonMechanicalAction.has_value()) {
        const auto &current = widget.buttonMechanicalAction.value();
        if (std::string_view(current.data(), current.size()) == action) {
            return false;
        }
    }

    widget.buttonMechanicalAction = std::string(action.data(), action.size());
    return true;
}

bool setButtonWidgetIndicatorRole(WidgetInstance &widget, bool indicator) {
    return setWidgetRole(
        widget,
        indicator ? WidgetRole::Indicator : WidgetRole::Control);
}

bool widgetBooleanValue(const WidgetInstance &widget) noexcept {
    return widget.booleanValue.value_or(false);
}

bool setWidgetBooleanValue(WidgetInstance &widget, bool value) {
    if (!isBooleanWidget(widget) && !isButtonWidget(widget)) {
        return false;
    }

    widget.booleanValue = value;
    widget.initialValue = value ? "true" : "false";
    return true;
}

bool isNumericWidget(const WidgetInstance &widget) noexcept {
    return widgetIsFamily(widget, WidgetFamily::Numeric);
}

bool isNumericControlWidget(const WidgetInstance &widget) noexcept {
    return isNumericWidget(widget) &&
        widgetHasRole(widget, WidgetRole::Control);
}

bool isNumericIndicatorWidget(const WidgetInstance &widget) noexcept {
    return isNumericWidget(widget) &&
        widgetHasRole(widget, WidgetRole::Indicator);
}

bool setNumericWidgetIndicatorRole(WidgetInstance &widget, bool indicator) {
    return setWidgetRole(
        widget,
        indicator ? WidgetRole::Indicator : WidgetRole::Control);
}

bool isStringWidget(const WidgetInstance &widget) noexcept {
    return widget.classId == "frog.widgets.string_control" ||
        widget.classId == "frog.widgets.string_indicator";
}

bool setStringWidgetIndicatorRole(WidgetInstance &widget, bool indicator) {
    return setWidgetRole(
        widget,
        indicator ? WidgetRole::Indicator : WidgetRole::Control);
}

bool isPathWidget(const WidgetInstance &widget) noexcept {
    return widget.classId == "frog.widgets.path_control" ||
        widget.classId == "frog.widgets.path_indicator";
}

bool setPathWidgetIndicatorRole(WidgetInstance &widget, bool indicator) {
    return setWidgetRole(
        widget,
        indicator ? WidgetRole::Indicator : WidgetRole::Control);
}

bool isPictureWidget(const WidgetInstance &widget) noexcept {
    return widget.classId == "frog.widgets.picture_control" ||
        widget.classId == "frog.widgets.picture_indicator";
}

bool isWaveformGraphWidget(const WidgetInstance &widget) noexcept {
    return widget.classId == graph::classId;
}

bool isXYGraphWidget(const WidgetInstance &widget) noexcept {
    return widget.classId == xy::classId;
}

bool isPictureIndicatorWidget(const WidgetInstance &widget) noexcept {
    return isPictureWidget(widget) &&
        widgetHasRole(widget, WidgetRole::Indicator);
}

bool setPictureWidgetIndicatorRole(WidgetInstance &widget, bool indicator) {
    return setWidgetRole(
        widget,
        indicator ? WidgetRole::Indicator : WidgetRole::Control);
}

bool isRingWidget(const WidgetInstance &widget) noexcept {
    return widgetIsFamily(widget, WidgetFamily::Ring);
}

bool isRingControlWidget(const WidgetInstance &widget) noexcept {
    return isRingWidget(widget) &&
        widgetHasRole(widget, WidgetRole::Control);
}

bool isEnumWidget(const WidgetInstance &widget) noexcept {
    return widgetIsFamily(widget, WidgetFamily::Enum);
}

bool isEnumControlWidget(const WidgetInstance &widget) noexcept {
    return isEnumWidget(widget) &&
        widgetHasRole(widget, WidgetRole::Control);
}

bool isArrayWidget(const WidgetInstance &widget) noexcept {
    return widget.classId == "frog.widgets.array" ||
        widget.classId == "frog.widgets.array_control" ||
        widget.classId == "frog.widgets.array_indicator";
}

bool isClusterWidget(const WidgetInstance &widget) noexcept {
    return widget.classId == "frog.widgets.cluster" ||
        widget.classId == "frog.widgets.cluster_control" ||
        widget.classId == "frog.widgets.cluster_indicator";
}

bool supportsPublicInterfaceBinding(const WidgetInstance &widget) noexcept {
    const auto *definition = findWidgetDefinitionByClassId(widget.classId);
    const auto role = parseWidgetRole(widget.role);
    if (definition != nullptr) {
        if (!role.has_value() ||
            !widgetDefinitionSupportsRole(*definition, role.value()) ||
            !widgetDefinitionHasCapability(
                *definition,
                WidgetCapability::PublicInterfaceBinding)) {
            return false;
        }
    } else if (isLabelWidget(widget) ||
               (widget.role != "control" && widget.role != "indicator")) {
        // Unknown extension widgets retain the historical role-based fallback.
        return false;
    }
    if (!isArrayWidget(widget) && !isClusterWidget(widget)) {
        return true;
    }
    if (isArrayWidget(widget)) {
        const auto *state = arrayContainerState(widget);
        return state != nullptr && state->elementTemplate != nullptr;
    }
    const auto *state = clusterContainerState(widget);
    return state != nullptr && !state->elements.empty();
}

bool setArrayWidgetIndicatorRole(WidgetInstance &widget, bool indicator) {
    return setWidgetRole(
        widget,
        indicator ? WidgetRole::Indicator : WidgetRole::Control);
}

bool isSupportedArrayElementWidget(const WidgetInstance &widget) noexcept {
    if (isTabWidget(widget)) return false;
    return widgetHasCapability(widget, WidgetCapability::ArrayElement);
}

const ArrayContainerState *arrayContainerState(const WidgetInstance &widget) noexcept {
    return isArrayWidget(widget) && widget.arrayContainer.has_value()
        ? &widget.arrayContainer.value()
        : nullptr;
}

ArrayContainerState *arrayContainerState(WidgetInstance &widget) noexcept {
    if (!isArrayWidget(widget)) {
        return nullptr;
    }
    if (!widget.arrayContainer.has_value()) {
        widget.arrayContainer = ArrayContainerState{};
    }
    return &widget.arrayContainer.value();
}

const ClusterContainerState *clusterContainerState(
    const WidgetInstance &widget) noexcept {
    return isClusterWidget(widget) && widget.clusterContainer.has_value()
        ? &widget.clusterContainer.value()
        : nullptr;
}

ClusterContainerState *clusterContainerState(WidgetInstance &widget) noexcept {
    if (!isClusterWidget(widget)) {
        return nullptr;
    }
    if (!widget.clusterContainer.has_value()) {
        widget.clusterContainer = ClusterContainerState{};
    }
    return &widget.clusterContainer.value();
}

bool setClusterWidgetIndicatorRole(WidgetInstance &widget, bool indicator) {
    return setWidgetRole(
        widget,
        indicator ? WidgetRole::Indicator : WidgetRole::Control);
}

bool isSupportedClusterElementWidget(const WidgetInstance &widget) noexcept {
    if (isTabWidget(widget)) return false;
    if (isClusterWidget(widget)) {
        const auto *nested = clusterContainerState(widget);
        // Empty nested Clusters are editable containers, just like empty Arrays.
        // Executability is checked by the Diagram type resolver, not this drop predicate.
        return nested != nullptr;
    }
    if (isArrayWidget(widget)) {
        const auto *array = arrayContainerState(widget);
        // An empty Array is an authoring container; its template can be added
        // after the Array itself has been inserted into a Cluster.
        return array != nullptr;
    }
    return widgetSupportsDiagramTerminal(widget) &&
        (widget.role == "control" || widget.role == "indicator");
}

void applyArrayElementWidgetValue(WidgetInstance &target, const WidgetInstance &value) {
    if (binding::widgetType(target).type != binding::widgetType(value).type) return;
    if(target.xyGraph || value.xyGraph) {
        if(!target.xyGraph || !value.xyGraph)return;
        if(target.xyGraph->preservedSource || value.xyGraph->preservedSource)return;
        const auto data=xy::displayedData(*value.xyGraph);
        if(!data || !xy::setInitialData(*target.xyGraph,data->datatype,data->exactSource))return;
    }
    if(target.collection||value.collection) {
        if(!target.collection||!value.collection)return;
        const auto incoming=collectionValue(*value.collection);if(!incoming||!assignCollectionData(*target.collection,*incoming))return;
    }
    target.initialValue = value.initialValue;
    target.numericValue = value.numericValue;
    target.booleanValue = value.booleanValue;
    target.supportTextValue = value.supportTextValue;
    if (isReferenceWidget(target)) target.referenceValue = value.referenceValue;
    if (auto *cluster = clusterContainerState(target)) {
        const auto *source = clusterContainerState(value);
        if (!source) return;
        for (auto &field : cluster->elements) {
            const auto match = std::find_if(source->elements.begin(), source->elements.end(),
                [&](const auto &other) { return other.fieldId == field.fieldId; });
            if (field.widget && match != source->elements.end() && match->widget)
                applyArrayElementWidgetValue(*field.widget, *match->widget);
        }
    } else if (auto *array = arrayContainerState(target)) {
        if (const auto *source = arrayContainerState(value)) {
            array->shape = source->shape;
            array->elements = source->elements;
            if (array->elementTemplate && source->elementTemplate)
                applyArrayElementWidgetValue(*array->elementTemplate, *source->elementTemplate);
        }
    } else if(target.waveformGraph && value.waveformGraph &&
        (value.waveformGraph->preservedSource || graph::validate(value.waveformGraph->initialData).empty())) {
        target.waveformGraph->initialData=value.waveformGraph->initialData;
        target.waveformGraph->current.reset();
        // A historical, unsupported Graph is opaque data, not an empty dataset.
        // Its ordinary full-widget codec can retain the exact source; the new
        // compact value writer separately refuses a reinterpretation.
        target.waveformGraph->preservedSource=value.waveformGraph->preservedSource;
        target.waveformGraph->diagnostic=value.waveformGraph->preservedSource
            ? value.waveformGraph->diagnostic:std::string{};
        ++target.waveformGraph->dataRevision;
    }
}

void materializeArrayScalarElementValue(
    WidgetInstance &widget,
    const ArrayElementValue *value) {
    if (value == nullptr) {
        return;
    }
    if (value->kind == ArrayElementValueKind::Widget) return;
    if (isStringWidget(widget)) {
        static_cast<void>(setWidgetStringValue(widget, value->displayValue));
        return;
    }
    if (isPathWidget(widget)) {
        static_cast<void>(setWidgetPathValue(widget, value->displayValue));
        return;
    }
    if (isBooleanWidget(widget) || isButtonWidget(widget)) {
        static_cast<void>(setWidgetBooleanValue(
            widget,
            value->displayValue == "true" || value->displayValue == "1"));
        return;
    }
    if (isEnumWidget(widget)) {
        const auto &items = ringItems(widget);
        const auto item = std::find_if(items.begin(), items.end(), [value](const auto &candidate) {
            return candidate.id == value->displayValue || candidate.label == value->displayValue;
        });
        if (item != items.end()) {
            static_cast<void>(setRingSelectedValue(widget, item->value));
            return;
        }
    }
    if (value->kind == ArrayElementValueKind::Number) {
        if(widget.runtimeIeeeDisplay && isNumericWidget(widget) &&
           (widget.valueType=="f32" || widget.valueType=="f64")) {
            std::optional<double> special;
            if(value->displayValue=="NaN")special=std::numeric_limits<double>::quiet_NaN();
            else if(value->displayValue=="Inf")special=std::numeric_limits<double>::infinity();
            else if(value->displayValue=="-Inf")special=-std::numeric_limits<double>::infinity();
            if(special){widget.numericValue=*special;widget.initialValue=value->displayValue;return;}
        }
        if (isNumericWidget(widget) && numericCanonicalValueTypeIsComplex(widget.valueType)) {
            static_cast<void>(setWidgetNumericComplexValueLexeme(widget, value->displayValue));
            return;
        }
        if(isNumericWidget(widget) && numericCanonicalValueTypeIsIntegral(widget.valueType)) {
            static_cast<void>(setWidgetNumericIntegerValueLexeme(widget,value->displayValue));
            return;
        }
        char *end = nullptr;
        const double numeric = std::strtod(value->displayValue.c_str(), &end);
        if (end != value->displayValue.c_str() && end != nullptr && *end == '\0' && std::isfinite(numeric)) {
            if (isRingWidget(widget) || isEnumWidget(widget)) {
                if (numeric >= 0.0 && numeric <= static_cast<double>((std::numeric_limits<std::uint32_t>::max)())) {
                    static_cast<void>(setRingSelectedValue(
                        widget,
                        static_cast<std::uint32_t>(std::llround(numeric))));
                }
            } else {
                static_cast<void>(setWidgetNumericValue(widget, numeric));
            }
        }
    }
}

std::optional<ArrayElementValue> prepareArrayScalarElementValue(
    const WidgetInstance &array,
    const std::vector<std::uint32_t> &indices,
    const WidgetInstance &elementWidget) {
    auto *state = arrayContainerState(array);
    if (state == nullptr || indices.size() != state->rank ||
        state->elementTemplate == nullptr ||
        !isSupportedArrayElementWidget(elementWidget)) {
        return std::nullopt;
    }

    const auto &elementTemplate = *state->elementTemplate;
    const bool matchingFamily =
        (isNumericWidget(elementTemplate) && isNumericWidget(elementWidget)) ||
        (isStringWidget(elementTemplate) && isStringWidget(elementWidget)) ||
        (isPathWidget(elementTemplate) && isPathWidget(elementWidget)) ||
        (isBooleanWidget(elementTemplate) && isBooleanWidget(elementWidget)) ||
        (isButtonWidget(elementTemplate) && isButtonWidget(elementWidget)) ||
        (isEnumWidget(elementTemplate) && isEnumWidget(elementWidget)) ||
        (isRingWidget(elementTemplate) && isRingWidget(elementWidget)) ||
        ((isClusterWidget(elementTemplate) || elementTemplate.valueType == "variant") &&
         binding::widgetType(elementTemplate).type == binding::widgetType(elementWidget).type);
    if (!matchingFamily || isClusterWidget(elementWidget) || elementWidget.valueType == "variant") {
        return std::nullopt;
    }

    ArrayElementValue next;
    next.indices = indices;
    next.defined = true;
    if (isNumericWidget(elementWidget)) {
        next.kind = ArrayElementValueKind::Number;
        const double numeric = widgetNumericValue(elementWidget);
        if (numericCanonicalValueTypeIsComplex(elementWidget.valueType)) {
            if (elementWidget.valueType != elementTemplate.valueType) return std::nullopt;
            const auto text = widgetNumericComplexValueLexeme(elementWidget);
            if (!text) return std::nullopt;
            next.displayValue = *text;
        } else if(numericCanonicalValueTypeIsIntegral(elementWidget.valueType)) {
            // A Number cell derives its type from its shared template; this
            // preservation route cannot reinterpret a different scalar type.
            if(elementWidget.valueType!=elementTemplate.valueType)return std::nullopt;
            const auto lexical=widgetNumericIntegerValueLexeme(elementWidget);
            if(!lexical)return std::nullopt;
            next.displayValue=*lexical;
        } else if (std::isfinite(numeric)) {
            // A cell stores data, not its editor's 15-digit presentation.
            // Keep integer lexeme/cache behavior and nonfinite fallback separate.
            std::array<char, 64> number{};
            const auto encoded = std::to_chars(number.data(), number.data() + number.size(),
                numeric, std::chars_format::general);
            if (encoded.ec != std::errc{}) return std::nullopt;
            next.displayValue.assign(number.data(), static_cast<std::size_t>(encoded.ptr - number.data()));
        } else {
            next.displayValue = formatNumericValue(numeric);
        }
    } else if (isBooleanWidget(elementWidget) || isButtonWidget(elementWidget)) {
        next.kind = ArrayElementValueKind::Boolean;
        next.displayValue = widgetBooleanValue(elementWidget) ? "true" : "false";
    } else if (isStringWidget(elementWidget)) {
        next.kind = ArrayElementValueKind::String;
        next.displayValue = std::string(widgetStringValue(elementWidget));
    } else if (isPathWidget(elementWidget)) {
        next.kind = ArrayElementValueKind::String;
        next.displayValue = std::string(widgetPathValue(elementWidget));
    } else if (isEnumWidget(elementWidget)) {
        next.kind = ArrayElementValueKind::String;
        next.displayValue = ringSelectedItemId(elementWidget);
        if (next.displayValue.empty()) {
            next.displayValue = std::to_string(static_cast<std::uint32_t>(
                (std::max)(0.0, elementWidget.numericValue.value_or(0.0))));
        }
    } else if (isRingWidget(elementWidget)) {
        next.kind = ArrayElementValueKind::Number;
        next.displayValue = std::to_string(static_cast<std::uint32_t>(
            (std::max)(0.0, elementWidget.numericValue.value_or(0.0))));
    } else {
        return std::nullopt;
    }

    return next;
}

std::vector<RingItem> defaultRingItems() {
    return {
        RingItem{ "Item 0", 0U, false, "item_1", true }
    };
}

std::vector<RingItem> defaultEnumItems() {
    return {
        RingItem{ "Item 0", 0U, false, "item_1", true }
    };
}

const std::vector<RingItem> &ringItems(const WidgetInstance &widget) noexcept {
    static const std::vector<RingItem> defaultRing = defaultRingItems();
    static const std::vector<RingItem> defaultEnum = defaultEnumItems();
    if (!widget.ringItems.empty() || isStringDropdownWidget(widget)) {
        return widget.ringItems;
    }
    return isEnumWidget(widget) ? defaultEnum : defaultRing;
}

std::string ringSelectedItemLabel(const WidgetInstance &widget) {
    if (!isRingWidget(widget) && !isEnumWidget(widget) && !isStringDropdownWidget(widget)) {
        return {};
    }

    const auto value = static_cast<std::uint32_t>(widget.numericValue.value_or(0.0));
    for (const auto &item : ringItems(widget)) {
        if (item.value == value) {
            return item.label;
        }
    }
    if (ringAllowUndefinedValues(widget)) {
        return std::to_string(value);
    }
    const auto &items = ringItems(widget);
    return items.empty() ? std::string{} : items.front().label;
}

std::string ringSelectedItemId(const WidgetInstance &widget) {
    if (!isRingWidget(widget) && !isEnumWidget(widget) && !isStringDropdownWidget(widget)) {
        return {};
    }

    const auto value = static_cast<std::uint32_t>(widget.numericValue.value_or(0.0));
    const auto &items = ringItems(widget);
    const auto found = std::find_if(items.begin(), items.end(), [value](const RingItem &item) {
        return item.value == value;
    });
    return found == items.end() ? std::string{} : found->id;
}

std::size_t ringSelectedItemIndex(const WidgetInstance &widget) noexcept {
    const auto value = static_cast<std::uint32_t>(widget.numericValue.value_or(0.0));
    const auto &items = ringItems(widget);
    for (std::size_t index = 0; index < items.size(); ++index) {
        if (items[index].value == value) {
            return index;
        }
    }
    return 0U;
}

bool setRingItems(WidgetInstance &widget, std::vector<RingItem> items) {
    const bool dropdown = isStringDropdownWidget(widget);
    if (!isRingWidget(widget) && !isEnumWidget(widget) && !dropdown) {
        return false;
    }

    for (std::size_t index = 0; index < items.size(); ++index) {
        if (!dropdown && items[index].label.empty()) {
            items[index].label = "Item " + std::to_string(index);
        }
    }
    normalizeRingItemIdentities(items);
    if (ringItemListsEqual(widget.ringItems, items)) {
        return false;
    }

    const auto selectedId = dropdown ? ringSelectedItemId(widget) : std::string{};
    const auto selectedValue = widget.numericValue;
    widget.ringItems = std::move(items);
    if (dropdown) {
        // Item edits preserve the selected identity, including duplicate/empty
        // labels. A disabled current item may remain displayed, not selected anew.
        auto selected = std::find_if(widget.ringItems.begin(), widget.ringItems.end(), [&](const auto &item) {
            return selectedId.empty() ? selectedValue == std::optional<double>{double(item.value)} : item.id == selectedId;
        });
        if (selected == widget.ringItems.end()) {
            selected = std::find_if(widget.ringItems.begin(), widget.ringItems.end(),
                [](const auto &item) { return item.visible && !item.disabled; });
            if (selected == widget.ringItems.end()) selected = widget.ringItems.begin();
        }
        if (selected == widget.ringItems.end()) {
            widget.numericValue = 0U; // Existing empty-domain transport sentinel.
            static_cast<void>(setWidgetStringValue(widget, {}));
        } else {
            widget.numericValue = selected->value;
            static_cast<void>(setWidgetStringValue(widget, selected->label));
        }
        return true;
    }
    if (!ringAllowUndefinedValues(widget) && !widget.ringItems.empty()) {
        const auto current = static_cast<std::uint32_t>(widget.numericValue.value_or(0.0));
        const auto found = std::find_if(
            widget.ringItems.begin(),
            widget.ringItems.end(),
            [current](const RingItem &item) {
                return !item.disabled && item.value == current;
            });
        if (found == widget.ringItems.end()) {
            const auto firstEnabled = std::find_if(
                widget.ringItems.begin(),
                widget.ringItems.end(),
                [](const RingItem &item) {
                    return !item.disabled;
                });
            if (firstEnabled != widget.ringItems.end()) {
                widget.numericValue = firstEnabled->value;
                widget.initialValue = std::to_string(firstEnabled->value);
            }
        }
    }
    return true;
}

bool setChoiceItemStrings(WidgetInstance &widget,std::span<const std::string> strings) {
    if(!isRingWidget(widget) && !isEnumWidget(widget) && !isStringDropdownWidget(widget))return false;
    if(!isStringDropdownWidget(widget) && std::any_of(strings.begin(),strings.end(),
        [](const auto &text){return text.empty();}))return false;
    // Position edits preserve existing identities/values/flags. Appended rows
    // receive the next free U32 transport value from the existing item model.
    auto items=widget.ringItems;items.resize(strings.size());
    std::unordered_set<std::uint32_t> used;
    for(std::size_t index=0;index<(std::min)(strings.size(),widget.ringItems.size());++index)
        used.insert(items[index].value);
    std::uint64_t next=0;
    for(std::size_t index=0;index<strings.size();++index) {
        items[index].label=strings[index];
        if(index>=widget.ringItems.size()) {
            while(next<=0xFFFFFFFFULL && used.contains(static_cast<std::uint32_t>(next)))++next;
            if(next>0xFFFFFFFFULL)return false;
            items[index].value=static_cast<std::uint32_t>(next++);used.insert(items[index].value);
        }
        if(isEnumWidget(widget) && items[index].value>numericMaximumForValueType(enumRepresentationValueType(widget)))return false;
    }
    if(isEnumWidget(widget) && !choiceItemLabelsAreUnique(items))return false;
    if(!choiceItemValuesAreUnique(items))return false;
    (void)setRingItems(widget,std::move(items));
    // The existing setter also normalizes empty Numeric labels. Do not claim
    // that an array of empty names was stored unchanged for Enum/Ring.
    return widget.ringItems.size()==strings.size() &&
        std::equal(widget.ringItems.begin(),widget.ringItems.end(),strings.begin(),
            [](const auto &item,const auto &text){return item.label==text;});
}

bool ringAllowUndefinedValues(const WidgetInstance &widget) noexcept {
    return (isRingWidget(widget) || isEnumWidget(widget)) && widget.ringAllowUndefinedValues.value_or(false);
}

bool setRingAllowUndefinedValues(WidgetInstance &widget, bool allow) {
    if (!isRingWidget(widget) && !isEnumWidget(widget)) {
        return false;
    }
    if (ringAllowUndefinedValues(widget) == allow) {
        return false;
    }
    widget.ringAllowUndefinedValues = allow ? std::optional<bool>{ true } : std::optional<bool>{};
    return true;
}

bool setRingSelectedValue(WidgetInstance &widget, std::uint32_t value) {
    if (!isRingWidget(widget) && !isEnumWidget(widget)) {
        return false;
    }

    if (!ringAllowUndefinedValues(widget)) {
        const auto &items = ringItems(widget);
        const auto found = std::find_if(
            items.begin(),
            items.end(),
            [value](const RingItem &item) {
                return !item.disabled && item.value == value;
            });
        if (found == items.end()) {
            return false;
        }
    }

    if (widget.numericValue.has_value() && widget.numericValue.value() == value) {
        return false;
    }
    widget.numericValue = value;
    widget.initialValue = std::to_string(value);
    return true;
}

bool setRingItemDisabled(WidgetInstance &widget, std::uint32_t value, bool disabled) {
    if (!isRingWidget(widget) && !isEnumWidget(widget) && !isStringDropdownWidget(widget)) {
        return false;
    }
    if (widget.ringItems.empty() && !isStringDropdownWidget(widget)) {
        widget.ringItems = defaultRingItems();
    }
    for (auto &item : widget.ringItems) {
        if (item.value != value) {
            continue;
        }
        if (item.disabled == disabled) {
            return false;
        }
        item.disabled = disabled;
        return true;
    }
    return false;
}

bool isLabelWidget(const WidgetInstance &widget) noexcept {
    return widgetIsFamily(widget, WidgetFamily::Label);
}

std::string_view widgetSupportTextValue(const WidgetInstance &widget) noexcept {
    if (!isLabelWidget(widget)) {
        return {};
    }
    if (widget.supportTextValue.has_value()) {
        return widget.supportTextValue.value();
    }
    return widget.label;
}

std::string_view widgetStringValue(const WidgetInstance &widget) noexcept {
    if (!isStringWidget(widget)) {
        return {};
    }
    if (widget.supportTextValue.has_value()) {
        return widget.supportTextValue.value();
    }
    return "abc";
}

std::string_view widgetPathValue(const WidgetInstance &widget) noexcept {
    if (!isPathWidget(widget)) {
        return {};
    }
    if (widget.supportTextValue.has_value()) {
        return widget.supportTextValue.value();
    }
    return "C:/FROG/hello_world.txt";
}

bool isStringDropdownWidget(const WidgetInstance &widget) noexcept {
    return isStringWidget(widget) && widget.stringDropdown;
}

bool setStringDropdownSelectedValue(WidgetInstance &widget, std::uint32_t value) {
    if (!isStringDropdownWidget(widget)) return false;
    const auto found = std::find_if(widget.ringItems.begin(), widget.ringItems.end(),
        [value](const RingItem &item) { return item.value == value && item.visible && !item.disabled; });
    if (found == widget.ringItems.end()) return false;
    const bool changed = widget.numericValue != std::optional<double>{double(value)};
    widget.numericValue = value;
    return setWidgetStringValue(widget, found->label) || changed;
}

bool setWidgetStringValue(WidgetInstance &widget, std::string text) {
    if (!isStringWidget(widget)) {
        return false;
    }
    // Keep the selected identity when duplicate labels are present. Assigning
    // String data never changes the schema or discards the other choices.
    if (isStringDropdownWidget(widget)) {
        const auto found = std::find_if(widget.ringItems.begin(), widget.ringItems.end(),
            [&](const RingItem &item) { return item.label == text &&
                widget.numericValue == std::optional<double>{double(item.value)}; });
        if (found == widget.ringItems.end()) {
            const auto matching = std::find_if(widget.ringItems.begin(), widget.ringItems.end(),
                [&](const RingItem &item) { return item.label == text; });
            if (matching != widget.ringItems.end()) widget.numericValue = matching->value;
        }
    }
    if (text == "abc") {
        if (!widget.supportTextValue.has_value()) {
            return false;
        }
        widget.supportTextValue.reset();
        return true;
    }
    if (widget.supportTextValue.has_value() && widget.supportTextValue.value() == text) {
        return false;
    }
    widget.supportTextValue = std::move(text);
    return true;
}

bool setWidgetPathValue(WidgetInstance &widget, std::string text) {
    if (!isPathWidget(widget)) {
        return false;
    }
    if (text == "C:/FROG/hello_world.txt") {
        if (!widget.supportTextValue.has_value()) {
            return false;
        }
        widget.supportTextValue.reset();
        return true;
    }
    if (widget.supportTextValue.has_value() && widget.supportTextValue.value() == text) {
        return false;
    }
    widget.supportTextValue = std::move(text);
    return true;
}

bool canMigrateContainedArrayValue(const WidgetInstance &target, const WidgetInstance &old) {
    const auto *array = arrayContainerState(target);
    const auto *source = arrayContainerState(old);
    if (!array || !source || !array->elementTemplate || !source->elementTemplate || array->rank != source->rank) return false;
    const auto *targetNested=arrayContainerState(*array->elementTemplate);
    const auto *sourceNested=arrayContainerState(*source->elementTemplate);
    const bool survivingAggregate=(isClusterWidget(*array->elementTemplate) && isClusterWidget(*source->elementTemplate)) ||
        (targetNested && sourceNested && targetNested->rank==sourceNested->rank);
    return survivingAggregate || (isNumericWidget(*array->elementTemplate) && isNumericWidget(*source->elementTemplate)) ||
        binding::widgetType(*array->elementTemplate).type == binding::widgetType(*source->elementTemplate).type;
}


[[nodiscard]] std::optional<bool> initialBooleanValueFor(const WidgetCatalogEntry &entry) noexcept {
    return entry.valueType == "bool" ? std::optional<bool>{ false } : std::nullopt;
}

[[nodiscard]] std::optional<double> initialNumericValueFor(const WidgetCatalogEntry &entry) noexcept {
    if (entry.family == WidgetFamily::Numeric ||
        entry.family == WidgetFamily::Ring ||
        entry.family == WidgetFamily::Enum) {
        return 0.0;
    }
    return std::nullopt;
}

[[nodiscard]] std::optional<double> numericMinimumFor(const WidgetCatalogEntry &) noexcept {
    return std::nullopt;
}

[[nodiscard]] std::optional<double> numericMaximumFor(const WidgetCatalogEntry &) noexcept {
    return std::nullopt;
}

WidgetInstance makeWidgetData(
    std::string instanceId,
    const WidgetCatalogEntry &entry,
    Rect bounds) {
    const auto booleanValue = initialBooleanValueFor(entry);
    const std::string defaultLabel = entry.classId == "frog.widgets.button"
        ? "Text Button Control"
        : entry.family == WidgetFamily::Boolean
            ? (entry.role == "indicator" ? "Boolean Indicator" : "Boolean Control")
        : (entry.classId == "frog.widgets.path_control" || entry.classId == "frog.widgets.path_indicator")
            ? "Path"
            : entry.displayName;
    auto widget = WidgetInstance{
        std::move(instanceId),
        entry.classId,
        entry.role,
        entry.valueType,
        bounds,
        defaultLabel,
        std::optional<std::string>{}, // Independent Front Panel caption.
        std::optional<bool>{},
        entry.family == WidgetFamily::Label
            ? std::optional<std::string>{ entry.displayName }
            : std::optional<std::string>{},
        entry.sourceExamplePath,
        entry.wfrogPackagePath,
        entry.realizationPackageRef,
        entry.assetRef,
        entry.popupAssetRef.empty() ? std::optional<std::string>{} : std::optional<std::string>{ entry.popupAssetRef },
        entry.defaultSvgAssetPath.empty() ? std::optional<std::string>{} : std::optional<std::string>{ entry.defaultSvgAssetPath },
        booleanValue.has_value()
            ? std::optional<std::string>{ booleanValue.value() ? "true" : "false" }
            : std::optional<std::string>{},
        booleanValue,
        initialNumericValueFor(entry),
        numericMinimumFor(entry),
        numericMaximumFor(entry),
        std::optional<double>{},
        std::optional<double>{},
        std::optional<double>{},
        std::optional<double>{},
        std::optional<double>{},
        std::optional<double>{},
        std::optional<double>{},
        std::optional<double>{},
        std::optional<std::string>{},
        std::optional<std::string>{},
        std::optional<std::string>{},
        std::optional<std::string>{},
        std::optional<std::string>{},
        std::optional<std::string>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::string>{},
        std::optional<std::string>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<bool>{},
        std::optional<std::string>{},
        std::optional<std::string>{},
        std::optional<bool>{},
        std::optional<bool>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        std::optional<std::uint32_t>{},
        entry.family == WidgetFamily::Button
            ? std::optional<std::string>{
                std::string(kDefaultButtonMechanicalAction.data(), kDefaultButtonMechanicalAction.size()) }
            : std::optional<std::string>{},
        std::optional<bool>{},
        std::optional<bool>{},
        std::optional<NumericIncrementDecrementSide>{}
    };

    if (entry.family == WidgetFamily::Boolean) {
        widget.buttonStateTextVisible = false;
    }
    if (entry.family == WidgetFamily::Numeric) {
        widget.initialValue = formatNumericValue(widget.numericValue.value_or(0.0));
        if (isColorBoxPaletteEntry(entry)) {
            widget.numericColorBox = true;
            widget.numericRepresentationKind = "uint32";
            widget.bounds = fixedColorBoxBounds(widget.bounds, kColorBoxWidgetSide);
        }
    } else if (entry.family == WidgetFamily::Enum) {
        widget.enumDomainId = "frog.studio." + widget.instanceId;
        widget.ringItems = defaultEnumItems();
        widget.numericValue = 0U;
        widget.initialValue = "item_1";
        widget.numericRepresentationKind = "uint16";
    } else if (entry.family == WidgetFamily::Ring) {
        widget.ringItems = defaultRingItems();
        widget.numericValue = 0U;
        widget.initialValue = "0";
    } else if (entry.classId == "frog.widgets.array_control" ||
               entry.classId == "frog.widgets.array_indicator") {
        widget.arrayContainer = ArrayContainerState{};
    } else if (entry.classId == "frog.widgets.cluster_control" ||
               entry.classId == "frog.widgets.cluster_indicator") {
        widget.clusterContainer = ClusterContainerState{};
    } else if (isXYGraphWidget(widget)) {
        widget.xyGraph.emplace();
        widget.valueType=widget.xyGraph->datatype.canonical();
        widget.labelVisible=true;
    } else if (isWaveformGraphWidget(widget)) {
        widget.waveformGraph = graph::State{};
    } else if (isPictureWidget(widget)) {
        widget.pictureDisplay = PictureDisplayState{};
        widget.pictureDisplay->width=picture_data::axisForBounds(bounds.width);
        widget.pictureDisplay->height=picture_data::axisForBounds(bounds.height);
    }
    if (entry.authoringPreset == WidgetAuthoringPreset::StringDropdown && isStringWidget(widget)) {
        widget.stringDropdown = true;
        widget.ringItems = { RingItem{ "abc", 0U, false, "abc", true } };
        static_cast<void>(setStringDropdownSelectedValue(widget, 0U));
    }
    if(widget.classId==kCollectionWidgetClass)widget.collection=CollectionAuthoringState{};
    if (isTabWidget(widget)) initializeTabControl(widget);
    return widget;
}

namespace data_operations {
bool supported(const WidgetInstance &w) {
    if(w.xyGraph && w.xyGraph->preservedSource)return false;
    return (w.classId==kCollectionWidgetClass&&bool(w.collection)) || isNumericWidget(w) || isBooleanWidget(w) || isButtonWidget(w) || isStringWidget(w) || isPathWidget(w) ||
        isEnumWidget(w) || isRingWidget(w) || isArrayWidget(w) || isClusterWidget(w) || w.valueType == "variant" || isXYGraphWidget(w) || isWaveformGraphWidget(w) || isReferenceWidget(w);
}

bool compatible(const WidgetInstance &target, const WidgetInstance &source) {
    if (!supported(target) || !supported(source)) return false;
    if (isWaveformGraphWidget(target) || isWaveformGraphWidget(source)) return isWaveformGraphWidget(target) && isWaveformGraphWidget(source);
    return values::compatibleTypes(binding::widgetType(target).type, binding::widgetType(source).type,values::FieldPolicy::LogicalOrder);
}

}
bool setEnumWidgetIndicatorRole(WidgetInstance &widget, bool indicator) {
    return setWidgetRole(
        widget,
        indicator ? WidgetRole::Indicator : WidgetRole::Control);
}

}
