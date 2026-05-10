use std::collections::{BTreeMap, HashMap};
use std::path::{Path, PathBuf};

use serde_json::{json, Map, Value};

use crate::contract::{
    load_contract_from_path, load_wfrog_from_path, BackendContract, ContractUnit, FrontPanel, HostBinding,
    PanelWidget, WfrogPackage, EXPECTED_OVERFLOW_BEHAVIOR, REFERENCE_BACKEND_FAMILY,
};
use crate::diagnostics::{ensure, Result, RuntimeError};
use crate::native_kernel::NativeKernelBridge;

const SUPPORTED_WIDGET_CLASSES: &[(&str, &str)] = &[
    ("frog.widgets.numeric_control", "control"),
    ("frog.widgets.numeric_indicator", "indicator"),
];
const SUPPORTED_PROPERTIES: &[&str] = &["value", "label", "visible", "enabled", "foreground_color"];

#[derive(Debug, Clone)]
pub struct WidgetState {
    pub widget_id: String,
    pub class_ref: String,
    pub role: String,
    pub layout: Value,
    pub properties: Map<String, Value>,
    pub asset_id: Option<String>,
    pub asset_path: Option<PathBuf>,
    pub supported_members: Vec<String>,
}

#[derive(Debug, Clone)]
pub struct RuntimeCore {
    pub contract_path: PathBuf,
    pub wfrog_path: PathBuf,
    pub contract: BackendContract,
    pub package: WfrogPackage,
    pub unit: ContractUnit,
    pub panel: FrontPanel,
    pub widgets: BTreeMap<String, WidgetState>,
    pub asset_map: HashMap<String, PathBuf>,
    pub applied_widget_references: Vec<Value>,
    pub last_final_state: u16,
    pub last_error: Option<String>,
}

fn ensure_u16(value: u32, label: &str) -> Result<u16> {
    if value <= u16::MAX as u32 {
        Ok(value as u16)
    } else {
        Err(RuntimeError::Message(format!("{label} must remain in the u16 domain.")))
    }
}

impl RuntimeCore {
    pub fn from_paths(contract_path: impl AsRef<Path>, wfrog_path: impl AsRef<Path>) -> Result<Self> {
        let contract_path = contract_path.as_ref().to_path_buf();
        let wfrog_path = wfrog_path.as_ref().to_path_buf();
        let contract = load_contract_from_path(&contract_path)?;
        let package = load_wfrog_from_path(&wfrog_path)?;
        let unit = validate_contract_and_package(&contract, &package)?;
        let panel = package.front_panels[0].clone();
        let asset_map: HashMap<String, PathBuf> = package
            .svg_assets
            .iter()
            .map(|entry| {
                (
                    entry.asset_id.clone(),
                    wfrog_path.parent().unwrap_or_else(|| Path::new(".")).join(&entry.path),
                )
            })
            .collect();
        let widgets = build_widgets(&unit, &panel, &asset_map)?;
        let mut runtime = Self {
            contract_path,
            wfrog_path,
            contract,
            package,
            unit,
            panel,
            widgets,
            asset_map,
            applied_widget_references: Vec::new(),
            last_final_state: 0,
            last_error: None,
        };
        runtime.apply_contract_property_writes()?;
        Ok(runtime)
    }

    pub fn apply_contract_property_writes(&mut self) -> Result<()> {
        self.applied_widget_references.clear();
        for write in &self.unit.property_writes {
            let widget = self
                .widgets
                .get_mut(&write.widget_id)
                .ok_or_else(|| RuntimeError::Message(format!("Unknown widget {}", write.widget_id)))?;
            ensure(
                widget.supported_members.iter().any(|member| member == &write.member),
                format!(
                    "Unsupported widget reference member {}.{}",
                    write.widget_id, write.member
                ),
            )?;
            widget
                .properties
                .insert(write.member.clone(), Value::String(write.value.value.clone()));
            self.applied_widget_references.push(json!({
                "widget_id": write.widget_id,
                "member": write.member,
                "value": write.value.value,
            }));
        }
        Ok(())
    }

