# C call: exact i64 scalar

C call: exact i64 scalar

The actual NativeCall node uses the C library shipped with the examples, through a document-relative path. The scalar5 profile passes I32/U32/I64/U64/F64 by value, on thread Any, using the C ABI. param.<id>.in/out IDs are persistent. Arguments retain ABI order even when their visual rows are reversed. Integers never pass through a double.

C call: exact i64 scalar

Mode: execution. Profile: studio-draft. Capability: windows-x64/native-call.scalar5@1.
Expected:
returned (i64) = 9007199254740993
error_status (bool) = false
error_code (i32) = 0
mirror_0 (i64) = 9007199254740993


[Open the actual FROG document](main.frog).
