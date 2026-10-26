# Max & Min — bounds i32

Compatible comparable x,y -> max/min of the common type.

Les extrêmes entiers restent des carriers entiers exacts, sans passage par DBL. Pour deux zéros égaux, Max et Min conservent tous deux les bits de x, ici -0.
Entrées : x (i32) = -2147483648; y (i32) = 2147483647.
Résultats attendus : max (i32) = 2147483647; min (i32) = -2147483648.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (i32) = 2147483647
output-min (i32) = -2147483648


[Open the actual FROG document](main.frog).
