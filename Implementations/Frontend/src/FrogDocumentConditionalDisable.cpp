#include "FrogDocumentConditionalDisable.hpp"
#include "frog_native_engine/ConditionalDisable.hpp"

namespace frog::native_engine::document_internal {
namespace {
bool arrayOfSize(const JsonValue *value, std::size_t maximum) {
    return value && value->type == JsonValue::Type::Array &&
        !value->array.empty() && value->array.size() <= maximum;
}
std::string validateNode(const JsonValue &node) {
    const auto *regions = objectField(node, "regions");
    if (!arrayOfSize(regions, 128)) return "Expected 1 to 128 subdiagrams.";
    for (const auto &region : regions->array) {
        if (region.type != JsonValue::Type::Object || !jsonStringField(region, "id"))
            return "Every subdiagram requires a string ID.";
        const auto *defaultValue = objectField(region, "default");
        if (defaultValue && defaultValue->type != JsonValue::Type::Boolean)
            return "Default must be a Boolean flag.";
        const auto *condition = objectField(region, "condition");
        const auto *draft = objectField(region, "condition_draft");
        if (draft) {
            if (draft->type != JsonValue::Type::Boolean || !draft->boolean || condition ||
                jsonBooleanField(region, "default").value_or(false)) return "Invalid explicit condition draft.";
            continue;
        }
        if (jsonBooleanField(region, "default").value_or(false)) {
            if (condition) return "Default cannot also have a condition.";
            continue;
        }
        if (!condition || condition->type != JsonValue::Type::Object || condition->object.size() != 1)
            return "Expected a canonical condition.any object (no legacy expression).";
        const auto *any = objectField(*condition, "any");
        if (!arrayOfSize(any, 64)) return "A condition requires 1 to 64 OR groups.";
        for (const auto &term : any->array) {
            if (term.type != JsonValue::Type::Object || term.object.size() != 1)
                return "Each OR group must contain only all.";
            const auto *all = objectField(term, "all");
            if (!arrayOfSize(all, 64)) return "An AND group requires 1 to 64 comparisons.";
            for (const auto &comparison : all->array) {
                if (comparison.type != JsonValue::Type::Object || comparison.object.size() != 3 ||
                    !jsonStringField(comparison, "symbol") || !jsonStringField(comparison, "operator") ||
                    !jsonStringField(comparison, "value"))
                    return "A comparison requires string symbol, operator and value fields.";
            }
        }
    }
    DiagramNode candidate;
    readConditionalDisableCases(node, candidate);
    return validateConditionalDisableCases(candidate.conditionalDisableCases, true);
}
}

void readConditionalDisableCases(const JsonValue &nodeValue, DiagramNode &node) {
    node.conditionalDisableCases.clear();
    const auto *regions = objectField(nodeValue, "regions");
    if (!regions) return;
    for (const auto &region : regions->array) {
        DiagramConditionalDisableCase item{jsonStringField(region, "id").value_or(""),
            jsonBooleanField(region, "default").value_or(false), {}};
        item.conditionDraft = jsonBooleanField(region, "condition_draft").value_or(false);
        item.subdiagramLabelText = jsonStringField(region, "subdiagram_label_text").value_or("");
        const auto *condition = objectField(region, "condition");
        const auto *any = condition ? objectField(*condition, "any") : nullptr;
        if (any) for (const auto &group : any->array) {
            DiagramConditionalDisableTerm term;
            const auto *all = objectField(group, "all");
            if (all) for (const auto &comparison : all->array) {
                term.comparisons.push_back({jsonStringField(comparison, "symbol").value_or(""),
                    jsonStringField(comparison, "operator").value_or(""),
                    jsonStringField(comparison, "value").value_or("")});
            }
            item.anyOf.push_back(std::move(term));
        }
        node.conditionalDisableCases.push_back(std::move(item));
    }
}

std::string validateConditionalDisableJson(const JsonValue &root) {
    std::vector<const JsonValue *> pending{&root};
    while (!pending.empty()) {
        const auto *current = pending.back();pending.pop_back();
        if (current->type == JsonValue::Type::Object) {
            if (jsonStringField(*current, "structure_type") == "conditional_disable_structure" ||
                jsonStringField(*current, "type") == "frog.studio.structures.conditional_disable_structure") {
                if (const auto message = validateNode(*current); !message.empty())
                    return "Conditional Disable '" + jsonStringField(*current, "id").value_or("?") + "': " + message;
            }
            for (auto member = current->object.rbegin(); member != current->object.rend(); ++member)
                pending.push_back(&member->second);
        } else if (current->type == JsonValue::Type::Array) {
            for (auto member = current->array.rbegin(); member != current->array.rend(); ++member)
                pending.push_back(&*member);
        }
    }
    return {};
}
}
