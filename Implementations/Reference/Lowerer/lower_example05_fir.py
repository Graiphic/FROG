#!/usr/bin/env python3
"""Lower the published Example 05 FIR into its published lowered artifact.

This tool is intentionally narrow. It is the first reproducible FIR -> lowering
reference step for the frozen Example 05 corridor. It does not implement a
general FROG lowering engine.

Run from the repository root:

    python Implementations/Reference/Lowerer/lower_example05_fir.py --check

or:

    python Implementations/Reference/Lowerer/lower_example05_fir.py \
      --fir Examples/05_bounded_ui_accumulator/main.fir.json \
      --output build/generated/main.lowering.json
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[3]

DEFAULT_FIR = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.fir.json"
DEFAULT_EXPECTED_LOWERING = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.lowering.json"


class LoweringError(RuntimeError):
    """Raised when the supported Example 05 lowering cannot proceed."""


def load_json(path: Path) -> dict[str, Any]:
    try:
        with path.open("r", encoding="utf-8") as handle:
            data = json.load(handle)
    except FileNotFoundError as exc:
        raise LoweringError(f"missing file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise LoweringError(f"invalid JSON in {path}: {exc}") from exc

    if not isinstance(data, dict):
        raise LoweringError(f"{path} must contain a JSON object")
    return data


def dump_json(data: dict[str, Any], path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", encoding="utf-8", newline="\n") as handle:
        json.dump(data, handle, indent=2)
        handle.write("\n")


def canonical_json_bytes(data: dict[str, Any]) -> bytes:
    return (json.dumps(data, sort_keys=True, separators=(",", ":")) + "\n").encode("utf-8")


def require_object(value: Any, name: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise LoweringError(f"{name} must be an object")
    return value


def require_list(value: Any, name: str) -> list[Any]:
    if not isinstance(value, list):
        raise LoweringError(f"{name} must be an array")
    return value


def single_unit(fir: dict[str, Any]) -> dict[str, Any]:
    units = require_list(fir.get("units"), "fir.units")
    if len(units) != 1:
        raise LoweringError(f"Example 05 expects exactly one FIR unit, found {len(units)}")
    unit = require_object(units[0], "fir.units[0]")
    if unit.get("unit_id") != "main":
        raise LoweringError("Example 05 FIR unit_id must be main")
    if unit.get("kind") != "bounded_stateful_ui_unit":
        raise LoweringError("Example 05 FIR kind must be bounded_stateful_ui_unit")
    return unit


def validate_public_interface(unit: dict[str, Any]) -> tuple[list[dict[str, str]], list[dict[str, str]]]:
    public_interface = require_object(unit.get("public_interface"), "unit.public_interface")
    inputs = require_list(public_interface.get("inputs"), "unit.public_interface.inputs")
    outputs = require_list(public_interface.get("outputs"), "unit.public_interface.outputs")

    def normalize(items: list[Any], name: str) -> list[dict[str, str]]:
        out: list[dict[str, str]] = []
        for idx, item in enumerate(items):
            obj = require_object(item, f"{name}[{idx}]")
            item_id = obj.get("id")
            item_type = obj.get("type")
            if not isinstance(item_id, str) or not isinstance(item_type, str):
                raise LoweringError(f"{name}[{idx}] must contain string id and type")
            out.append({"id": item_id, "type": item_type})
        return out

    norm_inputs = normalize(inputs, "unit.public_interface.inputs")
    norm_outputs = normalize(outputs, "unit.public_interface.outputs")

    if len(norm_inputs) != 1 or len(norm_outputs) != 1:
        raise LoweringError("Example 05 expects one public input and one public output")
    return norm_inputs, norm_outputs


def derive_iteration_body(execution_model: dict[str, Any]) -> list[dict[str, Any]]:
    body_rule = require_object(execution_model.get("body_rule"), "unit.execution_model.body_rule")
    expression = body_rule.get("expression")
    kind = body_rule.get("kind")

    if kind != "accumulate_with_explicit_state":
        raise LoweringError("Example 05 lowerer supports only accumulate_with_explicit_state")

    if expression != "state_next = state_current + input_value":
        raise LoweringError(f"unsupported Example 05 expression: {expression!r}")

    return [
        {
            "op": "add",
            "dst": "state_next",
            "src": [
                "state_current",
                "input_value"
            ]
        }
    ]


def derive_lowering(fir: dict[str, Any], fir_path: Path) -> dict[str, Any]:
    if fir.get("artifact_kind") != "frog_fir_unit":
        raise LoweringError("input artifact_kind must be frog_fir_unit")

    source_ref = require_object(fir.get("source_ref"), "fir.source_ref")
    example_id = source_ref.get("example_id")
    if example_id != "05_bounded_ui_accumulator":
        raise LoweringError("this lowerer currently supports only 05_bounded_ui_accumulator")

    unit = single_unit(fir)
    inputs, outputs = validate_public_interface(unit)

    ui_bindings = require_object(unit.get("ui_bindings"), "unit.ui_bindings")
    state_model = require_object(unit.get("state_model"), "unit.state_model")
    execution_model = require_object(unit.get("execution_model"), "unit.execution_model")
    publications = require_list(unit.get("publications"), "unit.publications")

    if execution_model.get("structure") != "for_loop":
        raise LoweringError("Example 05 expects a for_loop execution model")

    iteration_count = execution_model.get("iteration_count")
    if not isinstance(iteration_count, int):
        raise LoweringError("iteration_count must be an integer")

    carrier = require_object(state_model.get("carrier"), "unit.state_model.carrier")
    initial_state = carrier.get("initial_value")
    state_type = carrier.get("type")
    if not isinstance(initial_state, int):
        raise LoweringError("state initial_value must be an integer")
    if not isinstance(state_type, str):
        raise LoweringError("state type must be a string")

    iteration_body = derive_iteration_body(execution_model)

    source_path = source_ref.get("path")
    entry_unit = source_ref.get("entry_unit")
    if not isinstance(source_path, str) or not isinstance(entry_unit, str):
        raise LoweringError("source_ref.path and source_ref.entry_unit must be strings")

    fir_rel = fir_path.relative_to(ROOT).as_posix() if fir_path.is_absolute() else fir_path.as_posix()

    return {
        "artifact_kind": "frog_lowered_unit",
        "artifact_version": "0.1-draft",
        "source_ref": {
            "example_id": example_id,
            "path": source_path,
            "entry_unit": entry_unit
        },
        "fir_ref": {
            "path": fir_rel,
            "unit_id": unit["unit_id"]
        },
        "lowering_intent": {
            "purpose": "make the bounded example corridor consumable by runtime families and by future compiler-family paths",
            "backend_family_target": "reference_host_runtime_ui_binding",
            "compiler_family_targets": [
                "llvm_oriented_native_path"
            ]
        },
        "lowered_units": [
            {
                "unit_id": unit["unit_id"],
                "kind": "bounded_accumulator_kernel_with_ui_bindings",
                "public_io": {
                    "inputs": inputs,
                    "outputs": outputs
                },
                "ui_bindings": ui_bindings,
                "execution_kernel": {
                    "initial_state": initial_state,
                    "state_type": state_type,
                    "iteration_count": iteration_count,
                    "iteration_body": iteration_body,
                    "commit_rule": "state_current <- state_next after each iteration",
                    "final_publication": publications
                }
            }
        ]
    }


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Lower Example 05 FIR into the published lowered artifact.")
    parser.add_argument("--fir", type=Path, default=DEFAULT_FIR, help="Path to the Example 05 FIR artifact.")
    parser.add_argument("--output", type=Path, default=None, help="Optional path where generated lowering should be written.")
    parser.add_argument("--expected", type=Path, default=DEFAULT_EXPECTED_LOWERING, help="Expected lowering artifact used by --check.")
    parser.add_argument("--check", action="store_true", help="Compare generated lowering against the expected published lowering.")
    parser.add_argument("--print", action="store_true", dest="print_json", help="Print generated lowering to stdout.")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    fir_path = args.fir
    if not fir_path.is_absolute():
        fir_path = ROOT / fir_path

    expected_path = args.expected
    if not expected_path.is_absolute():
        expected_path = ROOT / expected_path

    try:
        fir = load_json(fir_path)
        generated = derive_lowering(fir, fir_path)

        if args.output:
            out_path = args.output if args.output.is_absolute() else ROOT / args.output
            dump_json(generated, out_path)
            print(f"wrote generated lowering: {out_path}")

        if args.print_json:
            json.dump(generated, sys.stdout, indent=2)
            sys.stdout.write("\n")

        if args.check:
            expected = load_json(expected_path)
            if canonical_json_bytes(generated) != canonical_json_bytes(expected):
                print("Lowering check: FAILED", file=sys.stderr)
                print(f"expected: {expected_path}", file=sys.stderr)
                return 1
            print("Lowering check: ok")
            print(f"fir:      {fir_path.relative_to(ROOT)}")
            print(f"expected: {expected_path.relative_to(ROOT)}")

        return 0
    except LoweringError as exc:
        print(f"Lowering error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
