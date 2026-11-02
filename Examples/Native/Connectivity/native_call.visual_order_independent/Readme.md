# C call: visual order differs from ABI order

C call: visual order differs from ABI order

The actual NativeCall node uses the C library shipped with the examples, through a document-relative path. The scalar5 profile passes I32/U32/I64/U64/F64 by value, on thread Any, using the C ABI. param.<id>.in/out IDs are persistent. Arguments retain ABI order even when their visual rows are reversed. Integers never pass through a double.

C call: visual order differs from ABI order

Mode: execution. Profile: studio-draft. Capability: windows-x64/native-call.scalar5@1.
Expected:
returned (i32) = 123
error_status (bool) = false
error_code (i32) = 0
mirror_0 (i32) = 1
mirror_1 (i32) = 2
mirror_2 (i32) = 3


[Open the actual FROG document](main.frog).
