# Compare Paths

Compare the paths C:/data and C:/data/logs. The child lies inside the parent and the relative path is logs.

Compare the paths C:/data and C:/data/logs. The child lies inside the parent and the relative path is logs.

Compare the paths C:/data and C:/data/logs. The child lies inside the parent and the relative path is logs.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
relative_path (path) = "logs"
path_2_relative_to_path_1 (bool) = true
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":0,"source":"","status":false}


[Open the actual FROG document](compare.frog).
