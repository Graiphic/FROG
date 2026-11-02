#pragma once
#include "frog_native_engine/ConnectivityConfiguration.hpp"
#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <unordered_set>
#include <utility>
namespace frog::native_engine {
namespace {
using Type = binding::TypeRef;
using Json = source::JsonValue;

Type errorType() { return Type::cluster({"status", "code", "source"},
    {Type::primitive("bool"), Type::primitive("i32"), Type::primitive("string")}); }
std::string_view familyToken(ConnectivityReferenceFamily family) {
    switch (family) {
    case ConnectivityReferenceFamily::ActiveX: return "activex";
    case ConnectivityReferenceFamily::Python: return "python";
    case ConnectivityReferenceFamily::Variant: return "variant";
    case ConnectivityReferenceFamily::Document: return "document";
    case ConnectivityReferenceFamily::DataFinder: return "datafinder";
    case ConnectivityReferenceFamily::Database: return "database";
    case ConnectivityReferenceFamily::InputDevice: return "input_device";
    case ConnectivityReferenceFamily::Registry: return "registry";
    case ConnectivityReferenceFamily::Catalog: return "catalog";
    }
    return {};
}
std::string_view kindToken(ConnectivityNodeKind kind) {
    switch (kind) {
    case ConnectivityNodeKind::Constructor: return "constructor";
    case ConnectivityNodeKind::Property: return "property";
    case ConnectivityNodeKind::Invoke: return "invoke";
    case ConnectivityNodeKind::Callback: return "callback";
    case ConnectivityNodeKind::Python: return "python";
    case ConnectivityNodeKind::TypeConversion: return "type_conversion";
    case ConnectivityNodeKind::StaticReference: return "static_reference";
    case ConnectivityNodeKind::AutomationOpen: return "automation_open";
    case ConnectivityNodeKind::Polymorphic: return "polymorphic";
    }
    return {};
}
std::string_view stateToken(ConnectivityMetadataState state) {
    switch (state) {
    case ConnectivityMetadataState::Unconfigured: return "unconfigured";
    case ConnectivityMetadataState::Available: return "available";
    case ConnectivityMetadataState::Unavailable: return "unavailable";
    }
    return {};
}
std::string_view directionToken(ConnectivityParameterDirection direction) {
    switch (direction) {
    case ConnectivityParameterDirection::Input: return "input";
    case ConnectivityParameterDirection::Output: return "output";
    case ConnectivityParameterDirection::InOut: return "inout";
    }
    return {};
}
bool familyReference(const Type &type, ConnectivityReferenceFamily family) {
    const auto prefix = "connectivity." + std::string(familyToken(family)) + ".";
    return type.known() && type.kind == binding::TypeKind::Reference && type.name.starts_with(prefix);
}
Type referenceType(const ConnectivityNodeConfiguration &config) {
    if (!config.referenceClassId.empty()) {
        const auto type = Type::reference(config.referenceClassId);
        return familyReference(type, config.family) ? type : Type{};
    }
    const auto family = std::string(familyToken(config.family));
    const auto suffix = config.family == ConnectivityReferenceFamily::ActiveX ? ".automation" : ".object";
    return Type::reference("connectivity." + family + suffix);
}
const Type *evidenceType(std::span<const ConnectivityTypeEvidence> evidence, std::string_view id) {
    const auto found = std::find_if(evidence.begin(), evidence.end(),
        [id](const auto &item) { return item.portId == id; });
    return found != evidence.end() ? &found->valueType : nullptr;
}
Type selectedType(const Type &saved, std::span<const ConnectivityTypeEvidence> evidence,
    std::string_view id) {
    if (const auto *wired = evidenceType(evidence, id)) return *wired;
    return saved;
}
std::string identity(ConnectivityNodeConfiguration &config, std::string_view prefix) {
    if (config.nextIdentity == std::numeric_limits<std::uint64_t>::max()) return {};
    return std::string(prefix) + std::to_string(config.nextIdentity++);
}
bool safeId(std::string_view id) {
    return !id.empty() && std::all_of(id.begin(), id.end(), [](unsigned char ch) {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') || ch == '_';
    });
}
Json object() { Json value; value.type = Json::Type::Object; return value; }
Json array() { Json value; value.type = Json::Type::Array; return value; }
Json string(std::string_view text) { Json value; value.type = Json::Type::String; value.string = text; return value; }
Json boolean(bool flag) { Json value; value.type = Json::Type::Boolean; value.boolean = flag; return value; }
Json integer(std::uint64_t number) {
    Json value; value.type = Json::Type::Number; value.number = static_cast<double>(number);
    value.string = std::to_string(number); return value;
}
Json typeJson(const Type &type) {
    auto value = object();
    value.object["canonical"] = string(type.canonical());
    auto names = array(); for (const auto &name : type.fieldNames) names.array.push_back(string(name));
    value.object["field_names"] = std::move(names);
    value.object["choice_kind"] = string(type.choiceKind == binding::ChoiceKind::Enum ? "enum" :
        type.choiceKind == binding::ChoiceKind::Ring ? "ring" :
        type.choiceKind == binding::ChoiceKind::Dropdown ? "dropdown" : "none");
    auto choices = array();
    for (const auto &choice : type.choices) {
        auto item = object(); item.object["id"] = string(choice.id); item.object["label"] = string(choice.label);
        item.object["value"] = integer(choice.value); item.object["disabled"] = boolean(choice.disabled);
        item.object["visible"] = boolean(choice.visible); choices.array.push_back(std::move(item));
    }
    value.object["choices"] = std::move(choices);
    auto children = array(); for (const auto &child : type.children) children.array.push_back(typeJson(child));
    value.object["children"] = std::move(children); return value;
}
bool readString(const Json &json, std::string_view key, std::string &result) {
    const auto *value = json.member(key);
    if (!value || value->type != Json::Type::String) return false;
    result = value->string; return true;
}
bool readBool(const Json &json, std::string_view key, bool &result) {
    const auto *value = json.member(key);
    if (!value || value->type != Json::Type::Boolean) return false;
    result = value->boolean; return true;
}
bool readInteger(const Json &json, std::string_view key, std::uint64_t &result) {
    const auto *value = json.member(key);
    if (!value || value->type != Json::Type::Number) return false;
    if (!value->string.empty()) {
        const auto parsed = std::from_chars(value->string.data(), value->string.data() + value->string.size(), result);
        return parsed.ec == std::errc{} && parsed.ptr == value->string.data() + value->string.size();
    }
    if (!std::isfinite(value->number) || value->number < 0 || value->number > 9007199254740991.0 ||
        std::floor(value->number) != value->number) return false;
    result = static_cast<std::uint64_t>(value->number); return true;
}
bool readType(const Json *json, Type &type, std::size_t depth = 0) {
    if (!json || json->type != Json::Type::Object || depth > 32) return false;
    std::string canonical, choiceKind;
    if (!readString(*json, "canonical", canonical) || !readString(*json, "choice_kind", choiceKind)) return false;
    auto parsed = binding::parseType(canonical); if (!parsed.valid()) return false;
    type = std::move(parsed.type);
    const auto *children = json->member("children"), *names = json->member("field_names"), *choices = json->member("choices");
    if (!children || children->type != Json::Type::Array || children->array.size() != type.children.size() ||
        !names || names->type != Json::Type::Array || !choices || choices->type != Json::Type::Array) return false;
    for (std::size_t index = 0; index < children->array.size(); ++index) {
        Type child; if (!readType(&children->array[index], child, depth + 1) ||
            child.canonical() != type.children[index].canonical()) return false;
        type.children[index] = std::move(child);
    }
    for (const auto &name : names->array) {
        if (name.type != Json::Type::String) return false;
        type.fieldNames.push_back(name.string);
    }
    if (!type.fieldNames.empty() && type.fieldNames.size() != type.fieldIds.size()) return false;
    if (choiceKind == "none") type.choiceKind = binding::ChoiceKind::None;
    else if (choiceKind == "enum") type.choiceKind = binding::ChoiceKind::Enum;
    else if (choiceKind == "ring") type.choiceKind = binding::ChoiceKind::Ring;
    else if (choiceKind == "dropdown") type.choiceKind = binding::ChoiceKind::Dropdown;
    else return false;
    std::unordered_set<std::string> choiceIds;
    for (const auto &choice : choices->array) {
        binding::ChoiceItemMetadata item; std::uint64_t number = 0;
        if (!readString(choice, "id", item.id) || !readString(choice, "label", item.label) ||
            !readInteger(choice, "value", number) || number > std::numeric_limits<std::uint32_t>::max() ||
            !readBool(choice, "disabled", item.disabled) || !readBool(choice, "visible", item.visible) ||
            (!item.id.empty() && !choiceIds.insert(item.id).second)) return false;
        item.value = static_cast<std::uint32_t>(number); type.choices.push_back(std::move(item));
    }
    return canonical == type.canonical();
}
// The private saved profile retains historical families even when an extension
// is absent from the active catalogue. Creation and decoding are separate paths.
std::optional<ConnectivityNodeConfiguration> savedConnectivityConfigurationForCatalog(std::string_view id) {
    ConnectivityNodeConfiguration config; config.catalogId = id;
    if (id.starts_with("connectivity.activex.")) config.family = ConnectivityReferenceFamily::ActiveX;
    else if (id.starts_with("connectivity.python.")) config.family = ConnectivityReferenceFamily::Python;
    else if (id.starts_with("connectivity.datafinder.")) config.family = ConnectivityReferenceFamily::DataFinder;
    else if (id.starts_with("connectivity.database.")) config.family = ConnectivityReferenceFamily::Database;
    else if (id.starts_with("connectivity.input_device.")) config.family = ConnectivityReferenceFamily::InputDevice;
    else if (id.starts_with("connectivity.registry.")) config.family = ConnectivityReferenceFamily::Registry;
    else if (id.starts_with("connectivity.")) config.family = ConnectivityReferenceFamily::Catalog;
    else return {};
    if (id.ends_with(".constructor")) config.kind = ConnectivityNodeKind::Constructor;
    else if (id.ends_with(".property_node")) config.kind = ConnectivityNodeKind::Property;
    else if (id.ends_with(".invoke_node")) config.kind = ConnectivityNodeKind::Invoke;
    else if (id.ends_with(".register_event_callback")) config.kind = ConnectivityNodeKind::Callback;
    else if (id == "connectivity.python.node") config.kind = ConnectivityNodeKind::Python;
    else if (id.ends_with(".static_vi_reference")) config.kind = ConnectivityNodeKind::StaticReference;
    else if (id.ends_with(".automation_open")) config.kind = ConnectivityNodeKind::AutomationOpen;
    else if (id == "connectivity.input_device.acquire_input_data" || id == "connectivity.input_device.close_input_device" ||
        id == "connectivity.registry.read_value_simple" || id == "connectivity.registry.write_value_simple" ||
        id == "connectivity.activex.close_reference")
        config.kind = ConnectivityNodeKind::Polymorphic;
    else if (id.ends_with(".to_more_generic_class") || id.ends_with(".to_more_specific_class") ||
        id.ends_with(".variant_to_data") || id.ends_with(".to_variant"))
        config.kind = ConnectivityNodeKind::TypeConversion;
    else config.kind = ConnectivityNodeKind::Polymorphic;
    config.unresolvedReason = "Select a type or provide the metadata needed by this node.";
    if (config.kind == ConnectivityNodeKind::Property) (void)connectivityAppendProperty(config);
    else if (config.kind == ConnectivityNodeKind::Callback) (void)connectivityAppendCallback(config);
    else if (config.kind == ConnectivityNodeKind::Python) {
        // Two initially visible rows reproduce the filmed help; the list is extensible.
        (void)connectivityAppendParameter(config); (void)connectivityAppendParameter(config);
    }
    return config;
}
} // namespace

}
