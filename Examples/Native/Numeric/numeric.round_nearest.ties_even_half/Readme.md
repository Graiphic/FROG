# Round To Nearest — ties even half

Numeric scalar, array or recursively numeric cluster; result preserves aggregate structure.

Exact ties round to the nearest even integer. The new Studio even64 profile is independent of the process FENV mode; the explicitly saved historical fenv64 profile retains its own meaning.
Inputs: in (f64) = 0.5.
Expected results: result (f64) = 0.0.

Numeric scalar, array or recursively numeric cluster; result preserves aggregate structure.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = 0.0


[Open the actual FROG document](main.frog).
