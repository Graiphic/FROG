# Rendezvous : diminuer le seuil de 1

Rendezvous : diminuer le seuil de 1

Le seuil initial est 3 ; size_change est une variation signée, pas un nouveau seuil absolu. new_size doit valoir 3 + size_change. La ressource est détruite après observation.

Rendezvous : diminuer le seuil de 1

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
new_size (i32) = 2
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
