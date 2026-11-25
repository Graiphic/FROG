# Search And Replace Regular Expression

With value = "cat", search_string = "(cat|dog)", replace_string = "\\$", error_in = {"status": false, "code": 0, "source": ""}, Run displays result = "$", number_of_replacements = 1, offset_past_replacement = 1, error_out = {"status": false, "code": 0, "source": ""}.

With value = "cat", search_string = "(cat|dog)", replace_string = "\\$", error_in = {"status": false, "code": 0, "source": ""}, Run displays result = "$", number_of_replacements = 1, offset_past_replacement = 1, error_out = {"status": false, "code": 0, "source": ""}.

With value = "cat", search_string = "(cat|dog)", replace_string = "\\$", error_in = {"status": false, "code": 0, "source": ""}, Run displays result = "$", number_of_replacements = 1, offset_past_replacement = 1, error_out = {"status": false, "code": 0, "source": ""}.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
result (string) = "$"
number_of_replacements (i32) = 1
offset_past_replacement (i32) = 1
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":0,"source":"","status":false}


[Open the actual FROG document](search_replace_regular_expression.frog).
