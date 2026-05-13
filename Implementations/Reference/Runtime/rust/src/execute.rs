use std::collections::BTreeMap;
use std::fs;
use std::path::{Path, PathBuf};

use serde_json::{json, Map, Value};

use crate::contract::{default_contract_path, default_wfrog_path, find_repo_root};
use crate::diagnostics::{ensure, Result, RuntimeError};

pub fn execute_contract(
    input_value: u16,
    contract_path: Option<PathBuf>,
    wfrog_path: Option<PathBuf>,
) -> Result<Value> {
    let contract_path = contract_path.unwrap_or(default_contract_path()?);
    let wfrog_path = wfrog_path.unwrap_or(default_wfrog_path()?);
    let contract: Value = serde_json::from_str(&fs::read_to_string(contract_path)?)?;
    let wfrog: Value = serde_json::from_str(&fs::read_to_string(wfrog_path)?)?;
    execute_reference_contract_case(
        &contract,
        &json!({"inputs": {"input_value": input_value}, "widget_values": {"ctrl_input": input_value}}),
        Some(&wfrog),
    )
}

fn message(text: impl Into<String>) -> RuntimeError {
    RuntimeError::Message(text.into())
}

fn object<'a>(value: &'a Value, name: &str) -> Result<&'a Map<String, Value>> {
    value
        .as_object()
        .ok_or_else(|| message(format!("{name} must be an object.")))
}

fn array<'a>(value: &'a Value, name: &str) -> Result<&'a Vec<Value>> {
    value
        .as_array()
        .ok_or_else(|| message(format!("{name} must be an array.")))
}

fn single_unit(contract: &Value) -> Result<&Map<String, Value>> {
    let root = object(contract, "contract")?;
    ensure(
        root.get("artifact_kind").and_then(Value::as_str) == Some("frog_backend_contract"),
        "contract artifact_kind must be frog_backend_contract.",
    )?;
    let units = array(
        root.get("units")
            .ok_or_else(|| message("contract.units is required."))?,
        "contract.units",
    )?;
    ensure(units.len() == 1, "contract must contain exactly one unit.")?;
    object(&units[0], "contract.units[0]")
}

fn contract_example_id(contract: &Value) -> Result<String> {
    let root = object(contract, "contract")?;
    if let Some(example_id) = root.get("example_id").and_then(Value::as_str) {
        return Ok(example_id.to_string());
    }
    let source_ref = object(
        root.get("source_ref")
            .ok_or_else(|| message("contract.source_ref is required."))?,
        "contract.source_ref",
    )?;
    Ok(source_ref
        .get("example_id")
        .and_then(Value::as_str)
        .ok_or_else(|| message("contract source_ref.example_id is required."))?
        .to_string())
}

fn public_input<'a>(case_value: &'a Value, key: &str) -> Result<&'a Value> {
    let inputs = object(
        object(case_value, "case")?
            .get("inputs")
            .ok_or_else(|| message("case.inputs is required."))?,
        "case.inputs",
    )?;
    inputs
        .get(key)
        .ok_or_else(|| message(format!("missing public input: {key}")))
}

fn widget_value<'a>(case_value: &'a Value, key: &str) -> Result<&'a Value> {
    let values = object(
        object(case_value, "case")?
            .get("widget_values")
            .ok_or_else(|| message("case.widget_values is required."))?,
        "case.widget_values",
    )?;
    values
        .get(key)
        .ok_or_else(|| message(format!("missing widget value: {key}")))
}

fn source_ref(contract: &Value) -> Result<Value> {
    let root = object(contract, "contract")?;
    if let Some(value) = root.get("source_ref") {
        return Ok(value.clone());
    }
    Ok(json!({"example_id": contract_example_id(contract)?}))
}

