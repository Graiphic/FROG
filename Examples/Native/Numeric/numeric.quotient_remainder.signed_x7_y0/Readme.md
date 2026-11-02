# Quotient & Remainder — signed x7 y0

Floor quotient q=floor(x/y), remainder r=x-y*q in the promoted numeric structure.

q=floor(x/y), r=x-y*q. Historical persisted identifiers are preserved: quotient is output R (remainder), and remainder is output IQ (integer quotient). Labels explain this mismatch without breaking existing wires.
Inputs: x (i32) = 7; y (i32) = 0.
Expected results: quotient (i32) = 7; remainder (i32) = 0.

Floor quotient q=floor(x/y), remainder r=x-y*q in the promoted numeric structure.

Mode: execution. Profile: studio-draft. Capability: explicit private Simulation; not a compiled-native claim.
Expected:
output-quotient (i32) = 7
output-remainder (i32) = 0


[Open the actual FROG document](main.frog).
