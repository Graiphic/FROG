# Read Text : refuser un chemin relatif sans base explicite

Read Text : refuser un chemin relatif sans base explicite

Le chemin relatif est câblé explicitement. FileIoRuntime le refuse par une erreur de données au lieu de lire arbitrairement dans le répertoire courant du processus. Aucun sélecteur n'est ouvert, aucun fichier n'est modifié.

Read Text : refuser un chemin relatif sans base explicite

Mode: execution. Profile: studio-draft. Capability: windows/highlight-fileio.
Expected:
cancelled (bool) = false
error_status (bool) = true
error_code (i32) = 1


[Open the actual FROG document](main.frog).
