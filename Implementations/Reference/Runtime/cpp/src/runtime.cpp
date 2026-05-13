#include "runtime.hpp"

#include <algorithm>
#include <set>
#include <stdexcept>

namespace frog::runtime {

namespace {

using frog::json::Array;
using frog::json::Object;
using frog::json::Value;

constexpr const char* SUPPORTED_WIDGET_PROPERTIES[] = {
    "value",
    "label",
    "visible",
    "enabled",
    "foreground_color",
};

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

std::string json_string(const Object& object, const std::string& key, const std::string& fallback = "") {
    const auto it = object.find(key);
    if (it == object.end() || !it->second.is_string()) {
        return fallback;
    }
    return it->second.as_string();
}

bool json_bool(const Object& object, const std::string& key, bool fallback = false) {
    const auto it = object.find(key);
    if (it == object.end() || !it->second.is_bool()) {
        return fallback;
    }
    return it->second.as_bool();
}

std::uint16_t json_u16(const Object& object, const std::string& key, std::uint16_t fallback = 0) {
    const auto it = object.find(key);
    if (it == object.end() || !it->second.is_number()) {
        return fallback;
    }
    const auto value = it->second.as_i64();
    require(value >= 0 && value <= 65535, "Widget value must remain in the u16 domain.");
    return static_cast<std::uint16_t>(value);
}

Value make_object(std::initializer_list<std::pair<const std::string, Value>> fields) {
    return Value(Object(fields));
}

Value make_array(std::initializer_list<Value> values) {
    return Value(Array(values));
}

std::filesystem::path resolve_repo_path(const std::filesystem::path& anchor, const std::string& path) {
    const std::filesystem::path candidate(path);
    if (candidate.is_absolute()) {
        return candidate;
    }
    return find_repo_root(anchor) / candidate;
}

const Object& require_object_value(const Value& value, const std::string& label) {
    require(value.is_object(), label + " must be an object.");
    return value.as_object();
}

const Array& require_array_value(const Value& value, const std::string& label) {
    require(value.is_array(), label + " must be an array.");
    return value.as_array();
}

struct EnumItem {
    std::string id;
    std::string text;
    std::uint16_t numeric_value = 0;
    bool enabled = true;
};

std::uint16_t json_u16_value(const Value& value, const std::string& label) {
    require(value.is_number(), label + " must be numeric.");
    const auto raw = value.as_i64();
    require(raw >= 0 && raw <= 65535, label + " must remain in the u16 domain.");
    return static_cast<std::uint16_t>(raw);
}

std::vector<EnumItem> enum_items_from_properties(const Object& properties, const std::string& widget_id) {
    const auto it = properties.find("items");
    require(it != properties.end(), "Enum widget " + widget_id + " must define items in front-panel instance properties.");
    const auto& items = require_array_value(it->second, "Enum widget " + widget_id + " items");

    std::vector<EnumItem> result;
    std::set<std::string> ids;
    std::set<std::uint16_t> numeric_values;
    for (std::size_t index = 0; index < items.size(); ++index) {
        const auto& item_object = require_object_value(items[index], "Enum item");
        const auto id = json_string(item_object, "id");
        const auto text = json_string(item_object, "text");
        require(!id.empty(), "Enum item id must not be empty.");
        require(!text.empty(), "Enum item text must not be empty.");
        const auto numeric_value_it = item_object.find("numeric_value");
        require(numeric_value_it != item_object.end(), "Enum item numeric_value is required.");
        const auto numeric_value = json_u16_value(numeric_value_it->second, "Enum item numeric_value");
        const bool enabled = json_bool(item_object, "enabled", true);
        require(ids.insert(id).second, "Duplicate enum item id: " + id);
        require(numeric_values.insert(numeric_value).second, "Duplicate enum item numeric_value.");
        result.push_back(EnumItem{id, text, numeric_value, enabled});
    }
    require(!result.empty(), "Enum widget " + widget_id + " must declare at least one item.");
    return result;
}

const EnumItem& enum_item_by_id(const std::vector<EnumItem>& items, const std::string& id, const std::string& label) {
    const auto it = std::find_if(items.begin(), items.end(), [&](const EnumItem& item) { return item.id == id; });
    require(it != items.end(), label + " must resolve to a declared enum item.");
    return *it;
}

const EnumItem& enum_item_by_numeric_value(const std::vector<EnumItem>& items, std::uint16_t numeric_value, const std::string& label) {
    const auto it = std::find_if(items.begin(), items.end(), [&](const EnumItem& item) { return item.numeric_value == numeric_value; });
    require(it != items.end(), label + " must resolve to a declared enum item.");
    return *it;
}

void require_same_enum_vocabulary(const std::vector<EnumItem>& left, const std::vector<EnumItem>& right) {
    require(left.size() == right.size(), "Enum control and indicator must publish the same item vocabulary.");
    for (std::size_t index = 0; index < left.size(); ++index) {
        require(left[index].id == right[index].id, "Enum item id mismatch between control and indicator.");
        require(left[index].text == right[index].text, "Enum item text mismatch between control and indicator.");
        require(left[index].numeric_value == right[index].numeric_value, "Enum item numeric value mismatch between control and indicator.");
    }
}

Value enum_items_to_runtime_value(const std::vector<EnumItem>& items) {
    Array out;
    for (const auto& item : items) {
        out.push_back(make_object({
            {"id", Value(item.id)},
            {"text", Value(item.text)},
            {"numeric_value", Value(static_cast<std::int64_t>(item.numeric_value))},
            {"enabled", Value(item.enabled)},
        }));
    }
    return Value(out);
}

} // namespace

Slice05RuntimeCore::Slice05RuntimeCore(std::filesystem::path contract_path_, std::filesystem::path wfrog_path_)
    : contract_path(std::move(contract_path_)),
      wfrog_path(std::move(wfrog_path_)),
      contract(load_contract_from_path(contract_path)),
      package(load_wfrog_from_path(wfrog_path)),
      panel(load_front_panel_from_frog_source_path(resolve_repo_path(contract_path, contract.source_ref.path))),
      unit(load_and_validate()) {
    for (const auto& asset : package.svg_assets) {
        asset_map.emplace(asset.asset_id, std::filesystem::absolute(wfrog_path.parent_path() / asset.path));
    }
    widgets = build_widgets();
    apply_contract_property_writes();
}

std::uint16_t Slice05RuntimeCore::checked_u16(std::uint32_t value, const std::string& label) {
    if (value > 65535u) {
        throw std::runtime_error(label + " must remain in the u16 domain.");
    }
    return static_cast<std::uint16_t>(value);
}

ContractUnit Slice05RuntimeCore::load_and_validate() const {
    require(contract.backend_family == REFERENCE_BACKEND_FAMILY, "Unexpected backend family.");
    require(contract.assumptions.runtime_family.name == REFERENCE_BACKEND_FAMILY, "Unexpected runtime-family assumption name.");
    require(contract.assumptions.runtime_family.ui_binding.widget_value_binding, "Contract must require widget_value_binding.");
    require(contract.assumptions.runtime_family.ui_binding.widget_reference_binding, "Contract must require widget_reference_binding.");
    require(contract.assumptions.numeric_behavior.value_domain == "u16", "Contract numeric behavior must target the u16 domain.");
    require(contract.assumptions.numeric_behavior.overflow_behavior == EXPECTED_OVERFLOW_BEHAVIOR, "Unexpected contract overflow behavior.");
    require(contract.units.size() == 1, "Expected exactly one contract unit.");
    const ContractUnit& current_unit = contract.units.front();
    require(current_unit.unit_id == "main", "Expected unit_id main.");
    require(current_unit.kind == "bounded_executable_ui_unit", "Unexpected runtime unit kind.");
    require(current_unit.public_interface.inputs.size() == 1, "Expected one public input.");
    require(current_unit.public_interface.outputs.size() == 1, "Expected one public output.");
    require(current_unit.public_interface.inputs.front().id == "input_value", "Expected public input input_value.");
    require(current_unit.public_interface.outputs.front().id == "result", "Expected public output result.");
    require(current_unit.execution_model.iteration_count == 5, "Slice 05 expects five iterations.");
    require(current_unit.state_model.carrier.initial_value == 0, "Slice 05 expects initial state 0.");

    const auto& current_panel = panel;
    require(current_panel.host_binding_ref == "reference_host_default", "Expected host_binding_ref reference_host_default.");

    const auto host_it = std::find_if(
        package.host_bindings.begin(),
        package.host_bindings.end(),
        [&](const HostBinding& binding) { return binding.binding_id == "reference_host_default"; });
    require(host_it != package.host_bindings.end(), "Missing reference_host_default host binding.");
    const std::set<std::string> required(host_it->required_capabilities.begin(), host_it->required_capabilities.end());
    require(required.count("window") == 1, "Missing host capability window.");
    require(required.count("basic_widget_rendering") == 1, "Missing host capability basic_widget_rendering.");
    require(required.count("property_write") == 1, "Missing host capability property_write.");
    require(required.count("widget_value_binding") == 1, "Missing host capability widget_value_binding.");
    require(required.count("widget_reference_binding") == 1, "Missing host capability widget_reference_binding.");

    std::map<std::string, const PanelWidget*> panel_widgets;
    for (const auto& widget : current_panel.widgets) {
        panel_widgets.emplace(widget.instance_id, &widget);
    }
    for (const auto& binding : current_unit.ui_binding.widgets) {
        const auto widget_it = panel_widgets.find(binding.widget_id);
        require(widget_it != panel_widgets.end(), "Missing panel widget " + binding.widget_id + ".");
        require(widget_it->second->class_ref == binding.widget_class, "Class mismatch for widget " + binding.widget_id + ".");
        require(
            binding.widget_class == "frog.widgets.numeric_control" || binding.widget_class == "frog.widgets.numeric_indicator",
            "Unsupported widget class " + binding.widget_class + ".");
    }

    std::map<std::string, std::set<std::string>> support;
    for (const auto& entry : current_unit.ui_binding.widget_reference_support) {
        support.emplace(entry.widget_id, std::set<std::string>(entry.supported_members.begin(), entry.supported_members.end()));
    }
    for (const auto& property_write : current_unit.property_writes) {
        const bool supported_property = std::any_of(
            std::begin(SUPPORTED_WIDGET_PROPERTIES),
            std::end(SUPPORTED_WIDGET_PROPERTIES),
            [&](const char* name) { return property_write.member == name; });
        require(supported_property, "Unsupported property write " + property_write.member + ".");
        const auto it = support.find(property_write.widget_id);
        require(it != support.end(), "Missing widget reference support for " + property_write.widget_id + ".");
        require(it->second.count(property_write.member) == 1, "Unsupported widget reference member " + property_write.widget_id + "." + property_write.member + ".");
    }

    return current_unit;
}

std::map<std::string, WidgetState> Slice05RuntimeCore::build_widgets() const {
    std::map<std::string, std::set<std::string>> support;
    for (const auto& entry : unit.ui_binding.widget_reference_support) {
        support.emplace(entry.widget_id, std::set<std::string>(entry.supported_members.begin(), entry.supported_members.end()));
    }

    std::map<std::string, const PanelWidget*> panel_widgets;
    for (const auto& widget : panel.widgets) {
        panel_widgets.emplace(widget.instance_id, &widget);
    }

    std::map<std::string, WidgetState> result;
    for (const auto& binding : unit.ui_binding.widgets) {
        const auto* panel_widget = panel_widgets.at(binding.widget_id);
        std::optional<std::string> asset_id;
        std::filesystem::path asset_path;
        if (const auto visual_it = panel_widget->visual.find("asset_ref"); visual_it != panel_widget->visual.end() && visual_it->second.is_string()) {
            const std::string& asset_ref = visual_it->second.as_string();
            if (asset_ref.rfind("asset:", 0) == 0) {
                asset_id = asset_ref.substr(6);
                const auto asset_it = asset_map.find(*asset_id);
                if (asset_it != asset_map.end()) {
                    asset_path = asset_it->second;
                }
            }
        }

        auto properties = panel_widget->props;
        properties.emplace("value", properties.count("value") ? properties.at("value") : Value(0));
        properties.emplace("label", properties.count("label") ? properties.at("label") : Value(""));
        properties.emplace("visible", properties.count("visible") ? properties.at("visible") : Value(true));
        properties.emplace("enabled", properties.count("enabled") ? properties.at("enabled") : Value(true));
        properties.emplace("foreground_color", properties.count("foreground_color") ? properties.at("foreground_color") : Value("#D8D8D8"));

        const auto support_it = support.find(binding.widget_id);
        std::vector<std::string> supported_members;
        if (support_it != support.end()) {
            supported_members.assign(support_it->second.begin(), support_it->second.end());
        }

        result.emplace(
            binding.widget_id,
            WidgetState{
                binding.widget_id,
                binding.widget_class,
                binding.role,
                panel_widget->layout,
                std::move(properties),
                asset_id,
                asset_path,
                std::move(supported_members),
            });
    }
    return result;
}

void Slice05RuntimeCore::apply_contract_property_writes() {
    applied_widget_references.clear();
    for (const auto& property_write : unit.property_writes) {
        auto widget_it = widgets.find(property_write.widget_id);
        require(widget_it != widgets.end(), "Unknown widget " + property_write.widget_id + ".");
        const auto& supported = widget_it->second.supported_members;
        require(
            std::find(supported.begin(), supported.end(), property_write.member) != supported.end(),
            "Property " + property_write.member + " is not supported by widget " + property_write.widget_id + ".");
        widget_it->second.properties[property_write.member] = Value(property_write.value.value);
        applied_widget_references.push_back(make_object({
            {"widget_id", Value(property_write.widget_id)},
            {"member", Value(property_write.member)},
            {"value", Value(property_write.value.value)},
        }));
    }
}

void Slice05RuntimeCore::set_control_value(std::uint16_t value) {
    widgets.at("ctrl_input").properties["value"] = Value(static_cast<std::int64_t>(value));
}

std::uint16_t Slice05RuntimeCore::control_value() const {
    return json_u16(widgets.at("ctrl_input").properties, "value", 0);
}

void Slice05RuntimeCore::reset_to_default_style(const std::string& widget_id) {
    const auto widget_it = std::find_if(
        panel.widgets.begin(), panel.widgets.end(), [&](const PanelWidget& widget) { return widget.instance_id == widget_id; });
    require(widget_it != panel.widgets.end(), "Unknown widget " + widget_id + ".");
    auto properties = widget_it->props;
    properties["value"] = properties.count("value") ? properties.at("value") : Value(0);
    properties["label"] = properties.count("label") ? properties.at("label") : Value("");
    properties["visible"] = properties.count("visible") ? properties.at("visible") : Value(true);
    properties["enabled"] = properties.count("enabled") ? properties.at("enabled") : Value(true);
    properties["foreground_color"] = properties.count("foreground_color") ? properties.at("foreground_color") : Value("#D8D8D8");
    widgets.at(widget_id).properties = std::move(properties);
}

void Slice05RuntimeCore::invoke_method(const std::string& widget_id, const std::string& method_name) {
    if (widget_id == "ctrl_input" && method_name == "focus") {
        return;
    }
    if (widget_id == "ind_result" && method_name == "reset_to_default_style") {
        reset_to_default_style(widget_id);
        return;
    }
    throw std::runtime_error("Unsupported method invocation: " + widget_id + "." + method_name + ".");
}

Value Slice05RuntimeCore::execute(std::optional<std::uint16_t> control_value_override) {
    apply_contract_property_writes();
    if (control_value_override.has_value()) {
        set_control_value(*control_value_override);
    }
    const std::uint32_t input_value = control_value();
    std::uint32_t state = unit.state_model.carrier.initial_value;
    for (std::uint32_t index = 0; index < unit.execution_model.iteration_count; ++index) {
        state = checked_u16(state + input_value, "final_state");
    }
    last_final_state = static_cast<std::uint16_t>(state);
    widgets.at("ind_result").properties["value"] = Value(static_cast<std::int64_t>(last_final_state));
    return execution_artifact();
}

Value Slice05RuntimeCore::execute_with_native_kernel_bridge(
    const NativeKernelBridge& bridge,
    std::optional<std::uint16_t> control_value_override) {
    require(bridge.manifest().source_lowered_unit == "Examples/05_bounded_ui_accumulator/main.lowering.json", "Unexpected native kernel source lowered unit.");
    apply_contract_property_writes();
    if (control_value_override.has_value()) {
        set_control_value(*control_value_override);
    }

    const auto result = bridge.run(control_value());
    if (!result.ok) {
        throw std::runtime_error(result.diagnostic.empty() ? "native kernel execution failed." : result.diagnostic);
    }

    last_final_state = result.result;
    widgets.at("ind_result").properties["value"] = Value(static_cast<std::int64_t>(last_final_state));
    return execution_artifact();
}

Value Slice05RuntimeCore::execution_artifact() const {
    Array widget_entries;
    for (const auto& entry : widgets) {
        const auto& widget = entry.second;
        widget_entries.push_back(make_object({
            {"widget_id", Value(widget.widget_id)},
            {"class_ref", Value(widget.class_ref)},
            {"role", Value(widget.role)},
            {"layout", widget.layout},
            {"runtime", make_object({
                {"value", widget.properties.count("value") ? widget.properties.at("value") : Value(0)},
                {"label", Value(json_string(widget.properties, "label"))},
                {"visible", Value(json_bool(widget.properties, "visible", true))},
                {"enabled", Value(json_bool(widget.properties, "enabled", true))},
                {"foreground_color", Value(json_string(widget.properties, "foreground_color", "#D8D8D8"))},
                {"asset_ref", widget.asset_id.has_value() ? Value("asset:" + *widget.asset_id) : Value(nullptr)},
            })},
        }));
    }

    Array applied = applied_widget_references;
    return make_object({
        {"artifact_kind", Value("frog_runtime_execution_result")},
        {"artifact_governance_ref", make_object({{"path", Value("Versioning/Readme.md")}})},
        {"status", Value("ok")},
        {"contract_ref", make_object({
            {"unit_ids", make_array({Value(unit.unit_id)})},
            {"backend_family", Value(contract.backend_family)},
            {"source_ref", make_object({
                {"example_id", Value(contract.source_ref.example_id)},
                {"path", Value(contract.source_ref.path)},
                {"entry_unit", Value(contract.source_ref.entry_unit)},
            })},
        })},
        {"execution_summary", make_object({
            {"mode", Value("contract_and_wfrog")},
            {"executed_unit", Value(unit.unit_id)},
            {"iterations", Value(static_cast<std::int64_t>(unit.execution_model.iteration_count))},
            {"state_initialized", Value(true)},
            {"initial_state", Value(static_cast<std::int64_t>(unit.state_model.carrier.initial_value))},
            {"final_state", Value(static_cast<std::int64_t>(last_final_state))},
        })},
        {"outputs", make_object({
            {"public", make_object({{"result", Value(static_cast<std::int64_t>(last_final_state))}})},
            {"ui", make_object({
                {"ctrl_input", Value(static_cast<std::int64_t>(control_value()))},
                {"ind_result", Value(static_cast<std::int64_t>(last_final_state))},
            })},
        })},
        {"ui_runtime", make_object({
            {"panel", make_object({
                {"panel_id", Value(panel.panel_id)},
                {"title", Value(panel.title)},
                {"class_ref", Value(panel.class_ref)},
                {"layout", panel.layout},
            })},
            {"widgets", Value(widget_entries)},
            {"applied_widget_references", Value(applied)},
        })},
        {"diagnostics", Value(Array{})},
    });
}

Slice06BooleanRuntimeCore::Slice06BooleanRuntimeCore(std::filesystem::path contract_path_, std::filesystem::path wfrog_path_)
    : contract_path(std::move(contract_path_)),
      wfrog_path(std::move(wfrog_path_)),
      contract(load_contract_from_path(contract_path)),
      package(load_wfrog_from_path(wfrog_path)),
      panel(load_front_panel_from_frog_source_path(resolve_repo_path(contract_path, contract.source_ref.path))),
      unit(load_and_validate()) {
    for (const auto& asset : package.svg_assets) {
        asset_map.emplace(asset.asset_id, std::filesystem::absolute(wfrog_path.parent_path() / asset.path));
    }
    widgets = build_widgets();
    last_result = control_value();
    widgets.at("bool_result").properties["value"] = Value(last_result);
}

ContractUnit Slice06BooleanRuntimeCore::load_and_validate() const {
    require(contract.backend_family == REFERENCE_BACKEND_FAMILY, "Unexpected backend family.");
    require(contract.source_ref.example_id == "06_boolean_value_roundtrip", "Slice 06 expects Example 06.");
    require(contract.assumptions.runtime_family.name == REFERENCE_BACKEND_FAMILY, "Unexpected runtime-family assumption name.");
    require(contract.assumptions.runtime_family.ui_binding.widget_value_binding, "Contract must require widget_value_binding.");
    require(contract.units.size() == 1, "Expected exactly one contract unit.");

    const ContractUnit& current_unit = contract.units.front();
    require(current_unit.unit_id == "main", "Expected unit_id main.");
    require(current_unit.kind == "boolean_value_roundtrip_ui_unit", "Unexpected runtime unit kind.");
    require(current_unit.public_interface.inputs.size() == 1, "Expected one public input.");
    require(current_unit.public_interface.outputs.size() == 1, "Expected one public output.");
    require(current_unit.public_interface.inputs.front().id == "input_value", "Expected public input input_value.");
    require(current_unit.public_interface.inputs.front().port_type == "bool", "Expected bool public input.");
    require(current_unit.public_interface.outputs.front().id == "result", "Expected public output result.");
    require(current_unit.public_interface.outputs.front().port_type == "bool", "Expected bool public output.");
    require(current_unit.execution_model.structure == "single_step", "Slice 06 expects a single-step copy execution model.");
    require(current_unit.execution_model.body_rule.kind == "copy", "Slice 06 expects a copy body rule.");
    require(current_unit.property_writes.empty(), "Slice 06 does not use property writes.");

    const auto& current_panel = panel;
    require(current_panel.host_binding_ref == "reference_host_default", "Expected host_binding_ref reference_host_default.");

    const auto host_it = std::find_if(
        package.host_bindings.begin(),
        package.host_bindings.end(),
        [&](const HostBinding& binding) { return binding.binding_id == "reference_host_default"; });
    require(host_it != package.host_bindings.end(), "Missing reference_host_default host binding.");
    const std::set<std::string> required(host_it->required_capabilities.begin(), host_it->required_capabilities.end());
    require(required.count("window") == 1, "Missing host capability window.");
    require(required.count("basic_widget_rendering") == 1, "Missing host capability basic_widget_rendering.");
    require(required.count("widget_value_binding") == 1, "Missing host capability widget_value_binding.");

    std::map<std::string, const PanelWidget*> panel_widgets;
    for (const auto& widget : current_panel.widgets) {
        panel_widgets.emplace(widget.instance_id, &widget);
    }
    require(panel_widgets.count("bool_input") == 1, "Missing panel widget bool_input.");
    require(panel_widgets.count("bool_result") == 1, "Missing panel widget bool_result.");

    for (const auto& binding : current_unit.ui_binding.widgets) {
        const auto widget_it = panel_widgets.find(binding.widget_id);
        require(widget_it != panel_widgets.end(), "Missing panel widget " + binding.widget_id + ".");
        require(widget_it->second->class_ref == binding.widget_class, "Class mismatch for widget " + binding.widget_id + ".");
        require(binding.value_type == "bool", "Slice 06 supports only bool widget values.");
        require(
            binding.widget_class == "frog.widgets.boolean_control" || binding.widget_class == "frog.widgets.boolean_indicator",
            "Unsupported widget class " + binding.widget_class + ".");
    }

    return current_unit;
}

std::map<std::string, WidgetState> Slice06BooleanRuntimeCore::build_widgets() const {
    std::map<std::string, const PanelWidget*> panel_widgets;
    for (const auto& widget : panel.widgets) {
        panel_widgets.emplace(widget.instance_id, &widget);
    }

    std::map<std::string, WidgetState> result;
    for (const auto& binding : unit.ui_binding.widgets) {
        const auto* panel_widget = panel_widgets.at(binding.widget_id);
        std::optional<std::string> asset_id;
        std::filesystem::path asset_path;
        if (const auto visual_it = panel_widget->visual.find("asset_ref"); visual_it != panel_widget->visual.end() && visual_it->second.is_string()) {
            const std::string& asset_ref = visual_it->second.as_string();
            if (asset_ref.rfind("asset:", 0) == 0) {
                asset_id = asset_ref.substr(6);
                const auto asset_it = asset_map.find(*asset_id);
                if (asset_it != asset_map.end()) {
                    asset_path = asset_it->second;
                }
            }
        }

        auto properties = panel_widget->props;
        properties.emplace("value", properties.count("value") ? properties.at("value") : Value(false));
        properties.emplace("caption.text", properties.count("caption.text") ? properties.at("caption.text") : Value(binding.widget_id));
        properties.emplace("state_text.true_text", properties.count("state_text.true_text") ? properties.at("state_text.true_text") : Value("TRUE"));
        properties.emplace("state_text.false_text", properties.count("state_text.false_text") ? properties.at("state_text.false_text") : Value("FALSE"));
        properties.emplace("interaction.enabled", properties.count("interaction.enabled") ? properties.at("interaction.enabled") : Value(binding.role == "control"));
        properties.emplace("realization.variant", properties.count("realization.variant") ? properties.at("realization.variant") : Value(""));

        result.emplace(
            binding.widget_id,
            WidgetState{
                binding.widget_id,
                binding.widget_class,
                binding.role,
                panel_widget->layout,
                std::move(properties),
                asset_id,
                asset_path,
                {},
            });
    }
    return result;
}

void Slice06BooleanRuntimeCore::set_control_value(bool value) {
    widgets.at("bool_input").properties["value"] = Value(value);
}

bool Slice06BooleanRuntimeCore::control_value() const {
    return json_bool(widgets.at("bool_input").properties, "value", false);
}

Value Slice06BooleanRuntimeCore::execute(std::optional<bool> control_value_override) {
    if (control_value_override.has_value()) {
        set_control_value(*control_value_override);
    }
    last_result = control_value();
    widgets.at("bool_result").properties["value"] = Value(last_result);
    return execution_artifact();
}

Value Slice06BooleanRuntimeCore::execute_with_native_kernel_bridge(
    const NativeBoolKernelBridge& bridge,
    std::optional<bool> control_value_override) {
    require(bridge.manifest().source_lowered_unit == "Examples/06_boolean_value_roundtrip/main.lowering.json", "Unexpected native bool kernel source lowered unit.");
    if (control_value_override.has_value()) {
        set_control_value(*control_value_override);
    }

    const auto result = bridge.run(control_value());
    if (!result.ok) {
        throw std::runtime_error(result.diagnostic.empty() ? "native bool kernel execution failed." : result.diagnostic);
    }

    last_result = result.result;
    widgets.at("bool_result").properties["value"] = Value(last_result);
    return execution_artifact();
}

Value Slice06BooleanRuntimeCore::execution_artifact() const {
    Array widget_entries;
    for (const auto& entry : widgets) {
        const auto& widget = entry.second;
        const bool value = json_bool(widget.properties, "value", false);
        Object runtime_fields{
            {"value", Value(value)},
            {"label.text", Value(json_string(widget.properties, "label.text"))},
            {"caption.text", Value(json_string(widget.properties, "caption.text"))},
            {"state_text.true_text", Value(json_string(widget.properties, "state_text.true_text", "TRUE"))},
            {"state_text.false_text", Value(json_string(widget.properties, "state_text.false_text", "FALSE"))},
            {"asset_ref", widget.asset_id.has_value() ? Value("asset:" + *widget.asset_id) : Value(nullptr)},
            {"realization.variant", Value(json_string(widget.properties, "realization.variant"))},
        };
        const auto copy_property = [&](const std::string& key) {
            const auto it = widget.properties.find(key);
            if (it != widget.properties.end()) {
                runtime_fields.emplace(key, it->second);
            }
        };
        copy_property("state_text.style.text_color.false");
        copy_property("state_text.style.text_color.true");
        copy_property("state_text.visible");
        copy_property("caption.visible");
        copy_property("caption.anchor.x");
        copy_property("caption.anchor.y");
        copy_property("caption.align.horizontal");
        copy_property("style.frame.visible");
        copy_property("style.outer.border_color.false");
        copy_property("style.outer.border_color.true");
        copy_property("style.outer.border_color.hover_false");
        copy_property("style.outer.border_color.hover_true");
        copy_property("style.outer.border_color.pressed_false");
        copy_property("style.outer.border_color.pressed_true");
        copy_property("style.inner.fill_color.false");
        copy_property("style.inner.fill_color.true");
        copy_property("style.inner.fill_color.hover_false");
        copy_property("style.inner.fill_color.hover_true");
        copy_property("style.inner.fill_color.pressed_false");
        copy_property("style.inner.fill_color.pressed_true");
        copy_property("style.inner.border_color.false");
        copy_property("style.inner.border_color.true");
        copy_property("style.inner.border_color.hover_false");
        copy_property("style.inner.border_color.hover_true");
        copy_property("style.inner.border_color.pressed_false");
        copy_property("style.inner.border_color.pressed_true");
        copy_property("style.inner.left");
        copy_property("style.inner.top");
        copy_property("style.inner.width");
        copy_property("style.inner.height");
        copy_property("style.focus_ring.visible");
        copy_property("style.focus_ring.color");
        copy_property("style.focus_ring.width");
        copy_property("style.pressed.inset");
        copy_property("style.transition.duration_ms");
        copy_property("style.transition.timing");
        widget_entries.push_back(make_object({
            {"widget_id", Value(widget.widget_id)},
            {"class_ref", Value(widget.class_ref)},
            {"role", Value(widget.role)},
            {"layout", widget.layout},
            {"runtime", Value(runtime_fields)},
        }));
    }

    return make_object({
        {"artifact_kind", Value("frog_runtime_execution_result")},
        {"artifact_governance_ref", make_object({{"path", Value("Versioning/Readme.md")}})},
        {"status", Value("ok")},
        {"contract_ref", make_object({
            {"unit_ids", make_array({Value(unit.unit_id)})},
            {"backend_family", Value(contract.backend_family)},
            {"source_ref", make_object({
                {"example_id", Value(contract.source_ref.example_id)},
                {"path", Value(contract.source_ref.path)},
                {"entry_unit", Value(contract.source_ref.entry_unit)},
            })},
        })},
        {"execution_summary", make_object({
            {"mode", Value("boolean_value_roundtrip")},
            {"executed_unit", Value(unit.unit_id)},
            {"operation", Value("copy")},
            {"input_value", Value(control_value())},
            {"result", Value(last_result)},
        })},
        {"outputs", make_object({
            {"public", make_object({{"result", Value(last_result)}})},
            {"ui", make_object({
                {"bool_input", Value(control_value())},
                {"bool_result", Value(last_result)},
            })},
        })},
        {"ui_runtime", make_object({
            {"panel", make_object({
                {"panel_id", Value(panel.panel_id)},
                {"title", Value(panel.title)},
                {"class_ref", Value(panel.class_ref)},
                {"layout", panel.layout},
            })},
            {"widgets", Value(widget_entries)},
        })},
        {"diagnostics", Value(Array{})},
    });
}

Slice07StringRuntimeCore::Slice07StringRuntimeCore(std::filesystem::path contract_path_, std::filesystem::path wfrog_path_)
    : contract_path(std::move(contract_path_)),
      wfrog_path(std::move(wfrog_path_)),
      contract(load_contract_from_path(contract_path)),
      package(load_wfrog_from_path(wfrog_path)),
      panel(load_front_panel_from_frog_source_path(resolve_repo_path(contract_path, contract.source_ref.path))),
      unit(load_and_validate()) {
    for (const auto& asset : package.svg_assets) {
        asset_map.emplace(asset.asset_id, std::filesystem::absolute(wfrog_path.parent_path() / asset.path));
    }
    widgets = build_widgets();
    last_result = control_value();
    widgets.at("str_result").properties["value"] = Value(last_result);
}

ContractUnit Slice07StringRuntimeCore::load_and_validate() const {
    require(contract.backend_family == REFERENCE_BACKEND_FAMILY, "Unexpected backend family.");
    require(contract.source_ref.example_id == "07_string_value_roundtrip", "Slice 07 expects Example 07.");
    require(contract.assumptions.runtime_family.name == REFERENCE_BACKEND_FAMILY, "Unexpected runtime-family assumption name.");
    require(contract.assumptions.runtime_family.ui_binding.widget_value_binding, "Contract must require widget_value_binding.");
    require(contract.units.size() == 1, "Expected exactly one contract unit.");

    const ContractUnit& current_unit = contract.units.front();
    require(current_unit.unit_id == "main", "Expected unit_id main.");
    require(current_unit.kind == "string_value_roundtrip_ui_unit", "Unexpected runtime unit kind.");
    require(current_unit.public_interface.inputs.size() == 1, "Expected one public input.");
    require(current_unit.public_interface.outputs.size() == 1, "Expected one public output.");
    require(current_unit.public_interface.inputs.front().id == "input_text", "Expected public input input_text.");
    require(current_unit.public_interface.inputs.front().port_type == "string", "Expected string public input.");
    require(current_unit.public_interface.outputs.front().id == "result_text", "Expected public output result_text.");
    require(current_unit.public_interface.outputs.front().port_type == "string", "Expected string public output.");
    require(current_unit.execution_model.structure == "single_step", "Slice 07 expects a single-step copy execution model.");
    require(current_unit.execution_model.body_rule.kind == "copy", "Slice 07 expects a copy body rule.");
    require(current_unit.property_writes.empty(), "Slice 07 does not use property writes.");

    const auto& current_panel = panel;
    require(current_panel.host_binding_ref == "reference_host_default", "Expected host_binding_ref reference_host_default.");

    const auto host_it = std::find_if(
        package.host_bindings.begin(),
        package.host_bindings.end(),
        [&](const HostBinding& binding) { return binding.binding_id == "reference_host_default"; });
    require(host_it != package.host_bindings.end(), "Missing reference_host_default host binding.");
    const std::set<std::string> required(host_it->required_capabilities.begin(), host_it->required_capabilities.end());
    require(required.count("window") == 1, "Missing host capability window.");
    require(required.count("basic_widget_rendering") == 1, "Missing host capability basic_widget_rendering.");
    require(required.count("widget_value_binding") == 1, "Missing host capability widget_value_binding.");

    std::map<std::string, const PanelWidget*> panel_widgets;
    for (const auto& widget : current_panel.widgets) {
        panel_widgets.emplace(widget.instance_id, &widget);
    }
    require(panel_widgets.count("str_input") == 1, "Missing panel widget str_input.");
    require(panel_widgets.count("str_result") == 1, "Missing panel widget str_result.");

    for (const auto& binding : current_unit.ui_binding.widgets) {
        const auto widget_it = panel_widgets.find(binding.widget_id);
        require(widget_it != panel_widgets.end(), "Missing panel widget " + binding.widget_id + ".");
        require(widget_it->second->class_ref == binding.widget_class, "Class mismatch for widget " + binding.widget_id + ".");
        require(binding.value_type == "string", "Slice 07 supports only string widget values.");
        require(
            binding.widget_class == "frog.widgets.string_control" || binding.widget_class == "frog.widgets.string_indicator",
            "Unsupported widget class " + binding.widget_class + ".");
    }

    return current_unit;
}

std::map<std::string, WidgetState> Slice07StringRuntimeCore::build_widgets() const {
    std::map<std::string, const PanelWidget*> panel_widgets;
    for (const auto& widget : panel.widgets) {
        panel_widgets.emplace(widget.instance_id, &widget);
    }

    std::map<std::string, WidgetState> result;
    for (const auto& binding : unit.ui_binding.widgets) {
        const auto* panel_widget = panel_widgets.at(binding.widget_id);
        std::optional<std::string> asset_id;
        std::filesystem::path asset_path;
        if (const auto visual_it = panel_widget->visual.find("asset_ref"); visual_it != panel_widget->visual.end() && visual_it->second.is_string()) {
            const std::string& asset_ref = visual_it->second.as_string();
            if (asset_ref.rfind("asset:", 0) == 0) {
                asset_id = asset_ref.substr(6);
                const auto asset_it = asset_map.find(*asset_id);
                if (asset_it != asset_map.end()) {
                    asset_path = asset_it->second;
                }
            }
        }

        auto properties = panel_widget->props;
        properties.emplace("value", properties.count("value") ? properties.at("value") : Value(""));
        properties.emplace("caption.text", properties.count("caption.text") ? properties.at("caption.text") : Value(binding.widget_id));
        properties.emplace("placeholder.text", properties.count("placeholder.text") ? properties.at("placeholder.text") : Value(""));
        properties.emplace("placeholder.visible", properties.count("placeholder.visible") ? properties.at("placeholder.visible") : Value(false));
        properties.emplace("interaction.enabled", properties.count("interaction.enabled") ? properties.at("interaction.enabled") : Value(binding.role == "control"));
        properties.emplace("interaction.read_only", properties.count("interaction.read_only") ? properties.at("interaction.read_only") : Value(binding.role != "control"));
        properties.emplace("realization.variant", properties.count("realization.variant") ? properties.at("realization.variant") : Value("rectangular"));

        result.emplace(
            binding.widget_id,
            WidgetState{
                binding.widget_id,
                binding.widget_class,
                binding.role,
                panel_widget->layout,
                std::move(properties),
                asset_id,
                asset_path,
                {},
            });
    }
    return result;
}

void Slice07StringRuntimeCore::set_control_value(const std::string& value) {
    require(value.size() <= 256, "input_text must remain within 256 UTF-8 bytes.");
    widgets.at("str_input").properties["value"] = Value(value);
}

std::string Slice07StringRuntimeCore::control_value() const {
    return json_string(widgets.at("str_input").properties, "value", "");
}

Value Slice07StringRuntimeCore::execute(std::optional<std::string> control_value_override) {
    if (control_value_override.has_value()) {
        set_control_value(*control_value_override);
    }
    last_result = control_value();
    widgets.at("str_result").properties["value"] = Value(last_result);
    return execution_artifact();
}

Value Slice07StringRuntimeCore::execute_with_native_kernel_bridge(
    const NativeStringKernelBridge& bridge,
    std::optional<std::string> control_value_override) {
    require(bridge.manifest().source_lowered_unit == "Examples/07_string_value_roundtrip/main.lowering.json", "Unexpected native string kernel source lowered unit.");
    if (control_value_override.has_value()) {
        set_control_value(*control_value_override);
    }

    const auto result = bridge.run(control_value());
    if (!result.ok) {
        throw std::runtime_error(result.diagnostic.empty() ? "native string kernel execution failed." : result.diagnostic);
    }

    last_result = result.result;
    widgets.at("str_result").properties["value"] = Value(last_result);
    return execution_artifact();
}

Value Slice07StringRuntimeCore::execution_artifact() const {
    Array widget_entries;
    for (const auto& entry : widgets) {
        const auto& widget = entry.second;
        Object runtime_fields{
            {"value", Value(json_string(widget.properties, "value"))},
            {"label.text", Value(json_string(widget.properties, "label.text"))},
            {"caption.text", Value(json_string(widget.properties, "caption.text"))},
            {"asset_ref", widget.asset_id.has_value() ? Value("asset:" + *widget.asset_id) : Value(nullptr)},
            {"realization.variant", Value(json_string(widget.properties, "realization.variant", "rectangular"))},
        };
        const auto copy_property = [&](const std::string& key) {
            const auto it = widget.properties.find(key);
            if (it != widget.properties.end()) {
                runtime_fields.emplace(key, it->second);
            }
        };
        copy_property("caption.visible");
        copy_property("caption.anchor.x");
        copy_property("caption.anchor.y");
        copy_property("caption.align.horizontal");
        copy_property("caption.style.text_color");
        copy_property("style.frame.fill_color");
        copy_property("style.frame.border_color");
        copy_property("style.frame.border_width");
        copy_property("style.text_region.fill_color");
        copy_property("style.text_region.fill_color.hover");
        copy_property("style.text_region.border_color");
        copy_property("style.text_region.border_color.hover");
        copy_property("style.text_region.border_width");
        copy_property("style.text_region.border_width.hover");
        copy_property("style.text.color");
        copy_property("style.text.font_size");
        copy_property("style.text.font_weight");
        copy_property("placeholder.text");
        copy_property("placeholder.visible");
        copy_property("interaction.enabled");
        copy_property("interaction.read_only");

        widget_entries.push_back(make_object({
            {"widget_id", Value(widget.widget_id)},
            {"class_ref", Value(widget.class_ref)},
            {"role", Value(widget.role)},
            {"layout", widget.layout},
            {"runtime", Value(runtime_fields)},
        }));
    }

    return make_object({
        {"artifact_kind", Value("frog_runtime_execution_result")},
        {"artifact_governance_ref", make_object({{"path", Value("Versioning/Readme.md")}})},
        {"status", Value("ok")},
        {"contract_ref", make_object({
            {"unit_ids", make_array({Value(unit.unit_id)})},
            {"backend_family", Value(contract.backend_family)},
            {"source_ref", make_object({
                {"example_id", Value(contract.source_ref.example_id)},
                {"path", Value(contract.source_ref.path)},
                {"entry_unit", Value(contract.source_ref.entry_unit)},
            })},
        })},
        {"execution_summary", make_object({
            {"mode", Value("string_value_roundtrip")},
            {"executed_unit", Value(unit.unit_id)},
            {"operation", Value("copy")},
            {"input_text", Value(control_value())},
            {"result_text", Value(last_result)},
        })},
        {"outputs", make_object({
            {"public", make_object({{"result_text", Value(last_result)}})},
            {"ui", make_object({
                {"str_input", Value(control_value())},
                {"str_result", Value(last_result)},
            })},
        })},
        {"ui_runtime", make_object({
            {"panel", make_object({
                {"panel_id", Value(panel.panel_id)},
                {"title", Value(panel.title)},
                {"class_ref", Value(panel.class_ref)},
                {"layout", panel.layout},
            })},
            {"widgets", Value(widget_entries)},
        })},
        {"diagnostics", Value(Array{})},
    });
}

Slice08EnumRuntimeCore::Slice08EnumRuntimeCore(std::filesystem::path contract_path_, std::filesystem::path wfrog_path_)
    : contract_path(std::move(contract_path_)),
      wfrog_path(std::move(wfrog_path_)),
      contract(load_contract_from_path(contract_path)),
      package(load_wfrog_from_path(wfrog_path)),
      panel(load_front_panel_from_frog_source_path(resolve_repo_path(contract_path, contract.source_ref.path))),
      unit(load_and_validate()) {
    for (const auto& asset : package.svg_assets) {
        asset_map.emplace(asset.asset_id, std::filesystem::absolute(wfrog_path.parent_path() / asset.path));
    }
    widgets = build_widgets();
    last_result = control_value();
    widgets.at("mode_result").properties["value"] = Value(last_result);
}

ContractUnit Slice08EnumRuntimeCore::load_and_validate() const {
    require(contract.backend_family == REFERENCE_BACKEND_FAMILY, "Unexpected backend family.");
    require(contract.source_ref.example_id == "08_enum_value_roundtrip", "Slice 08 expects Example 08.");
    require(contract.assumptions.runtime_family.name == REFERENCE_BACKEND_FAMILY, "Unexpected runtime-family assumption name.");
    require(contract.assumptions.runtime_family.ui_binding.widget_value_binding, "Contract must require widget_value_binding.");
    require(contract.units.size() == 1, "Expected exactly one contract unit.");

    const ContractUnit& current_unit = contract.units.front();
    require(current_unit.unit_id == "main", "Expected unit_id main.");
    require(current_unit.kind == "enum_value_roundtrip_ui_unit", "Unexpected runtime unit kind.");
    require(current_unit.public_interface.inputs.size() == 1, "Expected one public input.");
    require(current_unit.public_interface.outputs.size() == 1, "Expected one public output.");
    require(current_unit.public_interface.inputs.front().id == "mode_value", "Expected public input mode_value.");
    require(current_unit.public_interface.inputs.front().port_type == "enum_item_id", "Expected enum_item_id public input.");
    require(current_unit.public_interface.outputs.front().id == "result_mode", "Expected public output result_mode.");
    require(current_unit.public_interface.outputs.front().port_type == "enum_item_id", "Expected enum_item_id public output.");
    require(current_unit.execution_model.structure == "single_step", "Slice 08 expects a single-step copy execution model.");
    require(current_unit.execution_model.body_rule.kind == "copy", "Slice 08 expects a copy body rule.");
    require(current_unit.property_writes.empty(), "Slice 08 does not use property writes.");

    const auto& current_panel = panel;
    require(current_panel.host_binding_ref == "reference_host_default", "Expected host_binding_ref reference_host_default.");

    const auto host_it = std::find_if(
        package.host_bindings.begin(),
        package.host_bindings.end(),
        [&](const HostBinding& binding) { return binding.binding_id == "reference_host_default"; });
    require(host_it != package.host_bindings.end(), "Missing reference_host_default host binding.");
    const std::set<std::string> required(host_it->required_capabilities.begin(), host_it->required_capabilities.end());
    require(required.count("window") == 1, "Missing host capability window.");
    require(required.count("basic_widget_rendering") == 1, "Missing host capability basic_widget_rendering.");
    require(required.count("widget_value_binding") == 1, "Missing host capability widget_value_binding.");

    std::map<std::string, const PanelWidget*> panel_widgets;
    for (const auto& widget : current_panel.widgets) {
        panel_widgets.emplace(widget.instance_id, &widget);
    }
    require(panel_widgets.count("mode_input") == 1, "Missing panel widget mode_input.");
    require(panel_widgets.count("mode_result") == 1, "Missing panel widget mode_result.");
    require_same_enum_vocabulary(
        enum_items_from_properties(panel_widgets.at("mode_input")->props, "mode_input"),
        enum_items_from_properties(panel_widgets.at("mode_result")->props, "mode_result"));

    for (const auto& binding : current_unit.ui_binding.widgets) {
        const auto widget_it = panel_widgets.find(binding.widget_id);
        require(widget_it != panel_widgets.end(), "Missing panel widget " + binding.widget_id + ".");
        require(widget_it->second->class_ref == binding.widget_class, "Class mismatch for widget " + binding.widget_id + ".");
        require(binding.value_type == "enum_item_id", "Slice 08 supports only enum_item_id widget values.");
        require(
            binding.widget_class == "frog.widgets.enum_control" || binding.widget_class == "frog.widgets.enum_indicator",
            "Unsupported widget class " + binding.widget_class + ".");
    }

    return current_unit;
}

std::map<std::string, WidgetState> Slice08EnumRuntimeCore::build_widgets() const {
    std::map<std::string, const PanelWidget*> panel_widgets;
    for (const auto& widget : panel.widgets) {
        panel_widgets.emplace(widget.instance_id, &widget);
    }
    std::map<std::string, const PanelWidget*> realization_default_widgets;
    if (!package.front_panels.empty()) {
        for (const auto& widget : package.front_panels.front().widgets) {
            realization_default_widgets.emplace(widget.instance_id, &widget);
        }
    }

    std::map<std::string, WidgetState> result;
    for (const auto& binding : unit.ui_binding.widgets) {
        const auto* panel_widget = panel_widgets.at(binding.widget_id);
        const auto default_it = realization_default_widgets.find(binding.widget_id);
        Object visual;
        Object properties;
        if (default_it != realization_default_widgets.end()) {
            visual = default_it->second->visual;
            properties = default_it->second->props;
        }
        for (const auto& entry : panel_widget->visual) {
            visual[entry.first] = entry.second;
        }
        for (const auto& entry : panel_widget->props) {
            properties[entry.first] = entry.second;
        }
        std::optional<std::string> asset_id;
        std::filesystem::path asset_path;
        if (const auto visual_it = visual.find("asset_ref"); visual_it != visual.end() && visual_it->second.is_string()) {
            const std::string& asset_ref = visual_it->second.as_string();
            if (asset_ref.rfind("asset:", 0) == 0) {
                asset_id = asset_ref.substr(6);
                const auto asset_it = asset_map.find(*asset_id);
                if (asset_it != asset_map.end()) {
                    asset_path = asset_it->second;
                }
            }
        }
        require(asset_id.has_value(), "Enum widget " + binding.widget_id + " must reference a .wfrog SVG asset.");
        require(!asset_path.empty() && std::filesystem::exists(asset_path), "Enum widget " + binding.widget_id + " asset path must exist.");

        enum_items_from_properties(properties, binding.widget_id);
        properties.emplace("value", properties.count("value") ? properties.at("value") : Value("idle"));
        properties.emplace("caption.text", properties.count("caption.text") ? properties.at("caption.text") : Value(binding.widget_id));
        properties.emplace("interaction.enabled", properties.count("interaction.enabled") ? properties.at("interaction.enabled") : Value(binding.role == "control"));
        properties.emplace("interaction.read_only", properties.count("interaction.read_only") ? properties.at("interaction.read_only") : Value(binding.role != "control"));
        properties.emplace("realization.variant", properties.count("realization.variant") ? properties.at("realization.variant") : Value("rectangular_ring"));

        result.emplace(
            binding.widget_id,
            WidgetState{
                binding.widget_id,
                binding.widget_class,
                binding.role,
                panel_widget->layout,
                std::move(properties),
                asset_id,
                asset_path,
                {},
            });
    }
    return result;
}

void Slice08EnumRuntimeCore::set_control_value(const std::string& value) {
    const auto items = enum_items_from_properties(widgets.at("mode_input").properties, "mode_input");
    const auto& item = enum_item_by_id(items, value, "mode_value");
    require(item.enabled, "mode_value must resolve to an enabled enum item.");
    widgets.at("mode_input").properties["value"] = Value(value);
}

std::string Slice08EnumRuntimeCore::control_value() const {
    return json_string(widgets.at("mode_input").properties, "value", "");
}

Value Slice08EnumRuntimeCore::execute(std::optional<std::string> control_value_override) {
    if (control_value_override.has_value()) {
        set_control_value(*control_value_override);
    }
    const auto control_items = enum_items_from_properties(widgets.at("mode_input").properties, "mode_input");
    const auto& item = enum_item_by_id(control_items, control_value(), "mode_value");
    require(item.enabled, "mode_value must resolve to an enabled enum item.");
    last_result = item.id;
    widgets.at("mode_result").properties["value"] = Value(last_result);
    return execution_artifact();
}

Value Slice08EnumRuntimeCore::execute_with_native_kernel_bridge(
    const NativeEnumKernelBridge& bridge,
    std::optional<std::string> control_value_override) {
    require(bridge.manifest().source_lowered_unit == "Examples/08_enum_value_roundtrip/main.lowering.json", "Unexpected native enum kernel source lowered unit.");
    if (control_value_override.has_value()) {
        set_control_value(*control_value_override);
    }

    const auto control_items = enum_items_from_properties(widgets.at("mode_input").properties, "mode_input");
    const auto& input_item = enum_item_by_id(control_items, control_value(), "mode_value");
    require(input_item.enabled, "mode_value must resolve to an enabled enum item.");
    const auto result = bridge.run(input_item.numeric_value);
    if (!result.ok) {
        throw std::runtime_error(result.diagnostic.empty() ? "native enum kernel execution failed." : result.diagnostic);
    }

    const auto result_items = enum_items_from_properties(widgets.at("mode_result").properties, "mode_result");
    const auto& output_item = enum_item_by_numeric_value(result_items, result.result_numeric_value, "result_mode");
    last_result = output_item.id;
    widgets.at("mode_result").properties["value"] = Value(last_result);
    return execution_artifact();
}

Value Slice08EnumRuntimeCore::execution_artifact() const {
    Array widget_entries;
    for (const auto& entry : widgets) {
        const auto& widget = entry.second;
        const auto items = enum_items_from_properties(widget.properties, widget.widget_id);
        const auto value = json_string(widget.properties, "value");
        const auto& selected = enum_item_by_id(items, value, widget.widget_id + ".value");
        Object runtime_fields{
            {"value", Value(selected.id)},
            {"selected.text", Value(selected.text)},
            {"selected.numeric_value", Value(static_cast<std::int64_t>(selected.numeric_value))},
            {"label.text", Value(json_string(widget.properties, "label.text"))},
            {"caption.text", Value(json_string(widget.properties, "caption.text"))},
            {"items", enum_items_to_runtime_value(items)},
            {"asset_ref", widget.asset_id.has_value() ? Value("asset:" + *widget.asset_id) : Value(nullptr)},
            {"realization.variant", Value(json_string(widget.properties, "realization.variant", "rectangular_ring"))},
        };
        const auto copy_property = [&](const std::string& key) {
            const auto it = widget.properties.find(key);
            if (it != widget.properties.end()) {
                runtime_fields.emplace(key, it->second);
            }
        };
        copy_property("enum.domain_id");
        copy_property("caption.visible");
        copy_property("caption.anchor.x");
        copy_property("caption.anchor.y");
        copy_property("caption.align.horizontal");
        copy_property("caption.style.text_color");
        copy_property("display.digital_display_visible");
        copy_property("display.increment_buttons_visible");
        copy_property("display.selector_visible");
        copy_property("display.text_overflow_visible");
        copy_property("style.frame.fill_color");
        copy_property("style.frame.border_color");
        copy_property("style.frame.border_width");
        copy_property("style.value_face.fill_color");
        copy_property("style.value_face.fill_color.hover");
        copy_property("style.value_face.border_color");
        copy_property("style.value_face.border_color.hover");
        copy_property("style.value_face.border_width");
        copy_property("style.value_display.color");
        copy_property("style.value_display.font_size");
        copy_property("style.value_display.font_weight");
        copy_property("style.value_display.vertical_offset");
        copy_property("style.selector_face.fill_color");
        copy_property("style.selector_face.fill_color.hover");
        copy_property("style.selector_face.border_color");
        copy_property("style.selector_face.border_color.hover");
        copy_property("style.selector_face.border_width");
        copy_property("style.selector_face.symbol_color");
        copy_property("style.selector_face.symbol_color.hover");
        copy_property("interaction.enabled");
        copy_property("interaction.read_only");

        widget_entries.push_back(make_object({
            {"widget_id", Value(widget.widget_id)},
            {"class_ref", Value(widget.class_ref)},
            {"role", Value(widget.role)},
            {"layout", widget.layout},
            {"runtime", Value(runtime_fields)},
        }));
    }

    const auto control_items = enum_items_from_properties(widgets.at("mode_input").properties, "mode_input");
    const auto result_items = enum_items_from_properties(widgets.at("mode_result").properties, "mode_result");
    const auto& input_item = enum_item_by_id(control_items, control_value(), "mode_value");
    const auto& output_item = enum_item_by_id(result_items, last_result, "result_mode");

    return make_object({
        {"artifact_kind", Value("frog_runtime_execution_result")},
        {"artifact_governance_ref", make_object({{"path", Value("Versioning/Readme.md")}})},
        {"status", Value("ok")},
        {"contract_ref", make_object({
            {"unit_ids", make_array({Value(unit.unit_id)})},
            {"backend_family", Value(contract.backend_family)},
            {"source_ref", make_object({
                {"example_id", Value(contract.source_ref.example_id)},
                {"path", Value(contract.source_ref.path)},
                {"entry_unit", Value(contract.source_ref.entry_unit)},
            })},
        })},
        {"execution_summary", make_object({
            {"mode", Value("enum_value_roundtrip")},
            {"executed_unit", Value(unit.unit_id)},
            {"operation", Value("copy")},
            {"input_mode", Value(input_item.id)},
            {"input_text", Value(input_item.text)},
            {"input_numeric_value", Value(static_cast<std::int64_t>(input_item.numeric_value))},
            {"result_mode", Value(output_item.id)},
            {"result_text", Value(output_item.text)},
            {"result_numeric_value", Value(static_cast<std::int64_t>(output_item.numeric_value))},
        })},
        {"outputs", make_object({
            {"public", make_object({{"result_mode", Value(output_item.id)}})},
            {"ui", make_object({
                {"mode_input", Value(input_item.id)},
                {"mode_result", Value(output_item.id)},
            })},
        })},
        {"ui_runtime", make_object({
            {"panel", make_object({
                {"panel_id", Value(panel.panel_id)},
                {"title", Value(panel.title)},
                {"class_ref", Value(panel.class_ref)},
                {"layout", panel.layout},
            })},
            {"widgets", Value(widget_entries)},
        })},
        {"diagnostics", Value(Array{})},
    });
}

} // namespace frog::runtime
