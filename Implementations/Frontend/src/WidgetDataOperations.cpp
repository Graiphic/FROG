// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/WidgetDataOperations.hpp"
#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/FrogDraftData.hpp"
#include "frog_native_engine/FrogDocumentWidgetValues.hpp"
#include "frog_native_engine/WidgetFamilyEngine.hpp"
#include "frog_native_engine/PreparedWidgetValueAssignment.hpp"
#include "frog_native_engine/ContainedWidgetValue.hpp"
#include "FrogJsonInternal.hpp"
#include <algorithm>
#include <cmath>
#include <charconv>
#include <memory>
#include <utility>
namespace frog::native_engine::data_operations {
namespace widget_data_detail {
bool hasInterpretedXY(const WidgetInstance &widget) {
    if(widget.xyGraph)return !widget.xyGraph->preservedSource;
    if(const auto *cluster=clusterContainerState(widget))
        for(const auto &field:cluster->elements)if(field.widget && hasInterpretedXY(*field.widget))return true;
    if(const auto *array=arrayContainerState(widget);array && array->elementTemplate)
        return hasInterpretedXY(*array->elementTemplate);
    return false;
}

bool compatibleWithFieldPolicy(const WidgetInstance &target,const WidgetInstance &source,
    bool useFieldIdentities) {
    if(!useFieldIdentities)return compatible(target,source);
    if(!supported(target) || !supported(source))return false;
    if(isWaveformGraphWidget(target) || isWaveformGraphWidget(source))
        return isWaveformGraphWidget(target) && isWaveformGraphWidget(source);
    const auto targetType=binding::widgetType(target),sourceType=binding::widgetType(source);
    // The schema owner rejects ancestry cycles, invalid IDs/order/tags first.
    return targetType.valid() && sourceType.valid() &&
        values::compatibleTypes(targetType.type,sourceType.type,values::FieldPolicy::FieldIdentity);
}

bool compactPayloadHasNoSavedDefaults(std::string_view payload) {
    // The real decoder has already validated the compact profile. With no
    // escapes or key spelling present, a data.default member cannot exist.
    if(payload.find("data.default")==std::string_view::npos && payload.find('\\')==std::string_view::npos)return true;
    auto parsed=source::internal::parseAuthoringJsonCandidate(payload);
    struct Cleanup {std::optional<source::JsonValue> &root;~Cleanup(){
        if(root)source::internal::clearJsonTree(*root);
    }} cleanup{parsed};
    if(!parsed)return false;
    std::vector<const source::JsonValue *> pending{&*parsed};
    while(!pending.empty()) {
        const auto *value=pending.back();pending.pop_back();
        if(value->object.contains("data.default"))return false;
        for(const auto &[key,child]:value->object)pending.push_back(&child);
        for(const auto &child:value->array)pending.push_back(&child);
    }
    return true;
}

void sanitize(WidgetInstance &widget) {
    widget.defaultData.reset();
    // A standard Type Def identifies the object's schema source, never its
    // copied data/default value. Native object Copy preserves it separately.
    widget.typeDefinition.reset();
    if (auto *cluster = clusterContainerState(widget)) for (auto &field : cluster->elements) if (field.widget) sanitize(*field.widget);
    if (auto *array = arrayContainerState(widget); array && array->elementTemplate) {
        const auto previous=*array->elementTemplate;
        sanitize(*array->elementTemplate);
        // Defaults/design are not cell values. Once the prototype is clean, a
        // validated compact cell can retain its exact bytes and inherit that
        // cleaned design, instead of expanding the same subtree once again
        // for every Array level. A live Graph snapshot can change inherited
        // data during sanitize, so prove those values unchanged before reuse.
        const auto prototypeDelta=serializeArrayElementValueWidget(*array->elementTemplate,previous);
        const bool inheritedValuesUnchanged=!prototypeDelta.empty() && prototypeDelta==
            serializeArrayElementValueWidget(previous,previous);
        std::optional<std::vector<ArrayElementValue>> changedCells;
        for (std::size_t index=0;index<array->elements.size();++index) {
            const auto &cell=array->elements[index];
            if (cell.kind != ArrayElementValueKind::Widget) continue;
            bool compactValueOnly=false;
            if (auto value = deserializeArrayElementWidget(cell.displayValue,&previous,false,&compactValueOnly)) {
                // Compact scalar leaves can retain ignored historical props.
                // A saved default there still needs the complete cleanup path,
                // including escaped keys and defaults in nested cell values.
                if(inheritedValuesUnchanged && compactValueOnly &&
                    compactPayloadHasNoSavedDefaults(cell.displayValue))continue;
                sanitize(*value);
                auto payload=serializeArrayElementValueWidget(*value,*array->elementTemplate);
                // Normalizing clipboard/default data cannot reinterpret an
                // unsupported legacy Graph. Retain its full cell verbatim.
                if(!payload.empty()&&payload!=cell.displayValue) {
                    if(!changedCells)changedCells.emplace(array->elements.values());
                    (*changedCells)[index].displayValue=std::move(payload);
                }
            }
        }
        if(changedCells)array->elements=std::move(*changedCells);
    }
    if(widget.xyGraph && !widget.xyGraph->preservedSource) {
        if(const auto data=xy::displayedData(*widget.xyGraph))
            static_cast<void>(xy::setInitialData(*widget.xyGraph,widget.xyGraph->datatype,data->exactSource));
        widget.xyGraph->current.reset();widget.xyGraph->navigation={};
    }
    if (widget.waveformGraph && !widget.waveformGraph->preservedSource) {
        const auto current=widget.waveformGraph->current;
        if(current) {
            if(current->datatype && current->exactSource)
                static_cast<void>(graph::setTypedInitialData(*widget.waveformGraph,*current->datatype,*current->exactSource));
            else static_cast<void>(graph::setInitialData(*widget.waveformGraph,*current));
            widget.valueType=widget.waveformGraph->datatype.canonical();
        }
        widget.waveformGraph->current.reset();
    }
}

void intrinsic(WidgetInstance &widget) {
    if(widget.collection) {
        if(const auto empty=makeEmptyCollectionState(collectionType(*widget.collection)))
            static_cast<void>(assignCollectionData(*widget.collection,*collectionValue(*empty)));
    }else if (auto *cluster = clusterContainerState(widget)) {
        for (auto &field : cluster->elements) if (field.widget) *field.widget = defaultValue(*field.widget);
    } else if (auto *array = arrayContainerState(widget)) {
        array->shape.assign(array->rank, 0); array->elements.clear();
    } else if(widget.xyGraph) {
        static_cast<void>(xy::setInitialData(*widget.xyGraph,widget.xyGraph->datatype,
            xy::emptyValue(widget.xyGraph->datatype)));
        widget.xyGraph->current.reset();
    } else if (widget.waveformGraph) {
        // Authored Initial data remains the legacy graph default.
        widget.waveformGraph->current.reset();
    } else if (isBooleanWidget(widget) || isButtonWidget(widget)) {
        static_cast<void>(setWidgetBooleanValue(widget, false));
    } else if (isEnumWidget(widget) || isRingWidget(widget)) {
        const auto item = std::find_if(widget.ringItems.begin(), widget.ringItems.end(), [](const auto &v) { return !v.disabled; });
        widget.numericValue = item == widget.ringItems.end() ? 0.0 : double(item->value);
        widget.initialValue = isEnumWidget(widget) && item != widget.ringItems.end() ? item->id : formatNumericValue(*widget.numericValue);
    } else if (isReferenceWidget(widget)) clearReferenceWidgetValue(widget);
    else if (isNumericWidget(widget)) static_cast<void>(setWidgetNumericValue(widget, 0));
    else if (isStringWidget(widget)) static_cast<void>(setWidgetStringValue(widget, {}));
    else if (isPathWidget(widget)) static_cast<void>(setWidgetPathValue(widget, {}));
    else widget.initialValue = std::string{};
}

bool materialize(WidgetInstance &widget, const ArrayElementValue *value) {
    if (!value || !value->defined) return true;
    if(value->kind==ArrayElementValueKind::Number && isNumericWidget(widget) &&
        numericCanonicalValueTypeIsIntegral(widget.valueType))
        return setWidgetNumericIntegerValueLexeme(widget,value->displayValue);
    if (value->kind == ArrayElementValueKind::Number && isNumericWidget(widget) &&
        numericCanonicalValueTypeIsComplex(widget.valueType))
        return setWidgetNumericComplexValueLexeme(widget, value->displayValue);
    if (value->kind == ArrayElementValueKind::Widget) {
        // Reading a stored cell is not a new data-transfer operation. Preserve
        // stable field identities and nested opaque cells without rewriting
        // their payloads through the destination's compact writer.
        if (const auto stored = deserializeArrayElementWidget(value->displayValue,&widget))
            applyArrayElementWidgetValue(widget,*stored);
    } else if (isStringWidget(widget)) static_cast<void>(setWidgetStringValue(widget,value->displayValue));
    else if (isPathWidget(widget)) static_cast<void>(setWidgetPathValue(widget,value->displayValue));
    else if (isBooleanWidget(widget) || isButtonWidget(widget)) static_cast<void>(setWidgetBooleanValue(widget,value->displayValue=="true" || value->displayValue=="1"));
    else {
        if (isEnumWidget(widget)) {
            const auto found=std::find_if(widget.ringItems.begin(),widget.ringItems.end(),[&](const auto &item){return item.id==value->displayValue;});
            if (found!=widget.ringItems.end()) { static_cast<void>(setRingSelectedValue(widget,found->value)); return true; }
        }
        double number=0; const auto &text=value->displayValue;
        const auto result=std::from_chars(text.data(),text.data()+text.size(),number);
        if (result.ec!=std::errc{} || result.ptr!=text.data()+text.size() || !std::isfinite(number)) return true;
        if (isEnumWidget(widget) || isRingWidget(widget)) {
            if (number>=0 && number<=UINT32_MAX && std::floor(number)==number) static_cast<void>(setRingSelectedValue(widget,static_cast<std::uint32_t>(number)));
        } else static_cast<void>(setWidgetNumericValue(widget,number));
    }
    return true;
}

PreparationOutcome assignWithFieldPolicy(WidgetInstance &target,const WidgetInstance &source,
    bool useFieldIdentities) {
    if (!compatibleWithFieldPolicy(target,source,useFieldIdentities)) return PreparationOutcome::Rejected;
    const auto before = encode(target);
    if (before.empty() || encode(source).empty()) return PreparationOutcome::Rejected;
    auto draft=target;
    if(!assignPreparedPrivateWidgetValues(draft,source,useFieldIdentities))return PreparationOutcome::Rejected;
    const auto after=encode(draft);
    if(after.empty())return PreparationOutcome::Rejected;
    if(before==after)return PreparationOutcome::Unchanged;
    target=std::move(draft);
    return PreparationOutcome::Changed;
}

ArrayDefaultPreparation prepareArrayDefaultValue(const WidgetInstance &widget) {
    // Retain the historical public projection even when the saved schema is
    // rejected. Mutating reset callers additionally consume the private status.
    ArrayDefaultPreparation result{widget};
    intrinsic(result.value);
    if(!widget.defaultData) {
        const auto original=encode(widget),projected=encode(result.value);
        if(!original.empty() && !projected.empty())result.outcome=original==projected
            ? PreparationOutcome::Unchanged:PreparationOutcome::Changed;
        return result;
    }
    const auto saved=decode(*widget.defaultData);
    if(saved && emptyEditingArray(widget) && emptyEditingArray(*saved) &&
        saved->classId==widget.classId && saved->role==widget.role &&
        arrayContainerState(*saved)->rank==arrayContainerState(widget)->rank &&
        binding::widgetType(widget).valid() && binding::widgetType(*saved).valid() &&
        binding::widgetType(widget).type==binding::widgetType(*saved).type) {
        // A palette Array shell has no element type to coerce. Restore its
        // saved logical extent without inventing a template or values.
        arrayContainerState(result.value)->shape=arrayContainerState(*saved)->shape;
        result.outcome=encode(result.value)==encode(widget) ?
            PreparationOutcome::Unchanged : PreparationOutcome::Changed;
    } else if(saved && isArrayWidget(*saved) && compatibleWithFieldPolicy(result.value,*saved,true))
        result.outcome=assignWithFieldPolicy(result.value,*saved,true);
    return result;
}

bool emptyEditingArray(const WidgetInstance &widget) {
    const auto *state=arrayContainerState(widget);
    return isArrayWidget(widget) && state && !state->elementTemplate &&
        state->elementClassId.empty() && state->elementValueType.empty() &&
        state->elementRole.empty() && !state->widgetComposedElements && state->elements.empty();
}






}
using namespace widget_data_detail;
bool assign(WidgetInstance &target, const WidgetInstance &source) {
    return assignWithFieldPolicy(target,source,false)==PreparationOutcome::Changed;
}

std::string encode(const WidgetInstance &widget) {
    auto value = widget; sanitize(value);
    auto result = serializeArrayElementWidget(value);
    if(result.size()>maximumBytes)return {};
    // A native graph may display more samples than the authored JSON reader
    // can persist. Refuse Make Default/Copy Data before installing an envelope
    // that the same reader would later reject and replace by intrinsic data.
    if(hasInterpretedXY(value) &&
        !source::parseJson(result,nullptr,source::JsonNumberPolicy::PreserveLexeme))return {};
    return result;
}

std::optional<WidgetInstance> decode(std::string_view value) {
    if (value.empty() || value.size() > maximumBytes) return {};
    auto result = deserializeArrayElementWidget(value);
    if (!result || !supported(*result)) return {};
    sanitize(*result); return result;
}

std::optional<WidgetInstance> read(const WidgetInstance &root, const ContainedWidgetPath &path) {
    const auto *current = &root;
    std::optional<WidgetInstance> cell;
    for (const auto &step : path) {
        if (!step.fieldId.empty()) {
            current = containedWidget(*current, {step});
            if (!current) return {};
        }
        else {
            const auto *array = arrayContainerState(*current);
            if (!array || !array->elementTemplate || step.indices.size() != array->rank) return {};
            auto next = *array->elementTemplate;
            const auto stored = array->elements.findByKey(step.indices);
            if(!materialize(next, stored == array->elements.end() ? nullptr : &*stored))return {};
            // The source/prototype may belong to the preceding materialized
            // cell. Finish the next owned value before releasing that owner.
            cell = std::move(next);
            current = &*cell;
        }
    }
    // No borrowed pointer escapes. Only the selected result is copied; a
    // selected complete Array cell can transfer its already prepared owner.
    if (cell && current == &*cell) return cell;
    return *current;
}

WidgetInstance defaultValue(const WidgetInstance &widget) {
    if(isArrayWidget(widget))return prepareArrayDefaultValue(widget).value;
    auto value = widget; intrinsic(value);
    if (widget.defaultData) if (auto saved = decode(*widget.defaultData)) {
        if (isClusterWidget(value) && isClusterWidget(*saved)) { auto draft=value;if(migrateContainedWidgetValue(draft,*saved))value=std::move(draft); }
        else if (compatibleWithFieldPolicy(value,*saved,isArrayWidget(value) && isArrayWidget(*saved))) {
            if(isArrayWidget(value) && isArrayWidget(*saved))
                static_cast<void>(assignWithFieldPolicy(value,*saved,true));
            else static_cast<void>(assign(value, *saved));
        }
    }
    return value;
}

}
