# Variant preserves an Array shape

Boxing and unboxing retain the complete rank-two I64 Array shape and row-major values, including an integer above 2^53. The type specimen contributes its schema, not its value.

Boxing and unboxing retain the complete rank-two I64 Array shape and row-major values, including an integer above 2^53. The type specimen contributes its schema, not its value.

Boxing and unboxing retain the complete rank-two I64 Array shape and row-major values, including an integer above 2^53. The type specimen contributes its schema, not its value.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
result (array<i64,rank=2>) = {"shape":[2,2],"elements":[1,9007199254740993,-4,7]}


[Open the actual FROG document](main.frog).
