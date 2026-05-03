#!/usr/bin/env python3
"""Emit or check the Example 05 LLVM module from the published lowering artifact.

This tool is intentionally narrow. It is the first reproducible lowering -> LLVM
module step for the frozen Example 05 corridor. It does not implement a general
LLVM backend.

Run from the repository root:

    python Implementations/Reference/LLVM/tools/emit_llvm_module.py --check

or:

    python Implementations/Reference/LLVM/tools/emit_llvm_module.py \
      --lowering Examples/05_bounded_ui_accumulator/main.lowering.json \
      --output build/generated/module.ll
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path
from typing import Any
import json


ROOT = Path(__file__).resolve().parents[4]

DEFAULT_LOWERING = ROOT / "Examples" / "05_bounded_ui_accumulator" / "main.lowering.json"
DEFAULT_EXPECTED_MODULE = (
    ROOT
    / "Implementations"
    / "Reference"
    / "LLVM"
    / "examples"
    / "05_bounded_ui_accumulator"
    / "module.ll"
)
DEFAULT_EXAMPLE_DIR = DEFAULT_EXPECTED_MODULE.parent
DEFAULT_EXECUTABLE = DEFAULT_EXAMPLE_DIR / "bounded_ui_accumulator_llvm"


class LLVMEmissionError(RuntimeError):
    """Raised when the supported Example 05 LLVM module cannot be emitted."""


def load_json(path: Path) -> dict[str, Any]:
    try:
        data = json.loads(path.read_text(encoding="utf-8"))
    except FileNotFoundError as exc:
        raise LLVMEmissionError(f"missing file: {path}") from exc
    except json.JSONDecodeError as exc:
        raise LLVMEmissionError(f"invalid JSON in {path}: {exc}") from exc

    if not isinstance(data, dict):
        raise LLVMEmissionError(f"{path} must contain a JSON object")
    return data


def require_object(value: Any, name: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise LLVMEmissionError(f"{name} must be an object")
    return value


def require_list(value: Any, name: str) -> list[Any]:
    if not isinstance(value, list):
        raise LLVMEmissionError(f"{name} must be an array")
    return value


def single_lowered_unit(lowering: dict[str, Any]) -> dict[str, Any]:
    units = require_list(lowering.get("lowered_units"), "lowering.lowered_units")
    if len(units) != 1:
        raise LLVMEmissionError(f"Example 05 expects exactly one lowered unit, found {len(units)}")
    unit = require_object(units[0], "lowering.lowered_units[0]")
    if unit.get("unit_id") != "main":
        raise LLVMEmissionError("Example 05 lowered unit_id must be main")
    if unit.get("kind") != "bounded_accumulator_kernel_with_ui_bindings":
        raise LLVMEmissionError("unexpected Example 05 lowered unit kind")
    return unit


def kernel_from_lowering(lowering: dict[str, Any]) -> dict[str, Any]:
    if lowering.get("artifact_kind") != "frog_lowered_unit":
        raise LLVMEmissionError("input artifact_kind must be frog_lowered_unit")

    source_ref = require_object(lowering.get("source_ref"), "lowering.source_ref")
    if source_ref.get("example_id") != "05_bounded_ui_accumulator":
        raise LLVMEmissionError("this emitter currently supports only 05_bounded_ui_accumulator")

    unit = single_lowered_unit(lowering)
    kernel = require_object(unit.get("execution_kernel"), "lowered_unit.execution_kernel")

    state_type = kernel.get("state_type")
    if state_type != "u16":
        raise LLVMEmissionError("Example 05 LLVM emitter currently supports only u16 state")

    initial_state = kernel.get("initial_state")
    if initial_state != 0:
        raise LLVMEmissionError("Example 05 LLVM emitter expects initial_state = 0")

    iteration_count = kernel.get("iteration_count")
    if not isinstance(iteration_count, int) or iteration_count < 0:
        raise LLVMEmissionError("execution_kernel.iteration_count must be a non-negative integer")

    iteration_body = require_list(kernel.get("iteration_body"), "execution_kernel.iteration_body")
    if len(iteration_body) != 1:
        raise LLVMEmissionError("Example 05 LLVM emitter expects one iteration operation")
    op = require_object(iteration_body[0], "execution_kernel.iteration_body[0]")
    if op.get("op") != "add" or op.get("dst") != "state_next" or op.get("src") != ["state_current", "input_value"]:
        raise LLVMEmissionError("Example 05 LLVM emitter supports only state_next = state_current + input_value")

    final_publication = require_list(kernel.get("final_publication"), "execution_kernel.final_publication")
    expected_publications = [
        {"target": "public_output.result", "source": "state_current"},
        {"target": "widget.ind_result.value", "source": "state_current"},
    ]
    if final_publication != expected_publications:
        raise LLVMEmissionError("Example 05 final_publication does not match the expected kernel publication shape")

    return {
        "iteration_count": iteration_count,
        "state_type": state_type,
        "initial_state": initial_state,
    }


def emit_module_text(lowering: dict[str, Any]) -> str:
    kernel = kernel_from_lowering(lowering)
    iterations = kernel["iteration_count"]

    return f"""; FROG example 05 - first LLVM-native closure

