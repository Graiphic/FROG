# Add — mixed i32 f64

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

The resolver promotes I32 and DBL to DBL. This variant explicitly executes in private Simulation until native lowering of mixed operands is admitted; there is no truncation or silent fallback.
Inputs: a (i32) = 2; b (f64) = 0.5.
Expected results: result (f64) = 2.5.

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: explicit private Simulation; not a compiled-native claim.
Expected:
output-result (f64) = 2.5


[Open the actual FROG document](main.frog).
