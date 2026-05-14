%FrogStringRunResult = type { i8, i16, i32, [256 x i8] }

define void @frog_example09_run(ptr %input_ptr, i32 %input_len, ptr %out_result) {
entry:
  %too_long = icmp ugt i32 %input_len, 256
  br i1 %too_long, label %error_too_long, label %copy_check

copy_check:
  %i = phi i32 [ 0, %entry ], [ %next, %copy_body ]
  %done = icmp uge i32 %i, %input_len
  br i1 %done, label %ok, label %copy_body

copy_body:
  %source_byte_ptr = getelementptr inbounds i8, ptr %input_ptr, i32 %i
  %source_byte = load i8, ptr %source_byte_ptr, align 1
  %target_byte_ptr = getelementptr inbounds %FrogStringRunResult, ptr %out_result, i32 0, i32 3, i32 %i
  store i8 %source_byte, ptr %target_byte_ptr, align 1
  %next = add i32 %i, 1
  br label %copy_check

ok:
  %ok_ptr = getelementptr inbounds %FrogStringRunResult, ptr %out_result, i32 0, i32 0
  store i8 1, ptr %ok_ptr, align 4
  %error_ptr = getelementptr inbounds %FrogStringRunResult, ptr %out_result, i32 0, i32 1
  store i16 0, ptr %error_ptr, align 2
  %len_ptr = getelementptr inbounds %FrogStringRunResult, ptr %out_result, i32 0, i32 2
  store i32 %input_len, ptr %len_ptr, align 4
  ret void

error_too_long:
  %err_ok_ptr = getelementptr inbounds %FrogStringRunResult, ptr %out_result, i32 0, i32 0
  store i8 0, ptr %err_ok_ptr, align 4
  %err_code_ptr = getelementptr inbounds %FrogStringRunResult, ptr %out_result, i32 0, i32 1
  store i16 1, ptr %err_code_ptr, align 2
  %err_len_ptr = getelementptr inbounds %FrogStringRunResult, ptr %out_result, i32 0, i32 2
  store i32 0, ptr %err_len_ptr, align 4
  ret void
}
