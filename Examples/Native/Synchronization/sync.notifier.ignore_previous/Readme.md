# Notifier: ignore a previous signal without clearing it

Notifier: ignore a previous signal without clearing it

Send publishes 42. Status can read that signal without consuming it. Wait with ignore_previous=True and timeout=0 requests a future signal: timed_out=True and notification=0. Release still returns last_notification=42 because ignoring a signal does not clear the resource.

Notifier: ignore a previous signal without clearing it

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
latest (f64) = 42
notification (f64) = 0
timed_out (bool) = true
last (f64) = 42
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
