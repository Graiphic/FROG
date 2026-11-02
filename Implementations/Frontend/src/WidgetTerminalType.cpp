#include "frog_native_engine/WidgetInstance.hpp"
#include "frog_native_engine/WidgetFamilyEngine.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include <unordered_set>
namespace frog::native_engine {
namespace {
std::string arrayElementValueType(const WidgetInstance &widget) {
    const auto *state = arrayContainerState(widget);
    if (state == nullptr || state->elementTemplate == nullptr) {
        return "unknown";
    }
    if (!state->elementValueType.empty()) {
        return state->elementValueType;
    }
    if (state->elementTemplate != nullptr &&
        !state->elementTemplate->valueType.empty()) {
        return state->elementTemplate->valueType;
    }
    return "unknown";
}
}
std::string diagramTerminalValueType(const WidgetInstance &widget) {
    // Cluster identity comes from the ordered field schema, not the legacy
    // display spelling (cluster:f64|bool), which loses field IDs and nesting.
    // Use the same type for the visible port and the semantic wire resolver.
    const auto *array = isArrayWidget(widget) ? arrayContainerState(widget) : nullptr;
    const WidgetInstance *element = &widget;
    std::unordered_set<const WidgetInstance *> ancestry;
    while (isArrayWidget(*element)) {
        // A repeated template is invalid; depth itself is not a type limit.
        if (!ancestry.insert(element).second) return "unknown";
        const auto *state = arrayContainerState(*element);
        if (state == nullptr || state->elementTemplate == nullptr) break;
        element = state->elementTemplate.get();
    }
    if (isClusterWidget(widget) || (array != nullptr && isClusterWidget(*element))) {
        const auto parsed = binding::widgetType(widget);
        return parsed.valid() ? parsed.type.canonical() : "unknown";
    }
    if (isPictureWidget(widget)) {
        return "frog.array<u32,rank=2>";
    }
    if (isArrayWidget(widget)) {
        const auto *state = arrayContainerState(widget);
        if (state != nullptr && state->elementTemplate != nullptr) {
            const auto &element = *state->elementTemplate;
            if (isBooleanWidget(element) || isButtonWidget(element)) {
                return "array:bool";
            }
            if (isStringWidget(element)) {
                return "array:string";
            }
            if (isPathWidget(element)) {
                return "array:path";
            }
            if (isEnumWidget(element)) {
                return "array:enum:" + enumRepresentationValueType(element);
            }
            if (isRingWidget(element)) {
                return "array:ring:" + ringRepresentationValueType(element);
            }
        }
        return "array:" + arrayElementValueType(widget);
    }
    if (isNumericWidget(widget)) {
        return widget.valueType.empty() ? "u16" : widget.valueType;
    }
    if (isBooleanWidget(widget) || isButtonWidget(widget)) {
        return "bool";
    }
    if (isStringWidget(widget)) {
        return "string";
    }
    if (isPathWidget(widget)) {
        return "path";
    }
    if (isEnumWidget(widget)) {
        return "enum:" + enumRepresentationValueType(widget);
    }
    if (isRingWidget(widget)) {
        return "ring:" + ringRepresentationValueType(widget);
    }
    return widget.valueType.empty() ? "unknown" : widget.valueType;
}
}
