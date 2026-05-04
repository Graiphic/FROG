; FROG example 01 - LLVM-native pure addition proof

@fmt_result = private unnamed_addr constant [11 x i8] c"result=%f\0A\00"
@fmt_status = private unnamed_addr constant [11 x i8] c"status=ok\0A\00"

declare i32 @printf(ptr, ...)
declare double @atof(ptr)

define double @frog_example01_add(double %a, double %b) {
entry:
  %result = fadd double %a, %b
  ret double %result
}

define i32 @main(i32 %argc, ptr %argv) {
entry:
  %has_args = icmp sgt i32 %argc, 2
  br i1 %has_args, label %parse_args, label %use_default

parse_args:
  %argv1ptr = getelementptr inbounds ptr, ptr %argv, i64 1
  %argv1 = load ptr, ptr %argv1ptr, align 8
  %a = call double @atof(ptr %argv1)
  %argv2ptr = getelementptr inbounds ptr, ptr %argv, i64 2
  %argv2 = load ptr, ptr %argv2ptr, align 8
  %b = call double @atof(ptr %argv2)
  br label %run

use_default:
  br label %run

run:
  %a_value = phi double [ %a, %parse_args ], [ 2.250000e+00, %use_default ]
  %b_value = phi double [ %b, %parse_args ], [ 3.750000e+00, %use_default ]
  %result = call double @frog_example01_add(double %a_value, double %b_value)

  %fmt_result_ptr = getelementptr inbounds [11 x i8], ptr @fmt_result, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_result_ptr, double %result)

  %fmt_status_ptr = getelementptr inbounds [11 x i8], ptr @fmt_status, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ptr)

  ret i32 0
}
