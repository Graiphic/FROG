# Retain First: keep the first warning when no error exists

Retain First: keep the first warning when no error exists

Both clusters have status=False and nonzero codes41 then42. With no error, Retain First keeps the first warning in terminal order: status=False, code41 and source first warning. A separate case presents a priority error.

Retain First: keep the first warning when no error exists

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
error_status (bool) = false
error_code (i32) = 41
source (string) = "first warning"


[Open the actual FROG document](main.frog).
