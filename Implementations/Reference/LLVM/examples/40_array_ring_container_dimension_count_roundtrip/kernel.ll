source_filename = "Examples/40_array_ring_container_dimension_count_roundtrip/main.lowering.json"
target triple = "x86_64-pc-windows-msvc"

%FrogRunResult = type { i8, i16, i16 }

define void @frog_example40_run(i16 %input_value, ptr %out_result) {
entry:
  %ok_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 0
  store i8 1, ptr %ok_ptr, align 2
  %result_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 1
  store i16 %input_value, ptr %result_ptr, align 2
  %error_ptr = getelementptr inbounds %FrogRunResult, ptr %out_result, i32 0, i32 2
  store i16 0, ptr %error_ptr, align 2
  ret void
}
