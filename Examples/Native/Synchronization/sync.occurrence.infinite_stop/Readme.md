# Occurrence: stop an infinite wait

Occurrence: stop an infinite wait

Run registers an actual wait with no deadline; no Set is present. Stop must cancel it immediately, release its ticket and end the generation without publishing a fabricated timed_out. Running the same document again creates a clean, stoppable wait. The test waits for observable registration, not an arbitrary delay.

Occurrence: stop an infinite wait

Mode: expected-abort. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:


[Open the actual FROG document](main.frog).
