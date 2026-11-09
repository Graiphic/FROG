#include "frog_native_engine/BindingType.hpp"
#include "WidgetCatalog.generated.hpp"

#include <algorithm>
#include <array>
#include <charconv>
#include <cctype>
#include <utility>

namespace frog::native_engine::binding {
namespace {
constexpr std::string_view primitiveNames[]{
    "bool", "i4", "i8", "i16", "i32", "i64", "i128", "u4", "u8", "u16", "u32", "u64", "u128", "biguint",
    "f8e4m3", "f8e5m2", "f16", "bf16", "f32", "f64", "f80", "f128",
    "c32", "c64", "c80", "c128", "c256", "fxp", "cfx", "decimal", "string", "path", "timestamp", "variant", "refnum"
};
std::string quoted(std::string_view text) {
    std::string result = "\"";
    for (const char c : text) {
        if (c == '\\' || c == '"') result += '\\';
        result += c;
    }
    return result + '"';
}
class Parser {
public:
    explicit Parser(std::string_view text) : text_(text) {}
    TypeParseResult run() {
        TypeParseResult result;
        if (text_.size() > 65536U) { result.error = "Type expression exceeds 64 KiB."; return result; }
        result.type = type(0U);
        whitespace();
        if (offset_ != text_.size() && error_.empty()) fail("Unexpected trailing type data.");
        result.error = error_;
        result.legacy = legacy_;
        return result;
    }
private:
    std::string_view text_;
    std::size_t offset_ = 0U;
    std::string error_;
    bool legacy_ = false;
    void fail(std::string_view message) { if (error_.empty()) error_ = message; }
    void whitespace() {
        while (offset_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[offset_]))) ++offset_;
    }
    bool take(std::string_view token) {
        whitespace();
        if (!text_.substr(offset_).starts_with(token)) return false;
        offset_ += token.size();
        return true;
    }
    bool expect(std::string_view token) {
        if (take(token)) return true;
        fail("Missing delimiter in type expression.");
        return false;
    }
    std::string identifier() {
        whitespace();
        const auto start = offset_;
        while (offset_ < text_.size()) {
            const auto c = static_cast<unsigned char>(text_[offset_]);
            if (!std::isalnum(c) && c != '_' && c != '.' && c != '-' && c != '/') break;
            ++offset_;
        }
        return std::string(text_.substr(start, offset_ - start));
    }
    std::string identity() {
        if (!take("\"")) return identifier();
        std::string result;
        while (offset_ < text_.size()) {
            char c = text_[offset_++];
            if (c == '"') return result;
            if (c == '\\') {
                if (offset_ == text_.size()) break;
                c = text_[offset_++];
                if (c != '"' && c != '\\') { fail("Unsupported identity escape."); return {}; }
            }
            if (static_cast<unsigned char>(c) < 32U) { fail("Control character in identity."); return {}; }
            result += c;
        }
        fail("Unterminated identity.");
        return {};
    }
    std::size_t number() {
        whitespace();
        std::size_t value = 0U;
        const char *first = text_.data() + offset_;
        const auto parsed = std::from_chars(first, text_.data() + text_.size(), value);
        if (parsed.ec != std::errc{} || parsed.ptr == first) { fail("Invalid positive integer."); return 0U; }
        offset_ += static_cast<std::size_t>(parsed.ptr - first);
        if (value == 0U) fail("Rank and fixed extent must be positive.");
        return value;
    }
    TypeRef type(std::size_t depth) {
        whitespace();
        if (offset_ == text_.size()) { fail("Missing type."); return {}; }
        if (take("array:")) {
            legacy_ = true;
            return TypeRef::array(type(depth + 1U));
        }
        if (take("ring:")) {
            legacy_ = true;
            auto result = type(depth + 1U);
            if (!result.integer()) fail("A Ring requires an integer representation.");
            return result;
        }
        if (take("enum:")) {
            legacy_ = true;
            auto representation = type(depth + 1U);
            if (!representation.integer()) fail("An Enum requires an integer representation.");
            // Missing identity remains unresolved; never merge unrelated legacy enums.
            return TypeRef::enumeration({}, std::move(representation));
        }
        if (take("cluster:")) {
            legacy_ = true;
            std::vector<TypeRef> fields;
            std::vector<std::string> ids;
            do { fields.push_back(type(depth + 1U)); ids.push_back("field_" + std::to_string(ids.size())); }
            while (take("|"));
            // Positional legacy spellings contain no field IDs. The importing
            // widget schema must supply them; inventing IDs would create false compatibility.
            return {};
        }
        if (take("frog.array<")) { legacy_ = true; return arrayPayload(depth); }
        if (take("array<")) return arrayPayload(depth);
        if (take("map<")) {
            auto key=type(depth+1U);expect(",");auto value=type(depth+1U);expect(">");
            return TypeRef::map(std::move(key),std::move(value));
        }
        if (take("set<")) {auto element=type(depth+1U);expect(">");return TypeRef::set(std::move(element));}
        if (take("matrix<")) {
            // A Matrix scalar domain is a leaf, never recursive Array lifting.
            const auto scalar=identifier();expect(">");
            if(scalar!="f64")fail("The private Matrix value profile supports f64 elements only.");
            return TypeRef::matrix(TypeRef::primitive(scalar));
        }
        if (take("ref<")) {
            auto classId = identity();
            std::optional<TypeRef> schema;
            if (take(",")) schema = type(depth + 1U);
            expect(">");
            auto result = TypeRef::reference(std::move(classId), std::move(schema));
            if (!result.known()) fail("Reference requires a class and an optional complete value schema.");
            return result;
        }
        if (take("enum<")) {
            auto domain = identity();
            if (domain.empty()) fail("Enum domain ID is required.");
            expect(",");
            auto representation = type(depth + 1U);
            expect(">");
            if (!representation.integer()) fail("An Enum requires an integer representation.");
            return TypeRef::enumeration(std::move(domain), std::move(representation));
        }
        if (take("cluster<")) {
            std::vector<std::string> ids;
            std::vector<TypeRef> fields;
            if (!take(">")) {
                do {
                    auto id = identity();
                    if (id.empty() || std::find(ids.begin(), ids.end(), id) != ids.end()) {
                        fail("Cluster field IDs must be nonempty and unique.");
                    }
                    ids.push_back(std::move(id));
                    expect(":");
                    fields.push_back(type(depth + 1U));
                } while (error_.empty() && take(","));
                expect(">");
            }
            return TypeRef::cluster(std::move(ids), std::move(fields));
        }
        const auto name = identifier();
        if (name == "frog.waveform_graph_value") {
            TypeRef graph; graph.kind = TypeKind::WaveformGraph; return graph;
        }
        if (name == "unknown") return {};
        if (std::find(std::begin(primitiveNames), std::end(primitiveNames), name) == std::end(primitiveNames)) {
            fail("Unknown primitive type.");
            return {};
        }
        return TypeRef::primitive(name);
    }
    TypeRef arrayPayload(std::size_t depth) {
        auto result = TypeRef::array(type(depth + 1U));
        if (take(",")) {
            if (take("rank=")) {
                result.rank = number();
                if (result.rank > 16U) fail("Array rank exceeds 16.");
            } else {
                result.extent = number();
            }
        }
        expect(">");
        return result;
    }
};
} // namespace

