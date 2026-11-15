# ErfC — reference 3

Evaluates the complementary error function erfc(q)=2/sqrt(pi)*integral(q,infinity,exp(-t*t),dt), directly rather than subtracting rounded erf from 1.

Relier les contrôles aux ports nommés, puis lancer Run. Les types et valeurs sont explicites; sélectionner la fiche n'exécute rien.
Entrées : x (f64) = -0.5.
Résultats attendus : result (f64) = 0.4795001221869535.

Evaluates the complementary error function erfc(q)=2/sqrt(pi)*integral(q,infinity,exp(-t*t),dt), directly rather than subtracting rounded erf from 1.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = 0.4795001221869535


[Open the actual FROG document](main.frog).
