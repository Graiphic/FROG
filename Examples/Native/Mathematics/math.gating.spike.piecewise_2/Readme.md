# Spike — piecewise 2

Returns 1 for 0≤x<1 and 0 elsewhere; a nonperiodic unit rectangular pulse.

Primitive de fenêtrage/palier. Gating Square est périodique de période 2; elle ne calcule jamais le carré numérique x*x.
Entrées : x (f64) = 0.5.
Résultats attendus : result (f64) = 1.

Returns 1 for 0≤x<1 and 0 elsewhere; a nonperiodic unit rectangular pulse.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = 1


[Open the actual FROG document](main.frog).
