# Retain First Error : une erreur prime sur un warning

Retain First Error : une erreur prime sur un warning

Le premier connecteur porte un warning 41 ; le deuxième porte une erreur 77. Retain First Error choisit d'abord la première erreur, puis seulement la première alerte s'il n'y a aucune erreur. La sortie attendue a status=True et code=77. Le type reste le vrai cluster status/code/source.

Retain First Error : une erreur prime sur un warning

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
error_status (bool) = true
error_code (i32) = 77


[Open the actual FROG document](main.frog).
