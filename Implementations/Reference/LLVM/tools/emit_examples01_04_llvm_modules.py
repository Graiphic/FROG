#!/usr/bin/env python3
"""Emit/check LLVM modules for Examples 01-04 from their published lowerings.

This tool is intentionally narrow and non-normative. It gives Examples 01-04
the same important property as Example 05 at native-proof level:

    main.lowering.json -> module.ll

It does not implement a general LLVM backend.
"""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[4]

EXAMPLES: dict[str, dict[str, str]] = {
    "01": {
        "example_id": "01_pure_addition",
        "lowering": "Examples/01_pure_addition/main.lowering.json",
        "module": "Implementations/Reference/LLVM/examples/01_pure_addition/module.ll",
        "example_dir": "Implementations/Reference/LLVM/examples/01_pure_addition",
    },
    "02": {
        "example_id": "02_ui_value_roundtrip",
        "lowering": "Examples/02_ui_value_roundtrip/main.lowering.json",
        "module": "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip/module.ll",
        "example_dir": "Implementations/Reference/LLVM/examples/02_ui_value_roundtrip",
    },
    "03": {
        "example_id": "03_ui_property_write",
        "lowering": "Examples/03_ui_property_write/main.lowering.json",
        "module": "Implementations/Reference/LLVM/examples/03_ui_property_write/module.ll",
        "example_dir": "Implementations/Reference/LLVM/examples/03_ui_property_write",
    },
    "04": {
        "example_id": "04_stateful_feedback_delay",
        "lowering": "Examples/04_stateful_feedback_delay/main.lowering.json",
        "module": "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay/module.ll",
        "example_dir": "Implementations/Reference/LLVM/examples/04_stateful_feedback_delay",
    },
}


class LLVMEmissionError(RuntimeError):
    pass


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


def normalize(text: str) -> str:
    return text.replace("\r\n", "\n").rstrip() + "\n"


def c_len(text: str) -> int:
    return len(text.encode("utf-8")) + 1


def c_const(name: str, text: str) -> str:
    escaped = text.replace("\\", "\\5C").replace("\n", "\\0A").replace('"', '\\"')
    return f'@{name} = private unnamed_addr constant [{c_len(text)} x i8] c"{escaped}\\00"'


def single_unit(lowering: dict[str, Any]) -> dict[str, Any]:
    units = lowering.get("lowered_units")
    if not isinstance(units, list) or len(units) != 1 or not isinstance(units[0], dict):
        raise LLVMEmissionError("lowering must contain exactly one lowered unit")
    return units[0]


def emit_example01(lowering: dict[str, Any]) -> str:
    unit = single_unit(lowering)
    if unit.get("kind") != "pure_addition_kernel":
        raise LLVMEmissionError("Example 01 expects pure_addition_kernel")
    op = unit["execution_kernel"]["operations"][0]
    if op != {"op": "add", "dst": "result", "type": "f64", "src": ["a", "b"]}:
        raise LLVMEmissionError("Example 01 lowering operation is not the supported f64 add shape")

    fmt_result = "result=%f\n"
    fmt_status = "status=ok\n"
    return f"""; FROG example 01 - LLVM-native pure addition proof

{c_const("fmt_result", fmt_result)}
{c_const("fmt_status", fmt_status)}

declare i32 @printf(ptr, ...)
declare double @atof(ptr)

define double @frog_example01_add(double %a, double %b) {{
entry:
  %result = fadd double %a, %b
  ret double %result
}}

define i32 @main(i32 %argc, ptr %argv) {{
entry:
  %has_args = icmp sgt i32 %argc, 2
  br i1 %has_args, label %parse_args, label %use_default

parse_args:
  %argv1ptr = getelementptr inbounds ptr, ptr %argv, i64 1
  %argv1 = load ptr, ptr %argv1ptr, align 8
  %a = call double @atof(ptr %argv1)
  %argv2ptr = getelementptr inbounds ptr, ptr %argv, i64 2
  %argv2 = load ptr, ptr %argv2ptr, align 8
  %b = call double @atof(ptr %argv2)
  br label %run

use_default:
  br label %run

run:
  %a_value = phi double [ %a, %parse_args ], [ 2.250000e+00, %use_default ]
  %b_value = phi double [ %b, %parse_args ], [ 3.750000e+00, %use_default ]
  %result = call double @frog_example01_add(double %a_value, double %b_value)

  %fmt_result_ptr = getelementptr inbounds [{c_len(fmt_result)} x i8], ptr @fmt_result, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_result_ptr, double %result)

  %fmt_status_ptr = getelementptr inbounds [{c_len(fmt_status)} x i8], ptr @fmt_status, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ptr)

  ret i32 0
}}
"""


