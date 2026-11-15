# Notifier : Multiple History conserve les identités de source

Notifier : Multiple History conserve les identités de source

Les sources A puis B publient 10 et 20 avant l'attente. Le tableau de références contient [A,B,A] : une source répétée ne crée pas une seconde notification. Avec timeout=0 et ignore_previous=False, les valeurs disponibles sont [10,20] dans l'ordre des sources, sans exiger qu'un nouveau signal arrive après le démarrage de Wait.

Notifier : Multiple History conserve les identités de source

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
notifications (array<f64>) = [10,20]
timed_out (bool) = false
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
