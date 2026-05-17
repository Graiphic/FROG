; Example 16 Picture path-to-image kernel boundary.
;
; This LLVM IR is the public native-artifact contract for the lowered FROG unit.
; It does not make the Picture widget an image decoder. The diagram calls the
; manifest-declared frog.image provider primitive, then publishes the resulting
; RGBA8 image buffer to the Picture indicator.

%FrogImageDecodeFileRgba8Result = type opaque

declare i32 @frog_image_decode_file_rgba8(ptr, ptr)
declare void @frog_image_free_decode_file_rgba8_result(ptr)

define i32 @frog_example16_run(ptr %path_utf8_z, ptr %out_result) {
entry:
  %status = call i32 @frog_image_decode_file_rgba8(ptr %path_utf8_z, ptr %out_result)
  ret i32 %status
}

define void @frog_example16_free_decode_file_rgba8_result(ptr %result) {
entry:
  call void @frog_image_free_decode_file_rgba8_result(ptr %result)
  ret void
}
