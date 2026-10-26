# Quotient & Remainder — signed x7 y0

Floor quotient q=floor(x/y), remainder r=x-y*q in the promoted numeric structure.

q=floor(x/y), r=x-y*q. Les identifiants persistés historiques sont conservés: quotient est la sortie R (reste), remainder est la sortie IQ (quotient entier). Les libellés expliquent ce décalage sans casser les fils existants.
Entrées : x (i32) = 7; y (i32) = 0.
Résultats attendus : quotient (i32) = 7; remainder (i32) = 0.

Floor quotient q=floor(x/y), remainder r=x-y*q in the promoted numeric structure.

Mode: execution. Profile: studio-draft. Capability: explicit private Simulation; not a compiled-native claim.
Expected:
output-quotient (i32) = 7
output-remainder (i32) = 0


[Open the actual FROG document](main.frog).
