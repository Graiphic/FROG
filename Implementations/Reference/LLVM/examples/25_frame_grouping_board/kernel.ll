; Example 25 Frame Grouping Board native proof kernel.
; Frame widgets are support visuals. This kernel only publishes a
; manifest-backed scene_ready bool so the example remains on the native artifact
; corridor without assigning scalar value semantics to frames.

%FrogBoolRunResult = type { i8, i8, i16 }

define dso_local void @frog_example25_run(i8 %scene_ready_request, ptr %out_result) {
entry:
  %ok_ptr = getelementptr inbounds %FrogBoolRunResult, ptr %out_result, i32 0, i32 0
  store i8 1, ptr %ok_ptr, align 2

  %result_ptr = getelementptr inbounds %FrogBoolRunResult, ptr %out_result, i32 0, i32 1
  %normalized = icmp ne i8 %scene_ready_request, 0
  %result = zext i1 %normalized to i8
  store i8 %result, ptr %result_ptr, align 1

  %error_ptr = getelementptr inbounds %FrogBoolRunResult, ptr %out_result, i32 0, i32 2
  store i16 0, ptr %error_ptr, align 2
  ret void
}
