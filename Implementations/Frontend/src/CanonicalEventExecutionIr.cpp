#include "frog_native_engine/CanonicalFrontendMetrics.hpp"
#include "frog_native_engine/CanonicalEventExecutionIr.hpp"
#include "frog_native_engine/Sha256.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <deque>
#include <set>

namespace frog::native_engine {
namespace {
using Json = source::JsonValue;
namespace rt = sdk::runtime;
struct Failure { CanonicalIrDiagnostic diagnostic; };
[[noreturn]] void fail(std::string code, std::string message, std::string location, bool unsupported = false) {
    throw Failure{{std::move(code), std::move(message), std::move(location), unsupported}};
}
const Json &object(const Json *v, std::string_view at) {
    if (!v || v->type != Json::Type::Object) fail("EVENT_IR_OBJECT", "An object is required.", std::string(at));
    return *v;
}
const std::vector<Json> &array(const Json *v, std::string_view at, std::size_t max, bool optional = false) {
    static const std::vector<Json> empty;
    if (!v && optional) return empty;
    if (!v || v->type != Json::Type::Array) fail("EVENT_IR_ARRAY", "An array is required.", std::string(at));
    if (v->array.size() > max) fail("EVENT_IR_LIMIT", "Collection exceeds its budget.", std::string(at), true);
    return v->array;
}
std::string text(const Json *v, std::string_view at, bool empty = false) {
    if (!v || v->type != Json::Type::String || (!empty && v->string.empty())) fail("EVENT_IR_STRING", "A string is required.", std::string(at));
    return v->string;
}
void fields(const Json &v, std::initializer_list<std::string_view> allowed, std::string_view at) {
    for (const auto &[key, _] : v.object) if (std::find(allowed.begin(), allowed.end(), key) == allowed.end())
        fail("EVENT_IR_FIELD_UNSUPPORTED", "Field is outside this public Event profile: " + key, std::string(at), true);
}
void requiredText(const Json &v, std::string_view key, std::string_view expected, std::string_view at) {
    if (text(v.member(key), at) != expected) fail("EVENT_IR_SOURCE_CONTRACT", "Source has the wrong " + std::string(key) + ".", std::string(at));
}
void requiredBool(const Json &v, std::string_view key, bool expected, std::string_view at) {
    const auto *f = v.member(key);
    if (!f || f->type != Json::Type::Boolean || f->boolean != expected) fail("EVENT_IR_SOURCE_CONTRACT", "Source has the wrong " + std::string(key) + ".", std::string(at));
}
bool canonicalId(std::string_view v) {
    return !v.empty() && std::all_of(v.begin(), v.end(), [](unsigned char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '.' || c == '_' || c == ':' || c == '-';
    });
}
// Exact numeric comparison against one, without rounding a Source version.
// Legal equivalent JSON forms such as 1.0/10e-1 retain the same integer value.
bool selectionVersionOne(const Json *v) {
    if (!v || v->type != Json::Type::Number || v->string.empty() || v->string.front() == '-') return false;
    const auto &token = v->string; const auto exponentAt = token.find_first_of("eE");
    const auto end = exponentAt == token.npos ? token.size() : exponentAt;
    std::size_t digits = 0, beforeDecimal = 0, oneAt = 0; bool decimal = false, foundOne = false;
    for (std::size_t i = 0; i < end; ++i) {
        const auto c = token[i];
        if (c == '.') { if (decimal) return false; decimal = true; beforeDecimal = digits; continue; }
        if (c < '0' || c > '9') return false;
        if (c != '0') { if (c != '1' || foundOne) return false; foundOne = true; oneAt = digits; }
        ++digits;
    }
    if (!foundOne) return false;
    if (!decimal) beforeDecimal = digits;
    std::int64_t exponent = 0;
    if (exponentAt != token.npos) {
        auto spelling = std::string_view(token).substr(exponentAt + 1);
        if (spelling.starts_with('+')) spelling.remove_prefix(1);
        const auto parsed = std::from_chars(spelling.data(), spelling.data() + spelling.size(), exponent);
        if (parsed.ec != std::errc{} || parsed.ptr != spelling.data() + spelling.size()) return false;
    }
    return exponent == static_cast<std::int64_t>(oneAt) + 1 - static_cast<std::int64_t>(beforeDecimal);
}
Json str(std::string_view v) { Json j; j.type = Json::Type::String; j.string = v; return j; }
Json obj(std::initializer_list<std::pair<const std::string, Json>> members) { Json j; j.type = Json::Type::Object; j.object = members; return j; }
Json list(std::vector<Json> values = {}) { Json j; j.type = Json::Type::Array; j.array = std::move(values); return j; }
std::string quote(std::string_view v) {
    constexpr char hex[] = "0123456789abcdef"; std::string r = "\"";
    for (const unsigned char c : v) {
        if (c == '"' || c == '\\') { r += '\\'; r += static_cast<char>(c); }
        else if (c < 32) { r += "\\u00"; r += hex[c >> 4]; r += hex[c & 15]; }
        else r += static_cast<char>(c);
    }
    return r + '"';
}
std::string json(const Json &v) {
    if (v.type == Json::Type::Null) return "null";
    if (v.type == Json::Type::Boolean) return v.boolean ? "true" : "false";
    if (v.type == Json::Type::String) return quote(v.string);
    if (v.type == Json::Type::Number) return v.string;
    std::string r = v.type == Json::Type::Object ? "{" : "["; bool first = true;
    if (v.type == Json::Type::Object) for (const auto &[key, entry] : v.object) {
        if (!first) r += ','; first = false; r += quote(key) + ':' + json(entry);
    } else for (const auto &entry : v.array) { if (!first) r += ','; first = false; r += json(entry); }
    return r + (v.type == Json::Type::Object ? '}' : ']');
}
bool equal(const Json &a, const Json &b) {
    if (a.type != b.type) return false;
    if (a.type == Json::Type::Boolean) return a.boolean == b.boolean;
    if (a.type == Json::Type::String || a.type == Json::Type::Number) return a.string == b.string;
    if (a.type == Json::Type::Array) {
        if (a.array.size() != b.array.size()) return false;
        for (std::size_t i = 0; i < a.array.size(); ++i) if (!equal(a.array[i], b.array[i])) return false;
    } else if (a.type == Json::Type::Object) {
        if (a.object.size() != b.object.size()) return false;
        for (const auto &[key, v] : a.object) { const auto *found = b.member(key); if (!found || !equal(v, *found)) return false; }
    }
    return true;
}
// Declared FROG wire types, reused from the established Pane catalogue.
// No NI enum code, runtime pointer or private editor alias is inferred.
const std::array<std::pair<std::string_view, std::string_view>, 5> paneFields{{
    {"pane_ref", R"(ref<"frog.front_panel">)"},
    {"coords", R"(cluster<"x":f64,"y":f64>)"},
    {"button", "u32"},
    {"mods", R"(cluster<"menu_key":bool,"keypad":bool,"double_click":bool>)"},
    {"platform_mods", R"(cluster<"shift":bool,"control":bool,"alt":bool,"windows":bool>)"}
}};
bool capturedFields(const rt::EventPayloadSchema &schema) {
    return std::any_of(schema.begin(), schema.end(), [](const auto &field) {
        return field.second.category == rt::ValueCategory::Cluster || field.second.category == rt::ValueCategory::ObjectReference;
    });
}
bool supportedType(std::string_view type) {
    constexpr std::array names{"bool", "string", "path", "f64", "i8", "i16", "i32", "i64", "u8", "u16", "u32", "u64"};
    return std::find(names.begin(), names.end(), type) != names.end();
}
rt::RuntimeValue scalar(const Json *v, std::string_view type, std::string_view at) {
    if (!v) fail("EVENT_IR_DEFAULT_REQUIRED", "No scalar default is invented.", std::string(at), true);
    if (type == "bool") {
        if (v->type != Json::Type::Boolean) fail("EVENT_IR_VALUE", "A Boolean is required.", std::string(at));
        return v->boolean;
    }
    if (type == "string" || type == "path") return text(v, at, true);
    if (v->type != Json::Type::Number) fail("EVENT_IR_VALUE", "A number is required.", std::string(at));
    const auto &token = v->string; const auto declared = rt::valueTypeFromCanonicalName(type);
    if (type == "f64") {
        double result = 0; const auto read = std::from_chars(token.data(), token.data() + token.size(), result);
        if (read.ec != std::errc{} || read.ptr != token.data() + token.size() || !std::isfinite(result)) fail("EVENT_IR_VALUE", "A finite f64 is required.", std::string(at));
        if (token.find_first_of(".eE") == token.npos && token != "-0") {
            std::array<char, 512> exact{}; const auto written = std::to_chars(exact.data(), exact.data() + exact.size(), result, std::chars_format::fixed, 0);
            if (written.ec != std::errc{} || std::string_view(exact.data(), static_cast<std::size_t>(written.ptr - exact.data())) != token)
                fail("EVENT_IR_VALUE", "Integer literal loses precision in f64.", std::string(at));
        }
        return token == "-0" ? 0. : result;
    }
    if (declared.category == rt::ValueCategory::SignedInteger) {
        std::int64_t result = 0; const auto read = std::from_chars(token.data(), token.data() + token.size(), result);
        if (read.ec != std::errc{} || read.ptr != token.data() + token.size()) fail("EVENT_IR_VALUE", "An exact signed integer is required.", std::string(at));
        if (declared.bitWidth < 64) { const auto limit = std::int64_t{1} << (declared.bitWidth - 1); if (result < -limit || result >= limit) fail("EVENT_IR_VALUE", "Signed value exceeds its type.", std::string(at)); }
        return result;
    }
    if (declared.category == rt::ValueCategory::UnsignedInteger) {
        std::uint64_t result = 0; const auto read = std::from_chars(token.data(), token.data() + token.size(), result);
        if (read.ec != std::errc{} || read.ptr != token.data() + token.size()) fail("EVENT_IR_VALUE", "An exact unsigned integer is required.", std::string(at));
        if (declared.bitWidth < 64 && result >= (std::uint64_t{1} << declared.bitWidth)) fail("EVENT_IR_VALUE", "Unsigned value exceeds its type.", std::string(at));
        return result;
    }
    fail("EVENT_IR_TYPE_UNSUPPORTED", "Type has no Event transport.", std::string(at), true);
}
struct Widget { const Json *declaration; std::string path, id, klass, role, type; std::optional<rt::RuntimeValue> initial; };
struct Draft {
    const FrogSourceDocument &source; const CanonicalIrReadOptions &options;
    std::string documentId, structureId, sourceStructureId, structurePath, timeoutWidget;
    std::int64_t timeoutDefault = -1;
    std::map<std::string, Widget> widgets;
    std::map<std::string, std::vector<std::string>> widgetTargets;
    std::vector<CanonicalEventRegion> regions;
    std::vector<Json> objects, connections, regionRecords, mappings, correspondence;
    std::set<std::string> ids;
    std::size_t nodeCount = 0, edgeCount = 0, recordBytes = 0, derivedDescriptorBytes = 0;
    void append(std::vector<Json> &records, Json record) {
        const auto size = json(record).size(), maximum = options.limits.maximumIrBytes;
        if (size > maximum || recordBytes > maximum - size) fail("EVENT_IR_LIMIT", "Canonical construction exceeds its aggregate byte budget.", "/", true);
        recordBytes += size; records.push_back(std::move(record));
    }
    std::string ref(std::string_view path) const { return options.sourceUri + "#" + std::string(path); }
    std::string sourceId(const Json *v, std::string_view at) const {
        auto r = text(v, at); if (r.size() > options.limits.maximumSourceIdBytes) fail("EVENT_IR_LIMIT", "Source identity exceeds its budget.", std::string(at), true); return r;
    }
    std::string encoded(std::string_view prefix, std::string_view identity) {
        constexpr char hex[] = "0123456789abcdef"; std::string r(prefix); r += ':';
        for (const unsigned char c : identity) { r += hex[c >> 4]; r += hex[c & 15]; }
        if (r.size() > options.limits.maximumIrIdBytes) fail("EVENT_IR_LIMIT", "Canonical identity exceeds its byte budget.", r, true);
        if (!ids.insert(r).second) fail("EVENT_IR_ID", "Canonical identity is duplicated.", r);
        return r;
    }
    void mapping(std::string_view path, std::string_view kind, std::string_view target, std::string_view targetKind, bool support = false) {
        if (mappings.size() >= options.limits.maximumMappingRecords) fail("EVENT_IR_LIMIT", "Mapping budget exceeded.", std::string(path), true);
        append(mappings, obj({{"id", str(encoded("map", std::to_string(mappings.size())))}, {"source_kind", str(kind)}, {"source_ref", str(ref(path))},
            {"target_kind", str(targetKind)}, {"target_refs", target.empty() ? list() : list({str(target)})}, {"relation", str(support ? "support_only" : target.empty() ? "non_primary" : "direct_primary")}}));
    }
    void nonPrimary(std::string_view path, std::string_view target = {}, std::string_view kind = "non_primary_outcome") {
        if (correspondence.size() >= options.limits.maximumMappingRecords) fail("EVENT_IR_LIMIT", "Correspondence budget exceeded.", std::string(path), true);
        append(correspondence, obj({{"id", str(encoded("correspondence", std::to_string(correspondence.size())))}, {"source_ref", str(ref(path))},
            {"relation_kind", str(kind)}, {"target_refs", target.empty() ? list() : list({str(target)})}}));
    }
    Json attribution(std::string_view path, std::optional<std::string_view> second = {}) const {
        std::vector<Json> refs{str(ref(path))}; if (second) refs.push_back(str(ref(*second)));
        return obj({{"source_refs", list(std::move(refs))}, {"mode", str(second ? "multi_contributor" : "direct")}});
    }
    Json port(std::string_view name, bool input, std::string_view type, std::string_view boundary = "none", std::string_view role = "data") const {
        return obj({{"id", str(name)}, {"direction", str(input ? "in" : "out")}, {"role", str(role)}, {"type_ref", str(type)}, {"boundary_kind", str(boundary)}});
    }
    void addNode(const CanonicalEventNode &node, std::string_view path, const Widget *widget = nullptr) {
        if (++nodeCount > options.limits.maximumNodes) fail("EVENT_IR_LIMIT", "Object budget exceeded.", std::string(path), true);
        std::vector<Json> ports; for (const auto &p : node.ports) ports.push_back(port(p.id, p.input, p.typeRef, node.family == "support" ? "structure_boundary" : "none"));
        Json attrs = obj({{"source_node_id", str(node.sourceNodeId)}});
        if (!node.typeRef.empty()) attrs.object["value_type"] = str(node.typeRef);
        if (node.kind == "frog.core.unbundle.by_name") {
            Json version; version.type = Json::Type::Number; version.string = "1";
            attrs.object["selected_fields_version"] = std::move(version);
            std::vector<Json> selected; std::size_t index = 0;
            for (const auto &p : node.ports) if (!p.input)
                selected.push_back(obj({{"port", str(p.id)}, {"field_id", str(node.selectedFields.at(index++))}}));
            attrs.object["selected_fields"] = list(std::move(selected));
        }
        if (widget) { attrs.object["widget_id"] = str(widget->id); attrs.object["widget_class"] = str(widget->klass);
            if (const auto *props = widget->declaration->member("props"); props && props->member("default_value")) attrs.object["default_value"] = *props->member("default_value"); }
        if (!node.fieldId.empty()) attrs.object[node.role == "event_response" ? "event_response_field" : "event_data_field"] = str(node.fieldId);
        if (node.suppliedByEvent) { Json deferred; deferred.type = Json::Type::Boolean; deferred.boolean = true; attrs.object["supplied_by_event"] = std::move(deferred); }
        auto classification = obj({{"primary_or_support", str(node.family == "support" ? "support" : "primary")}});
        if (node.family == "primitive") classification.object["library_id"] = str("frog.core");
        append(objects, obj({{"id", str(node.id)}, {"family", str(node.family)}, {"kind", str(node.kind)}, {"role", str(node.role)},
            {"classification", std::move(classification)}, {"ports", list(std::move(ports))}, {"attributes", std::move(attrs)},
            {"attribution", attribution(path, widget ? std::optional<std::string_view>{widget->path} : std::nullopt)}}));
        mapping(path, node.family == "support" ? "structure_terminal" : "diagram_node", node.id, node.family == "support" ? "support_object" : "object", node.family == "support");
        if (widget) nonPrimary(widget->path, node.id, "declaration_reference");
        if (node.kind == "frog.core.unbundle.by_name") {
            mapping(std::string(path) + "/selected_fields_version", "other", node.id, "object");
            mapping(std::string(path) + "/selected_fields", "other", node.id, "object");
            for (std::size_t i = 0; i < node.selectedFields.size(); ++i) {
                const auto selectionPath = std::string(path) + "/selected_fields/" + std::to_string(i);
                mapping(selectionPath, "other", node.id, "object");
                mapping(selectionPath + "/port", "other", node.id, "object");
                mapping(selectionPath + "/field_id", "other", node.id, "object");
            }
        }
    }
    void addConnection(const CanonicalEventConnection &edge, std::string_view path) {
        if (++edgeCount > options.limits.maximumEdges) fail("EVENT_IR_LIMIT", "Connection budget exceeded.", std::string(path), true);
        const auto endpoint = [](const CanonicalIrEndpoint &e) { return obj({{"object_id", str(e.objectId)}, {"port_id", str(e.portId)}}); };
        append(connections, obj({{"id", str(edge.id)}, {"kind", str("dataflow")}, {"from", endpoint(edge.from)}, {"to", endpoint(edge.to)}, {"attribution", attribution(path)}}));
        mapping(path, "diagram_edge", edge.id, "connection");
    }
    void readWidgets(const Json &root);
    CanonicalEventNode widgetNode(const Json &node, std::string_view path, std::string_view region, bool emit = true);
    void resolveUnbundles(CanonicalEventRegion &region);
    void readRegion(const Json &raw, std::size_t ordinal);
    void admitRegion(const CanonicalEventRegion &region) const;
    Json build();
};
void Draft::readWidgets(const Json &root) {
    const auto &panel = object(root.member("front_panel"), "/front_panel"); fields(panel, {"widgets", "canvas"}, "/front_panel");
    const auto &declarations = array(panel.member("widgets"), "/front_panel/widgets", options.limits.maximumWidgets);
    for (std::size_t i = 0; i < declarations.size(); ++i) {
        const auto path = "/front_panel/widgets/" + std::to_string(i); const auto &decl = object(&declarations[i], path);
        fields(decl, {"id", "role", "widget", "class_ref", "value_type", "layout", "props", "parts", "doc", "tags"}, path);
        Widget widget{&decl, path, sourceId(decl.member("id"), path), text(decl.member("widget") ? decl.member("widget") : decl.member("class_ref"), path),
            text(decl.member("role"), path), text(decl.member("value_type"), path), {}};
        if (!supportedType(widget.type)) fail("EVENT_IR_TYPE_UNSUPPORTED", "Composite, nominal, f32 or unknown widget types have no transport here.", path, true);
        if (widget.role != "control" && widget.role != "indicator") fail("EVENT_IR_WIDGET_ROLE", "Control or indicator role is required.", path);
        const auto suffix = widget.role == "control" ? "_control" : "_indicator";
        const auto family = widget.type == "bool" ? "boolean" : widget.type == "string" ? "string" : widget.type == "path" ? "path" : "numeric";
        if (widget.klass != "frog.widgets." + std::string(family) + suffix && widget.klass != "frog.ui.standard." + std::string(family) + suffix)
            fail("EVENT_IR_WIDGET_CLASS", "Widget class must match its scalar type and role.", path, true);
        if (decl.member("widget") && decl.member("class_ref") && text(decl.member("class_ref"), path) != widget.klass) fail("EVENT_IR_WIDGET_CLASS", "Class references conflict.", path);
        if (const auto *props = decl.member("props")) {
            object(props, path); fields(*props, {"default_value", "data_type.representation"}, path + "/props");
            if (props->member("data_type.representation") && text(props->member("data_type.representation"), path) != widget.type) fail("EVENT_IR_WIDGET_TYPE", "Representation and value type conflict.", path);
            if (props->member("default_value")) widget.initial = scalar(props->member("default_value"), widget.type, path + "/props/default_value");
        }
        if (widget.role == "control" && !widget.initial) fail("EVENT_IR_DEFAULT_REQUIRED", "Control needs a real source default.", path, true);
        if (const auto *parts = decl.member("parts")) {
            object(parts, path); fields(*parts, {"label", "caption"}, path);
            for (const auto &[name, part] : parts->object) {
                object(&part, path); fields(part, {"class", "class_ref", "props", "layout"}, path);
                const auto klass = text(part.member("class") ? part.member("class") : part.member("class_ref"), path);
                if (klass != "frog.widgets.label" && klass != "frog.ui.standard.label_part") fail("EVENT_IR_WIDGET_PART", "Unsupported widget part.", path, true);
                if (part.member("class") && part.member("class_ref") && text(part.member("class_ref"), path) != klass) fail("EVENT_IR_WIDGET_CLASS", "Part class references conflict.", path);
                if (const auto *props = part.member("props")) {
                    object(props, path); fields(*props, {"text", "visible", "style"}, path);
                    if (props->member("text")) text(props->member("text"), path, true);
                    if (const auto *visible = props->member("visible"); visible && visible->type != Json::Type::Boolean) fail("EVENT_IR_WIDGET_PART", "Visibility must be Boolean.", path);
                }
                nonPrimary(path + "/parts/" + name);
            }
        }
        const auto widgetId = widget.id;
        if (!widgets.emplace(widgetId, std::move(widget)).second) fail("EVENT_IR_DUPLICATE_WIDGET", "Widget identity is duplicated.", path);
    }
    if (panel.member("canvas")) nonPrimary("/front_panel/canvas");
}
CanonicalEventNode Draft::widgetNode(const Json &node, std::string_view path, std::string_view region, bool emit) {
    fields(node, {"id", "kind", "widget", "value_type", "layout", "doc", "tags"}, path); requiredText(node, "kind", "widget_value", path);
    const auto sid = sourceId(node.member("id"), path), wid = sourceId(node.member("widget"), path); const auto found = widgets.find(wid);
    if (found == widgets.end()) fail("EVENT_IR_WIDGET_MISSING", "Participation has no actual widget declaration.", std::string(path));
    const auto &widget = found->second;
    if (node.member("value_type") && text(node.member("value_type"), path) != widget.type) fail("EVENT_IR_WIDGET_TYPE", "Node and widget types conflict.", std::string(path));
    CanonicalEventNode result{encoded("node", std::to_string(region.size()) + ":" + std::string(region) + sid), sid, ref(path), "widget_value", "widget_value", widget.role, wid, {}, widget.type,
        {{"value", widget.role == "indicator", widget.type}}, widget.initial};
    widgetTargets[wid].push_back(result.id);
    if (emit) addNode(result, path, &widget);
    for (const auto key : {"layout", "doc", "tags"}) if (node.member(key)) nonPrimary(std::string(path) + "/" + key);
    return result;
}
void Draft::readRegion(const Json &raw, std::size_t ordinal) {
    const auto path = structurePath + "/regions/" + std::to_string(ordinal); const auto &decl = object(&raw, path);
    fields(decl, {"id", "event", "event_label", "event_data_fields", "event_payload_fields", "event_response_fields", "diagram"}, path);
    CanonicalEventRegion region; region.sourceRegionId = sourceId(raw.member("id"), path); region.id = encoded("region", region.sourceRegionId); region.sourceRef = ref(path);
    const auto &event = object(raw.member("event"), path + "/event"); fields(event, {"kind", "source", "scope"}, path + "/event");
    const auto kind = text(event.member("kind"), path); const auto sourceName = event.member("source") ? sourceId(event.member("source"), path) : std::string{};
    const bool paneFilter = kind == "mouse_down_filter";
    const bool paneMouse = kind == "mouse_down" || kind == "mouse_up" || paneFilter;
    if (event.member("scope") && !paneMouse) fail("EVENT_IR_SCOPE_UNSUPPORTED", "An explicit Pane scope belongs only to admitted Pane mouse notifications.", path, true);
    if (kind == "timeout") { if (!sourceName.empty()) fail("EVENT_IR_TIMEOUT", "Timeout has no producer source.", path); }
    else if (kind == "value_change") {
        if (!sourceName.starts_with("widget:") || !widgets.contains(sourceName.substr(7)) || widgets.at(sourceName.substr(7)).role != "control")
            fail("EVENT_IR_SOURCE_MISSING", "Value Change must identify an actual scalar control as widget:<id>.", path);
        region.subscription = rt::EventIdentity{"control", sourceName, kind, false};
    } else if (kind == "user") region.subscription = rt::EventIdentity{sourceName.empty() ? "application" : "vi", sourceName, kind, false};
    else if (paneMouse) {
        requiredText(event, "scope", "pane", path); requiredText(event, "source", "/front_panel", path);
        region.subscription = rt::EventIdentity{"pane", sourceName, kind, paneFilter};
    } else fail("EVENT_IR_NOTIFICATION_UNSUPPORTED", "No public source/runtime adapter exists for this notification kind.", path, true);
    std::map<std::string, std::string> payloadPaths;
    if (const auto *payload = raw.member("event_payload_fields")) {
        if (kind != "value_change" && !paneMouse)
            fail("EVENT_IR_PAYLOAD_KIND", "No typed payload contract is declared for this event kind.", path);
        const auto &declared = array(payload, path + "/event_payload_fields", paneMouse ? 5 : 2);
        if (declared.size() != (paneMouse ? paneFields.size() : 2))
            fail("EVENT_IR_PAYLOAD_CONTRACT", "Every actual field must be declared once in its established order.", path);
        for (std::size_t i = 0; i < declared.size(); ++i) {
            const auto fieldPath = path + "/event_payload_fields/" + std::to_string(i);
            const auto &field = object(&declared[i], fieldPath); fields(field, {"id", "type", "body_port"}, fieldPath);
            const std::string name = paneMouse ? std::string(paneFields[i].first) : i == 0 ? "old_value" : "new_value";
            const std::string type = paneMouse ? std::string(paneFields[i].second) : widgets.at(sourceName.substr(7)).type;
            requiredText(field, "id", name, fieldPath); requiredText(field, "type", type, fieldPath);
            requiredText(field, "body_port", "event_data." + name, fieldPath);
            region.payloadSchema.emplace(name, rt::valueTypeFromCanonicalName(type));
            payloadPaths.emplace(name, fieldPath);
            mapping(fieldPath, "other", structureId, "object");
        }
    } else if (paneMouse) fail("EVENT_IR_PAYLOAD_REQUIRED", "Pane Mouse must explicitly declare its five captured fields.", path);
    std::set<std::string> selected; std::map<std::string, CanonicalIrEndpoint> eventInputs;
    for (const auto &field : array(raw.member("event_data_fields"), path + "/event_data_fields", paneMouse ? 8 : 5)) {
        const auto name = text(&field, path);
        const bool occurrenceValue = region.payloadSchema.contains(name);
        if (!selected.insert(name).second || (!occurrenceValue && name != "source" && name != "type" && name != "time") ||
            (occurrenceValue && !region.payloadSchema.contains(name)))
            fail("EVENT_IR_EVENT_FIELD", "Selected fields must be distinct, actually declared Event Data identities.", path);
        const auto index = name == "source" ? 0U : name == "type" ? 1U : 2U;
        const auto fieldPath = occurrenceValue ? payloadPaths.at(name) : structurePath + "/structure_terminals/event_data/fields/" + std::to_string(index);
        const auto type = occurrenceValue ? region.payloadSchema.at(name).canonicalName : std::string{name == "time" ? "u64" : "string"};
        const auto id = encoded("event-field", region.sourceRegionId + ":" + name);
        std::optional<rt::RuntimeValue> initial;
        if (!(paneMouse && occurrenceValue)) initial = occurrenceValue ? *widgets.at(sourceName.substr(7)).initial :
            name == "time" ? rt::RuntimeValue{std::uint64_t{0}} : rt::RuntimeValue{std::string{}};
        CanonicalEventNode support{id, sourceStructureId, ref(fieldPath), "support", "event_data", "event_data", {}, name, type, {{"value", false, type}}, initial};
        support.suppliedByEvent = paneMouse && occurrenceValue;
        addNode(support, fieldPath); mapping(path + "/event_data_fields", "other", id, "support_object", true);
        region.nodes.push_back(std::move(support)); eventInputs.emplace("event_data." + name, CanonicalIrEndpoint{id, "value"});
    }
    std::map<std::string, std::string> responsePaths;
    std::map<std::string, CanonicalIrEndpoint> eventOutputs;
    if (const auto *responses = raw.member("event_response_fields")) {
        if (!paneFilter) fail("EVENT_IR_RESPONSE_KIND", "Only a declared Mouse Down filter has response terminals.", path);
        const auto &declared = array(responses, path + "/event_response_fields", 4);
        for (std::size_t i = 0; i < declared.size(); ++i) {
            const auto fieldPath = path + "/event_response_fields/" + std::to_string(i);
            const auto &field = object(&declared[i], fieldPath); fields(field, {"id", "type", "body_port"}, fieldPath);
            const auto name = text(field.member("id"), fieldPath);
            if (name != "discard" && name != "button" && name != "mods" && name != "platform_mods")
                fail("EVENT_IR_RESPONSE_READ_ONLY", "This field has no verified writable Mouse Down response.", fieldPath);
            const auto type = name == "discard" ? std::string{"bool"} : region.payloadSchema.at(name).canonicalName;
            requiredText(field, "type", type, fieldPath); requiredText(field, "body_port", "event_response." + name, fieldPath);
            if (!region.responseSchema.emplace(name, rt::valueTypeFromCanonicalName(type)).second)
                fail("EVENT_IR_RESPONSE_DUPLICATE", "A response field must be declared once.", fieldPath);
            responsePaths.emplace(name, fieldPath); nonPrimary(fieldPath);
        }
    }
    const auto &graph = object(raw.member("diagram"), path + "/diagram"); fields(graph, {"nodes", "edges", "annotations", "wire_fragments"}, path);
    if (!array(graph.member("wire_fragments"), path, options.limits.maximumEdges, true).empty()) fail("EVENT_IR_INCOMPLETE_SOURCE", "Incomplete body wires cannot execute.", path);
    std::map<std::string, std::size_t> local; std::set<std::string> participants;
    const auto &nodes = array(graph.member("nodes"), path, options.limits.maximumNodes);
    // Preserve the historical Add-only emission byte path. Regions containing
    // Unbundle defer body realization until actual edges resolve every type.
    const bool twoPhase = std::any_of(nodes.begin(), nodes.end(), [](const auto &n) {
        const auto *type = n.member("type");
        return type && type->type == Json::Type::String && type->string == "frog.core.unbundle.by_name";
    });
    std::map<std::string, std::pair<std::string, const Widget *>> deferredNodes;
    std::vector<std::pair<std::string, std::string>> deferredNonPrimary;
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        const auto np = path + "/diagram/nodes/" + std::to_string(i); const auto &rn = object(&nodes[i], np);
        const auto family = text(rn.member("kind"), np), sid = sourceId(rn.member("id"), np);
        if (sid == sourceStructureId || !local.emplace(sid, region.nodes.size()).second) fail("EVENT_IR_DUPLICATE_NODE", "Body node identity is ambiguous.", np);
        if (family == "widget_value") {
            auto node = widgetNode(rn, np, region.sourceRegionId, !twoPhase);
            if (node.widgetId == timeoutWidget || !participants.insert(node.widgetId).second) fail("EVENT_IR_WIDGET_PARTICIPATION", "Repeated/timeout widget body participation is outside this profile.", np, true);
            if (twoPhase) deferredNodes.emplace(node.id, std::pair{np, &widgets.at(node.widgetId)});
            region.nodes.push_back(std::move(node));
        } else if (family == "primitive") {
            const auto operation = text(rn.member("type"), np);
            CanonicalEventNode node{encoded("node", std::to_string(region.sourceRegionId.size()) + ":" + region.sourceRegionId + sid), sid, ref(np), "primitive", operation, "operation", {}, {}, {}, {}, {}};
            if (operation == "frog.core.unbundle.by_name") {
                fields(rn, {"id", "kind", "type", "layout", "doc", "tags", "selected_fields_version", "selected_fields"}, np);
                if (!selectionVersionOne(rn.member("selected_fields_version")))
                    fail("EVENT_IR_UNBUNDLE_VERSION", "selected_fields_version must be the integer 1.", np + "/selected_fields_version");
                const auto *selection = rn.member("selected_fields");
                if (!selection || selection->type != Json::Type::Array || selection->array.empty())
                    fail("EVENT_IR_UNBUNDLE_SELECTION_REQUIRED", "An ordered, nonempty selected_fields array is required.", np + "/selected_fields");
                const auto &selected = array(selection, np + "/selected_fields", 64);
                node.ports.push_back({"record", true, {}});
                std::set<std::string> outputIds;
                for (std::size_t index = 0; index < selected.size(); ++index) {
                    const auto sp = np + "/selected_fields/" + std::to_string(index);
                    const auto &field = object(&selected[index], sp); fields(field, {"port", "field_id"}, sp);
                    const auto pid = text(field.member("port"), sp + "/port");
                    if (pid.size() > std::min<std::size_t>(1024, options.limits.maximumSourceIdBytes))
                        fail("EVENT_IR_LIMIT", "Unbundle output identity exceeds this target's port-byte budget.", sp + "/port", true);
                    if (!canonicalId(pid) || pid == "record" || !outputIds.insert(pid).second)
                        fail("EVENT_IR_UNBUNDLE_PORT", "Output ports require distinct, bounded IR id_string identities other than record.", sp + "/port");
                    const auto fid = text(field.member("field_id"), sp + "/field_id");
                    if (fid.size() > 1024)
                        fail("EVENT_IR_LIMIT", "Unbundle field identity exceeds this target's byte budget.", sp + "/field_id", true);
                    if (!rt::structural_detail::identity(fid))
                        fail("EVENT_IR_UNBUNDLE_FIELD", "field_id must be an exact bounded canonical cluster identity.", sp + "/field_id");
                    node.selectedFields.push_back(fid); node.ports.push_back({pid, false, {}});
                }
            } else {
                fields(rn, {"id", "kind", "type", "layout", "doc", "tags"}, np);
                if (operation != "frog.core.add") fail("EVENT_IR_PRIMITIVE_UNSUPPORTED", "Only f64 Add and versioned cluster Unbundle have Event body providers.", np, true);
                node.typeRef = "f64"; node.ports = {{"a", true, "f64"}, {"b", true, "f64"}, {"result", false, "f64"}};
            }
            if (twoPhase) deferredNodes.emplace(node.id, std::pair{np, static_cast<const Widget *>(nullptr)});
            else addNode(node, np);
            region.nodes.push_back(std::move(node));
            for (const auto key : {"layout", "doc", "tags"}) if (rn.member(key)) {
                if (twoPhase) deferredNonPrimary.emplace_back(np + "/" + key, std::string{});
                else nonPrimary(np + "/" + key);
            }
        } else fail("EVENT_IR_BODY_FAMILY_UNSUPPORTED", "Nested structures, calls, references and state have no Event body adapter here.", np, true);
    }
    std::set<std::string> edgeIds; const auto &edges = array(graph.member("edges"), path, options.limits.maximumEdges);
    for (std::size_t i = 0; i < edges.size(); ++i) {
        const auto ep = path + "/diagram/edges/" + std::to_string(i); const auto &re = object(&edges[i], ep);
        fields(re, {"id", "from", "to", "bends", "layout", "doc", "tags"}, ep); const auto sid = sourceId(re.member("id"), ep);
        if (!edgeIds.insert(sid).second) fail("EVENT_IR_DUPLICATE_EDGE", "Body edge identity is duplicated.", ep);
        const auto endpoint = [&](std::string_view side, bool input) {
            const auto &rawEnd = object(re.member(side), ep); fields(rawEnd, {"node", "port"}, ep);
            const auto name = sourceId(rawEnd.member("node"), ep), pid = text(rawEnd.member("port"), ep);
            if (name == sourceStructureId) {
                if (!input && pid == "timeout_value") fail("EVENT_IR_TIMEOUT_BODY_UNSUPPORTED", "Reading the timeout body value requires a separate boundary realization.", ep, true);
                if (!input) {
                    if (!eventInputs.contains(pid)) fail("EVENT_IR_EVENT_FIELD", "Only selected read-only Event Data fields are available.", ep);
                    return eventInputs.at(pid);
                }
                if (!pid.starts_with("event_response.") || !region.responseSchema.contains(pid.substr(15)))
                    fail("EVENT_IR_RESPONSE_PORT", "Only explicitly declared writable response terminals accept a body wire.", ep);
                const auto field = pid.substr(15);
                if (const auto existing = eventOutputs.find(pid); existing != eventOutputs.end()) return existing->second;
                const auto type = region.responseSchema.at(field).canonicalName;
                const auto id = encoded("event-response", region.sourceRegionId + ":" + field);
                CanonicalEventNode support{id, sourceStructureId, ref(responsePaths.at(field)), "support", "event_response", "event_response", {}, field, type, {{"value", true, type}}, {}};
                if (twoPhase) deferredNodes.emplace(id, std::pair{responsePaths.at(field), static_cast<const Widget *>(nullptr)});
                else addNode(support, responsePaths.at(field));
                region.nodes.push_back(std::move(support));
                const auto endpoint = CanonicalIrEndpoint{id, "value"}; eventOutputs.emplace(pid, endpoint); return endpoint;
            }
            const auto found = local.find(name); if (found == local.end()) fail("EVENT_IR_ENDPOINT_SCOPE", "Endpoint is outside its owned Event region.", ep);
            const auto &node = region.nodes[found->second]; const auto port = std::find_if(node.ports.begin(), node.ports.end(), [&](const auto &p) { return p.id == pid && p.input == input; });
            if (port == node.ports.end()) fail("EVENT_IR_PORT", "Port identity or direction is invalid.", ep);
            return CanonicalIrEndpoint{node.id, pid};
        };
        CanonicalEventConnection edge{encoded("edge", std::to_string(region.sourceRegionId.size()) + ":" + region.sourceRegionId + sid), ref(ep), endpoint("from", false), endpoint("to", true)};
        region.connections.push_back(edge); if (!twoPhase) addConnection(edge, ep);
        for (const auto key : {"bends", "layout", "doc", "tags"}) if (re.member(key)) {
            if (twoPhase) deferredNonPrimary.emplace_back(ep + "/" + key, std::string{});
            else nonPrimary(ep + "/" + key);
        }
    }
    if (twoPhase) resolveUnbundles(region);
    admitRegion(region);
    if (twoPhase) {
        for (const auto &node : region.nodes) if (const auto found = deferredNodes.find(node.id); found != deferredNodes.end())
            addNode(node, found->second.first, found->second.second);
        for (const auto &edge : region.connections) addConnection(edge, std::string_view(edge.sourceRef).substr(options.sourceUri.size() + 1));
        for (const auto &[np, unused] : deferredNonPrimary) nonPrimary(np);
    }
    std::vector<Json> members; for (const auto &node : region.nodes) members.push_back(str(node.id));
    append(regionRecords, obj({{"id", str(region.id)}, {"owner_object_id", str(structureId)}, {"role", str("case_arm")}, {"member_object_ids", list(std::move(members))}, {"attribution", attribution(path)}}));
    mapping(path, "other", region.id, "region");
    if (raw.member("event_label")) { text(raw.member("event_label"), path, true); nonPrimary(path + "/event_label"); }
    if (graph.member("annotations")) nonPrimary(path + "/diagram/annotations");
    regions.push_back(std::move(region));
}
void Draft::resolveUnbundles(CanonicalEventRegion &region) {
    std::map<std::string, CanonicalEventNode *> nodes;
    std::map<std::string, std::size_t> indegree;
    std::map<std::string, std::vector<std::string>> outgoing;
    std::map<std::pair<std::string, std::string>, const CanonicalEventConnection *> drivers;
    const auto pointer = [&](std::string_view reference) { return std::string(reference.substr(options.sourceUri.size() + 1)); };
    for (auto &node : region.nodes) { nodes.emplace(node.id, &node); indegree.emplace(node.id, 0); }
    for (const auto &edge : region.connections) {
        if (!drivers.emplace(std::pair{edge.to.objectId, edge.to.portId}, &edge).second)
            fail("EVENT_IR_INPUT_DRIVEN_TWICE", "A single input has multiple sources.", pointer(edge.sourceRef));
        ++indegree.at(edge.to.objectId); outgoing[edge.from.objectId].push_back(edge.to.objectId);
    }
    std::deque<std::string> ready;
    for (const auto &node : region.nodes) {
        for (const auto &port : node.ports) if (port.input && !drivers.contains({node.id, port.id}))
            fail("EVENT_IR_INPUT_MISSING", "Every admitted body input needs its actual dataflow producer.", pointer(node.sourceRef));
        if (!indegree.at(node.id)) ready.push_back(node.id);
    }
    const auto descriptor = [&](std::string_view type, std::string_view at) {
        const auto maximum = std::min<std::size_t>(options.limits.maximumIrBytes, 16U * 1024U * 1024U);
        if (type.size() > maximum || derivedDescriptorBytes > maximum - type.size())
            fail("EVENT_IR_UNBUNDLE_DESCRIPTOR_LIMIT", "Derived type descriptors exceed the aggregate byte budget.", std::string(at), true);
        derivedDescriptorBytes += type.size();
    };
    std::size_t visited = 0;
    while (!ready.empty()) {
        const auto id = std::move(ready.front()); ready.pop_front(); ++visited;
        auto &node = *nodes.at(id);
        if (node.kind == "frog.core.unbundle.by_name") {
            const auto np = pointer(node.sourceRef);
            const auto &edge = *drivers.at({node.id, "record"});
            const auto &sourceNode = *nodes.at(edge.from.objectId);
            const auto sourcePort = std::find_if(sourceNode.ports.begin(), sourceNode.ports.end(), [&](const auto &p) { return !p.input && p.id == edge.from.portId; });
            const auto &type = sourcePort->typeRef;
            if (rt::valueTypeFromCanonicalName(type).category != rt::ValueCategory::Cluster ||
                !rt::structural_detail::TypeSyntax(type).transport())
                fail("EVENT_IR_UNBUNDLE_RECORD_REQUIRED", "record must be driven by an actual transportable canonical cluster.", np + "/selected_fields");
            const auto fields = rt::structural_detail::TypeSyntax(type).recordFields();
            if (!fields) fail("EVENT_IR_UNBUNDLE_RECORD_REQUIRED", "The driver lost its canonical cluster schema.", np + "/selected_fields");
            // Charge each descriptor before cloning it into the retained graph.
            descriptor(type, np); node.ports.front().typeRef = type;
            for (std::size_t i = 0; i < node.selectedFields.size(); ++i) {
                const auto field = std::find_if(fields->begin(), fields->end(), [&](const auto &f) { return f.first == node.selectedFields[i]; });
                if (field == fields->end()) fail("EVENT_IR_UNBUNDLE_FIELD", "Selected field is absent from the actual driver cluster.", np + "/selected_fields/" + std::to_string(i) + "/field_id");
                descriptor(field->second, np); node.ports[i + 1].typeRef = field->second;
            }
        }
        for (const auto &next : outgoing[id]) if (--indegree.at(next) == 0) ready.push_back(next);
    }
    if (visited != region.nodes.size()) fail("EVENT_IR_BODY_CYCLE", "Event body contains a dataflow cycle without declared state.", pointer(region.sourceRef));
}
void Draft::admitRegion(const CanonicalEventRegion &region) const {
    std::map<std::string, const CanonicalEventNode *> nodes;
    std::map<std::string, std::size_t> indegree;
    std::map<std::string, std::vector<std::string>> outgoing;
    std::set<std::pair<std::string, std::string>> drivers;
    for (const auto &node : region.nodes) { nodes.emplace(node.id, &node); indegree.emplace(node.id, 0); }
    const auto pointer = [&](std::string_view reference) { return std::string(reference.substr(options.sourceUri.size() + 1)); };
    for (const auto &edge : region.connections) {
        const auto &from = *nodes.at(edge.from.objectId), &to = *nodes.at(edge.to.objectId);
        const auto sourcePort = std::find_if(from.ports.begin(), from.ports.end(), [&](const auto &p) { return p.id == edge.from.portId && !p.input; });
        const auto sinkPort = std::find_if(to.ports.begin(), to.ports.end(), [&](const auto &p) { return p.id == edge.to.portId && p.input; });
        if (sourcePort->typeRef != sinkPort->typeRef) fail("EVENT_IR_WIRE_TYPE", "Source and sink must have exactly the same canonical type.", pointer(edge.sourceRef));
        if (!drivers.emplace(to.id, sinkPort->id).second) fail("EVENT_IR_INPUT_DRIVEN_TWICE", "A single input has multiple sources.", pointer(edge.sourceRef));
        ++indegree.at(to.id); outgoing[from.id].push_back(to.id);
    }
    std::deque<std::string> ready;
    for (const auto &node : region.nodes) {
        for (const auto &port : node.ports) if (port.input && !drivers.contains({node.id, port.id}))
            fail("EVENT_IR_INPUT_MISSING", "Every admitted body input needs its actual dataflow producer.", pointer(node.sourceRef));
        if (!indegree.at(node.id)) ready.push_back(node.id);
    }
    std::size_t visited = 0;
    while (!ready.empty()) {
        const auto id = std::move(ready.front()); ready.pop_front(); ++visited;
        for (const auto &next : outgoing[id]) if (--indegree.at(next) == 0) ready.push_back(next);
    }
    if (visited != region.nodes.size()) fail("EVENT_IR_BODY_CYCLE", "Event body contains a dataflow cycle without declared state.", pointer(region.sourceRef));
}
Json Draft::build() {
    if(options.metrics)options.metrics->eventAdmissionIncludesAssembly=true;
    CanonicalFrontendPhaseScope admission(options.metrics,CanonicalFrontendPhase::Semantics);
    if (source.envelopeStatus() != FrogSourceEnvelopeStatus::Recognized) fail("EVENT_IR_SOURCE_ENVELOPE", "Recognized public source authority is required.", "/", true);
    if (source.originalUtf8().size() > options.limits.maximumSourceBytes) fail("EVENT_IR_LIMIT", "Source exceeds its admitted byte budget.", "/", true);
    if (options.sourceUri.size() > 4096) fail("EVENT_IR_LIMIT", "Source URI exceeds its byte budget.", "/", true);
    if (options.sourceUri.empty() || options.sourceUri.find('#') != std::string::npos) fail("EVENT_IR_SOURCE_IDENTITY", "A fragment-free source URI is required.", "/");
    if (documentId.size() > options.limits.maximumIrIdBytes) fail("EVENT_IR_LIMIT", "Document ID exceeds its byte budget.", "/document_id", true);
    if (!canonicalId(documentId) || documentId == "unit:event") fail("EVENT_IR_DOCUMENT_ID", "Distinct canonical document/unit identities are required.", "/document_id");
    ids.insert(documentId); ids.insert("unit:event");
    const auto &root = object(&source.root(), "/");
    fields(root, {"spec_version", "metadata", "interface", "diagram", "front_panel", "connector", "icon", "ide", "cache", "host", "execution_policy"}, "/");
    const auto &boundary = object(root.member("interface"), "/interface"); fields(boundary, {"inputs", "outputs"}, "/interface");
    for (const auto side : {"inputs", "outputs"}) if (!array(boundary.member(side), "/interface", options.limits.maximumNodes, true).empty()) fail("EVENT_IR_PUBLIC_INTERFACE_UNSUPPORTED", "Public ports need a separate boundary adapter.", "/interface", true);
    for (const auto section : {"host", "execution_policy"}) if (const auto *policy = root.member(section); policy && !object(policy, section).object.empty()) fail("EVENT_IR_POLICY_UNSUPPORTED", "Source policy is not implemented by this Event profile.", section, true);
    readWidgets(root);
    const auto &graph = object(root.member("diagram"), "/diagram"); fields(graph, {"nodes", "edges", "annotations", "wire_fragments"}, "/diagram");
    if (!array(graph.member("wire_fragments"), "/diagram", options.limits.maximumEdges, true).empty()) fail("EVENT_IR_INCOMPLETE_SOURCE", "Incomplete outer wires cannot execute.", "/diagram");
    const auto &nodes = array(graph.member("nodes"), "/diagram/nodes", options.limits.maximumNodes);
    if (nodes.size() != 2) fail("EVENT_IR_OUTER_GRAPH_UNSUPPORTED", "Exactly one Event Structure and its real i64 timeout control are required.", "/diagram/nodes", true);
    const Json *structure = nullptr, *timeout = nullptr; std::string timeoutPath;
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        object(&nodes[i], "/diagram/nodes"); const auto kind = text(nodes[i].member("kind"), "/diagram/nodes");
        if (kind == "structure" && !structure) { structure = &nodes[i]; structurePath = "/diagram/nodes/" + std::to_string(i); }
        else if (kind == "widget_value" && !timeout) { timeout = &nodes[i]; timeoutPath = "/diagram/nodes/" + std::to_string(i); }
        else fail("EVENT_IR_OUTER_GRAPH_UNSUPPORTED", "Outer execution has no adapter beyond the timeout control.", "/diagram/nodes", true);
    }
    if (!structure || !timeout) fail("EVENT_IR_TIMEOUT_SOURCE", "A real timeout control and one Event Structure are required.", "/diagram");
    fields(*structure, {"id", "kind", "structure_type", "boundary", "structure_terminals", "regions", "layout", "doc", "tags"}, structurePath);
    requiredText(*structure, "structure_type", "event_structure", structurePath);
    sourceStructureId = sourceId(structure->member("id"), structurePath); structureId = encoded("structure", sourceStructureId);
    const auto &sb = object(structure->member("boundary"), structurePath); fields(sb, {"inputs", "outputs"}, structurePath);
    for (const auto side : {"inputs", "outputs"}) if (!array(sb.member(side), structurePath, options.limits.maximumNodes).empty()) fail("EVENT_IR_TUNNEL_UNSUPPORTED", "Shared tunnels need a separate boundary adapter.", structurePath, true);
    const auto &terminals = object(structure->member("structure_terminals"), structurePath); fields(terminals, {"timeout", "event_data"}, structurePath);
    const auto &td = object(terminals.member("timeout"), structurePath);
    fields(td, {"type", "outer_visible", "exposed_in_body", "read_only", "role", "outer_port", "body_port"}, structurePath);
    requiredText(td, "type", "i64", structurePath); requiredText(td, "outer_port", "timeout", structurePath);
    requiredText(td, "body_port", "timeout_value", structurePath); requiredText(td, "role", "timeout_milliseconds", structurePath);
    for (const auto flag : {"outer_visible", "exposed_in_body", "read_only"}) requiredBool(td, flag, true, structurePath);
    const auto &data = object(terminals.member("event_data"), structurePath);
    fields(data, {"outer_visible", "exposed_in_body", "read_only", "role", "fields"}, structurePath);
    requiredBool(data, "outer_visible", false, structurePath); requiredBool(data, "exposed_in_body", true, structurePath);
    requiredBool(data, "read_only", true, structurePath); requiredText(data, "role", "event_data", structurePath);
    const auto &declared = array(data.member("fields"), structurePath, 3);
    if (declared.size() != 3) fail("EVENT_IR_EVENT_FIELD", "Three public Event Data fields must be declared.", structurePath);
    for (std::size_t i = 0; i < 3; ++i) {
        const auto &field = object(&declared[i], structurePath); fields(field, {"id", "type", "body_port"}, structurePath);
        const auto name = i == 0 ? "source" : i == 1 ? "type" : "time";
        requiredText(field, "id", name, structurePath); requiredText(field, "type", i == 2 ? "u64" : "string", structurePath);
        requiredText(field, "body_port", "event_data." + std::string(name), structurePath);
    }
    auto timeoutValue = widgetNode(*timeout, timeoutPath, ""); timeoutWidget = timeoutValue.widgetId;
    if (timeoutValue.role != "control" || timeoutValue.typeRef != "i64" || timeoutValue.sourceNodeId == sourceStructureId)
        fail("EVENT_IR_TIMEOUT_SOURCE", "Timeout must use a distinct actual i64 control.", timeoutPath);
    timeoutDefault = std::get<std::int64_t>(*timeoutValue.defaultValue);
    if (timeoutDefault < -1 || timeoutDefault > 2147483647) fail("EVENT_IR_TIMEOUT_CAPABILITY", "Current SDK admits timeout -1 through INT32_MAX milliseconds.", timeoutPath, true);
    const auto &outerEdges = array(graph.member("edges"), "/diagram/edges", options.limits.maximumEdges);
    if (outerEdges.size() != 1) fail("EVENT_IR_TIMEOUT_SOURCE", "One outer edge must connect the actual timeout control.", "/diagram/edges");
    const auto &re = object(&outerEdges[0], "/diagram/edges/0"); fields(re, {"id", "from", "to", "bends", "doc", "tags"}, "/diagram/edges/0");
    const auto &from = object(re.member("from"), "/diagram/edges/0"), &to = object(re.member("to"), "/diagram/edges/0");
    fields(from, {"node", "port"}, "/diagram/edges/0"); fields(to, {"node", "port"}, "/diagram/edges/0");
    requiredText(from, "node", timeoutValue.sourceNodeId, "/diagram/edges/0"); requiredText(from, "port", "value", "/diagram/edges/0");
    requiredText(to, "node", sourceStructureId, "/diagram/edges/0"); requiredText(to, "port", "timeout", "/diagram/edges/0");
    addConnection({encoded("outer-edge", sourceId(re.member("id"), "/diagram/edges/0")), ref("/diagram/edges/0"), {timeoutValue.id, "value"}, {structureId, "timeout"}}, "/diagram/edges/0");
    const auto &rawRegions = array(structure->member("regions"), structurePath + "/regions", 256);
    if (rawRegions.empty()) fail("EVENT_IR_TIMEOUT", "At least one Timeout region is required.", structurePath);
    for (std::size_t i = 0; i < rawRegions.size(); ++i) readRegion(rawRegions[i], i);
    std::set<rt::EventIdentity> subscriptions; std::size_t timeoutCount = 0; std::vector<Json> owned, descriptors;
    for (const auto &region : regions) {
        owned.push_back(str(region.id));
        if (!region.subscription) ++timeoutCount;
        else if (!subscriptions.insert(*region.subscription).second) fail("EVENT_IR_DUPLICATE_SUBSCRIPTION", "Descriptor kind/source is duplicated.", region.sourceRef);
        auto descriptor = obj({{"region_id", str(region.id)}, {"kind", str(region.subscription ? region.subscription->kind : "timeout")},
            {"source", str(region.subscription ? region.subscription->source : "")}, {"scope", str(region.subscription ? region.subscription->scope : "")}});
        if (!region.payloadSchema.empty()) {
            std::vector<Json> fields;
            for (const auto &[name, type] : region.payloadSchema) fields.push_back(obj({{"id", str(name)}, {"type", str(type.canonicalName)}}));
            descriptor.object["payload_fields"] = list(std::move(fields));
        }
        if (region.subscription && region.subscription->filter) {
            Json filtered; filtered.type = Json::Type::Boolean; filtered.boolean = true;
            descriptor.object["filter"] = std::move(filtered);
        }
        if (!region.responseSchema.empty()) {
            std::vector<Json> fields;
            for (const auto &[name, type] : region.responseSchema) fields.push_back(obj({{"id", str(name)}, {"type", str(type.canonicalName)}}));
            descriptor.object["response_fields"] = list(std::move(fields));
        }
        descriptors.push_back(std::move(descriptor));
    }
    if (timeoutCount != 1) fail("EVENT_IR_TIMEOUT", "Exactly one Timeout region is required.", structurePath);
    if (++nodeCount > options.limits.maximumNodes) fail("EVENT_IR_LIMIT", "Object budget exceeded.", structurePath, true);
    admission.finish();
    const CanonicalFrontendPhaseScope emission(options.metrics,CanonicalFrontendPhase::FirConstruction);
    append(objects, obj({{"id", str(structureId)}, {"family", str("structure")}, {"kind", str("event_structure")},
        {"classification", obj({{"primary_or_support", str("primary")}, {"structure_family", str("event_structure")}})},
        {"ports", list({port("timeout", true, "i64", "structure_boundary", "other")})}, {"regions_owned", list(std::move(owned))},
        {"attributes", obj({{"event_cases", list(std::move(descriptors))}, {"timeout_widget_id", str(timeoutWidget)}})}, {"attribution", attribution(structurePath)}}));
    mapping(structurePath, "structure", structureId, "object"); mapping(structurePath + "/structure_terminals/timeout", "structure_terminal", structureId, "object");
    for (const auto &[_, widget] : widgets) {
        mapping(widget.path, "widget_instance", {}, "none");
        const auto &targets = widgetTargets[widget.id];
        auto &declarationMap = mappings.back(); std::vector<Json> targetRefs;
        for (const auto &target : targets) targetRefs.push_back(str(target));
        declarationMap.object["target_refs"] = list(targetRefs);
        declarationMap.object["target_kind"] = str(targets.empty() ? "none" : "object");
        if (const auto *props = widget.declaration->member("props"); props && props->member("default_value")) {
            mapping(widget.path + "/props/default_value", "widget_member", {}, "none");
            auto &defaultMap = mappings.back();
            defaultMap.object["target_refs"] = list(std::move(targetRefs));
            defaultMap.object["target_kind"] = str(targets.empty() ? "none" : "object");
            defaultMap.object["relation"] = str(targets.empty() ? "non_primary" : targets.size() > 1 ? "multi_target" : "direct_primary");
        }
        for (const auto key : {"layout", "doc", "tags"}) if (widget.declaration->member(key)) nonPrimary(widget.path + "/" + key);
    }
    for (const auto section : {"connector", "icon", "ide", "cache", "metadata"}) if (root.member(section)) nonPrimary("/" + std::string(section));
    for (const auto key : {"layout", "doc", "tags"}) if (structure->member(key)) nonPrimary(structurePath + "/" + key);
    if (graph.member("annotations")) nonPrimary("/diagram/annotations");
    return obj({{"schema_family", str("frog.execution_ir")}, {"schema_version", str("0.1")}, {"document_id", str(documentId)},
        {"metadata", obj({{"producer", str("frog.native.canonical-event-execution-ir")}, {"producer_version", str("1")},
            {"notes", list({str(std::any_of(regions.begin(), regions.end(), [](const auto &r) { return capturedFields(r.payloadSchema); }) ? "Accepted subset: frog.canonical.event-captured.v1" : "Accepted subset: frog.canonical.event-scalar.v1"), str("Exact source bytes: sha256:" + hashing::sha256Hex(source.originalUtf8())), str("Source URI: " + options.sourceUri)})}})},
        {"unit", obj({{"id", str("unit:event")}, {"objects", list(std::move(objects))}, {"connections", list(std::move(connections))},
            {"regions", list(std::move(regionRecords))}, {"source_map", list(std::move(mappings))}, {"correspondence", list(std::move(correspondence))}})}});
}
} // namespace
struct CanonicalEventExecutionIr::Storage {
    explicit Storage(const FrogSourceDocument &source) : source(source) {}
    FrogSourceDocument source;
    std::string original, documentId, sourceUri, sourceHash, structureId, timeoutWidget;
    std::int64_t timeoutDefault = -1;
    Json tree;
    std::vector<CanonicalEventRegion> regions;
    std::vector<CanonicalEventWidget> widgets;
    CanonicalIrLimits admittedLimits;
};
std::optional<CanonicalEventExecutionIr> CanonicalEventExecutionIr::derive(const FrogSourceDocument &source,
    std::string_view documentId, const CanonicalIrReadOptions &options, std::vector<CanonicalIrDiagnostic> *diagnostics) {
    if (diagnostics) diagnostics->clear();
    try {
        Draft draft{source, options}; draft.documentId = documentId;
        auto storage = std::make_shared<Storage>(source); storage->tree = draft.build();
        storage->admittedLimits = options.limits;
        {const CanonicalFrontendPhaseScope encoding(options.metrics,CanonicalFrontendPhase::FirConstruction);storage->original = json(storage->tree);}
        if (storage->original.size() > options.limits.maximumIrBytes) fail("EVENT_IR_LIMIT", "Open artifact exceeds its byte budget.", "/", true);
        storage->documentId = documentId; storage->sourceUri = options.sourceUri; storage->sourceHash = hashing::sha256Hex(source.originalUtf8());
        storage->structureId = std::move(draft.structureId); storage->timeoutWidget = std::move(draft.timeoutWidget);
        storage->timeoutDefault = draft.timeoutDefault; storage->regions = std::move(draft.regions);
        for (const auto &[_, widget] : draft.widgets) storage->widgets.push_back({widget.id, widget.role, widget.type, widget.initial});
        return CanonicalEventExecutionIr{std::move(storage)};
    } catch (const Failure &error) { if (diagnostics) diagnostics->push_back(error.diagnostic); }
    return {};
}
std::optional<CanonicalEventExecutionIr> CanonicalEventExecutionIr::read(std::string_view bytes,
    const FrogSourceDocument &source, const CanonicalIrReadOptions &options, std::vector<CanonicalIrDiagnostic> *diagnostics) {
    const CanonicalFrontendPhaseScope sealing(options.metrics,CanonicalFrontendPhase::Sealing);
    if (diagnostics) diagnostics->clear();
    try {
        if (bytes.size() > options.limits.maximumIrBytes) fail("EVENT_IR_LIMIT", "Open artifact exceeds its byte budget.", "/", true);
        std::string error; const auto tree = frog::native_engine::source::parseJson(bytes, &error, source::JsonNumberPolicy::PreserveLexeme);
        if (!tree) fail("EVENT_IR_JSON", std::move(error), "/");
        const auto id = text(object(&*tree, "/").member("document_id"), "/document_id");
        auto expected = derive(source, id, options, diagnostics); if (!expected) return {};
        if (!equal(*tree, expected->root())) fail("EVENT_IR_SOURCE_MISMATCH", "Open Event artifact differs from the complete realization of exact authoritative source.", "/");
        auto retained = std::make_shared<Storage>(*expected->storage_);
        retained->original = bytes; retained->tree = *tree;
        return CanonicalEventExecutionIr{std::move(retained)};
    } catch (const Failure &error) { if (diagnostics) diagnostics->push_back(error.diagnostic); }
    return {};
}
std::string_view CanonicalEventExecutionIr::originalUtf8() const noexcept { return storage_->original; }
const source::JsonValue &CanonicalEventExecutionIr::root() const noexcept { return storage_->tree; }
const FrogSourceDocument &CanonicalEventExecutionIr::authoritativeSource() const noexcept { return storage_->source; }
std::string_view CanonicalEventExecutionIr::documentId() const noexcept { return storage_->documentId; }
std::string_view CanonicalEventExecutionIr::unitId() const noexcept { return "unit:event"; }
std::string_view CanonicalEventExecutionIr::sourceUri() const noexcept { return storage_->sourceUri; }
std::string_view CanonicalEventExecutionIr::sourceHash() const noexcept { return storage_->sourceHash; }
bool CanonicalEventExecutionIr::requiresCapturedValues() const noexcept {
    return storage_ && std::any_of(storage_->regions.begin(), storage_->regions.end(), [](const auto &r) { return capturedFields(r.payloadSchema); });
}
bool CanonicalEventExecutionIr::admittedWithin(const CanonicalIrLimits &requested) const noexcept {
    if (!storage_) return false;
    const auto &admitted = storage_->admittedLimits;
    return requested.maximumIrBytes >= admitted.maximumIrBytes &&
        requested.maximumSourceBytes >= admitted.maximumSourceBytes &&
        requested.maximumNodes >= admitted.maximumNodes &&
        requested.maximumEdges >= admitted.maximumEdges &&
        requested.maximumWidgets >= admitted.maximumWidgets &&
        requested.maximumSourceIdBytes >= admitted.maximumSourceIdBytes &&
        requested.maximumIrIdBytes >= admitted.maximumIrIdBytes &&
        requested.maximumMappingRecords >= admitted.maximumMappingRecords;
}
std::string_view CanonicalEventExecutionIr::structureId() const noexcept { return storage_->structureId; }
std::string_view CanonicalEventExecutionIr::timeoutWidgetId() const noexcept { return storage_->timeoutWidget; }
std::int64_t CanonicalEventExecutionIr::timeoutDefault() const noexcept { return storage_->timeoutDefault; }
std::span<const CanonicalEventRegion> CanonicalEventExecutionIr::regions() const noexcept { return storage_->regions; }
std::span<const CanonicalEventWidget> CanonicalEventExecutionIr::widgets() const noexcept { return storage_->widgets; }
} // namespace frog::native_engine
