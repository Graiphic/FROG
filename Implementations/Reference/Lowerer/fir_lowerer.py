"""Rule-oriented lowering helpers for the non-normative reference workspace.

This module supports the published Examples 01 through 05 through explicit
FIR-unit-kind recognition and FIR-to-lowering projection rules.

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


def base_lowering(fir: dict[str, Any], fir_rel: str, unit_id: str, purpose: str, backend_family_target: str, compiler_family_targets: list[str]) -> dict[str, Any]:
    return {
        "artifact_kind": "frog_lowered_unit",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "source_ref": source_ref(fir),
        "fir_ref": {"path": fir_rel, "unit_id": unit_id},
        "lowering_intent": {
            "purpose": purpose,
            "backend_family_target": backend_family_target,
            "compiler_family_targets": compiler_family_targets,
        },
        "lowered_units": [],
    }


def ensure_unit_kind(fir: dict[str, Any], expected: str) -> dict[str, Any]:
    u = unit(fir)
    observed = u.get("kind")
    if observed != expected:
        raise LoweringError(f"expected FIR unit kind {expected!r}, got {observed!r}")
    return u


def lower_pure_dataflow_arithmetic(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "pure_dataflow_arithmetic_unit")
    public_interface = require_object(u.get("public_interface"), "unit.public_interface")
    out = base_lowering(
        fir,
        fir_rel,
        u["unit_id"],
        "make the pure arithmetic slice consumable by simple runtime or compiler-family paths",
        "reference_pure_dataflow_arithmetic",
        ["llvm_oriented_native_path"],
    )
    out["lowered_units"] = [{
        "unit_id": u["unit_id"],
        "kind": "pure_addition_kernel",
        "public_io": public_interface,
        "execution_kernel": {
            "operations": [{"op": "add", "dst": "result", "type": "f64", "src": ["a", "b"]}],
            "final_publication": [{"target": "public_output.result", "source": "result"}],
        },
    }]
    return out


def lower_ui_value_roundtrip(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "ui_value_roundtrip_unit")
    out = base_lowering(
        fir,
        fir_rel,
        u["unit_id"],
        "make the widget-value roundtrip slice consumable by a simple UI-value runtime family",
        "reference_ui_value_roundtrip",
        [],
    )
    out["lowered_units"] = [{
        "unit_id": u["unit_id"],
        "kind": "ui_value_roundtrip_kernel",
        "public_io": u["public_interface"],
        "ui_bindings": u["ui_bindings"],
        "execution_kernel": {
            "operations": [{
                "op": "add",
                "dst": "result_value",
                "type": "f64",
                "src": ["widget.ctrl_a.value", "widget.ctrl_b.value"],
            }],
            "final_publication": [{"target": "widget.ind_result.value", "source": "result_value"}],
        },
    }]
    return out


def lower_ui_property_write(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "ui_property_write_unit")
    out = base_lowering(
        fir,
        fir_rel,
        u["unit_id"],
        "make the object-style UI property-write slice consumable by a simple UI-effect runtime family",
        "reference_ui_property_write",
        [],
    )
    out["lowered_units"] = [{
        "unit_id": u["unit_id"],
        "kind": "ui_property_write_effect_unit",
        "public_io": u["public_interface"],
        "ui_bindings": u["ui_bindings"],
        "execution_effects": [{
            "op": "frog.ui.property_write",
            "widget_id": "ctrl_gain",
            "member": "label.text",
            "value_source": "public_input.status",
        }],
    }]
    return out


def lower_stateful_feedback_delay(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "stateful_feedback_delay_unit")
    state_model = require_object(u.get("state_model"), "unit.state_model")
    carrier = require_object(state_model.get("carrier"), "unit.state_model.carrier")
    state_id = carrier.get("state_id")
    if not isinstance(state_id, str):
        raise LoweringError("stateful_feedback_delay_unit state carrier must expose state_id")
    out = base_lowering(
        fir,
        fir_rel,
        u["unit_id"],
        "make the explicit feedback delay slice consumable by a simple stateful runtime or compiler-family path",
        "reference_stateful_feedback_delay",
        ["llvm_oriented_native_path"],
    )
    out["lowered_units"] = [{
        "unit_id": u["unit_id"],
        "kind": "stateful_feedback_delay_kernel",
        "public_io": u["public_interface"],
        "execution_kernel": {
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
    }]
    return out


def lower_bounded_stateful_ui(fir: dict[str, Any], fir_rel: str) -> dict[str, Any]:
    u = ensure_unit_kind(fir, "bounded_stateful_ui_unit")
    public_interface = require_object(u.get("public_interface"), "unit.public_interface")
    state_model = require_object(u.get("state_model"), "unit.state_model")
    carrier = require_object(state_model.get("carrier"), "unit.state_model.carrier")
    execution_model = require_object(u.get("execution_model"), "unit.execution_model")
    out = base_lowering(
        fir,
        fir_rel,
        u["unit_id"],
        "make the bounded example corridor consumable by runtime families and by future compiler-family paths",
        "reference_host_runtime_ui_binding",
        ["llvm_oriented_native_path"],
    )
    out["lowered_units"] = [{
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
            "final_publication": u["publications"],
        },
    }]
    return out


LOWERING_RULES = [
    LoweringRule("lower_pure_dataflow_arithmetic", "pure_dataflow_arithmetic_unit", "pure_addition_kernel", lower_pure_dataflow_arithmetic),
    LoweringRule("lower_ui_value_roundtrip", "ui_value_roundtrip_unit", "ui_value_roundtrip_kernel", lower_ui_value_roundtrip),
    LoweringRule("lower_ui_property_write", "ui_property_write_unit", "ui_property_write_effect_unit", lower_ui_property_write),
    LoweringRule("lower_stateful_feedback_delay", "stateful_feedback_delay_unit", "stateful_feedback_delay_kernel", lower_stateful_feedback_delay),
    LoweringRule("lower_bounded_stateful_ui", "bounded_stateful_ui_unit", "bounded_accumulator_kernel_with_ui_bindings", lower_bounded_stateful_ui),
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
