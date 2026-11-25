# Match Regular Expression

With value = "abc", regular_expression = "(Z)", error_in = {"status": false, "code": 0, "source": ""}, Run displays before_match = "abc", whole_match = "", after_match = "", offset_past_match = -1, error_out = {"status": false, "code": 0, "source": ""}, submatch = "".

With value = "abc", regular_expression = "(Z)", error_in = {"status": false, "code": 0, "source": ""}, Run displays before_match = "abc", whole_match = "", after_match = "", offset_past_match = -1, error_out = {"status": false, "code": 0, "source": ""}, submatch = "".

With value = "abc", regular_expression = "(Z)", error_in = {"status": false, "code": 0, "source": ""}, Run displays before_match = "abc", whole_match = "", after_match = "", offset_past_match = -1, error_out = {"status": false, "code": 0, "source": ""}, submatch = "".

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
before_match (string) = "abc"
whole_match (string) = ""
after_match (string) = ""
offset_past_match (i32) = -1
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":0,"source":"","status":false}
submatch (string) = ""


[Open the actual FROG document](match_regular_expression.frog).
