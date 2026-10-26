# Sample a two-variable surface

For 100*x1+x2, rows follow x2 and columns follow x1 in the explicit FROG v1 layout. The output is a rank-two Array, distinct from a Matrix.

For 100*x1+x2, rows follow x2 and columns follow x1 in the explicit FROG v1 layout. The output is a rank-two Array, distinct from a Matrix.

For 100*x1+x2, rows follow x2 and columns follow x1 in the explicit FROG v1 layout. The output is a rank-two Array, distinct from a Matrix.

Mode: execution. Profile: studio-draft. Capability: frog.studio.formula.evaluation@1; compiled owned-value service.
Expected:
output-x1_values (array<f64>) = [0,1,2]
output-x2_values (array<f64>) = [10,20,30]
output-y_values (array<f64,rank=2>) = {"shape":[3,3],"elements":[10,110,210,20,120,220,30,130,230]}
output-error (i32) = 0


[Open the actual FROG document](main.frog).
