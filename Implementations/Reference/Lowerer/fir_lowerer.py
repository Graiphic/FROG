"""Rule-oriented lowering helpers for the non-normative reference workspace."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


class LoweringError(RuntimeError):
    """Raised when a supported FIR lowering cannot proceed."""


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


def source_ref(fir: dict[str, Any]) -> dict[str, Any]:
    return require_object(fir.get("source_ref"), "fir.source_ref")


def lower_example01(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = unit(fir)
    public_interface = require_object(u.get("public_interface"), "unit.public_interface")
    return {
        "artifact_kind": "frog_lowered_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": source_ref(fir),
        "fir_ref": {"path": fir_rel, "unit_id": u["unit_id"]},
        "lowering_intent": {
            "purpose": "make the pure arithmetic slice consumable by simple runtime or compiler-family paths",
            "backend_family_target": "reference_pure_dataflow_arithmetic",
            "compiler_family_targets": ["llvm_oriented_native_path"]
        },
        "lowered_units": [{
            "unit_id": u["unit_id"],
            "kind": "pure_addition_kernel",
            "public_io": public_interface,
            "execution_kernel": {
                "operations": [{"op": "add", "dst": "result", "type": "f64", "src": ["a", "b"]}],
                "final_publication": [{"target": "public_output.result", "source": "result"}]
            }
        }]
    }


def lower_example02(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = unit(fir)
    return {
        "artifact_kind": "frog_lowered_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": source_ref(fir),
        "fir_ref": {"path": fir_rel, "unit_id": u["unit_id"]},
        "lowering_intent": {
            "purpose": "make the widget-value roundtrip slice consumable by a simple UI-value runtime family",
            "backend_family_target": "reference_ui_value_roundtrip",
            "compiler_family_targets": []
        },
        "lowered_units": [{
            "unit_id": u["unit_id"],
            "kind": "ui_value_roundtrip_kernel",
            "public_io": u["public_interface"],
            "ui_bindings": u["ui_bindings"],
            "execution_kernel": {
                "operations": [{"op": "add", "dst": "result_value", "type": "f64", "src": ["widget.ctrl_a.value", "widget.ctrl_b.value"]}],
                "final_publication": [{"target": "widget.ind_result.value", "source": "result_value"}]
            }
        }]
    }


def lower_example03(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = unit(fir)
    return {
        "artifact_kind": "frog_lowered_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": source_ref(fir),
        "fir_ref": {"path": fir_rel, "unit_id": u["unit_id"]},
        "lowering_intent": {
            "purpose": "make the object-style UI property-write slice consumable by a simple UI-effect runtime family",
            "backend_family_target": "reference_ui_property_write",
            "compiler_family_targets": []
        },
        "lowered_units": [{
            "unit_id": u["unit_id"],
            "kind": "ui_property_write_effect_unit",
            "public_io": u["public_interface"],
            "ui_bindings": u["ui_bindings"],
            "execution_effects": [{
                "op": "frog.ui.property_write",
                "widget_id": "ctrl_gain",
                "member": "label.text",
                "value_source": "public_input.status"
            }]
        }]
    }


def lower_example04(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = unit(fir)
    state_model = require_object(u.get("state_model"), "unit.state_model")
    carrier = require_object(state_model.get("carrier"), "unit.state_model.carrier")
    return {
        "artifact_kind": "frog_lowered_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": source_ref(fir),
        "fir_ref": {"path": fir_rel, "unit_id": u["unit_id"]},
        "lowering_intent": {
            "purpose": "make the explicit feedback delay slice consumable by a simple stateful runtime or compiler-family path",
            "backend_family_target": "reference_stateful_feedback_delay",
            "compiler_family_targets": ["llvm_oriented_native_path"]
        },
        "lowered_units": [{
            "unit_id": u["unit_id"],
            "kind": "stateful_feedback_delay_kernel",
            "public_io": u["public_interface"],
            "execution_kernel": {
                "initial_state": carrier["initial_value"],
                "state_type": carrier["type"],
                "step_body": [{"op": "add", "dst": "state_next", "type": carrier["type"], "src": ["state_current", "x"]}],
                "commit_rule": "state_current <- state_next after each execution step",
                "final_publication": [{"target": "public_output.y", "source": "state_next"}]
            }
        }]
    }


def lower_example05(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = unit(fir)
    public_interface = require_object(u.get("public_interface"), "unit.public_interface")
    state_model = require_object(u.get("state_model"), "unit.state_model")
    carrier = require_object(state_model.get("carrier"), "unit.state_model.carrier")
    execution_model = require_object(u.get("execution_model"), "unit.execution_model")
    return {
        "artifact_kind": "frog_lowered_unit",
        "artifact_version": "0.1-draft",
        "source_ref": source_ref(fir),
        "fir_ref": {"path": fir_rel, "unit_id": u["unit_id"]},
        "lowering_intent": {
            "purpose": "make the bounded example corridor consumable by runtime families and by future compiler-family paths",
            "backend_family_target": "reference_host_runtime_ui_binding",
            "compiler_family_targets": ["llvm_oriented_native_path"]
        },
        "lowered_units": [{
            "unit_id": u["unit_id"],
            "kind": "bounded_accumulator_kernel_with_ui_bindings",
            "public_io": public_interface,
            "ui_bindings": u["ui_bindings"],
            "execution_kernel": {
                "initial_state": carrier["initial_value"],
                "state_type": carrier["type"],
                "iteration_count": execution_model["iteration_count"],
                "iteration_body": [{"op": "add", "dst": "state_next", "src": ["state_current", "input_value"]}],
                "commit_rule": "state_current <- state_next after each iteration",
                "final_publication": u["publications"]
            }
        }]
    }


def lower_fir_artifact(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    if fir.get("artifact_kind") != "frog_fir_unit":
        raise LoweringError("input artifact_kind must be frog_fir_unit")
    example_id = source_ref(fir).get("example_id")
    if example_id == "01_pure_addition":
        return lower_example01(fir, fir_rel)
    if example_id == "02_ui_value_roundtrip":
        return lower_example02(fir, fir_rel)
    if example_id == "03_ui_property_write":
        return lower_example03(fir, fir_rel)
    if example_id == "04_stateful_feedback_delay":
        return lower_example04(fir, fir_rel)
    if example_id == "05_bounded_ui_accumulator":
        return lower_example05(fir, fir_rel)
    raise LoweringError(f"unsupported FIR example for lowering: {example_id!r}")
