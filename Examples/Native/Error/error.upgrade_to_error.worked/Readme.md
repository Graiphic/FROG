# Upgrade To Error

With error_in = {"status": false, "code": 41, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}, Run displays error_out = {"status": true, "code": 41, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}.

With error_in = {"status": false, "code": 41, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}, Run displays error_out = {"status": true, "code": 41, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}.

With error_in = {"status": false, "code": 41, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}, Run displays error_out = {"status": true, "code": 41, "source": "<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}"}.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":41,"source":"<3>{\"desc\":\"sensor unplugged\",\"loc\":\"read-sensor\"}","status":true}


[Open the actual FROG document](upgrade_to_error.frog).
