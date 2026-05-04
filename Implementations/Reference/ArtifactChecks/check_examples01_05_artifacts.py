#!/usr/bin/env python3
"""Check that the published Examples 01-05 executable corridor artifacts exist.

This checker is non-normative. It protects repository coherence only.

It verifies that the files announced by the example and reference-workspace
documentation are present before the heavier derivation, lowering, contract,
runtime, and LLVM checks run.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[3]


EXAMPLE_ARTIFACTS: dict[str, list[str]] = {
    "01_pure_addition": [
        "Examples/01_pure_addition/main.frog",
        "Examples/01_pure_addition/main.fir.json",
        "Examples/01_pure_addition/main.lowering.json",
        "Implementations/Reference/ContractEmitter/examples/01_pure_addition.reference_pure_dataflow_arithmetic.contract.json",
        "Implementations/Reference/Runtime/acceptance/example01_pure_addition.acceptance.json",
        "Implementations/Reference/Runtime/acceptance/example01_pure_addition.snapshot.json",
        "Implementations/Reference/LLVM/examples/01_pure_addition/module.ll",
        "Implementations/Reference/LLVM/examples/01_pure_addition/build.sh",
        "Implementations/Reference/LLVM/examples/01_pure_addition/expected-output.json",
    ],
    "02_ui_value_roundtrip": [
        "Examples/02_ui_value_roundtrip/main.frog",
        "Examples/02_ui_value_roundtrip/main.fir.json",
        "Examples/02_ui_value_roundtrip/main.lowering.json",
        "Implementations/Reference/ContractEmitter/examples/02_ui_value_roundtrip.reference_ui_value_roundtrip.contract.json",
        "Implementations/Reference/Runtime/acceptance/example02_ui_value_roundtrip.acceptance.json",
        "Implementations/Reference/Runtime/acceptance/example02_ui_value_roundtrip.snapshot.json",
        "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/module.ll",
        "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/build.sh",
        "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/expected-output.json",
    ],
    "03_ui_property_write": [
        "Examples/03_ui_property_write/main.frog",
        "Examples/03_ui_property_write/main.fir.json",
        "Examples/03_ui_property_write/main.lowering.json",
        "Implementations/Reference/ContractEmitter/examples/03_ui_property_write.reference_ui_property_write.contract.json",
        "Implementations/Reference/Runtime/acceptance/example03_ui_property_write.acceptance.json",
        "Implementations/Reference/Runtime/acceptance/example03_ui_property_write.snapshot.json",
        "Implementations/Reference/LLVM/examples/03_ui_property_write/module.ll",
        "Implementations/Reference/LLVM/examples/03_ui_property_write/build.sh",
        "Implementations/Reference/LLVM/examples/03_ui_property_write/expected-output.json",
    ],
    "04_stateful_feedback_delay": [
        "Examples/04_stateful_feedback_delay/main.frog",
        "Examples/04_stateful_feedback_delay/main.fir.json",
        "Examples/04_stateful_feedback_delay/main.lowering.json",
        "Implementations/Reference/ContractEmitter/examples/04_stateful_feedback_delay.reference_stateful_feedback_delay.contract.json",
        "Implementations/Reference/Runtime/acceptance/example04_stateful_feedback_delay.acceptance.json",
        "Implementations/Reference/Runtime/acceptance/example04_stateful_feedback_delay.snapshot.json",
        "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/module.ll",
        "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/build.sh",
        "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/expected-output.json",
    ],
    "05_bounded_ui_accumulator": [
        "Examples/05_bounded_ui_accumulator/main.frog",
        "Examples/05_bounded_ui_accumulator/main.fir.json",
        "Examples/05_bounded_ui_accumulator/main.lowering.json",
        "Examples/05_bounded_ui_accumulator/ui/accumulator_panel.wfrog",
        "Implementations/Reference/ContractEmitter/examples/05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json",
        "Implementations/Reference/Runtime/acceptance/example05_runtime_family.acceptance.json",
        "Implementations/Reference/Runtime/acceptance/example05_input_3.snapshot.json",
        "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/module.ll",
        "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/build.sh",
        "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/expected-output.json",
    ],
}


REQUIRED_TOOLS = [
    "Implementations/Reference/Deriver/derive_fir.py",
    "Implementations/Reference/Lowerer/lower_fir.py",
    "Implementations/Reference/ContractEmitter/emit_examples01_04_contracts.py",
    "Implementations/Reference/ContractEmitter/reference_contract_emitter.py",
    "Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py",
    "Implementations/Reference/LLVM/tools/emit_examples01_04_llvm_modules.py",
    "Implementations/Reference/LLVM/tools/emit_llvm_module.py",
    "Implementations/Reference/Pipeline/check_examples01_05_full.py",
]


class ArtifactCheckError(RuntimeError):
    pass


def load_json(path: Path) -> dict[str, Any]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise ArtifactCheckError(f"missing JSON file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise ArtifactCheckError(f"invalid JSON in {path}: {exc}") from exc
    if not isinstance(data, dict):
        raise ArtifactCheckError(f"{path} must contain a JSON object")
    return data


def check_exists(rel_path: str) -> None:
    path = ROOT / rel_path
    if not path.is_file():
        raise ArtifactCheckError(f"missing required artifact: {rel_path}")


def check_json_identity(example_id: str, rel_path: str, key: str, expected: str) -> None:
    data = load_json(ROOT / rel_path)
    observed = data.get(key)
    if observed != expected:
        raise ArtifactCheckError(
            f"{rel_path}: expected {key}={expected!r}, got {observed!r} for {example_id}"
        )


def check_example(example_id: str, rel_paths: list[str]) -> None:
    for rel_path in rel_paths:
        check_exists(rel_path)

    fir_path = next(p for p in rel_paths if p.endswith("main.fir.json"))
    lowering_path = next(p for p in rel_paths if p.endswith("main.lowering.json"))
    contract_paths = [p for p in rel_paths if p.endswith(".contract.json")]

    check_json_identity(example_id, fir_path, "artifact_kind", "frog_fir_unit")
    check_json_identity(example_id, lowering_path, "artifact_kind", "frog_lowered_unit")

    fir = load_json(ROOT / fir_path)
    lowering = load_json(ROOT / lowering_path)
    if fir.get("source_ref", {}).get("example_id") != example_id:
        raise ArtifactCheckError(f"{fir_path}: source_ref.example_id mismatch")
    if lowering.get("source_ref", {}).get("example_id") != example_id:
        raise ArtifactCheckError(f"{lowering_path}: source_ref.example_id mismatch")

    for contract_path in contract_paths:
        contract = load_json(ROOT / contract_path)
        if contract.get("artifact_kind") != "frog_backend_contract":
            raise ArtifactCheckError(f"{contract_path}: artifact_kind must be frog_backend_contract")
        if contract.get("example_id") != example_id:
            raise ArtifactCheckError(f"{contract_path}: example_id mismatch")

    print(f"Artifact preflight ok: {example_id}")


def run_preflight() -> None:
    for rel_path in REQUIRED_TOOLS:
        check_exists(rel_path)

    for example_id, rel_paths in EXAMPLE_ARTIFACTS.items():
        check_example(example_id, rel_paths)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Check published Examples 01-05 reference artifacts.")
    parser.parse_args(argv)

    try:
        run_preflight()
    except ArtifactCheckError as exc:
        print(f"Artifact preflight error: {exc}", file=sys.stderr)
        return 1

    print("Artifact preflight status: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
