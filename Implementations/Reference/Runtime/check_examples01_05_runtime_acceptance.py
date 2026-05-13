#!/usr/bin/env python3
"""Check runtime acceptance for the published reference runtime examples.

Examples 01-04 use the generic reference contract executor.
Example 05 delegates to the existing richer runtime-family acceptance checker.
Example 06 uses the generic executor with .frog-owned front-panel instances and a .wfrog realization package.
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

try:
    from .contract_executor import ContractExecutionError, check_acceptance_against_snapshot, load_json
except ImportError:  # pragma: no cover
    from contract_executor import ContractExecutionError, check_acceptance_against_snapshot, load_json  # type: ignore


ROOT = Path(__file__).resolve().parents[3]

ACCEPTANCE_FILES = [
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


def check_acceptance_file(rel_path: str) -> None:
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


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Check runtime acceptance for the published reference examples.")
    parser.add_argument("--skip-example05", action="store_true")
    args = parser.parse_args(argv)

    try:
        for rel_path in ACCEPTANCE_FILES:
            check_acceptance_file(rel_path)
        if not args.skip_example05:
            check_example05()
        print("Runtime acceptance status: ok")
        return 0
    except (RuntimeAcceptanceError, ContractExecutionError) as exc:
        print(f"Runtime acceptance error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
