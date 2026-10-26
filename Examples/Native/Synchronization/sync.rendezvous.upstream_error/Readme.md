# Rendezvous : erreur standard et exception de destruction

Rendezvous : erreur standard et exception de destruction

Avec l'erreur amont 77, Wait ne s'inscrit pas et Resize n'applique pas +1 : le seuil reste 2 et waiting=0. Destroy s'exécute malgré l'erreur et conserve l'erreur prioritaire. Ces règles diffèrent de celles d'Acquire/Release Semaphore.

Rendezvous : erreur standard et exception de destruction

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
timed_out (bool) = true
size (i32) = 2
waiting (i32) = 0
error_status (bool) = true
error_code (i32) = 77


[Open the actual FROG document](main.frog).
