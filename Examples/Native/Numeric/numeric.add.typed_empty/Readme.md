# Add — typed empty

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Un tableau vide conserve son type I32 et sa forme [0]. La levée scalaire n'invente aucun élément.
Entrées : a (i32) = 10; b (array<i32>) = [].
Résultats attendus : result (array<i32>) = [].

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: explicit private Simulation; not a compiled-native claim.
Expected:
output-result (array<i32>) = []


[Open the actual FROG document](main.frog).
