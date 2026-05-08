"""Generic-by-kind LLVM emitter for the non-normative FROG reference workspace.

This module is intentionally narrow and non-normative. It does not implement a
general production LLVM backend. It consolidates the current Examples 01-05
native proof emitters around one dispatch key:

    lowered_units[0].kind

Supported lowered unit kinds:

- pure_addition_kernel
- ui_value_roundtrip_kernel
- ui_property_write_effect_unit
- stateful_feedback_delay_kernel
- bounded_accumulator_kernel_with_ui_bindings
"""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any, Callable


OVERFLOW_ERROR = "final_state must remain in the u16 domain."


class LLVMEmissionError(RuntimeError):
    """Raised when a supported lowering cannot be emitted as LLVM."""


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


def normalize_text(text: str) -> str:
    return text.replace("\r\n", "\n").rstrip() + "\n"


def write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(normalize_text(text), encoding="utf-8", newline="\n")


def c_len(text: str) -> int:
    return len(text.encode("utf-8")) + 1


def c_const(name: str, text: str) -> str:
    escaped = text.replace("\\", "\\5C").replace("\n", "\\0A").replace('"', '\\"')
    return f'@{name} = private unnamed_addr constant [{c_len(text)} x i8] c"{escaped}\\00"'


def require_object(value: Any, name: str) -> dict[str, Any]:
    if not isinstance(value, dict):
        raise LLVMEmissionError(f"{name} must be an object")
    return value


def require_list(value: Any, name: str) -> list[Any]:
    if not isinstance(value, list):
        raise LLVMEmissionError(f"{name} must be an array")
    return value


def expect_equal(value: Any, expected: Any, message: str) -> None:
    if value != expected:
        raise LLVMEmissionError(message)


def single_lowered_unit(lowering: dict[str, Any]) -> dict[str, Any]:
    if lowering.get("artifact_kind") != "frog_lowered_unit":
        raise LLVMEmissionError("input artifact_kind must be frog_lowered_unit")
    units = require_list(lowering.get("lowered_units"), "lowering.lowered_units")
    if len(units) != 1:
        raise LLVMEmissionError(f"expected exactly one lowered unit, found {len(units)}")
    return require_object(units[0], "lowering.lowered_units[0]")


def unit_kind(lowering: dict[str, Any]) -> str:
    unit = single_lowered_unit(lowering)
    kind = unit.get("kind")
    if not isinstance(kind, str):
        raise LLVMEmissionError("lowered unit kind must be a string")
    return kind


def execution_kernel(unit: dict[str, Any]) -> dict[str, Any]:
    return require_object(unit.get("execution_kernel"), "unit.execution_kernel")


def single_operation(unit: dict[str, Any]) -> dict[str, Any]:
    kernel = execution_kernel(unit)
    operations = require_list(kernel.get("operations"), "unit.execution_kernel.operations")
    if len(operations) != 1:
        raise LLVMEmissionError("expected exactly one execution operation")
    return require_object(operations[0], "unit.execution_kernel.operations[0]")


def single_effect(unit: dict[str, Any]) -> dict[str, Any]:
    effects = require_list(unit.get("execution_effects"), "unit.execution_effects")
    if len(effects) != 1:
        raise LLVMEmissionError("ui_property_write_effect_unit supports exactly one effect")
    return require_object(effects[0], "unit.execution_effects[0]")


def single_step_operation(kernel: dict[str, Any]) -> dict[str, Any]:
    step_body = require_list(kernel.get("step_body"), "unit.execution_kernel.step_body")
    if len(step_body) != 1:
        raise LLVMEmissionError("stateful_feedback_delay_kernel supports exactly one step operation")
    return require_object(step_body[0], "unit.execution_kernel.step_body[0]")


def single_iteration_operation(kernel: dict[str, Any]) -> dict[str, Any]:
    iteration_body = require_list(kernel.get("iteration_body"), "unit.execution_kernel.iteration_body")
    if len(iteration_body) != 1:
        raise LLVMEmissionError("bounded accumulator supports exactly one iteration operation")
    return require_object(iteration_body[0], "unit.execution_kernel.iteration_body[0]")


