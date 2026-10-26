# Notifier : ignorer un signal précédent sans l'effacer

Notifier : ignorer un signal précédent sans l'effacer

Send publie 42. Status peut lire ce signal sans le consommer. Wait avec ignore_previous=True et timeout=0 demande un signal futur : timed_out=True et notification=0. Release retourne pourtant toujours last_notification=42, car ignorer n'efface pas la ressource.

Notifier : ignorer un signal précédent sans l'effacer

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
latest (f64) = 42
notification (f64) = 0
timed_out (bool) = true
last (f64) = 42
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
