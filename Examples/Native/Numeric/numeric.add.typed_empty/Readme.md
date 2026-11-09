# Add — typed empty

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

An empty array retains its I32 type and shape [0]. Scalar lifting fabricates no element.
Inputs: a (i32) = 10; b (array<i32>) = [].
Expected results: result (array<i32>) = [].

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: explicit private Simulation; not a compiled-native claim.
Expected:
output-result (array<i32>) = []


[Open the actual FROG document](main.frog).