def emit_pure_addition(lowering: dict[str, Any]) -> str:
    unit = single_lowered_unit(lowering)
    op = single_operation(unit)
    expect_equal(op, {"op": "add", "dst": "result", "type": "f64", "src": ["a", "b"]}, "pure_addition_kernel supports only f64 result = a + b")

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


def emit_ui_value_roundtrip(lowering: dict[str, Any]) -> str:
    unit = single_lowered_unit(lowering)
    op = single_operation(unit)
    if op.get("op") != "add" or op.get("type") != "f64" or op.get("src") != ["widget.ctrl_a.value", "widget.ctrl_b.value"]:
        raise LLVMEmissionError("ui_value_roundtrip_kernel supports only f64 widget ctrl_a + ctrl_b")

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


def emit_ui_property_write(lowering: dict[str, Any]) -> str:
    unit = single_lowered_unit(lowering)
    effect = single_effect(unit)
    if effect.get("op") != "frog.ui.property_write" or effect.get("widget_id") != "ctrl_gain" or effect.get("member") != "label.text":
        raise LLVMEmissionError("ui_property_write_effect_unit supports only ctrl_gain.label.text property_write")

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


def emit_stateful_feedback_delay(lowering: dict[str, Any]) -> str:
    unit = single_lowered_unit(lowering)
    kernel = execution_kernel(unit)
    if kernel.get("initial_state") != 0.0 or kernel.get("state_type") != "f64":
        raise LLVMEmissionError("stateful_feedback_delay_kernel expects f64 initial state 0.0")
    op = single_step_operation(kernel)
    if op.get("op") != "add" or op.get("src") != ["state_current", "x"]:
        raise LLVMEmissionError("stateful_feedback_delay_kernel supports only state_current + x")

    state_id = kernel.get("state_id", "delay_1")
    if state_id != "delay_1":
        raise LLVMEmissionError("current stateful_feedback_delay LLVM proof supports only delay_1")

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


