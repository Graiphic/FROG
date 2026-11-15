# Read Text : conserver l'erreur amont sans accès disque

Read Text : conserver l'erreur amont sans accès disque

L'erreur 77 arrive avant Read Text. L'opération propage cette même erreur sans ouvrir le chemin, ni afficher un sélecteur. Un défaut de chemin ne doit pas remplacer l'erreur du traitement amont.

Read Text : conserver l'erreur amont sans accès disque

Mode: execution. Profile: studio-draft. Capability: windows/highlight-fileio.
Expected:
cancelled (bool) = false
error_status (bool) = true
error_code (i32) = 77


[Open the actual FROG document](main.frog).
