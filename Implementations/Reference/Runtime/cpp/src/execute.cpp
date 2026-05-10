#include "execute.hpp"

#include <stdexcept>
#include <utility>

#include "runtime.hpp"

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

Value object(std::initializer_list<std::pair<const std::string, Value>> fields) {
    return Value(Object(fields));
}

Value array(std::initializer_list<Value> values) {
    return Value(Array(values));
}

Value field_or_null(const Object& object, const std::string& key) {
    const auto it = object.find(key);
    return it == object.end() ? Value(nullptr) : it->second;
}

const Object& as_object(const Value& value, const std::string& label) {
    require(value.is_object(), label + " must be an object.");
    return value.as_object();
}

const Array& as_array(const Value& value, const std::string& label) {
    require(value.is_array(), label + " must be an array.");
    return value.as_array();
}

const Object& single_unit(const Value& contract) {
    const auto& root = as_object(contract, "contract");
    require(root.at("artifact_kind").as_string() == "frog_backend_contract", "contract artifact_kind must be frog_backend_contract.");
    const auto& units = as_array(root.at("units"), "contract.units");
    require(units.size() == 1, "contract must contain exactly one unit.");
    return as_object(units.front(), "contract.units[0]");
}

std::string contract_example_id(const Value& contract) {
    const auto& root = as_object(contract, "contract");
    if (const auto direct = root.find("example_id"); direct != root.end() && direct->second.is_string()) {
        return direct->second.as_string();
    }
    const auto& source_ref = as_object(root.at("source_ref"), "contract.source_ref");
    return source_ref.at("example_id").as_string();
}

const Value& public_input(const Value& case_value, const std::string& key) {
    const auto& inputs = as_object(as_object(case_value, "case").at("inputs"), "case.inputs");
    return inputs.at(key);
}

const Value& widget_value(const Value& case_value, const std::string& key) {
    const auto& values = as_object(as_object(case_value, "case").at("widget_values"), "case.widget_values");
    return values.at(key);
}

Value source_ref_object(const Value& contract) {
    const auto& root = as_object(contract, "contract");
    if (const auto source_ref = root.find("source_ref"); source_ref != root.end() && source_ref->second.is_object()) {
        return source_ref->second;
    }
    return object({{"example_id", Value(contract_example_id(contract))}});
}

const Object& wfrog_panel(const Value* wfrog) {
    require(wfrog != nullptr, "runtime slice requires a .wfrog package.");
    const auto& root = as_object(*wfrog, "wfrog");
    require(root.at("format").as_string() == "frog.wfrog", "Expected frog.wfrog package.");
    const auto& panels = as_array(root.at("front_panels"), "wfrog.front_panels");
    require(panels.size() == 1, "Expected exactly one front panel.");
    return as_object(panels.front(), "wfrog.front_panels[0]");
}

Object widget_map(const Object& panel) {
    Object result;
    for (const auto& widget : as_array(panel.at("widgets"), "wfrog.front_panels[0].widgets")) {
        const auto& entry = as_object(widget, "wfrog widget");
        result.emplace(entry.at("instance_id").as_string(), widget);
    }
    return result;
}

Value execute_pure_addition_case(const Value& contract, const Object&, const Value& case_value) {
    const double a = public_input(case_value, "a").as_f64();
    const double b = public_input(case_value, "b").as_f64();
    return object({
        {"artifact_kind", Value("frog_reference_runtime_snapshot")},
        {"example_id", Value(contract_example_id(contract))},
        {"status", Value("ok")},
        {"inputs", as_object(case_value, "case").at("inputs")},
        {"public_outputs", object({{"result", Value(a + b)}})},
    });
}

Value execute_ui_value_roundtrip_case(const Value& contract, const Object&, const Value& case_value) {
    const double a = widget_value(case_value, "ctrl_a").as_f64();
    const double b = widget_value(case_value, "ctrl_b").as_f64();
    Object values = as_object(as_object(case_value, "case").at("widget_values"), "case.widget_values");
    values["ind_result"] = Value(a + b);
    return object({
        {"artifact_kind", Value("frog_reference_runtime_snapshot")},
        {"example_id", Value(contract_example_id(contract))},
        {"status", Value("ok")},
        {"widget_values", Value(values)},
    });
}

