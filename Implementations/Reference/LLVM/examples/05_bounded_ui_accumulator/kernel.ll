; FROG Example 05 - native kernel ABI proof artifact
; This module is intended for the compiler-agnostic runtime kernel bridge.
; It exposes a manifest-declared C-compatible entry surface:
;
;   void frog_example05_run(uint16_t input_value, FrogRunResult* out_result)
;
; The runtime consumes this through the native kernel manifest. The runtime does
; not depend on LLVM as its conceptual execution authority.

%FrogRunResult = type { i8, i16, i16 }

define void @frog_example05_run(i16 %input_value, ptr %out_result) {
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
  %ok_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 0
  store i8 1, ptr %ok_ptr, align 2
  %result_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 1
  store i16 %state_current, ptr %result_ptr, align 2
  %error_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 2
  store i16 0, ptr %error_ptr, align 2
  ret void

exit_overflow:
  %err_ok_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 0
  store i8 0, ptr %err_ok_ptr, align 2
  %err_result_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 1
  store i16 0, ptr %err_result_ptr, align 2
  %err_code_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 2
  store i16 1, ptr %err_code_ptr, align 2
  ret void
}
