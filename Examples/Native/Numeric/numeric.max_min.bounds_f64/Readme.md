# Max & Min — bounds f64

Compatible comparable x,y -> max/min of the common type.

Integer extrema retain exact integer carriers without passing through DBL. For two equal zeros, Max and Min both preserve the bits of x, here -0.
Inputs: x (f64) = -0.0; y (f64) = 0.0.
Expected results: max (f64) = -0.0; min (f64) = -0.0.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (f64) = -0.0
output-min (f64) = -0.0


[Open the actual FROG document](main.frog).
