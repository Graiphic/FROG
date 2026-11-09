# Max & Min — bounds u32

Compatible comparable x,y -> max/min of the common type.

Integer extrema retain exact integer carriers without passing through DBL. For two equal zeros, Max and Min both preserve the bits of x, here -0.
Inputs: x (u32) = 0; y (u32) = 4294967295.
Expected results: max (u32) = 4294967295; min (u32) = 0.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (u32) = 4294967295
output-min (u32) = 0


[Open the actual FROG document](main.frog).
