# Exponential Integral — reference 1

Evaluates E_n(q)=integral(1,infinity,exp(-q*t)/t^n,dt), q=abs(x), with required integer n>=0 and no default order.

Connect the controls to the named ports, then select Run. Types and values are explicit; selecting the example does not execute it.
Inputs: x (f64) = 0.10000000000000001; n (i32) = 0.
Expected results: result (f64) = 9.0483741803595947.

Evaluates E_n(q)=integral(1,infinity,exp(-q*t)/t^n,dt), q=abs(x), with required integer n>=0 and no default order.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-result (f64) = 9.0483741803595947


[Open the actual FROG document](main.frog).
