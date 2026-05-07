#!/usr/bin/env python3
"""Run the current Example 05 reference pipeline checks.

This script is intentionally narrow and non-normative. It verifies that the
published Example 05 staged artifacts are reproducible across the current
reference implementation stages:

    main.frog -> main.fir.json -> main.lowering.json -> backend contract -> runtime acceptance -> LLVM module

Run from the repository root:

    python Implementations/Reference/Pipeline/check_example05_pipeline.py
"""

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


def run_stage(stage: Stage) -> int:
    print(f"\n== {stage.name} ==")
    print("$ " + " ".join(stage.command))
    result = subprocess.run(
        stage.command,
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
        print(f"Stage failed: {stage.name} (exit {result.returncode})", file=sys.stderr)
    else:
        print(f"Stage ok: {stage.name}")
    return result.returncode


def build_stages(
    *,
    include_widget_validator: bool,
    skip_runtime_acceptance: bool,
    skip_llvm: bool,
    include_llvm_build: bool,
) -> list[Stage]:
    python = sys.executable

    stages: list[Stage] = []

    if include_widget_validator:
        stages.append(
            Stage(
                "widget-layer validation",
                [python, "Implementations/Reference/WidgetValidator/validate_widget_layer.py"],
            )
        )

    stages.extend(
        [
            Stage(
                "derive .frog -> FIR",
                [python, "Implementations/Reference/Deriver/derive_example05_fir.py", "--check"],
            ),
            Stage(
                "lower FIR -> lowering",
                [python, "Implementations/Reference/Lowerer/lower_example05_fir.py", "--check"],
            ),
            Stage(
                "emit lowering -> backend contract",
                [python, "-m", "Implementations.Reference.ContractEmitter.reference_contract_emitter", "--check"],
            ),
        ]
    )

    if not skip_runtime_acceptance:
        stages.append(
            Stage(
                "runtime acceptance",
                [python, "Implementations/Reference/Runtime/check_example05_runtime_acceptance.py"],
            )
        )

    if not skip_llvm:
        llvm_command = [python, "Implementations/Reference/LLVM/tools/emit_llvm_module.py", "--check"]
        if include_llvm_build:
            llvm_command.append("--build")
        stages.append(Stage("emit lowering -> LLVM module", llvm_command))

    return stages


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run Example 05 staged pipeline checks.")
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
    parser.add_argument(
        "--skip-runtime-acceptance",
        action="store_true",
        help="Skip the contract -> runtime acceptance stage.",
    )
    parser.add_argument(
        "--skip-llvm",
        action="store_true",
        help="Skip the lowering -> LLVM module reproducibility stage.",
    )
    parser.add_argument(
        "--include-llvm-build",
        action="store_true",
        help="Run the LLVM build.sh check after verifying module emission. Requires clang.",
    )
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    include_widget_validator = args.include_widget_validator or not args.skip_widget_validator

    print("FROG Example 05 reference pipeline check")
    print("=======================================")
    print(f"Repository root: {ROOT}")

    for stage in build_stages(
        include_widget_validator=include_widget_validator,
        skip_runtime_acceptance=args.skip_runtime_acceptance,
        skip_llvm=args.skip_llvm,
        include_llvm_build=args.include_llvm_build,
    ):
        code = run_stage(stage)
        if code != 0:
            return code

    print("\nPipeline status: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
