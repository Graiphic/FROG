# Notifier : Status, Cancel et Release ont des rôles différents

Notifier : Status, Cancel et Release ont des rôles différents

Send publie 42. Status lit 42 sans consommer. Cancel retourne 42 puis efface le signal retenu ; Wait avec timeout=0 retourne sa valeur de type par défaut 0 et timed_out=True. Release retrouve une notification vide 0 et ferme le handle.

Notifier : Status, Cancel et Release ont des rôles différents

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
latest (f64) = 42
notification (f64) = 0
timed_out (bool) = true
last (f64) = 0
error_status (bool) = false
error_code (i32) = 0
canceled (f64) = 42


[Open the actual FROG document](main.frog).
