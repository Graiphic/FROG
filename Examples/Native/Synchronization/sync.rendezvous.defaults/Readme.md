# Rendezvous: default threshold and zero change

Rendezvous: default threshold and zero change

Create uses initial threshold 2. One arrival with timeout=0 is insufficient and is withdrawn. Resize without size_change uses delta 0 and preserves threshold 2. The waiter count returns to 0. No message is transported.

Rendezvous: default threshold and zero change

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
timed_out (bool) = true
size (i32) = 2
new_size (i32) = 2
waiting (i32) = 0
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
