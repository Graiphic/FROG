# Max & Min — bounds i64

Compatible comparable x,y -> max/min of the common type.

Les extrêmes entiers restent des carriers entiers exacts, sans passage par DBL. Pour deux zéros égaux, Max et Min conservent tous deux les bits de x, ici -0.
Entrées : x (i64) = -9223372036854775808; y (i64) = 9223372036854775807.
Résultats attendus : max (i64) = 9223372036854775807; min (i64) = -9223372036854775808.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (i64) = 9223372036854775807
output-min (i64) = -9223372036854775808


[Open the actual FROG document](main.frog).
