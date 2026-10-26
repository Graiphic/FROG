# File Exists : le chemin vide désigne la liste des lecteurs Windows

File Exists : le chemin vide désigne la liste des lecteurs Windows

Ce test n'accède à aucun chemin utilisateur. Sous Windows, le chemin vide correspond à la liste des lecteurs pour File Exists et renvoie True. Cette règle propre à Exists ne rend pas un chemin vide valide pour lire ou écrire un fichier.

File Exists : le chemin vide désigne la liste des lecteurs Windows

Mode: execution. Profile: studio-draft. Capability: windows/highlight-fileio.
Expected:
exists (bool) = true
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
