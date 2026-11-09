# Add — arity 2

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Adds 2 FROG operands in canonical order: a, b, then input_3… when present. This is a left fold. NI Add remains binary; this extensibility is a preserved FROG language rule.
Inputs: a (i32) = 1; b (i32) = 2.
Expected results: result (i32) = 3.

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (i32) = 3


[Open the actual FROG document](main.frog).
