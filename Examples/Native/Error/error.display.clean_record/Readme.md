# Display Error: inspect an error-free record

Display Error: inspect an error-free record

The record has status=False and code0, with a descriptive source. Display Error returns False,0, the location and an empty description. The default mode shows no dialog for this error-free record. This test does not establish interactive Continue/Stop modes, which require a separate host contract.

Display Error: inspect an error-free record

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
error (bool) = false
code (i32) = 0
location (string) = "example location"
description (string) = ""


[Open the actual FROG document](main.frog).
