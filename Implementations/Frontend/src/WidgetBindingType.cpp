#include "frog_native_engine/ReferenceWidget.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include "frog_native_engine/WidgetInstance.hpp"
#include "frog_native_engine/TabControl.hpp"
#include "frog_native_engine/WidgetFamilyEngine.hpp"
#include "frog_native_engine/CollectionAuthoringState.hpp"

#include <algorithm>
#include <unordered_set>

namespace frog::native_engine::binding {
namespace {
void attachChoices(TypeRef &type, const WidgetInstance &widget, ChoiceKind kind) {
    type.choiceKind=kind;
    for(const auto &item:ringItems(widget))
        type.choices.push_back({item.id,item.label,item.value,item.disabled,item.visible});
}
TypeParseResult leafType(const WidgetInstance &widget) {
    if(isXYGraphWidget(widget)) {
        if(!widget.xyGraph)return {{},"Graph XY has no typed state.",false};
        const auto &type=widget.xyGraph->datatype;
        const auto diagnostic=xy::validateType(type);
        if(!diagnostic.empty() || widget.valueType!=type.canonical())
            return {{},diagnostic.empty()?"Graph XY state and terminal type disagree.":diagnostic,false};
        return {type,{},false};
    }
    if(widget.classId==kCollectionWidgetClass) {
        if(!widget.collection)return {{},"Collection widget has no typed authoring state.",false};
        const auto type=collectionType(*widget.collection);const auto declared=parseType(widget.valueType);
        if(!declared.valid()||declared.type.canonical()!=type.canonical()||(type.known()&&!collectionValue(*widget.collection)))return {{},"Collection widget state and declared type disagree.",false};
        return {type,{},false};
    }
    if (isReferenceWidget(widget)) {
        auto type=referenceWidgetType(widget);
        const auto declared=parseType(widget.valueType);
        // Canonical text is the declared structural identity, not the private
        // schema envelope. Enum choices and display metadata live in the
        // symbolic value and must survive this check unchanged.
        if(!type.known() || !declared.valid() || declared.type.canonical()!=type.canonical())
            return {{}, "Reference widget value does not match its declared reference type.", false};
        return {std::move(type),{},false};
    }
    if (isPictureWidget(widget)) return {TypeRef::array(TypeRef::primitive("u32"), 2U), {}, false};
    if (isEnumWidget(widget)) {
        if (isTabWidget(widget) && !tabPagesValid(widget))
            return {{}, "Tab page identities, names and enum order must form a valid nonempty model.", false};
        auto representation = parseType(enumRepresentationValueType(widget));
        if (!representation.valid()) return representation;
        // Existing enum.domain_id is persisted by the widget serializer. Legacy
        // instances derive a stable local domain from their persisted instance ID.
        auto type=TypeRef::enumeration(widget.enumDomainId && !widget.enumDomainId->empty()
                ? *widget.enumDomainId : "frog.studio." + widget.instanceId,
            std::move(representation.type));
        attachChoices(type,widget,ChoiceKind::Enum);
        return {std::move(type), {}, false};
    }
    if (isRingWidget(widget)) {
        auto parsed=parseType(ringRepresentationValueType(widget));
        if(parsed.valid()) attachChoices(parsed.type,widget,ChoiceKind::Ring);
        return parsed;
    }
    if (isBooleanWidget(widget) || isButtonWidget(widget)) return parseType("bool");
    if (isStringWidget(widget)) {
        auto parsed=parseType("string");
        if(isStringDropdownWidget(widget)) attachChoices(parsed.type,widget,ChoiceKind::Dropdown);
        return parsed;
    }
    if (isPathWidget(widget)) return parseType("path");
    return parseType(widget.valueType.empty() ? "unknown" : widget.valueType);
}
}
TypeParseResult widgetType(const WidgetInstance &widget) {
    // Bound work by the number of authored nodes, never an arbitrary depth.
    // Keep only the active ancestry in the cycle set: two fields may share a
    // template without forming a cycle. Move completed types into their parent.
    constexpr std::size_t maximumSchemaNodes = 1000000;
    struct Frame {
        const WidgetInstance *widget;
        bool entered = false;
        bool array = false;
        bool legacy = false;
        std::size_t rank = 0;
        std::vector<const WidgetInstance *> children;
        std::vector<std::string> ids, names;
        std::vector<TypeRef> types;
        std::size_t next = 0;
    };
    std::vector<Frame> pending;
    std::unordered_set<const WidgetInstance *> ancestry;
    pending.push_back(Frame{&widget});
    std::size_t visited = 0;
    while (!pending.empty()) {
        auto &frame = pending.back();
        if (!frame.entered) {
            if (++visited > maximumSchemaNodes)
                return {{}, "Widget type exceeds the one-million-node schema budget.", false};
            if (!ancestry.insert(frame.widget).second)
                return {{}, "Widget type contains a cyclic container ancestry.", false};
            frame.entered = true;
            if (isArrayWidget(*frame.widget)) {
                const auto *state = arrayContainerState(*frame.widget);
                frame.array = true;
                frame.rank = state ? state->rank : 1U;
                frame.children.push_back(state ? state->elementTemplate.get() : nullptr);
            } else if (isClusterWidget(*frame.widget)) {
                const auto *state = clusterContainerState(*frame.widget);
                if (!state) return {{}, "Cluster field schema is missing.", false};
                std::vector<const ClusterElement *> fields;
                fields.reserve(state->elements.size());
                for (const auto &field : state->elements) fields.push_back(&field);
                std::stable_sort(fields.begin(), fields.end(), [](auto *a, auto *b) {
                    return a->logicalOrder < b->logicalOrder;
                });
                std::unordered_set<std::string_view> ids;
                std::optional<std::uint32_t> previousOrder;
                frame.children.reserve(fields.size());
                frame.ids.reserve(fields.size()); frame.names.reserve(fields.size());
                for (const auto *field : fields) {
                    if (previousOrder && *previousOrder == field->logicalOrder)
                        return {{}, "Cluster logical field orders must be unique.", false};
                    previousOrder = field->logicalOrder;
                    if (field->fieldId.empty() || !ids.insert(field->fieldId).second)
                        return {{}, "Cluster field IDs must be unique and nonempty.", false};
                    frame.children.push_back(field->widget.get());
                    frame.ids.push_back(field->fieldId);
                    frame.names.push_back(field->widget ? field->widget->label : std::string{});
                }
            }
            frame.types.reserve(frame.children.size());
        }
        if (frame.next < frame.children.size()) {
            const auto *child = frame.children[frame.next++];
            if (child) pending.push_back(Frame{child});
            else frame.types.emplace_back();
            continue;
        }
        TypeParseResult result;
        if (frame.array) {
            result.type = TypeRef::array(std::move(frame.types.front()), frame.rank);
            result.legacy = frame.legacy;
        }
        else if (isClusterWidget(*frame.widget)) {
            result.type = TypeRef::cluster(std::move(frame.ids), std::move(frame.types));
            result.type.fieldNames = std::move(frame.names);
        } else result = leafType(*frame.widget);
        if (!result.valid()) return result;
        ancestry.erase(frame.widget);
        pending.pop_back();
        if (pending.empty()) return result;
        if (pending.back().array) pending.back().legacy = result.legacy;
        pending.back().types.push_back(std::move(result.type));
    }
    return {};
}
} // namespace frog::native_engine::binding
