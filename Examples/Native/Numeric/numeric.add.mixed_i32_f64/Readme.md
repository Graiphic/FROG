# Add — mixed i32 f64

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Le résolveur promeut I32 et DBL vers DBL. Cette variante est explicitement exécutée en Simulation privée tant que le lowering natif des opérandes mixtes n'est pas admis; aucune troncature ni repli silencieux.
Entrées : a (i32) = 2; b (f64) = 0.5.
Résultats attendus : result (f64) = 2.5.

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: explicit private Simulation; not a compiled-native claim.
Expected:
output-result (f64) = 2.5


[Open the actual FROG document](main.frog).
