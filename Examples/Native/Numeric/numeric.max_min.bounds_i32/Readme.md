# Max & Min — bounds i32

Compatible comparable x,y -> max/min of the common type.

Integer extrema retain exact integer carriers without passing through DBL. For two equal zeros, Max and Min both preserve the bits of x, here -0.
Inputs: x (i32) = -2147483648; y (i32) = 2147483647.
Expected results: max (i32) = 2147483647; min (i32) = -2147483648.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (i32) = 2147483647
output-min (i32) = -2147483648


[Open the actual FROG document](main.frog).