def emit_bounded_accumulator(lowering: dict[str, Any]) -> str:
    unit = single_lowered_unit(lowering)
    kernel = execution_kernel(unit)

    if kernel.get("state_type") != "u16":
        raise LLVMEmissionError("bounded_accumulator_kernel_with_ui_bindings supports only u16 state")
    if kernel.get("initial_state") != 0:
        raise LLVMEmissionError("bounded_accumulator_kernel_with_ui_bindings expects initial_state = 0")

    iterations = kernel.get("iteration_count")
    if not isinstance(iterations, int) or iterations < 0:
        raise LLVMEmissionError("iteration_count must be a non-negative integer")

    op = single_iteration_operation(kernel)
    if op.get("op") != "add" or op.get("dst") != "state_next" or op.get("src") != ["state_current", "input_value"]:
        raise LLVMEmissionError("bounded accumulator supports only state_next = state_current + input_value")

    final_publication = require_list(kernel.get("final_publication"), "unit.execution_kernel.final_publication")
    expected_publications = [
        {"target": "public_output.result", "source": "state_current"},
        {"target": "widget.ind_result.value", "source": "state_current"},
    ]
    if final_publication != expected_publications:
        raise LLVMEmissionError("bounded accumulator final_publication shape mismatch")

    overflow_literal = f"error={OVERFLOW_ERROR}"
    overflow_global_len = len(overflow_literal) + 2

    return f"""; FROG example 05 - first LLVM-native closure
; Emitted from the published Example 05 lowered kernel.
;
; Lowered kernel shape:
;   initial_state = 0
;   state_type = u16
;   iteration_count = {iterations}
;   iteration_body = add state_current + input_value -> state_next
;   commit_rule = state_current <- state_next after each iteration
;
; Native proof policy:
;   reject u16 overflow with status=error

@fmt_state = private unnamed_addr constant [16 x i8] c"final_state=%d\\0A\\00"
@fmt_output = private unnamed_addr constant [18 x i8] c"public_output=%d\\0A\\00"
@fmt_status_ok = private unnamed_addr constant [11 x i8] c"status=ok\\0A\\00"
@fmt_status_error = private unnamed_addr constant [14 x i8] c"status=error\\0A\\00"
@fmt_error_overflow = private unnamed_addr constant [{overflow_global_len} x i8] c"error={OVERFLOW_ERROR}\\0A\\00"

declare i32 @printf(ptr, ...)
declare i32 @atoi(ptr)

define i32 @frog_example05_accumulate_checked(i16 %input_value) {{
entry:
  br label %loop

loop:
  %i = phi i32 [ 0, %entry ], [ %i_next, %loop_commit ]
  %state_current = phi i16 [ 0, %entry ], [ %state_next, %loop_commit ]
  %done = icmp uge i32 %i, {iterations}
  br i1 %done, label %exit_ok, label %loop_body

loop_body:
  %state_i32 = zext i16 %state_current to i32
  %input_i32 = zext i16 %input_value to i32
  %sum_i32 = add i32 %state_i32, %input_i32
  %overflow = icmp ugt i32 %sum_i32, 65535
  br i1 %overflow, label %exit_overflow, label %loop_commit

loop_commit:
  %state_next = trunc i32 %sum_i32 to i16
  %i_next = add i32 %i, 1
  br label %loop

exit_ok:
  %result_i32 = zext i16 %state_current to i32
  ret i32 %result_i32

exit_overflow:
  ret i32 -1
}}

define i32 @main(i32 %argc, ptr %argv) {{
entry:
  %has_arg = icmp sgt i32 %argc, 1
  br i1 %has_arg, label %parse_arg, label %use_default

parse_arg:
  %argv1ptr = getelementptr inbounds ptr, ptr %argv, i64 1
  %argv1 = load ptr, ptr %argv1ptr, align 8
  %parsed = call i32 @atoi(ptr %argv1)
  %input_negative = icmp slt i32 %parsed, 0
  %input_too_large = icmp sgt i32 %parsed, 65535
  %invalid_input = or i1 %input_negative, %input_too_large
  br i1 %invalid_input, label %print_error, label %run_parsed

run_parsed:
  %trunc = trunc i32 %parsed to i16
  br label %run

use_default:
  br label %run

run:
  %input_value = phi i16 [ %trunc, %run_parsed ], [ 3, %use_default ]
  %result = call i32 @frog_example05_accumulate_checked(i16 %input_value)
  %has_overflow = icmp slt i32 %result, 0
  br i1 %has_overflow, label %print_error, label %print_ok

print_ok:
  %fmt_state_ptr = getelementptr inbounds [16 x i8], ptr @fmt_state, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_state_ptr, i32 %result)

  %fmt_output_ptr = getelementptr inbounds [18 x i8], ptr @fmt_output, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_output_ptr, i32 %result)

  %fmt_status_ok_ptr = getelementptr inbounds [11 x i8], ptr @fmt_status_ok, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ok_ptr)

  ret i32 0

print_error:
  %fmt_status_error_ptr = getelementptr inbounds [14 x i8], ptr @fmt_status_error, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_error_ptr)

  %fmt_error_overflow_ptr = getelementptr inbounds [{overflow_global_len} x i8], ptr @fmt_error_overflow, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_error_overflow_ptr)

  ret i32 1
}}
"""


EMITTERS_BY_KIND: dict[str, Callable[[dict[str, Any]], str]] = {
    "pure_addition_kernel": emit_pure_addition,
    "ui_value_roundtrip_kernel": emit_ui_value_roundtrip,
    "ui_property_write_effect_unit": emit_ui_property_write,
    "stateful_feedback_delay_kernel": emit_stateful_feedback_delay,
    "bounded_accumulator_kernel_with_ui_bindings": emit_bounded_accumulator,
}


def emit_llvm_from_lowering(lowering: dict[str, Any]) -> str:
    kind = unit_kind(lowering)
    emitter = EMITTERS_BY_KIND.get(kind)
    if emitter is None:
        raise LLVMEmissionError(f"unsupported lowered unit kind for LLVM emission: {kind!r}")
    return normalize_text(emitter(lowering))


def emit_llvm_from_lowering_path(path: Path) -> str:
    return emit_llvm_from_lowering(load_json(path))
