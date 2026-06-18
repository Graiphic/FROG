from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Dict, Optional


REFERENCE_BACKEND_FAMILY = "reference_host_runtime_ui_binding"
DEFAULT_UI_PACKAGE_SUFFIX = "ui/accumulator_panel.wfrog"
EXPECTED_OVERFLOW_BEHAVIOR = "reject_execution_on_u16_overflow"
SUPPORTED_LOWERED_UNIT_KIND = "bounded_accumulator_kernel_with_ui_bindings"
CONTRACT_UNIT_KIND = "bounded_executable_ui_unit"

UI_PACKAGE_BY_EXAMPLE = {
    "05_bounded_ui_accumulator": "accumulator_panel.wfrog",
    "06_boolean_value_roundtrip": "boolean_panel.wfrog",
    "07_string_value_roundtrip": "string_panel.wfrog",
    "08_enum_value_roundtrip": "enum_panel.wfrog",
    "09_path_value_roundtrip": "path_panel.wfrog",
    "10_button_press_to_boolean": "button_panel.wfrog",
    "11_button_switch_when_pressed": "button_panel.wfrog",
    "12_button_switch_when_released": "button_panel.wfrog",
    "13_button_latch_when_pressed": "button_panel.wfrog",
    "14_button_latch_when_released": "button_panel.wfrog",
    "15_button_latch_until_released": "button_panel.wfrog",
    "45_button_placement_ide_view": "button_ide_panel.wfrog",
}

