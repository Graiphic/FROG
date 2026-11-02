# Notifier: Status, Cancel and Release have distinct roles

Notifier: Status, Cancel and Release have distinct roles

Send publishes 42. Status reads 42 without consuming it. Cancel returns 42, then clears the retained signal; Wait with timeout=0 returns its type default, 0, and timed_out=True. Release finds an empty notification, 0, and closes the handle.

Notifier: Status, Cancel and Release have distinct roles

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
latest (f64) = 42
notification (f64) = 0
timed_out (bool) = true
last (f64) = 0
error_status (bool) = false
error_code (i32) = 0
canceled (f64) = 42


[Open the actual FROG document](main.frog).
