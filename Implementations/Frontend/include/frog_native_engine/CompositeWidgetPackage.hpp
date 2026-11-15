// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "frog_native_engine/FrogJson.hpp"
#include <algorithm>
#include <charconv>
#include <set>

namespace frog::native_engine::composite_widget {
inline constexpr std::size_t maximumPackageBytes = 128 * 1024;
inline constexpr std::string_view profile = "frog.widget.composite.native-parts@1";
inline constexpr std::string_view referencePrefix = "wfrog:local/";
struct Part {
    std::string id, classId, role, type, realization, asset, label, action;
    int x{}, y{}, width{}, height{};
};
struct Binding {
    std::string id, part, member, direction, type;
};
struct Package {
    std::string id, version, classId, name;
    std::vector<std::string> imports;
    std::vector<Part> parts;
    std::vector<Binding> bindings;
    int width{}, height{};
};

inline bool identity(std::string_view value) {
    return !value.empty() && value.size() <= 100 && value.front() != '.' &&
        std::ranges::all_of(value, [](char c) {
            return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '-' || c == '_';
        });
}

inline std::optional<Package> read(std::string_view bytes, std::string &error) {
    using Json = source::JsonValue;
    using Type = Json::Type;
    const auto text = [](const Json &obj, std::string_view name) {
        const auto *value = obj.member(name);
        return value && value->type == Type::String ? value->string : std::string{};
    };
    const auto shape = [](const Json *obj, std::initializer_list<std::string_view> keys) {
        return obj && obj->type == Type::Object && obj->object.size() == keys.size() &&
            std::ranges::all_of(keys, [&](auto key) { return obj->member(key) != nullptr; });
    };
    const auto array = [](const Json &obj, std::string_view key) {
        const auto *value = obj.member(key);
        return value && value->type == Type::Array ? value : nullptr;
    };
    const auto integer = [](const Json &obj, std::string_view key) {
        int value = -1;
        const auto *number = obj.member(key);
        if (!number || number->type != Type::Number) return -1;
        const auto *end = number->string.data() + number->string.size();
        const auto parsed = std::from_chars(number->string.data(), end, value);
        return parsed.ec == std::errc{} && parsed.ptr == end && value >= 0 && value <= 4096 ? value : -1;
    };
    const auto fail = [&](std::string_view at, std::string_view why) -> std::optional<Package> {
        error = "wfrog.composite.invalid " + std::string(at) + " " + std::string(why);
        return {};
    };
    error.clear();
    if (bytes.size() > maximumPackageBytes) return fail("/", "Package exceeds 128 KiB.");
    std::string parseError;
    const auto root = source::parseJson(bytes, &parseError, source::JsonNumberPolicy::PreserveLexeme);
    if (!root) return fail("/", parseError);
    if (!shape(&*root, {"wfrog_version", "package_kind", "profile", "package", "imports", "exports", "composite_classes"}) ||
        text(*root, "wfrog_version") != "0.1" || text(*root, "package_kind") != "widget_composite_library" ||
        text(*root, "profile") != profile) return fail("/", "Unsupported native-parts package profile.");
    const auto *metadata = root->member("package");
    const auto *exports = root->member("exports");
    const auto *imports = array(*root, "imports");
    const auto *classes = array(*root, "composite_classes");
    if (!shape(metadata, {"id", "version", "name", "license"}) || text(*metadata, "license").empty() ||
        !shape(exports, {"composite_classes"}) || !imports || imports->array.empty() || imports->array.size() > 16 ||
        !classes || classes->array.size() != 1) return fail("/", "Expected one exported class, a license and native imports.");
    Package result;
    result.id = text(*metadata, "id");
    result.version = text(*metadata, "version");
    result.name = text(*metadata, "name");
    if (!identity(result.id) || !identity(result.version) || result.name.empty() || result.name.size() > 120 ||
        std::ranges::any_of(result.name, [](unsigned char c) { return c < 32 || c == 127; }))
        return fail("/package", "Invalid identity, version or name.");
    for (const auto &import : imports->array) {
        if (import.type != Type::String || import.string.empty() || import.string.size() > 256 ||
            std::ranges::find(result.imports, import.string) != result.imports.end())
            return fail("/imports", "Import paths must be unique native realization identities.");
        result.imports.push_back(import.string);
    }
    const auto &klass = classes->array.front();
    if (!shape(&klass, {"id", "children", "bindings"})) return fail("/composite_classes/0", "Invalid class fields.");
    result.classId = text(klass, "id");
    const auto *exported = array(*exports, "composite_classes");
    if (!identity(result.classId) || !exported || exported->array.size() != 1 ||
        exported->array.front().type != Type::String || exported->array.front().string != result.classId)
        return fail("/exports/composite_classes", "The class must be explicitly exported.");
    const auto *children = array(klass, "children");
    if (!children || children->array.size() < 2 || children->array.size() > 16)
        return fail("/composite_classes/0/children", "Expected two to sixteen native children.");
    std::set<std::string> partIds, usedImports;
    for (const auto &child : children->array) {
        const auto path = "/composite_classes/0/children/" + std::to_string(result.parts.size());
        if (!shape(&child, {"id", "class", "role", "type", "realization", "asset", "label", "bounds", "action"}))
            return fail(path, "Invalid child fields.");
        Part part{text(child, "id"), text(child, "class"), text(child, "role"), text(child, "type"),
            text(child, "realization"), text(child, "asset"), text(child, "label"), text(child, "action")};
        const auto *bounds = child.member("bounds");
        if (!shape(bounds, {"x", "y", "width", "height"})) return fail(path + "/bounds", "Expected relative integer bounds.");
        part.x = integer(*bounds, "x"); part.y = integer(*bounds, "y");
        part.width = integer(*bounds, "width"); part.height = integer(*bounds, "height");
        if (!identity(part.id) || !partIds.insert(part.id).second || !identity(part.classId) ||
            part.label.size() > 120 || part.asset.empty() || part.type.empty() ||
            (part.role != "control" && part.role != "indicator") || part.x < 0 || part.y < 0 ||
            part.width < 16 || part.height < 16 || part.x + part.width > 4096 || part.y + part.height > 4096 ||
            std::ranges::find(result.imports, part.realization) == result.imports.end() ||
            child.member("action")->type != Type::String)
            return fail(path, "Invalid native child, identity, geometry or realization import.");
        usedImports.insert(part.realization);
        result.width = (std::max)(result.width, part.x + part.width);
        result.height = (std::max)(result.height, part.y + part.height);
        result.parts.push_back(std::move(part));
    }
    if (usedImports.size() != result.imports.size()) return fail("/imports", "Every import must be used.");
    const auto *bindings = array(klass, "bindings");
    if (!bindings || bindings->array.size() != result.parts.size())
        return fail("/composite_classes/0/bindings", "Expected one binding per part.");
    std::set<std::string> bindingIds, exposedParts;
    for (const auto &row : bindings->array) {
        const auto path = "/composite_classes/0/bindings/" + std::to_string(result.bindings.size());
        if (!shape(&row, {"id", "part", "member", "direction", "type"})) return fail(path, "Invalid binding fields.");
        Binding binding{text(row, "id"), text(row, "part"), text(row, "member"), text(row, "direction"), text(row, "type")};
        const auto part = std::ranges::find(result.parts, binding.part, &Part::id);
        if (!identity(binding.id) || !bindingIds.insert(binding.id).second || part == result.parts.end() ||
            !exposedParts.insert(binding.part).second || binding.type != part->type ||
            !((part->role == "indicator" && binding.member == "Value" && binding.direction == "input") ||
              (part->role == "control" && binding.member == "Value Change" && binding.direction == "event")))
            return fail(path, "Invalid member, direction or value type.");
        result.bindings.push_back(std::move(binding));
    }
    return result;
}
} // namespace frog::native_engine::composite_widget