    pub fn set_control_value(&mut self, value: u16) {
        if let Some(widget) = self.widgets.get_mut("ctrl_input") {
            widget.properties.insert("value".to_string(), Value::Number(value.into()));
        }
    }

    pub fn control_value(&self) -> Result<u16> {
        let widget = self
            .widgets
            .get("ctrl_input")
            .ok_or_else(|| RuntimeError::Message("Missing ctrl_input widget.".to_string()))?;
        let value = widget
            .properties
            .get("value")
            .and_then(|item| item.as_u64())
            .unwrap_or(0) as u32;
        ensure_u16(value, "input_value")
    }

    pub fn invoke_method(&mut self, widget_id: &str, method_name: &str) -> Result<()> {
        match (widget_id, method_name) {
            ("ctrl_input", "focus") => Ok(()),
            ("ind_result", "reset_to_default_style") => {
                let defaults = self
                    .panel
                    .widgets
                    .iter()
                    .find(|widget| widget.instance_id == widget_id)
                    .ok_or_else(|| RuntimeError::Message(format!("Unknown widget {}", widget_id)))?;
                let widget = self
                    .widgets
                    .get_mut(widget_id)
                    .ok_or_else(|| RuntimeError::Message(format!("Unknown widget {}", widget_id)))?;
                widget.properties = defaults.props.clone();
                widget
                    .properties
                    .entry("value".to_string())
                    .or_insert(Value::Number(0u16.into()));
                Ok(())
            }
            _ => Err(RuntimeError::Message(format!(
                "Unsupported method invocation: {}.{}",
                widget_id, method_name
            ))),
        }
    }

    pub fn execute(&mut self, control_value: Option<u16>) -> Result<Value> {
        self.apply_contract_property_writes()?;
        if let Some(value) = control_value {
            self.set_control_value(value);
        }
        let input_value = self.control_value()? as u32;
        let iterations = self.unit.execution_model.iteration_count;
        let mut state = self.unit.state_model.carrier.initial_value as u32;
        for _ in 0..iterations {
            state = state + input_value;
            let checked = ensure_u16(state, "final_state")?;
            state = checked as u32;
        }
        let final_state = state as u16;
        self.last_final_state = final_state;
        if let Some(indicator) = self.widgets.get_mut("ind_result") {
            indicator
                .properties
                .insert("value".to_string(), Value::Number(final_state.into()));
        }
        Ok(self.execution_artifact())
    }

    pub fn execute_with_native_kernel_bridge(
        &mut self,
        bridge: &NativeKernelBridge,
        control_value: Option<u16>,
    ) -> Result<Value> {
        ensure(
            bridge.manifest().source_lowered_unit == "Examples/05_bounded_ui_accumulator/main.lowering.json",
            "Unexpected native kernel source lowered unit.",
        )?;
        self.apply_contract_property_writes()?;
        if let Some(value) = control_value {
            self.set_control_value(value);
        }
        let result = bridge.run(self.control_value()?);
        if !result.ok {
            return Err(RuntimeError::Message(bridge.manifest().diagnostic(result.error_code)));
        }
        self.last_final_state = result.result;
        if let Some(indicator) = self.widgets.get_mut("ind_result") {
            indicator
                .properties
                .insert("value".to_string(), Value::Number(result.result.into()));
        }
        Ok(self.execution_artifact())
    }

