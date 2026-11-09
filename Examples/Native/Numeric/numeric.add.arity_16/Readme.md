# Add — arity 16

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Adds 16 FROG operands in canonical order: a, b, then input_3… when present. This is a left fold. NI Add remains binary; this extensibility is a preserved FROG language rule.
Inputs: a (i32) = 1; b (i32) = 2; input_3 (i32) = 3; input_4 (i32) = 4; input_5 (i32) = 5; input_6 (i32) = 6; input_7 (i32) = 7; input_8 (i32) = 8; input_9 (i32) = 9; input_10 (i32) = 10; input_11 (i32) = 11; input_12 (i32) = 12; input_13 (i32) = 13; input_14 (i32) = 14; input_15 (i32) = 15; input_16 (i32) = 16.
Expected results: result (i32) = 136.

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (i32) = 136


[Open the actual FROG document](main.frog).
