# Max & Min — bounds i64

Compatible comparable x,y -> max/min of the common type.

Integer extrema retain exact integer carriers without passing through DBL. For two equal zeros, Max and Min both preserve the bits of x, here -0.
Inputs: x (i64) = -9223372036854775808; y (i64) = 9223372036854775807.
Expected results: max (i64) = 9223372036854775807; min (i64) = -9223372036854775808.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (i64) = 9223372036854775807
output-min (i64) = -9223372036854775808


[Open the actual FROG document](main.frog).
