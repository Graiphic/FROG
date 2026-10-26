# Max & Min — bounds f64

Compatible comparable x,y -> max/min of the common type.

Les extrêmes entiers restent des carriers entiers exacts, sans passage par DBL. Pour deux zéros égaux, Max et Min conservent tous deux les bits de x, ici -0.
Entrées : x (f64) = -0.0; y (f64) = 0.0.
Résultats attendus : max (f64) = -0.0; min (f64) = -0.0.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (f64) = -0.0
output-min (f64) = -0.0


[Open the actual FROG document](main.frog).
