# Max & Min — bounds u64

Compatible comparable x,y -> max/min of the common type.

Integer extrema retain exact integer carriers without passing through DBL. For two equal zeros, Max and Min both preserve the bits of x, here -0.
Inputs: x (u64) = 0; y (u64) = 18446744073709551615.
Expected results: max (u64) = 18446744073709551615; min (u64) = 0.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (u64) = 18446744073709551615
output-min (u64) = 0


[Open the actual FROG document](main.frog).
