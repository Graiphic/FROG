# Add — scalar array

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

A scalar value applies to each array element. The three expected values are 11,12,13 and the shape remains [3]. This is not broadcasting across axes.
Inputs: a (i32) = 10; b (array<i32>) = [1, 2, 3].
Expected results: result (array<i32>) = [11, 12, 13].

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: explicit private Simulation; not a compiled-native claim.
Expected:
output-result (array<i32>) = [11, 12, 13]


[Open the actual FROG document](main.frog).
