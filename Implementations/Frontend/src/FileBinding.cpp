#include "frog_native_engine/FileBinding.hpp"
#include "frog_native_engine/FunctionBindingCatalog.hpp"

#include <algorithm>
#include <array>

namespace frog::native_engine {
namespace {
using binding::TypeRef;
using binding::TypeKind;
using binding::ResolutionStatus;
std::string_view identity(std::string_view id) {
    if (id.starts_with("frog.studio.")) id.remove_prefix(12);
    return id;
}
FileTypeResult rejected(std::string code, std::string message) {
    return {{ResolutionStatus::Incompatible, {}, std::move(code), std::move(message)}, false};
}
bool textType(const TypeRef &type) {
    return binding::constrain(type,binding::parseTypeExpr("T:text").expression).accepted();
}
bool spreadsheetType(const TypeRef &type) {
    return binding::constrain(type,binding::parseTypeExpr("T:spreadsheet").expression).accepted();
}
bool binaryType(const TypeRef &type) {
    return binding::constrain(type,binding::parseTypeExpr("T:binary_array").expression).accepted();
}
bool xmlType(const TypeRef &type, unsigned depth = 0) {
    if (depth > 128 || !type.known()) return false;
    if (type.kind == TypeKind::Array || type.kind == TypeKind::Cluster || type.kind == TypeKind::Enum)
        return std::all_of(type.children.begin(), type.children.end(),
            [&](const auto &child) { return xmlType(child, depth + 1); });
    return type.kind == TypeKind::Primitive &&
        (type.numeric() || type.name == "bool" || type.name == "string" ||
         type.name == "path" || type.name == "timestamp");
}
}

bool isFileFunction(std::string_view id) noexcept {
    id = identity(id);
    const auto *entry = findFunctionCatalogEntry(id);
    return entry && entry->family == FunctionFamily::File && isFunctionCatalogPlaceable(*entry);
}

std::optional<std::string> fileOptionalInputDefault(const DiagramNode &node, std::string_view port,
    std::span<const std::string> connectedInputPorts) {
    const auto id = identity(node.type);
    if (id == "file.set_position" && port == "origin") return
        std::find(connectedInputPorts.begin(), connectedInputPorts.end(), "offset") == connectedInputPorts.end() ? "2" : "0";
    if (id == "file.text_read" && port == "count" && node.fileReadLines) return "1";
    if ((id == "file.spreadsheet_write" || id == "file.spreadsheet_read") && port == "format") {
        if (node.fileDataType == "i64") return "%d";
        if (node.fileDataType == "string") return "%s";
    }
    struct Default { std::string_view function; std::string_view port; std::string_view value; };
    static constexpr Default defaults[]{
#define FILE_DEFAULT(function, port, value) {function, port, value},
#include "../manifests/file/defaults.inc"
#undef FILE_DEFAULT
    };
    for (const auto &value : defaults)
        if (value.function == id && value.port == port) return std::string(value.value);
    return {};
}

FileTypeResult resolveFilePort(const DiagramNode &node, std::string_view requested,
    std::span<const FileTypeEvidence> evidence) {
    const auto id = identity(node.type);
    const auto *definition = findFunctionBindingDefinition(id);
    if (!isFileFunction(id) || !definition)
        return rejected("binding.file_unknown", "The File operation is not registered.");
    const auto *port = findFunctionBindingPort(*definition, requested);
    if (!port) return rejected("binding.file_port", "The File terminal does not exist.");
    const auto supplied = [&](std::string_view name) -> const TypeRef * {
        const auto found = std::find_if(evidence.begin(), evidence.end(),
            [&](const auto &item) { return item.portId == name; });
        return found == evidence.end() ? nullptr : &found->valueType;
    };
    bool complete = true;
    for (const auto &input : definition->ports) {
        if (input.direction != FunctionPortDirection::Input) continue;
        const auto *type = supplied(input.portId);
        if (!type) {
            complete = complete && input.requirement != FunctionPortRequirement::Required;
            continue;
        }
        if (!type->known()) return {{ResolutionStatus::Unresolved, {}, "binding.file_unknown_type",
            "A connected File input has no established type."}, false};
        bool accepts = false;
        if (input.typeExpression == "T:path_or_file")
            accepts = *type == TypeRef::primitive("path") || *type == TypeRef::reference("file");
        else if (input.typeExpression == "T:text") accepts = textType(*type);
        else if (input.typeExpression == "T:spreadsheet") accepts = spreadsheetType(*type) &&
            (id != "file.spreadsheet_write" || type->children.front() == TypeRef::primitive(node.fileDataType));
        else if (input.typeExpression == "T:binary_array") accepts = binaryType(*type);
        else if (input.typeExpression == "T:any") accepts = xmlType(*type);
        else {
            const auto expected = binding::parseType(input.typeExpression).type;
            accepts = binding::connect(*type, expected,
                expected.numeric() ? binding::ConnectionPolicy::NumericImplicit : binding::ConnectionPolicy::Exact).accepted();
        }
        if (!accepts) return rejected("binding.file_input_type", "Node " + node.id +
            ", terminal " + input.portId + ": type " + type->canonical() +
            " is incompatible with " + input.typeExpression + ".");
    }
    TypeRef resolved = binding::parseType(port->defaultValueType).type;
    if (port->typeExpression == "T:path_or_file") {
        if (const auto *type = supplied(port->portId)) resolved = *type;
    } else if (port->typeExpression == "T:any") {
        const auto *type = supplied(id == "file.unflatten_from_xml" ? "type" : "data");
        if (!type) return {{ResolutionStatus::Unresolved, {}, "binding.file_type_specimen",
            "XML conversion requires the complete connected data/type specimen."}, false};
        resolved = *type;
    } else if (port->typeExpression == "T:binary_array") {
        if (const auto *type = supplied("data")) resolved = *type;
        else complete = false;
    } else if (port->typeExpression == "T:spreadsheet" || port->typeExpression == "T:spreadsheet_row") {
        if (id == "file.spreadsheet_write") {
            if (const auto *type = supplied("data")) resolved = *type;
            else complete = false;
        } else {
            if (node.fileDataType != "f64" && node.fileDataType != "i64" && node.fileDataType != "string")
                return rejected("binding.file_spreadsheet_variant", "The selected spreadsheet variant is not supported.");
            resolved = TypeRef::array(TypeRef::primitive(node.fileDataType),
                port->portId == "first_row" ? 1 : 2);
        }
    } else if (port->typeExpression == "T:text") {
        if (id == "file.text_read") {
            resolved = node.fileReadLines && supplied("count") ?
                TypeRef::array(TypeRef::primitive("string")) : TypeRef::primitive("string");
        } else if (id == "file.read_xml") {
            resolved = node.fileXmlArray ? TypeRef::array(TypeRef::primitive("string")) :
                TypeRef::primitive("string");
        } else if (const auto *type = supplied(port->portId)) resolved = *type;
    }
    return {{ResolutionStatus::Compatible, std::move(resolved), {}, {}}, complete};
}
} // namespace frog::native_engine
