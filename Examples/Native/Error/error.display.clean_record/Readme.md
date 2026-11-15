# Display Error : inspecter un record sans erreur

Display Error : inspecter un record sans erreur

Le record possède status=False et code0, avec une source descriptive. Display Error restitue False,0, la localisation et une description vide. Le mode par défaut ne présente aucun dialogue pour ce record sans erreur. Ce test n'établit pas les modes interactifs Continue/Stop, qui exigent un contrat d'hôte distinct.

Display Error : inspecter un record sans erreur

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
error (bool) = false
code (i32) = 0
location (string) = "example location"
description (string) = ""


[Open the actual FROG document](main.frog).
