; FROG Example 05 - native kernel ABI proof artifact
; This module is intended for the compiler-agnostic runtime kernel bridge.
; It exposes a manifest-declared C-compatible entry surface:
;
;   FrogRunResult frog_example05_run(uint16_t input_value)
;
; The runtime consumes this through the native kernel manifest. The runtime does
; not depend on LLVM as its conceptual execution authority.

%FrogRunResult = type { i8, i16, i16 }

define %FrogRunResult @frog_example05_run(i16 %input_value) {
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
  %ok0 = insertvalue %FrogRunResult poison, i8 1, 0
  %ok1 = insertvalue %FrogRunResult %ok0, i16 %state_current, 1
  %ok2 = insertvalue %FrogRunResult %ok1, i16 0, 2
  ret %FrogRunResult %ok2

exit_overflow:
  %err0 = insertvalue %FrogRunResult poison, i8 0, 0
  %err1 = insertvalue %FrogRunResult %err0, i16 0, 1
  %err2 = insertvalue %FrogRunResult %err1, i16 1, 2
  ret %FrogRunResult %err2
}
