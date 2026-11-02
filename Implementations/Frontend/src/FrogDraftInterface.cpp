#include "frog_native_engine/FrogDraftInterface.hpp"
#include "frog_native_engine/FrogJson.hpp"
#include "frog_native_engine/BindingType.hpp"
#include "frog_native_engine/NumericValueLexeme.hpp"

#include <charconv>
#include <cmath>
#include <cstdint>
#include <utility>
#include <algorithm>
#include <unordered_set>

namespace frog::native_engine {
namespace {
using source::JsonValue;

std::string quote(std::string_view text) {
    constexpr char hex[] = "0123456789abcdef";
    std::string result = "\"";
    for (const unsigned char byte : text) {
        if (byte == '"' || byte == '\\') { result += '\\'; result += static_cast<char>(byte); }
        else if (byte < 0x20U) {
            result += "\\u00"; result += hex[byte >> 4U]; result += hex[byte & 15U];
        } else result += static_cast<char>(byte);
    }
    return result + '"';
}

bool stringIs(const JsonValue *value, std::string_view text) {
    return value && value->type == JsonValue::Type::String && value->string == text;
}

std::optional<JsonValue> tree(std::string_view bytes) {
    return source::parseJson(bytes, nullptr, source::JsonNumberPolicy::PreserveLexeme);
}

std::string replace(std::string_view bytes, std::size_t begin, std::size_t end,
    std::string_view value) {
    std::string result(bytes.substr(0U, begin));
    result += value; result += bytes.substr(end);
    return result;
}

// Remove only a parsed token/member and its separating comma. All other raw
// bytes (including unknown metadata and exact number lexemes) are preserved.
std::string removeToken(std::string_view bytes, std::size_t begin, std::size_t end) {
    auto left=begin;while(left && (bytes[left-1]==' ' || bytes[left-1]=='\r' || bytes[left-1]=='\n' || bytes[left-1]=='\t'))--left;
    if(left && bytes[left-1]==',') return replace(bytes,left-1,end,{});
    auto right=end;while(right<bytes.size() && (bytes[right]==' ' || bytes[right]=='\r' || bytes[right]=='\n' || bytes[right]=='\t'))++right;
    return replace(bytes,begin,right<bytes.size() && bytes[right]==','?right+1:end,{});
}

enum class DefaultCompatibility { Compatible, Incompatible, Unverified };

template<class Integer>
bool integerDefaultFits(const JsonValue &value, std::string_view transportType) {
    // The JSON scanner and exact decimal/exponent normalizer are the same
    // owners as document Numeric transport. Never validate U64 through double.
    const auto lexical = numeric_value::canonicalWideIntegerLexeme(value.string, transportType);
    if (!lexical) return false;
    Integer number{};
    const auto parsed = std::from_chars(lexical->data(), lexical->data() + lexical->size(), number);
    return parsed.ec == std::errc{} && parsed.ptr == lexical->data() + lexical->size();
}

DefaultCompatibility retainedDefaultCompatibility(const JsonValue &value,
    const binding::TypeRef &type) {
    // This is a bounded private authoring refusal, not a public validator or
    // an implicit conversion. Composite/extended literal encodings currently
    // have no shared source compatibility owner; do not guess or rewrite them.
    if (!type.known() || type.kind != binding::TypeKind::Primitive)
        return DefaultCompatibility::Unverified;
    const auto result = [](bool accepted) {
        return accepted ? DefaultCompatibility::Compatible : DefaultCompatibility::Incompatible;
    };
    if (type.name == "bool") return result(value.type == JsonValue::Type::Boolean);
    if (type.name == "string" || type.name == "path")
        return result(value.type == JsonValue::Type::String);
    const bool integer = type.name == "i8" || type.name == "i16" || type.name == "i32" ||
        type.name == "i64" || type.name == "u8" || type.name == "u16" || type.name == "u32" ||
        type.name == "u64" || type.name == "i128" || type.name == "u128";
    if (integer) {
        if (value.type != JsonValue::Type::Number) return DefaultCompatibility::Incompatible;
        if (type.name == "i8") return result(integerDefaultFits<std::int8_t>(value, "i64"));
        if (type.name == "i16") return result(integerDefaultFits<std::int16_t>(value, "i64"));
        if (type.name == "i32") return result(integerDefaultFits<std::int32_t>(value, "i64"));
        if (type.name == "i64") return result(integerDefaultFits<std::int64_t>(value, "i64"));
        if (type.name == "u8") return result(integerDefaultFits<std::uint8_t>(value, "u64"));
        if (type.name == "u16") return result(integerDefaultFits<std::uint16_t>(value, "u64"));
        if (type.name == "u32") return result(integerDefaultFits<std::uint32_t>(value, "u64"));
        if (type.name == "u64") return result(integerDefaultFits<std::uint64_t>(value, "u64"));
        return result(numeric_value::canonicalWideIntegerLexeme(value.string, type.name).has_value());
    }
    if (type.name == "f32" || type.name == "f64") {
        if (value.type != JsonValue::Type::Number) return DefaultCompatibility::Incompatible;
        if (type.name == "f32") {
            float number{};
            const auto parsed = std::from_chars(value.string.data(), value.string.data() + value.string.size(), number);
            if (parsed.ec == std::errc::result_out_of_range) return DefaultCompatibility::Unverified;
            return result(parsed.ec == std::errc{} && parsed.ptr == value.string.data() + value.string.size() && std::isfinite(number));
        }
        double number{};
        const auto parsed = std::from_chars(value.string.data(), value.string.data() + value.string.size(), number);
        if (parsed.ec == std::errc::result_out_of_range) return DefaultCompatibility::Unverified;
        return result(parsed.ec == std::errc{} && parsed.ptr == value.string.data() + value.string.size() && std::isfinite(number));
    }
    return DefaultCompatibility::Unverified;
}
} // namespace

FrogDraftInterface::FrogDraftInterface()
    : FrogDraftInterface(R"({"inputs":[],"outputs":[]})") {}

FrogDraftInterface::FrogDraftInterface(std::string json)
    : bytes_(std::make_shared<const std::string>(std::move(json))) {}

std::optional<FrogDraftInterface> FrogDraftInterface::read(std::string_view json, std::string *error) {
    auto value = source::parseJson(json, error, source::JsonNumberPolicy::PreserveLexeme);
    if (!value) return std::nullopt;
    // Invalid authoring declarations remain inspectable; do not repair them.
    return FrogDraftInterface(std::string(json));
}

std::string_view FrogDraftInterface::declarationUtf8() const noexcept { return *bytes_; }
FrogDraftInterface FrogDraftInterface::retainParsedDraftDeclaration(std::string json,
    std::optional<std::string> legacyRootMapUtf8) {
    FrogDraftInterface result(std::move(json));
    if (legacyRootMapUtf8)
        result.legacyRootMapBytes_ = std::make_shared<const std::string>(std::move(*legacyRootMapUtf8));
    return result;
}
std::optional<std::string_view> FrogDraftInterface::retainedLegacyRootInterfaceMapUtf8() const noexcept {
    if (!legacyRootMapBytes_) return std::nullopt;
    return *legacyRootMapBytes_;
}
std::size_t FrogDraftInterface::retainedBytes() const noexcept {
    return bytes_->capacity() + 1U + (legacyRootMapBytes_ ? legacyRootMapBytes_->capacity() + 1U : 0U);
}

bool FrogDraftInterface::containsPort(std::string_view id) const {
    const auto root = tree(*bytes_);
    if (!root || root->type != JsonValue::Type::Object) return false;
    for (const auto key : {"inputs", "outputs"}) {
        const auto *ports = root->member(key);
        if (!ports || ports->type != JsonValue::Type::Array) continue;
        for (const auto &port : ports->array)
            if (stringIs(port.member("id"), id)) return true;
    }
    return false;
}

bool FrogDraftInterface::declarePort(bool input, std::string_view id,
    std::string_view type, std::string_view connection) {
    if (id.empty() || type.empty()) return false;
    const auto root = tree(*bytes_);
    if (!root || root->type != JsonValue::Type::Object) return false;
    const std::string_view destination = input ? "inputs" : "outputs";
    bool found = false;
    for (const auto key : {"inputs", "outputs"}) {
        const auto *ports = root->member(key);
        if (!ports) continue;
        if (ports->type != JsonValue::Type::Array) return false;
        for (const auto &port : ports->array) {
            if (!stringIs(port.member("id"), id)) continue;
            if (found || key != destination || !stringIs(port.member("type"), type)) return false;
            found = true;
        }
    }
    if (found) return true;
    const auto port = "{\"id\":" + quote(id) + ",\"type\":" + quote(type) +
        (input && !connection.empty() ? ",\"connection\":" + quote(connection) : "") + "}";
    std::string candidate;
    if (const auto *ports = root->member(destination)) {
        const auto insertion = (ports->array.empty() ? "" : ",") + port;
        candidate = replace(*bytes_, ports->sourceEnd - 1U, ports->sourceEnd - 1U, insertion);
    } else {
        const auto insertion = (root->object.empty() ? "" : ",") + quote(destination) + ":[" + port + "]";
        candidate = replace(*bytes_, root->sourceEnd - 1U, root->sourceEnd - 1U, insertion);
    }
    if (!tree(candidate)) return false;
    bytes_ = std::make_shared<const std::string>(std::move(candidate));
    return true;
}

std::string FrogDraftInterface::withMap(std::string_view mapJson) const {
    const auto root = tree(*bytes_);
    const auto map = tree(mapJson);
    if (!root || root->type != JsonValue::Type::Object || !map || map->type != JsonValue::Type::Object) return *bytes_;
    if (const auto *existing = root->member("map")) {
        if (existing->type != JsonValue::Type::Object) return *bytes_;
        // Merge explicitly edited fields, preserving all unrelated metadata.
        std::string result = *bytes_;
        for (const auto &[key, value] : map->object) {
            const auto current = tree(result);
            const auto *currentMap = current ? current->member("map") : nullptr;
            if (!currentMap) return *bytes_;
            const auto valueBytes = mapJson.substr(value.sourceBegin, value.sourceEnd - value.sourceBegin);
            if (const auto *old = currentMap->member(key))
                result = replace(result, old->sourceBegin, old->sourceEnd, valueBytes);
            else {
                const auto addition = (currentMap->object.empty() ? std::string{} : ",") + quote(key) + ":" + std::string(valueBytes);
                result = replace(result, currentMap->sourceEnd - 1U, currentMap->sourceEnd - 1U, addition);
            }
        }
        return result;
    }
    const auto insertion = (root->object.empty() ? std::string{} : ",") + std::string("\"map\":") + std::string(mapJson);
    return replace(*bytes_, root->sourceEnd - 1U, root->sourceEnd - 1U, insertion);
}

bool FrogDraftInterface::setMapSettings(std::string_view profile, std::string_view pattern,
    const std::vector<std::string> &transforms) {
    std::string map = "{\"binding_map_profile\":" + quote(profile) + ",\"layout_pattern_id\":" + quote(pattern) + ",\"visual_transforms\":[";
    for (std::size_t i = 0U; i < transforms.size(); ++i) {
        if (i) map += ',';
        map += quote(transforms[i]);
    }
    map += "]}";
    const auto root = tree(*bytes_);
    if (!root || root->type != JsonValue::Type::Object || !tree(map)) return false;
    if (const auto *existing = root->member("map"); existing && existing->type != JsonValue::Type::Object)
        return false; // Editing presentation settings is not a repair/migration command.
    auto result = withMap(map);
    if (!tree(result)) return false;
    bytes_ = std::make_shared<const std::string>(std::move(result));
    return true;
}


bool FrogDraftInterface::editPortForAuthoring(std::string_view id,
    bool expectedInput, std::string_view expectedTypeExpression,
    bool nextInput, std::string_view nextTypeExpression,
    std::optional<std::string_view> nextInputConnection, std::string *diagnostic) {
    if(diagnostic)diagnostic->clear();
    if(id.empty() || expectedTypeExpression.empty() || nextTypeExpression.empty() ||
        (nextInput && nextInputConnection && (*nextInputConnection!="required" &&
            *nextInputConnection!="recommended" && *nextInputConnection!="optional")) ||
        (nextInput && !expectedInput && !nextInputConnection) ||
        (!nextInput && nextInputConnection)) return false;
    const auto root=tree(*bytes_);if(!root || root->type!=JsonValue::Type::Object)return false;
    const JsonValue *selected=nullptr;bool selectedInput=false;std::unordered_set<std::string> ids;
    for(const bool input:{true,false}) {
        const auto *ports=root->member(input?"inputs":"outputs");if(!ports)continue;
        if(ports->type!=JsonValue::Type::Array)return false;
        for(const auto &port:ports->array) {
            const auto *portId=port.member("id"),*type=port.member("type");
            if(port.type!=JsonValue::Type::Object || !portId || portId->type!=JsonValue::Type::String || portId->string.empty() ||
                !type || type->type!=JsonValue::Type::String || type->string.empty() || !ids.insert(portId->string).second)return false;
            if(const auto *policy=port.member("connection");policy && (!input || policy->type!=JsonValue::Type::String ||
                (policy->string!="required" && policy->string!="recommended" && policy->string!="optional")))return false;
            if(portId->string==id){selected=&port;selectedInput=input;}
        }
    }
    if(!selected || selectedInput!=expectedInput || !stringIs(selected->member("type"),expectedTypeExpression))return false;
    // A retained input default is author data. A role command must neither
    // publish it as output metadata nor silently discard/migrate its bytes.
    if(expectedInput && !nextInput && selected->member("default")) {
        if(diagnostic)*diagnostic="Input port '"+std::string(id)+"' retains a default value; changing it to an output is refused without removing that author data.";
        return false;
    }
    std::string portBytes(bytes_->substr(selected->sourceBegin,selected->sourceEnd-selected->sourceBegin));
    auto port=tree(portBytes);const auto *type=port->member("type");
    if(type->string!=nextTypeExpression)portBytes=replace(portBytes,type->sourceBegin,type->sourceEnd,quote(nextTypeExpression));
    port=tree(portBytes);const auto *policy=port->member("connection");
    if(!nextInput && policy)portBytes=removeToken(portBytes,policy->memberBegin,policy->sourceEnd);
    else if(nextInput && nextInputConnection && policy && !stringIs(policy,*nextInputConnection))
        portBytes=replace(portBytes,policy->sourceBegin,policy->sourceEnd,quote(*nextInputConnection));
    else if(nextInput && nextInputConnection && !policy)portBytes=replace(portBytes,port->sourceEnd-1,port->sourceEnd-1,
        (port->object.empty()?std::string{}:",")+std::string("\"connection\":")+quote(*nextInputConnection));
    std::string candidate;
    if(nextInput==expectedInput)candidate=replace(*bytes_,selected->sourceBegin,selected->sourceEnd,portBytes);
    else {
        candidate=removeToken(*bytes_,selected->sourceBegin,selected->sourceEnd);
        const auto reduced=tree(candidate);if(!reduced)return false;
        const auto destination=nextInput?"inputs":"outputs";
        if(const auto *ports=reduced->member(destination))
            candidate=replace(candidate,ports->sourceEnd-1,ports->sourceEnd-1,(ports->array.empty()?"":",")+portBytes);
        else candidate=replace(candidate,reduced->sourceEnd-1,reduced->sourceEnd-1,
            (reduced->object.empty()?std::string{}:",")+quote(destination)+":["+portBytes+"]");
    }
    const auto prepared=tree(candidate);if(!prepared)return false;
    if(nextInput && selected->member("default") && expectedTypeExpression!=nextTypeExpression) {
        const auto previousType=binding::parseType(expectedTypeExpression);
        const auto nextType=binding::parseType(nextTypeExpression);
        const bool sameCanonical=previousType.valid() && nextType.valid() &&
            previousType.type.known() && nextType.type.known() && previousType.type==nextType.type;
        if(!sameCanonical) {
            const auto *inputs=prepared->member("inputs");const JsonValue *preparedPort=nullptr;
            if(inputs && inputs->type==JsonValue::Type::Array)
                for(const auto &entry:inputs->array)if(stringIs(entry.member("id"),id))preparedPort=&entry;
            const auto *retainedDefault=preparedPort?preparedPort->member("default"):nullptr;
            const auto compatibility=retainedDefault && nextType.valid()
                ? retainedDefaultCompatibility(*retainedDefault,nextType.type) : DefaultCompatibility::Unverified;
            if(compatibility!=DefaultCompatibility::Compatible) {
                if(diagnostic)*diagnostic="Input port '"+std::string(id)+"' retains a default value which for '"+
                    std::string(nextTypeExpression)+(compatibility==DefaultCompatibility::Incompatible
                        ? "' is incompatible; the type change is refused without rewriting that author data."
                        : "' cannot be verified by this private authoring capability; the type change is refused without rewriting that author data.");
                return false;
            }
        }
    }
    if(candidate==*bytes_)return true;
    bytes_=std::make_shared<const std::string>(std::move(candidate));return true;
}

bool FrogDraftInterface::setInputConnection(std::string_view id, std::string_view connection) {
    if (connection != "required" && connection != "recommended" && connection != "optional") return false;
    const auto root = tree(*bytes_);
    if (!root) return false;
    const auto *inputs = root->member("inputs");
    if (!inputs || inputs->type != JsonValue::Type::Array) return false;
    if (const auto *outputs = root->member("outputs"); outputs && outputs->type == JsonValue::Type::Array)
        for (const auto &port : outputs->array) if (stringIs(port.member("id"), id)) return false;
    const JsonValue *selected = nullptr;
    for (const auto &port : inputs->array) {
        if (!stringIs(port.member("id"), id)) continue;
        if (selected) return false;
        selected = &port;
    }
    if (!selected) return false;
    if (stringIs(selected->member("connection"), connection)) return true;
    std::string candidate;
    if (const auto *old = selected->member("connection"))
        candidate = replace(*bytes_, old->sourceBegin, old->sourceEnd, quote(connection));
    else candidate = replace(*bytes_, selected->sourceEnd - 1U, selected->sourceEnd - 1U,
        (selected->object.empty() ? std::string{} : ",") + std::string("\"connection\":") + quote(connection));
    if (!tree(candidate)) return false;
    bytes_ = std::make_shared<const std::string>(std::move(candidate));
    return true;
}

} // namespace frog::native_engine
