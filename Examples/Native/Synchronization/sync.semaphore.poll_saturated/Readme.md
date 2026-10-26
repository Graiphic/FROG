# Semaphore : timeout zéro sur une capacité épuisée

Semaphore : timeout zéro sur une capacité épuisée

La capacité par défaut est 1. La première acquisition réussit. La seconde teste immédiatement une ressource saturée : timed_out=True, sans erreur opérationnelle. Seule la place réellement acquise est restituée avant fermeture.

Semaphore : timeout zéro sur une capacité épuisée

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
first_timeout (bool) = false
second_timeout (bool) = true
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
