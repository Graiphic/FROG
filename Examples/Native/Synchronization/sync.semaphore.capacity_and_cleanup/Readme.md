# Semaphore : capacité, acquisition, restitution et fermeture

Semaphore : capacité, acquisition, restitution et fermeture

Obtain crée un handle de capacité 2. Acquire prend une place : il en reste 1. Release restitue exactement une place : il en reste 2. Release Reference ferme le handle ; ce sont quatre opérations distinctes, jamais quatre noms d'une même destruction.

Semaphore : capacité, acquisition, restitution et fermeture

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
capacity (i32) = 2
available_held (i32) = 1
available_restored (i32) = 2
timed_out (bool) = false
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
