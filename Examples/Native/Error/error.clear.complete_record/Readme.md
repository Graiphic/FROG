# Clear Error: reset all three cluster fields

Clear Error: reset all three cluster fields

The input error is {status:True, code:77, source:example operation failed}. Clear Error returns the complete record {False,0,empty string}, then Unbundle exposes its three fields separately. The error is a typed value transported through the diagram, not a C++ exception or a runtime stop.

Clear Error: reset all three cluster fields

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
error_status (bool) = false
error_code (i32) = 0
source (string) = ""


[Open the actual FROG document](main.frog).
