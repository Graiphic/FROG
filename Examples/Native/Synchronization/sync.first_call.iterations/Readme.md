# First Call?: first visit, not every iteration

First Call?: first visit, not every iteration

The same First Call? node is visited three times in For. It publishes True, False, False for that execution activity. This is neither an implicit comparison with i=0 nor an initialization barrier. The auto-indexed output retains all three observations.

First Call?: first visit, not every iteration

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
samples (array<bool>) = [true,false,false]


[Open the actual FROG document](main.frog).
