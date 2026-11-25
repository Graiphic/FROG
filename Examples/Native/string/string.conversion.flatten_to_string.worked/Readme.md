# Flatten To String

With anything = true, error_in = {"status": false, "code": 0, "source": ""}, Run displays flattened_string = "\u0001", error_out = {"status": false, "code": 0, "source": ""}.

With anything = true, error_in = {"status": false, "code": 0, "source": ""}, Run displays flattened_string = "\u0001", error_out = {"status": false, "code": 0, "source": ""}.

With anything = true, error_in = {"status": false, "code": 0, "source": ""}, Run displays flattened_string = "\u0001", error_out = {"status": false, "code": 0, "source": ""}.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
flattened_string (string) = "\u0001"
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":0,"source":"","status":false}


[Open the actual FROG document](flatten_to_string.frog).
