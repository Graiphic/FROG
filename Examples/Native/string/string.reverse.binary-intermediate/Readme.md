# Reverse bytes without repairing UTF-8

Reverse moves bytes, not Unicode characters. Reversing the UTF-8 encoding of é creates bytes 169, 195. The next real function reads this binary intermediate as U8; no invalid UTF-8 string is saved as a literal.

Reverse moves bytes, not Unicode characters. Reversing the UTF-8 encoding of é creates bytes 169, 195. The next real function reads this binary intermediate as U8; no invalid UTF-8 string is saved as a literal.

Reverse moves bytes, not Unicode characters. Reversing the UTF-8 encoding of é creates bytes 169, 195. The next real function reads this binary intermediate as U8; no invalid UTF-8 string is saved as a literal.

Mode: execution. Profile: studio-draft. Capability: frog.studio.string.raw-bytes@1.
Expected:
result (array<u8>) = [169,195]


[Open the actual FROG document](main.frog).