SCALAR_COPY_CONTRACT_SPECS: dict[str, dict[str, Any]] = {
    "boolean_value_roundtrip_kernel_with_ui_bindings": {
        "contract_kind": "boolean_value_roundtrip_ui_unit",
        "behavior_key": "boolean_behavior",
        "behavior": {"value_domain": "bool", "operation": "copy"},
        "input_binding_origin": "widget.bool_input.value",
        "output_binding_target": "widget.bool_result.value",
        "widgets": [
            ("bool_input", "frog.widgets.boolean_control", "bool", "control", {"mode": "widget_value", "public_input_id": "input_value"}),
            ("bool_result", "frog.widgets.boolean_indicator", "bool", "indicator", {"mode": "widget_value", "public_output_id": "result"}),
        ],
        "kernel_keys": ["operation", "src", "dst", "type"],
    },
    "string_value_roundtrip_kernel_with_ui_bindings": {
        "contract_kind": "string_value_roundtrip_ui_unit",
        "behavior_key": "string_behavior",
        "behavior": {"value_domain": "utf8_string", "operation": "copy", "max_utf8_bytes": 256},
        "input_binding_origin": "widget.str_input.value",
        "output_binding_target": "widget.str_result.value",
        "widgets": [
            ("str_input", "frog.widgets.string_control", "string", "control", {"mode": "widget_value", "public_input_id": "input_text"}),
            ("str_result", "frog.widgets.string_indicator", "string", "indicator", {"mode": "widget_value", "public_output_id": "result_text"}),
        ],
        "kernel_keys": ["operation", "src", "dst", "type", "max_utf8_bytes"],
    },
    "enum_value_roundtrip_kernel_with_ui_bindings": {
        "contract_kind": "enum_value_roundtrip_ui_unit",
        "behavior_key": "enum_behavior",
        "behavior": {"value_domain": "enum_item_id", "enum_domain": "example08.mode", "operation": "copy", "representation": "u16"},
        "input_binding_origin": "widget.mode_input.value",
        "output_binding_target": "widget.mode_result.value",
        "widgets": [
            ("mode_input", "frog.widgets.enum_control", "enum_item_id", "control", {"mode": "widget_value", "public_input_id": "mode_value"}),
            ("mode_result", "frog.widgets.enum_indicator", "enum_item_id", "indicator", {"mode": "widget_value", "public_output_id": "result_mode"}),
        ],
        "kernel_keys": ["operation", "src", "dst", "type", "enum_domain", "representation"],
    },
    "path_value_roundtrip_kernel_with_ui_bindings": {
        "contract_kind": "path_value_roundtrip_ui_unit",
        "behavior_key": "path_behavior",
        "behavior": {"value_domain": "utf8_path_string", "operation": "copy", "max_utf8_bytes": 256},
        "input_binding_origin": "widget.path_input.value",
        "output_binding_target": "widget.path_result.value",
        "widgets": [
            ("path_input", "frog.widgets.path_control", "path", "control", {"mode": "widget_value", "public_input_id": "input_path"}),
            ("path_result", "frog.widgets.path_indicator", "path", "indicator", {"mode": "widget_value", "public_output_id": "result_path"}),
        ],
        "kernel_keys": ["operation", "src", "dst", "type", "encoding", "max_utf8_bytes"],
    },
    "button_press_to_boolean_kernel_with_ui_bindings": {
        "contract_kind": "button_press_to_boolean_ui_unit",
        "behavior_key": "boolean_behavior",
        "behavior": {"value_domain": "bool", "button_press_event": "trigger_button.pressed"},
        "input_binding_origin": "widget.trigger_button.pressed",
        "output_binding_target": "interface.pressed",
        "widgets": [
            ("trigger_button", "frog.widgets.button", "bool", "control", {"mode": "widget_event_value", "public_input_id": "trigger_pressed"}),
            ("pressed_indicator", "frog.widgets.boolean_indicator", "bool", "indicator", {"mode": "widget_value", "public_output_id": "pressed"}),
        ],
        "kernel_keys": ["operation", "dst", "type", "src", "final_publication"],
    },
    "button_switch_when_pressed_kernel_with_ui_bindings": {
        "contract_kind": "button_switch_when_pressed_ui_unit",
        "behavior_key": "boolean_behavior",
        "behavior": {
            "value_domain": "bool",
            "button_value": "trigger_button.value",
            "mechanical_action": "switch_when_pressed",
        },
        "input_binding_origin": "widget.trigger_button.value",
        "output_binding_target": "interface.switched",
        "widgets": [
            ("trigger_button", "frog.widgets.button", "bool", "control", {"mode": "widget_value", "public_input_id": "trigger_value"}),
            ("switched_indicator", "frog.widgets.boolean_indicator", "bool", "indicator", {"mode": "widget_value", "public_output_id": "switched"}),
        ],
        "kernel_keys": ["operation", "dst", "type", "src", "final_publication"],
    },
    "button_switch_when_released_kernel_with_ui_bindings": {
        "contract_kind": "button_switch_when_released_ui_unit",
        "behavior_key": "boolean_behavior",
        "behavior": {
            "value_domain": "bool",
            "button_value": "trigger_button.value",
            "mechanical_action": "switch_when_released",
        },
        "input_binding_origin": "widget.trigger_button.value",
        "output_binding_target": "interface.switched",
        "widgets": [
            ("trigger_button", "frog.widgets.button", "bool", "control", {"mode": "widget_value", "public_input_id": "trigger_value"}),
            ("switched_indicator", "frog.widgets.boolean_indicator", "bool", "indicator", {"mode": "widget_value", "public_output_id": "switched"}),
        ],
        "kernel_keys": ["operation", "dst", "type", "src", "final_publication"],
    },
    "button_latch_when_pressed_kernel_with_ui_bindings": {
        "contract_kind": "button_latch_when_pressed_ui_unit",
        "behavior_key": "boolean_behavior",
        "behavior": {
            "value_domain": "bool",
            "button_value": "trigger_button.value",
            "mechanical_action": "latch_when_pressed",
            "latch_reset_policy": "reset_on_natural_value_consumption",
        },
        "input_binding_origin": "widget.trigger_button.value",
        "output_binding_target": "interface.latched",
        "widgets": [
            ("trigger_button", "frog.widgets.button", "bool", "control", {"mode": "widget_value", "public_input_id": "trigger_value"}),
            ("latched_indicator", "frog.widgets.boolean_indicator", "bool", "indicator", {"mode": "widget_value", "public_output_id": "latched"}),
        ],
        "kernel_keys": ["operation", "dst", "type", "src", "final_publication"],
    },
    "button_latch_when_released_kernel_with_ui_bindings": {
        "contract_kind": "button_latch_when_released_ui_unit",
        "behavior_key": "boolean_behavior",
        "behavior": {
            "value_domain": "bool",
            "button_value": "trigger_button.value",
            "mechanical_action": "latch_when_released",
            "latch_reset_policy": "reset_on_natural_value_consumption",
        },
        "input_binding_origin": "widget.trigger_button.value",
        "output_binding_target": "interface.latched",
        "widgets": [
            ("trigger_button", "frog.widgets.button", "bool", "control", {"mode": "widget_value", "public_input_id": "trigger_value"}),
            ("latched_indicator", "frog.widgets.boolean_indicator", "bool", "indicator", {"mode": "widget_value", "public_output_id": "latched"}),
        ],
        "kernel_keys": ["operation", "dst", "type", "src", "final_publication"],
    },
    "button_latch_until_released_kernel_with_ui_bindings": {
        "contract_kind": "button_latch_until_released_ui_unit",
        "behavior_key": "boolean_behavior",
        "behavior": {
            "value_domain": "bool",
            "button_value": "trigger_button.value",
            "mechanical_action": "latch_until_released",
            "latch_reset_policy": "reset_on_natural_value_consumption",
        },
        "input_binding_origin": "widget.trigger_button.value",
        "output_binding_target": "interface.latched",
        "widgets": [
            ("trigger_button", "frog.widgets.button", "bool", "control", {"mode": "widget_value", "public_input_id": "trigger_value"}),
            ("latched_indicator", "frog.widgets.boolean_indicator", "bool", "indicator", {"mode": "widget_value", "public_output_id": "latched"}),
        ],
        "kernel_keys": ["operation", "dst", "type", "src", "final_publication"],
    },
}


