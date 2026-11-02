# Square — array

Numeric scalar, array or recursively numeric cluster; result preserves aggregate structure.

Square is unary and squares each element while preserving rank, shape and I32 representation. The aggregate variant explicitly uses private Simulation.
Inputs: in (array<i32>) = [-3, 0, 4].
Expected results: result (array<i32>) = [9, 0, 16].

Numeric scalar, array or recursively numeric cluster; result preserves aggregate structure.

Mode: execution. Profile: studio-draft. Capability: explicit private Simulation; not a compiled-native claim.
Expected:
output-result (array<i32>) = [9, 0, 16]


[Open the actual FROG document](main.frog).
