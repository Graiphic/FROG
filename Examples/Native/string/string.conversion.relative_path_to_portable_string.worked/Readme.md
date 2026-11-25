# Relative Path To Portable String

With value = "a\\b\\c", Run displays result = "a/b/c", error_out = {"status": false, "code": 0, "source": ""}.

With value = "a\\b\\c", Run displays result = "a/b/c", error_out = {"status": false, "code": 0, "source": ""}.

With value = "a\\b\\c", Run displays result = "a/b/c", error_out = {"status": false, "code": 0, "source": ""}.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
result (string) = "a/b/c"
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":0,"source":"","status":false}


[Open the actual FROG document](relative_path_to_portable_string.frog).
