# Read Text: preserve the upstream error without disk access

Read Text: preserve the upstream error without disk access

Error 77 arrives before Read Text. The operation propagates that same error without opening the path or displaying a picker. A path failure must not replace the upstream processing error.

Read Text: preserve the upstream error without disk access

Mode: execution. Profile: studio-draft. Capability: windows/highlight-fileio.
Expected:
cancelled (bool) = false
error_status (bool) = true
error_code (i32) = 77


[Open the actual FROG document](main.frog).
