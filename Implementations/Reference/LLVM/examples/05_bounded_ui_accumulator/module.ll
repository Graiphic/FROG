; FROG example 05 - first LLVM-native closure
; Emitted from the published Example 05 lowered kernel.
;
; Lowered kernel shape:
;   initial_state = 0
;   state_type = u16
;   iteration_count = 5
;   iteration_body = add state_current + input_value -> state_next
;   commit_rule = state_current <- state_next after each iteration

@fmt_state = private unnamed_addr constant [16 x i8] c"final_state=%d\0A\00"
@fmt_output = private unnamed_addr constant [18 x i8] c"public_output=%d\0A\00"
@fmt_status = private unnamed_addr constant [11 x i8] c"status=ok\0A\00"

declare i32 @printf(ptr, ...)
declare i32 @atoi(ptr)

define i16 @frog_example05_accumulate(i16 %input_value) {
entry:
  br label %loop

loop:
  %i = phi i32 [ 0, %entry ], [ %i_next, %loop_body ]
  %state_current = phi i16 [ 0, %entry ], [ %state_next, %loop_body ]
  %done = icmp uge i32 %i, 5
  br i1 %done, label %exit, label %loop_body

loop_body:
  %state_next = add i16 %state_current, %input_value
  %i_next = add i32 %i, 1
  br label %loop

exit:
  ret i16 %state_current
}

define i32 @main(i32 %argc, ptr %argv) {
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
  %result_i32 = zext i16 %result to i32

  %fmt_state_ptr = getelementptr inbounds [16 x i8], ptr @fmt_state, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_state_ptr, i32 %result_i32)

  %fmt_output_ptr = getelementptr inbounds [18 x i8], ptr @fmt_output, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_output_ptr, i32 %result_i32)

  %fmt_status_ptr = getelementptr inbounds [11 x i8], ptr @fmt_status, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ptr)

  ret i32 0
}
