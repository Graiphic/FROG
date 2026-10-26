# Exponential Integral — reference 3

Evaluates E_n(q)=integral(1,infinity,exp(-q*t)/t^n,dt), q=abs(x), with required integer n>=0 and no default order.

Relier les contrôles aux ports nommés, puis lancer Run. Les types et valeurs sont explicites; sélectionner la fiche n'exécute rien.
Entrées : x (f64) = 1; n (i32) = 0.
Résultats attendus : result (f64) = 0.36787944117144233.

Evaluates E_n(q)=integral(1,infinity,exp(-q*t)/t^n,dt), q=abs(x), with required integer n>=0 and no default order.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = 0.36787944117144233


[Open the actual FROG document](main.frog).