fn normalize_source_front_panel(source: &Value) -> Result<Value> {
    let root = object(source, "source")?;
    let metadata = root.get("metadata").and_then(Value::as_object);
    let panel = object(root.get("front_panel").ok_or_else(|| message("source.front_panel is required."))?, "source.front_panel")?;
    let title = panel
        .get("title")
        .and_then(Value::as_str)
        .or_else(|| metadata.and_then(|item| item.get("summary").or_else(|| item.get("name"))).and_then(Value::as_str))
        .unwrap_or("FROG Front Panel");
    let panel_id = panel
        .get("panel_id")
        .and_then(Value::as_str)
        .map(ToString::to_string)
        .or_else(|| metadata.and_then(|item| item.get("name")).and_then(Value::as_str).map(|name| format!("{name}_panel")))
        .unwrap_or_else(|| "frog_panel".to_string());
    let mut widgets = Vec::new();
    for widget_value in array(panel.get("widgets").ok_or_else(|| message("source.front_panel.widgets is required."))?, "source.front_panel.widgets")? {
        let widget = object(widget_value, "source front-panel widget")?;
        let instance_id = widget
            .get("instance_ref")
            .or_else(|| widget.get("instance_id"))
            .or_else(|| widget.get("id"))
            .and_then(Value::as_str)
            .ok_or_else(|| message("source front-panel widget must expose id/instance_ref."))?;
        let mut entry = widget.clone();
        entry.insert("instance_id".to_string(), Value::String(instance_id.to_string()));
        entry.entry("layout".to_string()).or_insert_with(|| json!({}));
        entry.entry("props".to_string()).or_insert_with(|| json!({}));
        entry.entry("visual".to_string()).or_insert_with(|| json!({}));
        widgets.push(Value::Object(entry));
    }
    Ok(json!({
        "panel_id": panel_id,
        "title": title,
        "class_ref": panel.get("class_ref").and_then(Value::as_str).unwrap_or("frog.front_panel"),
        "layout": panel.get("canvas").or_else(|| panel.get("layout")).cloned().unwrap_or_else(|| json!({})),
        "widgets": widgets,
        "host_binding_ref": panel.get("host_binding_ref").and_then(Value::as_str).unwrap_or("reference_host_default")
    }))
}

fn source_panel(contract: &Value) -> Result<Value> {
    let root = object(contract, "contract")?;
    let source_ref = object(root.get("source_ref").ok_or_else(|| message("contract.source_ref is required."))?, "contract.source_ref")?;
    let source_path = source_ref.get("path").and_then(Value::as_str).ok_or_else(|| message("contract.source_ref.path is required."))?;
    let mut path = PathBuf::from(source_path);
    if !path.is_absolute() {
        path = find_repo_root(Path::new(env!("CARGO_MANIFEST_DIR")))?.join(path);
    }
    let source: Value = serde_json::from_str(&fs::read_to_string(path)?)?;
    normalize_source_front_panel(&source)
}

fn wfrog_panel(wfrog: Option<&Value>) -> Result<Value> {
    let wfrog = wfrog.ok_or_else(|| message("runtime slice requires a .wfrog package."))?;
    let root = object(wfrog, "wfrog")?;
    ensure(root.get("format").and_then(Value::as_str) == Some("frog.wfrog"), "Expected frog.wfrog package.")?;
    let panels = array(
        root.get("front_panels")
            .ok_or_else(|| message("wfrog.front_panels is not published by this realization package."))?,
        "wfrog.front_panels",
    )?;
    ensure(panels.len() == 1, "Expected exactly one front panel.")?;
    Ok(panels[0].clone())
}

fn runtime_panel(contract: &Value, wfrog: Option<&Value>) -> Result<Value> {
    if let Some(wfrog_value) = wfrog {
        if object(wfrog_value, "wfrog")?.get("front_panels").is_some() {
            return wfrog_panel(Some(wfrog_value));
        }
    }
    source_panel(contract)
}

fn widget_map(panel: &Map<String, Value>) -> Result<BTreeMap<String, Value>> {
    let widgets = array(
        panel
            .get("widgets")
            .ok_or_else(|| message("front panel widgets are required."))?,
        "front_panel.widgets",
    )?;
    let mut result = BTreeMap::new();
    for widget in widgets {
        let entry = object(widget, "front panel widget")?;
        let id = entry
            .get("instance_id")
            .and_then(Value::as_str)
            .ok_or_else(|| message("front panel widget instance_id is required."))?;
        result.insert(id.to_string(), widget.clone());
    }
    Ok(result)
}

