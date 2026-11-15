# Fresnel — reference 3

Evaluates normalized C(q)=integral(0,q,cos(pi*t*t/2),dt) and S(q)=integral(0,q,sin(pi*t*t/2),dt).

Relier les contrôles aux ports nommés, puis lancer Run. Les types et valeurs sont explicites; sélectionner la fiche n'exécute rien.
Entrées : x (f64) = -0.5.
Résultats attendus : cosine_integral (f64) = 0.4923442258714464; sine_integral (f64) = 0.06473243285999927.

Evaluates normalized C(q)=integral(0,q,cos(pi*t*t/2),dt) and S(q)=integral(0,q,sin(pi*t*t/2),dt).

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-cosine_integral (f64) = 0.4923442258714464
output-sine_integral (f64) = 0.06473243285999927


[Open the actual FROG document](main.frog).
