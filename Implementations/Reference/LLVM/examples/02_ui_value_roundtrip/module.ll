; FROG example 02 - LLVM-native widget value arithmetic proof

@fmt_result = private unnamed_addr constant [28 x i8] c"widget.ind_result.value=%f\0A\00"
@fmt_status = private unnamed_addr constant [11 x i8] c"status=ok\0A\00"

declare i32 @printf(ptr, ...)
declare double @atof(ptr)

define double @frog_example02_widget_add(double %ctrl_a, double %ctrl_b) {
entry:
  %result = fadd double %ctrl_a, %ctrl_b
  ret double %result
}

define i32 @main(i32 %argc, ptr %argv) {
entry:
  %has_args = icmp sgt i32 %argc, 2
  br i1 %has_args, label %parse_args, label %use_default

parse_args:
  %argv1ptr = getelementptr inbounds ptr, ptr %argv, i64 1
  %argv1 = load ptr, ptr %argv1ptr, align 8
  %ctrl_a = call double @atof(ptr %argv1)
  %argv2ptr = getelementptr inbounds ptr, ptr %argv, i64 2
  %argv2 = load ptr, ptr %argv2ptr, align 8
  %ctrl_b = call double @atof(ptr %argv2)
  br label %run

use_default:
  br label %run

run:
  %ctrl_a_value = phi double [ %ctrl_a, %parse_args ], [ 1.000000e+00, %use_default ]
  %ctrl_b_value = phi double [ %ctrl_b, %parse_args ], [ 2.000000e+00, %use_default ]
  %result = call double @frog_example02_widget_add(double %ctrl_a_value, double %ctrl_b_value)

  %fmt_result_ptr = getelementptr inbounds [28 x i8], ptr @fmt_result, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_result_ptr, double %result)

  %fmt_status_ptr = getelementptr inbounds [11 x i8], ptr @fmt_status, i64 0, i64 0
  call i32 (ptr, ...) @printf(ptr %fmt_status_ptr)

  ret i32 0
}
