from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Dict, List, Optional

try:
    from .native_kernel import NativeKernelBridge
except ImportError:  # pragma: no cover
    from native_kernel import NativeKernelBridge


REFERENCE_BACKEND_FAMILY = "reference_host_runtime_ui_binding"
EXPECTED_OVERFLOW_BEHAVIOR = "reject_execution_on_u16_overflow"

SUPPORTED_WIDGET_CLASSES = {
    "frog.widgets.numeric_control": "control",
    "frog.widgets.numeric_indicator": "indicator",
}

SUPPORTED_WIDGET_PROPERTIES = {
    "value",
    "label",
    "visible",
    "enabled",
    "foreground_color",
}


class RuntimeValidationError(RuntimeError):
    """Raised when the contract or the UI package do not satisfy the bounded runtime surface."""


class RuntimeExecutionError(RuntimeError):
    """Raised when execution cannot proceed."""


def find_repo_root(start: Path) -> Path:
    current = start.resolve()
    for candidate in [current, *current.parents]:
        if (candidate / "Examples").is_dir() and (candidate / "Implementations").is_dir():
            return candidate
    raise RuntimeValidationError("Unable to locate the repository root from the current path.")


def default_contract_path() -> Path:
    repo_root = find_repo_root(Path(__file__).resolve())
    return (
        repo_root
        / "Implementations"
        / "Reference"
        / "ContractEmitter"
        / "examples"
        / "05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json"
    )


def default_wfrog_path() -> Path:
    repo_root = find_repo_root(Path(__file__).resolve())
    return repo_root / "Examples" / "05_bounded_ui_accumulator" / "ui" / "accumulator_panel.wfrog"


def load_json(path: Path | str) -> Dict[str, Any]:
    return json.loads(Path(path).read_text(encoding="utf-8"))


def resolve_repo_path(anchor: Path, path: str) -> Path:
    candidate = Path(path)
    if candidate.is_absolute():
        return candidate
    try:
        repo = find_repo_root(anchor)
    except RuntimeValidationError:
        repo = find_repo_root(Path(__file__).resolve())
    return repo / candidate


def normalize_source_front_panel(source: Dict[str, Any]) -> Dict[str, Any]:
    metadata = require_object(source.get("metadata"), "source.metadata")
    panel = require_object(source.get("front_panel"), "source.front_panel")
    normalized: Dict[str, Any] = {
        "panel_id": panel.get("panel_id", f"{metadata.get('name', 'frog')}_panel"),
        "title": panel.get("title", metadata.get("summary", metadata.get("name", "FROG Front Panel"))),
        "class_ref": panel.get("class_ref", "frog.front_panel"),
        "layout": panel.get("canvas", panel.get("layout", {})),
        "widgets": [],
        "host_binding_ref": panel.get("host_binding_ref", "reference_host_default"),
    }
    for raw_widget in require_list(panel.get("widgets"), "source.front_panel.widgets"):
        widget = require_object(raw_widget, "source.front_panel.widgets[]")
        instance_id = widget.get("instance_ref") or widget.get("instance_id") or widget.get("id")
        ensure(isinstance(instance_id, str) and instance_id, "source front-panel widget must expose id/instance_ref.")
        entry = dict(widget)
        entry["instance_id"] = instance_id
        entry.setdefault("layout", {})
        entry.setdefault("props", {})
        entry.setdefault("visual", {})
        normalized["widgets"].append(entry)
    return normalized


def load_source_front_panel_from_contract(contract: Dict[str, Any], contract_path: Path | str) -> Dict[str, Any]:
    source_ref = require_object(contract.get("source_ref"), "contract.source_ref")
    source_path = resolve_repo_path(Path(contract_path), str(source_ref.get("path", "")))
    return normalize_source_front_panel(load_json(source_path))


