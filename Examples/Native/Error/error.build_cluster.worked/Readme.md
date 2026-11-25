# Build Error Cluster

Start with an incoming error 17 from Input sensor, then request creation of error 42. The original error passes through intact.

Start with an incoming error 17 from Input sensor, then request creation of error 42. The original error passes through intact.

Start with an incoming error 17 from Input sensor, then request creation of error 42. The original error passes through intact.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
error_out (cluster<"status":bool,"code":i32,"source":string>) = {"code":17,"source":"Input sensor","status":true}


[Open the actual FROG document](build_cluster.frog).
