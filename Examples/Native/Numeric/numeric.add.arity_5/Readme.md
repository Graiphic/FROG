# Add — arity 5

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Addition de 5 opérandes FROG dans l'ordre canonique a,b puis input_3… si présents. La réduction est à gauche. NI Add reste binaire; cette extensibilité est une loi FROG conservée.
Entrées : a (i32) = 1; b (i32) = 2; input_3 (i32) = 3; input_4 (i32) = 4; input_5 (i32) = 5.
Résultats attendus : result (i32) = 15.

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (i32) = 15


[Open the actual FROG document](main.frog).
