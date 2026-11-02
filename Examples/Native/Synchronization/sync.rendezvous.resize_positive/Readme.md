# Rendezvous: increase the threshold by 2

Rendezvous: increase the threshold by 2

The initial threshold is 3; size_change is a signed delta, not a new absolute threshold. new_size must equal 3 + size_change. The resource is destroyed after observation.

Rendezvous: increase the threshold by 2

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
new_size (i32) = 5
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
