#!/usr/bin/env python3
"""Run the repository-visible FROG reference workspace checks."""

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
    result = subprocess.run(stage.command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    if result.returncode != 0:
        print(f"Stage failed: {stage.name} (exit {result.returncode})", file=sys.stderr)
    else:
        print(f"Stage ok: {stage.name}")
    return result.returncode


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Run FROG reference workspace checks.")
    parser.add_argument("--include-pytest", action="store_true")
    parser.add_argument("--include-llvm-build", action="store_true")
    parser.add_argument(
        "--widget-layer-only",
        action="store_true",
        help="Run only the non-normative widget-layer repository hygiene validator.",
    )
    parser.add_argument(
        "--skip-widget-validator",
        action="store_true",
        help="Skip widget-layer validation inside the Examples 01-05 pipeline.",
    )
    args = parser.parse_args(argv)

    py = sys.executable

    if args.widget_layer_only:
        stages = [
            Stage(
                "Widget layer validation",
                [py, "Implementations/Reference/WidgetValidator/validate_widget_layer.py"],
            ),
        ]
    else:
        stages = [
            Stage(
                "Examples 01-05 full pipeline",
                [
                    py,
                    "Implementations/Reference/Pipeline/check_examples01_05_full.py",
                ]
                + (["--skip-widget-validator"] if args.skip_widget_validator else [])
                + (["--include-llvm-build"] if args.include_llvm_build else []),
            ),
        ]

        if args.include_pytest:
            stages.extend([
                Stage("ArtifactChecks tests", [py, "-m", "pytest", "Implementations/Reference/ArtifactChecks/tests"]),
                Stage("Deriver tests", [py, "-m", "pytest", "Implementations/Reference/Deriver/tests"]),
                Stage("Lowerer tests", [py, "-m", "pytest", "Implementations/Reference/Lowerer/tests"]),
                Stage("ContractEmitter tests", [py, "-m", "pytest", "Implementations/Reference/ContractEmitter/tests"]),
                Stage("Runtime tests", [py, "-m", "pytest", "Implementations/Reference/Runtime/tests"]),
                Stage("LLVM tests", [py, "-m", "pytest", "Implementations/Reference/LLVM/tests"]),
                Stage("Pipeline tests", [py, "-m", "pytest", "Implementations/Reference/Pipeline/tests"]),
            ])

    for stage in stages:
        code = run_stage(stage)
        if code != 0:
            return code

    print("\nReference workspace status: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
