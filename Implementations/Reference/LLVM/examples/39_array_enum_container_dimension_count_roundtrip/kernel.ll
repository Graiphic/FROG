source_filename = "Examples/39_array_enum_container_dimension_count_roundtrip/main.lowering.json"
target triple = "x86_64-pc-windows-msvc"

%FrogEnumRunResult = type { i8, i16, i16 }

define void @frog_example39_run(i16 %input_value, ptr %out_result) {
entry:
  %valid_upper = icmp ule i16 %input_value, 2
  %ok = zext i1 %valid_upper to i8
  %result = select i1 %valid_upper, i16 %input_value, i16 0
  %error = select i1 %valid_upper, i16 0, i16 1

  %ok_ptr = getelementptr inbounds %FrogEnumRunResult, ptr %out_result, i32 0, i32 0
  store i8 %ok, ptr %ok_ptr, align 2
  %result_ptr = getelementptr inbounds %FrogEnumRunResult, ptr %out_result, i32 0, i32 1
  store i16 %result, ptr %result_ptr, align 2
  %error_ptr = getelementptr inbounds %FrogEnumRunResult, ptr %out_result, i32 0, i32 2
  store i16 %error, ptr %error_ptr, align 2
  ret void
}
