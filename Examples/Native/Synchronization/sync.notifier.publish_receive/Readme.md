# Notifier : lire le dernier signal et nettoyer

Notifier : lire le dernier signal et nettoyer

Notifier<f64> retains the latest notification, 42. The error wire orders Send before Wait; ignore_previous=False and timeout=0 read that existing signal. Release closes the reference after reading. This behavior does not promise a message queue.

Notifier : lire le dernier signal et nettoyer

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
received (f64) = 42
timed_out (bool) = false
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
