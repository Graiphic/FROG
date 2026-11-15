#include "frog_native_engine/PolymorphicFrog.hpp"

#include "CanonicalJsonSupport.hpp"
#include "frog_native_engine/CanonicalObjectReferenceDescriptor.hpp"
#include "frog_native_engine/DraftInterfaceInspection.hpp"
#include "frog_native_engine/WidgetBindingType.hpp"
#include <algorithm>
#include <set>

namespace frog::native_engine {
namespace {
using Json = source::JsonValue;
namespace cj = canonical_json;

bool hasFields(const Json &object, std::initializer_list<std::string_view> keys) {
    return object.type == Json::Type::Object && object.object.size() == keys.size() &&
        std::all_of(keys.begin(), keys.end(), [&](auto key) { return object.member(key); });
}

bool readString(const Json &object, std::string_view key, std::string &target) {
    const auto *value = object.member(key);
    if (!value || value->type != Json::Type::String) return false;
    target = value->string;
    return true;
}

bool readBool(const Json &object, std::string_view key, bool &target) {
    const auto *value = object.member(key);
    if (!value || value->type != Json::Type::Boolean) return false;
    target = value->boolean;
    return true;
}

bool isFamily(const Json &document) {
    const auto *format = document.member("format");
    return format && format->type == Json::Type::String && format->string == kPolymorphicFrogFormat;
}

bool validMenuName(std::string_view name) {
    if (name.empty()) return true;
    while (true) {
        const auto separator = name.find(':');
        const auto item = name.substr(0, separator);
        if (item.find_first_not_of(" \t") == item.npos ||
            std::any_of(item.begin(), item.end(), [](unsigned char c) { return c < 32; })) return false;
        if (separator == name.npos) return true;
        name.remove_prefix(separator + 1);
    }
}

bool exactInput(const PolymorphicFrogInterface &signature, const std::string &slotId,
    const binding::TypeRef &type) {
    const auto slot = signature.slots.find(slotId);
    return slot != signature.slots.end() && slot->second.input &&
        (!type.known() || canonicalObjectReferenceFullTypeEqual(type, slot->second.type));
}
} // namespace

bool isPolymorphicFrog(std::string_view sourceText) {
    const auto json = source::parseJson(sourceText, nullptr, source::JsonNumberPolicy::PreserveLexeme);
    return json && isFamily(*json);
}

std::optional<PolymorphicFrog> readPolymorphicFrog(std::string_view sourceText, std::string *error) {
    if (error) error->clear();
    auto fail = [&](std::string message) -> std::optional<PolymorphicFrog> {
        if (error) *error = std::move(message);
        return {};
    };
    std::string parseError;
    const auto json = source::parseJson(sourceText, &parseError, source::JsonNumberPolicy::PreserveLexeme);
    if (!json) return fail(std::move(parseError));
    if (!isFamily(*json)) return fail("Expected a polymorphic FROG document.");
    const auto *revision = json->member("revision");
    if (!revision || revision->type != Json::Type::Number || revision->string != "1")
        return fail("Unsupported polymorphic FROG revision.");
    if (!hasFields(*json, {"format", "revision", "id", "title", "description", "documentation_uri",
            "icon_svg", "options", "members"})) return fail("Invalid polymorphic FROG document fields.");

    PolymorphicFrog family;
    const auto &options = *json->member("options");
    const auto &members = *json->member("members");
    if (!readString(*json, "id", family.id) || family.id.empty() ||
        !readString(*json, "title", family.title) ||
        !readString(*json, "description", family.description) ||
        !readString(*json, "documentation_uri", family.documentationUri) ||
        !readString(*json, "icon_svg", family.iconSvg) ||
        !hasFields(options, {"draw_instance_icon", "show_selector_by_default", "adapt_to_type"}) ||
        !readBool(options, "draw_instance_icon", family.drawInstanceIcon) ||
        !readBool(options, "show_selector_by_default", family.showSelectorByDefault) ||
        !readBool(options, "adapt_to_type", family.adaptToType) ||
        members.type != Json::Type::Array || members.array.size() > 1024)
        return fail("Invalid polymorphic FROG identity, metadata, options or member list.");
    std::set<std::string> identities;
    for (const auto &entry : members.array) {
        PolymorphicFrogMember member;
        if (!hasFields(entry, {"id", "document_path", "menu_name", "selector_name"}) ||
            !readString(entry, "id", member.id) || member.id.empty() ||
            !identities.insert(member.id).second ||
            !readString(entry, "document_path", member.documentPath) ||
            !readString(entry, "menu_name", member.menuName) ||
            !readString(entry, "selector_name", member.selectorName))
            return fail("Invalid or duplicate polymorphic FROG member identity or fields.");
        family.members.push_back(std::move(member));
    }
    return family;
}

std::string writePolymorphicFrog(const PolymorphicFrog &family) {
    auto json = cj::object();
    json.object["format"] = cj::string(std::string(kPolymorphicFrogFormat));
    auto &revision = json.object["revision"];
    revision.type = Json::Type::Number;
    revision.string = "1";
    json.object["id"] = cj::string(family.id);
    json.object["title"] = cj::string(family.title);
    json.object["description"] = cj::string(family.description);
    json.object["documentation_uri"] = cj::string(family.documentationUri);
    json.object["icon_svg"] = cj::string(family.iconSvg);
    auto options = cj::object();
    options.object["draw_instance_icon"] = cj::boolean(family.drawInstanceIcon);
    options.object["show_selector_by_default"] = cj::boolean(family.showSelectorByDefault);
    options.object["adapt_to_type"] = cj::boolean(family.adaptToType);
    json.object["options"] = std::move(options);
    auto members = cj::array();
    for (const auto &member : family.members) {
        auto entry = cj::object();
        entry.object["id"] = cj::string(member.id);
        entry.object["document_path"] = cj::string(member.documentPath);
        entry.object["menu_name"] = cj::string(member.menuName);
        entry.object["selector_name"] = cj::string(member.selectorName);
        members.array.push_back(std::move(entry));
    }
    json.object["members"] = std::move(members);
    return cj::encode(json) + "\n";
}

std::vector<PolymorphicFrogIssue> validatePolymorphicFrog(const PolymorphicFrog &family) {
    std::vector<PolymorphicFrogIssue> issues;
    if (family.members.empty())
        issues.push_back({"polymorphic.empty", {}, {}, "Add at least one ordinary FROG instance."});
    std::set<std::string_view> references;
    for (const auto &member : family.members) {
        if (member.documentPath.empty() || member.documentPath.size() > 8192 ||
            member.documentPath.find('\0') != member.documentPath.npos)
            issues.push_back({"polymorphic.member.path_invalid", member.id, {}, "The instance path is invalid."});
        else if (!references.insert(member.documentPath).second)
            issues.push_back({"polymorphic.member.duplicate", member.id, {}, "This instance is already in the family."});
        if (!validMenuName(member.menuName))
            issues.push_back({"polymorphic.member.menu_invalid", member.id, {}, "Each part of the colon-separated menu name needs a label."});
    }
    return issues;
}

PolymorphicFrogResolution resolvePolymorphicFrog(const PolymorphicFrog &family,
    const PolymorphicFrogSelection &selection,
    std::span<const std::optional<PolymorphicFrogInterface>> signatures,
    const std::map<std::string, binding::TypeRef> &inputs, std::span<const std::string> wiredSlots) {
    PolymorphicFrogResolution result;
    result.issues = validatePolymorphicFrog(family);
    if (signatures.size() != family.members.size()) {
        result.issues.push_back({"polymorphic.interface.unavailable", {}, {}, "The instance interfaces were not captured."});
        return result;
    }
    const PolymorphicFrogInterface *reference = nullptr;
    std::map<std::string, const PolymorphicFrogPort *> commonSlots;
    for (std::size_t i = 0; i < signatures.size(); ++i) {
        const auto &member = family.members[i];
        const auto &signature = signatures[i];
        if (!signature) {
            result.issues.push_back({"polymorphic.member.interface_invalid", member.id, {}, "This instance has no valid connector."});
            continue;
        }
        if (!reference) reference = &*signature;
        if (signature->profileId != reference->profileId || signature->patternId != reference->patternId ||
            signature->transforms != reference->transforms)
            result.issues.push_back({"polymorphic.member.connector_mismatch", member.id, {}, "The connector pattern differs from the family connector."});
        for (const auto &[slotId, port] : signature->slots) {
            const auto [prior, inserted] = commonSlots.emplace(slotId, &port);
            if (!inserted && (prior->second->input != port.input || prior->second->requirement != port.requirement))
                result.issues.push_back({"polymorphic.member.slot_mismatch", member.id, slotId, "The connector slot direction or connection requirement differs."});
        }
    }
    if (!result.issues.empty()) return result;

    const auto &fixedMember = selection.manualMemberId || family.adaptToType
        ? selection.manualMemberId : selection.lastAutomaticMemberId;
    if (fixedMember) {
        const auto member = std::find_if(family.members.begin(), family.members.end(),
            [&](const auto &entry) { return entry.id == *fixedMember; });
        if (member == family.members.end()) {
            result.issues.push_back({"polymorphic.selection.member_missing", *fixedMember, {}, "The selected instance has been removed."});
            return result;
        }
        result.memberIndex = static_cast<std::size_t>(member - family.members.begin());
    } else {
        if (!family.adaptToType) {
            result.issues.push_back({"polymorphic.selection.manual_required", {}, {}, "Select an instance explicitly when adaptation is disabled."});
            return result;
        }
        const auto matching = std::find_if(signatures.begin(), signatures.end(), [&](const auto &entry) {
            return std::all_of(inputs.begin(), inputs.end(), [&](const auto &input) {
                return exactInput(*entry, input.first, input.second);
            });
        });
        if (matching == signatures.end()) {
            result.issues.push_back({"polymorphic.selection.no_match", {}, {}, "No instance accepts the connected input types exactly."});
            return result;
        }
        result.memberIndex = static_cast<std::size_t>(matching - signatures.begin());
    }
    const auto &selected = *signatures[*result.memberIndex];
    const auto &id = family.members[*result.memberIndex].id;
    for (const auto &slotId : wiredSlots) {
        if (!selected.slots.contains(slotId))
            result.issues.push_back({"polymorphic.selection.slot_missing", id, slotId, "The selected instance does not use this wired connector slot."});
    }
    for (const auto &[slotId, type] : inputs) {
        if (selected.slots.contains(slotId) && !exactInput(selected, slotId, type))
            result.issues.push_back({"polymorphic.selection.type_mismatch", id, slotId, "The input type differs from the selected instance's connector type."});
    }
    return result;
}

std::optional<PolymorphicFrogInterface> inspectPolymorphicFrogInterface(const FrogDraftData &member) {
    if (!member.declaredInterface) return {};
    const auto settings = readFrogDraftInterfaceMapSettings(*member.declaredInterface);
    const auto declaration = inspectFrogDraftInterface(*member.declaredInterface);
    if (!settings || !settings->mapPresent || settings->patternId.empty() || !declaration.contractValid)
        return {};
    PolymorphicFrogInterface signature;
    signature.profileId = settings->profileId;
    signature.patternId = settings->patternId;
    signature.transforms = settings->transformTokens;
    for (const auto &port : declaration.ports) {
        const WidgetInstance *widget = nullptr;
        for (const auto &object : member.widgets.objects()) {
            if (!object.widget) continue;
            const auto &candidate = *object.widget;
            if ((port.input ? candidate.publicInputId : candidate.publicOutputId) != port.id) continue;
            if (widget) return {};
            widget = &candidate;
        }
        if (!widget || !widget->interfaceMapSlotId || widget->interfaceMapSlotId->empty()) return {};
        const auto schema = binding::widgetType(*widget);
        if (!schema.valid() || !schema.type.known() ||
            schema.type.canonical() != port.parsedType.type.canonical()) return {};
        if (!signature.slots.emplace(*widget->interfaceMapSlotId, PolymorphicFrogPort{
            port.id, port.sourceLabel.value_or(port.sourceName.value_or(port.id)),
            schema.type, port.input, port.connection}).second) return {};
    }
    return signature;
}
} // namespace frog::native_engine
