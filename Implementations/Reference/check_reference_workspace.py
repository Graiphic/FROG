#!/usr/bin/env python3
"""Run the repository-visible FROG reference workspace checks."""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]


def run_stage(name: str, command: list[str]) -> int:
    print(f"\n== {name} ==")
    print("$ " + " ".join(command))
    result = subprocess.run(command, cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    if result.returncode != 0:
        print(f"Stage failed: {name} (exit {result.returncode})", file=sys.stderr)
    else:
        print(f"Stage ok: {name}")
    return result.returncode


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Run FROG reference workspace checks.")
    parser.add_argument("--include-pytest", action="store_true")
    parser.add_argument("--include-llvm-build", action="store_true")
    parser.add_argument("--include-native-kernel-bridge", action="store_true")
    parser.add_argument("--widget-layer-only", action="store_true")
    parser.add_argument("--skip-widget-validator", action="store_true")
    args = parser.parse_args(argv)

    py = sys.executable
    stages: list[tuple[str, list[str]]] = []

    if args.widget_layer_only:
        stages.append(("Widget layer validation", [py, "Implementations/Reference/WidgetValidator/validate_widget_layer.py"]))
    else:
        pipeline = [py, "Implementations/Reference/Pipeline/check_examples01_10_full.py"]
        if args.skip_widget_validator:
            pipeline.append("--skip-widget-validator")
        if args.include_llvm_build:
            pipeline.append("--include-llvm-build")
        stages.append(("Examples 01-10 full pipeline", pipeline))

        if args.include_native_kernel_bridge:
            stages.extend([
                (
                    "Example 05 native kernel bridge publication",
                    [py, "Implementations/Reference/Runtime/check_example05_native_kernel_bridge.py"],
                ),
                (
                    "Example 05 C++ LLVM-produced native kernel bridge",
                    [py, "Implementations/Reference/Runtime/check_example05_cpp_native_kernel_bridge.py"],
                ),
                (
                    "Example 06 native bool kernel bridge publication",
                    [py, "Implementations/Reference/Runtime/check_example06_native_kernel_bridge.py"],
                ),
                (
                    "Example 06 C++ LLVM-produced native bool kernel bridge",
                    [py, "Implementations/Reference/Runtime/check_example06_cpp_native_kernel_bridge.py"],
                ),
                (
                    "Example 11 C++ LLVM-produced native Button switch kernel bridge",
                    [py, "Implementations/Reference/Runtime/check_example11_cpp_native_kernel_bridge.py"],
                ),
                (
                    "Example 12 C++ LLVM-produced native Button switch-when-released kernel bridge",
                    [py, "Implementations/Reference/Runtime/check_example12_cpp_native_kernel_bridge.py"],
                ),
                (
                    "Python dynamic LLVM-produced native kernel bridge",
                    [py, "Implementations/Reference/Runtime/check_python_native_kernel_bridge.py"],
                ),
                (
                    "Rust dynamic LLVM-produced native kernel bridge",
                    [py, "Implementations/Reference/Runtime/check_rust_native_kernel_bridge.py"],
                ),
            ])

        if args.include_pytest:
            stages.extend([
                ("ArtifactChecks tests", [py, "-m", "pytest", "Implementations/Reference/ArtifactChecks/tests"]),
                ("Deriver tests", [py, "-m", "pytest", "Implementations/Reference/Deriver/tests"]),
                ("Lowerer tests", [py, "-m", "pytest", "Implementations/Reference/Lowerer/tests"]),
                ("ContractEmitter tests", [py, "-m", "pytest", "Implementations/Reference/ContractEmitter/tests"]),
                ("Runtime tests", [py, "-m", "pytest", "Implementations/Reference/Runtime/tests"]),
                ("Runtime Python tests", [py, "-m", "pytest", "Implementations/Reference/Runtime/python/tests"]),
                ("LLVM tests", [py, "-m", "pytest", "Implementations/Reference/LLVM/tests"]),
                ("Pipeline tests", [py, "-m", "pytest", "Implementations/Reference/Pipeline/tests"]),
            ])

    for name, command in stages:
        code = run_stage(name, command)
        if code != 0:
            return code

    print("\nReference workspace status: ok")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
