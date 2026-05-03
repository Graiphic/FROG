#!/usr/bin/env python3
"""Run the repository-visible FROG reference workspace checks.

This script is non-normative. It is a repository-quality coordinator for the
current bounded Example 05 corridor and widget layer.

Run from the repository root:

    python Implementations/Reference/check_reference_workspace.py

Useful CI form:

    python Implementations/Reference/check_reference_workspace.py --include-pytest
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


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


def build_stages(*, include_pytest: bool, include_llvm_build: bool) -> list[Stage]:
    python = sys.executable

    stages: list[Stage] = [
        Stage(
            "widget layer validation",
            [
                python,
                "Implementations/Reference/WidgetValidator/validate_widget_layer.py",
            ],
        ),
        Stage(
            "Example 05 staged pipeline",
            [
                python,
                "Implementations/Reference/Pipeline/check_example05_pipeline.py",
            ],
        ),
    ]

    if include_llvm_build:
        stages.append(
            Stage(
                "LLVM native build proof",
                [
                    python,
                    "Implementations/Reference/LLVM/tools/emit_llvm_module.py",
                    "--check",
                    "--build",
                ],
            )
        )

    if include_pytest:
        stages.extend(
            [
                Stage(
                    "Deriver tests",
                    [
                        python,
                        "-m",
                        "pytest",
                        "Implementations/Reference/Deriver/tests",
                    ],
                ),
                Stage(
                    "Lowerer tests",
                    [
                        python,
                        "-m",
                        "pytest",
                        "Implementations/Reference/Lowerer/tests",
                    ],
                ),
                Stage(
                    "ContractEmitter tests",
                    [
                        python,
                        "-m",
                        "pytest",
                        "Implementations/Reference/ContractEmitter/tests",
                    ],
                ),
                Stage(
                    "Runtime tests",
                    [
                        python,
                        "-m",
                        "pytest",
                        "Implementations/Reference/Runtime/tests",
                    ],
                ),
                Stage(
                    "LLVM emission tests",
                    [
                        python,
                        "-m",
                        "pytest",
                        "Implementations/Reference/LLVM/tests",
                    ],
                ),
                Stage(
                    "Pipeline tests",
                    [
                        python,
                        "-m",
                        "pytest",
                        "Implementations/Reference/Pipeline/tests",
                    ],
                ),
            ]
        )

    return stages


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run FROG reference workspace checks.")
    parser.add_argument(
        "--include-pytest",
        action="store_true",
        help="Run pytest-based stage tests in addition to direct reference checks.",
    )
    parser.add_argument(
        "--include-llvm-build",
        action="store_true",
        help="Run the native LLVM build proof. Requires clang.",
    )
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    print("FROG reference workspace checks")
    print("===============================")
    print(f"Repository root: {ROOT}")

    for stage in build_stages(
        include_pytest=args.include_pytest,
        include_llvm_build=args.include_llvm_build,
    ):
        code = run_stage(stage)
        if code != 0:
            return code

    print("\nReference workspace status: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
