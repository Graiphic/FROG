# Semaphore: zero timeout with exhausted capacity

Semaphore: zero timeout with exhausted capacity

The default capacity is 1. The first acquisition succeeds. The second immediately tests a saturated resource: timed_out=True, without an operational error. Only the slot actually acquired is returned before closing.

Semaphore: zero timeout with exhausted capacity

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
first_timeout (bool) = false
second_timeout (bool) = true
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
