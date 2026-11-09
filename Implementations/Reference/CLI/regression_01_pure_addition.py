from __future__ import annotations

import json
import math
import subprocess
import sys
from pathlib import Path
from typing import Any, Dict


EXPECTED_BACKEND_FAMILY = "reference_host_runtime_ui_binding"
EXPECTED_OBJECT_COUNT = 4
EXPECTED_CONNECTION_COUNT = 3
EXPECTED_RESULT = 6.0
TEST_INPUTS = {"a": 2.25, "b": 3.75}


class RegressionFailure(RuntimeError):
    pass


def repo_root_from_here() -> Path:
    return Path(__file__).resolve().parents[3]


def pipeline_script_path() -> Path:
    return repo_root_from_here() / "Implementations" / "Reference" / "CLI" / "frog_demo_pipeline.py"


def example_source_path() -> Path:
    return repo_root_from_here() / "Examples" / "01_pure_addition" / "main.frog"


def run_cli_command(*args: str) -> Dict[str, Any]:
    repo_root = repo_root_from_here()
    pipeline = pipeline_script_path()
    source = example_source_path()

    if not pipeline.is_file():
        raise RegressionFailure(f"Pipeline script not found: {pipeline}")

    if not source.is_file():
        raise RegressionFailure(f"Example source not found: {source}")

    command = [sys.executable, str(pipeline), *args]
    completed = subprocess.run(
        command,
        cwd=str(repo_root),
        text=True,
        capture_output=True,
        check=False,
    )

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


def assert_close(actual: float, expected: float, label: str, tol: float = 1e-12) -> None:
    if not math.isclose(actual, expected, rel_tol=tol, abs_tol=tol):
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
    assert_equal(subset["explicit_local_memory"], False, "validate.validated_subset.explicit_local_memory")


def check_derive_ir() -> None:
    data = run_cli_command("derive-ir", str(example_source_path()))

    assert_equal(data["artifact_kind"], "frog_derived_execution_ir", "derive_ir.artifact_kind")

    unit = data["execution_unit"]
    assert_equal(unit["id"], "unit:main", "derive_ir.execution_unit.id")
    assert_equal(len(unit["objects"]), EXPECTED_OBJECT_COUNT, "derive_ir.object_count")
    assert_equal(len(unit["connections"]), EXPECTED_CONNECTION_COUNT, "derive_ir.connection_count")


def check_emit_contract() -> None:
    data = run_cli_command(
        "emit-contract",
        str(example_source_path()),
        "--backend-family",
        EXPECTED_BACKEND_FAMILY,
    )

    assert_equal(data["artifact_kind"], "frog_backend_contract", "contract.artifact_kind")
    assert_equal(data["backend_family"], EXPECTED_BACKEND_FAMILY, "contract.backend_family")
    assert_equal(len(data["units"]), 1, "contract.unit_count")

    unit = data["units"][0]
    assert_equal(unit["id"], "main", "contract.unit.id")

    boundaries = unit["boundaries"]
    public_inputs = [boundary for boundary in boundaries if boundary["kind"] == "public_input"]
    public_outputs = [boundary for boundary in boundaries if boundary["kind"] == "public_output"]

    assert_equal(len(public_inputs), 2, "contract.public_input_count")
    assert_equal(len(public_outputs), 1, "contract.public_output_count")


def check_run() -> None:
    data = run_cli_command(
        "run",
        str(example_source_path()),
        "--backend-family",
        EXPECTED_BACKEND_FAMILY,
        "--inputs",
        json.dumps(TEST_INPUTS),
    )

    assert_equal(data["status"], "ok", "run.status")
    assert_equal(data["lowered"]["backend_family"], EXPECTED_BACKEND_FAMILY, "run.lowered.backend_family")

    runtime = data["runtime"]
    assert_equal(runtime["artifact_kind"], "frog_runtime_result", "run.runtime.artifact_kind")
    assert_equal(runtime["status"], "ok", "run.runtime.status")

    result_value = float(runtime["outputs"]["public"]["result"])
    assert_close(result_value, EXPECTED_RESULT, "run.runtime.outputs.public.result")


def main() -> int:
    check_validate()
    check_derive_ir()
    check_emit_contract()
    check_run()
    print("01_pure_addition regression: PASS")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except RegressionFailure as exc:
        print(f"01_pure_addition regression: FAIL\n{exc}", file=sys.stderr)
        raise SystemExit(1)
