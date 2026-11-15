# Max & Min — bounds u32

Compatible comparable x,y -> max/min of the common type.

Les extrêmes entiers restent des carriers entiers exacts, sans passage par DBL. Pour deux zéros égaux, Max et Min conservent tous deux les bits de x, ici -0.
Entrées : x (u32) = 0; y (u32) = 4294967295.
Résultats attendus : max (u32) = 4294967295; min (u32) = 0.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (u32) = 4294967295
output-min (u32) = 0


[Open the actual FROG document](main.frog).
