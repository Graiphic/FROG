# Max & Min — nan x

Compatible comparable x,y -> max/min of the common type.

Le diagramme produit réellement NaN avec Divide 0/0, puis le transmet à x. Ce n'est pas un littéral JSON non fini ni une valeur d'indicateur préfabriquée. Un seul NaN donne 5 sur les deux sorties; deux NaN donnent NaN.
Entrées : x (f64) = 0; y (f64) = 5.
Résultats attendus : max (f64) = 5; min (f64) = 5.

Compatible comparable x,y -> max/min of the common type.

Mode: execution. Profile: studio-draft. Capability: windows-x64; LLVM 22.1.7; exact declared scalar signature.
Expected:
output-max (f64) = 5
output-min (f64) = 5


[Open the actual FROG document](main.frog).
