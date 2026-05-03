#!/usr/bin/env python3
"""Check Example 05 runtime-family acceptance against the published snapshot.

This tool is intentionally narrow and non-normative. It verifies that the
published backend contract and .wfrog package can be consumed by the Python
reference runtime and that the observable result matches the shared acceptance
snapshot.

Run from the repository root:

    python Implementations/Reference/Runtime/check_example05_runtime_acceptance.py
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

try:
    from .python.execute_contract import execute_contract
    from .python.runtime_core import RuntimeExecutionError
except ImportError:  # pragma: no cover
    from python.execute_contract import execute_contract
    from python.runtime_core import RuntimeExecutionError


ROOT = Path(__file__).resolve().parents[3]
DEFAULT_ACCEPTANCE = ROOT / "Implementations" / "Reference" / "Runtime" / "acceptance" / "example05_runtime_family.acceptance.json"


class RuntimeAcceptanceError(RuntimeError):
    """Raised when runtime acceptance cannot be verified."""


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


def repo_path(path_text: str) -> Path:
    path = Path(path_text)
    return path if path.is_absolute() else ROOT / path


def check_headless_snapshot(acceptance: dict[str, Any], *, print_json: bool = False) -> None:
    refs = acceptance.get("artifact_refs")
    if not isinstance(refs, dict):
        raise RuntimeAcceptanceError("acceptance.artifact_refs must be an object")

    contract_path = repo_path(str(refs.get("contract_path")))
    wfrog_path = repo_path(str(refs.get("wfrog_path")))
    snapshot_path = repo_path(str(refs.get("snapshot_path")))

    headless = acceptance.get("headless")
    if not isinstance(headless, dict):
        raise RuntimeAcceptanceError("acceptance.headless must be an object")

    input_value = headless.get("input_value")
    if not isinstance(input_value, int):
        raise RuntimeAcceptanceError("acceptance.headless.input_value must be an integer")

    expected = load_json(snapshot_path)
    observed = execute_contract(
        input_value,
        contract_path=contract_path,
        wfrog_path=wfrog_path,
    )

    if canonical_json_bytes(observed) != canonical_json_bytes(expected):
        raise RuntimeAcceptanceError(
            "runtime snapshot mismatch for Example 05 input "
            f"{input_value}; expected {snapshot_path}"
        )

    if print_json:
        json.dump(observed, sys.stdout, indent=2)
        sys.stdout.write("\n")


def check_overflow_rejection(acceptance: dict[str, Any]) -> None:
    refs = acceptance.get("artifact_refs")
    if not isinstance(refs, dict):
        raise RuntimeAcceptanceError("acceptance.artifact_refs must be an object")

    contract_path = repo_path(str(refs.get("contract_path")))
    wfrog_path = repo_path(str(refs.get("wfrog_path")))

    overflow = acceptance.get("overflow")
    if not isinstance(overflow, dict):
        raise RuntimeAcceptanceError("acceptance.overflow must be an object")

    input_value = overflow.get("input_value")
    expected_error = overflow.get("expected_error")
    if not isinstance(input_value, int) or not isinstance(expected_error, str):
        raise RuntimeAcceptanceError("acceptance.overflow must contain integer input_value and string expected_error")

    try:
        execute_contract(
            input_value,
            contract_path=contract_path,
            wfrog_path=wfrog_path,
        )
    except RuntimeExecutionError as exc:
        if str(exc) != expected_error:
            raise RuntimeAcceptanceError(
                f"overflow error mismatch: expected {expected_error!r}, got {str(exc)!r}"
            ) from exc
        return

    raise RuntimeAcceptanceError("overflow input was accepted, but rejection was expected")


def check_acceptance(acceptance_path: Path, *, print_json: bool = False) -> None:
    acceptance = load_json(acceptance_path)

    if acceptance.get("artifact_kind") != "frog_reference_runtime_acceptance":
        raise RuntimeAcceptanceError("acceptance artifact_kind must be frog_reference_runtime_acceptance")
    if acceptance.get("runtime_family") != "reference_host_runtime_ui_binding":
        raise RuntimeAcceptanceError("runtime_family must be reference_host_runtime_ui_binding")
    if acceptance.get("example_id") != "05_bounded_ui_accumulator":
        raise RuntimeAcceptanceError("example_id must be 05_bounded_ui_accumulator")

    check_headless_snapshot(acceptance, print_json=print_json)
    check_overflow_rejection(acceptance)


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Check Example 05 runtime-family acceptance.")
    parser.add_argument("--acceptance", type=Path, default=DEFAULT_ACCEPTANCE)
    parser.add_argument("--print", action="store_true", dest="print_json", help="Print observed headless snapshot JSON.")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    acceptance_path = args.acceptance if args.acceptance.is_absolute() else ROOT / args.acceptance

    try:
        check_acceptance(acceptance_path, print_json=args.print_json)
    except RuntimeAcceptanceError as exc:
        print(f"Runtime acceptance check: FAILED", file=sys.stderr)
        print(str(exc), file=sys.stderr)
        return 1

    print("Runtime acceptance check: ok")
    print(f"acceptance: {acceptance_path.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
