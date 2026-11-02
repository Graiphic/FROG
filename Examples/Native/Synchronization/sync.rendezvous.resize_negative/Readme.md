# Rendezvous: decrease the threshold by 1

Rendezvous: decrease the threshold by 1

The initial threshold is 3; size_change is a signed delta, not a new absolute threshold. new_size must equal 3 + size_change. The resource is destroyed after observation.

Rendezvous: decrease the threshold by 1

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
new_size (i32) = 2
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