@fmt_state = private unnamed_addr constant [16 x i8] c"final_state=%d\\0A\\00"
@fmt_output = private unnamed_addr constant [18 x i8] c"public_output=%d\\0A\\00"
@fmt_status = private unnamed_addr constant [11 x i8] c"status=ok\\0A\\00"

declare i32 @printf(ptr, ...)
declare i32 @atoi(ptr)

define i16 @frog_example05_accumulate(i16 %input_value) {{
entry:
  %mul = mul i16 %input_value, {iterations}
  ret i16 %mul
}}

define i32 @main(i32 %argc, ptr %argv) {{
entry:
  %has_arg = icmp sgt i32 %argc, 1
  br i1 %has_arg, label %parse_arg, label %use_default

parse_arg:
  %argv1ptr = getelementptr inbounds ptr, ptr %argv, i64 1
  %argv1 = load ptr, ptr %argv1ptr, align 8
  %parsed = call i32 @atoi(ptr %argv1)
  %trunc = trunc i32 %parsed to i16
  br label %run

use_default:
  br label %run

run:
  %input_value = phi i16 [ %trunc, %parse_arg ], [ 3, %use_default ]
  %result = call i16 @frog_example05_accumulate(i16 %input_value)
  %result_i32 = sext i16 %result to i32

  %fmt_state_ptr = getelementptr inbounds [16 x i8], ptr @fmt_state, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_state_ptr, i32 %result_i32)

  %fmt_output_ptr = getelementptr inbounds [18 x i8], ptr @fmt_output, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_output_ptr, i32 %result_i32)

  %fmt_status_ptr = getelementptr inbounds [11 x i8], ptr @fmt_status, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ptr)

  ret i32 0
}}
"""


def normalize_text(text: str) -> str:
    return text.replace("\r\n", "\n").rstrip() + "\n"


def write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(normalize_text(text), encoding="utf-8", newline="\n")


def check_expected_module(generated: str, expected_path: Path) -> bool:
    expected = expected_path.read_text(encoding="utf-8")
    return normalize_text(generated) == normalize_text(expected)


def run_build(example_dir: Path) -> int:
    build_script = example_dir / "build.sh"
    if not build_script.is_file():
        raise LLVMEmissionError(f"missing build script: {build_script}")

    result = subprocess.run(
        ["bash", str(build_script)],
        cwd=example_dir,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    return result.returncode


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Emit or check Example 05 LLVM module from lowering.")
    parser.add_argument("--lowering", type=Path, default=DEFAULT_LOWERING)
    parser.add_argument("--output", type=Path, default=None, help="Optional path where generated module.ll should be written.")
    parser.add_argument("--expected", type=Path, default=DEFAULT_EXPECTED_MODULE)
    parser.add_argument("--check", action="store_true", help="Compare emitted LLVM module with the published module.ll.")
    parser.add_argument("--print", action="store_true", dest="print_module", help="Print emitted LLVM module to stdout.")
    parser.add_argument("--build", action="store_true", help="After check/write, run the example build.sh script.")
    return parser.parse_args(argv)


def main(argv: list[str]) -> int:
    args = parse_args(argv)

    lowering_path = args.lowering if args.lowering.is_absolute() else ROOT / args.lowering
    expected_path = args.expected if args.expected.is_absolute() else ROOT / args.expected
    output_path = args.output if args.output is None or args.output.is_absolute() else ROOT / args.output

    try:
        lowering = load_json(lowering_path)
        generated = emit_module_text(lowering)

        if args.output:
            write_text(output_path, generated)
            print(f"wrote LLVM module: {output_path}")

        if args.print_module:
            sys.stdout.write(normalize_text(generated))

        if args.check:
            if not check_expected_module(generated, expected_path):
                print("LLVM module emission check: FAILED", file=sys.stderr)
                print(f"lowering: {lowering_path}", file=sys.stderr)
                print(f"expected: {expected_path}", file=sys.stderr)
                return 1
            print("LLVM module emission check: ok")
            print(f"lowering: {lowering_path.relative_to(ROOT)}")
            print(f"expected: {expected_path.relative_to(ROOT)}")

        if args.build:
            code = run_build(DEFAULT_EXAMPLE_DIR)
            if code != 0:
                print(f"LLVM build check: FAILED (exit {code})", file=sys.stderr)
                return code
            print("LLVM build check: ok")

        return 0

    except LLVMEmissionError as exc:
        print(f"LLVM module emission error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
