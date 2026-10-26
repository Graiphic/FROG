# Round To Nearest — ties even negative one half

Numeric scalar, array or recursively numeric cluster; result preserves aggregate structure.

Une égalité exacte se départage vers l'entier pair. Le nouveau profil Studio even64 est indépendant du mode FENV du processus; le profil historique fenv64 explicitement enregistré garde son sens propre.
Entrées : in (f64) = -1.5.
Résultats attendus : result (f64) = -2.0.

Numeric scalar, array or recursively numeric cluster; result preserves aggregate structure.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = -2.0


[Open the actual FROG document](main.frog).