class ContractEmissionError(RuntimeError):
    """Raised when the published lowering cannot be emitted as the reference runtime-family contract."""


def _ensure(condition: bool, message: str) -> None:
    if not condition:
        raise ContractEmissionError(message)


def find_repo_root(start: Path) -> Path:
    current = start.resolve()
    for candidate in [current, *current.parents]:
        if (candidate / "Examples").is_dir() and (candidate / "Implementations").is_dir():
            return candidate
    raise ContractEmissionError("Unable to locate repository root from the provided path.")


def load_json_from_path(path: Path | str) -> Dict[str, Any]:
    return json.loads(Path(path).read_text(encoding="utf-8"))


def _expect_single_lowered_unit(lowering: Dict[str, Any]) -> Dict[str, Any]:
    lowered_units = lowering.get("lowered_units")
    _ensure(isinstance(lowered_units, list) and len(lowered_units) == 1, "Expected exactly one lowered unit.")
    unit = lowered_units[0]
    _ensure(isinstance(unit, dict), "Lowered unit must be an object.")
    return unit


def _expect_single_fir_unit(fir: Dict[str, Any]) -> Dict[str, Any]:
    units = fir.get("units")
    _ensure(isinstance(units, list) and len(units) == 1, "Expected exactly one FIR unit.")
    unit = units[0]
    _ensure(isinstance(unit, dict), "FIR unit must be an object.")
    return unit


def _expect_public_io(unit: Dict[str, Any]) -> Dict[str, Any]:
    public_io = unit.get("public_io")
    _ensure(isinstance(public_io, dict), "Missing public_io section.")
    inputs = public_io.get("inputs")
    outputs = public_io.get("outputs")
    _ensure(isinstance(inputs, list) and len(inputs) == 1, "Expected exactly one public input.")
    _ensure(isinstance(outputs, list) and len(outputs) == 1, "Expected exactly one public output.")
    return public_io


def _expect_ui_bindings(unit: Dict[str, Any]) -> Dict[str, Any]:
    ui_bindings = unit.get("ui_bindings")
    _ensure(isinstance(ui_bindings, dict), "Missing ui_bindings section.")
    control_bindings = ui_bindings.get("control_bindings")
    indicator_bindings = ui_bindings.get("indicator_bindings")
    reference_writes = ui_bindings.get("reference_writes")
    _ensure(isinstance(control_bindings, list) and len(control_bindings) == 1, "Expected exactly one control binding.")
    _ensure(isinstance(indicator_bindings, list) and len(indicator_bindings) == 1, "Expected exactly one indicator binding.")
    _ensure(isinstance(reference_writes, list) and len(reference_writes) == 2, "Expected exactly two reference writes.")
    return ui_bindings


def _expect_execution_kernel(unit: Dict[str, Any]) -> Dict[str, Any]:
    kernel = unit.get("execution_kernel")
    _ensure(isinstance(kernel, dict), "Missing execution_kernel section.")
    _ensure(kernel.get("state_type") == "u16", "Bounded accumulator contract supports only u16 state.")
    _ensure(kernel.get("initial_state") == 0, "Bounded accumulator contract expects initial state 0.")
    _ensure(kernel.get("iteration_count") == 5, "Bounded accumulator contract expects exactly five iterations.")

    body = kernel.get("iteration_body")
    _ensure(isinstance(body, list) and len(body) == 1, "Bounded accumulator contract expects exactly one iteration body operation.")
    operation = body[0]
    _ensure(operation.get("op") == "add", "Bounded accumulator contract expects an add iteration body.")
    _ensure(operation.get("dst") == "state_next", "Bounded accumulator contract expects iteration body dst state_next.")
    _ensure(operation.get("src") == ["state_current", "input_value"], "Unexpected iteration body sources.")

    publication = kernel.get("final_publication")
    _ensure(isinstance(publication, list) and len(publication) == 2, "Bounded accumulator contract expects exactly two final publications.")
    return kernel


