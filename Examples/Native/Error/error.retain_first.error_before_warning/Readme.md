# Retain First Error: an error takes priority over a warning

Retain First Error: an error takes priority over a warning

The first connector carries warning 41; the second carries error 77. Retain First Error selects the first error, or the first warning only when no error exists. The expected output has status=True and code=77. Its type remains the actual status/code/source cluster.

Retain First Error: an error takes priority over a warning

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
error_status (bool) = true
error_code (i32) = 77


[Open the actual FROG document](main.frog).
