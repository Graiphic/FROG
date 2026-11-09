# Stacked Sequence: do not publish before the final frame

Stacked Sequence: do not publish before the final frame

The first frame computes 7, but that external output remains private during the final frame's infinite wait. Stop must withdraw the wait without publishing a premature result. A new run owns a new wait.

Stacked Sequence: do not publish before the final frame

Mode: expected-abort. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:


[Open the actual FROG document](main.frog).
