# Scan From String

With value = "N:3;tail", format_string = "%.;N:%d;", default_value = 1, error_in = {"status": false, "code": 0, "source": ""}, Run displays result = 3, remaining_string = "tail", offset_past_scan = 4, error_out = {"status": false, "code": 0, "source": ""}.

With value = "N:3;tail", format_string = "%.;N:%d;", default_value = 1, error_in = {"status": false, "code": 0, "source": ""}, Run displays result = 3, remaining_string = "tail", offset_past_scan = 4, error_out = {"status": false, "code": 0, "source": ""}.

With value = "N:3;tail", format_string = "%.;N:%d;", default_value = 1, error_in = {"status": false, "code": 0, "source": ""}, Run displays result = 3, remaining_string = "tail", offset_past_scan = 4, error_out = {"status": false, "code": 0, "source": ""}.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
result (i32) = 3
remaining_string (string) = "tail"
offset_past_scan (i32) = 4
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":0,"source":"","status":false}


[Open the actual FROG document](scan_from_string.frog).