Value execute_ui_property_write_case(const Value& contract, const Object& unit, const Value& case_value) {
    const auto& effects = as_array(unit.at("effects"), "unit.effects");
    Array observed;
    Object widget_state;
    for (const auto& item : effects) {
        const auto& effect = as_object(item, "unit.effects[]");
        require(effect.at("op").as_string() == "frog.ui.property_write", "Only frog.ui.property_write is supported.");
        const auto widget_id = effect.at("widget_id").as_string();
        const auto member = effect.at("member").as_string();
        const auto value_source = effect.at("value_source").as_string();
        require(value_source.rfind("public_input.", 0) == 0, "Unsupported property write value source.");
        const auto& value = public_input(case_value, value_source.substr(std::string("public_input.").size()));
        Object member_state;
        if (auto existing = widget_state.find(widget_id); existing != widget_state.end() && existing->second.is_object()) {
            member_state = existing->second.as_object();
        }
        member_state[member] = value;
        widget_state[widget_id] = Value(member_state);
        observed.push_back(object({
            {"op", Value("frog.ui.property_write")},
            {"widget_id", Value(widget_id)},
            {"member", Value(member)},
            {"value", value},
        }));
    }
    return object({
        {"artifact_kind", Value("frog_reference_runtime_snapshot")},
        {"example_id", Value(contract_example_id(contract))},
        {"status", Value("ok")},
        {"public_inputs", as_object(case_value, "case").at("inputs")},
        {"widget_state", Value(widget_state)},
        {"effects", Value(observed)},
    });
}

Value execute_stateful_feedback_case(const Value& contract, const Object& unit, const Value& case_value) {
    const auto& kernel = as_object(unit.at("execution_kernel"), "unit.execution_kernel");
    const auto state_id = kernel.at("state_id").as_string();
    const double initial_state = kernel.at("initial_state").as_f64();
    const double x = public_input(case_value, "x").as_f64();
    const double state_next = initial_state + x;
    return object({
        {"artifact_kind", Value("frog_reference_runtime_snapshot")},
        {"example_id", Value(contract_example_id(contract))},
        {"status", Value("ok")},
        {"inputs", as_object(case_value, "case").at("inputs")},
        {"initial_state", object({{state_id, Value(initial_state)}})},
        {"public_outputs", object({{"y", Value(state_next)}})},
        {"final_state", object({{state_id, Value(state_next)}})},
    });
}

Value execute_bounded_ui_case(const Value& contract, const Object& unit, const Value& case_value, const Value* wfrog) {
    const auto& panel = wfrog_panel(wfrog);
    const auto input_value = public_input(case_value, "input_value").as_i64();
    require(input_value >= 0 && input_value <= 65535, "final_state must remain in the u16 domain.");
    const auto& kernel = as_object(unit.at("execution_kernel"), "unit.execution_kernel");
    std::int64_t state = kernel.at("initial_state").as_i64();
    const auto iterations = kernel.at("iteration_count").as_i64();
    for (std::int64_t index = 0; index < iterations; ++index) {
        state += input_value;
        require(state <= 65535, "final_state must remain in the u16 domain.");
    }

    Object properties;
    Array applied;
    for (const auto& item : as_array(unit.at("effects"), "unit.effects")) {
        const auto& effect = as_object(item, "unit.effects[]");
        const auto widget_id = effect.at("widget_id").as_string();
        const auto member = effect.at("member").as_string();
        const auto& value_object = as_object(effect.at("value"), "effect.value");
        Object widget_props;
        if (auto existing = properties.find(widget_id); existing != properties.end() && existing->second.is_object()) {
            widget_props = existing->second.as_object();
        }
        widget_props[member] = value_object.at("value");
        properties[widget_id] = Value(widget_props);
        applied.push_back(object({{"widget_id", Value(widget_id)}, {"member", Value(member)}, {"value", value_object.at("value")}}));
    }

    Array widgets;
    for (const auto& widget_value_entry : as_array(panel.at("widgets"), "panel.widgets")) {
        const auto& widget = as_object(widget_value_entry, "panel widget");
        const auto widget_id = widget.at("instance_id").as_string();
        const auto& props = as_object(widget.at("props"), "widget.props");
        const auto& visual = as_object(widget.at("visual"), "widget.visual");
        const auto runtime_value = widget_id == "ctrl_input" ? input_value : (widget_id == "ind_result" ? state : props.at("value").as_i64());
        const auto default_foreground = props.find("foreground_color");
        std::string foreground = default_foreground != props.end() && default_foreground->second.is_string()
            ? default_foreground->second.as_string()
            : "";
        if (auto props_it = properties.find(widget_id); props_it != properties.end() && props_it->second.is_object()) {
            if (auto fg = props_it->second.as_object().find("foreground_color"); fg != props_it->second.as_object().end() && fg->second.is_string()) {
                foreground = fg->second.as_string();
            }
        }
        widgets.push_back(object({
            {"widget_id", Value(widget_id)},
            {"class_ref", widget.at("class_ref")},
            {"role", widget.at("role")},
            {"layout", widget.at("layout")},
            {"runtime", object({
                {"value", Value(runtime_value)},
                {"label", field_or_null(props, "label")},
                {"visible", field_or_null(props, "visible")},
                {"enabled", field_or_null(props, "enabled")},
                {"foreground_color", Value(foreground)},
                {"asset_ref", field_or_null(visual, "asset_ref")},
            })},
        }));
    }

    return object({
        {"artifact_kind", Value("frog_runtime_execution_result")},
        {"artifact_governance_ref", object({{"path", Value("Versioning/Readme.md")}})},
        {"status", Value("ok")},
        {"contract_ref", object({{"unit_ids", array({unit.at("unit_id")})}, {"backend_family", as_object(contract, "contract").at("backend_family")}, {"source_ref", source_ref_object(contract)}})},
        {"execution_summary", object({{"mode", Value("contract_and_wfrog")}, {"executed_unit", unit.at("unit_id")}, {"iterations", Value(iterations)}, {"state_initialized", Value(true)}, {"initial_state", kernel.at("initial_state")}, {"final_state", Value(state)}})},
        {"outputs", object({{"public", object({{"result", Value(state)}})}, {"ui", object({{"ctrl_input", Value(input_value)}, {"ind_result", Value(state)}})}})},
        {"ui_runtime", object({{"panel", object({{"panel_id", panel.at("panel_id")}, {"title", panel.at("title")}, {"class_ref", panel.at("class_ref")}, {"layout", panel.at("layout")}})}, {"widgets", Value(widgets)}, {"applied_widget_references", Value(applied)}})},
        {"diagnostics", Value(Array{})},
    });
}