fn execute_pure_addition_case(contract: &Value, case_value: &Value) -> Result<Value> {
    let a = public_input(case_value, "a")?.as_f64().ok_or_else(|| message("a must be numeric."))?;
    let b = public_input(case_value, "b")?.as_f64().ok_or_else(|| message("b must be numeric."))?;
    Ok(json!({
        "artifact_kind": "frog_reference_runtime_snapshot",
        "example_id": contract_example_id(contract)?,
        "status": "ok",
        "inputs": object(case_value, "case")?.get("inputs").cloned().unwrap_or(Value::Null),
        "public_outputs": {"result": a + b}
    }))
}

fn execute_ui_value_roundtrip_case(contract: &Value, case_value: &Value) -> Result<Value> {
    let a = widget_value(case_value, "ctrl_a")?.as_f64().ok_or_else(|| message("ctrl_a must be numeric."))?;
    let b = widget_value(case_value, "ctrl_b")?.as_f64().ok_or_else(|| message("ctrl_b must be numeric."))?;
    let mut values = object(
        object(case_value, "case")?
            .get("widget_values")
            .ok_or_else(|| message("case.widget_values is required."))?,
        "case.widget_values",
    )?
    .clone();
    values.insert("ind_result".to_string(), json!(a + b));
    Ok(json!({
        "artifact_kind": "frog_reference_runtime_snapshot",
        "example_id": contract_example_id(contract)?,
        "status": "ok",
        "widget_values": values
    }))
}

fn execute_ui_property_write_case(contract: &Value, unit: &Map<String, Value>, case_value: &Value) -> Result<Value> {
    let effects = array(
        unit.get("effects").ok_or_else(|| message("unit.effects is required."))?,
        "unit.effects",
    )?;
    let mut widget_state = Map::new();
    let mut observed = Vec::new();
    for effect_value in effects {
        let effect = object(effect_value, "unit.effects[]")?;
        ensure(effect.get("op").and_then(Value::as_str) == Some("frog.ui.property_write"), "Only frog.ui.property_write is supported.")?;
        let widget_id = effect.get("widget_id").and_then(Value::as_str).ok_or_else(|| message("effect.widget_id is required."))?;
        let member = effect.get("member").and_then(Value::as_str).ok_or_else(|| message("effect.member is required."))?;
        let source = effect.get("value_source").and_then(Value::as_str).ok_or_else(|| message("effect.value_source is required."))?;
        let public_id = source
            .strip_prefix("public_input.")
            .ok_or_else(|| message("Unsupported property write value source."))?;
        let value = public_input(case_value, public_id)?.clone();
        let mut members = widget_state
            .get(widget_id)
            .and_then(Value::as_object)
            .cloned()
            .unwrap_or_default();
        members.insert(member.to_string(), value.clone());
        widget_state.insert(widget_id.to_string(), Value::Object(members));
        observed.push(json!({"op": "frog.ui.property_write", "widget_id": widget_id, "member": member, "value": value}));
    }
    Ok(json!({
        "artifact_kind": "frog_reference_runtime_snapshot",
        "example_id": contract_example_id(contract)?,
        "status": "ok",
        "public_inputs": object(case_value, "case")?.get("inputs").cloned().unwrap_or(Value::Null),
        "widget_state": widget_state,
        "effects": observed
    }))
}

fn execute_stateful_feedback_case(contract: &Value, unit: &Map<String, Value>, case_value: &Value) -> Result<Value> {
    let kernel = object(
        unit.get("execution_kernel")
            .ok_or_else(|| message("unit.execution_kernel is required."))?,
        "unit.execution_kernel",
    )?;
    let state_id = kernel.get("state_id").and_then(Value::as_str).ok_or_else(|| message("state_id is required."))?;
    let initial = kernel.get("initial_state").and_then(Value::as_f64).ok_or_else(|| message("initial_state must be numeric."))?;
    let x = public_input(case_value, "x")?.as_f64().ok_or_else(|| message("x must be numeric."))?;
    let next = initial + x;
    Ok(json!({
        "artifact_kind": "frog_reference_runtime_snapshot",
        "example_id": contract_example_id(contract)?,
        "status": "ok",
        "inputs": object(case_value, "case")?.get("inputs").cloned().unwrap_or(Value::Null),
        "initial_state": {state_id: initial},
        "public_outputs": {"y": next},
        "final_state": {state_id: next}
    }))
}

