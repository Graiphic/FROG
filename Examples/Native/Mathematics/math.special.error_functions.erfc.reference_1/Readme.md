# ErfC — reference 1

Evaluates the complementary error function erfc(q)=2/sqrt(pi)*integral(q,infinity,exp(-t*t),dt), directly rather than subtracting rounded erf from 1.

Connect the controls to the named ports, then select Run. Types and values are explicit; selecting the example does not execute it.
Inputs: x (f64) = -2.0.
Expected results: result (f64) = 0.004677734981047266.

Evaluates the complementary error function erfc(q)=2/sqrt(pi)*integral(q,infinity,exp(-t*t),dt), directly rather than subtracting rounded erf from 1.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = 0.004677734981047266


[Open the actual FROG document](main.frog).
