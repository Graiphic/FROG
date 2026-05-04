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
    ("01", "Examples/01_pure_addition/main.frog", "Examples/01_pure_addition/main.fir.json", "Examples/01_pure_addition/main.lowering.json"),
    ("02", "Examples/02_ui_value_roundtrip/main.frog", "Examples/02_ui_value_roundtrip/main.fir.json", "Examples/02_ui_value_roundtrip/main.lowering.json"),
    ("03", "Examples/03_ui_property_write/main.frog", "Examples/03_ui_property_write/main.fir.json", "Examples/03_ui_property_write/main.lowering.json"),
    ("04", "Examples/04_stateful_feedback_delay/main.frog", "Examples/04_stateful_feedback_delay/main.fir.json", "Examples/04_stateful_feedback_delay/main.lowering.json"),
    ("05", "Examples/05_bounded_ui_accumulator/main.frog", "Examples/05_bounded_ui_accumulator/main.fir.json", "Examples/05_bounded_ui_accumulator/main.lowering.json"),
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


def stages(include_widget_validator: bool, include_llvm_build: bool) -> list[Stage]:
    py = sys.executable
    out: list[Stage] = []
    if include_widget_validator:
        out.append(Stage("widget layer validation", [py, "Implementations/Reference/WidgetValidator/validate_widget_layer.py"]))
    for key, source, fir, lowering in EXAMPLES:
        out.append(Stage(f"Example {key} .frog -> FIR", [py, "Implementations/Reference/Deriver/derive_fir.py", "--source", source, "--expected", fir, "--check"]))
        out.append(Stage(f"Example {key} FIR -> lowering", [py, "Implementations/Reference/Lowerer/lower_fir.py", "--fir", fir, "--expected", lowering, "--check"]))
    out.append(Stage("Examples 01-04 lowering -> backend contract", [py, "Implementations/Reference/ContractEmitter/emit_examples01_04_contracts.py", "--check"]))
    out.append(Stage("Examples 01-05 runtime acceptance", [py, "Implementations/Reference/Runtime/check_examples01_05_runtime_acceptance.py"]))
    out.append(Stage("Examples 01-04 LLVM modules", [py, "Implementations/Reference/LLVM/tools/check_examples01_04_llvm_modules.py", "--check"]))
    out.append(Stage("Example 05 LLVM module", [py, "Implementations/Reference/LLVM/tools/emit_llvm_module.py", "--check"]))
    if include_llvm_build:
        out.append(Stage("Examples 01-04 LLVM native build", [py, "Implementations/Reference/LLVM/tools/check_examples01_04_llvm_modules.py", "--build"]))
        out.append(Stage("Example 05 LLVM native build", [py, "Implementations/Reference/LLVM/tools/emit_llvm_module.py", "--check", "--build"]))
    return out


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--include-widget-validator", action="store_true")
    parser.add_argument("--include-llvm-build", action="store_true")
    args = parser.parse_args()
    for stage in stages(args.include_widget_validator, args.include_llvm_build):
        code = run(stage)
        if code != 0:
            return code
    print("\nExamples 01-05 full status: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