fn execute_bounded_ui_case(contract: &Value, unit: &Map<String, Value>, case_value: &Value, wfrog: Option<&Value>) -> Result<Value> {
    let panel_value = runtime_panel(contract, wfrog)?;
    let panel = object(&panel_value, "front_panel")?;
    let input_value = public_input(case_value, "input_value")?
        .as_u64()
        .ok_or_else(|| message("input_value must be a u16 integer."))?;
    ensure(input_value <= u16::MAX as u64, "final_state must remain in the u16 domain.")?;
    let kernel = object(unit.get("execution_kernel").ok_or_else(|| message("unit.execution_kernel is required."))?, "unit.execution_kernel")?;
    let mut state = kernel.get("initial_state").and_then(Value::as_u64).ok_or_else(|| message("initial_state must be integer."))?;
    let iterations = kernel.get("iteration_count").and_then(Value::as_u64).ok_or_else(|| message("iteration_count must be integer."))?;
    for _ in 0..iterations {
        state += input_value;
        ensure(state <= u16::MAX as u64, "final_state must remain in the u16 domain.")?;
    }

    let mut property_map: BTreeMap<(String, String), Value> = BTreeMap::new();
    let mut applied = Vec::new();
    for effect_value in array(unit.get("effects").ok_or_else(|| message("unit.effects is required."))?, "unit.effects")? {
        let effect = object(effect_value, "unit.effects[]")?;
        let widget_id = effect.get("widget_id").and_then(Value::as_str).ok_or_else(|| message("effect.widget_id is required."))?;
        let member = effect.get("member").and_then(Value::as_str).ok_or_else(|| message("effect.member is required."))?;
        let value = object(effect.get("value").ok_or_else(|| message("effect.value is required."))?, "effect.value")?
            .get("value")
            .cloned()
            .unwrap_or(Value::Null);
        property_map.insert((widget_id.to_string(), member.to_string()), value.clone());
        applied.push(json!({"widget_id": widget_id, "member": member, "value": value}));
    }

    let widgets = array(panel.get("widgets").ok_or_else(|| message("panel.widgets is required."))?, "panel.widgets")?
        .iter()
        .map(|widget_value| {
            let widget = object(widget_value, "panel widget")?;
            let id = widget.get("instance_id").and_then(Value::as_str).ok_or_else(|| message("widget.instance_id is required."))?;
            let props = object(widget.get("props").ok_or_else(|| message("widget.props is required."))?, "widget.props")?;
            let visual = object(widget.get("visual").ok_or_else(|| message("widget.visual is required."))?, "widget.visual")?;
            let value = if id == "ctrl_input" {
                json!(input_value)
            } else if id == "ind_result" {
                json!(state)
            } else {
                props.get("value").cloned().unwrap_or(Value::Null)
            };
            let foreground = property_map
                .get(&(id.to_string(), "foreground_color".to_string()))
                .cloned()
                .or_else(|| props.get("foreground_color").cloned())
                .unwrap_or(Value::Null);
            Ok(json!({
                "widget_id": id,
                "class_ref": widget.get("class_ref").cloned().unwrap_or(Value::Null),
                "role": widget.get("role").cloned().unwrap_or(Value::Null),
                "layout": widget.get("layout").cloned().unwrap_or(Value::Null),
                "runtime": {
                    "value": value,
                    "label": props.get("label").cloned().unwrap_or(Value::Null),
                    "visible": props.get("visible").cloned().unwrap_or(Value::Null),
                    "enabled": props.get("enabled").cloned().unwrap_or(Value::Null),
                    "foreground_color": foreground,
                    "asset_ref": visual.get("asset_ref").cloned().unwrap_or(Value::Null)
                }
            }))
        })
        .collect::<Result<Vec<Value>>>()?;

    Ok(json!({
        "artifact_kind": "frog_runtime_execution_result",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "status": "ok",
        "contract_ref": {"unit_ids": [unit.get("unit_id").cloned().unwrap_or(Value::Null)], "backend_family": object(contract, "contract")?.get("backend_family").cloned().unwrap_or(Value::Null), "source_ref": source_ref(contract)?},
        "execution_summary": {"mode": "contract_and_wfrog", "executed_unit": unit.get("unit_id").cloned().unwrap_or(Value::Null), "iterations": iterations, "state_initialized": true, "initial_state": kernel.get("initial_state").cloned().unwrap_or(Value::Null), "final_state": state},
        "outputs": {"public": {"result": state}, "ui": {"ctrl_input": input_value, "ind_result": state}},
        "ui_runtime": {"panel": {"panel_id": panel.get("panel_id").cloned().unwrap_or(Value::Null), "title": panel.get("title").cloned().unwrap_or(Value::Null), "class_ref": panel.get("class_ref").cloned().unwrap_or(Value::Null), "layout": panel.get("layout").cloned().unwrap_or(Value::Null)}, "widgets": widgets, "applied_widget_references": applied},
        "diagnostics": []
    }))
}