def emit_example02(lowering: dict[str, Any]) -> str:
    unit = single_unit(lowering)
    if unit.get("kind") != "ui_value_roundtrip_kernel":
        raise LLVMEmissionError("Example 02 expects ui_value_roundtrip_kernel")
    op = unit["execution_kernel"]["operations"][0]
    if op.get("op") != "add" or op.get("src") != ["widget.ctrl_a.value", "widget.ctrl_b.value"]:
        raise LLVMEmissionError("Example 02 lowering operation is not the supported widget-value add shape")

    fmt_result = "widget.ind_result.value=%f\n"
    fmt_status = "status=ok\n"
    return f"""; FROG example 02 - LLVM-native widget value arithmetic proof

{c_const("fmt_result", fmt_result)}
{c_const("fmt_status", fmt_status)}

declare i32 @printf(ptr, ...)
declare double @atof(ptr)

define double @frog_example02_widget_add(double %ctrl_a, double %ctrl_b) {{
entry:
  %result = fadd double %ctrl_a, %ctrl_b
  ret double %result
}}

define i32 @main(i32 %argc, ptr %argv) {{
entry:
  %has_args = icmp sgt i32 %argc, 2
  br i1 %has_args, label %parse_args, label %use_default

parse_args:
  %argv1ptr = getelementptr inbounds ptr, ptr %argv, i64 1
  %argv1 = load ptr, ptr %argv1ptr, align 8
  %ctrl_a = call double @atof(ptr %argv1)
  %argv2ptr = getelementptr inbounds ptr, ptr %argv, i64 2
  %argv2 = load ptr, ptr %argv2ptr, align 8
  %ctrl_b = call double @atof(ptr %argv2)
  br label %run

use_default:
  br label %run

run:
  %ctrl_a_value = phi double [ %ctrl_a, %parse_args ], [ 1.000000e+00, %use_default ]
  %ctrl_b_value = phi double [ %ctrl_b, %parse_args ], [ 2.000000e+00, %use_default ]
  %result = call double @frog_example02_widget_add(double %ctrl_a_value, double %ctrl_b_value)

  %fmt_result_ptr = getelementptr inbounds [{c_len(fmt_result)} x i8], ptr @fmt_result, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_result_ptr, double %result)

  %fmt_status_ptr = getelementptr inbounds [{c_len(fmt_status)} x i8], ptr @fmt_status, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ptr)

  ret i32 0
}}
"""


def emit_example03(lowering: dict[str, Any]) -> str:
    unit = single_unit(lowering)
    if unit.get("kind") != "ui_property_write_effect_unit":
        raise LLVMEmissionError("Example 03 expects ui_property_write_effect_unit")
    effect = unit["execution_effects"][0]
    if effect.get("op") != "frog.ui.property_write" or effect.get("member") != "label.text":
        raise LLVMEmissionError("Example 03 lowering effect is not the supported label.text property write shape")

    default_value = "Ready"
    fmt_widget = "widget=ctrl_gain\n"
    fmt_member = "member=label.text\n"
    fmt_value = "value=%s\n"
    fmt_status = "status=ok\n"
    return f"""; FROG example 03 - LLVM-native UI property effect proof

{c_const("default_value", default_value)}
{c_const("fmt_widget", fmt_widget)}
{c_const("fmt_member", fmt_member)}
{c_const("fmt_value", fmt_value)}
{c_const("fmt_status", fmt_status)}

declare i32 @printf(ptr, ...)

define i32 @main(i32 %argc, ptr %argv) {{
entry:
  %has_arg = icmp sgt i32 %argc, 1
  br i1 %has_arg, label %parse_arg, label %use_default

parse_arg:
  %argv1ptr = getelementptr inbounds ptr, ptr %argv, i64 1
  %value_arg = load ptr, ptr %argv1ptr, align 8
  br label %run

use_default:
  %default_ptr = getelementptr inbounds [{c_len(default_value)} x i8], ptr @default_value, i64 0, i64 0
  br label %run

run:
  %value = phi ptr [ %value_arg, %parse_arg ], [ %default_ptr, %use_default ]

  %fmt_widget_ptr = getelementptr inbounds [{c_len(fmt_widget)} x i8], ptr @fmt_widget, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_widget_ptr)

  %fmt_member_ptr = getelementptr inbounds [{c_len(fmt_member)} x i8], ptr @fmt_member, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_member_ptr)

  %fmt_value_ptr = getelementptr inbounds [{c_len(fmt_value)} x i8], ptr @fmt_value, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_value_ptr, ptr %value)

  %fmt_status_ptr = getelementptr inbounds [{c_len(fmt_status)} x i8], ptr @fmt_status, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ptr)

  ret i32 0
}}
"""


