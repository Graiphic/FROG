#!/usr/bin/env python3
"""Emit or check backend contracts for reference Examples 01-04.

This tool is intentionally narrow and non-normative. It makes the currently
published FIR/lowering slices for Examples 01-04 consumable by simple runtime
acceptance checks.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[3]

EXAMPLES: dict[str, dict[str, str]] = {
    "01": {
        "example_id": "01_pure_addition",
        "lowering": "Examples/01_pure_addition/main.lowering.json",
        "contract": "Implementations/Reference/ContractEmitter/examples/01_pure_addition.reference_pure_dataflow_arithmetic.contract.json",
    },
    "02": {
        "example_id": "02_ui_value_roundtrip",
        "lowering": "Examples/02_ui_value_roundtrip/main.lowering.json",
        "contract": "Implementations/Reference/ContractEmitter/examples/02_ui_value_roundtrip.reference_ui_value_roundtrip.contract.json",
    },
    "03": {
        "example_id": "03_ui_property_write",
        "lowering": "Examples/03_ui_property_write/main.lowering.json",
        "contract": "Implementations/Reference/ContractEmitter/examples/03_ui_property_write.reference_ui_property_write.contract.json",
    },
    "04": {
        "example_id": "04_stateful_feedback_delay",
        "lowering": "Examples/04_stateful_feedback_delay/main.lowering.json",
        "contract": "Implementations/Reference/ContractEmitter/examples/04_stateful_feedback_delay.reference_stateful_feedback_delay.contract.json",
    },
}


class ContractEmissionError(RuntimeError):
    pass


def load_json(path: Path) -> dict[str, Any]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ContractEmissionError(f"missing file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise ContractEmissionError(f"invalid JSON in {path}: {exc}") from exc
    if not isinstance(data, dict):
        raise ContractEmissionError(f"{path} must contain a JSON object")
    return data


def canonical_json_bytes(data: dict[str, Any]) -> bytes:
    return (json.dumps(data, sort_keys=True, separators=(",", ":")) + "\n").encode("utf-8")


def repo_path(path_text: str) -> Path:
    return ROOT / path_text


def single_unit(lowering: dict[str, Any]) -> dict[str, Any]:
    units = lowering.get("lowered_units")
    if not isinstance(units, list) or len(units) != 1 or not isinstance(units[0], dict):
        raise ContractEmissionError("lowering must contain exactly one lowered unit")
    return units[0]


def emit_contract(lowering: dict[str, Any]) -> dict[str, Any]:
    example_id = lowering.get("source_ref", {}).get("example_id")
    unit = single_unit(lowering)
    kind = unit.get("kind")
    refs = {
        "source_path": lowering["source_ref"]["path"],
        "fir_path": lowering["fir_ref"]["path"],
        "lowering_path": next(v["lowering"] for v in EXAMPLES.values() if v["example_id"] == example_id),
    }

    if example_id == "01_pure_addition":
        return {
            "artifact_kind": "frog_backend_contract",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "backend_family": "reference_pure_dataflow_arithmetic",
            "example_id": example_id,
            "artifact_refs": refs,
            "units": [{
                "unit_id": "main",
                "kind": kind,
                "public_io": unit["public_io"],
                "execution": unit["execution_kernel"]["operations"][0],
                "publications": unit["execution_kernel"]["final_publication"],
            }],
        }

    if example_id == "02_ui_value_roundtrip":
        return {
            "artifact_kind": "frog_backend_contract",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "backend_family": "reference_ui_value_roundtrip",
            "example_id": example_id,
            "artifact_refs": refs,
            "units": [{
                "unit_id": "main",
                "kind": kind,
                "public_io": unit["public_io"],
                "ui_bindings": unit["ui_bindings"],
                "execution": unit["execution_kernel"]["operations"][0],
                "publications": unit["execution_kernel"]["final_publication"],
            }],
        }

    if example_id == "03_ui_property_write":
        return {
            "artifact_kind": "frog_backend_contract",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "backend_family": "reference_ui_property_write",
            "example_id": example_id,
            "artifact_refs": refs,
            "units": [{
                "unit_id": "main",
                "kind": kind,
                "public_io": unit["public_io"],
                "ui_bindings": unit["ui_bindings"],
                "effects": unit["execution_effects"],
            }],
        }

    if example_id == "04_stateful_feedback_delay":
        return {
            "artifact_kind": "frog_backend_contract",
            "artifact_governance_ref": {"path": "Versioning/Readme.md"},
            "backend_family": "reference_stateful_feedback_delay",
            "example_id": example_id,
            "artifact_refs": refs,
            "units": [{
                "unit_id": "main",
                "kind": kind,
                "public_io": unit["public_io"],
                "execution_kernel": unit["execution_kernel"],
            }],
        }

    raise ContractEmissionError(f"unsupported example: {example_id!r}")


def check_example(key: str) -> None:
    spec = EXAMPLES[key]
    lowering = load_json(repo_path(spec["lowering"]))
    generated = emit_contract(lowering)
    expected = load_json(repo_path(spec["contract"]))
    if canonical_json_bytes(generated) != canonical_json_bytes(expected):
        raise ContractEmissionError(f"contract mismatch for {spec['example_id']}")


def write_example(key: str, output: Path | None) -> None:
    spec = EXAMPLES[key]
    lowering = load_json(repo_path(spec["lowering"]))
    generated = emit_contract(lowering)
    out_path = output if output is not None else repo_path(spec["contract"])
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(json.dumps(generated, indent=2) + "\n", encoding="utf-8")
    print(out_path)


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Emit or check backend contracts for Examples 01-04.")
    parser.add_argument("--example", choices=sorted(EXAMPLES), default=None)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--output", type=Path, default=None)
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    keys = [args.example] if args.example else sorted(EXAMPLES)

    try:
        if args.check:
            for key in keys:
                check_example(key)
                print(f"Contract emission check ok: {EXAMPLES[key]['example_id']}")
            return 0

        if args.example is None and args.output is not None:
            raise ContractEmissionError("--output requires --example")

        for key in keys:
            write_example(key, args.output)
        return 0

    except ContractEmissionError as exc:
        print(f"Contract emission error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
