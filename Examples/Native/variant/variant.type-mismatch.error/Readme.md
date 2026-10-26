# Variant type mismatch is a normal error

A boxed String cannot be extracted as I32. The result is the I32 intrinsic default and an error status; no numeric parsing or reinterpretation is attempted. Native and historical Simulation currently have distinct error codes, so this example compares the shared status and data behavior.

A boxed String cannot be extracted as I32. The result is the I32 intrinsic default and an error status; no numeric parsing or reinterpretation is attempted. Native and historical Simulation currently have distinct error codes, so this example compares the shared status and data behavior.

A boxed String cannot be extracted as I32. The result is the I32 intrinsic default and an error status; no numeric parsing or reinterpretation is attempted. Native and historical Simulation currently have distinct error codes, so this example compares the shared status and data behavior.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
result (i32) = 0
error-status (bool) = true


[Open the actual FROG document](main.frog).
