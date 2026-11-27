// SPDX-License-Identifier: Apache-2.0
#include "frog_native_engine/ClassDefinition.hpp"

#include "CanonicalJsonSupport.hpp"
#include "frog_native_engine/CanonicalRecordConstant.hpp"

#include <algorithm>
#include <set>
#include <stdexcept>

namespace frog::native_engine {
namespace {
using Json = source::JsonValue;
using J = Json::Type;
namespace cj = canonical_json;
constexpr std::size_t maxMembers = 64;
constexpr std::size_t maxPorts = 32;
constexpr std::size_t maxBytes = 4U * 1024 * 1024;
constexpr double maxFieldLayoutCoordinate = 10'000'000.0;

struct InvalidClass : std::runtime_error { using std::runtime_error::runtime_error; };
void require(bool condition, std::string_view path, std::string_view message) {
    if (!condition) throw InvalidClass(std::string(path) + ": " + std::string(message));
}

const Json &get(const Json &object, std::string_view key, J kind, const std::string &path) {
    const auto *value = object.member(key);
    require(value && value->type == kind, cj::pointer(path, key), "Missing or invalid field.");
    return *value;
}

const Json &any(const Json &object, std::string_view key, const std::string &path) {
    const auto *value = object.member(key);
    require(value != nullptr, cj::pointer(path, key), "Missing field.");
    return *value;
}

void keys(const Json &object, std::initializer_list<std::string_view> expected, const std::string &path) {
    require(object.type == J::Object && object.object.size() == expected.size() &&
        std::all_of(expected.begin(), expected.end(), [&](auto key) { return object.member(key); }),
        path, "Unexpected class declaration fields.");
}

std::string str(const Json &object, std::string_view key, const std::string &path) {
    return get(object, key, J::String, path).string;
}

std::uint32_t revision(const Json &object, std::string_view key, const std::string &path) {
    const auto &lexeme = get(object, key, J::Number, path).string;
    std::uint32_t result = 0;
    const auto parsed = std::from_chars(lexeme.data(), lexeme.data() + lexeme.size(), result);
    require(parsed.ec == std::errc{} && parsed.ptr == lexeme.data() + lexeme.size(),
        cj::pointer(path, key), "Expected a uint32 revision.");
    return result;
}

Json number(std::uint32_t value) {
    Json result;
    result.type = J::Number;
    result.string = std::to_string(value);
    return result;
}

double readLayoutCoordinate(const Json &object, std::string_view key, const std::string &path) {
    const auto &lexeme = get(object, key, J::Number, path).string;
    double value = 0.0;
    const auto parsed = std::from_chars(lexeme.data(), lexeme.data() + lexeme.size(), value);
    require(parsed.ec == std::errc{} && parsed.ptr == lexeme.data() + lexeme.size() &&
        std::isfinite(value) && std::abs(value) <= maxFieldLayoutCoordinate,
        cj::pointer(path, key), "Expected a finite layout coordinate within the supported range.");
    return value;
}

Rect readFieldLayout(const Json &object, const std::string &path) {
    keys(object, {"x", "y", "width", "height"}, path);
    const Rect layout{readLayoutCoordinate(object, "x", path), readLayoutCoordinate(object, "y", path),
        readLayoutCoordinate(object, "width", path), readLayoutCoordinate(object, "height", path)};
    require(layout.width > 0.0 && layout.height > 0.0, path, "Field layout extents must be positive.");
    return layout;
}

Json layoutNumber(double value) {
    // std::to_chars gives the shortest representation that survives rereading.
    std::array<char, 64> buffer{};
    const auto encoded = std::to_chars(buffer.data(), buffer.data() + buffer.size(), value);
    require(encoded.ec == std::errc{}, "/fields/layout", "Could not serialize the field layout.");
    Json result;
    result.type = J::Number;
    result.string.assign(buffer.data(), encoded.ptr);
    return result;
}

Json writeFieldLayout(const Rect &layout) {
    require(std::isfinite(layout.x) && std::abs(layout.x) <= maxFieldLayoutCoordinate &&
        std::isfinite(layout.y) && std::abs(layout.y) <= maxFieldLayoutCoordinate &&
        layout.width > 0.0 && layout.width <= maxFieldLayoutCoordinate &&
        layout.height > 0.0 && layout.height <= maxFieldLayoutCoordinate,
        "/fields/layout", "Field layout requires finite coordinates and positive supported extents.");
    auto object = cj::object();
    object.object = {{"x", layoutNumber(layout.x)}, {"y", layoutNumber(layout.y)},
        {"width", layoutNumber(layout.width)}, {"height", layoutNumber(layout.height)}};
    return object;
}

bool identity(std::string_view id) {
    return !id.empty() && id.size() <= 128 &&
        std::all_of(id.begin(), id.end(), [](unsigned char c) {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') || c == '_' || c == '-' || c == '.';
        });
}

bool displayName(std::string_view name) {
    return !name.empty() && name.size() <= 1024 &&
        name.find_first_not_of(" ") != std::string_view::npos &&
        std::none_of(name.begin(), name.end(), [](unsigned char c) { return c < 32 || c == 127; });
}

bool dependency(const ClassDependency &ref, std::string_view extension) {
    const auto actualExtension = ref.path.size() < extension.size() ? std::string_view{} :
        std::string_view(ref.path).substr(ref.path.size() - extension.size());
    const auto equalExtension = std::ranges::equal(actualExtension, extension, [](char a, char b) {
        return (a >= 'A' && a <= 'Z' ? a - 'A' + 'a' : a) == b;
    });
    if (!identity(ref.id) || !ref.revision || ref.path.size() > 1024 ||
        !equalExtension || ref.path.starts_with('/') ||
        ref.path.find_first_of(":\\") != std::string::npos ||
        std::any_of(ref.path.begin(), ref.path.end(), [](unsigned char c) { return c < 32 || c == 127; })) return false;
    std::string_view remaining = ref.path;
    while (true) {
        const auto next = remaining.find('/');
        const auto part = remaining.substr(0, next);
        if (part.empty() || part == ".") return false;
        if (next == remaining.npos) return true;
        remaining.remove_prefix(next + 1);
    }
}

ClassDependency readDependency(const Json &object, const std::string &path) {
    keys(object, {"id", "path", "revision"}, path);
    return {str(object, "id", path), str(object, "path", path), revision(object, "revision", path)};
}

Json writeDependency(const ClassDependency &dependency) {
    auto result = cj::object();
    result.object = {{"id", cj::string(dependency.id)}, {"path", cj::string(dependency.path)},
        {"revision", number(dependency.revision)}};
    return result;
}

std::string_view visibilityName(ClassVisibility visibility) {
    switch (visibility) {
    case ClassVisibility::Public: return "public";
    case ClassVisibility::Protected: return "protected";
    case ClassVisibility::Private: return "private";
    }
    return {};
}

binding::TypeRef dataType(std::string_view name, const std::string &path) {
    // Validate the shared Record grammar's size/depth before recursive parsing;
    // wrapping a leaf in a Record also restricts it to the admitted value types.
    const auto wrapped = "cluster<\"value\":" + std::string(name) + ">";
    const auto schema = canonicalRecordTransportType(wrapped);
    require(schema && schema->children.size() == 1, path,
        "Expected an exact canonical immutable Record value type.");
    return schema->children.front();
}

bool supportedType(const binding::TypeRef &type) {
    const auto record = binding::TypeRef::cluster({"value"}, {type});
    const auto validated = canonicalRecordTransportType(record.canonical());
    return validated && validated->children.size() == 1 && validated->children.front() == type;
}

bool validDefault(const ClassField &field) {
    const auto record = binding::TypeRef::cluster({"value"}, {field.type});
    auto value = cj::object();
    value.object["value"] = field.defaultValue;
    return canonicalRecordLiteralValid(record, value);
}

std::string atIndex(std::string_view section, std::size_t index) {
    return std::string(section) + '/' + std::to_string(index);
}

bool compatibleOverride(const ClassMethod &base, const ClassMethod &derived) {
    if (base.dispatch != ClassDispatch::Dynamic || derived.dispatch != ClassDispatch::Dynamic ||
        base.visibility != derived.visibility || base.ports.size() != derived.ports.size()) return false;
    return std::all_of(base.ports.begin(), base.ports.end(), [&](const ClassPort &port) {
        const auto it = std::find_if(derived.ports.begin(), derived.ports.end(),
            [&](const ClassPort &candidate) { return candidate.slot == port.slot; });
        return it != derived.ports.end() && it->input == port.input && it->kind == port.kind &&
            (it->kind == ClassPortKind::Object || binding::fullTypeEqual(port.dataType, it->dataType));
    });
}

std::string fieldSlot(std::string_view classId, std::string_view fieldId) {
    return cj::encodedId("field", classId) + ":" + std::string(fieldId);
}

constexpr std::string_view tagSlot = "class:concrete";

CanonicalObjectReferencePort physicalPort(std::string id, const binding::TypeRef &type, bool input) {
    return {std::move(id), canonicalObjectReferencePhysicalType(type).canonical(),
        cj::encode(canonicalObjectReferenceWriteType(type)), input, true, {}};
}
} // namespace

std::optional<ClassDependency> readClassDependency(const source::JsonValue &value,std::string_view extension,std::string *error) {
    try {
        auto result=readDependency(value,"/class");
        require(dependency(result,extension),"/class","Invalid class dependency identity, path or revision.");
        return result;
    } catch(const InvalidClass &failure){if(error)*error=failure.what();return {};}
}

std::optional<ClassDefinition> readClassDefinition(std::string_view bytes, std::string *error) {
    if (error) error->clear();
    try {
        require(bytes.size() <= maxBytes, "/", "Class definition exceeds 4 MiB.");
        std::string parseError;
        const auto root = source::parseJson(bytes, &parseError, source::JsonNumberPolicy::PreserveLexeme);
        require(root.has_value(), "/", parseError);
        keys(*root, {"format", "version", "id", "name", "revision", "parent", "fields", "methods"}, "");
        require(str(*root, "format", "") == kClassDefinitionFormat && revision(*root, "version", "") == 1,
            "/version", "Unsupported class definition format/version.");
        ClassDefinition result;
        result.id = str(*root, "id", "");
        result.name = str(*root, "name", "");
        result.revision = revision(*root, "revision", "");
        const auto &parent = any(*root, "parent", "");
        if (parent.type != J::Null) result.parent = readDependency(parent, "/parent");
        const auto &fields = get(*root, "fields", J::Array, "");
        require(fields.array.size() <= maxMembers, "/fields", "Too many fields.");
        for (const auto &entry : fields.array) {
            const auto path = atIndex("/fields", result.fields.size());
            const auto *layout = entry.member("layout");
            if (layout)
                keys(entry, {"id", "label", "type", "default", "layout"}, path);
            else
                keys(entry, {"id", "label", "type", "default"}, path);
            result.fields.push_back({str(entry, "id", path), str(entry, "label", path),
                dataType(str(entry, "type", path), path + "/type"), any(entry, "default", path),
                layout ? std::optional<Rect>{readFieldLayout(*layout, path + "/layout")} : std::nullopt});
        }
        const auto &methods = get(*root, "methods", J::Array, "");
        require(methods.array.size() <= maxMembers, "/methods", "Too many methods.");
        for (const auto &entry : methods.array) {
            const auto path = atIndex("/methods", result.methods.size());
            keys(entry, {"id", "name", "implementation", "visibility", "dispatch", "overrides", "ports"}, path);
            ClassMethod method;
            method.id = str(entry, "id", path);
            method.name = str(entry, "name", path);
            method.implementation = readDependency(get(entry, "implementation", J::Object, path), path + "/implementation");
            const auto visibility = str(entry, "visibility", path);
            if (visibility == "public") method.visibility = ClassVisibility::Public;
            else if (visibility == "protected") method.visibility = ClassVisibility::Protected;
            else require(visibility == "private", path + "/visibility", "Unknown method visibility.");
            const auto dispatch = str(entry, "dispatch", path);
            if (dispatch == "dynamic") method.dispatch = ClassDispatch::Dynamic;
            else require(dispatch == "static", path + "/dispatch", "Unknown method dispatch.");
            const auto &overrides = any(entry, "overrides", path);
            if (overrides.type != J::Null) {
                keys(overrides, {"class_id", "method_id"}, path + "/overrides");
                method.overrides = ClassOverride{str(overrides, "class_id", path + "/overrides"),
                    str(overrides, "method_id", path + "/overrides")};
            }
            const auto &ports = get(entry, "ports", J::Array, path);
            require(ports.array.size() <= maxPorts, path + "/ports", "Too many connector slots.");
            for (const auto &port : ports.array) {
                const auto p = atIndex(path + "/ports", method.ports.size());
                keys(port, {"slot", "port_id", "direction", "kind", "type"}, p);
                ClassPort value;
                value.slot = str(port, "slot", p);
                value.publicPortId = str(port, "port_id", p);
                const auto direction = str(port, "direction", p);
                require(direction == "input" || direction == "output", p + "/direction", "Unknown port direction.");
                value.input = direction == "input";
                const auto kind = str(port, "kind", p);
                require(kind == "object" || kind == "data", p + "/kind", "Unknown port kind.");
                if (kind == "object") {
                    value.kind = ClassPortKind::Object;
                    (void)get(port, "type", J::Null, p);
                } else value.dataType = dataType(str(port, "type", p), p + "/type");
                method.ports.push_back(std::move(value));
            }
            result.methods.push_back(std::move(method));
        }
        return result;
    } catch (const InvalidClass &e) {
        if (error) *error = e.what();
        return {};
    }
}

std::string writeClassDefinition(const ClassDefinition &definition) {
    auto root = cj::object();
    root.object = {{"format", cj::string(std::string(kClassDefinitionFormat))}, {"version", number(1)},
        {"id", cj::string(definition.id)}, {"name", cj::string(definition.name)}, {"revision", number(definition.revision)},
        {"parent", definition.parent ? writeDependency(*definition.parent) : Json{}}};
    auto fields = cj::array();
    for (const auto &field : definition.fields) {
        auto entry = cj::object();
        entry.object = {{"id", cj::string(field.id)}, {"label", cj::string(field.label)},
            {"type", cj::string(field.type.canonical())}, {"default", field.defaultValue}};
        if (field.layout) entry.object.emplace("layout", writeFieldLayout(*field.layout));
        fields.array.push_back(std::move(entry));
    }
    auto methods = cj::array();
    for (const auto &method : definition.methods) {
        auto entry = cj::object();
        auto overrides = Json{};
        if (method.overrides) {
            overrides = cj::object();
            overrides.object = {{"class_id", cj::string(method.overrides->classId)},
                {"method_id", cj::string(method.overrides->methodId)}};
        }
        auto ports = cj::array();
        for (const auto &port : method.ports) {
            auto value = cj::object();
            value.object = {{"slot", cj::string(port.slot)}, {"port_id", cj::string(port.publicPortId)},
                {"direction", cj::string(port.input ? "input" : "output")},
                {"kind", cj::string(port.kind == ClassPortKind::Object ? "object" : "data")},
                {"type", port.kind == ClassPortKind::Object ? Json{} : cj::string(port.dataType.canonical())}};
            ports.array.push_back(std::move(value));
        }
        entry.object = {{"id", cj::string(method.id)}, {"name", cj::string(method.name)},
            {"implementation", writeDependency(method.implementation)},
            {"visibility", cj::string(std::string(visibilityName(method.visibility)))},
            {"dispatch", cj::string(method.dispatch == ClassDispatch::Dynamic ? "dynamic" : "static")},
            {"overrides", std::move(overrides)}, {"ports", std::move(ports)}};
        methods.array.push_back(std::move(entry));
    }
    root.object["fields"] = std::move(fields);
    root.object["methods"] = std::move(methods);
    return cj::encode(root) + '\n';
}

std::optional<ClassField> prepareClassField(std::string id, std::string label,
    std::string_view canonicalType, std::string_view defaultJson, std::string *error) {
    if (error) error->clear();
    try {
        require(identity(id), "/id", "Invalid field identity.");
        require(displayName(label), "/label", "Invalid field label.");
        require(canonicalType.size() < 65536 && defaultJson.size() <= maxBytes, "/default", "Field exceeds its input budget.");
        std::string parseError;
        const auto initial = source::parseJson(defaultJson, &parseError, source::JsonNumberPolicy::PreserveLexeme);
        require(initial.has_value(), "/default", parseError);
        ClassField field{std::move(id), std::move(label), dataType(canonicalType, "/type"), *initial, std::nullopt};
        require(validDefault(field), "/default", "The default does not match the exact admitted field type.");
        return field;
    } catch (const InvalidClass &e) {
        if (error) *error = e.what();
        return {};
    }
}

std::vector<ClassIssue> validateClassImplementation(const ClassDefinition &definition,
    std::size_t methodIndex, const ClassImplementationInterface &implementation) {
    std::vector<ClassIssue> issues;
    const auto path = atIndex("/methods", methodIndex);
    const auto report = [&](std::string code, std::string suffix, std::string message) {
        issues.push_back({std::move(code), definition.id, path + suffix, std::move(message), {}});
    };
    if (methodIndex >= definition.methods.size()) {
        report("class.method_missing", "", "No declaration for this method.");
        return issues;
    }
    const auto &method = definition.methods[methodIndex];
    if (method.implementation.id != implementation.id || method.implementation.revision != implementation.revision)
        report("class.method_dependency", "/implementation", "The captured member identity or revision does not match.");
    if (method.ports.size() != implementation.ports.size())
        report("class.method_connector", "/ports", "The captured member has a different number of public ports.");
    for (std::size_t i = 0; i < method.ports.size(); ++i) {
        const auto &declared = method.ports[i];
        const auto match = [&](const ClassImplementationPort &port) { return port.id == declared.publicPortId; };
        const auto found = std::find_if(implementation.ports.begin(), implementation.ports.end(), match);
        if (found == implementation.ports.end() ||
            std::find_if(found + 1, implementation.ports.end(), match) != implementation.ports.end() ||
            found->input != declared.input || found->kind != declared.kind ||
            (declared.kind == ClassPortKind::Object ? found->objectClassId != definition.id :
                !binding::fullTypeEqual(declared.dataType, found->dataType)))
            report("class.method_connector", "/ports/" + std::to_string(i), "Member public port identity, direction or type differs from the declaration.");
    }
    return issues;
}

std::optional<ClassHierarchy> ClassHierarchy::validate(ClassDefinitions definitions, std::vector<ClassIssue> &issues) {
    issues.clear();
    const auto report = [&](std::string code, const std::string &id, std::string path, std::string message) {
        if (issues.size() < 256) issues.push_back({std::move(code), id, std::move(path), std::move(message), {}});
    };
    if (definitions.empty() || definitions.size() > kMaximumClassClosure) {
        report("class.closure_limit", "", "", "A class closure must contain between 1 and 1024 definitions.");
        return {};
    }
    ClassHierarchy hierarchy(std::move(definitions));
    // No two members in the same closure may claim the same implementation.
    // Privilege is bound to a unique captured member, never a caller-supplied name.
    std::set<std::string> implementations;
    for (const auto &[id, value] : hierarchy.definitions_) {
        if (id != value.id || !identity(id)) report("class.identity", id, "/id", "Invalid or mismatched class identity.");
        if (!displayName(value.name)) report("class.name", id, "/name", "Invalid class name.");
        if (!value.revision) report("class.revision", id, "/revision", "Definition revision must be positive.");
        if (value.parent) {
            const auto *parent = hierarchy.findClass(value.parent->id);
            if (!dependency(*value.parent, kClassDefinitionExtension) || !parent || parent->revision != value.parent->revision)
                report("class.parent_dependency", id, "/parent", "The parent reference or captured identity/revision does not match.");
            std::set<std::string_view> visited;
            for (const auto *ancestor = &value; ancestor; ancestor = ancestor->parent ? hierarchy.findClass(ancestor->parent->id) : nullptr) {
                if (!visited.insert(ancestor->id).second) {
                    report("class.inheritance_cycle", id, "/parent", "A class cannot inherit from itself or one of its descendants.");
                    break;
                }
                if (visited.size() > kMaximumClassDepth) {
                    report("class.inheritance_limit", id, "/parent", "Inheritance depth exceeds 32 classes.");
                    break;
                }
            }
        }
        if (value.fields.size() > maxMembers || value.methods.size() > maxMembers) {
            report("class.member_limit", id, "", "Class definition exceeds the field or method limit.");
            continue;
        }
        std::set<std::string> fields, methods;
        for (std::size_t i = 0; i < value.fields.size(); ++i) {
            const auto &field = value.fields[i];
            const auto path = atIndex("/fields", i);
            if (!identity(field.id) || !fields.insert(field.id).second)
                report("class.field_identity", id, path + "/id", "Field identity must be valid and unique in its declaring class.");
            if (!displayName(field.label)) report("class.field_label", id, path + "/label", "Invalid field label.");
            if (!supportedType(field.type) || !validDefault(field))
                report("class.field_default", id, path + "/default", "The default does not match the exact admitted field type.");
        }
        std::set<std::pair<std::string, std::string>> overrideSlots;
        for (std::size_t i = 0; i < value.methods.size(); ++i) {
            const auto &method = value.methods[i];
            const auto path = atIndex("/methods", i);
            if (!identity(method.id) || !methods.insert(method.id).second)
                report("class.method_identity", id, path + "/id", "Method identity must be valid and unique in its declaring class.");
            if (!displayName(method.name)) report("class.method_name", id, path + "/name", "Invalid method name.");
            if (!dependency(method.implementation, ".frog") || !implementations.insert(method.implementation.id).second)
                report("class.method_dependency", id, path + "/implementation", "The member FROG dependency needs a valid, unique identity and revision.");
            if (method.dispatch == ClassDispatch::Dynamic && method.visibility == ClassVisibility::Private)
                report("class.private_dispatch", id, path + "/visibility", "A dynamic method cannot be private.");
            std::set<std::string> slots, ports;
            unsigned objectInputs = 0, objectOutputs = 0;
            if (method.ports.size() > maxPorts)
                report("class.port_limit", id, path + "/ports", "Too many connector slots.");
            else for (std::size_t p = 0; p < method.ports.size(); ++p) {
                const auto &port = method.ports[p];
                if (!identity(port.slot) || !slots.insert(port.slot).second ||
                    !identity(port.publicPortId) || !ports.insert(port.publicPortId).second ||
                    (port.kind == ClassPortKind::Data && !supportedType(port.dataType)) ||
                    (port.kind == ClassPortKind::Object && port.dataType.kind != binding::TypeKind::Unknown))
                    report("class.method_port", id, atIndex(path + "/ports", p), "Invalid slot identity, public port identity or port type.");
                if (port.kind == ClassPortKind::Object) (port.input ? objectInputs : objectOutputs)++;
            }
            if (objectInputs != 1 || objectOutputs > 1)
                report("class.object_ports", id, path + "/ports", "A member needs one object input and at most one object output.");
            if (method.overrides) {
                const ClassCallTarget target{method.overrides->classId, method.overrides->methodId};
                const auto *base = hierarchy.findMethod(target);
                // Overrides always name the original slot, even through an intermediate override.
                if (target.classId == id || !hierarchy.assignable(id, target.classId) || !base || base->overrides ||
                    !compatibleOverride(*base, method) || !overrideSlots.emplace(target.classId, target.methodId).second)
                    report("class.override_signature", id, path + "/overrides",
                        "An override must match a unique ancestor dynamic slot, including visibility, port directions and data types.");
            }
        }
    }
    if (!issues.empty()) return {};
    return hierarchy;
}

const ClassDefinition *ClassHierarchy::findClass(std::string_view id) const {
    const auto found = definitions_.find(id);
    return found != definitions_.end() ? &found->second : nullptr;
}

const ClassMethod *ClassHierarchy::findMethod(const ClassCallTarget &target) const {
    const auto *owner = findClass(target.classId);
    if (!owner) return nullptr;
    const auto found = std::find_if(owner->methods.begin(), owner->methods.end(),
        [&](const auto &method) { return method.id == target.methodId; });
    return found != owner->methods.end() ? &*found : nullptr;
}

bool ClassHierarchy::assignable(std::string_view from, std::string_view to) const {
    if (!findClass(to)) return false;
    const auto *current = findClass(from);
    for (std::size_t i = 0; current && i < kMaximumClassDepth; ++i) {
        if (current->id == to) return true;
        current = current->parent ? findClass(current->parent->id) : nullptr;
    }
    return false;
}

binding::TypeResolution ClassHierarchy::connectObjects(const binding::TypeRef &from,const binding::TypeRef &to) const {
    if(from.kind!=binding::TypeKind::Object||to.kind!=binding::TypeKind::Object||!from.known()||!to.known())
        return {binding::ResolutionStatus::Incompatible,{},"class.object_required","Only a nominal object value can connect to an object terminal."};
    if(!findClass(from.name)||!findClass(to.name))
        return {binding::ResolutionStatus::Unresolved,{},"class.dependency_unavailable","The declared class is absent from the captured closure."};
    if(assignable(from.name,to.name))return {binding::ResolutionStatus::Compatible,to,{},{}};
    return {binding::ResolutionStatus::Incompatible,{},assignable(to.name,from.name)?"class.downcast_forbidden":"class.unrelated_types",
        "A parent cannot connect directly to a child; unrelated nominal classes remain incompatible."};
}
std::optional<binding::TypeRef> ClassHierarchy::commonObjectType(std::span<const binding::TypeRef> values) const {
    if(values.empty())return {};
    for(const auto &value:values)if(value.kind!=binding::TypeKind::Object||!value.known()||!findClass(value.name))return {};
    for(auto *candidate=findClass(values.front().name);candidate;candidate=candidate->parent?findClass(candidate->parent->id):nullptr)
        if(std::all_of(values.begin(),values.end(),[&](const auto &v){return assignable(v.name,candidate->id);}))return binding::TypeRef::object(candidate->id);
    return {};
}

bool ClassHierarchy::canAccess(const std::optional<ClassCallTarget> &caller, const ClassCallTarget &target) const {
    const auto *method = findMethod(target);
    if (!method) return false;
    if (method->visibility == ClassVisibility::Public) return true;
    if (!caller || !findMethod(*caller)) return false;
    return method->visibility == ClassVisibility::Private ? caller->classId == target.classId :
        assignable(caller->classId, target.classId);
}

bool ClassHierarchy::canAccessField(const ClassCallTarget &caller, std::string_view owner, std::string_view field) const {
    if (caller.classId != owner || !findMethod(caller)) return false;
    const auto *definition = findClass(owner);
    return definition && std::any_of(definition->fields.begin(), definition->fields.end(),
        [&](const auto &entry) { return entry.id == field; });
}

std::optional<ClassRecordLayout> ClassHierarchy::recordLayout(std::string_view wireClass, ClassIssue *issue) const {
    if (issue) *issue = {};
    const auto *root = findClass(wireClass);
    if (!root) return {};
    while (root->parent) root = findClass(root->parent->id);
    ClassRecordLayout result;
    // Include the root's nominal ID in the physical schema as well. The
    // private Source admission boundary must still prevent arbitrary Records
    // from taking the place of an object from the same family.
    std::vector<std::string> fields{std::string(tagSlot), cj::encodedId("family", root->id)};
    std::vector<binding::TypeRef> types{binding::TypeRef::primitive("u32"), binding::TypeRef::primitive("bool")};
    for (const auto &[id, definition] : definitions_) {
        if (!assignable(id, root->id)) continue;
        result.discriminants.emplace(id, static_cast<std::uint32_t>(result.discriminants.size() + 1));
        for (const auto &field : definition.fields) {
            fields.push_back(fieldSlot(id, field.id));
            types.push_back(field.type);
        }
    }
    result.schema = binding::TypeRef::cluster(std::move(fields), std::move(types));
    CanonicalRecordDiagnostic diagnostic;
    if (!canonicalRecordTransportType(result.schema.canonical(), &diagnostic)) {
        if (issue) *issue = {"class.transport_limit", root->id, "/fields", std::move(diagnostic.message), {}};
        return {};
    }
    return result;
}

std::optional<source::JsonValue> ClassHierarchy::defaultRecord(std::string_view concreteClass,
    const ClassRecordLayout &layout) const {
    const auto canonical = recordLayout(concreteClass);
    if (!canonical || canonical->schema != layout.schema || canonical->discriminants != layout.discriminants) return {};
    auto value = cj::object();
    value.object.emplace(std::string(tagSlot), number(layout.discriminants.at(std::string(concreteClass))));
    value.object.emplace(layout.schema.fieldIds[1], cj::boolean(true));
    for (const auto &[id, ignored] : layout.discriminants) {
        (void)ignored;
        for (const auto &field : findClass(id)->fields)
            value.object.emplace(fieldSlot(id, field.id), field.defaultValue);
    }
    if (!canonicalRecordLiteralValid(layout.schema, value)) return {};
    return value;
}

std::optional<std::string> ClassHierarchy::privateRecordSlot(const ClassCallTarget &caller,
    std::string_view wireClass, std::string_view field) const {
    if (!assignable(wireClass, caller.classId) || !canAccessField(caller, caller.classId, field)) return {};
    return fieldSlot(caller.classId, field);
}

std::optional<CanonicalObjectReferenceDescriptor> ClassHierarchy::lowerDefaultObject(
    std::string_view concreteClass, std::string_view pointer) const {
    const auto layout = recordLayout(concreteClass);
    if (!layout) return {};
    const auto literal = defaultRecord(concreteClass, *layout);
    if (!literal) return {};
    return canonicalObjectReferenceRecordDescriptor(CanonicalObjectReferenceKind::Resolve, layout->schema,
        {physicalPort("value", layout->schema, false)}, {}, &*literal, pointer);
}

std::optional<CanonicalObjectReferenceDescriptor> ClassHierarchy::lowerPrivateField(
    const ClassCallTarget &caller, std::string_view wireClass, std::string_view field, bool update,
    std::string_view pointer) const {
    const auto slot = privateRecordSlot(caller, wireClass, field);
    if (!slot) return {};
    const auto layout = recordLayout(wireClass);
    if (!layout) return {};
    const auto &schema = layout->schema;
    const auto fieldId = std::find(schema.fieldIds.begin(), schema.fieldIds.end(), *slot);
    if (fieldId == schema.fieldIds.end()) return {};
    const auto &type = schema.children[static_cast<std::size_t>(fieldId - schema.fieldIds.begin())];
    std::vector<CanonicalObjectReferencePort> ports{physicalPort("cluster", schema, true),
        physicalPort("element1", type, update)};
    if (update) ports.push_back(physicalPort("result", schema, false));
    return canonicalObjectReferenceRecordDescriptor(
        update ? CanonicalObjectReferenceKind::Update : CanonicalObjectReferenceKind::Select,
        schema, std::move(ports), {{*slot}}, nullptr, pointer);
}

std::optional<ClassDispatchPlan> ClassHierarchy::planDynamicCall(std::string_view wireClass,
    const ClassCallTarget &declaration, const std::optional<ClassCallTarget> &caller, std::string_view pointer) const {
    const auto *method = findMethod(declaration);
    if (!method || method->dispatch != ClassDispatch::Dynamic) return {};
    const auto targets = dispatchTargets(wireClass, declaration, caller);
    auto layout = recordLayout(wireClass);
    if (!layout || targets.empty()) return {};
    auto selector = canonicalObjectReferenceRecordDescriptor(CanonicalObjectReferenceKind::Select, layout->schema,
        {physicalPort("cluster", layout->schema, true), physicalPort("element1", binding::TypeRef::primitive("u32"), false)},
        {{std::string(tagSlot)}}, nullptr, pointer);
    if (!selector) return {};
    ClassDispatchPlan plan{std::move(*layout), std::move(*selector), {}};
    for (const auto &[id, implementation] : targets)
        plan.arms.push_back({plan.layout.discriminants.at(id), id, implementation});
    return plan;
}

std::map<std::string, ClassCallTarget, std::less<>> ClassHierarchy::dispatchTargets(
    std::string_view wireClass, const ClassCallTarget &declaration,
    const std::optional<ClassCallTarget> &caller) const {
    std::map<std::string, ClassCallTarget, std::less<>> targets;
    const auto *method = findMethod(declaration);
    if (!method || method->overrides || !assignable(wireClass, declaration.classId) || !canAccess(caller, declaration))
        return targets;
    for (const auto &[id, definition] : definitions_) {
        if (!assignable(id, wireClass)) continue;
        auto selected = declaration;
        if (method->dispatch == ClassDispatch::Dynamic) {
            for (auto *current = &definition; current && current->id != declaration.classId;
                current = current->parent ? findClass(current->parent->id) : nullptr) {
                const auto override = std::find_if(current->methods.begin(), current->methods.end(), [&](const ClassMethod &candidate) {
                    return candidate.overrides && candidate.overrides->classId == declaration.classId &&
                        candidate.overrides->methodId == declaration.methodId;
                });
                if (override != current->methods.end()) {
                    selected = {current->id, override->id};
                    break;
                }
            }
        }
        targets.emplace(id, std::move(selected));
    }
    return targets;
}

CapturedClassHierarchy captureClassHierarchy(std::span<const ClassDependencyRoot> roots,
    const DraftDependencyResolver &resolver) {
    constexpr std::size_t maxClosureBytes = 64U * 1024 * 1024;
    CapturedClassHierarchy result;
    ClassDefinitions definitions;
    std::map<std::string, std::size_t, std::less<>> locations;
    std::map<std::string, std::string, std::less<>> classUris;
    std::size_t bytes = 0;
    const auto issue = [&](const ClassDependencyRoot &ref, std::string code, std::string message) {
        if (result.issues.size() < 256)
            result.issues.push_back({std::move(code), ref.reference.id, ref.pointer, std::move(message), ref.ownerUri});
    };
    if (roots.empty() || roots.size() > kMaximumClassClosure) {
        issue({}, "class.closure_limit", "Expected between 1 and 1024 root class references.");
        return result;
    }
    std::vector<ClassDependencyRoot> pending(roots.begin(), roots.end());
    for (std::size_t i = 0; i < pending.size(); ++i) {
        const auto ref = pending[i]; // appending a parent may reallocate pending
        if (!dependency(ref.reference, kClassDefinitionExtension)) {
            issue(ref, "class.dependency_reference", "Invalid class dependency reference.");
            continue;
        }
        auto captured = resolveDraftDependency({DraftDependencyKind::ClassDefinition, ref.ownerUri,
            ref.reference.path, ref.pointer, {}, {}, {}}, resolver);
        if (captured.state != DraftDependencyState::Present || !captured.bytes) {
            issue(ref, captured.diagnosticCode.empty() ? "class.dependency_unavailable" : captured.diagnosticCode,
                captured.message.empty() ? "The class dependency could not be captured." : std::move(captured.message));
            continue;
        }
        if (const auto it = locations.find(captured.canonicalUri); it != locations.end()) {
            const auto &previous = result.definitions[it->second];
            if (previous.sha256 != captured.sha256)
                issue(ref, "class.dependency_changed", "The same class location resolved to different bytes.");
            else if (previous.identity != ref.reference.id || previous.version != std::to_string(ref.reference.revision))
                issue(ref, "class.dependency_identity", "The class identity or revision does not match the reference.");
            continue;
        }
        if (definitions.size() >= kMaximumClassClosure || captured.bytes->size() > maxClosureBytes - bytes) {
            issue(ref, "class.closure_limit", "The captured class closure exceeds its size limit.");
            break;
        }
        std::string error;
        auto parsed = readClassDefinition(*captured.bytes, &error);
        if (!parsed) {
            issue(ref, "class.dependency_format", std::move(error));
            continue;
        }
        if (parsed->id != ref.reference.id || parsed->revision != ref.reference.revision ||
            (!captured.identity.empty() && captured.identity != parsed->id) ||
            (!captured.version.empty() && captured.version != std::to_string(parsed->revision))) {
            issue(ref, "class.dependency_identity", "The class identity or revision does not match the reference.");
            continue;
        }
        if (!classUris.emplace(parsed->id, captured.canonicalUri).second) {
            issue(ref, "class.dependency_duplicate", "Two different class locations claim the same nominal identity.");
            continue;
        }
        captured.identity = parsed->id;
        captured.version = std::to_string(parsed->revision);
        if (parsed->parent) pending.push_back({captured.canonicalUri, "/parent", *parsed->parent});
        bytes += captured.bytes->size();
        locations.emplace(captured.canonicalUri, result.definitions.size());
        result.definitions.push_back(std::move(captured));
        const auto key = parsed->id;
        definitions.emplace(key, std::move(*parsed));
    }
    if (!result.issues.empty()) return result;
    result.hierarchy = ClassHierarchy::validate(std::move(definitions), result.issues);
    for (auto &entry : result.issues)
        if (const auto it = classUris.find(entry.classId); it != classUris.end()) entry.sourceUri = it->second;
    return result;
}
} // namespace frog::native_engine
