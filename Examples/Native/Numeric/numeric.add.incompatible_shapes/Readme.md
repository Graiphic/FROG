# Add — incompatible shapes

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Negative case: the arrays have shapes [2] and [3]. Private Simulation must report exactly Incompatible aggregate shapes or Cluster fields and publish no indicator value. This is a private-engine message, not an invented public code. No silent broadcasting is allowed.
Inputs: a (array<i32>) = [1, 2]; b (array<i32>) = [3, 4, 5].
Expected results: .

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: expected-diagnostic. Profile: studio-draft. Capability: explicit private Simulation; not a compiled-native claim.
Expected:
Diagnostic: Highlight Execution: operation (numeric.add): Incompatible aggregate shapes or Cluster fields

[Open the actual FROG document](main.frog).
