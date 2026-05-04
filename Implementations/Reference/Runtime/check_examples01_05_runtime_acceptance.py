#!/usr/bin/env python3
"""Check runtime acceptance for Examples 01-05.

Examples 01-04 use the simple reference contracts published in
ContractEmitter/examples. Example 05 delegates to the existing runtime-family
acceptance checker.
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[3]

ACCEPTANCE_FILES = [
    "Implementations/Reference/Runtime/acceptance/example01_pure_addition.acceptance.json",
    "Implementations/Reference/Runtime/acceptance/example02_ui_value_roundtrip.acceptance.json",
    "Implementations/Reference/Runtime/acceptance/example03_ui_property_write.acceptance.json",
    "Implementations/Reference/Runtime/acceptance/example04_stateful_feedback_delay.acceptance.json",
]


class RuntimeAcceptanceError(RuntimeError):
    pass


def load_json(path: Path) -> dict[str, Any]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise RuntimeAcceptanceError(f"missing file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise RuntimeAcceptanceError(f"invalid JSON in {path}: {exc}") from exc
    if not isinstance(data, dict):
        raise RuntimeAcceptanceError(f"{path} must contain a JSON object")
    return data


def canonical_json_bytes(data: dict[str, Any]) -> bytes:
    return (json.dumps(data, sort_keys=True, separators=(",", ":")) + "\n").encode("utf-8")


def execute_contract(contract: dict[str, Any], acceptance: dict[str, Any]) -> dict[str, Any]:
    example_id = contract.get("example_id")
    unit = contract["units"][0]
    case = acceptance["cases"][0]

    if example_id == "01_pure_addition":
        inputs = case["inputs"]
        result = float(inputs["a"]) + float(inputs["b"])
        return {"artifact_kind": "frog_reference_runtime_snapshot", "example_id": example_id, "status": "ok", "inputs": inputs, "public_outputs": {"result": result}}

    if example_id == "02_ui_value_roundtrip":
        values = dict(case["widget_values"])
        values["ind_result"] = float(values["ctrl_a"]) + float(values["ctrl_b"])
        return {"artifact_kind": "frog_reference_runtime_snapshot", "example_id": example_id, "status": "ok", "widget_values": values}

    if example_id == "03_ui_property_write":
        status = case["inputs"]["status"]
        return {
            "artifact_kind": "frog_reference_runtime_snapshot",
            "example_id": example_id,
            "status": "ok",
            "public_inputs": {"status": status},
            "widget_state": {"ctrl_gain": {"label.text": status}},
            "effects": [{"op": "frog.ui.property_write", "widget_id": "ctrl_gain", "member": "label.text", "value": status}],
        }

    if example_id == "04_stateful_feedback_delay":
        x = float(case["inputs"]["x"])
        initial = float(unit["execution_kernel"]["initial_state"])
        y = initial + x
        return {"artifact_kind": "frog_reference_runtime_snapshot", "example_id": example_id, "status": "ok", "inputs": {"x": x}, "initial_state": {"delay_1": initial}, "public_outputs": {"y": y}, "final_state": {"delay_1": y}}

    raise RuntimeAcceptanceError(f"unsupported runtime example: {example_id!r}")


def check_acceptance_file(rel_path: str) -> None:
    acceptance_path = ROOT / rel_path
    acceptance = load_json(acceptance_path)
    refs = acceptance.get("artifact_refs", {})
    contract = load_json(ROOT / refs["contract_path"])
    snapshot = load_json(ROOT / refs["snapshot_path"])
    observed = execute_contract(contract, acceptance)
    if canonical_json_bytes(observed) != canonical_json_bytes(snapshot):
        raise RuntimeAcceptanceError(f"runtime snapshot mismatch: {acceptance['example_id']}")
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


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Check runtime acceptance for Examples 01-05.")
    parser.add_argument("--skip-example05", action="store_true")
    args = parser.parse_args(argv)

    try:
        for rel_path in ACCEPTANCE_FILES:
            check_acceptance_file(rel_path)
        if not args.skip_example05:
            check_example05()
        print("Runtime acceptance status: ok")
        return 0
    except RuntimeAcceptanceError as exc:
        print(f"Runtime acceptance error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
