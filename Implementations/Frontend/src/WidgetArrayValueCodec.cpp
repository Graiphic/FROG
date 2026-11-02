// SPDX-License-Identifier: Apache-2.0
// Extracted from the existing Studio data owner; no editor or runtime.
#include "frog_native_engine/FrogDocumentWidgetValues.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/ContainedWidgetValue.hpp"
#include "frog_native_engine/ReferenceWidget.hpp"
#include <algorithm>
#include <limits>
#include <utility>
namespace frog::native_engine {

void materializeArrayElementValue(WidgetInstance &widget, const ArrayElementValue *value) {
    if (!value) return;
    if (value->kind == ArrayElementValueKind::Widget) {
        if (const auto stored = deserializeArrayElementWidget(value->displayValue, &widget))
            applyArrayElementWidgetValue(widget, *stored);
        return;
    }
    materializeArrayScalarElementValue(widget, value);
}

std::optional<ArrayElementValue> prepareArrayElementValueFromWidget(
    const WidgetInstance &array, const std::vector<std::uint32_t> &indices,
    const WidgetInstance &elementWidget) {
    const auto *state = arrayContainerState(array);
    if (!state || indices.size() != state->rank || !state->elementTemplate ||
        !isSupportedArrayElementWidget(elementWidget) ||
        std::any_of(indices.begin(), indices.end(), [](auto index) {
            return index == (std::numeric_limits<std::uint32_t>::max)();
        })) return std::nullopt;
    const bool dropdown = isStringDropdownWidget(elementWidget);
    if (!isClusterWidget(elementWidget) && elementWidget.valueType != "variant" && !dropdown && !isReferenceWidget(elementWidget)&&!elementWidget.collection)
        return prepareArrayScalarElementValue(array, indices, elementWidget);
    const auto &prototype = *state->elementTemplate;
    if (!(isClusterWidget(prototype) || prototype.valueType == "variant" ||
          (dropdown && isStringDropdownWidget(prototype)) ||
          (elementWidget.collection&&prototype.collection) ||
          (isReferenceWidget(elementWidget) && isReferenceWidget(prototype))) ||
        binding::widgetType(prototype).type != binding::widgetType(elementWidget).type)
        return std::nullopt;
    ArrayElementValue next;
    next.indices = indices; next.defined = true; next.kind = ArrayElementValueKind::Widget;
    next.displayValue = serializeArrayElementValueWidget(elementWidget, prototype);
    // Validate BEFORE shape, flags, presentation or history can change.
    if (next.displayValue.empty() || !deserializeArrayElementWidget(next.displayValue, &prototype))
        return std::nullopt;
    // A no-op must not normalize a genuine old full-widget cell into a new
    // compact spelling. Compare its resolved value with the same existing
    // private value codec, then retain the original bytes when values match.
    const auto old = std::find_if(state->elements.begin(),state->elements.end(),
        [&indices](const auto &cell) { return cell.indices==indices; });
    if (dropdown && old!=state->elements.end() && old->defined && old->kind==ArrayElementValueKind::String) {
        // A historical String cell has no independent selected-key payload.
        // Preserve its bytes on a true no-op; a changed key needs the existing
        // compact Widget profile even when both item labels are identical.
        auto previous = prototype;
        materializeArrayScalarElementValue(previous, &*old);
        if (serializeArrayElementValueWidget(previous, prototype)==next.displayValue) return *old;
    }
    if (old!=state->elements.end() && old->defined && old->kind==ArrayElementValueKind::Widget) {
        if (const auto value=deserializeArrayElementWidget(old->displayValue,&prototype)) {
            const auto oldValue=serializeArrayElementValueWidget(*value,prototype);
            if (!oldValue.empty() && oldValue==next.displayValue) next.displayValue=old->displayValue;
        }
    }
    return next;
}

bool setArrayElementTemplate(WidgetInstance &array, const WidgetInstance &element) {
    const auto *state = arrayContainerState(std::as_const(array));
    if (!state || !isSupportedArrayElementWidget(element)) return false;
    // The private writer validates ownership cycles before any recursive model
    // copy of either the current Array or the incoming prototype can occur.
    const auto previousBytes = serializeArrayElementWidget(array);
    const auto incomingBytes = serializeArrayElementWidget(element);
    if (previousBytes.empty() || incomingBytes.empty()) return false;
    auto draft = array;
    if (!installPreparedArrayElementTemplate(draft, element, state->elements)) return false;
    // Reinstalling an identical prototype is not a legacy-cell migration.
    // Keep its existing marker spelling and authored source bytes unchanged.
    const auto preparedBytes = serializeArrayElementWidget(draft);
    if (preparedBytes.empty()) return false;
    if (preparedBytes == previousBytes) return true;
    if (!rebaseContainedWidgetValues(draft, array)) return false;
    array = std::move(draft);
    return true;
}


}