    pub fn execution_artifact(&self) -> Value {
        let widgets: Vec<Value> = self
            .widgets
            .values()
            .map(|widget| {
                json!({
                    "widget_id": widget.widget_id,
                    "class_ref": widget.class_ref,
                    "role": widget.role,
                    "layout": widget.layout,
                    "runtime": {
                        "value": widget.properties.get("value").cloned().unwrap_or_else(|| json!(0)),
                        "label": widget.properties.get("label").cloned().unwrap_or_else(|| json!("")),
                        "visible": widget.properties.get("visible").cloned().unwrap_or_else(|| json!(true)),
                        "enabled": widget.properties.get("enabled").cloned().unwrap_or_else(|| json!(true)),
                        "foreground_color": widget.properties.get("foreground_color").cloned().unwrap_or_else(|| json!("#D8D8D8")),
                        "asset_ref": widget.asset_id.as_ref().map(|id| format!("asset:{id}")),
                    }
                })
            })
            .collect();

        json!({
            "artifact_kind": "frog_runtime_execution_result",
            "artifact_governance_ref": { "path": "Versioning/Readme.md" },
            "status": "ok",
            "contract_ref": {
                "unit_ids": [self.unit.unit_id],
                "backend_family": self.contract.backend_family,
                "source_ref": self.contract.source_ref,
            },
            "execution_summary": {
                "mode": "contract_and_wfrog",
                "executed_unit": self.unit.unit_id,
                "iterations": self.unit.execution_model.iteration_count,
                "state_initialized": true,
                "initial_state": self.unit.state_model.carrier.initial_value,
                "final_state": self.last_final_state,
            },
            "outputs": {
                "public": { "result": self.last_final_state },
                "ui": {
                    "ctrl_input": self.control_value().unwrap_or(0),
                    "ind_result": self.last_final_state,
                }
            },
            "ui_runtime": {
                "panel": {
                    "panel_id": self.panel.panel_id,
                    "title": self.panel.title,
                    "class_ref": self.panel.class_ref,
                    "layout": self.panel.layout,
                },
                "widgets": widgets,
                "applied_widget_references": self.applied_widget_references,
            },
            "diagnostics": [],
        })
    }
}

fn validate_contract_and_package(contract: &BackendContract, package: &WfrogPackage) -> Result<ContractUnit> {
    ensure(contract.backend_family == REFERENCE_BACKEND_FAMILY, "Unexpected backend family.")?;
    ensure(
        contract.assumptions.runtime_family.name == REFERENCE_BACKEND_FAMILY,
        "Unexpected runtime-family assumption name.",
    )?;
    ensure(
        contract.assumptions.runtime_family.ui_binding.widget_value_binding,
        "Contract must require widget_value_binding.",
    )?;
    ensure(
        contract.assumptions.runtime_family.ui_binding.widget_reference_binding,
        "Contract must require widget_reference_binding.",
    )?;
    ensure(
        contract.assumptions.numeric_behavior.value_domain == "u16",
        "Contract numeric behavior must target the u16 domain.",
    )?;
    ensure(
        contract.assumptions.numeric_behavior.overflow_behavior == EXPECTED_OVERFLOW_BEHAVIOR,
        format!(
            "Contract overflow behavior must be {}.",
            EXPECTED_OVERFLOW_BEHAVIOR
        ),
    )?;
    ensure(contract.units.len() == 1, "Expected exactly one contract unit.")?;
    let unit = contract.units[0].clone();
    ensure(unit.unit_id == "main", "Expected unit_id main.")?;
    ensure(unit.kind == "bounded_executable_ui_unit", "Unexpected unit kind.")?;
    ensure(unit.public_interface.inputs.len() == 1, "Expected one public input.")?;
    ensure(unit.public_interface.outputs.len() == 1, "Expected one public output.")?;
    ensure(unit.public_interface.inputs[0].id == "input_value", "Expected public input input_value.")?;
    ensure(unit.public_interface.outputs[0].id == "result", "Expected public output result.")?;
    ensure(unit.execution_model.iteration_count == 5, "Slice 05 expects five iterations.")?;
    ensure(unit.state_model.carrier.initial_value == 0, "Slice 05 expects initial state 0.")?;

    ensure(package.front_panels.len() == 1, "Expected exactly one front panel.")?;
    let panel = &package.front_panels[0];
    ensure(panel.host_binding_ref == "reference_host_default", "Expected host_binding_ref reference_host_default.")?;
    let host_binding: &HostBinding = package
        .host_bindings
        .iter()
        .find(|entry| entry.binding_id == "reference_host_default")
        .ok_or_else(|| RuntimeError::Message("Missing reference_host_default host binding.".to_string()))?;
    let required: std::collections::HashSet<&str> = host_binding
        .required_capabilities
        .iter()
        .map(|item| item.as_str())
        .collect();
    ensure(required.contains("window"), "Missing host capability window.")?;
    ensure(required.contains("basic_widget_rendering"), "Missing host capability basic_widget_rendering.")?;
    ensure(required.contains("property_write"), "Missing host capability property_write.")?;
    ensure(required.contains("widget_value_binding"), "Missing host capability widget_value_binding.")?;
    ensure(required.contains("widget_reference_binding"), "Missing host capability widget_reference_binding.")?;

    let panel_widgets: HashMap<&str, &PanelWidget> = panel
        .widgets
        .iter()
        .map(|widget| (widget.instance_id.as_str(), widget))
        .collect();
    for widget in &unit.ui_binding.widgets {
        let panel_widget = panel_widgets
            .get(widget.widget_id.as_str())
            .ok_or_else(|| RuntimeError::Message(format!("Missing panel widget {}", widget.widget_id)))?;
        ensure(
            panel_widget.class_ref == widget.widget_class,
            format!("Class mismatch for widget {}", widget.widget_id),
        )?;
        ensure(
            SUPPORTED_WIDGET_CLASSES.iter().any(|(class_id, _)| *class_id == widget.widget_class),
            format!("Unsupported widget class {}", widget.widget_class),
        )?;
    }

    let support: HashMap<&str, Vec<String>> = unit
        .ui_binding
        .widget_reference_support
        .iter()
        .map(|entry| (entry.widget_id.as_str(), entry.supported_members.clone()))
        .collect();
    for property_write in &unit.property_writes {
        ensure(
            SUPPORTED_PROPERTIES
                .iter()
                .any(|member| *member == property_write.member.as_str()),
            format!(
                "Unsupported property write {}.{}",
                property_write.widget_id, property_write.member
            ),
        )?;
        let members = support
            .get(property_write.widget_id.as_str())
            .ok_or_else(|| RuntimeError::Message(format!("No widget reference support for {}", property_write.widget_id)))?;
        ensure(
            members.iter().any(|member| member == &property_write.member),
            format!(
                "Unsupported widget reference member {}.{}",
                property_write.widget_id, property_write.member
            ),
        )?;
    }

    Ok(unit)
}