def _infer_ui_package_path(lowering: Dict[str, Any], explicit_ui_package_path: Optional[str]) -> str:
    if explicit_ui_package_path:
        return explicit_ui_package_path.replace("\\", "/")
    source_ref = lowering.get("source_ref", {})
    source_path = source_ref.get("path")
    _ensure(isinstance(source_path, str) and source_path.endswith("main.frog"), "Unable to infer UI package path from source_ref.path.")
    source_path = source_path.replace("\\", "/")
    example_dir = source_path.rsplit("/", 1)[0]
    example_id = source_ref.get("example_id")
    panel_file = UI_PACKAGE_BY_EXAMPLE.get(str(example_id))
    if panel_file:
        return f"{example_dir}/ui/{panel_file}"
    return f"{example_dir}/{DEFAULT_UI_PACKAGE_SUFFIX}"


def _resolve_repo_relative_path(repo_root: Path, text_path: str) -> Path:
    candidate = Path(text_path)
    return candidate if candidate.is_absolute() else (repo_root / candidate)


def _load_and_validate_fir(
    lowering: Dict[str, Any],
    *,
    lowering_path: Path | str,
    ui_package_path: str,
) -> Dict[str, Any]:
    fir_ref = lowering.get("fir_ref")
    _ensure(isinstance(fir_ref, dict), "Missing fir_ref.")
    fir_path_text = fir_ref.get("path")
    _ensure(isinstance(fir_path_text, str) and fir_path_text.endswith("main.fir.json"), "Invalid fir_ref.path.")

    repo_root = find_repo_root(Path(lowering_path).resolve())
    fir_path = _resolve_repo_relative_path(repo_root, fir_path_text)
    _ensure(fir_path.is_file(), f"Missing FIR artifact: {fir_path_text}")
    fir = load_json_from_path(fir_path)

    _ensure(fir.get("artifact_kind") == "frog_fir_unit", "Expected artifact_kind == frog_fir_unit.")
    fir_source_ref = fir.get("source_ref")
    lowering_source_ref = lowering.get("source_ref")
    _ensure(isinstance(fir_source_ref, dict), "Missing FIR source_ref.")
    _ensure(isinstance(lowering_source_ref, dict), "Missing lowering source_ref.")
    for key in ("example_id", "path", "entry_unit"):
        _ensure(fir_source_ref.get(key) == lowering_source_ref.get(key), f"FIR source_ref.{key} must match lowering source_ref.{key}.")

    front_panel_ref = fir.get("front_panel_ref")
    _ensure(isinstance(front_panel_ref, dict), "Missing front_panel_ref in FIR.")
    _ensure(front_panel_ref.get("package_path") == ui_package_path, "FIR front_panel_ref.package_path must match the published UI package path.")
    _ensure(front_panel_ref.get("panel_id") == "main_panel", "FIR front_panel_ref.panel_id must be main_panel.")

    fir_unit = _expect_single_fir_unit(fir)
    lowering_unit = _expect_single_lowered_unit(lowering)

    _ensure(fir_unit.get("unit_id") == lowering_unit.get("unit_id"), "FIR unit_id must match lowering unit_id.")
    _ensure(fir_unit.get("public_interface") == lowering_unit.get("public_io"), "FIR public_interface must match lowering public_io.")
    _ensure(fir_unit.get("ui_bindings") == lowering_unit.get("ui_bindings"), "FIR ui_bindings must match lowering ui_bindings.")

    fir_state_model = fir_unit.get("state_model")
    execution_kernel = _expect_execution_kernel(lowering_unit)
    _ensure(isinstance(fir_state_model, dict), "Missing FIR state_model.")
    _ensure(fir_state_model.get("explicit_state") is True, "FIR must declare explicit_state = true.")

    carrier = fir_state_model.get("carrier")
    _ensure(isinstance(carrier, dict), "Missing FIR state_model.carrier.")
    _ensure(carrier.get("primitive") == "frog.core.delay", "FIR state carrier must use frog.core.delay.")
    _ensure(carrier.get("type") == execution_kernel.get("state_type"), "FIR state type must match lowering execution_kernel.state_type.")
    _ensure(carrier.get("initial_value") == execution_kernel.get("initial_state"), "FIR initial_value must match lowering initial_state.")

    fir_execution_model = fir_unit.get("execution_model")
    _ensure(isinstance(fir_execution_model, dict), "Missing FIR execution_model.")
    _ensure(fir_execution_model.get("structure") == "for_loop", "FIR execution_model.structure must be for_loop.")
    _ensure(fir_execution_model.get("iteration_count") == execution_kernel.get("iteration_count"), "FIR iteration_count must match lowering iteration_count.")

    body_rule = fir_execution_model.get("body_rule")
    _ensure(isinstance(body_rule, dict), "Missing FIR execution_model.body_rule.")
    _ensure(body_rule.get("kind") == "accumulate_with_explicit_state", "Unexpected FIR body_rule.kind.")
    _ensure(body_rule.get("expression") == "state_next = state_current + input_value", "Unexpected FIR body_rule.expression.")
    _ensure(fir_unit.get("publications") == execution_kernel.get("final_publication"), "FIR publications must match lowering final_publication.")
    return fir