fn execute_boolean_case(contract: &Value, unit: &Map<String, Value>, case_value: &Value, wfrog: Option<&Value>) -> Result<Value> {
    let panel_value = runtime_panel(contract, wfrog)?;
    let panel = object(&panel_value, "front_panel")?;
    let widgets = widget_map(panel)?;
    let input_value = object(case_value, "case")?
        .get("input_value")
        .and_then(Value::as_bool)
        .ok_or_else(|| message("input_value must be boolean."))?;
    let runtime_for = |widget_id: &str| -> Result<Value> {
        let widget = object(widgets.get(widget_id).ok_or_else(|| message(format!("missing widget {widget_id}")))? , widget_id)?;
        let props = object(widget.get("props").ok_or_else(|| message("widget.props is required."))?, "widget.props")?;
        let visual = object(widget.get("visual").ok_or_else(|| message("widget.visual is required."))?, "widget.visual")?;
        let mut runtime = json!({
            "value": input_value,
            "label.text": props.get("label.text").cloned().unwrap_or(Value::Null),
            "caption.text": props.get("caption.text").cloned().unwrap_or(Value::Null),
            "state_text.true_text": props.get("state_text.true_text").cloned().unwrap_or(Value::Null),
            "state_text.false_text": props.get("state_text.false_text").cloned().unwrap_or(Value::Null),
            "asset_ref": visual.get("asset_ref").cloned().unwrap_or(Value::Null),
            "realization.variant": props.get("realization.variant").cloned().unwrap_or(Value::Null)
        });
        if let Some(runtime_object) = runtime.as_object_mut() {
            for member in [
                "state_text.style.text_color.false",
                "state_text.style.text_color.true",
                "state_text.visible",
                "caption.visible",
                "caption.anchor.x",
                "caption.anchor.y",
                "caption.align.horizontal",
                "style.frame.visible",
                "style.outer.border_color.false",
                "style.outer.border_color.true",
                "style.outer.border_color.hover_false",
                "style.outer.border_color.hover_true",
                "style.outer.border_color.pressed_false",
                "style.outer.border_color.pressed_true",
                "style.inner.fill_color.false",
                "style.inner.fill_color.true",
                "style.inner.fill_color.hover_false",
                "style.inner.fill_color.hover_true",
                "style.inner.fill_color.pressed_false",
                "style.inner.fill_color.pressed_true",
                "style.inner.border_color.false",
                "style.inner.border_color.true",
                "style.inner.border_color.hover_false",
                "style.inner.border_color.hover_true",
                "style.inner.border_color.pressed_false",
                "style.inner.border_color.pressed_true",
                "style.inner.left",
                "style.inner.top",
                "style.inner.width",
                "style.inner.height",
                "style.focus_ring.visible",
                "style.focus_ring.color",
                "style.focus_ring.width",
                "style.pressed.inset",
                "style.transition.duration_ms",
                "style.transition.timing",
            ] {
                if let Some(value) = props.get(member) {
                    runtime_object.insert(member.to_string(), value.clone());
                }
            }
        }
        Ok(runtime)
    };
    let bool_input = object(widgets.get("bool_input").ok_or_else(|| message("missing bool_input"))?, "bool_input")?;
    let bool_result = object(widgets.get("bool_result").ok_or_else(|| message("missing bool_result"))?, "bool_result")?;
    Ok(json!({
        "artifact_kind": "frog_runtime_execution_result",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "status": "ok",
        "contract_ref": {"unit_ids": [unit.get("unit_id").cloned().unwrap_or(Value::Null)], "backend_family": object(contract, "contract")?.get("backend_family").cloned().unwrap_or(Value::Null), "source_ref": source_ref(contract)?},
        "execution_summary": {"mode": "boolean_value_roundtrip", "executed_unit": unit.get("unit_id").cloned().unwrap_or(Value::Null), "operation": "copy", "input_value": input_value, "result": input_value},
        "outputs": {"public": {"result": input_value}, "ui": {"bool_input": input_value, "bool_result": input_value}},
        "ui_runtime": {
            "panel": {"panel_id": panel.get("panel_id").cloned().unwrap_or(Value::Null), "title": panel.get("title").cloned().unwrap_or(Value::Null), "class_ref": panel.get("class_ref").cloned().unwrap_or(Value::Null), "layout": panel.get("layout").cloned().unwrap_or(Value::Null)},
            "widgets": [
                {"widget_id": "bool_input", "class_ref": bool_input.get("class_ref").cloned().unwrap_or(Value::Null), "role": "control", "layout": bool_input.get("layout").cloned().unwrap_or(Value::Null), "runtime": runtime_for("bool_input")?},
                {"widget_id": "bool_result", "class_ref": bool_result.get("class_ref").cloned().unwrap_or(Value::Null), "role": "indicator", "layout": bool_result.get("layout").cloned().unwrap_or(Value::Null), "runtime": runtime_for("bool_result")?}
            ]
        },
        "diagnostics": []
    }))
}

