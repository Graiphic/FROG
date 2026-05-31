; Example 32 FROG native kernel artifact: horizontal 1D Array visible-count property roundtrip.
; The FROG runtime consumes this artifact through native_kernel_manifest.json.
; LLVM remains a backend producer, not the runtime identity.

source_filename = "Examples/32_array_1d_horizontal_visible_count_property_roundtrip/main.lowering.json"
target triple = "x86_64-pc-windows-msvc"

%FrogRunResult = type { i8, i16, i16 }

define void @frog_example32_run(i16 %element_value, ptr %out_result) {
entry:
  %ok_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 0
  store i8 1, ptr %ok_ptr, align 1
  %result_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 1
  store i16 %element_value, ptr %result_ptr, align 2
  %error_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 2
  store i16 0, ptr %error_ptr, align 2
  ret void
}
