# Override Call Chain

Record the location Count samples in the error source while retaining status false and code zero.

Record the location Count samples in the error source while retaining status false and code zero.

Record the location Count samples in the error source while retaining status false and code zero.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":0,"source":"<3>{\"desc\":\"\",\"loc\":\"Count samples\"}","status":false}


[Open the actual FROG document](override_call_chain.frog).
