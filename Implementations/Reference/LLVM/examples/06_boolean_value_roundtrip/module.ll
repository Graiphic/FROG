; FROG example 06 - LLVM Boolean value roundtrip proof
; Emitted from the published Example 06 lowered Boolean copy kernel.
;
; Lowered kernel shape:
;   input_value : bool
;   result      : bool
;   operation   : copy input_value -> result

@text_true = private unnamed_addr constant [5 x i8] c"true\00"
@text_false = private unnamed_addr constant [6 x i8] c"false\00"
@fmt_input = private unnamed_addr constant [16 x i8] c"input_value=%s\0A\00"
@fmt_output = private unnamed_addr constant [18 x i8] c"public_output=%s\0A\00"
@fmt_status_ok = private unnamed_addr constant [11 x i8] c"status=ok\0A\00"

declare i32 @printf(ptr, ...)
declare i32 @atoi(ptr)

define i1 @frog_example06_copy_bool(i1 %input_value) {
entry:
  ret i1 %input_value
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
  %input_value = phi i1 [ %parsed_bool, %parse_arg ], [ true, %use_default ]
  %result = call i1 @frog_example06_copy_bool(i1 %input_value)

  %input_text = select i1 %input_value, ptr %true_ptr, ptr %false_ptr
  %result_text = select i1 %result, ptr %true_ptr, ptr %false_ptr

  %fmt_input_ptr = getelementptr inbounds [16 x i8], ptr @fmt_input, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_input_ptr, ptr %input_text)

  %fmt_output_ptr = getelementptr inbounds [18 x i8], ptr @fmt_output, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_output_ptr, ptr %result_text)

  %fmt_status_ok_ptr = getelementptr inbounds [11 x i8], ptr @fmt_status_ok, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ok_ptr)

  ret i32 0
}