Value execute_boolean_case(const Value& contract, const Object& unit, const Value& case_value, const Value* wfrog) {
    const auto& panel = wfrog_panel(wfrog);
    const auto widgets_by_id = widget_map(panel);
    const bool input_value = as_object(case_value, "case").at("input_value").as_bool();

    auto runtime_for = [&](const std::string& widget_id) {
        const auto& widget = as_object(widgets_by_id.at(widget_id), "boolean widget");
        const auto& props = as_object(widget.at("props"), "boolean widget props");
        const auto& visual = as_object(widget.at("visual"), "boolean widget visual");
        Object runtime{
            {"value", Value(input_value)},
            {"label.text", props.at("label.text")},
            {"caption.text", props.at("caption.text")},
            {"state_text.true_text", props.at("state_text.true_text")},
            {"state_text.false_text", props.at("state_text.false_text")},
            {"asset_ref", visual.at("asset_ref")},
            {"realization.variant", props.at("realization.variant")},
        };
        const auto copy_prop = [&](const std::string& key) {
            const auto it = props.find(key);
            if (it != props.end()) {
                runtime.emplace(key, it->second);
            }
        };
        copy_prop("state_text.style.text_color.false");
        copy_prop("state_text.style.text_color.true");
        copy_prop("state_text.visible");
        copy_prop("caption.align.horizontal");
        copy_prop("style.frame.visible");
        copy_prop("style.outer.border_color.false");
        copy_prop("style.outer.border_color.true");
        copy_prop("style.outer.border_color.hover_false");
        copy_prop("style.outer.border_color.hover_true");
        copy_prop("style.outer.border_color.pressed_false");
        copy_prop("style.outer.border_color.pressed_true");
        copy_prop("style.inner.fill_color.false");
        copy_prop("style.inner.fill_color.true");
        copy_prop("style.inner.fill_color.hover_false");
        copy_prop("style.inner.fill_color.hover_true");
        copy_prop("style.inner.fill_color.pressed_false");
        copy_prop("style.inner.fill_color.pressed_true");
        copy_prop("style.inner.border_color.false");
        copy_prop("style.inner.border_color.true");
        copy_prop("style.inner.border_color.hover_false");
        copy_prop("style.inner.border_color.hover_true");
        copy_prop("style.inner.border_color.pressed_false");
        copy_prop("style.inner.border_color.pressed_true");
        copy_prop("style.inner.left");
        copy_prop("style.inner.top");
        copy_prop("style.inner.width");
        copy_prop("style.inner.height");
        copy_prop("style.pressed.inset");
        copy_prop("style.transition.duration_ms");
        copy_prop("style.transition.timing");
        return Value(runtime);
    };

    return object({
        {"artifact_kind", Value("frog_runtime_execution_result")},
        {"artifact_governance_ref", object({{"path", Value("Versioning/Readme.md")}})},
        {"status", Value("ok")},
        {"contract_ref", object({{"unit_ids", array({unit.at("unit_id")})}, {"backend_family", as_object(contract, "contract").at("backend_family")}, {"source_ref", source_ref_object(contract)}})},
        {"execution_summary", object({{"mode", Value("boolean_value_roundtrip")}, {"executed_unit", unit.at("unit_id")}, {"operation", Value("copy")}, {"input_value", Value(input_value)}, {"result", Value(input_value)}})},
        {"outputs", object({{"public", object({{"result", Value(input_value)}})}, {"ui", object({{"bool_input", Value(input_value)}, {"bool_result", Value(input_value)}})}})},
        {"ui_runtime", object({
            {"panel", object({{"panel_id", panel.at("panel_id")}, {"title", panel.at("title")}, {"class_ref", panel.at("class_ref")}, {"layout", panel.at("layout")}})},
            {"widgets", array({
                object({{"widget_id", Value("bool_input")}, {"class_ref", as_object(widgets_by_id.at("bool_input"), "bool_input").at("class_ref")}, {"role", Value("control")}, {"layout", as_object(widgets_by_id.at("bool_input"), "bool_input").at("layout")}, {"runtime", runtime_for("bool_input")}}),
                object({{"widget_id", Value("bool_result")}, {"class_ref", as_object(widgets_by_id.at("bool_result"), "bool_result").at("class_ref")}, {"role", Value("indicator")}, {"layout", as_object(widgets_by_id.at("bool_result"), "bool_result").at("layout")}, {"runtime", runtime_for("bool_result")}}),
            })},
        })},
        {"diagnostics", Value(Array{})},
    });
}

} // namespace

