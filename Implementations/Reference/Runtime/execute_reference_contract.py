#!/usr/bin/env python3
"""Execute or check a reference backend contract acceptance.

This is the CLI wrapper around Runtime/contract_executor.py.
It supports the generic contract acceptance surface used by the current
workspace checks. When an acceptance artifact declares artifact_refs.wfrog_path,
the referenced .wfrog package is loaded and provided to the generic contract
executor as support_artifacts['wfrog'].
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

try:
    from .contract_executor import (
        ContractExecutionError,
        check_acceptance_against_snapshot,
        execute_acceptance,
        load_json,
    )
except ImportError:  # pragma: no cover
    from contract_executor import (  # type: ignore
        ContractExecutionError,
        check_acceptance_against_snapshot,
        execute_acceptance,
        load_json,
    )


ROOT = Path(__file__).resolve().parents[3]


def repo_path(path_text: str) -> Path:
    path = Path(path_text)
    return path if path.is_absolute() else ROOT / path


def support_artifacts_from_refs(refs: dict[str, object]) -> dict[str, object]:
    wfrog_path = refs.get("wfrog_path")
    if not isinstance(wfrog_path, str) or not wfrog_path:
        return {}
    return {"wfrog": load_json(repo_path(wfrog_path))}


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Execute or check a reference backend contract acceptance.")
    parser.add_argument("--acceptance", type=Path, required=True)
    parser.add_argument("--contract", type=Path, default=None)
    parser.add_argument("--snapshot", type=Path, default=None)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--print", action="store_true", dest="print_json")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    acceptance_path = args.acceptance if args.acceptance.is_absolute() else ROOT / args.acceptance

    try:
        acceptance = load_json(acceptance_path)
        refs = acceptance.get("artifact_refs")
        if not isinstance(refs, dict):
            raise ContractExecutionError("acceptance.artifact_refs must be an object")

        contract_path = args.contract if args.contract is not None else repo_path(str(refs.get("contract_path")))
        snapshot_path = args.snapshot if args.snapshot is not None else repo_path(str(refs.get("snapshot_path")))

        if not contract_path.is_absolute():
            contract_path = ROOT / contract_path
        if not snapshot_path.is_absolute():
            snapshot_path = ROOT / snapshot_path

        contract = load_json(contract_path)
        support_artifacts = support_artifacts_from_refs(refs)
        observed = execute_acceptance(acceptance, contract, support_artifacts)

        if args.print_json:
            json.dump(observed, sys.stdout, indent=2)
            sys.stdout.write("\n")

        if args.check:
            snapshot = load_json(snapshot_path)
            check_acceptance_against_snapshot(acceptance, contract, snapshot, support_artifacts)
            print(f"Reference contract execution check ok: {acceptance.get('example_id')}")

        return 0

    except ContractExecutionError as exc:
        print(f"Reference contract execution error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
