# Flat Sequence: an early output feeds a later input

Flat Sequence: an early output feeds a later input

The first frame publishes 7 to an external Increment. Its result 8 becomes an input to the final frame, which increments again and publishes 9. Waiting for all inputs before the first frame would create a false deadlock: each Flat frame requires only its own inputs when entered.

Flat Sequence: an early output feeds a later input

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
result (i64) = 9


[Open the actual FROG document](main.frog).