pub fn execute_reference_contract_case(contract: &Value, case_value: &Value, wfrog: Option<&Value>) -> Result<Value> {
    let unit = single_unit(contract)?;
    let kind = unit
        .get("kind")
        .and_then(Value::as_str)
        .ok_or_else(|| message("unit.kind is required."))?;
    match kind {
        "pure_addition_kernel" => execute_pure_addition_case(contract, case_value),
        "ui_value_roundtrip_kernel" => execute_ui_value_roundtrip_case(contract, case_value),
        "ui_property_write_effect_unit" => execute_ui_property_write_case(contract, unit, case_value),
        "stateful_feedback_delay_kernel" => execute_stateful_feedback_case(contract, unit, case_value),
        "bounded_executable_ui_unit" => execute_bounded_ui_case(contract, unit, case_value, wfrog),
        "boolean_value_roundtrip_ui_unit" => execute_boolean_case(contract, unit, case_value, wfrog),
        _ => Err(message(format!("unsupported contract unit kind: {kind}"))),
    }
}

pub fn execute_reference_acceptance(acceptance: &Value, contract: &Value, wfrog: Option<&Value>) -> Result<Value> {
    let unit = single_unit(contract)?;
    let kind = unit
        .get("kind")
        .and_then(Value::as_str)
        .ok_or_else(|| message("unit.kind is required."))?;
    let acceptance_object = object(acceptance, "acceptance")?;
    if kind == "bounded_executable_ui_unit" {
        let input = object(
            acceptance_object
                .get("headless")
                .ok_or_else(|| message("acceptance.headless is required."))?,
            "acceptance.headless",
        )?
        .get("input_value")
        .cloned()
        .ok_or_else(|| message("acceptance.headless.input_value is required."))?;
        return execute_reference_contract_case(contract, &json!({"inputs": {"input_value": input}, "widget_values": {"ctrl_input": input}}), wfrog);
    }
    if kind == "boolean_value_roundtrip_ui_unit" {
        let input = object(
            acceptance_object
                .get("headless")
                .ok_or_else(|| message("acceptance.headless is required."))?,
            "acceptance.headless",
        )?
        .get("input_value")
        .cloned()
        .ok_or_else(|| message("acceptance.headless.input_value is required."))?;
        return execute_reference_contract_case(contract, &json!({"input_value": input}), wfrog);
    }
    let cases = array(
        acceptance_object
            .get("cases")
            .ok_or_else(|| message("acceptance.cases is required."))?,
        "acceptance.cases",
    )?;
    ensure(cases.len() == 1, "acceptance must contain exactly one case.")?;
    execute_reference_contract_case(contract, &cases[0], wfrog)
}
