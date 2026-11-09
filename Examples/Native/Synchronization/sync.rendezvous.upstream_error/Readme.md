# Rendezvous: standard error behavior and destruction exception

Rendezvous: standard error behavior and destruction exception

With upstream error 77, Wait does not register and Resize does not apply +1: the threshold remains 2 and waiting=0. Destroy executes despite the error and preserves the priority error. These rules differ from Acquire/Release Semaphore.

Rendezvous: standard error behavior and destruction exception

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
timed_out (bool) = true
size (i32) = 2
waiting (i32) = 0
error_status (bool) = true
error_code (i32) = 77


[Open the actual FROG document](main.frog).
