# String bytes retain UTF-8 and NUL

This is a byte conversion, not Unicode character indexing. é, NUL and A produce bytes 195, 169, 0, 65.

This is a byte conversion, not Unicode character indexing. é, NUL and A produce bytes 195, 169, 0, 65.

This is a byte conversion, not Unicode character indexing. é, NUL and A produce bytes 195, 169, 0, 65.

Mode: execution. Profile: studio-draft. Capability: frog.studio.string.raw-bytes@1.
Expected:
result (array<u8>) = [195,169,0,65]


[Open the actual FROG document](main.frog).
