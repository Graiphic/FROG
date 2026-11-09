# Semaphore: acquire and clean up despite an error

Semaphore: acquire and clean up despite an error

Error 77 is data. Acquire takes a slot despite that error; Release returns it; Release Reference then closes the handle while preserving the priority error. Status inspects without an upstream error to observe the actual count.

Semaphore: acquire and clean up despite an error

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
capacity (i32) = 1
available_held (i32) = 0
available_restored (i32) = 1
timed_out (bool) = false
error_status (bool) = true
error_code (i32) = 77


[Open the actual FROG document](main.frog).
