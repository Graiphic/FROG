# Add — incompatible shapes

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Cas négatif: les tableaux ont des formes [2] et [3]. La Simulation privée doit produire exactement Incompatible aggregate shapes or Cluster fields et ne publier aucune valeur d'indicateur. Ce message est celui du moteur privé, pas un code public inventé. Aucun broadcasting silencieux n'est admis.
Entrées : a (array<i32>) = [1, 2]; b (array<i32>) = [3, 4, 5].
Résultats attendus : .

Adds connected operands in canonical port order with a canonical left fold after coercion to the resolved result leaf type.

Mode: expected-diagnostic. Profile: studio-draft. Capability: explicit private Simulation; not a compiled-native claim.
Expected:
Diagnostic: Highlight Execution: operation (numeric.add): Incompatible aggregate shapes or Cluster fields

[Open the actual FROG document](main.frog).
