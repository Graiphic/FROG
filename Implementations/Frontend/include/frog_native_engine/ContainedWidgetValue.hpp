#pragma once

#include "ContainedWidgetPath.hpp"
#include "WidgetBindingType.hpp"
#include "WidgetDataCompatibility.hpp"
#include "FrogDraftData.hpp"
#include "FrogDocumentWidgetValues.hpp"
#include <algorithm>

namespace frog::native_engine {
namespace data_operations {
std::string encode(const WidgetInstance &);
std::optional<WidgetInstance> decode(std::string_view);
}


// Value traversal materializes each selected Array cell; layout traversal
// above intentionally follows the shared templates instead.
inline std::optional<WidgetInstance> containedWidgetValueAtPath(
    const WidgetInstance &root, const ContainedWidgetPath &path) {
    auto value = root;
    for (const auto &step : path) {
        if (step.fieldId.empty()) {
            const auto *array = arrayContainerState(value);
            if (!array || !array->elementTemplate || step.indices.size() != array->rank) return std::nullopt;
            auto cell = *array->elementTemplate;
            const auto stored = std::find_if(array->elements.begin(), array->elements.end(),
                [&](const auto &item) { return item.indices == step.indices; });
            materializeArrayElementValue(cell, stored == array->elements.end() ? nullptr : &*stored);
            value = std::move(cell);
        } else {
            const auto *field = containedWidget(value, {step});
            if (!field) return std::nullopt;
            auto child = *field;
            value = std::move(child);
        }
    }
    return value;
}

// The caller owns a detached draft. Write back from the leaf through every
// Array cell so a nested edit never changes the shared value template.
inline bool setContainedWidgetValue(WidgetInstance &widget, const ContainedWidgetPath &path,
    const WidgetInstance &value, std::size_t index = 0) {
    if (index > path.size()) return false;
    auto draft = std::make_unique<WidgetInstance>(widget);
    struct ArrayFrame {
        WidgetInstance *parent;
        std::vector<std::uint32_t> indices;
        std::unique_ptr<WidgetInstance> cell;
    };
    std::vector<ArrayFrame> pending;
    auto *current = draft.get();
    for (; index < path.size(); ++index) {
        const auto &step = path[index];
        if (step.fieldId.empty()) {
            auto cell = containedWidgetValueAtPath(*current, {step});
            if (!cell) return false;
            auto owned = std::make_unique<WidgetInstance>(std::move(*cell));
            auto *next = owned.get();
            pending.push_back({current, step.indices, std::move(owned)});
            current = next;
        } else {
            current = containedWidget(*current, {step});
            if (!current) return false;
        }
    }
    if (binding::widgetType(*current).type != binding::widgetType(value).type) return false;
    applyArrayElementWidgetValue(*current, value);
    for (auto frame = pending.rbegin(); frame != pending.rend(); ++frame)
        if (!setArrayElementValueFromWidget(*frame->parent, frame->indices, *frame->cell)) return false;
    widget = std::move(*draft);
    return true;
}

// Schema changes must not discard the values of surviving fields. The ordinary
// value copier deliberately rejects unequal aggregate types; here the aggregate
// schema HAS changed, so match the surviving fields recursively by stable ID.
inline bool migrateContainedWidgetValue(WidgetInstance &target, const WidgetInstance &old);

// Scalar cells have no per-cell design. Convert their old value against the
// new shared representation without changing extent, indices or presence.
inline bool migrateContainedNumericArrayCell(WidgetInstance &array,
    const WidgetInstance &before, ArrayElementValue &cell) {
    const auto *next=arrayContainerState(array),*previous=arrayContainerState(before);
    if(cell.kind!=ArrayElementValueKind::Number||!cell.defined||!next||!previous||
       !next->elementTemplate||!previous->elementTemplate||
       !isNumericWidget(*next->elementTemplate)||!isNumericWidget(*previous->elementTemplate)||
       binding::widgetType(*next->elementTemplate).type==binding::widgetType(*previous->elementTemplate).type)return true;
    auto oldValue=*previous->elementTemplate;
    materializeArrayScalarElementValue(oldValue,&cell);
    auto value=*next->elementTemplate;
    if(!migrateContainedWidgetValue(value,oldValue))return false;
    const auto prepared=prepareArrayScalarElementValue(array,cell.indices,value);
    if(!prepared)return false;
    cell.kind=prepared->kind;cell.displayValue=prepared->displayValue;
    return true;
}

inline bool migrateContainedWidgetValue(WidgetInstance &target, const WidgetInstance &old) {
    if (auto *cluster = clusterContainerState(target)) {
        const auto *source = clusterContainerState(old);
        if (!source) return true;
        for (auto &field : cluster->elements) {
            const auto found = std::find_if(source->elements.begin(), source->elements.end(),
                [&](const auto &candidate) { return candidate.fieldId == field.fieldId; });
            if (field.widget && found != source->elements.end() && found->widget &&
                !migrateContainedWidgetValue(*field.widget, *found->widget))return false;
        }
    } else if (auto *array = arrayContainerState(target)) {
        const auto *source = arrayContainerState(old);
        if (!source || !array->elementTemplate || !source->elementTemplate) return true;
        if (array->rank != source->rank) {
            auto migrated = std::make_unique<WidgetInstance>(old);
            return setArrayRank(*migrated, array->rank) && migrateContainedWidgetValue(target, *migrated);
        }
        const bool survivingNestedArray = isArrayWidget(*array->elementTemplate) &&
            isArrayWidget(*source->elementTemplate);
        if (!canMigrateContainedArrayValue(target, old) && !survivingNestedArray) return true;
        const auto targetType = binding::widgetType(target);
        const auto sourceType = binding::widgetType(old);
        const bool sameSchema = targetType.valid() && sourceType.valid() &&
            targetType.type.known() && sourceType.type.known() && targetType.type == sourceType.type;
        if(!migrateContainedWidgetValue(*array->elementTemplate,*source->elementTemplate))return false;
        array->shape = source->shape;array->elements = source->elements;
        for (auto &cell : array->elements) {
            if(cell.kind!=ArrayElementValueKind::Widget) {
                if(!migrateContainedNumericArrayCell(target,old,cell))return false;
                continue;
            }
            bool compactValueOnly=false;
            const auto stored=deserializeArrayElementWidget(cell.displayValue,source->elementTemplate.get(),false,&compactValueOnly);
            if(!stored)return false;
            if(sameSchema&&compactValueOnly)continue;
            auto value=std::make_unique<WidgetInstance>(*array->elementTemplate);
            if(!migrateContainedWidgetValue(*value,*stored))return false;
            auto payload=serializeArrayElementValueWidget(*value,*array->elementTemplate);
            if(payload.empty())return false;
            cell.displayValue=std::move(payload);
        }
    } else if (isNumericWidget(target) && isNumericWidget(old) &&
        binding::widgetType(target).type != binding::widgetType(old).type) {
        // Use the same representation conversion as the standalone control,
        // including truncation, saturation and exact surviving integer lexemes.
        auto converted=old;
        if(!setNumericRepresentationKind(converted,numericRepresentationKind(target))&&
           binding::widgetType(converted).type!=binding::widgetType(target).type)return false;
        if(binding::widgetType(converted).type!=binding::widgetType(target).type)return false;
        applyArrayElementWidgetValue(target,converted);
    } else if (binding::widgetType(target).type == binding::widgetType(old).type) {
        applyArrayElementWidgetValue(target, old);
    }
    return true;
}

// A compact cell inherits unchanged data from its original prototype. Before
// replacing that prototype, preserve existing cells against the new definition.
// The caller owns a detached draft; changed/new cell payloads already use its
// current prototype and must not be mistaken for untouched cells from before.
inline bool rebaseContainedWidgetValues(WidgetInstance &root,const WidgetInstance &before) {
    struct Frame {WidgetInstance *value;const WidgetInstance *before;bool ready;};
    std::vector<Frame> pending{{&root,&before,false}};
    while(!pending.empty()) {
        const auto frame=pending.back();pending.pop_back();
        auto *array=arrayContainerState(*frame.value);
        const auto *previousArray=arrayContainerState(*frame.before);
        if(!frame.ready) {
            pending.push_back({frame.value,frame.before,true});
            if(auto *cluster=clusterContainerState(*frame.value)) {
                const auto *previousCluster=clusterContainerState(*frame.before);
                if(!previousCluster)continue;
                for(auto &field:cluster->elements)if(field.widget) {
                    const auto found=std::find_if(previousCluster->elements.begin(),previousCluster->elements.end(),
                        [&](const auto &old){return old.fieldId==field.fieldId;});
                    if(found!=previousCluster->elements.end() && found->widget)
                        pending.push_back({field.widget.get(),found->widget.get(),false});
                }
            } else if(array && previousArray && array->elementTemplate && previousArray->elementTemplate) {
                pending.push_back({array->elementTemplate.get(),previousArray->elementTemplate.get(),false});
            }
            continue;
        }
        const auto currentType=binding::widgetType(*frame.value),oldType=binding::widgetType(*frame.before);
        if(frame.value->defaultData&&currentType.valid()&&oldType.valid()&&currentType.type.known()&&oldType.type.known()&&
           currentType.type!=oldType.type&&(isArrayWidget(*frame.value)||isClusterWidget(*frame.value))) {
            const auto saved=data_operations::decode(*frame.value->defaultData);
            if(!saved)return false;
            const auto savedType=binding::widgetType(*saved);
            if(!savedType.valid())return false;
            if(savedType.type!=currentType.type) {
                if(savedType.type!=oldType.type)return false;
                auto migrated=*frame.value;migrated.defaultData.reset();
                if(!migrateContainedWidgetValue(migrated,*saved))return false;
                const auto payload=data_operations::encode(migrated);
                const auto verified=payload.empty()?std::optional<WidgetInstance>{}:data_operations::decode(payload);
                if(!verified||binding::widgetType(*verified).type!=currentType.type)return false;
                frame.value->defaultData=payload;
            }
        }
        if(!array || !previousArray || !array->elementTemplate || !previousArray->elementTemplate)continue;
        // The existing value writer defines equality independently of design
        // and saved defaults. Evaluate it once for this prototype pair, not
        // once per cell. A changed type/value still takes the normal rebase.
        const auto prototypeDelta=serializeArrayElementValueWidget(
            *array->elementTemplate,*previousArray->elementTemplate);
        const bool inheritedValuesUnchanged=!prototypeDelta.empty() && prototypeDelta==
            serializeArrayElementValueWidget(*previousArray->elementTemplate,*previousArray->elementTemplate);
        for(auto &cell:array->elements) {
            if(cell.kind!=ArrayElementValueKind::Widget) {
                const auto previous=std::find_if(previousArray->elements.begin(),previousArray->elements.end(),[&](const auto &old){return old.indices==cell.indices;});
                if(previous!=previousArray->elements.end()&&previous->displayValue==cell.displayValue&&
                   previous->kind==cell.kind&&previous->defined==cell.defined&&previous->enabled==cell.enabled&&
                   !migrateContainedNumericArrayCell(*frame.value,*frame.before,cell))return false;
                continue;
            }
            const auto found=std::find_if(previousArray->elements.begin(),previousArray->elements.end(),
                [&](const auto &old){return old.indices==cell.indices;});
            if(found==previousArray->elements.end() || found->kind!=cell.kind ||
                found->displayValue!=cell.displayValue || found->defined!=cell.defined || found->enabled!=cell.enabled)continue;
            bool compactValueOnly=false;
            const auto old=deserializeArrayElementWidget(cell.displayValue,
                previousArray->elementTemplate.get(),false,&compactValueOnly);
            if(!old)return false;
            // Compact scalar leaves may contain ignored historical design
            // fields. Re-encoding equal values would needlessly replace those
            // exact bytes with the new prototype's label/default/geometry.
            if(inheritedValuesUnchanged && compactValueOnly)continue;
            auto value=*array->elementTemplate;
            if(!migrateContainedWidgetValue(value,*old))return false;
            auto payload=serializeArrayElementValueWidget(value,*array->elementTemplate);
            if(payload.empty() || !deserializeArrayElementWidget(payload,array->elementTemplate.get()))return false;
            cell.displayValue=std::move(payload);
        }
    }
    return true;
}


} // namespace frog::native_engine
