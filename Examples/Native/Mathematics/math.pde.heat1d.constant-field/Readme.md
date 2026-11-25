# Heat 1D — solve and render a constant temperature field

Sample x and t from 0 to 1 at three points each. With zero source and temperature 2 at both boundaries and at t=0, the solver and renderer both return a 3 by 3 Array filled with 2.

Sample x and t from 0 to 1 at three points each. With zero source and temperature 2 at both boundaries and at t=0, the solver and renderer both return a 3 by 3 Array filled with 2.

Sample x and t from 0 to 1 at three points each. With zero source and temperature 2 at both boundaries and at t=0, the solver and renderer both return a 3 by 3 Array filled with 2.

Mode: execution. Profile: studio-draft. Capability: studio.native.composed@1.
Expected:
temperature (array<f64,rank=2>) = {"shape":[3,3],"elements":[2,2,2,2,2,2,2,2,2]}
rendered-temperature (array<f64,rank=2>) = {"shape":[3,3],"elements":[2,2,2,2,2,2,2,2,2]}


[Open the actual FROG document](main.frog).
