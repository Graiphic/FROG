# Flat Sequence: publish a completed frame before a wait

Flat Sequence: publish a completed frame before a wait

The first frame publishes 7 before the final frame's infinite wait. Stop withdraws the wait and closes the instance without undoing the already published value. The targeted test checks publication before Abort, then repeats after Restart.

Flat Sequence: publish a completed frame before a wait

Mode: expected-abort. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
early (i64) = 7


[Open the actual FROG document](main.frog).
