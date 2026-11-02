# Add — left fold f64

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

The public FROG order is (a+b)+input_3. With 10^16, -10^16, then 1, the result is 1; reassociating as a+(b+input_3) would give 0. The example checks an actual reduction, not merely a mathematical sum.
Inputs: a (f64) = 1e+16; b (f64) = -1e+16; input_3 (f64) = 1.
Expected results: result (f64) = 1.

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = 1


[Open the actual FROG document](main.frog).
