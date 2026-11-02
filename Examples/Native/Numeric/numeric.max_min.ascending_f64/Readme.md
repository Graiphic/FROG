# Max & Min — ascending f64

Compatible comparable x,y -> max/min of the common type.

Max/Min preserve exact types and publish both outputs from the same activation. The authored v2 profile ignores a single NaN; the explicit historical exact.v1 profile remains separate.
Inputs: x (f64) = 2; y (f64) = 7.
Expected results: max (f64) = 7; min (f64) = 2.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (f64) = 7
output-min (f64) = 2


[Open the actual FROG document](main.frog).