def ensure(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeValidationError(message)


def require_object(value: Any, name: str) -> Dict[str, Any]:
    ensure(isinstance(value, dict), f"{name} must be an object.")
    return value


def require_list(value: Any, name: str) -> List[Any]:
    ensure(isinstance(value, list), f"{name} must be a list.")
    return value


def checked_u16(value: int, *, label: str) -> int:
    if value < 0 or value > 65535:
        raise RuntimeExecutionError(f"{label} must remain in the u16 domain.")
    return value


@dataclass
class WidgetState:
    widget_id: str
    class_ref: str
    role: str
    layout: Dict[str, Any]
    properties: Dict[str, Any]
    asset_id: Optional[str]
    asset_path: Optional[Path]
    supported_members: List[str]


class Slice05RuntimeCore:
    """Strict, minimal runtime core for the published Example 05 normalized contract corridor."""

    def __init__(self, contract_path: Path | str | None = None, wfrog_path: Path | str | None = None) -> None:
        self.contract_path = Path(contract_path or default_contract_path()).resolve()
        self.wfrog_path = Path(wfrog_path or default_wfrog_path()).resolve()

        self.contract = load_json(self.contract_path)
        self.package = load_json(self.wfrog_path)
        self.panel = load_source_front_panel_from_contract(self.contract, self.contract_path)

        self.unit = self._load_and_validate()
        self.public_io = require_object(self.unit.get("public_io"), "unit.public_io")
        self.ui_bindings = require_object(self.unit.get("ui_bindings"), "unit.ui_bindings")
        self.execution_kernel = require_object(self.unit.get("execution_kernel"), "unit.execution_kernel")
        self.effects = require_list(self.unit.get("effects"), "unit.effects")
        self.publications = require_list(self.unit.get("publications"), "unit.publications")

        self.asset_map = {
            item["asset_id"]: (self.wfrog_path.parent / Path(item["path"])).resolve()
            for item in self.package.get("svg_assets", [])
        }

        self.widgets = self._build_widgets()
        self.last_final_state: int = 0
        self.last_public_outputs: Dict[str, int] = {"result": 0}
        self.diagnostics: List[Dict[str, Any]] = []
        self.applied_widget_references: List[Dict[str, Any]] = []

        self.apply_contract_effects()

    def _load_and_validate(self) -> Dict[str, Any]:
        ensure(self.contract.get("artifact_kind") == "frog_backend_contract", "Expected frog_backend_contract.")
        ensure(self.contract.get("backend_family") == REFERENCE_BACKEND_FAMILY, f"Expected backend family {REFERENCE_BACKEND_FAMILY}.")
        ensure(self.contract.get("example_id") == "05_bounded_ui_accumulator", "Expected example_id 05_bounded_ui_accumulator.")

        assumptions = require_object(self.contract.get("assumptions"), "contract.assumptions")
        runtime_family = require_object(assumptions.get("runtime_family"), "contract.assumptions.runtime_family")
        ensure(runtime_family.get("name") == REFERENCE_BACKEND_FAMILY, "Unexpected assumptions.runtime_family.name.")

        ui_binding_assumptions = require_object(runtime_family.get("ui_binding"), "contract.assumptions.runtime_family.ui_binding")
        ensure(ui_binding_assumptions.get("widget_value_binding") is True, "Contract must require widget_value_binding.")
        ensure(ui_binding_assumptions.get("widget_reference_binding") is True, "Contract must require widget_reference_binding.")

        numeric_behavior = require_object(assumptions.get("numeric_behavior"), "contract.assumptions.numeric_behavior")
        ensure(numeric_behavior.get("value_domain") == "u16", "Contract numeric behavior must target the u16 domain.")
        ensure(numeric_behavior.get("overflow_behavior") == EXPECTED_OVERFLOW_BEHAVIOR, f"Contract overflow behavior must be {EXPECTED_OVERFLOW_BEHAVIOR}.")

        units = require_list(self.contract.get("units"), "contract.units")
        ensure(len(units) == 1, "Expected exactly one contract unit.")
        unit = require_object(units[0], "contract.units[0]")
        ensure(unit.get("unit_id") == "main", "Expected unit_id main.")
        ensure(unit.get("kind") == "bounded_executable_ui_unit", "Unexpected runtime unit kind.")

        public_io = require_object(unit.get("public_io"), "unit.public_io")
        ui_bindings = require_object(unit.get("ui_bindings"), "unit.ui_bindings")
        execution_kernel = require_object(unit.get("execution_kernel"), "unit.execution_kernel")
        effects = require_list(unit.get("effects"), "unit.effects")
        publications = require_list(unit.get("publications"), "unit.publications")

        public_inputs = require_list(public_io.get("inputs"), "unit.public_io.inputs")
        public_outputs = require_list(public_io.get("outputs"), "unit.public_io.outputs")
        ensure(len(public_inputs) == 1 and public_inputs[0]["id"] == "input_value", "Expected public input input_value.")
        ensure(len(public_outputs) == 1 and public_outputs[0]["id"] == "result", "Expected public output result.")

        ensure(execution_kernel.get("state_id") == "accumulator_state", "Expected execution_kernel.state_id accumulator_state.")
        ensure(execution_kernel.get("state_type") == "u16", "Expected execution_kernel.state_type u16.")
        ensure(execution_kernel.get("initial_state") == 0, "Expected execution_kernel.initial_state 0.")
        ensure(execution_kernel.get("iteration_count") == 5, "Slice 05 expects five iterations.")
        ensure(execution_kernel.get("overflow_behavior") == EXPECTED_OVERFLOW_BEHAVIOR, "Unexpected execution_kernel.overflow_behavior.")

        iteration_body = require_list(execution_kernel.get("iteration_body"), "unit.execution_kernel.iteration_body")
        ensure(len(iteration_body) == 1, "Expected one iteration body operation.")
        op = require_object(iteration_body[0], "unit.execution_kernel.iteration_body[0]")
        ensure(op.get("op") == "add", "Expected add iteration operation.")
        ensure(op.get("dst") == "state_next", "Expected state_next iteration destination.")
        ensure(op.get("src") == ["state_current", "input_value"], "Unexpected iteration sources.")
        ensure(publications == execution_kernel.get("final_publication"), "unit.publications must match execution_kernel.final_publication.")

        ensure(self.package.get("format") == "frog.wfrog", "Unsupported .wfrog format.")
        ensure(self.package.get("kind") in {"front_panel_package", "widget_realization_package"}, "Only front_panel_package or widget_realization_package is supported.")
        current_panel = require_object(self.panel, "source.front_panel")
        ensure(current_panel.get("host_binding_ref") == "reference_host_default", "Expected host_binding_ref reference_host_default.")

        widget_classes = {entry["class_id"]: entry for entry in require_list(self.package.get("widget_classes"), "wfrog.widget_classes")}
        ensure("frog.widgets.numeric_control" in widget_classes, "Missing numeric_control class in .wfrog.")
        ensure("frog.widgets.numeric_indicator" in widget_classes, "Missing numeric_indicator class in .wfrog.")

        host_bindings = {entry["binding_id"]: entry for entry in require_list(self.package.get("host_bindings"), "wfrog.host_bindings")}
        ensure("reference_host_default" in host_bindings, "Missing reference_host_default host binding.")
        required_capabilities = set(host_bindings["reference_host_default"].get("required_capabilities", []))
        ensure({"window", "basic_widget_rendering", "property_write", "widget_value_binding", "widget_reference_binding"} <= required_capabilities, "Host binding is missing required capabilities.")

        panel_widgets = {entry["instance_id"]: entry for entry in current_panel.get("widgets", [])}
        widgets = require_list(ui_bindings.get("widgets"), "unit.ui_bindings.widgets")
        for contract_widget in widgets:
            widget_id = contract_widget["widget_id"]
            ensure(widget_id in panel_widgets, f"Panel is missing widget {widget_id}.")
            panel_widget = panel_widgets[widget_id]
            ensure(panel_widget["class_ref"] == contract_widget["widget_class"], f"Class mismatch for widget {widget_id}.")
            ensure(panel_widget["class_ref"] in SUPPORTED_WIDGET_CLASSES, f"Unsupported widget class {panel_widget['class_ref']}.")

        reference_support = {
            item["widget_id"]: item["supported_members"]
            for item in require_list(ui_bindings.get("widget_reference_support"), "unit.ui_bindings.widget_reference_support")
        }
        for item in effects:
            effect = require_object(item, "unit.effects[]")
            widget_id = effect["widget_id"]
            member = effect["member"]
            ensure(effect.get("op") == "frog.ui.property_write", "Only frog.ui.property_write effects are supported.")
            ensure(member in SUPPORTED_WIDGET_PROPERTIES, f"Unsupported property write {member}.")
            ensure(widget_id in reference_support and member in reference_support[widget_id], f"Unsupported widget reference member {widget_id}.{member}.")

        return unit

    def _build_widgets(self) -> Dict[str, WidgetState]:
        panel_widgets = {entry["instance_id"]: entry for entry in self.panel.get("widgets", [])}
        support = {item["widget_id"]: list(item["supported_members"]) for item in self.ui_bindings["widget_reference_support"]}
        widgets: Dict[str, WidgetState] = {}

        for contract_widget in self.ui_bindings["widgets"]:
            widget_id = contract_widget["widget_id"]
            panel_widget = panel_widgets[widget_id]
            visual = panel_widget.get("visual", {})
            asset_ref = visual.get("asset_ref")
            asset_id = None
            asset_path = None

            if isinstance(asset_ref, str) and asset_ref.startswith("asset:"):
                asset_id = asset_ref.split(":", 1)[1]
                asset_path = self.asset_map.get(asset_id)

            props = dict(panel_widget.get("props", {}))
            props.setdefault("value", 0)
            props.setdefault("label", "")
            props.setdefault("visible", True)
            props.setdefault("enabled", True)
            props.setdefault("foreground_color", "#D8D8D8")

            widgets[widget_id] = WidgetState(
                widget_id=widget_id,
                class_ref=contract_widget["widget_class"],
                role=contract_widget["role"],
                layout=dict(panel_widget.get("layout", {})),
                properties=props,
                asset_id=asset_id,
                asset_path=asset_path,
                supported_members=support.get(widget_id, []),
            )

        return widgets

    def apply_contract_effects(self) -> None:
        self.applied_widget_references = []
        for item in self.effects:
            widget = self.widgets[item["widget_id"]]
            member = item["member"]
            value_field = item["value"]
            value = value_field["value"] if isinstance(value_field, dict) else value_field
            ensure(member in widget.supported_members, f"Property {member} is not supported by widget {widget.widget_id}.")
            widget.properties[member] = value
            self.applied_widget_references.append({"widget_id": widget.widget_id, "member": member, "value": value})

    def set_control_value(self, value: int) -> None:
        value = checked_u16(int(value), label="input_value")
        self.widgets["ctrl_input"].properties["value"] = value

    def get_control_value(self) -> int:
        return checked_u16(int(self.widgets["ctrl_input"].properties.get("value", 0)), label="input_value")

    def reset_to_default_style(self, widget_id: str) -> None:
        panel_widgets = {entry["instance_id"]: entry for entry in self.panel.get("widgets", [])}
        panel_widget = panel_widgets[widget_id]
        props = dict(panel_widget.get("props", {}))
        props.setdefault("visible", True)
        props.setdefault("enabled", True)
        props.setdefault("foreground_color", "#D8D8D8")
        props.setdefault("label", "")
        props.setdefault("value", 0)
        self.widgets[widget_id].properties.update(props)

    def invoke_method(self, widget_id: str, method_name: str) -> None:
        if widget_id == "ind_result" and method_name == "reset_to_default_style":
            self.reset_to_default_style(widget_id)
            return
        if widget_id == "ctrl_input" and method_name == "focus":
            return
        raise RuntimeExecutionError(f"Unsupported method invocation: {widget_id}.{method_name}")

    def execute(self, *, control_value: Optional[int] = None) -> Dict[str, Any]:
        self.diagnostics = []
        self.apply_contract_effects()

        if control_value is not None:
            self.set_control_value(control_value)

        input_value = self.get_control_value()
        iterations = int(self.execution_kernel["iteration_count"])
        state = int(self.execution_kernel["initial_state"])

        for _ in range(iterations):
            state = checked_u16(state + input_value, label="final_state")

        self.widgets["ind_result"].properties["value"] = state
        self.last_final_state = state
        self.last_public_outputs = {"result": state}
        return self.execution_artifact()

    def execute_with_native_kernel_bridge(
        self,
        bridge: NativeKernelBridge,
        *,
        control_value: Optional[int] = None,
    ) -> Dict[str, Any]:
        if bridge.manifest.source_lowered_unit != "Examples/05_bounded_ui_accumulator/main.lowering.json":
            raise RuntimeValidationError("Unexpected native kernel source lowered unit.")

        self.diagnostics = []
        self.apply_contract_effects()

        if control_value is not None:
            self.set_control_value(control_value)

        result = bridge.run(self.get_control_value())
        if not result.ok:
            raise RuntimeExecutionError(result.diagnostic or "native kernel execution failed.")

        self.widgets["ind_result"].properties["value"] = result.result
        self.last_final_state = result.result
        self.last_public_outputs = {"result": result.result}
        return self.execution_artifact()

    def execution_artifact(self) -> Dict[str, Any]:
        ui_widget_entries = []
        for widget in self.widgets.values():
            runtime_fields = {
                "value": widget.properties.get("value", 0),
                "label": widget.properties.get("label", ""),
                "visible": bool(widget.properties.get("visible", True)),
                "enabled": bool(widget.properties.get("enabled", True)),
                "foreground_color": str(widget.properties.get("foreground_color", "#D8D8D8")),
                "asset_ref": f"asset:{widget.asset_id}" if widget.asset_id else None,
            }
            for key in [
                "caption.text",
                "caption.visible",
                "caption.anchor.x",
                "caption.anchor.y",
                "caption.align.horizontal",
                "style.caption.text_color",
                "style.caption.font_family",
                "style.caption.font_size",
                "style.caption.font_weight",
                "style.text_value.color",
                "style.text_value.font_family",
                "style.text_value.font_size",
                "style.text_value.font_weight",
            ]:
                if key in widget.properties:
                    runtime_fields[key] = widget.properties[key]
            ui_widget_entries.append(
                {
                    "widget_id": widget.widget_id,
                    "class_ref": widget.class_ref,
                    "role": widget.role,
                    "layout": dict(widget.layout),
                    "runtime": runtime_fields,
                }
            )

        return {
            "artifact_kind": "frog_runtime_execution_result",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "status": "ok",
            "contract_ref": {
                "unit_ids": [self.unit["unit_id"]],
                "backend_family": self.contract["backend_family"],
                "source_ref": dict(self.contract["source_ref"]),
            },
            "execution_summary": {
                "mode": "contract_and_wfrog",
                "executed_unit": self.unit["unit_id"],
                "iterations": int(self.execution_kernel["iteration_count"]),
                "state_initialized": True,
                "initial_state": int(self.execution_kernel["initial_state"]),
                "final_state": int(self.last_final_state),
            },
            "outputs": {
                "public": dict(self.last_public_outputs),
                "ui": {
                    "ctrl_input": self.get_control_value(),
                    "ind_result": int(self.widgets["ind_result"].properties.get("value", 0)),
                },
            },
            "ui_runtime": {
                "panel": {
                    "panel_id": self.panel["panel_id"],
                    "title": self.panel["title"],
                    "class_ref": self.panel["class_ref"],
                    "layout": dict(self.panel["layout"]),
                },
                "widgets": ui_widget_entries,
                "applied_widget_references": list(self.applied_widget_references),
            },
            "diagnostics": list(self.diagnostics),
        }


def execute_slice05_contract(
    control_value: int = 3,
    *,
    contract_path: Path | str | None = None,
    wfrog_path: Path | str | None = None,
) -> Dict[str, Any]:
    runtime = Slice05RuntimeCore(contract_path=contract_path, wfrog_path=wfrog_path)
    return runtime.execute(control_value=control_value)
