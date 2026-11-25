# Write Error Code

With code = 101, error_in = {"status": true, "code": 41, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}, Run displays error_out = {"status": true, "code": 101, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}.

With code = 101, error_in = {"status": true, "code": 41, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}, Run displays error_out = {"status": true, "code": 101, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}.

With code = 101, error_in = {"status": true, "code": 41, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}, Run displays error_out = {"status": true, "code": 101, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":101,"source":"<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}","status":true}


[Open the actual FROG document](write_code.frog).
