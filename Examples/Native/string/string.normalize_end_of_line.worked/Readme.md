# Normalize End Of Line

With value = "A\rB\nC\r\n", Run displays result = "A\r\nB\r\nC\r\n".

With value = "A\rB\nC\r\n", Run displays result = "A\r\nB\r\nC\r\n".

With value = "A\rB\nC\r\n", Run displays result = "A\r\nB\r\nC\r\n".

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
result (string) = "A\u000d\u000aB\u000d\u000aC\u000d\u000a"


[Open the actual FROG document](normalize_end_of_line.frog).
