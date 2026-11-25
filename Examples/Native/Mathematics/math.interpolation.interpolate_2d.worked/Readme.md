# Interpolate 2D

With x = [0, 2], y = [0, 4], z = 2 x 2 array, method = 1, ntimes = 1, Run displays zi = 3 x 3 array, xi_used = 3 x 3 array, yi_used = 3 x 3 array, error = 0.

With x = [0, 2], y = [0, 4], z = 2 x 2 array, method = 1, ntimes = 1, Run displays zi = 3 x 3 array, xi_used = 3 x 3 array, yi_used = 3 x 3 array, error = 0.

With x = [0, 2], y = [0, 4], z = 2 x 2 array, method = 1, ntimes = 1, Run displays zi = 3 x 3 array, xi_used = 3 x 3 array, yi_used = 3 x 3 array, error = 0.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
zi (array<f64,rank=2>) = {"elements":[1,3,5,21,23,25,41,43,45],"shape":[3,3]}
xi_used (array<f64,rank=2>) = {"elements":[0,1,2,0,1,2,0,1,2],"shape":[3,3]}
yi_used (array<f64,rank=2>) = {"elements":[0,0,0,2,2,2,4,4,4],"shape":[3,3]}
error (i32) = 0


[Open the actual FROG document](interpolate_2d.frog).
