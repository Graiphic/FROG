# Queue: enqueue, dequeue and clean up

Queue: enqueue, dequeue and clean up

Obtain uses the explicit f64 prototype. Enqueue adds 42, Dequeue removes it in FIFO order, then Release closes the reference. The error wire establishes the order of actual effects; timeout=0 never blocks.

Queue: enqueue, dequeue and clean up

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
received (f64) = 42
timed_out (bool) = false
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
