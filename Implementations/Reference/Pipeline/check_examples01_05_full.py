#!/usr/bin/env python3
"""Check Examples 01-05 through their current published executable depth."""

from __future__ import annotations

import argparse
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[3]


@dataclass(frozen=True)
class Stage:
    name: str
    command: list[str]


EXAMPLES = [
    (
        "01",
        "Examples/01_pure_addition/main.frog",
        "Examples/01_pure_addition/main.fir.json",
        "Examples/01_pure_addition/main.lowering.json",
        "Implementations/Reference/LLVM/examples/01_pure_addition/module.ll",
        "Implementations/Reference/LLVM/examples/01_pure_addition",
    ),
    (
        "02",
        "Examples/02_ui_value_roundtrip/main.frog",
        "Examples/02_ui_value_roundtrip/main.fir.json",
        "Examples/02_ui_value_roundtrip/main.lowering.json",
        "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/module.ll",
        "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip",
    ),
    (
        "03",
        "Examples/03_ui_property_write/main.frog",
        "Examples/03_ui_property_write/main.fir.json",
        "Examples/03_ui_property_write/main.lowering.json",
        "Implementations/Reference/LLVM/examples/03_ui_property_write/module.ll",
        "Implementations/Reference/LLVM/examples/03_ui_property_write",
    ),
    (
        "04",
        "Examples/04_stateful_feedback_delay/main.frog",
        "Examples/04_stateful_feedback_delay/main.fir.json",
        "Examples/04_stateful_feedback_delay/main.lowering.json",
        "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/module.ll",
        "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay",
    ),
    (
        "05",
        "Examples/05_bounded_ui_accumulator/main.frog",
        "Examples/05_bounded_ui_accumulator/main.fir.json",
        "Examples/05_bounded_ui_accumulator/main.lowering.json",
        "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/module.ll",
        "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator",
    ),
]


def run(stage: Stage) -> int:
    print(f"\n== {stage.name} ==")
    print("$ " + " ".join(stage.command))
    result = subprocess.run(stage.command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    if result.returncode != 0:
        print(f"Stage failed: {stage.name}", file=sys.stderr)
    else:
        print(f"Stage ok: {stage.name}")
    return result.returncode


def stages(include_widget_validator: bool, include_llvm_build: bool, skip_artifact_preflight: bool) -> list[Stage]:
    py = sys.executable
    out: list[Stage] = []

    if not skip_artifact_preflight:
        out.append(Stage("Examples 01-05 artifact preflight", [py, "Implementations/Reference/ArtifactChecks/check_examples01_05_artifacts.py"]))

    if include_widget_validator:
        out.append(Stage("Widget layer validation", [py, "Implementations/Reference/WidgetValidator/validate_widget_layer.py"]))

    for key, source, fir, lowering, module, example_dir in EXAMPLES:
        out.append(Stage(f"Example {key} .frog -> FIR", [py, "Implementations/Reference/Deriver/derive_fir.py", "--source", source, "--expected", fir, "--check"]))
        out.append(Stage(f"Example {key} FIR -> lowering", [py, "Implementations/Reference/Lowerer/lower_fir.py", "--fir", fir, "--expected", lowering, "--check"]))

    out.append(Stage("Examples 01-04 lowering -> backend contract", [py, "Implementations/Reference/ContractEmitter/emit_examples01_04_contracts.py", "--check"]))
    out.append(Stage("Example 05 lowering -> backend contract", [py, "-m", "Implementations.Reference.ContractEmitter.reference_contract_emitter", "--check"]))
    out.append(Stage("Examples 01-05 runtime acceptance", [py, "Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py"]))

    for key, source, fir, lowering, module, example_dir in EXAMPLES:
        cmd = [
            py,
            "Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py",
            "--lowering",
            lowering,
            "--expected",
            module,
            "--check",
        ]
        out.append(Stage(f"Example {key} lowering -> LLVM module", cmd))

    if include_llvm_build:
        for key, source, fir, lowering, module, example_dir in EXAMPLES:
            cmd = [
                py,
                "Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py",
                "--lowering",
                lowering,
                "--expected",
                module,
                "--check",
                "--build",
                "--example-dir",
                example_dir,
            ]
            out.append(Stage(f"Example {key} LLVM native build", cmd))

    return out


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--include-widget-validator",
        action="store_true",
        help="Compatibility flag. Widget validation is included by default unless --skip-widget-validator is used.",
    )
    parser.add_argument(
        "--skip-widget-validator",
        action="store_true",
        help="Skip the non-normative widget-layer repository hygiene validator.",
    )
    parser.add_argument("--include-llvm-build", action="store_true")
    parser.add_argument("--skip-artifact-preflight", action="store_true")
    args = parser.parse_args()

    include_widget_validator = args.include_widget_validator or not args.skip_widget_validator

    for stage in stages(
        include_widget_validator=include_widget_validator,
        include_llvm_build=args.include_llvm_build,
        skip_artifact_preflight=args.skip_artifact_preflight,
    ):
        code = run(stage)
        if code != 0:
            return code

    print("\nExamples 01-05 full status: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
