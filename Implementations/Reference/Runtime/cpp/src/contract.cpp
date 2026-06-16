#include "contract.hpp"

#include <cstdlib>
#include <set>
#include <stdexcept>

#ifndef FROG_RUNTIME_CPP_SOURCE_DIR
#define FROG_RUNTIME_CPP_SOURCE_DIR "."
#endif

namespace frog::runtime {

namespace {

using frog::json::Array;
using frog::json::Object;
using frog::json::Value;

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

const Object& as_object(const Value& value, const std::string& message) {
    require(value.is_object(), message);
    return value.as_object();
}

const Array& as_array(const Value& value, const std::string& message) {
    require(value.is_array(), message);
    return value.as_array();
}

std::string get_string(const Object& object, const std::string& key) {
    const auto it = object.find(key);
    require(it != object.end() && it->second.is_string(), "Expected string field: " + key);
    return it->second.as_string();
}

const Value* optional_field(const Object& object, const std::string& key) {
    const auto it = object.find(key);
    if (it == object.end()) {
        return nullptr;
    }
    return &it->second;
}

std::optional<std::string> get_optional_string(const Object& object, const std::string& key) {
    const auto it = object.find(key);
    if (it == object.end() || it->second.is_null()) {
        return std::nullopt;
    }
    require(it->second.is_string(), "Expected string field: " + key);
    return it->second.as_string();
}

std::int64_t get_i64(const Object& object, const std::string& key) {
    const auto it = object.find(key);
    require(it != object.end() && it->second.is_number(), "Expected integer field: " + key);
    return it->second.as_i64();
}

bool get_bool(const Object& object, const std::string& key) {
    const auto it = object.find(key);
    require(it != object.end() && it->second.is_bool(), "Expected boolean field: " + key);
    return it->second.as_bool();
}

std::vector<std::string> parse_string_vector(const Value& value, const std::string& label) {
    std::vector<std::string> result;
    for (const auto& item : as_array(value, label)) {
        require(item.is_string(), "Expected string item in " + label);
        result.push_back(item.as_string());
    }
    return result;
}


UiBindingAssumptions parse_ui_binding_assumptions(const Object& object) {
    UiBindingAssumptions ui_binding;
    if (const auto* value = optional_field(object, "widget_value_binding")) {
        ui_binding.widget_value_binding = get_bool(object, "widget_value_binding");
    }
    if (const auto* value = optional_field(object, "widget_reference_binding")) {
        ui_binding.widget_reference_binding = get_bool(object, "widget_reference_binding");
    }
    return ui_binding;
}

RuntimeFamilyAssumptions parse_runtime_family_assumptions(const Value& value) {
    const auto& object = as_object(value, "Expected runtime_family assumptions object.");
    RuntimeFamilyAssumptions runtime_family;
    if (const auto* name = optional_field(object, "name")) {
        runtime_family.name = get_string(object, "name");
    }
    if (const auto* host_model = optional_field(object, "host_model")) {
        runtime_family.host_model = get_string(object, "host_model");
    }
    if (const auto* ui_binding = optional_field(object, "ui_binding"); ui_binding != nullptr && ui_binding->is_object()) {
        runtime_family.ui_binding = parse_ui_binding_assumptions(ui_binding->as_object());
    }
    return runtime_family;
}

NumericBehaviorAssumptions parse_numeric_behavior_assumptions(const Value& value) {
    const auto& object = as_object(value, "Expected numeric_behavior assumptions object.");
    NumericBehaviorAssumptions numeric_behavior;
    if (const auto* value_domain = optional_field(object, "value_domain")) {
        numeric_behavior.value_domain = get_string(object, "value_domain");
    }
    if (const auto* overflow_behavior = optional_field(object, "overflow_behavior")) {
        numeric_behavior.overflow_behavior = get_string(object, "overflow_behavior");
    }
    return numeric_behavior;
}

ContractAssumptions parse_contract_assumptions(const Value& value) {
    const auto& object = as_object(value, "Expected assumptions object.");
    ContractAssumptions assumptions;
    if (const auto* runtime_family = optional_field(object, "runtime_family")) {
        assumptions.runtime_family = parse_runtime_family_assumptions(*runtime_family);
    }
    if (const auto* numeric_behavior = optional_field(object, "numeric_behavior")) {
        assumptions.numeric_behavior = parse_numeric_behavior_assumptions(*numeric_behavior);
    }
    return assumptions;
}

InterfacePort parse_interface_port(const Value& value) {
    const auto& object = as_object(value, "Expected interface port object.");
    return InterfacePort{
        get_string(object, "id"),
        get_string(object, "type"),
        get_optional_string(object, "binding_origin"),
        get_optional_string(object, "binding_target"),
    };
}

WidgetBindingMode parse_widget_binding_mode(const Value& value) {
    const auto& object = as_object(value, "Expected widget binding mode object.");
    return WidgetBindingMode{
        get_string(object, "mode"),
        get_optional_string(object, "public_input_id"),
        get_optional_string(object, "public_output_id"),
    };
}

WidgetBinding parse_widget_binding(const Value& value) {
    const auto& object = as_object(value, "Expected widget binding object.");
    return WidgetBinding{
        get_string(object, "widget_id"),
        get_string(object, "widget_class"),
        get_string(object, "value_type"),
        get_string(object, "role"),
        parse_widget_binding_mode(object.at("binding")),
    };
}

WidgetReferenceSupport parse_widget_reference_support(const Value& value) {
    const auto& object = as_object(value, "Expected widget reference support object.");
    return WidgetReferenceSupport{
        get_string(object, "widget_id"),
        parse_string_vector(object.at("supported_members"), "supported_members"),
    };
}

PropertyWrite parse_property_write(const Value& value) {
    const auto& object = as_object(value, "Expected property write object.");
    const auto& value_object = as_object(object.at("value"), "Expected property write value object.");
    const auto operation = get_optional_string(object, "operation");
    return PropertyWrite{
        operation.has_value() ? *operation : get_string(object, "op"),
        get_string(object, "widget_id"),
        get_string(object, "member"),
        PropertyWriteValue{get_string(value_object, "type"), get_string(value_object, "value")},
    };
}

WidgetClass parse_widget_class(const Value& value) {
    const auto& object = as_object(value, "Expected widget class object.");
    WidgetClass result;
    result.class_id = get_string(object, "class_id");
    if (const auto* properties = optional_field(object, "properties")) {
        for (const auto& item : as_array(*properties, "widget_class.properties")) {
            const auto& entry = as_object(item, "Expected widget property object.");
            result.properties.push_back(WidgetProperty{get_string(entry, "name")});
        }
    }
    if (const auto* methods = optional_field(object, "methods")) {
        for (const auto& item : as_array(*methods, "widget_class.methods")) {
            const auto& entry = as_object(item, "Expected widget method object.");
            result.methods.push_back(WidgetMethod{get_string(entry, "name")});
        }
    }
    return result;
}

SvgAsset parse_svg_asset(const Value& value) {
    const auto& object = as_object(value, "Expected svg asset object.");
    const auto kind = get_optional_string(object, "kind").value_or(get_optional_string(object, "target_class").value_or(""));
    return SvgAsset{get_string(object, "asset_id"), get_string(object, "path"), kind};
}

HostBinding parse_host_binding(const Value& value) {
    const auto& object = as_object(value, "Expected host binding object.");
    HostBinding binding;
    binding.binding_id = get_string(object, "binding_id");
    binding.target = get_optional_string(object, "target").value_or(get_optional_string(object, "host_family").value_or(""));
    if (const auto* required = optional_field(object, "required_capabilities")) {
        binding.required_capabilities = parse_string_vector(*required, "required_capabilities");
    }
    if (const auto* optional = optional_field(object, "optional_capabilities")) {
        binding.optional_capabilities = parse_string_vector(*optional, "optional_capabilities");
    }
    return binding;
}

PanelWidget parse_panel_widget(const Value& value) {
    const auto& object = as_object(value, "Expected panel widget object.");
    PanelWidget widget;
    if (const auto instance_id = get_optional_string(object, "instance_id")) {
        widget.instance_id = *instance_id;
    } else if (const auto instance_ref = get_optional_string(object, "instance_ref")) {
        widget.instance_id = *instance_ref;
    } else {
        widget.instance_id = get_string(object, "id");
    }
    widget.class_ref = get_string(object, "class_ref");
    widget.layout = object.at("layout");
    if (const auto* props = optional_field(object, "props")) {
        widget.props = as_object(*props, "Expected panel widget props object.");
    }
    if (const auto* visual = optional_field(object, "visual")) {
        widget.visual = as_object(*visual, "Expected panel widget visual object.");
    }
    return widget;
}

FrontPanel parse_front_panel(const Value& value) {
    const auto& object = as_object(value, "Expected front panel object.");
    FrontPanel panel;
    panel.panel_id = get_optional_string(object, "panel_id").value_or("main_panel");
    panel.title = get_optional_string(object, "title").value_or("FROG Front Panel");
    panel.class_ref = get_optional_string(object, "class_ref").value_or("frog.widgets.panel");
    if (const auto* layout = optional_field(object, "layout")) {
        panel.layout = *layout;
    } else {
        panel.layout = object.at("canvas");
    }
    panel.host_binding_ref = get_optional_string(object, "host_binding_ref").value_or("reference_host_default");
    for (const auto& item : as_array(object.at("widgets"), "front_panel.widgets")) {
        panel.widgets.push_back(parse_panel_widget(item));
    }
    return panel;
}

} // namespace

std::filesystem::path find_repo_root(const std::filesystem::path& start) {
    if (const char* spec_root = std::getenv("FROG_SPEC_ROOT"); spec_root != nullptr && spec_root[0] != '\0') {
        auto configured = std::filesystem::absolute(std::filesystem::path(spec_root));
        if (std::filesystem::is_directory(configured / "Examples") && std::filesystem::is_directory(configured / "Libraries")) {
            return configured;
        }
        throw std::runtime_error("FROG_SPEC_ROOT must point to a FROG specification repository with Examples and Libraries directories.");
    }
    auto current = std::filesystem::absolute(start);
    for (auto candidate = current; !candidate.empty();) {
        if (std::filesystem::is_directory(candidate / "Examples") && std::filesystem::is_directory(candidate / "Implementations")) {
            return candidate;
        }
        const auto sibling_spec_repo = candidate / "FROG";
        if (std::filesystem::is_directory(sibling_spec_repo / "Examples") &&
            std::filesystem::is_directory(sibling_spec_repo / "Implementations")) {
            return sibling_spec_repo;
        }
        const auto parent = candidate.parent_path();
        if (candidate == candidate.root_path() || parent == candidate) {
            break;
        }
        candidate = parent;
    }
    throw std::runtime_error("Unable to locate the repository root from the current path.");
}

std::filesystem::path default_contract_path() {
    const auto repo_root = find_repo_root(std::filesystem::path(FROG_RUNTIME_CPP_SOURCE_DIR));
    return repo_root / "Implementations" / "Reference" / "ContractEmitter" / "examples" /
           "05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json";
}

std::filesystem::path default_wfrog_path() {
    const auto repo_root = find_repo_root(std::filesystem::path(FROG_RUNTIME_CPP_SOURCE_DIR));
    return repo_root / "Examples" / "05_bounded_ui_accumulator" / "ui" / "accumulator_panel.wfrog";
}

BackendContract load_contract_from_path(const std::filesystem::path& path) {
    const auto root = as_object(frog::json::parse_file(path), "Expected contract JSON object.");
    BackendContract contract;
    contract.artifact_kind = get_string(root, "artifact_kind");
    require(contract.artifact_kind == "frog_backend_contract", "Expected frog_backend_contract.");
    if (const auto* governance = optional_field(root, "artifact_governance_ref"); governance != nullptr && governance->is_object()) {
        const auto& governance_object = governance->as_object();
        contract.artifact_governance_ref = ArtifactReference{get_string(governance_object, "path")};
    }
    contract.backend_family = get_string(root, "backend_family");
    require(contract.backend_family == REFERENCE_BACKEND_FAMILY, "Unsupported backend family.");
    {
        const auto& source = as_object(root.at("source_ref"), "Expected source_ref object.");
        contract.source_ref = SourceRef{get_string(source, "example_id"), get_string(source, "path"), get_string(source, "entry_unit")};
    }
    if (const auto* assumptions = optional_field(root, "assumptions"); assumptions != nullptr && assumptions->is_object()) {
        contract.assumptions = parse_contract_assumptions(*assumptions);
    }
    for (const auto& unit_value : as_array(root.at("units"), "contract.units")) {
        const auto& unit_object = as_object(unit_value, "Expected contract unit object.");
        ContractUnit unit;
        unit.unit_id = get_string(unit_object, "unit_id");
        unit.kind = get_string(unit_object, "kind");
        {
            auto public_interface_value = optional_field(unit_object, "public_interface");
            if (public_interface_value == nullptr) {
                public_interface_value = optional_field(unit_object, "public_io");
            }
            require(public_interface_value != nullptr, "Expected public_interface or public_io object.");
            const auto& public_interface = as_object(*public_interface_value, "Expected public_interface or public_io object.");
            for (const auto& item : as_array(public_interface.at("inputs"), "public_interface.inputs")) {
                unit.public_interface.inputs.push_back(parse_interface_port(item));
            }
            for (const auto& item : as_array(public_interface.at("outputs"), "public_interface.outputs")) {
                unit.public_interface.outputs.push_back(parse_interface_port(item));
            }
        }
        {
            auto ui_binding_value = optional_field(unit_object, "ui_binding");
            if (ui_binding_value == nullptr) {
                ui_binding_value = optional_field(unit_object, "ui_bindings");
            }
            require(ui_binding_value != nullptr, "Expected ui_binding or ui_bindings object.");
            const auto& ui_binding = as_object(*ui_binding_value, "Expected ui_binding or ui_bindings object.");
            if (const auto* package_refs = optional_field(ui_binding, "package_refs")) {
                unit.ui_binding.package_refs = parse_string_vector(*package_refs, "ui_binding.package_refs");
            }
            for (const auto& item : as_array(ui_binding.at("widgets"), "ui_binding.widgets")) {
                unit.ui_binding.widgets.push_back(parse_widget_binding(item));
            }
            if (const auto* widget_reference_support = optional_field(ui_binding, "widget_reference_support")) {
                for (const auto& item : as_array(*widget_reference_support, "ui_binding.widget_reference_support")) {
                    unit.ui_binding.widget_reference_support.push_back(parse_widget_reference_support(item));
                }
            }
        }
        if (const auto* state_model_value = optional_field(unit_object, "state_model")) {
            const auto& state_model = as_object(*state_model_value, "Expected state_model object.");
            const auto& carrier = as_object(state_model.at("carrier"), "Expected state carrier object.");
            unit.state_model = StateModel{
                get_bool(state_model, "explicit_state"),
                StateCarrier{
                    get_string(carrier, "primitive"),
                    get_string(carrier, "state_id"),
                    get_string(carrier, "type"),
                    static_cast<std::uint16_t>(get_i64(carrier, "initial_value")),
                },
                get_string(state_model, "commit_rule"),
            };
        } else {
            const auto& execution_kernel = as_object(unit_object.at("execution_kernel"), "Expected execution_kernel object.");
            const auto operation = get_optional_string(execution_kernel, "operation").value_or("");
            if (operation == "copy") {
                unit.state_model = StateModel{
                    false,
                    StateCarrier{"", "", get_optional_string(execution_kernel, "type").value_or(""), 0},
                    "",
                };
            } else {
                unit.state_model = StateModel{
                    true,
                    StateCarrier{
                        "frog.core.delay",
                        get_string(execution_kernel, "state_id"),
                        get_string(execution_kernel, "state_type"),
                        static_cast<std::uint16_t>(get_i64(execution_kernel, "initial_state")),
                    },
                    get_string(execution_kernel, "commit_rule"),
                };
            }
        }
        if (const auto* execution_model_value = optional_field(unit_object, "execution_model")) {
            const auto& execution_model = as_object(*execution_model_value, "Expected execution_model object.");
            const auto& body_rule = as_object(execution_model.at("body_rule"), "Expected body_rule object.");
            unit.execution_model = ExecutionModel{
                get_string(execution_model, "structure"),
                static_cast<std::uint32_t>(get_i64(execution_model, "iteration_count")),
                get_optional_string(execution_model, "iteration_variable"),
                BodyRule{get_string(body_rule, "kind"), get_string(body_rule, "expression")},
                get_string(execution_model, "final_result_rule"),
            };
        } else {
            const auto& execution_kernel = as_object(unit_object.at("execution_kernel"), "Expected execution_kernel object.");
            const auto operation = get_optional_string(execution_kernel, "operation").value_or("");
            if (operation == "copy") {
                unit.execution_model = ExecutionModel{
                    "single_step",
                    1,
                    std::nullopt,
                    BodyRule{"copy", get_string(execution_kernel, "src") + " -> " + get_string(execution_kernel, "dst")},
                    get_string(execution_kernel, "src"),
                };
            } else {
                unit.execution_model = ExecutionModel{
                    "bounded_loop",
                    static_cast<std::uint32_t>(get_i64(execution_kernel, "iteration_count")),
                    std::nullopt,
                    BodyRule{"kernel_commit_rule", get_string(execution_kernel, "commit_rule")},
                    "state_current",
                };
            }
        }
        auto property_writes = optional_field(unit_object, "property_writes");
        if (property_writes == nullptr) {
            property_writes = optional_field(unit_object, "effects");
        }
        if (property_writes != nullptr) {
            for (const auto& item : as_array(*property_writes, "property_writes")) {
                unit.property_writes.push_back(parse_property_write(item));
            }
        }
        if (const auto* publication_value = optional_field(unit_object, "public_output_publication")) {
            const auto& publication = as_object(*publication_value, "Expected public_output_publication object.");
            unit.public_output_publication = PublicOutputPublication{get_string(publication, "output_id"), get_string(publication, "source")};
        } else {
            bool found_public_output = false;
            for (const auto& item : as_array(unit_object.at("publications"), "publications")) {
                const auto& publication = as_object(item, "Expected publication object.");
                const auto target = get_string(publication, "target");
                const std::string prefix = "public_output.";
                if (target.rfind(prefix, 0) == 0) {
                    unit.public_output_publication = PublicOutputPublication{target.substr(prefix.size()), get_string(publication, "source")};
                    found_public_output = true;
                    break;
                }
            }
            require(found_public_output, "Expected public output publication.");
        }
        contract.units.push_back(std::move(unit));
    }
    return contract;
}

WfrogPackage load_wfrog_from_path(const std::filesystem::path& path) {
    const auto root = as_object(frog::json::parse_file(path), "Expected .wfrog JSON object.");
    WfrogPackage package;
    package.format = get_string(root, "format");
    require(package.format == "frog.wfrog", "Unsupported .wfrog format.");
    package.kind = get_string(root, "kind");
    require(
        package.kind == "front_panel_package" || package.kind == "widget_realization_package",
        "Only front_panel_package or widget_realization_package is supported.");

    if (const auto* widget_classes = optional_field(root, "widget_classes")) {
        for (const auto& item : as_array(*widget_classes, "widget_classes")) {
            package.widget_classes.push_back(parse_widget_class(item));
        }
    }
    if (const auto* svg_assets = optional_field(root, "svg_assets")) {
        for (const auto& item : as_array(*svg_assets, "svg_assets")) {
            package.svg_assets.push_back(parse_svg_asset(item));
        }
    }
    if (const auto* host_bindings = optional_field(root, "host_bindings")) {
        for (const auto& item : as_array(*host_bindings, "host_bindings")) {
            package.host_bindings.push_back(parse_host_binding(item));
        }
    }
    if (const auto* front_panels = optional_field(root, "front_panels")) {
        for (const auto& item : as_array(*front_panels, "front_panels")) {
            package.front_panels.push_back(parse_front_panel(item));
        }
    }
    return package;
}

FrontPanel load_front_panel_from_frog_source_path(const std::filesystem::path& path) {
    const auto root = as_object(frog::json::parse_file(path), "Expected .frog JSON object.");
    const auto& front_panel = as_object(root.at("front_panel"), "Expected source front_panel object.");
    FrontPanel panel = parse_front_panel(Value(front_panel));
    if (panel.title == "FROG Front Panel") {
        if (const auto* metadata_value = optional_field(root, "metadata"); metadata_value != nullptr && metadata_value->is_object()) {
            panel.title = get_optional_string(metadata_value->as_object(), "title")
                              .value_or(get_optional_string(metadata_value->as_object(), "summary")
                                            .value_or(get_optional_string(metadata_value->as_object(), "name").value_or(panel.title)));
        }
    }
    return panel;
}

} // namespace frog::runtime
