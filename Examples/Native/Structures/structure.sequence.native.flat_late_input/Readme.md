# Flat Sequence : une sortie précoce alimente une entrée tardive

Flat Sequence : une sortie précoce alimente une entrée tardive

La première frame publie 7 vers un Increment extérieur. Son résultat 8 devient une entrée de la dernière frame, qui incrémente encore et publie 9. Attendre toutes les entrées avant la première frame créerait un faux blocage : seules les entrées de chaque frame Flat sont requises à son entrée.

Flat Sequence : une sortie précoce alimente une entrée tardive

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
result (i64) = 9


[Open the actual FROG document](main.frog).
