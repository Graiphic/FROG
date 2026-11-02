# Fresnel — reference 3

Evaluates normalized C(q)=integral(0,q,cos(pi*t*t/2),dt) and S(q)=integral(0,q,sin(pi*t*t/2),dt).

Connect the controls to the named ports, then select Run. Types and values are explicit; selecting the example does not execute it.
Inputs: x (f64) = -0.5.
Expected results: cosine_integral (f64) = 0.4923442258714464; sine_integral (f64) = 0.06473243285999927.

Evaluates normalized C(q)=integral(0,q,cos(pi*t*t/2),dt) and S(q)=integral(0,q,sin(pi*t*t/2),dt).

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-cosine_integral (f64) = 0.4923442258714464
output-sine_integral (f64) = 0.06473243285999927


[Open the actual FROG document](main.frog).