def _value_dict(value_type: str, value_literal: Any) -> Dict[str, Any]:
    return {"type": value_type, "value": value_literal}


def _normalized_public_io(public_io: Dict[str, Any]) -> Dict[str, Any]:
    public_input = public_io["inputs"][0]
    public_output = public_io["outputs"][0]
    return {
        "inputs": [
            {
                "id": public_input.get("id"),
                "type": public_input.get("type"),
                "binding_origin": "widget.ctrl_input.value",
            }
        ],
        "outputs": [
            {
                "id": public_output.get("id"),
                "type": public_output.get("type"),
                "binding_target": "interface.result",
            }
        ],
    }


def _normalized_execution_kernel(execution_kernel: Dict[str, Any]) -> Dict[str, Any]:
    return {
        "state_id": "accumulator_state",
        "initial_state": execution_kernel.get("initial_state"),
        "state_type": execution_kernel.get("state_type"),
        "iteration_count": execution_kernel.get("iteration_count"),
        "iteration_body": execution_kernel.get("iteration_body"),
        "commit_rule": execution_kernel.get("commit_rule"),
        "overflow_behavior": EXPECTED_OVERFLOW_BEHAVIOR,
        "final_publication": execution_kernel.get("final_publication"),
    }


def _build_normalized_widget_bindings(ui_bindings: Dict[str, Any], public_io: Dict[str, Any], ui_package_path: str) -> Dict[str, Any]:
    public_input = public_io["inputs"][0]
    public_output = public_io["outputs"][0]

    control_binding = ui_bindings["control_bindings"][0]
    indicator_binding = ui_bindings["indicator_bindings"][0]
    reference_writes = ui_bindings["reference_writes"]

    _ensure(control_binding.get("widget_id") == "ctrl_input", "Expected control widget ctrl_input.")
    _ensure(control_binding.get("mode") == "widget_value", "Control binding must use widget_value mode.")
    _ensure(control_binding.get("public_input_id") == public_input.get("id"), "Unexpected control public_input_id.")

    _ensure(indicator_binding.get("widget_id") == "ind_result", "Expected indicator widget ind_result.")
    _ensure(indicator_binding.get("mode") == "widget_value", "Indicator binding must use widget_value mode.")
    _ensure(indicator_binding.get("public_output_id") == public_output.get("id"), "Unexpected indicator public_output_id.")

    widget_reference_support = []
    reference_write_contracts = []
    effects = []

    for item in reference_writes:
        widget_id = item.get("widget_id")
        member = item.get("member")
        value_type = item.get("value_type")
        value_literal = item.get("value_literal")

        _ensure(widget_id in {"ctrl_input", "ind_result"}, f"Unexpected widget reference target: {widget_id}")
        _ensure(member == "foreground_color", "Bounded accumulator contract only supports foreground_color property writes.")
        _ensure(value_type == "frog.color.rgba8", "foreground_color writes must use frog.color.rgba8.")

        widget_reference_support.append({"widget_id": widget_id, "supported_members": [member]})
        reference_write_contracts.append(
            {
                "widget_id": widget_id,
                "member": member,
                "value_type": value_type,
                "value_literal": value_literal,
            }
        )
        effects.append(
            {
                "op": "frog.ui.property_write",
                "widget_id": widget_id,
                "member": member,
                "value": _value_dict(value_type, value_literal),
            }
        )

    support_by_id = {entry["widget_id"]: entry for entry in widget_reference_support}
    ordered_support = [support_by_id["ctrl_input"], support_by_id["ind_result"]]

    widgets = [
        {
            "widget_id": "ctrl_input",
            "widget_class": "frog.widgets.numeric_control",
            "value_type": public_input.get("type"),
            "role": "control",
            "binding": {"mode": "widget_value", "public_input_id": public_input.get("id")},
        },
        {
            "widget_id": "ind_result",
            "widget_class": "frog.widgets.numeric_indicator",
            "value_type": public_output.get("type"),
            "role": "indicator",
            "binding": {"mode": "widget_value", "public_output_id": public_output.get("id")},
        },
    ]

    return {
        "ui_bindings": {
            "package_refs": [ui_package_path],
            "widgets": widgets,
            "control_bindings": ui_bindings["control_bindings"],
            "indicator_bindings": ui_bindings["indicator_bindings"],
            "widget_reference_support": ordered_support,
            "reference_writes": reference_write_contracts,
        },
        "effects": effects,
    }


