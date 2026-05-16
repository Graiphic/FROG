; FROG example 14 - LLVM Button latch_when_released Boolean proof
; Emitted from the published Example 14 lowered Button value copy kernel.
;
; Lowered kernel shape:
;   trigger_value : bool
;   latched       : bool
;   operation     : copy trigger_value -> latched

@text_true = private unnamed_addr constant [5 x i8] c"true\00"
@text_false = private unnamed_addr constant [6 x i8] c"false\00"
@fmt_input = private unnamed_addr constant [18 x i8] c"trigger_value=%s\0A\00"
@fmt_output = private unnamed_addr constant [19 x i8] c"public_latched=%s\0A\00"
@fmt_status_ok = private unnamed_addr constant [11 x i8] c"status=ok\0A\00"

declare i32 @printf(ptr, ...)
declare i32 @atoi(ptr)

define i1 @frog_example14_copy_button_value(i1 %trigger_value) {
entry:
  ret i1 %trigger_value
}

define i32 @main(i32 %argc, ptr %argv) {
entry:
  %true_ptr = getelementptr inbounds [5 x i8], ptr @text_true, i64 0, i64 0
  %false_ptr = getelementptr inbounds [6 x i8], ptr @text_false, i64 0, i64 0
  %has_arg = icmp sgt i32 %argc, 1
  br i1 %has_arg, label %parse_arg, label %use_default

parse_arg:
  %argv1ptr = getelementptr inbounds ptr, ptr %argv, i64 1
  %argv1 = load ptr, ptr %argv1ptr, align 8
  %parsed = call i32 @atoi(ptr %argv1)
  %parsed_bool = icmp ne i32 %parsed, 0
  br label %run

use_default:
  br label %run

run:
  %trigger_value = phi i1 [ %parsed_bool, %parse_arg ], [ false, %use_default ]
  %latched = call i1 @frog_example14_copy_button_value(i1 %trigger_value)

  %input_text = select i1 %trigger_value, ptr %true_ptr, ptr %false_ptr
  %result_text = select i1 %latched, ptr %true_ptr, ptr %false_ptr

  %fmt_input_ptr = getelementptr inbounds [18 x i8], ptr @fmt_input, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_input_ptr, ptr %input_text)

  %fmt_output_ptr = getelementptr inbounds [19 x i8], ptr @fmt_output, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_output_ptr, ptr %result_text)

  %fmt_status_ok_ptr = getelementptr inbounds [11 x i8], ptr @fmt_status_ok, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ok_ptr)

  ret i32 0
}
