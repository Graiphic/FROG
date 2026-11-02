# Square — piecewise 3

Returns a numeric 0/1 pulse of period 2: 1 on [2n,2n+1) and 0 elsewhere, also for negative x.

Windowing/step primitive. Gating Square is periodic with period 2; it never computes the numeric square x*x.
Inputs: x (f64) = 2.
Expected results: result (f64) = 1.

Returns a numeric 0/1 pulse of period 2: 1 on [2n,2n+1) and 0 elsewhere, also for negative x.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = 1


[Open the actual FROG document](main.frog).