def _load_and_validate_scalar_fir(
    lowering: Dict[str, Any],
    *,
    lowering_path: Path | str,
    ui_package_path: str,
    expected_fir_kind: str,
) -> Dict[str, Any]:
    fir_ref = lowering.get("fir_ref")
    _ensure(isinstance(fir_ref, dict), "Missing fir_ref.")
    fir_path_text = fir_ref.get("path")
    _ensure(isinstance(fir_path_text, str) and fir_path_text.endswith("main.fir.json"), "Invalid fir_ref.path.")

    repo_root = find_repo_root(Path(lowering_path).resolve())
    fir_path = _resolve_repo_relative_path(repo_root, fir_path_text)
    _ensure(fir_path.is_file(), f"Missing FIR artifact: {fir_path_text}")
    fir = load_json_from_path(fir_path)

    _ensure(fir.get("artifact_kind") == "frog_fir_unit", "Expected artifact_kind == frog_fir_unit.")
    fir_source_ref = fir.get("source_ref")
    lowering_source_ref = lowering.get("source_ref")
    _ensure(isinstance(fir_source_ref, dict), "Missing FIR source_ref.")
    _ensure(isinstance(lowering_source_ref, dict), "Missing lowering source_ref.")
    for key in ("example_id", "path", "entry_unit"):
        _ensure(fir_source_ref.get(key) == lowering_source_ref.get(key), f"FIR source_ref.{key} must match lowering source_ref.{key}.")

    front_panel_ref = fir.get("front_panel_ref")
    _ensure(isinstance(front_panel_ref, dict), "Missing front_panel_ref in FIR.")
    _ensure(front_panel_ref.get("package_path") == ui_package_path, "FIR front_panel_ref.package_path must match the published UI package path.")
    _ensure(front_panel_ref.get("panel_id") == "main_panel", "FIR front_panel_ref.panel_id must be main_panel.")

    fir_unit = _expect_single_fir_unit(fir)
    lowering_unit = _expect_single_lowered_unit(lowering)
    _ensure(fir_unit.get("kind") == expected_fir_kind, f"Expected FIR unit kind {expected_fir_kind}.")
    _ensure(fir_unit.get("unit_id") == lowering_unit.get("unit_id"), "FIR unit_id must match lowering unit_id.")
    _ensure(fir_unit.get("public_interface") == lowering_unit.get("public_io"), "FIR public_interface must match lowering public_io.")
    _ensure(fir_unit.get("ui_bindings") == lowering_unit.get("ui_bindings"), "FIR ui_bindings must match lowering ui_bindings.")
    kernel = _expect_single_lowered_unit(lowering).get("execution_kernel")
    _ensure(isinstance(kernel, dict), "Missing execution_kernel.")
    _ensure(fir_unit.get("publications") == kernel.get("final_publication"), "FIR publications must match lowering final_publication.")
    return fir


def _normalized_scalar_public_io(public_io: Dict[str, Any], spec: dict[str, Any]) -> Dict[str, Any]:
    inputs = public_io["inputs"]
    outputs = public_io["outputs"]
    _ensure(isinstance(inputs, list) and len(inputs) == 1, "Scalar copy contract expects exactly one public input.")
    _ensure(isinstance(outputs, list) and len(outputs) == 1, "Scalar copy contract expects exactly one public output.")
    public_input = dict(inputs[0])
    public_output = dict(outputs[0])
    public_input["binding_origin"] = spec["input_binding_origin"]
    public_output["binding_target"] = spec["output_binding_target"]
    return {"inputs": [public_input], "outputs": [public_output]}


def _normalized_scalar_widgets(spec: dict[str, Any]) -> list[dict[str, Any]]:
    widgets = []
    for widget_id, widget_class, value_type, role, binding in spec["widgets"]:
        widgets.append({
            "widget_id": widget_id,
            "widget_class": widget_class,
            "value_type": value_type,
            "role": role,
            "binding": binding,
        })
    return widgets


def _scalar_execution_kernel(kernel: Dict[str, Any], spec: dict[str, Any]) -> Dict[str, Any]:
    out = {}
    for key in spec["kernel_keys"]:
        if key in kernel:
            out[key] = kernel[key]
    return out


