# Case enum : sélection exacte et tunnels

Case enum : sélection exacte et tunnels

Le sélecteur choisit la branche exécutée, indépendamment de la branche affichée. La branche chosen incrémente l'entrée 40 et publie 41 ; l'autre publie 99. Tous les contenus et les producteurs du tunnel de sortie persistent, y compris dans la branche cachée. Les sélecteurs I64/U64 conservent les valeurs exactes ; les enums comparent les IDs de domaine/items.

Case enum : sélection exacte et tunnels

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
result (i64) = 41


[Open the actual FROG document](main.frog).