fn build_widgets(
    unit: &ContractUnit,
    panel: &FrontPanel,
    asset_map: &HashMap<String, PathBuf>,
) -> Result<BTreeMap<String, WidgetState>> {
    let support: HashMap<&str, Vec<String>> = unit
        .ui_binding
        .widget_reference_support
        .iter()
        .map(|entry| (entry.widget_id.as_str(), entry.supported_members.clone()))
        .collect();
    let panel_widgets: HashMap<&str, &PanelWidget> = panel
        .widgets
        .iter()
        .map(|widget| (widget.instance_id.as_str(), widget))
        .collect();
    let mut widgets = BTreeMap::new();

    for binding in &unit.ui_binding.widgets {
        let panel_widget = panel_widgets
            .get(binding.widget_id.as_str())
            .ok_or_else(|| RuntimeError::Message(format!("Missing panel widget {}", binding.widget_id)))?;
        let asset_ref = panel_widget.visual.get("asset_ref").and_then(|item| item.as_str());
        let asset_id = asset_ref
            .and_then(|value| value.split_once(':').map(|(_, right)| right.to_string()));
        let asset_path = asset_id
            .as_ref()
            .and_then(|id| asset_map.get(id).cloned());
        let mut properties = panel_widget.props.clone();
        properties
            .entry("value".to_string())
            .or_insert(Value::Number(0u16.into()));
        properties
            .entry("label".to_string())
            .or_insert(Value::String(String::new()));
        properties
            .entry("visible".to_string())
            .or_insert(Value::Bool(true));
        properties
            .entry("enabled".to_string())
            .or_insert(Value::Bool(true));
        properties
            .entry("foreground_color".to_string())
            .or_insert(Value::String("#D8D8D8".to_string()));
        widgets.insert(
            binding.widget_id.clone(),
            WidgetState {
                widget_id: binding.widget_id.clone(),
                class_ref: binding.widget_class.clone(),
                role: binding.role.clone(),
                layout: panel_widget.layout.clone(),
                properties,
                asset_id,
                asset_path,
                supported_members: support
                    .get(binding.widget_id.as_str())
                    .cloned()
                    .unwrap_or_default(),
            },
        );
    }

    Ok(widgets)
}
