; FROG Example 08 - native Enum kernel ABI proof artifact
; This module is intended for the compiler-agnostic runtime kernel bridge.
; It exposes a manifest-declared C-compatible entry surface:
;
;   void frog_example08_run(uint16_t mode_value, FrogEnumRunResult* out_result)
;
; The runtime consumes this through the native kernel manifest. The runtime does
; not depend on LLVM as its conceptual execution authority.

%FrogEnumRunResult = type { i8, i16, i16 }

define void @frog_example08_run(i16 %mode_value, ptr %out_result) {
entry:
  %valid = icmp ule i16 %mode_value, 2
  br i1 %valid, label %ok, label %invalid_enum_value

ok:
  %ok_ptr = getelementptr inbounds %FrogEnumRunResult, ptr %out_result, i32 0, i32 0
  store i8 1, ptr %ok_ptr, align 2
  %result_ptr = getelementptr inbounds %FrogEnumRunResult, ptr %out_result, i32 0, i32 1
  store i16 %mode_value, ptr %result_ptr, align 2
  %error_ptr = getelementptr inbounds %FrogEnumRunResult, ptr %out_result, i32 0, i32 2
  store i16 0, ptr %error_ptr, align 2
  ret void

invalid_enum_value:
  %err_ok_ptr = getelementptr inbounds %FrogEnumRunResult, ptr %out_result, i32 0, i32 0
  store i8 0, ptr %err_ok_ptr, align 2
  %err_result_ptr = getelementptr inbounds %FrogEnumRunResult, ptr %out_result, i32 0, i32 1
  store i16 0, ptr %err_result_ptr, align 2
  %err_code_ptr = getelementptr inbounds %FrogEnumRunResult, ptr %out_result, i32 0, i32 2
  store i16 1, ptr %err_code_ptr, align 2
  ret void
}
