# Fresnel — reference 2

Evaluates normalized C(q)=integral(0,q,cos(pi*t*t/2),dt) and S(q)=integral(0,q,sin(pi*t*t/2),dt).

Relier les contrôles aux ports nommés, puis lancer Run. Les types et valeurs sont explicites; sélectionner la fiche n'exécute rien.
Entrées : x (f64) = -1.0.
Résultats attendus : cosine_integral (f64) = 0.7798934003768229; sine_integral (f64) = 0.43825914739035476.

Evaluates normalized C(q)=integral(0,q,cos(pi*t*t/2),dt) and S(q)=integral(0,q,sin(pi*t*t/2),dt).

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-cosine_integral (f64) = 0.7798934003768229
output-sine_integral (f64) = 0.43825914739035476


[Open the actual FROG document](main.frog).
