from __future__ import annotations

from typing import Any, Dict, List

from Implementations.Reference.common import (
    DEFAULT_BACKEND_FAMILY,
    BackendContract,
    FrogPipelineError,
    RuntimeResult,
    ensure,
)

SUPPORTED_WIDGETS = {
    "ctrl_input": "frog.widgets.numeric_control",
    "ind_result": "frog.widgets.numeric_indicator",
}
SUPPORTED_WIDGET_MEMBER = "foreground_color"
SUPPORTED_COLOR_TYPE = "frog.color.rgba8"


def _ensure_dict(value: Any, *, stage: str, error_code: str, message: str) -> Dict[str, Any]:
    ensure(isinstance(value, dict), stage=stage, error_code=error_code, message=message)
    return value


def _ensure_list(value: Any, *, stage: str, error_code: str, message: str) -> List[Any]:
    ensure(isinstance(value, list), stage=stage, error_code=error_code, message=message)
    return value


def _coerce_u16(value: Any, *, stage: str, error_code: str, label: str) -> int:
    try:
        numeric = int(value)
    except (TypeError, ValueError) as exc:
        raise FrogPipelineError(
            stage=stage,
            error_code=error_code,
            message=f"{label} must be coercible to u16.",
        ) from exc

    ensure(
        0 <= numeric <= 65535,
        stage=stage,
        error_code=error_code,
        message=f"{label} must remain within the u16 domain.",
    )
    return numeric


def _extract_foreground_color(write: Dict[str, Any], *, stage: str) -> str:
    value = _ensure_dict(
        write.get("value"),
        stage=stage,
        error_code="invalid_property_write_value",
        message="Property write value must be a JSON object.",
    )
    ensure(
        value.get("type") == SUPPORTED_COLOR_TYPE,
        stage=stage,
        error_code="unsupported_property_write_value_type",
        message=f"{SUPPORTED_WIDGET_MEMBER} property writes must carry {SUPPORTED_COLOR_TYPE} values.",
    )
    literal = value.get("value")
    ensure(
        isinstance(literal, str) and literal != "",
        stage=stage,
        error_code="invalid_property_write_literal",
        message=f"{SUPPORTED_WIDGET_MEMBER} property writes must carry a non-empty string literal value.",
    )
    return literal


