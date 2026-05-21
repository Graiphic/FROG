; Example 24 Tree Node Selection Roundtrip native proof kernel.
; The selected Tree numeric node value is copied to the public output so the
; example remains on the manifest-backed native artifact corridor.

%FrogRunResult = type { i8, i16, i16 }

define dso_local void @frog_example24_run(i16 %node_value, ptr %out_result) {
entry:
  %ok_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 0
  store i8 1, ptr %ok_ptr, align 2

  %result_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 1
  store i16 %node_value, ptr %result_ptr, align 2

  %error_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 2
  store i16 0, ptr %error_ptr, align 2
  ret void
}
