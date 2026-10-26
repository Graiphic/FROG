# Sample a surface with parameters

With a=2,b=3, a*x1+b*x2 is sampled at x1=[0,1],x2=[0,2]. Named parameter Records and the rank-two output persist in the actual document.

With a=2,b=3, a*x1+b*x2 is sampled at x1=[0,1],x2=[0,2]. Named parameter Records and the rank-two output persist in the actual document.

With a=2,b=3, a*x1+b*x2 is sampled at x1=[0,1],x2=[0,2]. Named parameter Records and the rank-two output persist in the actual document.

Mode: execution. Profile: studio-draft. Capability: frog.studio.formula.evaluation@1; compiled owned-value service.
Expected:
output-x1_values (array<f64>) = [0,1]
output-x2_values (array<f64>) = [0,2]
output-y_values (array<f64,rank=2>) = {"shape":[2,2],"elements":[0,2,6,8]}
output-error (i32) = 0


[Open the actual FROG document](main.frog).
