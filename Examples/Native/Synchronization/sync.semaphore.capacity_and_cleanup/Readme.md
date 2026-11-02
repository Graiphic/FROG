# Semaphore: capacity, acquisition, release and closure

Semaphore: capacity, acquisition, release and closure

Obtain creates a handle with capacity 2. Acquire takes one slot, leaving 1. Release returns exactly one slot, leaving 2. Release Reference closes the handle; these are four distinct operations, never four names for the same destruction.

Semaphore: capacity, acquisition, release and closure

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
capacity (i32) = 2
available_held (i32) = 1
available_restored (i32) = 2
timed_out (bool) = false
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
