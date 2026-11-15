# Semaphore : acquérir et nettoyer malgré une erreur

Semaphore : acquérir et nettoyer malgré une erreur

L'erreur 77 est une donnée. Acquire retire une place malgré cette erreur ; Release la restitue ; Release Reference ferme ensuite le handle tout en conservant l'erreur prioritaire. Status inspecte sans erreur amont pour observer le vrai compteur.

Semaphore : acquérir et nettoyer malgré une erreur

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
capacity (i32) = 1
available_held (i32) = 0
available_restored (i32) = 1
timed_out (bool) = false
error_status (bool) = true
error_code (i32) = 77


[Open the actual FROG document](main.frog).