frog::json::Value execute_contract(
    std::uint16_t input_value,
    std::optional<std::filesystem::path> contract_path,
    std::optional<std::filesystem::path> wfrog_path) {
    Slice05RuntimeCore runtime(
        contract_path.value_or(default_contract_path()),
        wfrog_path.value_or(default_wfrog_path()));
    return runtime.execute(input_value);
}

frog::json::Value execute_boolean_contract(
    bool input_value,
    std::filesystem::path contract_path,
    std::filesystem::path wfrog_path) {
    Slice06BooleanRuntimeCore runtime(std::move(contract_path), std::move(wfrog_path));
    return runtime.execute(input_value);
}

frog::json::Value execute_reference_contract_case(
    const frog::json::Value& contract,
    const frog::json::Value& case_value,
    const frog::json::Value* wfrog) {
    const auto& unit = single_unit(contract);
    const auto kind = unit.at("kind").as_string();
    if (kind == "pure_addition_kernel") {
        return execute_pure_addition_case(contract, unit, case_value);
    }
    if (kind == "ui_value_roundtrip_kernel") {
        return execute_ui_value_roundtrip_case(contract, unit, case_value);
    }
    if (kind == "ui_property_write_effect_unit") {
        return execute_ui_property_write_case(contract, unit, case_value);
    }
    if (kind == "stateful_feedback_delay_kernel") {
        return execute_stateful_feedback_case(contract, unit, case_value);
    }
    if (kind == "bounded_executable_ui_unit") {
        return execute_bounded_ui_case(contract, unit, case_value, wfrog);
    }
    if (kind == "boolean_value_roundtrip_ui_unit") {
        return execute_boolean_case(contract, unit, case_value, wfrog);
    }
    throw std::runtime_error("unsupported contract unit kind: " + kind);
}

frog::json::Value execute_reference_acceptance(
    const frog::json::Value& acceptance,
    const frog::json::Value& contract,
    const frog::json::Value* wfrog) {
    const auto& unit = single_unit(contract);
    const auto kind = unit.at("kind").as_string();
    const auto& root = as_object(acceptance, "acceptance");
    if (kind == "bounded_executable_ui_unit") {
        const auto& headless = as_object(root.at("headless"), "acceptance.headless");
        const auto input_value = headless.at("input_value").as_i64();
        return execute_reference_contract_case(
            contract,
            object({{"inputs", object({{"input_value", Value(input_value)}})}, {"widget_values", object({{"ctrl_input", Value(input_value)}})}}),
            wfrog);
    }
    if (kind == "boolean_value_roundtrip_ui_unit") {
        const auto& headless = as_object(root.at("headless"), "acceptance.headless");
        return execute_reference_contract_case(contract, object({{"input_value", headless.at("input_value")}}), wfrog);
    }
    const auto& cases = as_array(root.at("cases"), "acceptance.cases");
    require(cases.size() == 1, "acceptance must contain exactly one case.");
    return execute_reference_contract_case(contract, cases.front(), wfrog);
}

} // namespace frog::runtime
