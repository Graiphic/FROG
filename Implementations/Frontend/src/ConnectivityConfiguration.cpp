#include "ConnectivityDataCodec.hpp"
#include "frog_native_engine/ConnectivityCatalog.hpp"
namespace frog::native_engine {
using Type=binding::TypeRef;using Json=source::JsonValue;
bool connectivityPythonTypeSupported(const Type &type) {
    if (!type.known()) return false;
    if (type.kind == binding::TypeKind::Array || type.kind == binding::TypeKind::Cluster)
        return std::all_of(type.children.begin(), type.children.end(), connectivityPythonTypeSupported);
    return type.kind == binding::TypeKind::Primitive && (type.numeric() || type.name == "bool" || type.name == "string");
}
std::vector<ConnectivityConfiguredPort> connectivityConfiguredPorts(
    const ConnectivityNodeConfiguration &authored, std::span<const ConnectivityTypeEvidence> evidence) {
    const auto &config=authored;
    std::vector<ConnectivityConfiguredPort> ports;
    const auto add = [&](std::string id, std::string name, bool output, Type type,
        std::string description, std::string reason = {}) {
        if (!type.known() && reason.empty()) reason = "A concrete type has not been established for this terminal.";
        ports.push_back({std::move(id), std::move(name), output, std::move(type), std::move(description), std::move(reason)});
    };
    const auto errors = [&] {
        add("error_in", "error in", false, errorType(), "Error conditions received before this node.");
        add("error_out", "error out", true, errorType(), "Error information produced by this node.");
    };
    auto reference = referenceType(config);
    if (config.referenceClassId.empty()) {
        if (const auto *wired = evidenceType(evidence, "reference"); wired && familyReference(*wired, config.family))
            reference = *wired;
    }
    const auto configured = config.metadataState != ConnectivityMetadataState::Unconfigured;
    switch (config.kind) {
    case ConnectivityNodeKind::Constructor:
        add("new_reference", "new reference", true,
            configured && !config.qualifiedTypeId.empty() && !config.referenceClassId.empty() ? reference : Type{},
            "Reference to the object of the selected constructor class.",
            configured ? std::string{} : "No class and constructor metadata have been selected.");
        errors(); break;
    case ConnectivityNodeKind::Property:
        add("reference", "reference", false, reference, "Object whose selected properties are accessed.");
        add("reference_out", "reference out", true, reference, "The same object reference after property access.");
        for (const auto &row : config.properties) {
            const auto writable = row.mode == ConnectivityPropertyMode::Write;
            const auto permitted = writable ? row.writable : row.readable;
            add(row.id + "_value", row.label.empty() ? "Property" : row.label, !writable,
                row.metadataAvailable && permitted ? row.valueType : Type{}, row.description,
                !row.metadataAvailable ? "Property metadata are unavailable; select a documented member." :
                !permitted ? "The selected property does not support this access mode." : "");
        }
        errors(); break;
    case ConnectivityNodeKind::Invoke:
        add("reference", "reference", false, reference, "Object or class owning the selected method.");
        add("reference_out", "reference out", true, reference, "The same reference after method invocation.");
        if (config.hasReturnValue) add("return_value", "return value", true, config.returnType,
            "Result of the selected method overload.");
        errors(); break;
    case ConnectivityNodeKind::AutomationOpen:
    {
        auto target = selectedType(config.targetType, evidence, "automation_refnum");
        if (!familyReference(target, ConnectivityReferenceFamily::ActiveX)) target = {};
        add("automation_refnum", "Automation refnum", false,
            target, "Class specifier for the ActiveX object to create.");
        add("machine_name", "machine name", false, Type::primitive("string"), "Computer hosting the automation server; empty selects the local computer.");
        add("open_new_instance", "open new instance", false, Type::primitive("bool"), "Whether to create a new instance of the selected automation server.");
        add("automation_refnum_out", "Automation refnum out", true,
            target, "Reference with the type supplied to Automation refnum.");
        errors(); break;
    }
    case ConnectivityNodeKind::Callback: {
        const auto callback = Type::reference("connectivity." + std::string(familyToken(config.family)) + ".event_callback");
        add("event_callback_ref", "event callback ref", false, callback, "Existing callback registration to update.");
        for (const auto &group : config.callbacks) {
            const auto userParameter = selectedType(group.userParameterType, evidence, group.id + "_user_parameter");
            const auto incompatibleUserParameter = group.metadataAvailable && userParameter.known() &&
                (!group.userParameterType.known() || userParameter.canonical() != group.userParameterType.canonical());
            add(group.id + "_event_source_ref", "event source ref", false,
                group.sourceType.known() ? group.sourceType : reference, "Local object that generates the selected event.");
            add(group.id + "_vi_reference", "vi reference", false,
                group.metadataAvailable && !incompatibleUserParameter ? group.callbackType : Type{},
                "Strict callback reference matching the selected event signature.",
                incompatibleUserParameter ? "The user-data type differs from the inspected callback connector; matching callback metadata are required." : "");
            add(group.id + "_user_parameter", "user parameter", false,
                userParameter, "Authored data passed to the callback alongside the event.");
        }
        add("event_callback_ref_out", "event callback ref out", true, callback, "New or updated registration reference.");
        errors(); break;
    }
    case ConnectivityNodeKind::Python: {
        const auto session = Type::reference("connectivity.python.session");
        add("session_in", "session in", false, session, "Python session shared by one or more Python Nodes.");
        add("module_path", "module path", false, Type::primitive("path"), "Path of the module containing the selected function.");
        add("function_name", "function name", false, Type::primitive("string"), "Name of the Python function within the module.");
        add("error_in", "error in", false, errorType(), "Error conditions received before this Python call.");
        auto returnType = selectedType(config.returnType, evidence, "return_type");
        if (!connectivityPythonTypeSupported(returnType)) returnType = {};
        add("return_type", "return type", false, returnType, "Type specimen defining the expected Python return value; leave unwired for no return value.");
        add("session_out", "session out", true, session, "The Python session used by this node.");
        add("error_out", "error out", true, errorType(), "Error information produced by this Python call.");
        add("return_value", "return value", true, returnType, "Result with the exact complete type supplied to return type.");
        break;
    }
    case ConnectivityNodeKind::TypeConversion:
        if (config.catalogId.ends_with(".to_more_generic_class") || config.catalogId.ends_with(".to_more_specific_class")) {
            auto target = selectedType(config.targetType, evidence, "target_class");
            if (!familyReference(target, config.family)) target = {};
            add("target_class", "target class", false, target, "Class specifier defining the converted reference type.");
            auto input = reference;
            if (!familyReference(input, config.family)) input = {};
            add("reference", "reference", false, input, "Reference to cast within the same documented class hierarchy.");
            const auto specific = config.catalogId.ends_with(".to_more_specific_class");
            // A shared reference family does not prove inheritance. This
            // authoring profile currently has no inspected class-relationship
            // provider; only an identity conversion can be established here.
            const auto established = input.known() && target.known() &&
                input.canonical()==target.canonical();
            add(specific ? "specific_class_reference" : "generic_class_reference",
                specific ? "specific class reference" : "generic class reference", true, established ? target : Type{},
                "Reference of the exact target class; class relationship metadata must establish compatibility.",
                target.known() && input.known() && !established ?
                    "Source and target classes differ; inspected class-relationship metadata are unavailable." : "");
            if (specific) errors();
        } else if (config.catalogId.ends_with(".to_variant")) {
            add("anything", "anything", false, selectedType(config.targetType, evidence, "anything"), "Typed data to encapsulate as a variant.");
            add("variant", "variant", true, Type::primitive("variant"), "Variant carrying the original typed data.");
        } else {
            const auto target = selectedType(config.targetType, evidence, "type");
            add("type", "type", false, target, "Type specimen defining the desired data schema.");
            add("variant", "variant", false, Type::primitive("variant"), "Variant data to convert to the specified type.");
            add("data", "data", true, target, "Converted data with the exact type supplied to type.");
            errors();
        }
        break;
    case ConnectivityNodeKind::StaticReference: {
        Type output;
        if (!config.strictReference) output = Type::reference("connectivity.document.reference");
        else if (!config.documentIdentity.empty()) {
            std::vector<std::string> ids; std::vector<Type> fields;
            for (const auto &port : config.documentPorts) {
                ids.push_back((port.output ? "out_" : "in_") + port.id); fields.push_back(port.valueType);
            }
            output = Type::reference("connectivity.document.strict", Type::cluster(std::move(ids), std::move(fields)));
        }
        add("vi_reference", "VI reference", true, output,
            "Static reference to the selected .frog document, preserving its strict connector schema when requested.");
        break;
    }
    case ConnectivityNodeKind::Polymorphic: break;
    }
    if (config.kind == ConnectivityNodeKind::Constructor || config.kind == ConnectivityNodeKind::Invoke ||
        config.kind == ConnectivityNodeKind::Python) {
        for (const auto &parameter : config.parameters) {
            auto type = config.kind == ConnectivityNodeKind::Python ? selectedType(parameter.valueType, evidence, parameter.id) : parameter.valueType;
            if (config.kind == ConnectivityNodeKind::Python) {
                if (!connectivityPythonTypeSupported(type)) type = {};
            } else if (!parameter.metadataAvailable) type = {};
            const auto name = parameter.label.empty() ? "input parameter" : parameter.label;
            if (parameter.direction == ConnectivityParameterDirection::InOut) {
                add(parameter.id + "_in", name, false, type, parameter.description);
                add(parameter.id + "_out", name, true, type, parameter.description);
            } else add(parameter.id, name, parameter.direction == ConnectivityParameterDirection::Output,
                type, parameter.description);
        }
    }
    return ports;
}
std::string connectivityAppendParameter(ConnectivityNodeConfiguration &config) {
    if (config.kind != ConnectivityNodeKind::Python) return {}; // method rows must come from actual member metadata
    const auto id = identity(config, "parameter_"); if (id.empty()) return {};
    ConnectivityParameter item; item.id = id; item.label = "input parameter " + std::to_string(config.parameters.size() + 1);
    item.description = "Argument passed at this position to the configured Python function.";
    config.parameters.push_back(std::move(item)); return id;
}
std::string connectivityAppendProperty(ConnectivityNodeConfiguration &config) {
    if (config.kind != ConnectivityNodeKind::Property) return {};
    const auto id = identity(config, "property_"); if (id.empty()) return {};
    ConnectivityPropertyRow row; row.id = id; config.properties.push_back(std::move(row)); return id;
}
std::string connectivityAppendCallback(ConnectivityNodeConfiguration &config) {
    if (config.kind != ConnectivityNodeKind::Callback) return {};
    const auto id = identity(config, "callback_"); if (id.empty()) return {};
    ConnectivityCallbackGroup group; group.id = id; config.callbacks.push_back(std::move(group)); return id;
}
Json serializeConnectivityConfiguration(const ConnectivityNodeConfiguration &config) {
    auto value = object(); value.object["profile"] = string("frog.studio.connectivity@1");
    value.object["catalog_id"] = string(config.catalogId); value.object["family"] = string(familyToken(config.family));
    value.object["kind"] = string(kindToken(config.kind)); value.object["metadata_state"] = string(stateToken(config.metadataState));
    value.object["metadata_source"] = string(config.metadataSource); value.object["unresolved_reason"] = string(config.unresolvedReason);
    value.object["assembly_identity"] = string(config.assemblyIdentity); value.object["qualified_type_id"] = string(config.qualifiedTypeId);
    value.object["reference_class_id"] = string(config.referenceClassId); value.object["member_id"] = string(config.memberId);
    value.object["member_label"] = string(config.memberLabel); value.object["return_type"] = typeJson(config.returnType);
    value.object["target_type"] = typeJson(config.targetType); value.object["has_return_value"] = boolean(config.hasReturnValue);
    value.object["instance_id"] = string(config.instanceId); value.object["module_path"] = string(config.modulePath);
    value.object["function_name"] = string(config.functionName); value.object["document_path"] = string(config.documentPath);
    value.object["document_identity"] = string(config.documentIdentity); value.object["strict_reference"] = boolean(config.strictReference);
    value.object["next_identity"] = integer(config.nextIdentity);
    auto parameters = array();
    for (const auto &parameter : config.parameters) {
        auto item = object(); item.object["id"] = string(parameter.id); item.object["member_parameter_id"] = string(parameter.memberParameterId);
        item.object["label"] = string(parameter.label); item.object["direction"] = string(directionToken(parameter.direction));
        item.object["value_type"] = typeJson(parameter.valueType); item.object["metadata_available"] = boolean(parameter.metadataAvailable);
        item.object["description"] = string(parameter.description); parameters.array.push_back(std::move(item));
    }
    value.object["parameters"] = std::move(parameters); auto properties = array();
    for (const auto &row : config.properties) {
        auto item = object(); item.object["id"] = string(row.id); item.object["member_id"] = string(row.memberId);
        item.object["label"] = string(row.label); item.object["mode"] = string(row.mode == ConnectivityPropertyMode::Read ? "read" : "write");
        item.object["value_type"] = typeJson(row.valueType); item.object["readable"] = boolean(row.readable);
        item.object["writable"] = boolean(row.writable); item.object["metadata_available"] = boolean(row.metadataAvailable);
        item.object["description"] = string(row.description); properties.array.push_back(std::move(item));
    }
    value.object["properties"] = std::move(properties); auto callbacks = array();
    for (const auto &group : config.callbacks) {
        auto item = object(); item.object["id"] = string(group.id); item.object["event_id"] = string(group.eventId);
        item.object["label"] = string(group.label); item.object["source_type"] = typeJson(group.sourceType);
        item.object["callback_type"] = typeJson(group.callbackType); item.object["user_parameter_type"] = typeJson(group.userParameterType);
        item.object["metadata_available"] = boolean(group.metadataAvailable); callbacks.array.push_back(std::move(item));
    }
    value.object["callbacks"] = std::move(callbacks); auto documentPorts = array();
    for (const auto &port : config.documentPorts) {
        auto item = object(); item.object["id"] = string(port.id); item.object["label"] = string(port.label);
        item.object["output"] = boolean(port.output); item.object["value_type"] = typeJson(port.valueType);
        documentPorts.array.push_back(std::move(item));
    }
    value.object["document_ports"] = std::move(documentPorts); return value;
}
std::optional<ConnectivityNodeConfiguration> deserializeConnectivityConfiguration(const Json &value, std::string *error) {
    const auto fail = [error](std::string message) -> std::optional<ConnectivityNodeConfiguration> {
        if (error) *error = "Connectivity configuration: " + std::move(message); return {};
    };
    std::string profile, catalog, family, kind, state;
    if (value.type != Json::Type::Object || !readString(value, "profile", profile) || profile != "frog.studio.connectivity@1" ||
        !readString(value, "catalog_id", catalog)) return fail("unsupported private profile or missing catalog identity.");
    auto initial = savedConnectivityConfigurationForCatalog(catalog); if (!initial) return fail("unknown configurable catalog entry.");
    ConnectivityNodeConfiguration config; config.catalogId = catalog; config.family = initial->family; config.kind = initial->kind;
    if (!readString(value, "family", family) || family != familyToken(config.family) ||
        !readString(value, "kind", kind) || kind != kindToken(config.kind) || !readString(value, "metadata_state", state))
        return fail("family or operation does not match the catalog identity.");
    if (state == "unconfigured") config.metadataState = ConnectivityMetadataState::Unconfigured;
    else if (state == "available") config.metadataState = ConnectivityMetadataState::Available;
    else if (state == "unavailable") config.metadataState = ConnectivityMetadataState::Unavailable;
    else return fail("unknown metadata state.");
    if (!readString(value, "metadata_source", config.metadataSource) || !readString(value, "unresolved_reason", config.unresolvedReason) ||
        !readString(value, "assembly_identity", config.assemblyIdentity) || !readString(value, "qualified_type_id", config.qualifiedTypeId) ||
        !readString(value, "reference_class_id", config.referenceClassId) || !readString(value, "member_id", config.memberId) ||
        !readString(value, "member_label", config.memberLabel) || !readString(value, "instance_id", config.instanceId) ||
        !readString(value, "module_path", config.modulePath) || !readString(value, "function_name", config.functionName) ||
        !readString(value, "document_path", config.documentPath) || !readString(value, "document_identity", config.documentIdentity) ||
        !readBool(value, "strict_reference", config.strictReference) || !readBool(value, "has_return_value", config.hasReturnValue) ||
        !readInteger(value, "next_identity", config.nextIdentity) || config.nextIdentity == 0 ||
        !readType(value.member("return_type"), config.returnType) || !readType(value.member("target_type"), config.targetType))
        return fail("invalid saved identity, type schema or counter.");
    if (!config.referenceClassId.empty() && !familyReference(Type::reference(config.referenceClassId), config.family))
        return fail("reference class belongs to another family or has an invalid identity.");
    const auto *parameters = value.member("parameters"), *properties = value.member("properties"),
        *callbacks = value.member("callbacks"), *documents = value.member("document_ports");
    if (!parameters || parameters->type != Json::Type::Array || !properties || properties->type != Json::Type::Array ||
        !callbacks || callbacks->type != Json::Type::Array || !documents || documents->type != Json::Type::Array)
        return fail("row collections must be arrays.");
    std::unordered_set<std::string> ids;
    const auto validIdentity = [&](std::string_view id) {
        if (!safeId(id) || !ids.insert(std::string(id)).second) return false;
        const auto separator = id.rfind('_');
        if (separator != std::string_view::npos) {
            std::uint64_t sequence = 0; const auto suffix = id.substr(separator + 1);
            const auto parsed = std::from_chars(suffix.data(), suffix.data() + suffix.size(), sequence);
            if (parsed.ec == std::errc{} && parsed.ptr == suffix.data() + suffix.size() && sequence >= config.nextIdentity) return false;
        }
        return true;
    };
    for (const auto &item : parameters->array) {
        ConnectivityParameter row; std::string direction;
        if (!readString(item, "id", row.id) || !validIdentity(row.id) || !readString(item, "member_parameter_id", row.memberParameterId) ||
            !readString(item, "label", row.label) || !readString(item, "direction", direction) ||
            !readType(item.member("value_type"), row.valueType) || !readBool(item, "metadata_available", row.metadataAvailable) ||
            !readString(item, "description", row.description)) return fail("invalid parameter row or repeated identity.");
        if (direction == "input") row.direction = ConnectivityParameterDirection::Input;
        else if (direction == "output") row.direction = ConnectivityParameterDirection::Output;
        else if (direction == "inout") row.direction = ConnectivityParameterDirection::InOut;
        else return fail("unknown parameter direction.");
        if (config.kind == ConnectivityNodeKind::Python && (row.direction != ConnectivityParameterDirection::Input ||
            (row.valueType.known() && !connectivityPythonTypeSupported(row.valueType)))) return fail("unsupported Python argument contract.");
        config.parameters.push_back(std::move(row));
    }
    for (const auto &item : properties->array) {
        ConnectivityPropertyRow row; std::string mode;
        if (!readString(item, "id", row.id) || !validIdentity(row.id) || !readString(item, "member_id", row.memberId) ||
            !readString(item, "label", row.label) || !readString(item, "mode", mode) ||
            !readType(item.member("value_type"), row.valueType) || !readBool(item, "readable", row.readable) ||
            !readBool(item, "writable", row.writable) || !readBool(item, "metadata_available", row.metadataAvailable) ||
            !readString(item, "description", row.description)) return fail("invalid property row or repeated identity.");
        if (mode == "read") row.mode = ConnectivityPropertyMode::Read;
        else if (mode == "write") row.mode = ConnectivityPropertyMode::Write;
        else return fail("unknown property mode.");
        config.properties.push_back(std::move(row));
    }
    for (const auto &item : callbacks->array) {
        ConnectivityCallbackGroup row;
        if (!readString(item, "id", row.id) || !validIdentity(row.id) || !readString(item, "event_id", row.eventId) ||
            !readString(item, "label", row.label) || !readType(item.member("source_type"), row.sourceType) ||
            !readType(item.member("callback_type"), row.callbackType) || !readType(item.member("user_parameter_type"), row.userParameterType) ||
            !readBool(item, "metadata_available", row.metadataAvailable)) return fail("invalid callback group or repeated identity.");
        config.callbacks.push_back(std::move(row));
    }
    ids.clear();
    for (const auto &item : documents->array) {
        ConnectivityDocumentPort port;
        if (!readString(item, "id", port.id) || port.id.empty() || !ids.insert(port.id).second ||
            !readString(item, "label", port.label) || !readBool(item, "output", port.output) ||
            !readType(item.member("value_type"), port.valueType)) return fail("invalid static document connector.");
        config.documentPorts.push_back(std::move(port));
    }
    if ((!config.properties.empty() && config.kind != ConnectivityNodeKind::Property) ||
        (!config.callbacks.empty() && config.kind != ConnectivityNodeKind::Callback) ||
        (!config.parameters.empty() && config.kind != ConnectivityNodeKind::Constructor && config.kind != ConnectivityNodeKind::Invoke && config.kind != ConnectivityNodeKind::Python) ||
        (!config.documentPorts.empty() && config.kind != ConnectivityNodeKind::StaticReference)) return fail("rows do not match the operation kind.");
    if (error) error->clear(); return config;
}
}
