# Write, read and remove a named attribute

Attach a DBL gain of 2.5 to a Variant containing 12. Read gain, remove it, then look it up again with a default of -1. The second lookup reports that the name is missing.

Attach a DBL gain of 2.5 to a Variant containing 12. Read gain, remove it, then look it up again with a default of -1. The second lookup reports that the name is missing.

Attach a DBL gain of 2.5 to a Variant containing 12. Read gain, remove it, then look it up again with a default of -1. The second lookup reports that the name is missing.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
stored-gain (f64) = 2.5
after-removal (f64) = -1
still-present (bool) = false


[Open the actual FROG document](attributes.frog).
