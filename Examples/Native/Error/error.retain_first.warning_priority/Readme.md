# Retain First : conserver le premier avertissement en l'absence d'erreur

Retain First : conserver le premier avertissement en l'absence d'erreur

Les deux clusters ont status=False et des codes non nuls41 puis42. En l'absence d'erreur, Retain First garde le premier avertissement dans l'ordre des terminaux : status=False, code41 et source first warning. Le cas d'une vraie erreur prioritaire est présenté séparément.

Retain First : conserver le premier avertissement en l'absence d'erreur

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
error_status (bool) = false
error_code (i32) = 41
source (string) = "first warning"


[Open the actual FROG document](main.frog).
