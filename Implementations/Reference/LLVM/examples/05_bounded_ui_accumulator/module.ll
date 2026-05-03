; FROG example 05 - first LLVM-native closure
; Emitted from the published Example 05 lowered kernel.
;
; Lowered kernel shape:
;   initial_state = 0
;   state_type = u16
;   iteration_count = 5
;   iteration_body = add state_current + input_value -> state_next
;   commit_rule = state_current <- state_next after each iteration
;
; Native proof policy:
;   reject u16 overflow with status=error

@fmt_state = private unnamed_addr constant [16 x i8] c"final_state=%d\0A\00"
@fmt_output = private unnamed_addr constant [18 x i8] c"public_output=%d\0A\00"
@fmt_status_ok = private unnamed_addr constant [11 x i8] c"status=ok\0A\00"
@fmt_status_error = private unnamed_addr constant [14 x i8] c"status=error\0A\00"
@fmt_error_overflow = private unnamed_addr constant [50 x i8] c"error=final_state must remain in the u16 domain.\0A\00"

declare i32 @printf(ptr, ...)
declare i32 @atoi(ptr)

define i32 @frog_example05_accumulate_checked(i16 %input_value) {
entry:
  br label %loop

loop:
  %i = phi i32 [ 0, %entry ], [ %i_next, %loop_commit ]
  %state_current = phi i16 [ 0, %entry ], [ %state_next, %loop_commit ]
  %done = icmp uge i32 %i, 5
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
}

define i32 @main(i32 %argc, ptr %argv) {
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

  %fmt_error_overflow_ptr = getelementptr inbounds [50 x i8], ptr @fmt_error_overflow, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_error_overflow_ptr)

  ret i32 1
}
