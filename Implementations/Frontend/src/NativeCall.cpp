#include "frog_native_engine/NativeCall.hpp"
#include "frog_native_engine/NumericValueLexeme.hpp"

#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <utility>

namespace frog::native_engine {
namespace {
using Json = source::JsonValue;
using Type = binding::TypeRef;
using Category = NativeCallDiagnosticCategory;
using Severity = NativeCallDiagnosticSeverity;

template<class E> std::span<const std::pair<E, std::string_view>> enumEntries();
#define NC_VALUE(E, V, T) std::pair{E::V, std::string_view{T}}
#define NC_ENUM(E, ...) template<> std::span<const std::pair<E, std::string_view>> enumEntries<E>() { \
    static constexpr std::pair<E, std::string_view> entries[]{__VA_ARGS__}; return entries; }
NC_ENUM(NativeCallCallingConvention, NC_VALUE(NativeCallCallingConvention,Cdecl,"cdecl"), NC_VALUE(NativeCallCallingConvention,Stdcall,"stdcall"))
NC_ENUM(NativeCallThread, NC_VALUE(NativeCallThread,Ui,"ui"), NC_VALUE(NativeCallThread,Any,"any"))
NC_ENUM(NativeCallErrorChecking, NC_VALUE(NativeCallErrorChecking,Default,"default"), NC_VALUE(NativeCallErrorChecking,Maximum,"maximum"), NC_VALUE(NativeCallErrorChecking,Disabled,"disabled"))
NC_ENUM(NativeCallPathMode, NC_VALUE(NativeCallPathMode,Absolute,"absolute"), NC_VALUE(NativeCallPathMode,DocumentRelative,"document_relative"), NC_VALUE(NativeCallPathMode,ProjectRelative,"project_relative"), NC_VALUE(NativeCallPathMode,ApplicationRelative,"application_relative"), NC_VALUE(NativeCallPathMode,LibraryName,"library_name"))
NC_ENUM(NativeCallKind, NC_VALUE(NativeCallKind,Void,"void"), NC_VALUE(NativeCallKind,Numeric,"numeric"), NC_VALUE(NativeCallKind,Pointer,"pointer"), NC_VALUE(NativeCallKind,Array,"array"), NC_VALUE(NativeCallKind,String,"string"), NC_VALUE(NativeCallKind,AdaptToType,"adapt_to_type"), NC_VALUE(NativeCallKind,Cluster,"cluster"), NC_VALUE(NativeCallKind,InstanceDataPointer,"instance_data_pointer"), NC_VALUE(NativeCallKind,LabViewHandle,"labview_handle"), NC_VALUE(NativeCallKind,Waveform,"waveform"), NC_VALUE(NativeCallKind,DigitalWaveform,"digital_waveform"), NC_VALUE(NativeCallKind,DigitalData,"digital_data"), NC_VALUE(NativeCallKind,ActiveX,"activex"))
NC_ENUM(NativeCallNumericType, NC_VALUE(NativeCallNumericType,I8,"i8"), NC_VALUE(NativeCallNumericType,U8,"u8"), NC_VALUE(NativeCallNumericType,I16,"i16"), NC_VALUE(NativeCallNumericType,U16,"u16"), NC_VALUE(NativeCallNumericType,I32,"i32"), NC_VALUE(NativeCallNumericType,U32,"u32"), NC_VALUE(NativeCallNumericType,I64,"i64"), NC_VALUE(NativeCallNumericType,U64,"u64"), NC_VALUE(NativeCallNumericType,F32,"f32"), NC_VALUE(NativeCallNumericType,F64,"f64"), NC_VALUE(NativeCallNumericType,IntPtr,"intptr"), NC_VALUE(NativeCallNumericType,UIntPtr,"uintptr"), NC_VALUE(NativeCallNumericType,CBool,"c_bool"), NC_VALUE(NativeCallNumericType,Win32Bool,"win32_bool"))
NC_ENUM(NativeCallPass, NC_VALUE(NativeCallPass,Value,"value"), NC_VALUE(NativeCallPass,Pointer,"pointer"))
NC_ENUM(NativeCallAccess, NC_VALUE(NativeCallAccess,ReadOnly,"read_only"), NC_VALUE(NativeCallAccess,ReadWrite,"read_write"), NC_VALUE(NativeCallAccess,WriteOnly,"write_only"))
NC_ENUM(NativeCallInputRequirement, NC_VALUE(NativeCallInputRequirement,Required,"required"), NC_VALUE(NativeCallInputRequirement,Recommended,"recommended"), NC_VALUE(NativeCallInputRequirement,Optional,"optional"))
NC_ENUM(NativeCallArrayFormat, NC_VALUE(NativeCallArrayFormat,DataPointer,"data_pointer"), NC_VALUE(NativeCallArrayFormat,LabViewHandle,"labview_handle"))
NC_ENUM(NativeCallStringFormat, NC_VALUE(NativeCallStringFormat,CString,"c_string"), NC_VALUE(NativeCallStringFormat,PascalString,"pascal_string"), NC_VALUE(NativeCallStringFormat,LabViewHandle,"labview_handle"))
NC_ENUM(NativeCallStringEncoding, NC_VALUE(NativeCallStringEncoding,Utf8,"utf8"), NC_VALUE(NativeCallStringEncoding,Utf16,"utf16"), NC_VALUE(NativeCallStringEncoding,Ansi,"ansi"))
NC_ENUM(NativeCallOwnership, NC_VALUE(NativeCallOwnership,Borrowed,"borrowed"), NC_VALUE(NativeCallOwnership,CallerOwned,"caller_owned"), NC_VALUE(NativeCallOwnership,LibraryOwned,"library_owned"))
NC_ENUM(NativeCallInitialization, NC_VALUE(NativeCallInitialization,Zero,"zero"), NC_VALUE(NativeCallInitialization,DefaultValue,"default_value"))
NC_ENUM(NativeCallCapacitySource, NC_VALUE(NativeCallCapacitySource,Unspecified,"unspecified"), NC_VALUE(NativeCallCapacitySource,Constant,"constant"), NC_VALUE(NativeCallCapacitySource,Parameter,"parameter"), NC_VALUE(NativeCallCapacitySource,InputLength,"input_length"))
NC_ENUM(NativeCallCapacityUnit, NC_VALUE(NativeCallCapacityUnit,Elements,"elements"), NC_VALUE(NativeCallCapacityUnit,Bytes,"bytes"), NC_VALUE(NativeCallCapacityUnit,Utf16CodeUnits,"utf16_code_units"))
#undef NC_ENUM
#undef NC_VALUE

template<class E> std::string_view token(E value) {
    for (const auto &[candidate, text] : enumEntries<E>()) if (candidate == value) return text;
    return {};
}
Json object() { Json value; value.type = Json::Type::Object; return value; }
Json array() { Json value; value.type = Json::Type::Array; return value; }
Json string(std::string_view text) { Json value; value.type = Json::Type::String; value.string = text; return value; }
Json boolean(bool flag) { Json value; value.type = Json::Type::Boolean; value.boolean = flag; return value; }
Json integer(std::uint64_t number) {
    Json value; value.type = Json::Type::Number; value.number = static_cast<double>(number);
    value.string = std::to_string(number); return value;
}
Json number(double number) {
    Json value; value.type = Json::Type::Number; value.number = number;
    char buffer[64]; const auto result = std::to_chars(buffer, buffer + sizeof(buffer), number);
    if (result.ec == std::errc{}) value.string.assign(buffer, result.ptr);
    return value;
}
template<class E> Json enumeration(E value) { return string(token(value)); }
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
bool readNumber(const Json &json, std::string_view key, double &result) {
    const auto *value = json.member(key);
    if (!value || value->type != Json::Type::Number) return false;
    result = value->number;
    if (!value->string.empty()) {
        const auto parsed = std::from_chars(value->string.data(), value->string.data() + value->string.size(), result);
        if (parsed.ec != std::errc{} || parsed.ptr != value->string.data() + value->string.size()) return false;
    }
    return std::isfinite(result);
}
template<class E> bool readEnum(const Json &json, std::string_view key, E &result) {
    std::string text; if (!readString(json, key, text)) return false;
    for (const auto &[candidate, name] : enumEntries<E>()) if (text == name) { result = candidate; return true; }
    return false;
}
bool readOptionalString(const Json &json, std::string_view key, std::optional<std::string> &result) {
    const auto *value = json.member(key);
    if (!value || value->type == Json::Type::Null) { result.reset(); return true; }
    if (value->type != Json::Type::String) return false;
    result = value->string; return true;
}
Json typeRefJson(const Type &type) {
    auto value = object(); value.object["canonical"] = string(type.canonical());
    auto names = array(); for (const auto &name : type.fieldNames) names.array.push_back(string(name));
    value.object["field_names"] = std::move(names);
    value.object["choice_kind"] = integer(static_cast<std::uint64_t>(type.choiceKind));
    auto choices = array();
    for (const auto &choice : type.choices) {
        auto item = object(); item.object["id"] = string(choice.id); item.object["label"] = string(choice.label);
        item.object["value"] = integer(choice.value); item.object["disabled"] = boolean(choice.disabled);
        item.object["visible"] = boolean(choice.visible); choices.array.push_back(std::move(item));
    }
    value.object["choices"] = std::move(choices);
    auto children = array(); for (const auto &child : type.children) children.array.push_back(typeRefJson(child));
    value.object["children"] = std::move(children); return value;
}
bool readTypeRef(const Json *json, Type &type, std::size_t depth = 0) {
    if (!json || json->type != Json::Type::Object || depth > 32) return false;
    std::string canonical; std::uint64_t choiceKind = 0;
    if (!readString(*json, "canonical", canonical) || !readInteger(*json, "choice_kind", choiceKind) ||
        choiceKind > static_cast<std::uint64_t>(binding::ChoiceKind::Dropdown)) return false;
    auto parsed = binding::parseType(canonical); if (!parsed.valid()) return false;
    type = std::move(parsed.type);
    const auto *children = json->member("children"), *names = json->member("field_names"), *choices = json->member("choices");
    if (!children || children->type != Json::Type::Array || children->array.size() != type.children.size() ||
        !names || names->type != Json::Type::Array || !choices || choices->type != Json::Type::Array) return false;
    for (std::size_t index = 0; index < children->array.size(); ++index) {
        Type child; if (!readTypeRef(&children->array[index], child, depth + 1) ||
            child.canonical() != type.children[index].canonical()) return false;
        type.children[index] = std::move(child);
    }
    for (const auto &name : names->array) {
        if (name.type != Json::Type::String) return false;
        type.fieldNames.push_back(name.string);
    }
    if (!type.fieldNames.empty() && type.fieldNames.size() != type.fieldIds.size()) return false;
    type.choiceKind = static_cast<binding::ChoiceKind>(choiceKind);
    for (const auto &choice : choices->array) {
        binding::ChoiceItemMetadata item; std::uint64_t value = 0;
        if (!readString(choice, "id", item.id) || !readString(choice, "label", item.label) ||
            !readInteger(choice, "value", value) || value > UINT32_MAX ||
            !readBool(choice, "disabled", item.disabled) || !readBool(choice, "visible", item.visible)) return false;
        item.value = static_cast<std::uint32_t>(value); type.choices.push_back(std::move(item));
    }
    return type.canonical() == canonical;
}
Json capacityJson(const NativeCallCapacity &capacity) {
    auto value = object(); value.object["source"] = enumeration(capacity.source);
    value.object["constant"] = integer(capacity.constant); value.object["parameter_id"] = string(capacity.parameterId);
    value.object["unit"] = enumeration(capacity.unit); return value;
}
bool readCapacity(const Json *json, NativeCallCapacity &capacity) {
    return json && json->type == Json::Type::Object && readEnum(*json, "source", capacity.source) &&
        readInteger(*json, "constant", capacity.constant) && readString(*json, "parameter_id", capacity.parameterId) &&
        readEnum(*json, "unit", capacity.unit);
}
Json nativeTypeJson(const NativeCallType &type) {
    auto value = object(); value.object["kind"] = enumeration(type.kind);
    value.object["numeric_type"] = enumeration(type.numericType); value.object["element_type"] = enumeration(type.elementType);
    value.object["rank"] = integer(type.rank); value.object["array_format"] = enumeration(type.arrayFormat);
    value.object["string_format"] = enumeration(type.stringFormat); value.object["string_encoding"] = enumeration(type.stringEncoding);
    value.object["ansi_code_page"] = integer(type.ansiCodePage); value.object["adapted_type"] = typeRefJson(type.adaptedType);
    auto layout = object(); layout.object["c_type_name"] = string(type.structLayout.cTypeName);
    layout.object["byte_size"] = integer(type.structLayout.byteSize); layout.object["alignment"] = integer(type.structLayout.alignment);
    layout.object["packing"] = integer(type.structLayout.packing); auto fields = array();
    for (const auto &field : type.structLayout.fields) {
        auto item = object(); item.object["field_id"] = string(field.fieldId);
        item.object["numeric_type"] = enumeration(field.numericType); item.object["offset"] = integer(field.offset);
        fields.array.push_back(std::move(item));
    }
    layout.object["fields"] = std::move(fields); value.object["struct_layout"] = std::move(layout); return value;
}
bool readNativeType(const Json *json, NativeCallType &type) {
    std::uint64_t rank = 0, page = 0;
    if (!json || json->type != Json::Type::Object || !readEnum(*json, "kind", type.kind) ||
        !readEnum(*json, "numeric_type", type.numericType) || !readEnum(*json, "element_type", type.elementType) ||
        !readInteger(*json, "rank", rank) || rank > SIZE_MAX || !readEnum(*json, "array_format", type.arrayFormat) ||
        !readEnum(*json, "string_format", type.stringFormat) || !readEnum(*json, "string_encoding", type.stringEncoding) ||
        !readInteger(*json, "ansi_code_page", page) || page > UINT32_MAX ||
        !readTypeRef(json->member("adapted_type"), type.adaptedType)) return false;
    type.rank = static_cast<std::size_t>(rank); type.ansiCodePage = static_cast<std::uint32_t>(page);
    const auto *layout = json->member("struct_layout");
    if (!layout || layout->type != Json::Type::Object ||
        !readString(*layout, "c_type_name", type.structLayout.cTypeName) ||
        !readInteger(*layout, "byte_size", type.structLayout.byteSize) ||
        !readInteger(*layout, "alignment", type.structLayout.alignment) ||
        !readInteger(*layout, "packing", type.structLayout.packing)) return false;
    const auto *fields = layout->member("fields"); if (!fields || fields->type != Json::Type::Array) return false;
    for (const auto &item : fields->array) {
        NativeCallStructField field;
        if (!readString(item, "field_id", field.fieldId) || !readEnum(item, "numeric_type", field.numericType) ||
            !readInteger(item, "offset", field.offset)) return false;
        type.structLayout.fields.push_back(std::move(field));
    }
    return true;
}
bool safeId(std::string_view id) {
    return !id.empty() && std::all_of(id.begin(), id.end(), [](unsigned char ch) {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') || ch == '_';
    });
}
NativeCallDiagnostic diagnostic(std::string code, std::string message, std::string_view id = {},
    Category category = Category::InvalidConfiguration, Severity severity = Severity::Error) {
    return {std::move(code), std::move(message), std::string(id), category, severity};
}
bool operationError(std::vector<NativeCallDiagnostic> *out, std::string code, std::string message, std::string_view id = {}) {
    if (out) out->push_back(diagnostic(std::move(code), std::move(message), id)); return false;
}
std::vector<std::string> abiIds(const NativeCallConfiguration &config) {
    std::vector<std::string> ids; for (const auto &parameter : config.arguments) ids.push_back(parameter.id); return ids;
}
bool identitySetValid(const NativeCallConfiguration &config) {
    std::set<std::string> identities;
    for (const auto &parameter : config.arguments)
        if (!safeId(parameter.id) || !identities.insert(parameter.id).second) return false;
    return true;
}
bool visualValid(const NativeCallConfiguration &config) {
    if (!identitySetValid(config) || config.presentation.visualParameterOrder.size() != config.arguments.size()) return false;
    auto visual = config.presentation.visualParameterOrder, abi = abiIds(config);
    std::sort(visual.begin(), visual.end()); std::sort(abi.begin(), abi.end()); return visual == abi;
}
Type numericTypeRef(NativeCallNumericType type, std::size_t pointerBits) {
    if (type == NativeCallNumericType::CBool || type == NativeCallNumericType::Win32Bool) return Type::primitive("bool");
    if (type == NativeCallNumericType::IntPtr || type == NativeCallNumericType::UIntPtr) {
        if (pointerBits != 32 && pointerBits != 64) return {};
        return Type::primitive(std::string(type == NativeCallNumericType::IntPtr ? "i" : "u") + std::to_string(pointerBits));
    }
    return Type::primitive(token(type));
}
bool integerNumeric(NativeCallNumericType type) {
    switch (type) {
    case NativeCallNumericType::I8: case NativeCallNumericType::U8: case NativeCallNumericType::I16: case NativeCallNumericType::U16:
    case NativeCallNumericType::I32: case NativeCallNumericType::U32: case NativeCallNumericType::I64: case NativeCallNumericType::U64:
    case NativeCallNumericType::IntPtr: case NativeCallNumericType::UIntPtr: return true;
    default: return false;
    }
}
std::uint64_t scalarBytes(NativeCallNumericType type, std::size_t pointerBits) {
    switch (type) {
    case NativeCallNumericType::I8: case NativeCallNumericType::U8: case NativeCallNumericType::CBool: return 1;
    case NativeCallNumericType::I16: case NativeCallNumericType::U16: return 2;
    case NativeCallNumericType::I32: case NativeCallNumericType::U32: case NativeCallNumericType::F32: case NativeCallNumericType::Win32Bool: return 4;
    case NativeCallNumericType::I64: case NativeCallNumericType::U64: case NativeCallNumericType::F64: return 8;
    case NativeCallNumericType::IntPtr: case NativeCallNumericType::UIntPtr: return pointerBits == 32 ? 4 : pointerBits == 64 ? 8 : 0;
    }
    return 0;
}
bool powerOfTwo(std::uint64_t value) { return value && (value & (value - 1)) == 0; }
bool validDefaultScalar(const Json &value, NativeCallNumericType type, std::size_t pointerBits) {
    if (type == NativeCallNumericType::CBool || type == NativeCallNumericType::Win32Bool) return value.type == Json::Type::Boolean;
    if (value.type != Json::Type::Number) return false;
    if (integerNumeric(type)) {
        const auto frogType = numericTypeRef(type, pointerBits);
        // Unknown pointer width stays a target diagnostic; validate the largest declared domain here.
        const auto name = frogType.known() ? frogType.name : type == NativeCallNumericType::IntPtr ? "i64" : "u64";
        const bool isSigned = name.starts_with('i');
        const auto canonical = numeric_value::canonicalWideIntegerLexeme(value.string, isSigned ? "i64" : "u64");
        if (!canonical) return false;
        if (name == "i64" || name == "u64") return true;
        if (isSigned) {
            std::int64_t number = 0; const auto result = std::from_chars(canonical->data(), canonical->data() + canonical->size(), number);
            const auto bits = scalarBytes(type, pointerBits) * 8;
            return result.ec == std::errc{} && bits && number >= -(INT64_C(1) << (bits - 1)) && number < (INT64_C(1) << (bits - 1));
        }
        std::uint64_t number = 0; const auto result = std::from_chars(canonical->data(), canonical->data() + canonical->size(), number);
        const auto bits = scalarBytes(type, pointerBits) * 8;
        return result.ec == std::errc{} && bits && number < (UINT64_C(1) << bits);
    }
    double number = 0; const auto result = std::from_chars(value.string.data(), value.string.data() + value.string.size(), number);
    return result.ec == std::errc{} && result.ptr == value.string.data() + value.string.size() && std::isfinite(number) &&
        (type != NativeCallNumericType::F32 || std::abs(number) <= std::numeric_limits<float>::max());
}
bool defaultArrayValid(const Json &value, NativeCallNumericType element, std::size_t rank, std::size_t pointerBits) {
    if (value.type != Json::Type::Array || rank == 0 || rank > 32) return false;
    const auto sameShape = [&](auto &&self, const Json &first, const Json &second, std::size_t depth) -> bool {
        if (first.type != Json::Type::Array || second.type != Json::Type::Array || first.array.size() != second.array.size()) return false;
        if (depth > 1) for (std::size_t i = 0; i < first.array.size(); ++i)
            if (!self(self, first.array[i], second.array[i], depth - 1)) return false;
        return true;
    };
    for (const auto &item : value.array) {
        if (rank == 1 ? !validDefaultScalar(item, element, pointerBits) : !defaultArrayValid(item, element, rank - 1, pointerBits)) return false;
        if (rank > 1 && !sameShape(sameShape, value.array.front(), item, rank - 1)) return false;
    }
    return true;
}
std::string cNumeric(NativeCallNumericType type) {
    switch (type) {
    case NativeCallNumericType::I8: return "int8_t"; case NativeCallNumericType::U8: return "uint8_t";
    case NativeCallNumericType::I16: return "int16_t"; case NativeCallNumericType::U16: return "uint16_t";
    case NativeCallNumericType::I32: return "int32_t"; case NativeCallNumericType::U32: return "uint32_t";
    case NativeCallNumericType::I64: return "int64_t"; case NativeCallNumericType::U64: return "uint64_t";
    case NativeCallNumericType::F32: return "float"; case NativeCallNumericType::F64: return "double";
    case NativeCallNumericType::IntPtr: return "intptr_t"; case NativeCallNumericType::UIntPtr: return "uintptr_t";
    case NativeCallNumericType::CBool: return "_Bool"; case NativeCallNumericType::Win32Bool: return "int32_t";
    }
    return {};
}
bool cIdentifier(std::string_view name) {
    const auto letter = [](unsigned char ch) { return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'); };
    return !name.empty() && (letter(static_cast<unsigned char>(name.front())) || name.front() == '_') &&
        std::all_of(name.begin(), name.end(), [&](unsigned char ch) { return letter(ch) || (ch >= '0' && ch <= '9') || ch == '_'; });
}
std::string cName(std::string_view name, std::string_view fallback, std::set<std::string> &used) {
    static const std::set<std::string_view> reserved{"auto","break","case","char","const","continue","default","do","double","else","enum","extern","float","for","goto","if","inline","int","long","register","restrict","return","short","signed","sizeof","static","struct","switch","typedef","union","unsigned","void","volatile","while","_Alignas","_Alignof","_Atomic","_Bool","_Complex","_Generic","_Imaginary","_Noreturn","_Static_assert","_Thread_local"};
    std::string result;
    for (unsigned char ch : name) result += ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9') || ch == '_') ? static_cast<char>(ch) : '_';
    if (result.empty()) result = fallback;
    if (result.front() == '_' || (result.front() >= '0' && result.front() <= '9') || reserved.contains(result)) result = "frog_" + result;
    const auto base = result; std::size_t suffix = 2;
    while (!used.insert(result).second) result = base + "_" + std::to_string(suffix++);
    return result;
}
std::string cType(const NativeCallType &type) {
    switch (type.kind) {
    case NativeCallKind::Void: return "void";
    case NativeCallKind::Numeric: return cNumeric(type.numericType);
    case NativeCallKind::Pointer: return "void *";
    case NativeCallKind::Array: return type.arrayFormat == NativeCallArrayFormat::DataPointer ? cNumeric(type.elementType) + " *" : std::string{};
    case NativeCallKind::String:
        if (type.stringFormat == NativeCallStringFormat::LabViewHandle) return {};
        return type.stringEncoding == NativeCallStringEncoding::Utf16 ? "uint16_t *" : "char *";
    case NativeCallKind::InstanceDataPointer: return "void **";
    case NativeCallKind::Cluster: case NativeCallKind::AdaptToType:
        if (type.adaptedType.kind == binding::TypeKind::Cluster && cIdentifier(type.structLayout.cTypeName)) return "struct " + type.structLayout.cTypeName;
        if (type.kind == NativeCallKind::AdaptToType && type.adaptedType.kind == binding::TypeKind::Primitive) {
            for (const auto &[numeric, name] : enumEntries<NativeCallNumericType>())
                if (type.adaptedType.name == name) return cNumeric(numeric);
        }
        return {};
    default: return {};
    }
}
void appendJsonText(std::string &output, const Json &value) {
    switch (value.type) {
    case Json::Type::Null: output += "null"; break;
    case Json::Type::Boolean: output += value.boolean ? "true" : "false"; break;
    case Json::Type::Number: output += value.string.empty() ? number(value.number).string : value.string; break;
    case Json::Type::String:
        output += '"';
        for (unsigned char ch : value.string) {
            if (ch == '"' || ch == '\\') { output += '\\'; output += static_cast<char>(ch); }
            else if (ch < 32) { constexpr char hex[]{"0123456789abcdef"}; output += "\\u00"; output += hex[ch >> 4]; output += hex[ch & 15]; }
            else output += static_cast<char>(ch);
        }
        output += '"'; break;
    case Json::Type::Array:
        output += '['; for (std::size_t i = 0; i < value.array.size(); ++i) { if (i) output += ','; appendJsonText(output, value.array[i]); } output += ']'; break;
    case Json::Type::Object:
        output += '{'; { bool first = true; for (const auto &[key, item] : value.object) {
            if (!first) output += ','; first = false; appendJsonText(output, string(key)); output += ':'; appendJsonText(output, item);
        } } output += '}'; break;
    }
}
} // namespace

const NativeCallParameter *nativeCallParameter(const NativeCallConfiguration &config, std::string_view id) {
    const auto found = std::find_if(config.arguments.begin(), config.arguments.end(),
        [id](const auto &parameter) { return parameter.id == id; });
    return found == config.arguments.end() ? nullptr : &*found;
}

binding::TypeRef nativeCallTypeRef(const NativeCallType &type, std::size_t pointerBits) {
    switch (type.kind) {
    case NativeCallKind::Numeric: return numericTypeRef(type.numericType, pointerBits);
    case NativeCallKind::Pointer: return Type::reference("connectivity.native_call.pointer");
    case NativeCallKind::String: return Type::primitive("string");
    case NativeCallKind::Array: {
        const auto element = numericTypeRef(type.elementType, pointerBits);
        return element.known() && type.rank > 0 && type.rank <= 32 ? Type::array(element, type.rank) : Type{};
    }
    case NativeCallKind::AdaptToType: case NativeCallKind::Cluster: case NativeCallKind::Waveform:
    case NativeCallKind::DigitalWaveform: case NativeCallKind::DigitalData: return type.adaptedType;
    case NativeCallKind::LabViewHandle: return Type::reference("connectivity.native_call.labview_handle");
    case NativeCallKind::ActiveX: return Type::reference("connectivity.activex.automation");
    default: return {};
    }
}

std::vector<NativeCallPort> nativeCallPorts(const NativeCallConfiguration &config, std::size_t pointerBits) {
    std::vector<NativeCallPort> result;
    const auto add = [&](std::string id, std::string name, bool output, Type type, NativeCallInputRequirement requirement,
        std::optional<std::string> defaultValue, std::string description) {
        const auto reason = type.known() ? std::string{} : "Native type needs an explicit FROG type or target pointer width.";
        result.push_back({std::move(id), std::move(name), output, std::move(type), requirement,
            std::move(defaultValue), std::move(description), reason});
    };
    if (config.library.specifyPathOnDiagram) {
        add("library_path_in", "library path", false, Type::primitive("path"), NativeCallInputRequirement::Required, {}, "Effective library path resolved against the configured base.");
        add("library_path_out", "library path", true, Type::primitive("path"), NativeCallInputRequirement::Optional, {}, "Effective library path passed through after binding.");
    }
    if (config.returnDefinition.type.kind != NativeCallKind::Void)
        add("return_value", config.returnDefinition.displayName, true, nativeCallTypeRef(config.returnDefinition.type, pointerBits),
            NativeCallInputRequirement::Optional, {}, "Native return value; never a generic execution error code.");
    // Logical ports use ABI identities. A renderer selects their visual row by ID separately.
    for (const auto &parameter : config.arguments) {
        if (parameter.type.kind == NativeCallKind::InstanceDataPointer) continue;
        const auto type = nativeCallTypeRef(parameter.type, pointerBits);
        add("param." + parameter.id + ".in", parameter.displayName, false, type,
            parameter.access == NativeCallAccess::WriteOnly ? NativeCallInputRequirement::Optional : parameter.inputRequirement,
            parameter.defaultCanonicalValue, parameter.access == NativeCallAccess::WriteOnly ?
            "Optional initializer for runtime-owned output storage." : "Value supplied to this ABI argument.");
        add("param." + parameter.id + ".out", parameter.displayName, true, type, NativeCallInputRequirement::Optional, {},
            parameter.pass == NativeCallPass::Value && parameter.type.kind == NativeCallKind::Numeric ?
            "The scalar value actually passed to the native function." : parameter.access == NativeCallAccess::ReadOnly ?
            "Original read-only value passed through." : "Value read from bounded argument storage after the native call.");
    }
    const auto errorType = Type::cluster({"status", "code", "source"},
        {Type::primitive("bool"), Type::primitive("i32"), Type::primitive("string")});
    add("error_in", "error in", false, errorType, NativeCallInputRequirement::Recommended, {}, "An incoming error bypasses the native invocation.");
    add("error_out", "error out", true, errorType, NativeCallInputRequirement::Optional, {}, "Binding or execution failure, separate from the native return value.");
    return result;
}

NativeCallValidation nativeCallValidate(const NativeCallConfiguration &config, const NativeCallTargetCapabilities *target) {
    NativeCallValidation result;
    const auto add = [&](std::string code, std::string message, std::string_view id = {}, Category category = Category::InvalidConfiguration,
        Severity severity = Severity::Error) { result.diagnostics.push_back(diagnostic(std::move(code), std::move(message), id, category, severity)); };
    const auto unavailable = [&](bool supported, std::string code, std::string message, std::string_view id = {}) {
        if (target && !supported) add(std::move(code), std::move(message), id, Category::Unavailable);
    };
    const auto pointerBits = target ? target->pointerBits : 0;
    if (token(config.callingConvention).empty() || token(config.thread).empty() || token(config.errorChecking).empty() ||
        token(config.library.mode).empty() || token(config.library.dynamicRelativeBase).empty())
        add("configuration_enum", "An ABI, path, thread or error policy is outside its declared enum domain.");
    if (config.schemaVersion != 1) add("schema_version", "Unsupported native call configuration version.");
    if (config.symbol.empty()) add("symbol_missing", "Select a native export before execution.", {}, Category::Incomplete);
    if (config.symbol.find('\0') != config.symbol.npos) add("symbol_nul", "The exact export name cannot contain a NUL byte.");
    if (!config.library.specifyPathOnDiagram && config.library.value.empty()) add("library_missing", "Select a library before execution.", {}, Category::Incomplete);
    if (config.library.value.find('\0') != config.library.value.npos) add("library_nul", "A library path cannot contain a NUL byte.");
    if (config.variadic) add("variadic_unavailable", "Variadic native signatures have no supported marshalling profile.", {}, Category::Unavailable);
    if (!std::isfinite(config.presentation.requestedWidth) || config.presentation.requestedWidth < 0) add("presentation_width", "Node width must be finite and nonnegative.");
    if (config.nextIdentity == 0) add("identity_counter", "The next parameter identity must be positive.");
    if (!visualValid(config)) add("visual_permutation", "Visual order must contain each valid ABI parameter ID once.");
    std::set<std::string> ids;
    for (const auto &parameter : config.arguments) {
        if (!safeId(parameter.id)) add("parameter_identity", "A parameter ID must be nonempty ASCII letters, digits or underscores.", parameter.id);
        if (!ids.insert(parameter.id).second) add("duplicate_parameter_identity", "Two ABI arguments have the same stable ID.", parameter.id);
        if (parameter.id.starts_with('p')) {
            std::uint64_t generated = 0; const auto digits = std::string_view(parameter.id).substr(1);
            const auto parsed = std::from_chars(digits.data(), digits.data() + digits.size(), generated);
            if (parsed.ec == std::errc{} && parsed.ptr == digits.data() + digits.size() && generated >= config.nextIdentity)
                add("identity_counter", "The stored identity counter would reuse an existing parameter identity.", parameter.id);
        }
    }
    const auto capacity = [&](const NativeCallCapacity &value, std::string_view id, bool allowInputLength, bool required) {
        if (token(value.source).empty() || token(value.unit).empty()) { add("capacity_enum", "A capacity source or unit is outside its declared enum domain.", id); return; }
        if (value.source == NativeCallCapacitySource::Unspecified) {
            if (required) add("capacity_missing", "A finite buffer capacity must be declared.", id, Category::Incomplete);
        } else if (value.source == NativeCallCapacitySource::InputLength) {
            if (!allowInputLength) add("capacity_input_length", "This storage cannot derive capacity from an unavailable input value.", id);
        } else if (value.source == NativeCallCapacitySource::Parameter) {
            const auto *source = nativeCallParameter(config, value.parameterId);
            if (!source) add("capacity_parameter_missing", "Capacity refers to a deleted or unknown parameter ID; it is not retargeted.", id);
            else if (source->id == id || source->type.kind != NativeCallKind::Numeric || !integerNumeric(source->type.numericType) || source->access == NativeCallAccess::WriteOnly)
                add("capacity_parameter_type", "Capacity needs another integer input whose value is available before invocation.", id);
        }
    };
    const auto validateType = [&](const NativeCallType &type, std::string_view id, bool isReturn) {
        if (token(type.kind).empty() || token(type.numericType).empty() || token(type.elementType).empty() ||
            token(type.arrayFormat).empty() || token(type.stringFormat).empty() || token(type.stringEncoding).empty()) {
            add("type_enum", "A native type representation is outside its declared enum domain.", id); return;
        }
        if (type.kind == NativeCallKind::Void) { if (!isReturn) add("argument_void", "An ABI argument cannot have Void type.", id); return; }
        if (type.kind == NativeCallKind::Numeric) {
            unavailable(target && target->scalar, "scalar_unavailable", "The target does not supply this scalar ABI profile.", id);
            if ((type.numericType == NativeCallNumericType::IntPtr || type.numericType == NativeCallNumericType::UIntPtr) && target && pointerBits != 32 && pointerBits != 64)
                add("pointer_width_unknown", "Pointer-sized integers require a known 32- or 64-bit target.", id, Category::Unavailable);
        }
        if (type.kind == NativeCallKind::Pointer) unavailable(target && target->pointers, "pointer_unavailable", "The target has no opaque-pointer ABI profile.", id);
        if (type.kind == NativeCallKind::Array) {
            if (type.rank == 0 || type.rank > 32) add("array_rank", "An array rank must be between 1 and 32.", id);
            unavailable(target && target->arrays, "array_unavailable", "The target has no bounded contiguous-array marshaller.", id);
            if (type.rank > 1) unavailable(target && target->multidimensionalArrays, "array_shape_unavailable", "This target does not marshal multidimensional arrays with explicit shape.", id);
            if (type.arrayFormat == NativeCallArrayFormat::LabViewHandle) unavailable(target && target->labViewHandles, "labview_array_unavailable", "NI array handles need an explicit NI memory manager/profile.", id);
            if ((type.elementType == NativeCallNumericType::IntPtr || type.elementType == NativeCallNumericType::UIntPtr) && target && pointerBits != 32 && pointerBits != 64)
                add("pointer_width_unknown", "Pointer-sized array elements require a known target width.", id, Category::Unavailable);
            if (isReturn) add("array_return_unavailable", "A raw returned array pointer has no owned-array shape/lifetime contract.", id, Category::Unavailable);
        }
        if (type.kind == NativeCallKind::String) {
            unavailable(target && target->strings, "string_unavailable", "The target has no bounded string marshaller.", id);
            if (type.stringEncoding == NativeCallStringEncoding::Utf16) unavailable(target && target->utf16, "utf16_unavailable", "The target does not implement the declared UTF-16 string profile.", id);
            if (type.stringEncoding == NativeCallStringEncoding::Ansi) {
                if (!type.ansiCodePage) add("ansi_code_page", "ANSI strings require an explicit code page; the host default is not assumed.", id, Category::Incomplete);
                unavailable(target && target->ansi, "ansi_unavailable", "The target cannot marshal the declared ANSI code page.", id);
            }
            if (type.stringFormat == NativeCallStringFormat::PascalString) {
                if (type.stringEncoding == NativeCallStringEncoding::Utf16) add("pascal_encoding", "The Pascal profile uses a one-byte length and byte characters, not UTF-16.", id);
                unavailable(target && target->pascalStrings, "pascal_unavailable", "The target does not implement the U8-length Pascal string profile.", id);
            }
            if (type.stringFormat == NativeCallStringFormat::LabViewHandle) unavailable(target && target->labViewHandles, "labview_string_unavailable", "NI string handles require an explicit NI memory manager/profile.", id);
        }
        if (type.kind == NativeCallKind::AdaptToType || type.kind == NativeCallKind::Cluster) {
            if (!type.adaptedType.known()) add("adapted_type_missing", "Select a concrete FROG type and native layout.", id, Category::Incomplete);
            if (type.kind == NativeCallKind::Cluster && type.adaptedType.known() && type.adaptedType.kind != binding::TypeKind::Cluster)
                add("cluster_type", "A struct argument must use an actual Cluster type.", id);
            unavailable(target && (type.kind == NativeCallKind::Cluster ? target->structs : target->adaptToType),
                "adapted_profile_unavailable", "The target does not implement this explicitly adapted native layout.", id);
            if (type.adaptedType.kind == binding::TypeKind::Cluster) {
                const auto &layout = type.structLayout;
                if (!layout.byteSize || !powerOfTwo(layout.alignment) || (layout.packing && !powerOfTwo(layout.packing)) || !cIdentifier(layout.cTypeName))
                    add("struct_layout", "A struct needs an explicit C type name, byte size, alignment and valid packing.", id, Category::Incomplete);
                if (layout.fields.size() != type.adaptedType.fieldIds.size()) add("struct_fields", "Native fields must describe every Cluster field by stable ID.", id);
                std::set<std::string> fieldIds;
                std::vector<std::pair<std::uint64_t, std::uint64_t>> ranges;
                for (const auto &field : layout.fields) {
                    const auto found = std::find(type.adaptedType.fieldIds.begin(), type.adaptedType.fieldIds.end(), field.fieldId);
                    if (found == type.adaptedType.fieldIds.end() || !fieldIds.insert(field.fieldId).second) { add("struct_field_identity", "Unknown or repeated native struct field ID.", id); continue; }
                    const auto index = static_cast<std::size_t>(found - type.adaptedType.fieldIds.begin());
                    const auto frog = numericTypeRef(field.numericType, pointerBits);
                    if (index >= type.adaptedType.children.size() ||
                        (frog.known() && frog != type.adaptedType.children[index])) add("struct_field_type", "Native field and Cluster field types disagree.", id);
                    const auto bytes = scalarBytes(field.numericType, pointerBits);
                    if (bytes) {
                        if (field.offset > layout.byteSize || bytes > layout.byteSize - field.offset) add("struct_field_bounds", "Native field extends outside the declared struct size.", id);
                        else ranges.emplace_back(field.offset, field.offset + bytes);
                    }
                }
                std::sort(ranges.begin(), ranges.end());
                for (std::size_t i = 1; i < ranges.size(); ++i) if (ranges[i].first < ranges[i - 1].second) add("struct_field_overlap", "Declared struct fields overlap.", id);
            }
        }
        if (type.kind == NativeCallKind::InstanceDataPointer) {
            if (isReturn) add("instance_data_return", "Instance Data Pointer is an internal argument, not a return type.", id);
            unavailable(target && target->instanceData && target->lifecycleCallbacks, "instance_data_unavailable", "Instance storage requires the Studio lifecycle ABI and a provider owning its lifetime.", id);
        }
        if (type.kind == NativeCallKind::LabViewHandle) unavailable(target && target->labViewHandles, "labview_handle_unavailable", "NI handle layout and allocation are not implied by an opaque pointer.", id);
        if (type.kind == NativeCallKind::Waveform || type.kind == NativeCallKind::DigitalWaveform) unavailable(target && target->waveforms, "waveform_unavailable", "Native waveform layout needs an explicit provider profile.", id);
        if (type.kind == NativeCallKind::DigitalData) unavailable(target && target->digitalData, "digital_data_unavailable", "Native digital-data layout needs an explicit provider profile.", id);
        if (type.kind == NativeCallKind::ActiveX) unavailable(target && target->activeX, "activex_unavailable", "ActiveX ownership and COM marshalling need an explicit provider profile.", id);
    };
    validateType(config.returnDefinition.type, "return", true);
    if (token(config.returnDefinition.ownership).empty()) add("return_ownership", "The return ownership policy is invalid.", "return");
    if (config.returnDefinition.type.kind == NativeCallKind::String) {
        capacity(config.returnDefinition.maximumRead, "return", false, true);
        if (config.returnDefinition.maximumRead.source == NativeCallCapacitySource::Constant && !config.returnDefinition.maximumRead.constant)
            add("return_read_bound", "A returned C string needs a positive finite read bound.", "return");
        if (config.returnDefinition.lifetime.empty()) add("return_lifetime", "Declare the returned string pointer's lifetime.", "return", Category::Incomplete);
        if (config.returnDefinition.ownership != NativeCallOwnership::Borrowed)
            add("return_ownership_unavailable", "Returned DLL storage cannot be freed without an explicit matching deallocator contract.", "return", Category::Unavailable);
        const auto unit = config.returnDefinition.type.stringEncoding == NativeCallStringEncoding::Utf16 ? NativeCallCapacityUnit::Utf16CodeUnits : NativeCallCapacityUnit::Bytes;
        if (config.returnDefinition.maximumRead.source != NativeCallCapacitySource::Unspecified && config.returnDefinition.maximumRead.unit != unit)
            add("return_capacity_unit", "The returned string read bound must use the declared encoding's code units.", "return");
    }
    for (const auto &parameter : config.arguments) {
        if (token(parameter.pass).empty() || token(parameter.access).empty() || token(parameter.inputRequirement).empty() || token(parameter.initialization).empty())
            add("parameter_enum", "A parameter passing, access or initialization policy is invalid.", parameter.id);
        validateType(parameter.type, parameter.id, false);
        if (parameter.type.kind == NativeCallKind::Numeric && parameter.pass == NativeCallPass::Pointer)
            unavailable(target && target->pointers, "scalar_pointer_unavailable", "The target does not marshal pointer-to-scalar storage.", parameter.id);
        if ((parameter.type.kind == NativeCallKind::Array || parameter.type.kind == NativeCallKind::String) && parameter.pass != NativeCallPass::Pointer)
            add("buffer_pass", "Arrays and strings are bounded pointer arguments, not by-value aggregates.", parameter.id);
        const bool buffer = parameter.type.kind == NativeCallKind::Array || parameter.type.kind == NativeCallKind::String;
        if (buffer) {
            capacity(parameter.minimumCapacity, parameter.id, parameter.access != NativeCallAccess::WriteOnly, true);
            const auto expected = parameter.type.kind == NativeCallKind::Array ? NativeCallCapacityUnit::Elements :
                parameter.type.stringEncoding == NativeCallStringEncoding::Utf16 ? NativeCallCapacityUnit::Utf16CodeUnits : NativeCallCapacityUnit::Bytes;
            if (parameter.minimumCapacity.source != NativeCallCapacitySource::Unspecified && parameter.minimumCapacity.unit != expected)
                add("capacity_unit", "Capacity units do not match the array element or string encoding contract.", parameter.id);
            if (parameter.type.kind == NativeCallKind::String && parameter.minimumCapacity.source == NativeCallCapacitySource::Constant) {
                if (parameter.type.stringFormat == NativeCallStringFormat::CString && parameter.minimumCapacity.constant == 0)
                    add("string_terminator_capacity", "C-string capacity includes room for the terminating NUL.", parameter.id);
                if (parameter.type.stringFormat == NativeCallStringFormat::PascalString && (parameter.minimumCapacity.constant < 1 || parameter.minimumCapacity.constant > 256))
                    add("pascal_capacity", "Pascal storage includes one length byte and at most 255 content bytes.", parameter.id);
            }
            if (parameter.type.kind == NativeCallKind::Array && (parameter.type.rank > 1 || !parameter.dimensions.empty()) && parameter.dimensions.size() != parameter.type.rank)
                add("array_dimensions", "Each array dimension needs an explicit extent source.", parameter.id, Category::Incomplete);
            if (parameter.type.kind == NativeCallKind::Array && parameter.access == NativeCallAccess::WriteOnly &&
                !parameter.validLengthParameterId && parameter.dimensions.empty())
                add("output_array_length", "An output array needs an explicit shape or a valid-length parameter; buffer capacity is not its logical length.", parameter.id, Category::Incomplete);
            for (const auto &dimension : parameter.dimensions) {
                capacity(dimension, parameter.id, false, true);
                if (dimension.unit != NativeCallCapacityUnit::Elements) add("dimension_unit", "Array extents are counts of elements.", parameter.id);
            }
        }
        if (parameter.validLengthParameterId) {
            const auto *length = nativeCallParameter(config, *parameter.validLengthParameterId);
            if (!buffer || !length || length->id == parameter.id || length->type.kind != NativeCallKind::Numeric ||
                !integerNumeric(length->type.numericType) || length->pass != NativeCallPass::Pointer || length->access == NativeCallAccess::ReadOnly)
                add("valid_length_parameter", "Valid output length must reference another writable integer pointer argument.", parameter.id);
        }
        if (parameter.initialization == NativeCallInitialization::DefaultValue && !parameter.defaultCanonicalValue)
            add("initialization_default_missing", "Default-value initialization needs an explicit canonical value.", parameter.id, Category::Incomplete);
        if (parameter.defaultCanonicalValue) {
            const auto parsed = source::parseJson(*parameter.defaultCanonicalValue, nullptr, source::JsonNumberPolicy::PreserveLexeme);
            bool valid = parsed.has_value();
            if (parsed) {
                if (parameter.type.kind == NativeCallKind::Numeric) valid = validDefaultScalar(*parsed, parameter.type.numericType, pointerBits);
                else if (parameter.type.kind == NativeCallKind::String) {
                    valid = parsed->type == Json::Type::String;
                    if (valid && parameter.type.stringFormat == NativeCallStringFormat::CString) valid = parsed->string.find('\0') == parsed->string.npos;
                    if (valid && parameter.type.stringFormat == NativeCallStringFormat::PascalString && parameter.type.stringEncoding == NativeCallStringEncoding::Utf8) valid = parsed->string.size() <= 255;
                }
                else if (parameter.type.kind == NativeCallKind::Array) valid = defaultArrayValid(*parsed, parameter.type.elementType, parameter.type.rank, pointerBits);
                else if (parameter.type.kind == NativeCallKind::Pointer) valid = parsed->type == Json::Type::Null && parameter.nullable;
            }
            if (!valid) add("default_value", "The canonical default does not represent a value of the declared native argument type.", parameter.id);
        }
    }
    const bool callbacks = !config.callbacks.reserve.empty() || !config.callbacks.unreserve.empty() || !config.callbacks.abort.empty();
    if (callbacks) {
        if (config.callbacks.profileVersion != 1) add("callback_profile", "Only Studio C lifecycle ABI version 1 is defined.");
        unavailable(target && target->lifecycleCallbacks && target->instanceData, "callbacks_unavailable", "The target does not implement reserve/unreserve/abort lifecycle callbacks.");
        for (const auto *name : {&config.callbacks.reserve, &config.callbacks.unreserve, &config.callbacks.abort})
            if (name->find('\0') != name->npos) add("callback_symbol_nul", "A lifecycle export name cannot contain NUL.");
        if (config.thread == NativeCallThread::Ui && !config.callbacks.abort.empty())
            add("ui_abort_unavailable", "A blocked UI-thread native call cannot dispatch a cooperative Abort callback. Choose a supported non-UI execution mode.", {}, Category::Unavailable);
    }
    if (target) {
        unavailable(target->providerAvailable, "provider_unavailable", "No native-call provider is installed in this execution target.");
        unavailable(target->pointerBits == 32 || target->pointerBits == 64, "target_architecture_unknown", "Native ABI binding requires the target pointer width.");
        if (config.thread == NativeCallThread::Ui) unavailable(target->uiThreadDispatcher, "ui_dispatcher_unavailable", "UI-thread mode needs a real UI-thread dispatcher.");
        else unavailable(target->anyThread, "any_thread_unavailable", "The provider does not implement calls on the current execution thread.");
        if (config.callingConvention == NativeCallCallingConvention::Stdcall && target->pointerBits == 32)
            unavailable(target->stdcall, "stdcall_unavailable", "This 32-bit target does not implement stdcall.");
        unavailable(target->libraryResolved, "library_unresolved", "The configured library has not been successfully resolved for this target.");
        unavailable(target->symbolResolved, "symbol_unresolved", "The exact export has not been successfully bound in the resolved library.");
    }
    bool available = true;
    for (const auto &item : result.diagnostics) {
        if (item.severity != Severity::Error) continue;
        if (item.category == Category::InvalidConfiguration) result.structurallyValid = false;
        if (item.category == Category::Incomplete) result.complete = false;
        if (item.category == Category::Unavailable) available = false;
    }
    result.executable = target && result.structurallyValid && result.complete && available;
    return result;
}

NativeCallPrototype nativeCallPrototype(const NativeCallConfiguration &config) {
    NativeCallPrototype result;
    const auto validation = nativeCallValidate(config);
    for (const auto &item : validation.diagnostics) if (item.category == Category::InvalidConfiguration) result.diagnostics.push_back(item);
    result.valid = validation.structurallyValid && !config.symbol.empty();
    auto returnType = cType(config.returnDefinition.type);
    if (returnType.empty()) { result.valid = false; result.diagnostics.push_back(diagnostic("prototype_return", "The native return type has no defined C declaration.", "return", Category::Unavailable)); }
    std::set<std::string> names;
    const auto symbol = cName(config.symbol, "unconfigured_function", names);
    std::string arguments;
    for (std::size_t i = 0; i < config.arguments.size(); ++i) {
        const auto &parameter = config.arguments[i]; auto type = cType(parameter.type);
        if (type.empty() || parameter.type.kind == NativeCallKind::Void) {
            result.valid = false; result.diagnostics.push_back(diagnostic("prototype_parameter", "This native parameter has no defined C declaration.", parameter.id, Category::Unavailable));
        }
        if (parameter.type.kind == NativeCallKind::Numeric || parameter.type.kind == NativeCallKind::Pointer ||
            parameter.type.kind == NativeCallKind::Cluster || parameter.type.kind == NativeCallKind::AdaptToType) {
            if (parameter.pass == NativeCallPass::Pointer) {
                if (parameter.access == NativeCallAccess::ReadOnly) type = "const " + type;
                type += " *";
            }
        } else if ((parameter.type.kind == NativeCallKind::Array || parameter.type.kind == NativeCallKind::String) && parameter.access == NativeCallAccess::ReadOnly) type = "const " + type;
        if (i) arguments += ", ";
        arguments += type + " " + cName(parameter.displayName, "arg_" + std::to_string(i + 1), names);
    }
    if (arguments.empty()) arguments = "void";
    if (result.valid) result.text = "#include <stdint.h>\n" + returnType + " " +
        (config.callingConvention == NativeCallCallingConvention::Stdcall ? "__stdcall " : "") + symbol + "(" + arguments + ");";
    else result.text = "C prototype unavailable: configure a valid signature with explicitly supported native declarations.";
    return result;
}

std::vector<NativeCallDiagnostic> nativeCallMigrateVisual(NativeCallConfiguration &config) {
    std::vector<NativeCallDiagnostic> diagnostics;
    if (!identitySetValid(config)) {
        diagnostics.push_back(diagnostic("abi_identity_unrepaired", "ABI IDs are invalid; visual migration does not guess or repair the native signature."));
        return diagnostics;
    }
    if (visualValid(config)) return diagnostics;
    std::set<std::string> seen; std::vector<std::string> repaired;
    for (const auto &id : config.presentation.visualParameterOrder)
        if (nativeCallParameter(config, id) && seen.insert(id).second) repaired.push_back(id);
    for (const auto &parameter : config.arguments) if (seen.insert(parameter.id).second) repaired.push_back(parameter.id);
    config.presentation.visualParameterOrder = std::move(repaired);
    diagnostics.push_back(diagnostic("visual_order_migrated", "Unknown/duplicate visual IDs were removed; missing IDs were appended in ABI order. ABI order is unchanged.", {}, Category::Migration, Severity::Warning));
    return diagnostics;
}

std::optional<std::string> nativeCallAddParameter(NativeCallConfiguration &config, NativeCallParameter parameter,
    std::optional<std::size_t> abiPosition, std::vector<NativeCallDiagnostic> *diagnostics) {
    if (!identitySetValid(config) || !visualValid(config)) { operationError(diagnostics, "invalid_identity_set", "Repair visual presentation or invalid ABI IDs before adding an argument."); return {}; }
    const auto index = abiPosition.value_or(config.arguments.size());
    if (index > config.arguments.size()) { operationError(diagnostics, "abi_position", "The ABI insertion index is outside the argument vector."); return {}; }
    if (!parameter.id.empty()) { operationError(diagnostics, "identity_supplied", "New argument IDs are assigned by the configuration's monotonic counter."); return {}; }
    if (!config.nextIdentity || config.nextIdentity == UINT64_MAX) { operationError(diagnostics, "identity_exhausted", "No fresh monotonic parameter identity is available."); return {}; }
    for (const auto &item : nativeCallValidate(config).diagnostics) {
        if (item.code == "identity_counter") {
            if (diagnostics) diagnostics->push_back(item);
            return {};
        }
    }
    const auto id = "p" + std::to_string(config.nextIdentity);
    if (nativeCallParameter(config, id)) { operationError(diagnostics, "identity_collision", "The stored identity counter collides with an ABI argument.", id); return {}; }
    parameter.id = id;
    if (parameter.displayName.empty()) parameter.displayName = "parameter " + std::to_string(index + 1);
    config.arguments.insert(config.arguments.begin() + static_cast<std::ptrdiff_t>(index), std::move(parameter));
    config.presentation.visualParameterOrder.push_back(id); ++config.nextIdentity; return id;
}

bool nativeCallRemoveParameter(NativeCallConfiguration &config, std::string_view id, std::vector<NativeCallDiagnostic> *diagnostics) {
    const std::string identity(id); // The caller may borrow id from the argument being erased.
    if (!identitySetValid(config)) return operationError(diagnostics, "invalid_identity_set", "Cannot remove an ambiguous ABI argument.", id);
    const auto found = std::find_if(config.arguments.begin(), config.arguments.end(), [id](const auto &p) { return p.id == id; });
    if (found == config.arguments.end()) return operationError(diagnostics, "parameter_missing", "Unknown parameter ID.", id);
    config.arguments.erase(found);
    std::erase(config.presentation.visualParameterOrder, identity);
    // Capacity references intentionally retain the deleted ID and receive a targeted validation diagnostic.
    if (diagnostics) { const auto validation = nativeCallValidate(config); diagnostics->insert(diagnostics->end(), validation.diagnostics.begin(), validation.diagnostics.end()); }
    return true;
}

bool nativeCallUpdateParameter(NativeCallConfiguration &config, std::string_view id, NativeCallParameter replacement,
    std::vector<NativeCallDiagnostic> *diagnostics) {
    if (!identitySetValid(config)) return operationError(diagnostics, "invalid_identity_set", "Cannot update an ambiguous ABI argument.", id);
    const auto found = std::find_if(config.arguments.begin(), config.arguments.end(), [id](const auto &p) { return p.id == id; });
    if (found == config.arguments.end()) return operationError(diagnostics, "parameter_missing", "Unknown parameter ID.", id);
    if (!replacement.id.empty() && replacement.id != id) return operationError(diagnostics, "identity_change", "An update cannot replace a stable parameter ID.", id);
    replacement.id = std::string(id); *found = std::move(replacement); return true;
}

bool nativeCallReorderAbi(NativeCallConfiguration &config, std::span<const std::string> ids,
    std::vector<NativeCallDiagnostic> *diagnostics) {
    if (!identitySetValid(config) || ids.size() != config.arguments.size()) return operationError(diagnostics, "abi_permutation", "ABI reorder needs each existing stable ID exactly once.");
    std::set<std::string> seen; std::vector<NativeCallParameter> reordered;
    for (const auto &id : ids) {
        const auto *parameter = nativeCallParameter(config, id);
        if (!parameter || !seen.insert(id).second) return operationError(diagnostics, "abi_permutation", "ABI reorder contains a missing or repeated ID.", id);
        reordered.push_back(*parameter);
    }
    config.arguments = std::move(reordered); return true;
}

bool nativeCallSwapVisual(NativeCallConfiguration &config, std::string_view firstId, std::string_view secondId,
    std::vector<NativeCallDiagnostic> *diagnostics) {
    if (!visualValid(config)) return operationError(diagnostics, "visual_permutation", "Visual order must be valid before swapping rows.");
    auto &order = config.presentation.visualParameterOrder;
    const auto first = std::find(order.begin(), order.end(), firstId), second = std::find(order.begin(), order.end(), secondId);
    if (first == order.end() || second == order.end()) return operationError(diagnostics, "parameter_missing", "Visual swap needs two existing parameter IDs.");
    std::iter_swap(first, second); return true;
}

bool nativeCallMoveVisual(NativeCallConfiguration &config, std::string_view id, std::size_t visualIndex,
    std::vector<NativeCallDiagnostic> *diagnostics) {
    if (!visualValid(config) || visualIndex >= config.arguments.size()) return operationError(diagnostics, "visual_position", "Visual move requires a valid permutation and destination index.", id);
    auto &order = config.presentation.visualParameterOrder; const auto found = std::find(order.begin(), order.end(), id);
    if (found == order.end()) return operationError(diagnostics, "parameter_missing", "Unknown visual parameter ID.", id);
    const auto identity = *found; order.erase(found); order.insert(order.begin() + static_cast<std::ptrdiff_t>(visualIndex), identity); return true;
}

bool nativeCallResetVisual(NativeCallConfiguration &config, std::vector<NativeCallDiagnostic> *diagnostics) {
    if (!identitySetValid(config)) return operationError(diagnostics, "invalid_identity_set", "Reset cannot invent distinct identities for an invalid ABI signature.");
    config.presentation.visualParameterOrder = abiIds(config); return true;
}

source::JsonValue nativeCallToJson(const NativeCallConfiguration &config) {
    auto value = object(); value.object["schema_version"] = integer(config.schemaVersion);
    auto library = object(); library.object["mode"] = enumeration(config.library.mode); library.object["value"] = string(config.library.value);
    library.object["specify_path_on_diagram"] = boolean(config.library.specifyPathOnDiagram);
    library.object["dynamic_relative_base"] = enumeration(config.library.dynamicRelativeBase); value.object["library"] = std::move(library);
    value.object["symbol"] = string(config.symbol); value.object["calling_convention"] = enumeration(config.callingConvention);
    value.object["thread"] = enumeration(config.thread); value.object["error_checking"] = enumeration(config.errorChecking);
    auto returned = object(); returned.object["display_name"] = string(config.returnDefinition.displayName);
    returned.object["type"] = nativeTypeJson(config.returnDefinition.type); returned.object["ownership"] = enumeration(config.returnDefinition.ownership);
    returned.object["maximum_read"] = capacityJson(config.returnDefinition.maximumRead); returned.object["lifetime"] = string(config.returnDefinition.lifetime);
    value.object["return"] = std::move(returned);
    auto arguments = array();
    for (const auto &parameter : config.arguments) {
        auto item = object(); item.object["id"] = string(parameter.id); item.object["display_name"] = string(parameter.displayName);
        item.object["type"] = nativeTypeJson(parameter.type); item.object["pass"] = enumeration(parameter.pass); item.object["access"] = enumeration(parameter.access);
        item.object["input_requirement"] = enumeration(parameter.inputRequirement); item.object["initialization"] = enumeration(parameter.initialization);
        item.object["default_canonical_value"] = parameter.defaultCanonicalValue ? string(*parameter.defaultCanonicalValue) : Json{};
        item.object["nullable"] = boolean(parameter.nullable); item.object["minimum_capacity"] = capacityJson(parameter.minimumCapacity);
        auto dimensions = array(); for (const auto &dimension : parameter.dimensions) dimensions.array.push_back(capacityJson(dimension));
        item.object["dimensions"] = std::move(dimensions);
        item.object["valid_length_parameter_id"] = parameter.validLengthParameterId ? string(*parameter.validLengthParameterId) : Json{};
        arguments.array.push_back(std::move(item));
    }
    value.object["arguments"] = std::move(arguments);
    auto callbacks = object(); callbacks.object["profile_version"] = integer(config.callbacks.profileVersion);
    callbacks.object["reserve"] = string(config.callbacks.reserve); callbacks.object["unreserve"] = string(config.callbacks.unreserve); callbacks.object["abort"] = string(config.callbacks.abort);
    value.object["callbacks"] = std::move(callbacks);
    auto presentation = object(); presentation.object["show_parameter_names"] = boolean(config.presentation.showParameterNames);
    presentation.object["header_title_visible"] = boolean(config.presentation.headerTitleVisible); presentation.object["compact"] = boolean(config.presentation.compact);
    presentation.object["requested_width"] = number(config.presentation.requestedWidth);
    auto order = array(); for (const auto &id : config.presentation.visualParameterOrder) order.array.push_back(string(id));
    presentation.object["visual_parameter_order"] = std::move(order); value.object["presentation"] = std::move(presentation);
    value.object["next_identity"] = integer(config.nextIdentity); value.object["variadic"] = boolean(config.variadic); return value;
}

std::optional<NativeCallConfiguration> nativeCallFromJson(const source::JsonValue &json, std::string *error,
    std::vector<NativeCallDiagnostic> *diagnostics) {
    const auto fail = [&](std::string_view field) -> std::optional<NativeCallConfiguration> {
        if (error) *error = "Malformed Native Call configuration field: " + std::string(field); return {};
    };
    NativeCallConfiguration config; std::uint64_t version = 0;
    if (json.type != Json::Type::Object || !readInteger(json, "schema_version", version) || version != 1) return fail("schema_version");
    const auto *library = json.member("library"), *returned = json.member("return"), *arguments = json.member("arguments"),
        *callbacks = json.member("callbacks"), *presentation = json.member("presentation");
    if (!library || library->type != Json::Type::Object || !readEnum(*library, "mode", config.library.mode) ||
        !readString(*library, "value", config.library.value) || !readBool(*library, "specify_path_on_diagram", config.library.specifyPathOnDiagram) ||
        !readEnum(*library, "dynamic_relative_base", config.library.dynamicRelativeBase)) return fail("library");
    if (!readString(json, "symbol", config.symbol) || !readEnum(json, "calling_convention", config.callingConvention) ||
        !readEnum(json, "thread", config.thread) || !readEnum(json, "error_checking", config.errorChecking) ||
        !readInteger(json, "next_identity", config.nextIdentity) || !readBool(json, "variadic", config.variadic)) return fail("function");
    if (!returned || returned->type != Json::Type::Object || !readString(*returned, "display_name", config.returnDefinition.displayName) ||
        !readNativeType(returned->member("type"), config.returnDefinition.type) || !readEnum(*returned, "ownership", config.returnDefinition.ownership) ||
        !readCapacity(returned->member("maximum_read"), config.returnDefinition.maximumRead) || !readString(*returned, "lifetime", config.returnDefinition.lifetime)) return fail("return");
    if (!arguments || arguments->type != Json::Type::Array) return fail("arguments");
    for (const auto &item : arguments->array) {
        NativeCallParameter parameter;
        if (!readString(item, "id", parameter.id) || !readString(item, "display_name", parameter.displayName) ||
            !readNativeType(item.member("type"), parameter.type) || !readEnum(item, "pass", parameter.pass) || !readEnum(item, "access", parameter.access) ||
            !readEnum(item, "input_requirement", parameter.inputRequirement) || !readEnum(item, "initialization", parameter.initialization) ||
            !readOptionalString(item, "default_canonical_value", parameter.defaultCanonicalValue) || !readBool(item, "nullable", parameter.nullable) ||
            !readCapacity(item.member("minimum_capacity"), parameter.minimumCapacity) || !readOptionalString(item, "valid_length_parameter_id", parameter.validLengthParameterId)) return fail("argument");
        const auto *dimensions = item.member("dimensions"); if (!dimensions || dimensions->type != Json::Type::Array) return fail("dimensions");
        for (const auto &dimension : dimensions->array) {
            NativeCallCapacity capacity; if (!readCapacity(&dimension, capacity)) return fail("dimension");
            parameter.dimensions.push_back(std::move(capacity));
        }
        config.arguments.push_back(std::move(parameter));
    }
    if (!callbacks || callbacks->type != Json::Type::Object || !readInteger(*callbacks, "profile_version", version) || version > UINT32_MAX ||
        !readString(*callbacks, "reserve", config.callbacks.reserve) || !readString(*callbacks, "unreserve", config.callbacks.unreserve) ||
        !readString(*callbacks, "abort", config.callbacks.abort)) return fail("callbacks");
    config.callbacks.profileVersion = static_cast<std::uint32_t>(version);
    if (presentation) {
        if (presentation->type != Json::Type::Object || !readBool(*presentation, "show_parameter_names", config.presentation.showParameterNames) ||
            !readBool(*presentation, "header_title_visible", config.presentation.headerTitleVisible) || !readBool(*presentation, "compact", config.presentation.compact) ||
            !readNumber(*presentation, "requested_width", config.presentation.requestedWidth)) return fail("presentation");
        if (const auto *order = presentation->member("visual_parameter_order")) {
            if (order->type != Json::Type::Array) return fail("visual_parameter_order");
            for (const auto &id : order->array) { if (id.type != Json::Type::String) return fail("visual_parameter_order"); config.presentation.visualParameterOrder.push_back(id.string); }
        }
    }
    const auto migrations = nativeCallMigrateVisual(config);
    if (diagnostics) {
        diagnostics->insert(diagnostics->end(), migrations.begin(), migrations.end());
        const auto validation = nativeCallValidate(config);
        diagnostics->insert(diagnostics->end(), validation.diagnostics.begin(), validation.diagnostics.end());
    }
    if (error) error->clear();
    return config;
}

std::string nativeCallSemanticKey(const NativeCallConfiguration &config) {
    auto json = nativeCallToJson(config); json.object.erase("presentation"); json.object.erase("next_identity");
    json.object["return"].object.erase("display_name");
    for (auto &parameter : json.object["arguments"].array) parameter.object.erase("display_name");
    const auto removeTypePresentation = [&](auto &&self, Json &type) -> void {
        type.object.erase("field_names"); type.object.erase("choice_kind");
        const bool enumeration = type.object["canonical"].string.starts_with("enum<");
        if (!enumeration) type.object.erase("choices");
        else for (auto &choice : type.object["choices"].array) {
            choice.object.erase("id"); choice.object.erase("disabled"); choice.object.erase("visible");
        }
        for (auto &child : type.object["children"].array) self(self, child);
    };
    const auto stripNativeType = [&](Json &type) {
        removeTypePresentation(removeTypePresentation, type.object["adapted_type"]);
        type.object["struct_layout"].object.erase("c_type_name");
    };
    stripNativeType(json.object["return"].object["type"]);
    for (auto &parameter : json.object["arguments"].array) stripNativeType(parameter.object["type"]);
    std::string result; appendJsonText(result, json); return result;
}

} // namespace frog::native_engine