def emit_example04(lowering: dict[str, Any]) -> str:
    unit = single_unit(lowering)
    if unit.get("kind") != "stateful_feedback_delay_kernel":
        raise LLVMEmissionError("Example 04 expects stateful_feedback_delay_kernel")
    kernel = unit["execution_kernel"]
    if kernel.get("initial_state") != 0.0 or kernel.get("state_type") != "f64":
        raise LLVMEmissionError("Example 04 expects f64 initial state 0.0")

    fmt_y = "public_output.y=%f\n"
    fmt_state = "state.delay_1=%f\n"
    fmt_status = "status=ok\n"
    return f"""; FROG example 04 - LLVM-native explicit delay step proof

{c_const("fmt_y", fmt_y)}
{c_const("fmt_state", fmt_state)}
{c_const("fmt_status", fmt_status)}

declare i32 @printf(ptr, ...)
declare double @atof(ptr)

define double @frog_example04_step(double %state_current, double %x) {{
entry:
  %state_next = fadd double %state_current, %x
  ret double %state_next
}}

define i32 @main(i32 %argc, ptr %argv) {{
entry:
  %has_arg = icmp sgt i32 %argc, 1
  br i1 %has_arg, label %parse_arg, label %use_default

parse_arg:
  %argv1ptr = getelementptr inbounds ptr, ptr %argv, i64 1
  %argv1 = load ptr, ptr %argv1ptr, align 8
  %x = call double @atof(ptr %argv1)
  br label %run

use_default:
  br label %run

run:
  %x_value = phi double [ %x, %parse_arg ], [ 2.500000e+00, %use_default ]
  %state_next = call double @frog_example04_step(double 0.000000e+00, double %x_value)

  %fmt_y_ptr = getelementptr inbounds [{c_len(fmt_y)} x i8], ptr @fmt_y, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_y_ptr, double %state_next)

  %fmt_state_ptr = getelementptr inbounds [{c_len(fmt_state)} x i8], ptr @fmt_state, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_state_ptr, double %state_next)

  %fmt_status_ptr = getelementptr inbounds [{c_len(fmt_status)} x i8], ptr @fmt_status, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ptr)

  ret i32 0
}}
"""


EMITTERS = {
    "01": emit_example01,
    "02": emit_example02,
    "03": emit_example03,
    "04": emit_example04,
}


def emit_module(key: str) -> str:
    spec = EXAMPLES[key]
    lowering = load_json(ROOT / spec["lowering"])
    example_id = lowering.get("source_ref", {}).get("example_id")
    if example_id != spec["example_id"]:
        raise LLVMEmissionError(f"Example {key} lowering has unexpected example_id {example_id!r}")
    return normalize(EMITTERS[key](lowering))


def check_module(key: str) -> None:
    spec = EXAMPLES[key]
    expected_path = ROOT / spec["module"]
    expected = normalize(expected_path.read_text(encoding="utf-8"))
    generated = emit_module(key)
    if generated != expected:
        raise LLVMEmissionError(f"LLVM module emission mismatch for Example {key}")
    print(f"LLVM module emission check ok: Example {key}")


def write_module(key: str, output: Path | None) -> None:
    generated = emit_module(key)
    out_path = output if output is not None else ROOT / EXAMPLES[key]["module"]
    out_path.parent.mkdir(parents=True, exist_ok=True)
    out_path.write_text(generated, encoding="utf-8", newline="\n")
    print(out_path)


def build_module(key: str) -> None:
    spec = EXAMPLES[key]
    example_dir = ROOT / spec["example_dir"]
    script = example_dir / "build.sh"
    result = subprocess.run(["bash", str(script)], cwd=example_dir, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, check=False)
    if result.stdout:
        print(result.stdout.rstrip())
    if result.stderr:
        print(result.stderr.rstrip(), file=sys.stderr)
    if result.returncode != 0:
        raise LLVMEmissionError(f"LLVM build failed for Example {key}")
    print(f"LLVM build ok: Example {key}")


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description="Emit/check LLVM modules for Examples 01-04.")
    parser.add_argument("--example", choices=sorted(EXAMPLES), default=None)
    parser.add_argument("--output", type=Path, default=None)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--build", action="store_true")
    args = parser.parse_args(argv)

    keys = [args.example] if args.example else sorted(EXAMPLES)

    try:
        if args.output is not None and args.example is None:
            raise LLVMEmissionError("--output requires --example")
        if args.check:
            for key in keys:
                check_module(key)
        elif not args.build:
            for key in keys:
                write_module(key, args.output)

        if args.build:
            for key in keys:
                build_module(key)

        return 0
    except LLVMEmissionError as exc:
        print(f"LLVM examples01-04 emission error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
