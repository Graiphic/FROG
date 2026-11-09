from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any, Dict

REPO_ROOT = Path(__file__).resolve().parents[3]
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

from Implementations.Reference.CLI.frog_demo_pipeline import (
    pipeline_validate,
    pipeline_derive_ir,
    pipeline_lower,
    pipeline_emit_contract,
)
from Implementations.Reference.Runtime.reference_runtime import create_runtime_for_family

EXPECTED_BACKEND_FAMILY = "reference_host_runtime_ui_binding"
EXPECTED_OBJECT_COUNT = 4
EXPECTED_CONNECTION_COUNT = 4
TEST_INPUT_1 = {"x": 2.0}
TEST_INPUT_2 = {"x": 3.0}


class RegressionFailure(RuntimeError):
    pass


def repo_root_from_here() -> Path:
    return Path(__file__).resolve().parents[3]


def pipeline_script_path() -> Path:
    return repo_root_from_here() / "Implementations" / "Reference" / "CLI" / "frog_demo_pipeline.py"


def example_source_path() -> Path:
    return repo_root_from_here() / "Examples" / "04_stateful_feedback_delay" / "main.frog"


def run_cli_command(*args: str) -> Dict[str, Any]:
    repo_root = repo_root_from_here()
    pipeline = pipeline_script_path()
    source = example_source_path()

    if not pipeline.is_file():
        raise RegressionFailure(f"Pipeline script not found: {pipeline}")
    if not source.is_file():
        raise RegressionFailure(f"Example source not found: {source}")

    command = [sys.executable, str(pipeline), *args]
    completed = subprocess.run(command, cwd=str(repo_root), text=True, capture_output=True, check=False)

    if completed.returncode != 0:
        raise RegressionFailure(
            "CLI command failed\n"
            f"command: {' '.join(command)}\n"
            f"stdout:\n{completed.stdout}\n"
            f"stderr:\n{completed.stderr}"
        )

    try:
        return json.loads(completed.stdout)
    except json.JSONDecodeError as exc:
        raise RegressionFailure(
            "CLI command did not return valid JSON\n"
            f"command: {' '.join(command)}\n"
            f"stdout:\n{completed.stdout}\n"
            f"stderr:\n{completed.stderr}"
        ) from exc


def assert_equal(actual: Any, expected: Any, label: str) -> None:
    if actual != expected:
        raise RegressionFailure(f"{label}: expected {expected!r}, got {actual!r}")


def assert_float(actual: Any, expected: float, label: str) -> None:
    if abs(float(actual) - expected) > 1e-12:
        raise RegressionFailure(f"{label}: expected {expected!r}, got {actual!r}")


def check_validate() -> None:
    data = run_cli_command("validate", str(example_source_path()))
    assert_equal(data["artifact_kind"], "frog_validation_result", "validate.artifact_kind")
    assert_equal(data["status"], "ok", "validate.status")
    subset = data["validated_subset"]
    assert_equal(subset["core_primitives"], True, "validate.validated_subset.core_primitives")
    assert_equal(subset["public_interface"], True, "validate.validated_subset.public_interface")
    assert_equal(subset["widget_value"], False, "validate.validated_subset.widget_value")
    assert_equal(subset["widget_reference"], False, "validate.validated_subset.widget_reference")
    assert_equal(subset["ui_object_primitives"], False, "validate.validated_subset.ui_object_primitives")
    assert_equal(subset["explicit_local_memory"], True, "validate.validated_subset.explicit_local_memory")


def check_derive_ir() -> None:
    data = run_cli_command("derive-ir", str(example_source_path()))
    assert_equal(data["artifact_kind"], "frog_derived_execution_ir", "derive_ir.artifact_kind")
    unit = data["execution_unit"]
    assert_equal(unit["id"], "unit:main", "derive_ir.execution_unit.id")
    assert_equal(len(unit["objects"]), EXPECTED_OBJECT_COUNT, "derive_ir.object_count")
    assert_equal(len(unit["connections"]), EXPECTED_CONNECTION_COUNT, "derive_ir.connection_count")
    delay_objects = [obj for obj in unit["objects"] if obj["kind"] == "explicit_local_memory_primitive"]
    assert_equal(len(delay_objects), 1, "derive_ir.delay_object_count")
    assert_float(delay_objects[0]["initial"], 0.0, "derive_ir.delay_initial")


def check_emit_contract() -> None:
    data = run_cli_command("emit-contract", str(example_source_path()), "--backend-family", EXPECTED_BACKEND_FAMILY)
    assert_equal(data["artifact_kind"], "frog_backend_contract", "contract.artifact_kind")
    assert_equal(data["backend_family"], EXPECTED_BACKEND_FAMILY, "contract.backend_family")
    assert_equal(data["assumptions"]["ui_binding"]["enabled"], False, "contract.assumptions.ui_binding.enabled")
    assert_equal(data["assumptions"]["ui_binding"]["kind"], "none", "contract.assumptions.ui_binding.kind")
    assert_equal(data["assumptions"]["state_model"], "explicit_local_memory", "contract.assumptions.state_model")
    assert_equal(len(data["units"]), 1, "contract.unit_count")
    unit = data["units"][0]
    assert_equal(unit["id"], "main", "contract.unit.id")
    assert_equal(len(unit["boundaries"]), 2, "contract.boundary_count")
    assert_equal(len(unit["state"]["cells"]), 1, "contract.state_cell_count")
    assert_float(unit["state"]["cells"][0]["initial"], 0.0, "contract.state.initial")


def check_runtime_stateful_execution() -> None:
    source = str(example_source_path())
    validation = pipeline_validate(source)
    _ = pipeline_derive_ir(source)
    lowered = pipeline_lower(source, EXPECTED_BACKEND_FAMILY)
    contract = pipeline_emit_contract(source, EXPECTED_BACKEND_FAMILY)

    assert_equal(validation.artifact["validated_subset"]["explicit_local_memory"], True, "pipeline.validate.explicit_local_memory")
    assert_equal(lowered.artifact["assumptions"]["explicit_local_memory"], True, "pipeline.lower.explicit_local_memory")

    runtime = create_runtime_for_family(EXPECTED_BACKEND_FAMILY)
    runtime.accept_contract(contract)

    result1 = runtime.execute(contract, TEST_INPUT_1).artifact
    assert_float(result1["outputs"]["public"]["y"], 2.0, "run1.outputs.public.y")
    assert_float(result1["state"]["before"]["op:delay_1"], 0.0, "run1.state.before")
    assert_float(result1["state"]["after"]["op:delay_1"], 2.0, "run1.state.after")

    result2 = runtime.execute(contract, TEST_INPUT_2).artifact
    assert_float(result2["outputs"]["public"]["y"], 5.0, "run2.outputs.public.y")
    assert_float(result2["state"]["before"]["op:delay_1"], 2.0, "run2.state.before")
    assert_float(result2["state"]["after"]["op:delay_1"], 5.0, "run2.state.after")


def main() -> int:
    check_validate()
    check_derive_ir()
    check_emit_contract()
    check_runtime_stateful_execution()
    print("04_stateful_feedback_delay regression: PASS")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except RegressionFailure as exc:
        print(f"04_stateful_feedback_delay regression: FAIL\n{exc}", file=sys.stderr)
        raise SystemExit(1)
