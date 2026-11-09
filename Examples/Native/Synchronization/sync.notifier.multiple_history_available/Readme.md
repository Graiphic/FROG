# Notifier: Multiple History preserves source identities

Notifier: Multiple History preserves source identities

Sources A then B publish 10 and 20 before the wait. The reference array contains [A,B,A]: a repeated source does not create a second notification. With timeout=0 and ignore_previous=False, available values are [10,20] in source order, without requiring a new signal after Wait starts.

Notifier: Multiple History preserves source identities

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
notifications (array<f64>) = [10,20]
timed_out (bool) = false
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
