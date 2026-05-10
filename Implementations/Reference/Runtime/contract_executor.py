"""Generic reference contract executor for the non-normative FROG runtime workspace."""

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


def require_object(value: Any, name: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise ContractExecutionError(f"{name} must be an object")
    return value


def require_list(value: Any, name: str) -> list[Any]:
    if not isinstance(value, list):
        raise ContractExecutionError(f"{name} must be an array")
    return value


def contract_example_id(contract: dict[str, Any]) -> str:
    example_id = contract.get("example_id")
    if isinstance(example_id, str):
        return example_id
    source_ref = contract.get("source_ref")
    if isinstance(source_ref, dict) and isinstance(source_ref.get("example_id"), str):
        return source_ref["example_id"]
    raise ContractExecutionError("contract must expose example_id or source_ref.example_id")


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


def execute_pure_addition(contract: dict[str, Any], unit: dict[str, Any], case: dict[str, Any], support_artifacts: dict[str, Any] | None = None) -> dict[str, Any]:
    env: dict[str, Any] = {}
    public_outputs: dict[str, Any] = {}
    widget_values: dict[str, Any] = {}
    execute_add(unit["execution"], env, case)
    publish_all(unit.get("publications", []), env, public_outputs, widget_values)
    return {"artifact_kind": "frog_reference_runtime_snapshot", "example_id": contract_example_id(contract), "status": "ok", "inputs": case["inputs"], "public_outputs": public_outputs}


def execute_ui_value_roundtrip(contract: dict[str, Any], unit: dict[str, Any], case: dict[str, Any], support_artifacts: dict[str, Any] | None = None) -> dict[str, Any]:
    env: dict[str, Any] = {}
    public_outputs: dict[str, Any] = {}
    widget_values = dict(case.get("widget_values", {}))
    working_case = dict(case)
    working_case["widget_values"] = widget_values
    execute_add(unit["execution"], env, working_case)
    publish_all(unit.get("publications", []), env, public_outputs, widget_values)
    return {"artifact_kind": "frog_reference_runtime_snapshot", "example_id": contract_example_id(contract), "status": "ok", "widget_values": widget_values}


def execute_ui_property_write(contract: dict[str, Any], unit: dict[str, Any], case: dict[str, Any], support_artifacts: dict[str, Any] | None = None) -> dict[str, Any]:
    widget_state: dict[str, dict[str, Any]] = {}
    observed_effects: list[dict[str, Any]] = []
    effects = unit.get("effects")
    if not isinstance(effects, list):
        raise ContractExecutionError("ui_property_write_effect_unit requires effects[]")
    for effect in effects:
        obj = require_object(effect, "effect")
        if obj.get("op") != "frog.ui.property_write":
            raise ContractExecutionError(f"unsupported effect op: {obj.get('op')!r}")
        widget_id = obj.get("widget_id")
        member = obj.get("member")
        value_source = obj.get("value_source")
        if not isinstance(widget_id, str) or not isinstance(member, str) or not isinstance(value_source, str):
            raise ContractExecutionError("property_write effects require widget_id, member, and value_source strings")
        value = resolve_reference(value_source, {}, case)
        widget_state.setdefault(widget_id, {})[member] = value
        observed_effects.append({"op": "frog.ui.property_write", "widget_id": widget_id, "member": member, "value": value})
    return {"artifact_kind": "frog_reference_runtime_snapshot", "example_id": contract_example_id(contract), "status": "ok", "public_inputs": case["inputs"], "widget_state": widget_state, "effects": observed_effects}


def execute_stateful_feedback_delay(contract: dict[str, Any], unit: dict[str, Any], case: dict[str, Any], support_artifacts: dict[str, Any] | None = None) -> dict[str, Any]:
    kernel = require_object(unit.get("execution_kernel"), "unit.execution_kernel")
    state_id = kernel.get("state_id")
    if not isinstance(state_id, str):
        raise ContractExecutionError("stateful execution_kernel must carry state_id")
    initial_state = kernel.get("initial_state")
    env: dict[str, Any] = {"state_current": initial_state}
    inputs = case.get("inputs")
    if not isinstance(inputs, dict):
        raise ContractExecutionError("stateful execution case requires inputs")
    for key, value in inputs.items():
        env[key] = value
    step_body = require_list(kernel.get("step_body"), "unit.execution_kernel.step_body")
    for op in step_body:
        execute_add(require_object(op, "step_body[]"), env, case)
    public_outputs: dict[str, Any] = {}
    widget_values: dict[str, Any] = {}
    publish_all(require_list(kernel.get("final_publication"), "unit.execution_kernel.final_publication"), env, public_outputs, widget_values)
    state_next = env.get("state_next")
    return {"artifact_kind": "frog_reference_runtime_snapshot", "example_id": contract_example_id(contract), "status": "ok", "inputs": inputs, "initial_state": {state_id: initial_state}, "public_outputs": public_outputs, "final_state": {state_id: state_next}}


def case_for_bounded_ui_acceptance(acceptance: dict[str, Any]) -> dict[str, Any]:
    headless = acceptance.get("headless")
    if isinstance(headless, dict) and isinstance(headless.get("input_value"), int):
        value = headless["input_value"]
        return {"inputs": {"input_value": value}, "widget_values": {"ctrl_input": value}}
    cases = acceptance.get("cases")
    if isinstance(cases, list) and len(cases) == 1 and isinstance(cases[0], dict):
        return cases[0]
    raise ContractExecutionError("bounded UI acceptance requires headless.input_value or one case object")


def checked_u16_add(left: int, right: int) -> int:
    result = left + right
    if result > 65535:
        raise ContractExecutionError("final_state must remain in the u16 domain.")
    return result


def wfrog_main_panel(wfrog: dict[str, Any]) -> dict[str, Any]:
    if wfrog.get("format") != "frog.wfrog" or wfrog.get("kind") != "front_panel_package":
        raise ContractExecutionError("Example 05 generic execution requires a frog.wfrog front_panel_package")
    panels = require_list(wfrog.get("front_panels"), "wfrog.front_panels")
    if len(panels) != 1:
        raise ContractExecutionError("Example 05 generic execution expects exactly one front panel")
    return require_object(panels[0], "wfrog.front_panels[0]")


def property_write_value(effect: dict[str, Any]) -> Any:
    value = effect.get("value")
    if isinstance(value, dict) and "value" in value:
        return value["value"]
    return value


def execute_bounded_executable_ui_unit(contract: dict[str, Any], unit: dict[str, Any], case: dict[str, Any], support_artifacts: dict[str, Any] | None = None) -> dict[str, Any]:
    support_artifacts = support_artifacts or {}
    wfrog = support_artifacts.get("wfrog")
    if not isinstance(wfrog, dict):
        raise ContractExecutionError("bounded_executable_ui_unit requires support_artifacts['wfrog']")

    assumptions = require_object(contract.get("assumptions"), "contract.assumptions")
    numeric_behavior = require_object(assumptions.get("numeric_behavior"), "contract.assumptions.numeric_behavior")
    if numeric_behavior.get("value_domain") != "u16":
        raise ContractExecutionError("bounded_executable_ui_unit currently supports only u16")
    if numeric_behavior.get("overflow_behavior") != "reject_execution_on_u16_overflow":
        raise ContractExecutionError("bounded_executable_ui_unit requires reject_execution_on_u16_overflow")

    public_io = require_object(unit.get("public_io"), "unit.public_io")
    inputs = require_list(public_io.get("inputs"), "unit.public_io.inputs")
    outputs = require_list(public_io.get("outputs"), "unit.public_io.outputs")
    if len(inputs) != 1 or inputs[0].get("id") != "input_value":
        raise ContractExecutionError("bounded_executable_ui_unit expects public input input_value")
    if len(outputs) != 1 or outputs[0].get("id") != "result":
        raise ContractExecutionError("bounded_executable_ui_unit expects public output result")

    input_value_raw = resolve_reference("input_value", {}, case)
    if not isinstance(input_value_raw, int):
        raise ContractExecutionError("input_value must be an integer")
    if input_value_raw < 0 or input_value_raw > 65535:
        raise ContractExecutionError("final_state must remain in the u16 domain.")

    kernel = require_object(unit.get("execution_kernel"), "unit.execution_kernel")
    if kernel.get("state_id") != "accumulator_state":
        raise ContractExecutionError("bounded_executable_ui_unit expects accumulator_state")
    initial_state = kernel.get("initial_state")
    if initial_state != 0:
        raise ContractExecutionError("bounded_executable_ui_unit expects initial state 0")
    iteration_count = kernel.get("iteration_count")
    if not isinstance(iteration_count, int) or iteration_count < 0:
        raise ContractExecutionError("iteration_count must be a non-negative integer")

    state_current = int(initial_state)
    for _ in range(iteration_count):
        state_current = checked_u16_add(state_current, input_value_raw)
    final_state = state_current

    panel = wfrog_main_panel(wfrog)
    effects = require_list(unit.get("effects"), "unit.effects")
    applied_widget_references: list[dict[str, Any]] = []
    property_map: dict[tuple[str, str], Any] = {}

    for effect in effects:
        obj = require_object(effect, "unit.effects[]")
        if obj.get("op") != "frog.ui.property_write":
            raise ContractExecutionError("bounded_executable_ui_unit supports only frog.ui.property_write effects")
        widget_id = obj.get("widget_id")
        member = obj.get("member")
        if not isinstance(widget_id, str) or not isinstance(member, str):
            raise ContractExecutionError("effects require widget_id and member strings")
        value = property_write_value(obj)
        property_map[(widget_id, member)] = value
        applied_widget_references.append({"widget_id": widget_id, "member": member, "value": value})

    widgets: list[dict[str, Any]] = []
    for widget in require_list(panel.get("widgets"), "wfrog.front_panels[0].widgets"):
        obj = require_object(widget, "wfrog widget")
        widget_id = obj.get("instance_id")
        if not isinstance(widget_id, str):
            raise ContractExecutionError("wfrog widget instance_id must be a string")
        props = require_object(obj.get("props"), f"wfrog widget {widget_id}.props")
        visual = require_object(obj.get("visual"), f"wfrog widget {widget_id}.visual")
        if widget_id == "ctrl_input":
            value = input_value_raw
        elif widget_id == "ind_result":
            value = final_state
        else:
            value = props.get("value")
        runtime = {
            "value": value,
            "label": props.get("label"),
            "visible": props.get("visible"),
            "enabled": props.get("enabled"),
            "foreground_color": property_map.get((widget_id, "foreground_color"), props.get("foreground_color")),
            "asset_ref": visual.get("asset_ref"),
        }
        widgets.append({"widget_id": widget_id, "class_ref": obj.get("class_ref"), "role": obj.get("role"), "layout": obj.get("layout"), "runtime": runtime})

    return {
        "artifact_kind": "frog_runtime_execution_result",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "status": "ok",
        "contract_ref": {"unit_ids": [unit.get("unit_id")], "backend_family": contract.get("backend_family"), "source_ref": contract.get("source_ref")},
        "execution_summary": {"mode": "contract_and_wfrog", "executed_unit": unit.get("unit_id"), "iterations": iteration_count, "state_initialized": True, "initial_state": initial_state, "final_state": final_state},
        "outputs": {"public": {"result": final_state}, "ui": {"ctrl_input": input_value_raw, "ind_result": final_state}},
        "ui_runtime": {"panel": {"panel_id": panel.get("panel_id"), "title": panel.get("title"), "class_ref": panel.get("class_ref"), "layout": panel.get("layout")}, "widgets": widgets, "applied_widget_references": applied_widget_references},
        "diagnostics": [],
    }


def execute_boolean_value_roundtrip_ui_unit(contract: dict[str, Any], unit: dict[str, Any], case: dict[str, Any], support_artifacts: dict[str, Any] | None = None) -> dict[str, Any]:
    support_artifacts = support_artifacts or {}
    wfrog = support_artifacts.get("wfrog")
    if not isinstance(wfrog, dict):
        raise ContractExecutionError("boolean_value_roundtrip_ui_unit requires support_artifacts['wfrog']")

    public_io = require_object(unit.get("public_io"), "unit.public_io")
    inputs = require_list(public_io.get("inputs"), "unit.public_io.inputs")
    outputs = require_list(public_io.get("outputs"), "unit.public_io.outputs")
    if len(inputs) != 1 or inputs[0].get("id") != "input_value" or inputs[0].get("type") != "bool":
        raise ContractExecutionError("boolean_value_roundtrip_ui_unit expects bool public input input_value")
    if len(outputs) != 1 or outputs[0].get("id") != "result" or outputs[0].get("type") != "bool":
        raise ContractExecutionError("boolean_value_roundtrip_ui_unit expects bool public output result")

    kernel = require_object(unit.get("execution_kernel"), "unit.execution_kernel")
    if kernel.get("operation") != "copy":
        raise ContractExecutionError("boolean_value_roundtrip_ui_unit expects copy execution_kernel")
    if kernel.get("src") != "input_value" or kernel.get("dst") != "result":
        raise ContractExecutionError("boolean_value_roundtrip_ui_unit expects input_value -> result")

    input_value = case.get("input_value")
    if input_value is None:
        inputs_case = case.get("inputs")
        if isinstance(inputs_case, dict):
            input_value = inputs_case.get("input_value")
    if not isinstance(input_value, bool):
        raise ContractExecutionError("input_value must be a boolean")

    panel = wfrog_main_panel(wfrog)
    widgets_by_id: dict[str, dict[str, Any]] = {}
    for widget in require_list(panel.get("widgets"), "wfrog.front_panels[0].widgets"):
        obj = require_object(widget, "wfrog widget")
        widget_id = obj.get("instance_id")
        if not isinstance(widget_id, str):
            raise ContractExecutionError("wfrog widget instance_id must be a string")
        widgets_by_id[widget_id] = obj

    for widget_id in ("bool_input", "bool_result"):
        if widget_id not in widgets_by_id:
            raise ContractExecutionError(f"boolean_value_roundtrip_ui_unit requires widget {widget_id}")

    def widget_runtime(widget_id: str, value: bool) -> dict[str, Any]:
        widget = widgets_by_id[widget_id]
        props = require_object(widget.get("props"), f"wfrog widget {widget_id}.props")
        visual = require_object(widget.get("visual"), f"wfrog widget {widget_id}.visual")
        runtime = {
            "value": value,
            "label.text": props.get("label.text"),
            "caption.text": props.get("caption.text"),
            "state_text.true_text": props.get("state_text.true_text"),
            "state_text.false_text": props.get("state_text.false_text"),
            "asset_ref": visual.get("asset_ref"),
            "realization.variant": props.get("realization.variant"),
        }
        for member in (
            "state_text.style.text_color.false",
            "state_text.style.text_color.true",
            "state_text.visible",
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
            "style.pressed.inset",
            "style.transition.duration_ms",
            "style.transition.timing",
        ):
            if member in props:
                runtime[member] = props[member]
        return runtime

    return {
        "artifact_kind": "frog_runtime_execution_result",
        "artifact_governance_ref": {"path": "Versioning/Readme.md"},
        "status": "ok",
        "contract_ref": {"unit_ids": [unit.get("unit_id")], "backend_family": contract.get("backend_family"), "source_ref": contract.get("source_ref")},
        "execution_summary": {"mode": "boolean_value_roundtrip", "executed_unit": unit.get("unit_id"), "operation": "copy", "input_value": input_value, "result": input_value},
        "outputs": {"public": {"result": input_value}, "ui": {"bool_input": input_value, "bool_result": input_value}},
        "ui_runtime": {
            "panel": {"panel_id": panel.get("panel_id"), "title": panel.get("title"), "class_ref": panel.get("class_ref"), "layout": panel.get("layout")},
            "widgets": [
                {
                    "widget_id": "bool_input",
                    "class_ref": widgets_by_id["bool_input"].get("class_ref"),
                    "role": "control",
                    "layout": widgets_by_id["bool_input"].get("layout"),
                    "runtime": widget_runtime("bool_input", input_value),
                },
                {
                    "widget_id": "bool_result",
                    "class_ref": widgets_by_id["bool_result"].get("class_ref"),
                    "role": "indicator",
                    "layout": widgets_by_id["bool_result"].get("layout"),
                    "runtime": widget_runtime("bool_result", input_value),
                },
            ],
        },
        "diagnostics": [],
    }


KIND_EXECUTORS = {
    "pure_addition_kernel": execute_pure_addition,
    "ui_value_roundtrip_kernel": execute_ui_value_roundtrip,
    "ui_property_write_effect_unit": execute_ui_property_write,
    "stateful_feedback_delay_kernel": execute_stateful_feedback_delay,
    "bounded_executable_ui_unit": execute_bounded_executable_ui_unit,
    "boolean_value_roundtrip_ui_unit": execute_boolean_value_roundtrip_ui_unit,
}


def execute_contract_case(contract: dict[str, Any], case: dict[str, Any], support_artifacts: dict[str, Any] | None = None) -> dict[str, Any]:
    unit = single_unit(contract)
    kind = unit.get("kind")
    if not isinstance(kind, str):
        raise ContractExecutionError("contract unit.kind must be a string")
    executor = KIND_EXECUTORS.get(kind)
    if executor is None:
        raise ContractExecutionError(f"unsupported contract unit kind: {kind}")
    return executor(contract, unit, case, support_artifacts)


def execute_acceptance(acceptance: dict[str, Any], contract: dict[str, Any], support_artifacts: dict[str, Any] | None = None) -> dict[str, Any]:
    unit = single_unit(contract)
    if unit.get("kind") == "bounded_executable_ui_unit":
        return execute_contract_case(contract, case_for_bounded_ui_acceptance(acceptance), support_artifacts)
    if unit.get("kind") == "boolean_value_roundtrip_ui_unit":
        headless = acceptance.get("headless")
        if isinstance(headless, dict) and isinstance(headless.get("input_value"), bool):
            return execute_contract_case(contract, {"input_value": headless["input_value"]}, support_artifacts)
    cases = acceptance.get("cases")
    if not isinstance(cases, list) or len(cases) != 1 or not isinstance(cases[0], dict):
        raise ContractExecutionError("runtime acceptance currently requires exactly one case object")
    return execute_contract_case(contract, cases[0], support_artifacts)


def check_acceptance_against_snapshot(acceptance: dict[str, Any], contract: dict[str, Any], snapshot: dict[str, Any], support_artifacts: dict[str, Any] | None = None) -> None:
    observed = execute_acceptance(acceptance, contract, support_artifacts)
    if canonical_json_bytes(observed) != canonical_json_bytes(snapshot):
        example_id = acceptance.get("example_id", contract_example_id(contract))
        raise ContractExecutionError(f"runtime snapshot mismatch: {example_id}")


def check_overflow_against_acceptance(acceptance: dict[str, Any], contract: dict[str, Any], support_artifacts: dict[str, Any] | None = None) -> None:
    overflow = acceptance.get("overflow")
    if not isinstance(overflow, dict):
        return
    input_value = overflow.get("input_value")
    expected_error = overflow.get("expected_error")
    if not isinstance(input_value, int) or not isinstance(expected_error, str):
        raise ContractExecutionError("overflow acceptance must contain integer input_value and string expected_error")
    try:
        execute_contract_case(contract, {"inputs": {"input_value": input_value}, "widget_values": {"ctrl_input": input_value}}, support_artifacts)
    except ContractExecutionError as exc:
        if str(exc) != expected_error:
            raise ContractExecutionError(f"overflow error mismatch: expected {expected_error!r}, got {str(exc)!r}") from exc
        return
    raise ContractExecutionError("overflow input was accepted, but rejection was expected")
