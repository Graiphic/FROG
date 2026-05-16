#!/usr/bin/env python3
"""Check runtime acceptance for the published reference runtime examples 01-10."""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[3]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from Implementations.Reference.Runtime.contract_executor import ContractExecutionError, check_acceptance_against_snapshot, load_json  # noqa: E402
from Implementations.Reference.Runtime.python.cli import (  # noqa: E402
    execute_example07_contract,
    execute_example08_contract,
    execute_example09_contract,
    execute_example10_contract,
)
from Implementations.Reference.Runtime.python.ui_runtime import (  # noqa: E402
    ButtonRuntimeCore,
    default_example10_contract_path,
    default_example10_wfrog_path,
)

SNAPSHOT_ACCEPTANCE_FILES = [
    "Implementations/Reference/Runtime/acceptance/example01_pure_addition.acceptance.json",
    "Implementations/Reference/Runtime/acceptance/example02_ui_value_roundtrip.acceptance.json",
    "Implementations/Reference/Runtime/acceptance/example03_ui_property_write.acceptance.json",
    "Implementations/Reference/Runtime/acceptance/example04_stateful_feedback_delay.acceptance.json",
    "Implementations/Reference/Runtime/acceptance/example06_boolean_value_roundtrip.acceptance.json",
]


class RuntimeAcceptanceError(RuntimeError):
    pass


def repo_path(path_text: str) -> Path:
    path = Path(path_text)
    return path if path.is_absolute() else ROOT / path


def check_snapshot_acceptance_file(rel_path: str) -> None:
    acceptance_path = ROOT / rel_path
    acceptance = load_json(acceptance_path)
    refs = acceptance.get("artifact_refs")
    if not isinstance(refs, dict):
        raise RuntimeAcceptanceError(f"{rel_path}: artifact_refs must be an object")

    contract = load_json(repo_path(str(refs.get("contract_path"))))
    snapshot = load_json(repo_path(str(refs.get("snapshot_path"))))
    support_artifacts = {}
    if refs.get("wfrog_path"):
        support_artifacts["wfrog"] = load_json(repo_path(str(refs["wfrog_path"])))
    check_acceptance_against_snapshot(acceptance, contract, snapshot, support_artifacts)
    print(f"Runtime acceptance ok: {acceptance['example_id']}")


def check_example05() -> None:
    result = subprocess.run(
        [sys.executable, "Implementations/Reference/Runtime/check_example05_runtime_acceptance.py"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    if result.returncode != 0:
        raise RuntimeAcceptanceError("Example 05 runtime acceptance failed")


def require_output(artifact: dict[str, Any], public_key: str, ui_key: str, expected: Any) -> None:
    outputs = artifact.get("outputs")
    if not isinstance(outputs, dict):
        raise RuntimeAcceptanceError("runtime artifact missing outputs")
    public_outputs = outputs.get("public")
    ui_outputs = outputs.get("ui")
    if not isinstance(public_outputs, dict) or public_outputs.get(public_key) != expected:
        raise RuntimeAcceptanceError(f"public output {public_key} mismatch")
    if not isinstance(ui_outputs, dict) or ui_outputs.get(ui_key) != expected:
        raise RuntimeAcceptanceError(f"ui output {ui_key} mismatch")


def check_examples07_to_10() -> None:
    artifact07 = execute_example07_contract("hello world")
    require_output(artifact07, "result_text", "str_result", "hello world")
    print("Runtime acceptance ok: 07_string_value_roundtrip")

    artifact08 = execute_example08_contract("fault")
    require_output(artifact08, "result_mode", "mode_result", "fault")
    print("Runtime acceptance ok: 08_enum_value_roundtrip")

    artifact09 = execute_example09_contract("C:/FROG/runtime_acceptance.txt")
    require_output(artifact09, "result_path", "path_result", "C:/FROG/runtime_acceptance.txt")
    print("Runtime acceptance ok: 09_path_value_roundtrip")

    artifact10_false = execute_example10_contract(False)
    require_output(artifact10_false, "pressed", "pressed_indicator", False)
    if artifact10_false["outputs"]["ui"]["trigger_button"] is not False:
        raise RuntimeAcceptanceError("Example 10 released Button state must remain momentary false")
    artifact10_true = execute_example10_contract(True)
    require_output(artifact10_true, "pressed", "pressed_indicator", True)
    if artifact10_true["outputs"]["ui"]["trigger_button"] is not True:
        raise RuntimeAcceptanceError("Example 10 Button control must show ON while pressed")

    example10_runtime = ButtonRuntimeCore(
        contract_path=default_example10_contract_path(),
        wfrog_path=default_example10_wfrog_path(),
    )
    pressed = example10_runtime.press_control()
    require_output(pressed, "pressed", "pressed_indicator", True)
    if pressed["outputs"]["ui"]["trigger_button"] is not True:
        raise RuntimeAcceptanceError("Example 10 Button press event must make the command visibly ON")
    released = example10_runtime.release_control()
    require_output(released, "pressed", "pressed_indicator", False)
    if released["outputs"]["ui"]["trigger_button"] is not False:
        raise RuntimeAcceptanceError("Example 10 Button release event must restore the command to OFF")
    print("Runtime acceptance ok: 10_button_press_to_boolean")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Check runtime acceptance for the published reference examples 01-10.")
    parser.add_argument("--skip-example05", action="store_true")
    args = parser.parse_args(argv)

    try:
        for rel_path in SNAPSHOT_ACCEPTANCE_FILES:
            check_snapshot_acceptance_file(rel_path)
        if not args.skip_example05:
            check_example05()
        check_examples07_to_10()
        print("Runtime acceptance status: ok")
        return 0
    except (RuntimeAcceptanceError, ContractExecutionError, KeyError, TypeError) as exc:
        print(f"Runtime acceptance error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
