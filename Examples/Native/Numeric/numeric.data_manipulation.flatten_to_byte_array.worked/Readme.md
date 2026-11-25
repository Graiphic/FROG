# Flatten To Byte Array

Flatten the U16 value 258 in big-endian order. The two bytes are 1 then 2.

Flatten the U16 value 258 in big-endian order. The two bytes are 1 then 2.

Flatten the U16 value 258 in big-endian order. The two bytes are 1 then 2.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
data_bytes (array<u8>) = [1,2]
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":0,"source":"","status":false}


[Open the actual FROG document](flatten_to_byte_array.frog).
