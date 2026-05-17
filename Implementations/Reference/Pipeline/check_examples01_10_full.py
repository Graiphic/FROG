#!/usr/bin/env python3
"""Check Examples 01-15 through their current published executable depth.

The file name is retained as a compatibility entry point for older automation.
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[3]


@dataclass(frozen=True)
class Example:
    key: str
    source: str
    fir: str
    lowering: str
    contract: str | None
    module: str
    llvm_dir: str


@dataclass(frozen=True)
class Stage:
    name: str
    command: list[str]


EXAMPLES = [
    Example("01", "Examples/01_pure_addition/main.frog", "Examples/01_pure_addition/main.fir.json", "Examples/01_pure_addition/main.lowering.json", None, "Implementations/Reference/LLVM/examples/01_pure_addition/module.ll", "Implementations/Reference/LLVM/examples/01_pure_addition"),
    Example("02", "Examples/02_ui_value_roundtrip/main.frog", "Examples/02_ui_value_roundtrip/main.fir.json", "Examples/02_ui_value_roundtrip/main.lowering.json", None, "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/module.ll", "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip"),
    Example("03", "Examples/03_ui_property_write/main.frog", "Examples/03_ui_property_write/main.fir.json", "Examples/03_ui_property_write/main.lowering.json", None, "Implementations/Reference/LLVM/examples/03_ui_property_write/module.ll", "Implementations/Reference/LLVM/examples/03_ui_property_write"),
    Example("04", "Examples/04_stateful_feedback_delay/main.frog", "Examples/04_stateful_feedback_delay/main.fir.json", "Examples/04_stateful_feedback_delay/main.lowering.json", None, "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/module.ll", "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay"),
    Example("05", "Examples/05_bounded_ui_accumulator/main.frog", "Examples/05_bounded_ui_accumulator/main.fir.json", "Examples/05_bounded_ui_accumulator/main.lowering.json", "Implementations/Reference/ContractEmitter/examples/05_bounded_ui_accumulator.reference_host_runtime_ui_binding.contract.json", "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator/module.ll", "Implementations/Reference/LLVM/examples/05_bounded_ui_accumulator"),
    Example("06", "Examples/06_boolean_value_roundtrip/main.frog", "Examples/06_boolean_value_roundtrip/main.fir.json", "Examples/06_boolean_value_roundtrip/main.lowering.json", "Implementations/Reference/ContractEmitter/examples/06_boolean_value_roundtrip.reference_host_runtime_ui_binding.contract.json", "Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip/module.ll", "Implementations/Reference/LLVM/examples/06_boolean_value_roundtrip"),
    Example("07", "Examples/07_string_value_roundtrip/main.frog", "Examples/07_string_value_roundtrip/main.fir.json", "Examples/07_string_value_roundtrip/main.lowering.json", "Implementations/Reference/ContractEmitter/examples/07_string_value_roundtrip.reference_host_runtime_ui_binding.contract.json", "Implementations/Reference/LLVM/examples/07_string_value_roundtrip/module.ll", "Implementations/Reference/LLVM/examples/07_string_value_roundtrip"),
    Example("08", "Examples/08_enum_value_roundtrip/main.frog", "Examples/08_enum_value_roundtrip/main.fir.json", "Examples/08_enum_value_roundtrip/main.lowering.json", "Implementations/Reference/ContractEmitter/examples/08_enum_value_roundtrip.reference_host_runtime_ui_binding.contract.json", "Implementations/Reference/LLVM/examples/08_enum_value_roundtrip/module.ll", "Implementations/Reference/LLVM/examples/08_enum_value_roundtrip"),
    Example("09", "Examples/09_path_value_roundtrip/main.frog", "Examples/09_path_value_roundtrip/main.fir.json", "Examples/09_path_value_roundtrip/main.lowering.json", "Implementations/Reference/ContractEmitter/examples/09_path_value_roundtrip.reference_host_runtime_ui_binding.contract.json", "Implementations/Reference/LLVM/examples/09_path_value_roundtrip/module.ll", "Implementations/Reference/LLVM/examples/09_path_value_roundtrip"),
    Example("10", "Examples/10_button_press_to_boolean/main.frog", "Examples/10_button_press_to_boolean/main.fir.json", "Examples/10_button_press_to_boolean/main.lowering.json", "Implementations/Reference/ContractEmitter/examples/10_button_press_to_boolean.reference_host_runtime_ui_binding.contract.json", "Implementations/Reference/LLVM/examples/10_button_press_to_boolean/module.ll", "Implementations/Reference/LLVM/examples/10_button_press_to_boolean"),
    Example("11", "Examples/11_button_switch_when_pressed/main.frog", "Examples/11_button_switch_when_pressed/main.fir.json", "Examples/11_button_switch_when_pressed/main.lowering.json", "Implementations/Reference/ContractEmitter/examples/11_button_switch_when_pressed.reference_host_runtime_ui_binding.contract.json", "Implementations/Reference/LLVM/examples/11_button_switch_when_pressed/kernel.ll", "Implementations/Reference/LLVM/examples/11_button_switch_when_pressed"),
    Example("12", "Examples/12_button_switch_when_released/main.frog", "Examples/12_button_switch_when_released/main.fir.json", "Examples/12_button_switch_when_released/main.lowering.json", "Implementations/Reference/ContractEmitter/examples/12_button_switch_when_released.reference_host_runtime_ui_binding.contract.json", "Implementations/Reference/LLVM/examples/12_button_switch_when_released/kernel.ll", "Implementations/Reference/LLVM/examples/12_button_switch_when_released"),
    Example("13", "Examples/13_button_latch_when_pressed/main.frog", "Examples/13_button_latch_when_pressed/main.fir.json", "Examples/13_button_latch_when_pressed/main.lowering.json", "Implementations/Reference/ContractEmitter/examples/13_button_latch_when_pressed.reference_host_runtime_ui_binding.contract.json", "Implementations/Reference/LLVM/examples/13_button_latch_when_pressed/kernel.ll", "Implementations/Reference/LLVM/examples/13_button_latch_when_pressed"),
    Example("14", "Examples/14_button_latch_when_released/main.frog", "Examples/14_button_latch_when_released/main.fir.json", "Examples/14_button_latch_when_released/main.lowering.json", "Implementations/Reference/ContractEmitter/examples/14_button_latch_when_released.reference_host_runtime_ui_binding.contract.json", "Implementations/Reference/LLVM/examples/14_button_latch_when_released/kernel.ll", "Implementations/Reference/LLVM/examples/14_button_latch_when_released"),
    Example("15", "Examples/15_button_latch_until_released/main.frog", "Examples/15_button_latch_until_released/main.fir.json", "Examples/15_button_latch_until_released/main.lowering.json", "Implementations/Reference/ContractEmitter/examples/15_button_latch_until_released.reference_host_runtime_ui_binding.contract.json", "Implementations/Reference/LLVM/examples/15_button_latch_until_released/kernel.ll", "Implementations/Reference/LLVM/examples/15_button_latch_until_released"),
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
        out.append(Stage("Examples 01-10 artifact preflight", [py, "Implementations/Reference/ArtifactChecks/check_examples01_10_artifacts.py"]))

    if include_widget_validator:
        out.append(Stage("Widget layer validation", [py, "Implementations/Reference/WidgetValidator/validate_widget_layer.py"]))

    for example in EXAMPLES:
        out.append(Stage(f"Example {example.key} .frog -> FIR", [py, "Implementations/Reference/Deriver/derive_fir.py", "--source", example.source, "--expected", example.fir, "--check"]))
        out.append(Stage(f"Example {example.key} FIR -> lowering", [py, "Implementations/Reference/Lowerer/lower_fir.py", "--fir", example.fir, "--expected", example.lowering, "--check"]))

    out.append(Stage("Examples 01-04 lowering -> backend contract", [py, "Implementations/Reference/ContractEmitter/emit_examples01_04_contracts.py", "--check"]))
    for example in EXAMPLES:
        if example.contract is None:
            continue
        out.append(Stage(
            f"Example {example.key} lowering -> backend contract",
            [py, "-m", "Implementations.Reference.ContractEmitter.reference_contract_emitter", "--lowering", example.lowering, "--expected", example.contract, "--check"],
        ))

    out.append(Stage("Examples 01-10 historical runtime acceptance", [py, "Implementations/Reference/Runtime/check_examples01_10_runtime_acceptance.py"]))

    for example in EXAMPLES:
        out.append(Stage(
            f"Example {example.key} lowering -> LLVM module",
            [py, "Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py", "--lowering", example.lowering, "--expected", example.module, "--check"],
        ))

    if include_llvm_build:
        for example in EXAMPLES:
            if not (ROOT / example.llvm_dir / "build.sh").is_file():
                continue
            out.append(Stage(
                f"Example {example.key} LLVM native build",
                [py, "Implementations/Reference/LLVM/tools/emit_lowering_to_llvm.py", "--lowering", example.lowering, "--expected", example.module, "--check", "--build", "--example-dir", example.llvm_dir],
            ))

    return out


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--include-widget-validator", action="store_true")
    parser.add_argument("--skip-widget-validator", action="store_true")
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

    print("\nExamples 01-15 full status: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
