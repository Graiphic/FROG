; FROG Example 06 - native Boolean kernel ABI proof artifact
; This module is intended for the compiler-agnostic runtime kernel bridge.
; It exposes a manifest-declared C-compatible entry surface:
;
;   void frog_example06_run(uint8_t input_value, FrogBoolRunResult* out_result)
;
; The runtime consumes this through the native kernel manifest. The runtime does
; not depend on LLVM as its conceptual execution authority.

%FrogBoolRunResult = type { i8, i8, i16 }

define void @frog_example06_run(i8 %input_value, ptr %out_result) {
entry:
  %is_true = icmp ne i8 %input_value, 0
  %result = zext i1 %is_true to i8

  %ok_ptr = getelementptr inbounds %FrogBoolRunResult, ptr %out_result, i32 0, i32 0
  store i8 1, ptr %ok_ptr, align 2
  %result_ptr = getelementptr inbounds %FrogBoolRunResult, ptr %out_result, i32 0, i32 1
  store i8 %result, ptr %result_ptr, align 1
  %error_ptr = getelementptr inbounds %FrogBoolRunResult, ptr %out_result, i32 0, i32 2
  store i16 0, ptr %error_ptr, align 2
  ret void
}
