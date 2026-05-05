"""Generic reference contract executor for the simple Examples 01-04 runtime family.

This module is non-normative. It interprets the published backend contracts for
Examples 01-04 through their contract shape and unit kind rather than through
hard-coded example identifiers.

Supported unit kinds:

- pure_addition_kernel
- ui_value_roundtrip_kernel
- ui_property_write_effect_unit
- stateful_feedback_delay_kernel
"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any


class ContractExecutionError(RuntimeError):
    """Raised when a reference backend contract cannot be executed."""


def load_json(path: Path) -> dict[str, Any]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ContractExecutionError(f"missing file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise ContractExecutionError(f"invalid JSON in {path}: {exc}") from exc
    if not isinstance(data, dict):
        raise ContractExecutionError(f"{path} must contain a JSON object")
    return data


def canonical_json_bytes(data: dict[str, Any]) -> bytes:
    return (json.dumps(data, sort_keys=True, separators=(",", ":")) + "\n").encode("utf-8")


def single_unit(contract: dict[str, Any]) -> dict[str, Any]:
    if contract.get("artifact_kind") != "frog_backend_contract":
        raise ContractExecutionError("contract artifact_kind must be frog_backend_contract")
    units = contract.get("units")
    if not isinstance(units, list) or len(units) != 1 or not isinstance(units[0], dict):
        raise ContractExecutionError("contract must contain exactly one unit")
    return units[0]


def get_public_input(case: dict[str, Any], name: str) -> Any:
    inputs = case.get("inputs")
    if not isinstance(inputs, dict) or name not in inputs:
        raise ContractExecutionError(f"missing public input: {name}")
    return inputs[name]


def get_widget_value(case: dict[str, Any], widget_id: str) -> Any:
    values = case.get("widget_values")
    if not isinstance(values, dict) or widget_id not in values:
        raise ContractExecutionError(f"missing widget value: {widget_id}")
    return values[widget_id]


def set_by_target(target: str, value: Any, public_outputs: dict[str, Any], widget_values: dict[str, Any]) -> None:
    if target.startswith("public_output."):
        public_outputs[target.removeprefix("public_output.")] = value
        return

    if target.startswith("widget.") and target.endswith(".value"):
        middle = target.removeprefix("widget.")
        widget_id = middle[: -len(".value")]
        widget_values[widget_id] = value
        return

    raise ContractExecutionError(f"unsupported publication target: {target}")


def resolve_reference(name: str, env: dict[str, Any], case: dict[str, Any]) -> Any:
    if name in env:
        return env[name]

    if name.startswith("public_input."):
        return get_public_input(case, name.removeprefix("public_input."))

    if name.startswith("widget.") and name.endswith(".value"):
        middle = name.removeprefix("widget.")
        widget_id = middle[: -len(".value")]
        return get_widget_value(case, widget_id)

    # Bare names in simple arithmetic kernels are public input names.
    inputs = case.get("inputs")
    if isinstance(inputs, dict) and name in inputs:
        return inputs[name]

    raise ContractExecutionError(f"unsupported value reference: {name}")


def execute_add(op: dict[str, Any], env: dict[str, Any], case: dict[str, Any]) -> Any:
    if op.get("op") != "add":
        raise ContractExecutionError(f"unsupported op: {op.get('op')!r}")

    src = op.get("src")
    if not isinstance(src, list) or len(src) != 2:
        raise ContractExecutionError("add operation requires two src entries")

    left = resolve_reference(str(src[0]), env, case)
    right = resolve_reference(str(src[1]), env, case)

    op_type = op.get("type")
    if op_type == "f64" or isinstance(left, float) or isinstance(right, float):
        result = float(left) + float(right)
    else:
        result = left + right

    dst = op.get("dst")
    if not isinstance(dst, str):
        raise ContractExecutionError("operation dst must be a string")
    env[dst] = result
    return result


def publish_all(publications: list[Any], env: dict[str, Any], public_outputs: dict[str, Any], widget_values: dict[str, Any]) -> None:
    for publication in publications:
        if not isinstance(publication, dict):
            raise ContractExecutionError("publication entries must be objects")
        target = publication.get("target")
        source = publication.get("source")
        if not isinstance(target, str) or not isinstance(source, str):
            raise ContractExecutionError("publication entries must contain target and source strings")
        set_by_target(target, resolve_reference(source, env, {}), public_outputs, widget_values)


def execute_pure_addition(contract: dict[str, Any], unit: dict[str, Any], case: dict[str, Any]) -> dict[str, Any]:
    env: dict[str, Any] = {}
    public_outputs: dict[str, Any] = {}
    widget_values: dict[str, Any] = {}

    execute_add(unit["execution"], env, case)
    publish_all(unit.get("publications", []), env, public_outputs, widget_values)

    return {
        "artifact_kind": "frog_reference_runtime_snapshot",
        "example_id": contract["example_id"],
        "status": "ok",
        "inputs": case["inputs"],
        "public_outputs": public_outputs,
    }


def execute_ui_value_roundtrip(contract: dict[str, Any], unit: dict[str, Any], case: dict[str, Any]) -> dict[str, Any]:
    env: dict[str, Any] = {}
    public_outputs: dict[str, Any] = {}
    widget_values = dict(case.get("widget_values", {}))

    # Make widget values available through case-like access.
    working_case = dict(case)
    working_case["widget_values"] = widget_values

    execute_add(unit["execution"], env, working_case)
    publish_all(unit.get("publications", []), env, public_outputs, widget_values)

    return {
        "artifact_kind": "frog_reference_runtime_snapshot",
        "example_id": contract["example_id"],
        "status": "ok",
        "widget_values": widget_values,
    }


def execute_ui_property_write(contract: dict[str, Any], unit: dict[str, Any], case: dict[str, Any]) -> dict[str, Any]:
    widget_state: dict[str, dict[str, Any]] = {}
    observed_effects: list[dict[str, Any]] = []

    effects = unit.get("effects")
    if not isinstance(effects, list):
        raise ContractExecutionError("ui_property_write_effect_unit requires effects[]")

    for effect in effects:
        if not isinstance(effect, dict):
            raise ContractExecutionError("effect entries must be objects")
        if effect.get("op") != "frog.ui.property_write":
            raise ContractExecutionError(f"unsupported effect op: {effect.get('op')!r}")

        widget_id = effect.get("widget_id")
        member = effect.get("member")
        value_source = effect.get("value_source")
        if not isinstance(widget_id, str) or not isinstance(member, str) or not isinstance(value_source, str):
            raise ContractExecutionError("property_write effects require widget_id, member, and value_source strings")

        value = resolve_reference(value_source, {}, case)
        widget_state.setdefault(widget_id, {})[member] = value
        observed_effects.append({
            "op": "frog.ui.property_write",
            "widget_id": widget_id,
            "member": member,
            "value": value,
        })

    return {
        "artifact_kind": "frog_reference_runtime_snapshot",
        "example_id": contract["example_id"],
        "status": "ok",
        "public_inputs": case["inputs"],
        "widget_state": widget_state,
        "effects": observed_effects,
    }


def execute_stateful_feedback_delay(contract: dict[str, Any], unit: dict[str, Any], case: dict[str, Any]) -> dict[str, Any]:
    kernel = unit.get("execution_kernel")
    if not isinstance(kernel, dict):
        raise ContractExecutionError("stateful_feedback_delay_kernel requires execution_kernel")

    state_id = kernel.get("state_id")
    if not isinstance(state_id, str):
        raise ContractExecutionError("stateful execution_kernel must carry state_id")

    initial_state = kernel.get("initial_state")
    env: dict[str, Any] = {
        "state_current": initial_state,
    }

    # Load public inputs into the environment by id.
    inputs = case.get("inputs")
    if not isinstance(inputs, dict):
        raise ContractExecutionError("stateful execution case requires inputs")
    for key, value in inputs.items():
        env[key] = value

    step_body = kernel.get("step_body")
    if not isinstance(step_body, list):
        raise ContractExecutionError("stateful execution_kernel requires step_body[]")

    for op in step_body:
        if not isinstance(op, dict):
            raise ContractExecutionError("step_body entries must be objects")
        execute_add(op, env, case)

    public_outputs: dict[str, Any] = {}
    widget_values: dict[str, Any] = {}
    final_publication = kernel.get("final_publication")
    if not isinstance(final_publication, list):
        raise ContractExecutionError("stateful execution_kernel requires final_publication[]")
    publish_all(final_publication, env, public_outputs, widget_values)

    state_next = env.get("state_next")
    return {
        "artifact_kind": "frog_reference_runtime_snapshot",
        "example_id": contract["example_id"],
        "status": "ok",
        "inputs": inputs,
        "initial_state": {state_id: initial_state},
        "public_outputs": public_outputs,
        "final_state": {state_id: state_next},
    }


KIND_EXECUTORS = {
    "pure_addition_kernel": execute_pure_addition,
    "ui_value_roundtrip_kernel": execute_ui_value_roundtrip,
    "ui_property_write_effect_unit": execute_ui_property_write,
    "stateful_feedback_delay_kernel": execute_stateful_feedback_delay,
}


def execute_contract_case(contract: dict[str, Any], case: dict[str, Any]) -> dict[str, Any]:
    unit = single_unit(contract)
    kind = unit.get("kind")
    if not isinstance(kind, str):
        raise ContractExecutionError("contract unit.kind must be a string")
    executor = KIND_EXECUTORS.get(kind)
    if executor is None:
        raise ContractExecutionError(f"unsupported contract unit kind: {kind}")
    return executor(contract, unit, case)


def execute_acceptance(acceptance: dict[str, Any], contract: dict[str, Any]) -> dict[str, Any]:
    cases = acceptance.get("cases")
    if not isinstance(cases, list) or len(cases) != 1 or not isinstance(cases[0], dict):
        raise ContractExecutionError("runtime acceptance currently requires exactly one case object")
    return execute_contract_case(contract, cases[0])


def check_acceptance_against_snapshot(acceptance: dict[str, Any], contract: dict[str, Any], snapshot: dict[str, Any]) -> None:
    observed = execute_acceptance(acceptance, contract)
    if canonical_json_bytes(observed) != canonical_json_bytes(snapshot):
        example_id = acceptance.get("example_id", contract.get("example_id", "<unknown>"))
        raise ContractExecutionError(f"runtime snapshot mismatch: {example_id}")
