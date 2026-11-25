# Filter Error Code

With codes = [41, 43], error_in = {"status": true, "code": 41, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}, Run displays codes_cleared = [41], error_cleared = true, error_out = {"status": false, "code": 0, "source": ""}.

With codes = [41, 43], error_in = {"status": true, "code": 41, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}, Run displays codes_cleared = [41], error_cleared = true, error_out = {"status": false, "code": 0, "source": ""}.

With codes = [41, 43], error_in = {"status": true, "code": 41, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}, Run displays codes_cleared = [41], error_cleared = true, error_out = {"status": false, "code": 0, "source": ""}.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
codes_cleared (array<i32>) = [41]
error_cleared (bool) = true
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":0,"source":"","status":false}


[Open the actual FROG document](filter_code.frog).
