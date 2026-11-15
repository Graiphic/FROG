# Clear Error : réinitialiser les trois champs du cluster

Clear Error : réinitialiser les trois champs du cluster

L'erreur d'entrée est {status:True, code:77, source:example operation failed}. Clear Error retourne le record complet {False,0,chaîne vide}, puis Unbundle expose séparément ses trois champs. L'erreur est une valeur typée transportée dans le diagramme, pas une exception C++ ni un arrêt du runtime.

Clear Error : réinitialiser les trois champs du cluster

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
error_status (bool) = false
error_code (i32) = 0
source (string) = ""


[Open the actual FROG document](main.frog).
