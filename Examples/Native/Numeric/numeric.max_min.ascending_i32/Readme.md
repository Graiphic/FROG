# Max & Min — ascending i32

Compatible comparable x,y -> max/min of the common type.

Max/Min preserve exact types and publish both outputs from the same activation. The authored v2 profile ignores a single NaN; the explicit historical exact.v1 profile remains separate.
Inputs: x (i32) = 2; y (i32) = 7.
Expected results: max (i32) = 7; min (i32) = 2.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (i32) = 7
output-min (i32) = 2


[Open the actual FROG document](main.frog).
