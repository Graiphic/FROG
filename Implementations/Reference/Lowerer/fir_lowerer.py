"""Rule-oriented lowering helpers for the non-normative reference workspace.

This module supports the published Examples 01 through 10 and the
post-boundary Example 16 Picture slice through explicit FIR-unit-kind
recognition and FIR-to-lowering projection rules.

It is intentionally narrow and does not claim general FROG compiler
completeness. The important boundary is that lowering rule selection is based
on the FIR unit kind and required FIR structure, not on trusting an example id
as the semantic lowering authority.
"""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any, Callable


class LoweringError(RuntimeError):
    """Raised when a supported FIR lowering cannot proceed."""


@dataclass(frozen=True)
class LoweringRule:
    """Bounded reference lowering rule for one supported FIR unit kind."""

    rule_id: str
    fir_unit_kind: str
    lowered_unit_kind: str
    lower: Callable[[dict[str, Any], str], dict[str, Any]]


def load_json(path: Path) -> dict[str, Any]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise LoweringError(f"missing file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise LoweringError(f"invalid JSON in {path}: {exc}") from exc
    if not isinstance(data, dict):
        raise LoweringError(f"{path} must contain a JSON object")
    return data


def dump_json(data: dict[str, Any], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(data, indent=2) + "\n", encoding="utf-8")


def canonical_json_bytes(data: dict[str, Any]) -> bytes:
    return (json.dumps(data, sort_keys=True, separators=(",", ":")) + "\n").encode("utf-8")


def require_object(value: Any, name: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise LoweringError(f"{name} must be an object")
    return value


def require_list(value: Any, name: str) -> list[Any]:
    if not isinstance(value, list):
        raise LoweringError(f"{name} must be an array")
    return value


def unit(fir: dict[str, Any]) -> dict[str, Any]:
    units = require_list(fir.get("units"), "fir.units")
    if len(units) != 1:
        raise LoweringError(f"expected one FIR unit, found {len(units)}")
    return require_object(units[0], "fir.units[0]")


def fir_unit_kind(fir: dict[str, Any]) -> str:
    kind = unit(fir).get("kind")
    if not isinstance(kind, str):
        raise LoweringError("FIR unit must expose string kind")
    return kind


def source_ref(fir: dict[str, Any]) -> dict[str, Any]:
    return require_object(fir.get("source_ref"), "fir.source_ref")


def ensure_unit_kind(fir: dict[str, Any], expected: str) -> dict[str, Any]:
    u = unit(fir)
    observed = u.get("kind")
    if observed != expected:
        raise LoweringError(f"expected FIR unit kind {expected!r}, got {observed!r}")
    return u


def unit_id(u: dict[str, Any]) -> str:
    value = u.get("unit_id")
    if not isinstance(value, str):
        raise LoweringError("FIR unit must expose string unit_id")
    return value


def public_interface(u: dict[str, Any]) -> dict[str, Any]:
    return require_object(u.get("public_interface"), "unit.public_interface")


def ui_bindings(u: dict[str, Any]) -> dict[str, Any]:
    return require_object(u.get("ui_bindings"), "unit.ui_bindings")


def state_carrier(u: dict[str, Any]) -> dict[str, Any]:
    state_model = require_object(u.get("state_model"), "unit.state_model")
    return require_object(state_model.get("carrier"), "unit.state_model.carrier")


def execution_model(u: dict[str, Any]) -> dict[str, Any]:
    return require_object(u.get("execution_model"), "unit.execution_model")


def base_lowering(fir: dict[str, Any], fir_rel: str, unit_id_value: str, purpose: str, backend_family_target: str, compiler_family_targets: list[str]) -> dict[str, Any]:
    return {
        "artifact_kind": "frog_lowered_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": source_ref(fir),
        "fir_ref": {"path": fir_rel, "unit_id": unit_id_value},
        "lowering_intent": {
            "purpose": purpose,
            "backend_family_target": backend_family_target,
            "compiler_family_targets": compiler_family_targets,
        },
        "lowered_units": [],
    }


def single_lowered_unit(unit_id_value: str, kind: str, **sections: Any) -> dict[str, Any]:
    out: dict[str, Any] = {"unit_id": unit_id_value, "kind": kind}
    out.update(sections)
    return out


def with_lowered_unit(lowering: dict[str, Any], lowered_unit: dict[str, Any]) -> dict[str, Any]:
    lowering["lowered_units"] = [lowered_unit]
    return lowering


def lower_pure_dataflow_arithmetic(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "pure_dataflow_arithmetic_unit")
    uid = unit_id(u)
    out = base_lowering(
        fir,
        fir_rel,
        uid,
        "make the pure arithmetic slice consumable by simple runtime or compiler-family paths",
        "reference_pure_dataflow_arithmetic",
        ["llvm_oriented_native_path"],
    )
    return with_lowered_unit(out, single_lowered_unit(
        uid,
        "pure_addition_kernel",
        public_io=public_interface(u),
        execution_kernel={
            "operations": [{"op": "add", "dst": "result", "type": "f64", "src": ["a", "b"]}],
            "final_publication": [{"target": "public_output.result", "source": "result"}],
        },
    ))


def lower_ui_value_roundtrip(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "ui_value_roundtrip_unit")
    uid = unit_id(u)
    out = base_lowering(
        fir,
        fir_rel,
        uid,
        "make the widget-value roundtrip slice consumable by a simple UI-value runtime family",
        "reference_ui_value_roundtrip",
        [],
    )
    return with_lowered_unit(out, single_lowered_unit(
        uid,
        "ui_value_roundtrip_kernel",
        public_io=public_interface(u),
        ui_bindings=ui_bindings(u),
        execution_kernel={
            "operations": [{
                "op": "add",
                "dst": "result_value",
                "type": "f64",
                "src": ["widget.ctrl_a.value", "widget.ctrl_b.value"],
            }],
            "final_publication": [{"target": "widget.ind_result.value", "source": "result_value"}],
        },
    ))


def lower_ui_property_write(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "ui_property_write_unit")
    uid = unit_id(u)
    out = base_lowering(
        fir,
        fir_rel,
        uid,
        "make the object-style UI property-write slice consumable by a simple UI-effect runtime family",
        "reference_ui_property_write",
        [],
    )
    return with_lowered_unit(out, single_lowered_unit(
        uid,
        "ui_property_write_effect_unit",
        public_io=public_interface(u),
        ui_bindings=ui_bindings(u),
        execution_effects=[{
            "op": "frog.ui.property_write",
            "widget_id": "ctrl_gain",
            "member": "label.text",
            "value_source": "public_input.status",
        }],
    ))


def lower_stateful_feedback_delay(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "stateful_feedback_delay_unit")
    uid = unit_id(u)
    carrier = state_carrier(u)
    state_id = carrier.get("state_id")
    if not isinstance(state_id, str):
        raise LoweringError("stateful_feedback_delay_unit state carrier must expose state_id")
    out = base_lowering(
        fir,
        fir_rel,
        uid,
        "make the explicit feedback delay slice consumable by a simple stateful runtime or compiler-family path",
        "reference_stateful_feedback_delay",
        ["llvm_oriented_native_path"],
    )
    return with_lowered_unit(out, single_lowered_unit(
        uid,
        "stateful_feedback_delay_kernel",
        public_io=public_interface(u),
        execution_kernel={
            "state_id": state_id,
            "initial_state": carrier["initial_value"],
            "state_type": carrier["type"],
            "step_body": [{
                "op": "add",
                "dst": "state_next",
                "type": carrier["type"],
                "src": ["state_current", "x"],
            }],
            "commit_rule": "state_current <- state_next after each execution step",
            "final_publication": [{"target": "public_output.y", "source": "state_next"}],
        },
    ))


def lower_bounded_stateful_ui(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "bounded_stateful_ui_unit")
    uid = unit_id(u)
    carrier = state_carrier(u)
    model = execution_model(u)
    out = base_lowering(
        fir,
        fir_rel,
        uid,
        "make the bounded example corridor consumable by runtime families and by future compiler-family paths",
        "reference_host_runtime_ui_binding",
        ["llvm_oriented_native_path"],
    )
    return with_lowered_unit(out, single_lowered_unit(
        uid,
        "bounded_accumulator_kernel_with_ui_bindings",
        public_io=public_interface(u),
        ui_bindings=ui_bindings(u),
        execution_kernel={
            "initial_state": carrier["initial_value"],
            "state_type": carrier["type"],
            "iteration_count": model["iteration_count"],
            "iteration_body": [{"op": "add", "dst": "state_next", "src": ["state_current", "input_value"]}],
            "commit_rule": "state_current <- state_next after each iteration",
            "final_publication": u["publications"],
        },
    ))


def lower_boolean_value_roundtrip(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "boolean_value_roundtrip_ui_unit")
    uid = unit_id(u)
    bindings = ui_bindings(u)
    expect_control = [{"widget_id": "bool_input", "mode": "widget_value", "public_input_id": "input_value", "value_type": "bool"}]
    expect_indicator = [{"widget_id": "bool_result", "mode": "widget_value", "public_output_id": "result", "value_type": "bool"}]
    if bindings.get("control_bindings") != expect_control or bindings.get("indicator_bindings") != expect_indicator:
        raise LoweringError("boolean_value_roundtrip_ui_unit bindings must map bool_input to input_value and bool_result to result")
    out = base_lowering(
        fir,
        fir_rel,
        uid,
        "make the Boolean value roundtrip consumable by a bounded runtime/UI-binding check",
        "reference_host_runtime_ui_binding",
        ["llvm_native_kernel_bridge"],
    )
    return with_lowered_unit(out, single_lowered_unit(
        uid,
        "boolean_value_roundtrip_kernel_with_ui_bindings",
        public_io=public_interface(u),
        ui_bindings=bindings,
        execution_kernel={
            "operation": "copy",
            "dst": "result",
            "type": "bool",
            "src": "input_value",
            "final_publication": u["publications"],
        },
    ))


def lower_string_value_roundtrip(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "string_value_roundtrip_ui_unit")
    uid = unit_id(u)
    bindings = ui_bindings(u)
    expect_control = [{"widget_id": "str_input", "mode": "widget_value", "public_input_id": "input_text", "value_type": "string"}]
    expect_indicator = [{"widget_id": "str_result", "mode": "widget_value", "public_output_id": "result_text", "value_type": "string"}]
    if bindings.get("control_bindings") != expect_control or bindings.get("indicator_bindings") != expect_indicator:
        raise LoweringError("string_value_roundtrip_ui_unit bindings must map str_input to input_text and str_result to result_text")
    out = base_lowering(
        fir,
        fir_rel,
        uid,
        "make the string value roundtrip consumable by a bounded runtime/UI-binding check",
        "reference_host_runtime_ui_binding",
        ["llvm_native_kernel_bridge"],
    )
    return with_lowered_unit(out, single_lowered_unit(
        uid,
        "string_value_roundtrip_kernel_with_ui_bindings",
        public_io=public_interface(u),
        ui_bindings=bindings,
        execution_kernel={
            "operation": "copy",
            "dst": "result_text",
            "type": "string",
            "src": "input_text",
            "max_utf8_bytes": 256,
            "final_publication": u["publications"],
        },
    ))


def lower_enum_value_roundtrip(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "enum_value_roundtrip_ui_unit")
    uid = unit_id(u)
    bindings = ui_bindings(u)
    expect_control = [{
        "widget_id": "mode_input",
        "mode": "widget_value",
        "public_input_id": "mode_value",
        "value_type": "enum_item_id",
        "enum_domain": "example08.mode",
    }]
    expect_indicator = [{
        "widget_id": "mode_result",
        "mode": "widget_value",
        "public_output_id": "result_mode",
        "value_type": "enum_item_id",
        "enum_domain": "example08.mode",
    }]
    if bindings.get("control_bindings") != expect_control or bindings.get("indicator_bindings") != expect_indicator:
        raise LoweringError("enum_value_roundtrip_ui_unit bindings must map mode_input to mode_value and mode_result to result_mode")
    out = base_lowering(
        fir,
        fir_rel,
        uid,
        "make the enum value roundtrip consumable by a bounded runtime/UI-binding check",
        "reference_host_runtime_ui_binding",
        ["llvm_native_kernel_bridge"],
    )
    return with_lowered_unit(out, single_lowered_unit(
        uid,
        "enum_value_roundtrip_kernel_with_ui_bindings",
        public_io=public_interface(u),
        ui_bindings=bindings,
        execution_kernel={
            "operation": "copy",
            "dst": "result_mode",
            "type": "enum_item_id",
            "representation": "u16",
            "src": "mode_value",
            "enum_domain": "example08.mode",
            "allowed_values": [
                {"id": "idle", "numeric_value": 0},
                {"id": "run", "numeric_value": 1},
                {"id": "fault", "numeric_value": 2},
            ],
            "final_publication": u["publications"],
        },
    ))


def lower_path_value_roundtrip(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "path_value_roundtrip_ui_unit")
    uid = unit_id(u)
    bindings = ui_bindings(u)
    expect_control = [{"widget_id": "path_input", "mode": "widget_value", "public_input_id": "input_path", "value_type": "path"}]
    expect_indicator = [{"widget_id": "path_result", "mode": "widget_value", "public_output_id": "result_path", "value_type": "path"}]
    if bindings.get("control_bindings") != expect_control or bindings.get("indicator_bindings") != expect_indicator:
        raise LoweringError("path_value_roundtrip_ui_unit bindings must map path_input to input_path and path_result to result_path")
    out = base_lowering(
        fir,
        fir_rel,
        uid,
        "make the path value roundtrip consumable by a bounded runtime/UI-binding check",
        "reference_host_runtime_ui_binding",
        ["llvm_native_kernel_bridge"],
    )
    return with_lowered_unit(out, single_lowered_unit(
        uid,
        "path_value_roundtrip_kernel_with_ui_bindings",
        public_io=public_interface(u),
        ui_bindings=bindings,
        execution_kernel={
            "operation": "copy",
            "dst": "result_path",
            "type": "path",
            "src": "input_path",
            "encoding": "utf8",
            "max_utf8_bytes": 256,
            "final_publication": u["publications"],
        },
    ))


def lower_button_press_to_boolean(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "button_press_to_boolean_ui_unit")
    uid = unit_id(u)
    bindings = ui_bindings(u)
    expect_control = [{
        "widget_id": "trigger_button",
        "mode": "widget_event_value",
        "event": "pressed",
        "public_input_id": "trigger_pressed",
        "value_type": "bool",
    }]
    expect_indicator = [{"widget_id": "pressed_indicator", "mode": "widget_value", "public_output_id": "pressed", "value_type": "bool"}]
    if bindings.get("control_bindings") != expect_control or bindings.get("indicator_bindings") != expect_indicator:
        raise LoweringError("button_press_to_boolean_ui_unit bindings must map trigger_button.pressed to pressed_indicator")
    return {
        "artifact_kind": "frog_lowered_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": source_ref(fir),
        "fir_ref": {"path": fir_rel, "unit_id": uid},
        "lowering_intent": {
            "summary": "make the Button press-to-Boolean example consumable by a bounded C++ runtime/UI-binding check",
            "backend_family_target": "reference_host_runtime_ui_binding",
            "capability_targets": ["llvm_native_kernel_bridge"],
        },
        "lowered_units": [single_lowered_unit(
            uid,
            "button_press_to_boolean_kernel_with_ui_bindings",
            public_io=public_interface(u),
            ui_bindings=bindings,
            execution_kernel={
                "operation": "copy",
                "dst": "pressed",
                "type": "bool",
                "src": "trigger_pressed",
                "final_publication": u["publications"],
            },
        )],
    }


BUTTON_MECHANICAL_LOWERING: dict[str, dict[str, str]] = {
    "button_switch_when_pressed_ui_unit": {
        "lowered_kind": "button_switch_when_pressed_kernel_with_ui_bindings",
        "action": "switch_when_pressed",
        "dst": "switched",
        "indicator_id": "switched_indicator",
    },
    "button_switch_when_released_ui_unit": {
        "lowered_kind": "button_switch_when_released_kernel_with_ui_bindings",
        "action": "switch_when_released",
        "dst": "switched",
        "indicator_id": "switched_indicator",
    },
    "button_latch_when_pressed_ui_unit": {
        "lowered_kind": "button_latch_when_pressed_kernel_with_ui_bindings",
        "action": "latch_when_pressed",
        "dst": "latched",
        "indicator_id": "latched_indicator",
    },
    "button_latch_when_released_ui_unit": {
        "lowered_kind": "button_latch_when_released_kernel_with_ui_bindings",
        "action": "latch_when_released",
        "dst": "latched",
        "indicator_id": "latched_indicator",
    },
    "button_latch_until_released_ui_unit": {
        "lowered_kind": "button_latch_until_released_kernel_with_ui_bindings",
        "action": "latch_until_released",
        "dst": "latched",
        "indicator_id": "latched_indicator",
    },
}


def lower_button_mechanical_value(fir: dict[str, Any], fir_rel: str, fir_kind: str) -> dict[str, Any]:
    spec = BUTTON_MECHANICAL_LOWERING[fir_kind]
    u = ensure_unit_kind(fir, fir_kind)
    uid = unit_id(u)
    bindings = ui_bindings(u)
    dst = spec["dst"]
    expect_control = [{"widget_id": "trigger_button", "mode": "widget_value", "public_input_id": "trigger_value", "value_type": "bool"}]
    expect_indicator = [{"widget_id": spec["indicator_id"], "mode": "widget_value", "public_output_id": dst, "value_type": "bool"}]
    if bindings.get("control_bindings") != expect_control or bindings.get("indicator_bindings") != expect_indicator:
        raise LoweringError(f"{fir_kind} bindings must map trigger_button.value to {spec['indicator_id']}")
    model = execution_model(u)
    body_rule = require_object(model.get("body_rule"), f"{fir_kind}.execution_model.body_rule")
    if body_rule.get("kind") != "copy_widget_value_to_output" or body_rule.get("expression") != f"{dst} = trigger_value":
        raise LoweringError(f"{fir_kind} must copy trigger_value to {dst}")
    return {
        "artifact_kind": "frog_lowered_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": source_ref(fir),
        "fir_ref": {"path": fir_rel, "unit_id": uid},
        "lowering_intent": {
            "summary": f"make one bounded Button {spec['action']} value example consumable by C++/Python/Rust reference runtime UI-binding checks",
            "backend_family_target": "reference_host_runtime_ui_binding",
            "capability_targets": [
                "cpp_contract_executor",
                "python_contract_executor",
                "rust_contract_executor",
                "llvm_native_kernel_bridge",
            ],
        },
        "lowered_units": [single_lowered_unit(
            uid,
            spec["lowered_kind"],
            public_io=public_interface(u),
            ui_bindings=bindings,
            execution_kernel={
                "operation": "copy",
                "dst": dst,
                "type": "bool",
                "src": "trigger_value",
                "final_publication": u["publications"],
            },
        )],
    }


def lower_button_switch_when_pressed(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    return lower_button_mechanical_value(fir, fir_rel, "button_switch_when_pressed_ui_unit")


def lower_button_switch_when_released(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    return lower_button_mechanical_value(fir, fir_rel, "button_switch_when_released_ui_unit")


def lower_button_latch_when_pressed(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    return lower_button_mechanical_value(fir, fir_rel, "button_latch_when_pressed_ui_unit")


def lower_button_latch_when_released(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    return lower_button_mechanical_value(fir, fir_rel, "button_latch_when_released_ui_unit")


def lower_button_latch_until_released(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    return lower_button_mechanical_value(fir, fir_rel, "button_latch_until_released_ui_unit")


def lower_picture_path_to_image(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "picture_path_to_image_ui_unit")
    uid = unit_id(u)
    bindings = ui_bindings(u)
    expect_control = [{"widget_id": "image_path", "mode": "widget_value", "public_input_id": "image_path", "value_type": "path"}]
    expect_indicator = [{"widget_id": "preview_picture", "mode": "decoded_image_value", "public_output_id": "preview_image", "value_type": "frog.image.buffer_rgba8"}]
    if bindings.get("control_bindings") != expect_control or bindings.get("indicator_bindings") != expect_indicator:
        raise LoweringError("picture_path_to_image_ui_unit bindings must map image_path to preview_picture through preview_image")
    model = execution_model(u)
    body_rule = require_object(model.get("body_rule"), "picture_path_to_image_ui_unit.execution_model.body_rule")
    if body_rule.get("kind") != "call_standard_primitive" or body_rule.get("primitive") != "frog.image.decode_file_rgba8":
        raise LoweringError("picture_path_to_image_ui_unit must call frog.image.decode_file_rgba8")
    return {
        "artifact_kind": "frog_lowered_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": source_ref(fir),
        "fir_ref": {"path": fir_rel, "unit_id": uid},
        "lowering_intent": {
            "summary": "make one post-boundary Picture path-to-image example consumable through a manifest-declared LLVM/native kernel boundary and standard-library image provider",
            "backend_family_target": "llvm_native_kernel_with_standard_provider",
            "capability_targets": [
                "standard_library:frog.image.decode_file_rgba8",
                "native_kernel_manifest",
                "standard_provider_dependency:frog.image",
                "private_runtime_path_widget_binding",
                "private_runtime_picture_binding",
            ],
            "native_artifact_ref": {
                "manifest": "Implementations/Reference/LLVM/examples/16_picture_logo_jpeg/native_kernel_manifest.json",
                "llvm_ir": "Implementations/Reference/LLVM/examples/16_picture_logo_jpeg/kernel.ll",
                "provider_dependency": "frog.image",
            },
            "public_reference_runtime_source_added": False,
        },
        "lowered_units": [single_lowered_unit(
            uid,
            "picture_path_to_image_with_ui_bindings",
            public_io=public_interface(u),
            ui_bindings=bindings,
            execution_kernel={
                "operation": "call_manifest_declared_standard_primitive",
                "primitive": "frog.image.decode_file_rgba8",
                "native_manifest": "Implementations/Reference/LLVM/examples/16_picture_logo_jpeg/native_kernel_manifest.json",
                "inputs": {"path": "widget.image_path.value"},
                "outputs": {"image": "preview_image", "success": "decode_success", "error_code": "decode_error_code"},
                "final_publication": u["publications"],
            },
        )],
    }


LOWERING_RULES = [
    LoweringRule("lower_pure_dataflow_arithmetic", "pure_dataflow_arithmetic_unit", "pure_addition_kernel", lower_pure_dataflow_arithmetic),
    LoweringRule("lower_ui_value_roundtrip", "ui_value_roundtrip_unit", "ui_value_roundtrip_kernel", lower_ui_value_roundtrip),
    LoweringRule("lower_ui_property_write", "ui_property_write_unit", "ui_property_write_effect_unit", lower_ui_property_write),
    LoweringRule("lower_stateful_feedback_delay", "stateful_feedback_delay_unit", "stateful_feedback_delay_kernel", lower_stateful_feedback_delay),
    LoweringRule("lower_bounded_stateful_ui", "bounded_stateful_ui_unit", "bounded_accumulator_kernel_with_ui_bindings", lower_bounded_stateful_ui),
    LoweringRule("lower_boolean_value_roundtrip", "boolean_value_roundtrip_ui_unit", "boolean_value_roundtrip_kernel_with_ui_bindings", lower_boolean_value_roundtrip),
    LoweringRule("lower_string_value_roundtrip", "string_value_roundtrip_ui_unit", "string_value_roundtrip_kernel_with_ui_bindings", lower_string_value_roundtrip),
    LoweringRule("lower_enum_value_roundtrip", "enum_value_roundtrip_ui_unit", "enum_value_roundtrip_kernel_with_ui_bindings", lower_enum_value_roundtrip),
    LoweringRule("lower_path_value_roundtrip", "path_value_roundtrip_ui_unit", "path_value_roundtrip_kernel_with_ui_bindings", lower_path_value_roundtrip),
    LoweringRule("lower_button_press_to_boolean", "button_press_to_boolean_ui_unit", "button_press_to_boolean_kernel_with_ui_bindings", lower_button_press_to_boolean),
    LoweringRule("lower_button_switch_when_pressed", "button_switch_when_pressed_ui_unit", "button_switch_when_pressed_kernel_with_ui_bindings", lower_button_switch_when_pressed),
    LoweringRule("lower_button_switch_when_released", "button_switch_when_released_ui_unit", "button_switch_when_released_kernel_with_ui_bindings", lower_button_switch_when_released),
    LoweringRule("lower_button_latch_when_pressed", "button_latch_when_pressed_ui_unit", "button_latch_when_pressed_kernel_with_ui_bindings", lower_button_latch_when_pressed),
    LoweringRule("lower_button_latch_when_released", "button_latch_when_released_ui_unit", "button_latch_when_released_kernel_with_ui_bindings", lower_button_latch_when_released),
    LoweringRule("lower_button_latch_until_released", "button_latch_until_released_ui_unit", "button_latch_until_released_kernel_with_ui_bindings", lower_button_latch_until_released),
    LoweringRule("lower_picture_path_to_image", "picture_path_to_image_ui_unit", "picture_path_to_image_with_ui_bindings", lower_picture_path_to_image),
]


def rule_for_fir_unit_kind(kind: str) -> LoweringRule:
    matches = [rule for rule in LOWERING_RULES if rule.fir_unit_kind == kind]
    if len(matches) == 1:
        return matches[0]
    if len(matches) > 1:
        matched_rules = ", ".join(rule.rule_id for rule in matches)
        raise LoweringError(f"ambiguous lowering rules for FIR unit kind {kind!r}: {matched_rules}")
    supported = ", ".join(rule.fir_unit_kind for rule in LOWERING_RULES)
    raise LoweringError(f"unsupported FIR unit kind for lowering: {kind!r}; supported: {supported}")


def lower_fir_artifact(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    if fir.get("artifact_kind") != "frog_fir_unit":
        raise LoweringError("input artifact_kind must be frog_fir_unit")
    kind = fir_unit_kind(fir)
    rule = rule_for_fir_unit_kind(kind)
    lowered = rule.lower(fir, fir_rel)
    lowered_unit = unit_like_single_lowered_unit(lowered)
    observed_lowered_kind = lowered_unit.get("kind")
    if observed_lowered_kind != rule.lowered_unit_kind:
        raise LoweringError(
            f"lowering rule {rule.rule_id} produced lowered unit kind {observed_lowered_kind!r}, expected {rule.lowered_unit_kind!r}"
        )
    return lowered


def unit_like_single_lowered_unit(lowering: dict[str, Any]) -> dict[str, Any]:
    if lowering.get("artifact_kind") != "frog_lowered_unit":
        raise LoweringError("lowering artifact_kind must be frog_lowered_unit")
    lowered_units = require_list(lowering.get("lowered_units"), "lowering.lowered_units")
    if len(lowered_units) != 1:
        raise LoweringError(f"expected one lowered unit, found {len(lowered_units)}")
    return require_object(lowered_units[0], "lowering.lowered_units[0]")
