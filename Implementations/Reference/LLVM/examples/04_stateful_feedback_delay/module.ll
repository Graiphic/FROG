; FROG example 04 - LLVM-native explicit delay step proof

@fmt_y = private unnamed_addr constant [20 x i8] c"public_output.y=%f\0A\00"
@fmt_state = private unnamed_addr constant [18 x i8] c"state.delay_1=%f\0A\00"
@fmt_status = private unnamed_addr constant [11 x i8] c"status=ok\0A\00"

declare i32 @printf(ptr, ...)
declare double @atof(ptr)

define double @frog_example04_step(double %state_current, double %x) {
entry:
  %state_next = fadd double %state_current, %x
  ret double %state_next
}

define i32 @main(i32 %argc, ptr %argv) {
entry:
  %has_arg = icmp sgt i32 %argc, 1
  br i1 %has_arg, label %parse_arg, label %use_default

parse_arg:
  %argv1ptr = getelementptr inbounds ptr, ptr %argv, i64 1
  %argv1 = load ptr, ptr %argv1ptr, align 8
  %x = call double @atof(ptr %argv1)
  br label %run

use_default:
  br label %run

run:
  %x_value = phi double [ %x, %parse_arg ], [ 2.500000e+00, %use_default ]
  %state_next = call double @frog_example04_step(double 0.000000e+00, double %x_value)

  %fmt_y_ptr = getelementptr inbounds [20 x i8], ptr @fmt_y, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_y_ptr, double %state_next)

  %fmt_state_ptr = getelementptr inbounds [18 x i8], ptr @fmt_state, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_state_ptr, double %state_next)

  %fmt_status_ptr = getelementptr inbounds [11 x i8], ptr @fmt_status, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ptr)

  ret i32 0
}
