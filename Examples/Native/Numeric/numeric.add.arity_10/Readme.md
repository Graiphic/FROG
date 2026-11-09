# Add — arity 10

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Adds 10 FROG operands in canonical order: a, b, then input_3… when present. This is a left fold. NI Add remains binary; this extensibility is a preserved FROG language rule.
Inputs: a (i32) = 1; b (i32) = 2; input_3 (i32) = 3; input_4 (i32) = 4; input_5 (i32) = 5; input_6 (i32) = 6; input_7 (i32) = 7; input_8 (i32) = 8; input_9 (i32) = 9; input_10 (i32) = 10.
Expected results: result (i32) = 55.

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (i32) = 55


[Open the actual FROG document](main.frog).
