# Exponential Integral — reference 1

Evaluates E_n(q)=integral(1,infinity,exp(-q*t)/t^n,dt), q=abs(x), with required integer n>=0 and no default order.

Relier les contrôles aux ports nommés, puis lancer Run. Les types et valeurs sont explicites; sélectionner la fiche n'exécute rien.
Entrées : x (f64) = 0.10000000000000001; n (i32) = 0.
Résultats attendus : result (f64) = 9.0483741803595947.

Evaluates E_n(q)=integral(1,infinity,exp(-q*t)/t^n,dt), q=abs(x), with required integer n>=0 and no default order.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = 9.0483741803595947


[Open the actual FROG document](main.frog).
