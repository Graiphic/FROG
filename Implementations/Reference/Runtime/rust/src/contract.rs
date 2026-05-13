use std::fs;
use std::path::{Path, PathBuf};

use serde::{Deserialize, Serialize};
use serde_json::{json, Value};

use crate::diagnostics::{ensure, Result, RuntimeError};

pub const REFERENCE_BACKEND_FAMILY: &str = "reference_host_runtime_ui_binding";
pub const EXPECTED_OVERFLOW_BEHAVIOR: &str = "reject_execution_on_u16_overflow";

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct ArtifactReference {
    pub path: String,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct SourceRef {
    pub example_id: String,
    pub path: String,
    pub entry_unit: String,
}

#[derive(Debug, Clone, Deserialize, Serialize, Default)]
pub struct UiBindingAssumptions {
    #[serde(default)]
    pub widget_value_binding: bool,
    #[serde(default)]
    pub widget_reference_binding: bool,
}

#[derive(Debug, Clone, Deserialize, Serialize, Default)]
pub struct RuntimeFamilyAssumptions {
    #[serde(default)]
    pub name: String,
    #[serde(default)]
    pub host_model: String,
    #[serde(default)]
    pub ui_binding: UiBindingAssumptions,
}

#[derive(Debug, Clone, Deserialize, Serialize, Default)]
pub struct NumericBehaviorAssumptions {
    #[serde(default)]
    pub value_domain: String,
    #[serde(default)]
    pub overflow_behavior: String,
}

#[derive(Debug, Clone, Deserialize, Serialize, Default)]
pub struct ContractAssumptions {
    #[serde(default)]
    pub runtime_family: RuntimeFamilyAssumptions,
    #[serde(default)]
    pub numeric_behavior: NumericBehaviorAssumptions,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct BackendContract {
    pub artifact_kind: String,
    #[serde(default)]
    pub artifact_governance_ref: Option<ArtifactReference>,
    pub backend_family: String,
    pub source_ref: SourceRef,
    #[serde(default)]
    pub assumptions: ContractAssumptions,
    pub units: Vec<ContractUnit>,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct ContractUnit {
    pub unit_id: String,
    pub kind: String,
    pub public_interface: PublicInterface,
    pub ui_binding: UiBinding,
    pub state_model: StateModel,
    pub execution_model: ExecutionModel,
    #[serde(default)]
    pub property_writes: Vec<PropertyWrite>,
    pub public_output_publication: PublicOutputPublication,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct PublicInterface {
    pub inputs: Vec<InterfacePort>,
    pub outputs: Vec<InterfacePort>,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct InterfacePort {
    pub id: String,
    #[serde(rename = "type")]
    pub port_type: String,
    #[serde(default)]
    pub binding_origin: Option<String>,
    #[serde(default)]
    pub binding_target: Option<String>,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct UiBinding {
    #[serde(default)]
    pub package_refs: Vec<String>,
    pub widgets: Vec<WidgetBinding>,
    pub widget_reference_support: Vec<WidgetReferenceSupport>,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct WidgetBinding {
    pub widget_id: String,
    pub widget_class: String,
    pub value_type: String,
    pub role: String,
    pub binding: WidgetBindingMode,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct WidgetBindingMode {
    pub mode: String,
    #[serde(default)]
    pub public_input_id: Option<String>,
    #[serde(default)]
    pub public_output_id: Option<String>,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct WidgetReferenceSupport {
    pub widget_id: String,
    pub supported_members: Vec<String>,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct StateModel {
    pub explicit_state: bool,
    pub carrier: StateCarrier,
    pub commit_rule: String,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct StateCarrier {
    pub primitive: String,
    pub state_id: String,
    #[serde(rename = "type")]
    pub state_type: String,
    pub initial_value: u16,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct ExecutionModel {
    pub structure: String,
    pub iteration_count: u32,
    #[serde(default)]
    pub iteration_variable: Option<String>,
    pub body_rule: BodyRule,
    pub final_result_rule: String,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct BodyRule {
    pub kind: String,
    pub expression: String,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct PropertyWrite {
    pub operation: String,
    pub widget_id: String,
    pub member: String,
    pub value: PropertyWriteValue,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct PropertyWriteValue {
    #[serde(rename = "type")]
    pub value_type: String,
    pub value: String,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct PublicOutputPublication {
    pub output_id: String,
    pub source: String,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct WfrogPackage {
    pub format: String,
    pub kind: String,
    #[serde(default)]
    pub widget_classes: Vec<WidgetClass>,
    #[serde(default)]
    pub svg_assets: Vec<SvgAsset>,
    #[serde(default)]
    pub host_bindings: Vec<HostBinding>,
    #[serde(default)]
    pub front_panels: Vec<FrontPanel>,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct WidgetClass {
    pub class_id: String,
    #[serde(default)]
    pub properties: Vec<WidgetProperty>,
    #[serde(default)]
    pub methods: Vec<WidgetMethod>,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct WidgetProperty {
    pub name: String,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct WidgetMethod {
    pub name: String,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct SvgAsset {
    pub asset_id: String,
    pub path: String,
    pub kind: String,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct HostBinding {
    pub binding_id: String,
    pub target: String,
    #[serde(default)]
    pub required_capabilities: Vec<String>,
    #[serde(default)]
    pub optional_capabilities: Vec<String>,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct FrontPanel {
    pub panel_id: String,
    pub title: String,
    pub class_ref: String,
    pub layout: serde_json::Value,
    pub widgets: Vec<PanelWidget>,
    pub host_binding_ref: String,
}

#[derive(Debug, Clone, Deserialize, Serialize)]
pub struct PanelWidget {
    pub instance_id: String,
    pub class_ref: String,
    pub layout: serde_json::Value,
    #[serde(default)]
    pub props: serde_json::Map<String, serde_json::Value>,
    #[serde(default)]
    pub visual: serde_json::Map<String, serde_json::Value>,
}

pub fn find_repo_root(start: &Path) -> Result<PathBuf> {
    for candidate in start.ancestors() {
        if candidate.join("Examples").is_dir() && candidate.join("Implementations").is_dir() {
            return Ok(candidate.to_path_buf());
        }
    }
    Err(RuntimeError::Message(
        "Unable to locate the repository root from the current path.".to_string(),
    ))
}

pub fn default_contract_path() -> Result<PathBuf> {
    let manifest_dir = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let repo_root = find_repo_root(&manifest_dir)?;
    Ok(repo_root
        .join("Implementations")
        .join("Reference")
        .join("ContractEmitter")
        .join("examples")
        .join("05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json"))
}

pub fn default_wfrog_path() -> Result<PathBuf> {
    let manifest_dir = PathBuf::from(env!("CARGO_MANIFEST_DIR"));
    let repo_root = find_repo_root(&manifest_dir)?;
    Ok(repo_root
        .join("Examples")
        .join("05_bounded_ui_accumulator")
        .join("ui")
        .join("accumulator_panel.wfrog"))
}

pub fn load_contract_from_path(path: &Path) -> Result<BackendContract> {
    let text = fs::read_to_string(path)?;
    let mut raw: Value = serde_json::from_str(&text)?;
    migrate_contract_shape(&mut raw);
    let contract: BackendContract = serde_json::from_value(raw)?;
    ensure(contract.artifact_kind == "frog_backend_contract", "Expected frog_backend_contract.")?;
    ensure(
        contract.backend_family == REFERENCE_BACKEND_FAMILY,
        format!("Unsupported backend family: {}", contract.backend_family),
    )?;
    Ok(contract)
}

fn migrate_contract_shape(raw: &mut Value) {
    let Some(units) = raw.get_mut("units").and_then(Value::as_array_mut) else {
        return;
    };

    for unit in units {
        let Some(object) = unit.as_object_mut() else {
            continue;
        };

        if !object.contains_key("public_interface") {
            if let Some(public_io) = object.get("public_io").cloned() {
                object.insert("public_interface".to_string(), public_io);
            }
        }

        if !object.contains_key("ui_binding") {
            if let Some(ui_bindings) = object.get("ui_bindings").cloned() {
                object.insert("ui_binding".to_string(), ui_bindings);
            }
        }

        if let Some(kernel) = object.get("execution_kernel").cloned() {
            if !object.contains_key("state_model") {
                let state_id = kernel
                    .get("state_id")
                    .and_then(Value::as_str)
                    .unwrap_or("state");
                let state_type = kernel
                    .get("state_type")
                    .and_then(Value::as_str)
                    .unwrap_or("u16");
                let initial_value = kernel
                    .get("initial_state")
                    .and_then(Value::as_u64)
                    .unwrap_or(0);
                let commit_rule = kernel
                    .get("commit_rule")
                    .and_then(Value::as_str)
                    .unwrap_or("");
                object.insert(
                    "state_model".to_string(),
                    json!({
                        "explicit_state": true,
                        "carrier": {
                            "primitive": "register",
                            "state_id": state_id,
                            "type": state_type,
                            "initial_value": initial_value
                        },
                        "commit_rule": commit_rule
                    }),
                );
            }

            if !object.contains_key("execution_model") {
                let iteration_count = kernel
                    .get("iteration_count")
                    .and_then(Value::as_u64)
                    .unwrap_or(0);
                object.insert(
                    "execution_model".to_string(),
                    json!({
                        "structure": "bounded_loop",
                        "iteration_count": iteration_count,
                        "body_rule": {
                            "kind": "iteration_body",
                            "expression": "state_current + input_value"
                        },
                        "final_result_rule": "state_current"
                    }),
                );
            }
        }

        if !object.contains_key("property_writes") {
            if let Some(effects) = object.get("effects").and_then(Value::as_array) {
                let writes: Vec<Value> = effects
                    .iter()
                    .filter_map(|effect| {
                        let effect = effect.as_object()?;
                        let widget_id = effect.get("widget_id")?.clone();
                        let member = effect.get("member")?.clone();
                        let value = effect.get("value")?.clone();
                        Some(json!({
                            "operation": effect.get("op").cloned().unwrap_or_else(|| json!("frog.ui.property_write")),
                            "widget_id": widget_id,
                            "member": member,
                            "value": value
                        }))
                    })
                    .collect();
                object.insert("property_writes".to_string(), Value::Array(writes));
            }
        }

        if !object.contains_key("public_output_publication") {
            let publication = object
                .get("publications")
                .and_then(Value::as_array)
                .and_then(|publications| {
                    publications.iter().find_map(|entry| {
                        let entry = entry.as_object()?;
                        let target = entry.get("target")?.as_str()?;
                        let output_id = target.strip_prefix("public_output.")?;
                        Some(json!({
                            "output_id": output_id,
                            "source": entry.get("source").cloned().unwrap_or_else(|| json!(""))
                        }))
                    })
                })
                .unwrap_or_else(|| json!({"output_id": "result", "source": "state_current"}));
            object.insert("public_output_publication".to_string(), publication);
        }
    }
}

pub fn load_wfrog_from_path(path: &Path) -> Result<WfrogPackage> {
    let text = fs::read_to_string(path)?;
    let package: WfrogPackage = serde_json::from_str(&text)?;
    ensure(package.format == "frog.wfrog", "Unsupported .wfrog format.")?;
    ensure(
        package.kind == "front_panel_package" || package.kind == "widget_realization_package",
        "Only front_panel_package or widget_realization_package is supported.",
    )?;
    Ok(package)
}

pub fn load_front_panel_from_frog_source_path(path: &Path) -> Result<FrontPanel> {
    let text = fs::read_to_string(path)?;
    let source: Value = serde_json::from_str(&text)?;
    let root = source
        .as_object()
        .ok_or_else(|| RuntimeError::Message("source must be an object.".to_string()))?;
    let metadata = root.get("metadata").and_then(Value::as_object);
    let panel = root
        .get("front_panel")
        .and_then(Value::as_object)
        .ok_or_else(|| RuntimeError::Message("source.front_panel is required.".to_string()))?;
    let title_fallback = metadata
        .and_then(|item| item.get("summary").or_else(|| item.get("name")))
        .and_then(Value::as_str)
        .unwrap_or("FROG Front Panel");
    let panel_id_fallback = metadata
        .and_then(|item| item.get("name"))
        .and_then(Value::as_str)
        .map(|name| format!("{name}_panel"))
        .unwrap_or_else(|| "frog_panel".to_string());
    let layout = panel
        .get("canvas")
        .or_else(|| panel.get("layout"))
        .cloned()
        .unwrap_or_else(|| json!({}));
    let widgets_value = panel
        .get("widgets")
        .and_then(Value::as_array)
        .ok_or_else(|| RuntimeError::Message("source.front_panel.widgets is required.".to_string()))?;
    let mut widgets = Vec::new();
    for widget_value in widgets_value {
        let widget = widget_value
            .as_object()
            .ok_or_else(|| RuntimeError::Message("source front-panel widget must be an object.".to_string()))?;
        let instance_id = widget
            .get("instance_ref")
            .or_else(|| widget.get("instance_id"))
            .or_else(|| widget.get("id"))
            .and_then(Value::as_str)
            .ok_or_else(|| RuntimeError::Message("source front-panel widget must expose id/instance_ref.".to_string()))?;
        let class_ref = widget
            .get("class_ref")
            .and_then(Value::as_str)
            .ok_or_else(|| RuntimeError::Message("source front-panel widget class_ref is required.".to_string()))?;
        widgets.push(PanelWidget {
            instance_id: instance_id.to_string(),
            class_ref: class_ref.to_string(),
            layout: widget.get("layout").cloned().unwrap_or_else(|| json!({})),
            props: widget.get("props").and_then(Value::as_object).cloned().unwrap_or_default(),
            visual: widget.get("visual").and_then(Value::as_object).cloned().unwrap_or_default(),
        });
    }
    Ok(FrontPanel {
        panel_id: panel.get("panel_id").and_then(Value::as_str).unwrap_or(&panel_id_fallback).to_string(),
        title: panel.get("title").and_then(Value::as_str).unwrap_or(title_fallback).to_string(),
        class_ref: panel.get("class_ref").and_then(Value::as_str).unwrap_or("frog.front_panel").to_string(),
        layout,
        widgets,
        host_binding_ref: panel.get("host_binding_ref").and_then(Value::as_str).unwrap_or("reference_host_default").to_string(),
    })
}
