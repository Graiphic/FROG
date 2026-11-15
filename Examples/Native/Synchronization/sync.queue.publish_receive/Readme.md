# Queue : enfiler, défiler et nettoyer

Queue : enfiler, défiler et nettoyer

Obtain utilise le prototype f64 explicite. Enqueue ajoute 42, Dequeue le retire dans l'ordre FIFO, puis Release ferme la référence. Le fil d'erreur établit l'ordre des effets réels ; timeout=0 ne bloque jamais.

Queue : enfiler, défiler et nettoyer

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
received (f64) = 42
timed_out (bool) = false
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