class ReferenceHostRuntime:
    def __init__(self, backend_family: str = DEFAULT_BACKEND_FAMILY):
        ensure(
            backend_family == DEFAULT_BACKEND_FAMILY,
            stage="runtime-create",
            error_code="unsupported_backend_family",
            message=f"Unsupported runtime family: {backend_family}",
        )
        self.backend_family = backend_family

    def accept_contract(self, contract: BackendContract) -> Dict[str, Any]:
        artifact = contract.artifact

        ensure(
            artifact.get("artifact_kind") == "frog_backend_contract",
            stage="runtime-accept",
            error_code="invalid_contract_kind",
            message="Runtime expected a frog_backend_contract artifact.",
        )
        ensure(
            artifact.get("backend_family") == self.backend_family,
            stage="runtime-accept",
            error_code="backend_family_mismatch",
            message="Runtime backend family mismatch.",
        )

        assumptions = _ensure_dict(
            artifact.get("assumptions"),
            stage="runtime-accept",
            error_code="missing_assumptions",
            message="Runtime expected assumptions on the backend contract.",
        )
        scheduling = _ensure_dict(
            assumptions.get("scheduling"),
            stage="runtime-accept",
            error_code="missing_scheduling_assumptions",
            message="Runtime expected scheduling assumptions on the backend contract.",
        )
        ensure(
            scheduling.get("family_rule") == "deterministic_step_execution",
            stage="runtime-accept",
            error_code="unsupported_scheduling_rule",
            message="Runtime expected deterministic_step_execution scheduling.",
        )

        units = _ensure_list(
            artifact.get("units"),
            stage="runtime-accept",
            error_code="missing_units",
            message="Runtime expected contract units.",
        )
        ensure(
            len(units) == 1,
            stage="runtime-accept",
            error_code="unsupported_unit_count",
            message="Demo runtime expects exactly one contract unit.",
        )

        unit = _ensure_dict(
            units[0],
            stage="runtime-accept",
            error_code="invalid_unit",
            message="Runtime expected one JSON object contract unit.",
        )
        ensure(
            unit.get("kind") == "bounded_executable_ui_unit",
            stage="runtime-accept",
            error_code="unsupported_unit_kind",
            message="Demo runtime expects one bounded_executable_ui_unit.",
        )

        execution_model = _ensure_dict(
            unit.get("execution_model"),
            stage="runtime-accept",
            error_code="missing_execution_model",
            message="Runtime expected execution_model on the contract unit.",
        )
        ensure(
            execution_model.get("structure") == "for_loop",
            stage="runtime-accept",
            error_code="unsupported_structure",
            message="Demo runtime expects a for_loop execution structure.",
        )
        ensure(
            execution_model.get("iteration_count") == 5,
            stage="runtime-accept",
            error_code="unsupported_iteration_count",
            message="Demo runtime expects exactly 5 iterations for the slice 05 contract.",
        )

        state_model = _ensure_dict(
            unit.get("state_model"),
            stage="runtime-accept",
            error_code="missing_state_model",
            message="Runtime expected state_model on the contract unit.",
        )
        ensure(
            state_model.get("explicit_state") is True,
            stage="runtime-accept",
            error_code="missing_explicit_state",
            message="Demo runtime requires explicit_state = true.",
        )
        carrier = _ensure_dict(
            state_model.get("carrier"),
            stage="runtime-accept",
            error_code="missing_state_carrier",
            message="Runtime expected an explicit state carrier.",
        )
        ensure(
            carrier.get("primitive") == "frog.core.delay",
            stage="runtime-accept",
            error_code="unsupported_state_carrier",
            message="Demo runtime expects frog.core.delay as the state carrier.",
        )
        initial_state = _coerce_u16(
            carrier.get("initial_value"),
            stage="runtime-accept",
            error_code="invalid_initial_state",
            label="Initial state",
        )

        public_interface = _ensure_dict(
            unit.get("public_interface"),
            stage="runtime-accept",
            error_code="missing_public_interface",
            message="Runtime expected public_interface on the contract unit.",
        )
        public_inputs = _ensure_list(
            public_interface.get("inputs"),
            stage="runtime-accept",
            error_code="missing_public_inputs",
            message="Runtime expected one public input.",
        )
        public_outputs = _ensure_list(
            public_interface.get("outputs"),
            stage="runtime-accept",
            error_code="missing_public_outputs",
            message="Runtime expected one public output.",
        )
        ensure(
            len(public_inputs) == 1 and public_inputs[0].get("id") == "input_value",
            stage="runtime-accept",
            error_code="unsupported_public_inputs",
            message="Demo runtime expects one public input named input_value.",
        )
        ensure(
            len(public_outputs) == 1 and public_outputs[0].get("id") == "result",
            stage="runtime-accept",
            error_code="unsupported_public_outputs",
            message="Demo runtime expects one public output named result.",
        )

        ui_binding = _ensure_dict(
            unit.get("ui_binding"),
            stage="runtime-accept",
            error_code="missing_ui_binding",
            message="Runtime expected ui_binding on the contract unit.",
        )
        widgets = _ensure_list(
            ui_binding.get("widgets"),
            stage="runtime-accept",
            error_code="missing_widgets",
            message="Runtime expected widget declarations.",
        )
        ensure(
            len(widgets) == 2,
            stage="runtime-accept",
            error_code="unsupported_widget_count",
            message="Demo runtime expects exactly two widgets.",
        )

        widget_map: Dict[str, Dict[str, Any]] = {}
        for raw_widget in widgets:
            widget = _ensure_dict(
                raw_widget,
                stage="runtime-accept",
                error_code="invalid_widget_binding",
                message="Widget entries must be JSON objects.",
            )
            widget_id = widget.get("widget_id")
            ensure(
                widget_id in SUPPORTED_WIDGETS,
                stage="runtime-accept",
                error_code="unsupported_widget_ids",
                message="Demo runtime expects ctrl_input and ind_result widget ids.",
            )
            ensure(
                widget.get("widget_class") == SUPPORTED_WIDGETS[widget_id],
                stage="runtime-accept",
                error_code="unsupported_widget_class",
                message=f"Widget {widget_id} must use class {SUPPORTED_WIDGETS[widget_id]}.",
            )
            widget_map[widget_id] = widget

        ensure(
            set(widget_map) == set(SUPPORTED_WIDGETS),
            stage="runtime-accept",
            error_code="missing_required_widgets",
            message="Demo runtime expects ctrl_input and ind_result widget bindings.",
        )
        ensure(
            widget_map["ctrl_input"].get("role") == "control",
            stage="runtime-accept",
            error_code="unsupported_widget_role",
            message="ctrl_input must have role control.",
        )
        ensure(
            widget_map["ind_result"].get("role") == "indicator",
            stage="runtime-accept",
            error_code="unsupported_widget_role",
            message="ind_result must have role indicator.",
        )

        widget_reference_support = _ensure_list(
            ui_binding.get("widget_reference_support"),
            stage="runtime-accept",
            error_code="missing_widget_reference_support",
            message="Runtime expected widget_reference_support declarations.",
        )
        supported_members: Dict[str, List[str]] = {}
        for raw_support in widget_reference_support:
            support = _ensure_dict(
                raw_support,
                stage="runtime-accept",
                error_code="invalid_widget_reference_support",
                message="widget_reference_support entries must be JSON objects.",
            )
            widget_id = support.get("widget_id")
            members = _ensure_list(
                support.get("supported_members"),
                stage="runtime-accept",
                error_code="invalid_supported_members",
                message="supported_members must be a JSON array.",
            )
            supported_members[str(widget_id)] = [str(member) for member in members]

        for widget_id in SUPPORTED_WIDGETS:
            ensure(
                SUPPORTED_WIDGET_MEMBER in supported_members.get(widget_id, []),
                stage="runtime-accept",
                error_code="missing_supported_member",
                message=f"{widget_id} must support {SUPPORTED_WIDGET_MEMBER}.",
            )

        property_writes = _ensure_list(
            unit.get("property_writes"),
            stage="runtime-accept",
            error_code="missing_property_writes",
            message="Runtime expected property_writes on the contract unit.",
        )
        ensure(
            len(property_writes) == 2,
            stage="runtime-accept",
            error_code="unsupported_property_write_count",
            message="Demo runtime expects exactly two property_write operations.",
        )

        seen_targets: set[str] = set()
        for write in property_writes:
            write_dict = _ensure_dict(
                write,
                stage="runtime-accept",
                error_code="invalid_property_write",
                message="Runtime expected property_write entries to be JSON objects.",
            )
            ensure(
                write_dict.get("operation") == "frog.ui.property_write",
                stage="runtime-accept",
                error_code="unsupported_property_write_operation",
                message="Demo runtime only supports frog.ui.property_write operations.",
            )
            ensure(
                write_dict.get("member") == SUPPORTED_WIDGET_MEMBER,
                stage="runtime-accept",
                error_code="unsupported_property_write_member",
                message=f"Demo runtime only supports {SUPPORTED_WIDGET_MEMBER} property writes.",
            )
            widget_id = write_dict.get("widget_id")
            ensure(
                widget_id in SUPPORTED_WIDGETS,
                stage="runtime-accept",
                error_code="unsupported_property_write_widget",
                message="Property writes must target ctrl_input or ind_result.",
            )
            seen_targets.add(str(widget_id))
            _extract_foreground_color(write_dict, stage="runtime-accept")

        ensure(
            seen_targets == set(SUPPORTED_WIDGETS),
            stage="runtime-accept",
            error_code="missing_property_write_targets",
            message="Property writes must cover ctrl_input and ind_result.",
        )

        return {
            "status": "ok",
            "unit_ids": [unit["unit_id"]],
            "backend_family": artifact["backend_family"],
            "example_id": artifact.get("source_ref", {}).get("example_id"),
            "initial_state": initial_state,
        }

    def execute(self, contract: BackendContract, public_inputs: Dict[str, Any]) -> RuntimeResult:
        self.accept_contract(contract)

        artifact = contract.artifact
        unit = artifact["units"][0]
        carrier = unit["state_model"]["carrier"]
        iterations = int(unit["execution_model"]["iteration_count"])
        initial_state = _coerce_u16(
            carrier["initial_value"],
            stage="run",
            error_code="invalid_initial_state",
            label="Initial state",
        )

        control_value_raw = public_inputs.get("input_value", public_inputs.get("ctrl_input"))
        ensure(
            control_value_raw is not None,
            stage="run",
            error_code="missing_control_input",
            message="Missing control value. Expected input_value or ctrl_input.",
        )
        control_value = _coerce_u16(
            control_value_raw,
            stage="run",
            error_code="invalid_control_input",
            label="Control value",
        )

        widget_foreground_colors: Dict[str, str] = {}
        applied_widget_references: List[Dict[str, Any]] = []
        for write in unit["property_writes"]:
            widget_id = str(write["widget_id"])
            color = _extract_foreground_color(write, stage="run")
            widget_foreground_colors[widget_id] = color
            applied_widget_references.append(
                {
                    "widget_id": widget_id,
                    "member": SUPPORTED_WIDGET_MEMBER,
                    "value": write["value"],
                }
            )

        state_current = initial_state
        for _ in range(iterations):
            state_next = state_current + control_value
            ensure(
                0 <= state_next <= 65535,
                stage="run",
                error_code="u16_overflow",
                message="u16 overflow during bounded accumulation.",
            )
            state_current = state_next

        final_value = state_current

        runtime_artifact = {
            "artifact_kind": "frog_runtime_result",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "status": "ok",
            "contract_ref": {
                "unit_ids": [unit["unit_id"]],
                "backend_family": artifact["backend_family"],
                "source_ref": artifact.get("source_ref"),
            },
            "execution_summary": {
                "mode": "deterministic_step_execution",
                "executed_unit": unit["unit_id"],
                "iterations": iterations,
                "state_initialized": True,
                "initial_state": initial_state,
                "final_state": final_value,
            },
            "outputs": {
                "public": {
                    unit["public_output_publication"]["output_id"]: final_value,
                },
                "ui": {
                    "ctrl_input": control_value,
                    "ind_result": final_value,
                },
            },
            "ui_runtime": {
                "widgets": [
                    {
                        "widget_id": "ctrl_input",
                        "runtime": {
                            "value": control_value,
                            "foreground_color": widget_foreground_colors.get("ctrl_input"),
                        },
                    },
                    {
                        "widget_id": "ind_result",
                        "runtime": {
                            "value": final_value,
                            "foreground_color": widget_foreground_colors.get("ind_result"),
                        },
                    },
                ],
                "applied_widget_references": applied_widget_references,
            },
            "diagnostics": [],
        }
        return RuntimeResult(artifact=runtime_artifact)


def create_runtime_for_family(backend_family: str = DEFAULT_BACKEND_FAMILY) -> ReferenceHostRuntime:
    return ReferenceHostRuntime(backend_family=backend_family)
