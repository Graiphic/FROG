# Max & Min — ascending u32

Compatible comparable x,y -> max/min of the common type.

Max/Min preserve exact types and publish both outputs from the same activation. The authored v2 profile ignores a single NaN; the explicit historical exact.v1 profile remains separate.
Inputs: x (u32) = 2; y (u32) = 7.
Expected results: max (u32) = 7; min (u32) = 2.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (u32) = 7
output-min (u32) = 2


[Open the actual FROG document](main.frog).
