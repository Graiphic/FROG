# Read Text: reject a relative path without an explicit base

Read Text: reject a relative path without an explicit base

The relative path is explicitly wired. FileIoRuntime rejects it with a data error instead of reading arbitrarily from the process working directory. No picker opens and no file is modified.

Read Text: reject a relative path without an explicit base

Mode: execution. Profile: studio-draft. Capability: windows/highlight-fileio.
Expected:
cancelled (bool) = false
error_status (bool) = true
error_code (i32) = 1


[Open the actual FROG document](main.frog).