std::span<const std::string_view> nativeObjectReferenceClasses() noexcept {
    static constexpr std::string_view classes[]{"frog.object", "frog.control",
        "frog.application", "frog.program", "frog.front_panel", "frog.menu",
        "frog.cursor", "frog.graph.cursor", "frog.graph.scale", "frog.graph.plot",
        "frog.text.label", "frog.text.caption", "frog.text.boolean", "frog.pane",
        "frog.control.color_box", "frog.control.timestamp", "frog.control.dropdown_string",
        "frog.control.error_cluster", "frog.project", "frog.project_item", "frog.target"};
    return classes;
}
bool referenceClassAssignable(std::string_view source, std::string_view target) noexcept {
    if (source.empty() || target.empty()) return false;
    if (source == target) return true;
    // NI explicitly defines Web Service Request as a Web Service subclass.
    // External reference families otherwise remain nominal and disjoint.
    if (source=="connectivity.web_service_request" && target=="connectivity.web_service")return true;
    // Use the same generated declarative classes as WidgetCatalog, without a
    // link from the foundation type layer to the widget-family implementation.
    const auto canonical=[](std::string_view id) {
        for(const auto &[alias,actual]:generated::widgets::aliases)if(id==alias)return actual;
        return id;
    };
    source=canonical(source);target=canonical(target);
    const auto specializedBase=[](std::string_view id)->std::string_view {
        if(id=="frog.control.color_box" || id=="frog.control.timestamp")return "frog.widgets.numeric_control";
        if(id=="frog.control.dropdown_string")return "frog.widgets.string_control";
        if(id=="frog.control.error_cluster")return "frog.widgets.cluster_control";
        return id;
    };
    const auto sourceBase=specializedBase(source);
    if(sourceBase!=source) {
        // Preset interfaces generalize to their historical concrete widget
        // class/role; no reverse narrowing or u32-to-ColorBox guess is allowed.
        const auto sourceDefinition=std::find_if(generated::widgets::definitions.begin(),generated::widgets::definitions.end(),
            [sourceBase](const auto &d){return d.classId==sourceBase;});
        const auto targetDefinition=std::find_if(generated::widgets::definitions.begin(),generated::widgets::definitions.end(),
            [target](const auto &d){return d.classId==target;});
        const bool historicalRole=target==sourceBase ||
            (sourceBase=="frog.widgets.numeric_control" && target=="frog.widgets.numeric_indicator") ||
            (sourceBase=="frog.widgets.string_control" && target=="frog.widgets.string_indicator") ||
            (sourceBase=="frog.widgets.cluster_control" && target=="frog.widgets.cluster_indicator");
        if(historicalRole && sourceDefinition!=generated::widgets::definitions.end() && targetDefinition!=generated::widgets::definitions.end())return true;
        source=sourceBase;
    }
    const auto found=std::find_if(generated::widgets::definitions.begin(),generated::widgets::definitions.end(),
        [source](const auto &definition){return definition.classId==source;});
    // Generic reference controls participate in the native object hierarchy.
    const bool generatedWidget=found!=generated::widgets::definitions.end();
    const bool hiddenReferenceWidget=source==kReferenceWidgetClass;
    const bool sourceWidget=generatedWidget || hiddenReferenceWidget;
    if (sourceWidget && source == target) return true;
    if (target == "frog.control") return hiddenReferenceWidget ||
        (generatedWidget && found->terminal.direction!=WidgetTerminalDirection::None);
    if (target == "frog.object") {
        const auto classes=nativeObjectReferenceClasses();
        return sourceWidget || std::find(classes.begin(),classes.end(),source)!=classes.end();
    }
    return false;
}
TypeRef TypeRef::reference(std::string classId, std::optional<TypeRef> valueType) {
    TypeRef result; result.kind = TypeKind::Reference; result.name = std::move(classId);
    if (valueType) result.children.push_back(std::move(*valueType));
    return result;
}
TypeRef TypeRef::primitive(std::string_view name) {
    TypeRef result; result.kind = TypeKind::Primitive; result.name = name; return result;
}
TypeRef TypeRef::array(TypeRef element, std::size_t rank) {
    TypeRef result; result.kind = TypeKind::Array; result.rank = rank;
    result.children.push_back(std::move(element)); return result;
}
TypeRef TypeRef::matrix(TypeRef element) {
    TypeRef result;result.kind=TypeKind::Matrix;result.rank=2;
    result.children.push_back(std::move(element));return result;
}
TypeRef TypeRef::map(TypeRef key, TypeRef value) {
    TypeRef result;result.kind=TypeKind::Map;
    result.children.push_back(std::move(key));result.children.push_back(std::move(value));return result;
}
TypeRef TypeRef::set(TypeRef element) {
    TypeRef result;result.kind=TypeKind::Set;result.children.push_back(std::move(element));return result;
}
bool fullTypeEqual(const TypeRef &a,const TypeRef &b) {
    if(a.kind!=b.kind||a.name!=b.name||a.rank!=b.rank||a.extent!=b.extent||a.fieldIds!=b.fieldIds||a.fieldNames!=b.fieldNames||a.choiceKind!=b.choiceKind||a.choices!=b.choices||a.children.size()!=b.children.size())return false;
    for(std::size_t i=0;i<a.children.size();++i)if(!fullTypeEqual(a.children[i],b.children[i]))return false;return true;
}
TypeRef TypeRef::enumeration(std::string domain, TypeRef representation) {
    TypeRef result; result.kind = TypeKind::Enum; result.name = std::move(domain);
    result.children.push_back(std::move(representation)); return result;
}
TypeRef TypeRef::cluster(std::vector<std::string> ids, std::vector<TypeRef> fields) {
    TypeRef result; result.kind = TypeKind::Cluster; result.fieldIds = std::move(ids);
    result.children = std::move(fields); return result;
}
bool TypeRef::known() const {
    if (kind == TypeKind::Unknown) return false;
    if (kind == TypeKind::Reference) {
        if (name.empty() || rank != 0U || extent || !fieldIds.empty() || children.size() > 1U ||
            (!std::all_of(name.begin(), name.end(), [](unsigned char c) {
                return std::isalnum(c) || c == '_' || c == '.' || c == '-';
            }))) return false;
        return children.empty() || children.front().known();
    }
    if (kind == TypeKind::Matrix) {
        if(!name.empty() || rank!=2 || extent || children.size()!=1 || !fieldIds.empty() || !fieldNames.empty() ||
            choiceKind!=ChoiceKind::None || !choices.empty())return false;
        const auto &element=children.front();
        return element.kind==TypeKind::Primitive && element.name=="f64" && element.rank==0 && !element.extent &&
            element.children.empty() && element.fieldIds.empty() && element.fieldNames.empty() &&
            element.choiceKind==ChoiceKind::None && element.choices.empty();
    }
    if (kind == TypeKind::Map) {
        return name.empty() && rank==0 && !extent && children.size()==2 &&
            fieldIds.empty() && fieldNames.empty() && choiceKind==ChoiceKind::None && choices.empty() &&
            children[0].known() && children[1].known();
    }
    if(kind==TypeKind::Set)return name.empty()&&rank==0&&!extent&&children.size()==1&&fieldIds.empty()&&fieldNames.empty()&&choiceKind==ChoiceKind::None&&choices.empty()&&children.front().known();
    if (kind == TypeKind::WaveformGraph) return children.empty() && fieldIds.empty() && name.empty();
    if (kind == TypeKind::Primitive) return children.empty() && fieldIds.empty() &&
        std::find(std::begin(primitiveNames), std::end(primitiveNames), name) != std::end(primitiveNames);
    if (kind == TypeKind::Array && (children.size() != 1U || rank < 1U || rank > 16U ||
        (extent && (*extent == 0U || rank != 1U)))) return false;
    if (kind == TypeKind::Enum && (name.empty() || children.size() != 1U || !children.front().integer())) return false;
    if (kind == TypeKind::Cluster) {
        if (fieldIds.size() != children.size()) return false;
        for (std::size_t i = 0U; i < fieldIds.size(); ++i) {
            if (fieldIds[i].empty() || std::find(fieldIds.begin(), fieldIds.begin() + i, fieldIds[i]) != fieldIds.begin() + i) return false;
        }
    }
    return std::all_of(children.begin(), children.end(), [](const auto &child) { return child.known(); });
}
bool TypeRef::integer() const {
    return kind == TypeKind::Primitive &&
        (name == "i4" || name == "i8" || name == "i16" || name == "i32" || name == "i64" || name == "i128" ||
         name == "u4" || name == "u8" || name == "u16" || name == "u32" || name == "u64" || name == "u128" || name == "biguint");
}
bool TypeRef::numeric() const {
    return kind == TypeKind::Primitive && known() && name != "bool" && name != "string" &&
        name != "path" && name != "timestamp" && name != "variant" && name != "refnum";
}
std::string TypeRef::canonical() const {
    switch (kind) {
    case TypeKind::Unknown: return "unknown";
    case TypeKind::Primitive: return name;
    case TypeKind::WaveformGraph: return "frog.waveform_graph_value";
    case TypeKind::Matrix: return known()?"matrix<"+children.front().canonical()+">":"unknown";
    case TypeKind::Map: return known()?"map<"+children[0].canonical()+","+children[1].canonical()+">":"unknown";
    case TypeKind::Set: return known()?"set<"+children.front().canonical()+">":"unknown";
    case TypeKind::Reference:
        if (!known()) return "unknown";
        return "ref<" + quoted(name) + (children.empty() ? "" : "," + children.front().canonical()) + ">";
    case TypeKind::Array:
        if (children.size() != 1U) return "unknown";
        return "array<" + children.front().canonical() +
            (extent.has_value() ? "," + std::to_string(*extent) :
             rank != 1U ? ",rank=" + std::to_string(rank) : "") + ">";
    case TypeKind::Enum:
        if (name.empty() || children.size() != 1U) return "unknown";
        return "enum<" + quoted(name) + "," + children.front().canonical() + ">";
    case TypeKind::Cluster: {
        std::string result = "cluster<";
        if (fieldIds.size() != children.size()) return "unknown";
        for (std::size_t i = 0U; i < children.size(); ++i) {
            if (i != 0U) result += ',';
            result += quoted(fieldIds[i]) + ":" + children[i].canonical();
        }
        return result + ">";
    }}
    return "unknown";
}
TypeParseResult parseType(std::string_view text) { return Parser(text).run(); }
bool isErrorCluster(const TypeRef &type) {
    if (type.kind != TypeKind::Cluster || type.children.size() != 3U || !type.known()) return false;
    constexpr std::string_view fields[]{"bool", "i32", "string"};
    for (std::size_t i = 0; i < 3U; ++i)
        if (type.children[i].kind != TypeKind::Primitive || type.children[i].name != fields[i]) return false;
    return true;
}
TypeRef withArrayRank(TypeRef type, std::size_t rank) {
    if (type.kind == TypeKind::Array && rank >= 1U && rank <= 16U) type.rank = rank;
    return type;
}
} // namespace frog::native_engine::binding
