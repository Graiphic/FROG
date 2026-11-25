# Flatten To JSON

With value = -3, error_in = {"status": false, "code": 0, "source": ""}, Run displays result = "-3", error_out = {"status": false, "code": 0, "source": ""}.

With value = -3, error_in = {"status": false, "code": 0, "source": ""}, Run displays result = "-3", error_out = {"status": false, "code": 0, "source": ""}.

With value = -3, error_in = {"status": false, "code": 0, "source": ""}, Run displays result = "-3", error_out = {"status": false, "code": 0, "source": ""}.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
result (string) = "-3"
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":0,"source":"","status":false}


[Open the actual FROG document](flatten_to_json.frog).
