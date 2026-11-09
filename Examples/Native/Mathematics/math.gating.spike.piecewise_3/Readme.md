# Spike — piecewise 3

Returns 1 for 0≤x<1 and 0 elsewhere; a nonperiodic unit rectangular pulse.

Windowing/step primitive. Gating Square is periodic with period 2; it never computes the numeric square x*x.
Inputs: x (f64) = 1.
Expected results: result (f64) = 0.

Returns 1 for 0≤x<1 and 0 elsewhere; a nonperiodic unit rectangular pulse.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = 0


[Open the actual FROG document](main.frog).