def _emit_scalar_copy_contract(
    lowering: Dict[str, Any],
    unit: Dict[str, Any],
    *,
    ui_package_path: Optional[str] = None,
    lowering_path: Path | str | None = None,
) -> Dict[str, Any]:
    lowered_kind = unit.get("kind")
    spec = SCALAR_COPY_CONTRACT_SPECS.get(str(lowered_kind))
    _ensure(spec is not None, f"Unsupported lowered unit kind {lowered_kind!r}.")

    public_io = _expect_public_io(unit)
    ui_bindings = unit.get("ui_bindings")
    _ensure(isinstance(ui_bindings, dict), "Missing ui_bindings section.")
    execution_kernel = unit.get("execution_kernel")
    _ensure(isinstance(execution_kernel, dict), "Missing execution_kernel section.")

    ui_package_path_value = _infer_ui_package_path(lowering, ui_package_path)
    if lowering_path is not None:
        _load_and_validate_scalar_fir(
            lowering,
            lowering_path=lowering_path,
            ui_package_path=ui_package_path_value,
            expected_fir_kind=str(spec["contract_kind"]),
        )

    normalized_public_io = _normalized_scalar_public_io(public_io, spec)
    unit_contract = {
        "unit_id": "main",
        "kind": spec["contract_kind"],
        "public_io": normalized_public_io,
        "ui_bindings": {
            "package_refs": [ui_package_path_value],
            "widgets": _normalized_scalar_widgets(spec),
            "control_bindings": ui_bindings.get("control_bindings", []),
            "indicator_bindings": ui_bindings.get("indicator_bindings", []),
        },
        "execution_kernel": _scalar_execution_kernel(execution_kernel, spec),
        "effects": [],
        "publications": execution_kernel.get("final_publication", []),
    }

    source_ref = lowering.get("source_ref")
    fir_ref = lowering.get("fir_ref")
    _ensure(isinstance(source_ref, dict), "Missing source_ref.")
    _ensure(isinstance(fir_ref, dict), "Missing fir_ref.")

    return {
        "artifact_kind": "frog_backend_contract",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "backend_family": REFERENCE_BACKEND_FAMILY,
        "example_id": source_ref.get("example_id"),
        "artifact_refs": {
            "source_path": source_ref.get("path"),
            "fir_path": fir_ref.get("path"),
            "lowering_path": source_ref.get("path", "").replace("main.frog", "main.lowering.json"),
            "wfrog_path": ui_package_path_value,
        },
        "producer": {
            "implementation": "FROG Reference ContractEmitter",
            "implementation_kind": "non_normative_reference_emitter",
            "version_governance_ref": "Versioning/Readme.md",
        },
        "compatibility": "family_specific",
        "source_ref": {
            "example_id": source_ref.get("example_id"),
            "path": source_ref.get("path"),
            "entry_unit": source_ref.get("entry_unit"),
        },
        "derived_from": {
            "fir_path": fir_ref.get("path"),
            "lowering_path": source_ref.get("path", "").replace("main.frog", "main.lowering.json"),
            "ui_package_path": ui_package_path_value,
        },
        "ownership_boundary": {
            "semantic_authority": "canonical_source_and_execution_facing_artifacts",
            "semantic_authority_refs": [
                source_ref.get("path"),
                fir_ref.get("path"),
                source_ref.get("path", "").replace("main.frog", "main.lowering.json"),
            ],
            "ui_package_authority": "published_widget_realization_package",
            "ui_package_authority_refs": [ui_package_path_value],
            "contract_authority": "this_file",
            "notes": [
                "This contract is downstream from canonical source, FIR, lowering, and the published UI package.",
                "This contract must not redefine source semantics.",
                "If this file diverges from the canonical execution-facing artifacts, those upstream artifacts win.",
            ],
        },
        "assumptions": {
            "runtime_family": {
                "name": REFERENCE_BACKEND_FAMILY,
                "host_model": "single_process_host_runtime",
                "ui_binding": {"widget_value_binding": True, "widget_reference_binding": False},
            },
            "scheduling": {"family_rule": "deterministic_step_execution", "parallelism_claim": "none"},
            "execution_start": {"input_binding_complete": True, "ui_host_available": True, "initial_state_materialized": True},
            spec["behavior_key"]: spec["behavior"],
        },
        "units": [unit_contract],
        "unsupported": [],
        "diagnostics": [],
    }


