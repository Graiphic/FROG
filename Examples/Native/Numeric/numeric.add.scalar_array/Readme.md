# Add — scalar array

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Une valeur scalaire s'applique à chaque élément du tableau. Les trois valeurs attendues sont 11,12,13 et la forme reste [3]. Ceci ne constitue pas du broadcasting entre axes.
Entrées : a (i32) = 10; b (array<i32>) = [1, 2, 3].
Résultats attendus : result (array<i32>) = [11, 12, 13].

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: explicit private Simulation; not a compiled-native claim.
Expected:
output-result (array<i32>) = [11, 12, 13]


[Open the actual FROG document](main.frog).
