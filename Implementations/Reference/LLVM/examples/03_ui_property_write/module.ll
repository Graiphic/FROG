; FROG example 03 - LLVM-native UI property effect proof

@default_value = private unnamed_addr constant [6 x i8] c"Ready\00"
@fmt_widget = private unnamed_addr constant [18 x i8] c"widget=ctrl_gain\0A\00"
@fmt_member = private unnamed_addr constant [19 x i8] c"member=label.text\0A\00"
@fmt_value = private unnamed_addr constant [10 x i8] c"value=%s\0A\00"
@fmt_status = private unnamed_addr constant [11 x i8] c"status=ok\0A\00"

declare i32 @printf(ptr, ...)

define i32 @main(i32 %argc, ptr %argv) {
entry:
  %has_arg = icmp sgt i32 %argc, 1
  br i1 %has_arg, label %parse_arg, label %use_default

parse_arg:
  %argv1ptr = getelementptr inbounds ptr, ptr %argv, i64 1
  %value_arg = load ptr, ptr %argv1ptr, align 8
  br label %run

use_default:
  %default_ptr = getelementptr inbounds [6 x i8], ptr @default_value, i64 0, i64 0
  br label %run

run:
  %value = phi ptr [ %value_arg, %parse_arg ], [ %default_ptr, %use_default ]

  %fmt_widget_ptr = getelementptr inbounds [18 x i8], ptr @fmt_widget, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_widget_ptr)

  %fmt_member_ptr = getelementptr inbounds [19 x i8], ptr @fmt_member, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_member_ptr)

  %fmt_value_ptr = getelementptr inbounds [10 x i8], ptr @fmt_value, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_value_ptr, ptr %value)

  %fmt_status_ptr = getelementptr inbounds [11 x i8], ptr @fmt_status, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ptr)

  ret i32 0
}
