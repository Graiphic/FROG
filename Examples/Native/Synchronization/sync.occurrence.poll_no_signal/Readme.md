# Occurrence : timeout zéro sans signal

Occurrence : timeout zéro sans signal

Le Generate crée l'occurrence vivante ; aucun Set n'est présent. Le Wait utilise timeout=0 et ignore_previous=True par défaut. Il retourne immédiatement timed_out=True et ne conserve aucun message fictif.

Occurrence : timeout zéro sans signal

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
timed_out (bool) = true


[Open the actual FROG document](main.frog).
