# Add — incompatible shapes

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Negative case: the arrays have shapes [2] and [3]. Native Run and Highlight must report incompatible array dimensions and publish no indicator value.
Inputs: a (array<i32>) = [1, 2]; b (array<i32>) = [3, 4, 5].

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: expected-diagnostic. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; native array shape validation.
Expected:
Diagnostic: native.numeric.incompatible_shape

[Open the actual FROG document](main.frog).
