# Max & Min — ascending i64

Compatible comparable x,y -> max/min of the common type.

Max/Min conserve les types exacts et publient les deux sorties de la même activation. Le profil authored v2 ignore un seul NaN; le profil explicite historique exact.v1 reste séparé.
Entrées : x (i64) = 2; y (i64) = 7.
Résultats attendus : max (i64) = 7; min (i64) = 2.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (i64) = 7
output-min (i64) = 2


[Open the actual FROG document](main.frog).
