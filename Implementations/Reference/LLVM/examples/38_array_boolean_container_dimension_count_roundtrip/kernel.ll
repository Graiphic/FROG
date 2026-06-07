source_filename = "Examples/38_array_boolean_container_dimension_count_roundtrip/main.lowering.json"
target triple = "x86_64-pc-windows-msvc"

%FrogBoolRunResult = type { i8, i8, i16 }

define void @frog_example38_run(i8 %input_value, ptr %out_result) {
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
