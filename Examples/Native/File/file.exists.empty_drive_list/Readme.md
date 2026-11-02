# File Exists: an empty path denotes the Windows drive list

File Exists: an empty path denotes the Windows drive list

This test accesses no user path. On Windows, an empty path denotes the drive list for File Exists and returns True. This Exists-specific rule does not make an empty path valid for reading or writing a file.

File Exists: an empty path denotes the Windows drive list

Mode: execution. Profile: studio-draft. Capability: windows/highlight-fileio.
Expected:
exists (bool) = true
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