def emit_reference_host_runtime_contract(
    lowering: Dict[str, Any],
    *,
    ui_package_path: Optional[str] = None,
    lowering_path: Path | str | None = None,
) -> Dict[str, Any]:
    _ensure(lowering.get("artifact_kind") == "frog_lowered_unit", "Expected artifact_kind == frog_lowered_unit.")

    source_ref = lowering.get("source_ref")
    fir_ref = lowering.get("fir_ref")
    _ensure(isinstance(source_ref, dict), "Missing source_ref.")
    _ensure(isinstance(fir_ref, dict), "Missing fir_ref.")

    lowering_intent = lowering.get("lowering_intent")
    _ensure(isinstance(lowering_intent, dict), "Missing lowering_intent.")
    _ensure(
        lowering_intent.get("backend_family_target") == REFERENCE_BACKEND_FAMILY,
        f"Expected lowering_intent.backend_family_target == {REFERENCE_BACKEND_FAMILY}.",
    )

    unit = _expect_single_lowered_unit(lowering)
    _ensure(unit.get("unit_id") == "main", "Expected lowered unit main.")
    if unit.get("kind") != SUPPORTED_LOWERED_UNIT_KIND:
        return _emit_scalar_copy_contract(
            lowering,
            unit,
            ui_package_path=ui_package_path,
            lowering_path=lowering_path,
        )

    public_io = _expect_public_io(unit)
    ui_bindings = _expect_ui_bindings(unit)
    execution_kernel = _expect_execution_kernel(unit)

    ui_package_path_value = _infer_ui_package_path(lowering, ui_package_path)
    if lowering_path is not None:
        _load_and_validate_fir(lowering, lowering_path=lowering_path, ui_package_path=ui_package_path_value)

    normalized_public_io = _normalized_public_io(public_io)
    normalized_execution_kernel = _normalized_execution_kernel(execution_kernel)
    widget_artifacts = _build_normalized_widget_bindings(ui_bindings, normalized_public_io, ui_package_path_value)

    unit_contract = {
        "unit_id": "main",
        "kind": CONTRACT_UNIT_KIND,
        "public_io": normalized_public_io,
        "ui_bindings": widget_artifacts["ui_bindings"],
        "execution_kernel": normalized_execution_kernel,
        "effects": widget_artifacts["effects"],
        "publications": normalized_execution_kernel["final_publication"],
    }

    return {
        "artifact_kind": "frog_backend_contract",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "backend_family": REFERENCE_BACKEND_FAMILY,
        "example_id": source_ref.get("example_id"),
        "artifact_refs": {
            "source_path": source_ref.get("path"),
            "fir_path": fir_ref.get("path"),
            "lowering_path": source_ref.get("path", "").replace("main.frog", "main.lowering.json"),
            "wfrog_path": ui_package_path_value,
        },
        "producer": {
            "implementation": "FROG Reference ContractEmitter",
            "implementation_kind": "non_normative_reference_emitter",
            "version_governance_ref": "Versioning/Readme.md",
        },
        "compatibility": "family_specific",
        "source_ref": {
            "example_id": source_ref.get("example_id"),
            "path": source_ref.get("path"),
            "entry_unit": source_ref.get("entry_unit"),
        },
        "derived_from": {
            "fir_path": fir_ref.get("path"),
            "lowering_path": source_ref.get("path", "").replace("main.frog", "main.lowering.json"),
            "ui_package_path": ui_package_path_value,
        },
        "ownership_boundary": {
            "semantic_authority": "canonical_source_and_execution_facing_artifacts",
            "semantic_authority_refs": [
                source_ref.get("path"),
                fir_ref.get("path"),
                source_ref.get("path", "").replace("main.frog", "main.lowering.json"),
            ],
            "ui_package_authority": "published_widget_realization_package",
            "ui_package_authority_refs": [ui_package_path_value],
            "contract_authority": "this_file",
            "notes": [
                "This contract is downstream from canonical source, FIR, lowering, and the published UI package.",
                "This contract must not redefine source semantics.",
                "If this file diverges from the canonical execution-facing artifacts, those upstream artifacts win.",
            ],
        },
        "assumptions": {
            "runtime_family": {
                "name": REFERENCE_BACKEND_FAMILY,
                "host_model": "single_process_host_runtime",
                "ui_binding": {"widget_value_binding": True, "widget_reference_binding": True},
            },
            "scheduling": {"family_rule": "deterministic_step_execution", "parallelism_claim": "none"},
            "execution_start": {"input_binding_complete": True, "ui_host_available": True, "initial_state_materialized": True},
            "numeric_behavior": {"value_domain": "u16", "overflow_behavior": EXPECTED_OVERFLOW_BEHAVIOR},
        },
        "units": [unit_contract],
        "unsupported": [],
        "diagnostics": [],
    }


def load_lowering_from_path(path: Path | str) -> Dict[str, Any]:
    return load_json_from_path(path)


def emit_contract_to_path(
    lowering_path: Path | str,
    output_path: Path | str,
    *,
    ui_package_path: Optional[str] = None,
) -> Dict[str, Any]:
    lowering = load_lowering_from_path(lowering_path)
    contract = emit_reference_host_runtime_contract(
        lowering,
        ui_package_path=ui_package_path,
        lowering_path=Path(lowering_path),
    )
    output = Path(output_path)
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(contract, indent=2) + "\n", encoding="utf-8")
    return contract
