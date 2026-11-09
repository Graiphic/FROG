# Step — piecewise 2

Returns 0 for negative x and 1 for positive or zero x.

Windowing/step primitive. Gating Square is periodic with period 2; it never computes the numeric square x*x.
Inputs: x (f64) = 1.
Expected results: result (f64) = 1.

Returns 0 for negative x and 1 for positive or zero x.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = 1


[Open the actual FROG document](main.frog).
