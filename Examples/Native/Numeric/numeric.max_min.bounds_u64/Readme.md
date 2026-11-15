# Max & Min — bounds u64

Compatible comparable x,y -> max/min of the common type.

Les extrêmes entiers restent des carriers entiers exacts, sans passage par DBL. Pour deux zéros égaux, Max et Min conservent tous deux les bits de x, ici -0.
Entrées : x (u64) = 0; y (u64) = 18446744073709551615.
Résultats attendus : max (u64) = 18446744073709551615; min (u64) = 0.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (u64) = 18446744073709551615
output-min (u64) = 0


[Open the actual FROG document](main.frog).
