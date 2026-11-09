# Max & Min — nan both

Compatible comparable x,y -> max/min of the common type.

The diagram actually produces NaN with Divide 0/0, then passes it to both. This is neither a non-finite JSON literal nor a prefabricated indicator value. One NaN yields 5 on both outputs; two NaNs yield NaN.
Inputs: x (f64) = 0; y (f64) = 0.
Expected results: max (f64) = NaN; min (f64) = NaN.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (f64) = NaN
output-min (f64) = NaN


[Open the actual FROG document](main.frog).
