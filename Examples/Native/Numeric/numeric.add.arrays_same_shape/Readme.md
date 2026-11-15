# Add — arrays same shape

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Deux tableaux de même forme s'additionnent élément par élément; aucune concaténation et aucune modification de leurs dimensions.
Entrées : a (array<i32>) = [1, 2, 3]; b (array<i32>) = [10, 20, 30].
Résultats attendus : result (array<i32>) = [11, 22, 33].

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: explicit private Simulation; not a compiled-native claim.
Expected:
output-result (array<i32>) = [11, 22, 33]


[Open the actual FROG document](main.frog).
