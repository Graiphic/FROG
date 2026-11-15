# Add — left fold f64

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

L'ordre public FROG est (a+b)+input_3. Avec 10^16, -10^16 puis 1, le résultat vaut 1; réassocier a+(b+input_3) donnerait 0. L'exemple contrôle une vraie réduction, pas seulement une somme mathématique.
Entrées : a (f64) = 1e+16; b (f64) = -1e+16; input_3 (f64) = 1.
Résultats attendus : result (f64) = 1.

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = 1


[Open the actual FROG document](main.frog).
