# Notifier History: return to A without replaying its old signal

Notifier History: return to A without replaying its old signal

B publishes 20, then A publishes 10. The same Wait History visits references A, B, A through an auto-indexed input. Its per-resource history remembers receiving A: values are [10,20,0], with [False,False,True] for timeouts. Returning to A does not replay its old signal, and handles are released after the loop.

Notifier History: return to A without replaying its old signal

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
samples (array<f64>) = [10,20,0]
timeouts (array<bool>) = [false,false,true]
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
