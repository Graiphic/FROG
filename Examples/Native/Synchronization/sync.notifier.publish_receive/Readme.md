# Notifier : lire le dernier signal et nettoyer

Notifier : lire le dernier signal et nettoyer

Le Notifier<f64> garde la dernière notification 42. Send précède Wait par le fil d'erreur ; ignore_previous=False et timeout=0 lisent ce signal existant. Release ferme la référence après lecture. Ce comportement ne promet pas une file de messages.

Notifier : lire le dernier signal et nettoyer

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
received (f64) = 42
timed_out (bool) = false
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
