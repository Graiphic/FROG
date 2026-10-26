# Occurrence : arrêter une attente infinie

Occurrence : arrêter une attente infinie

Run inscrit une vraie attente sans échéance : aucun Set n'est présent. Stop doit annuler l'attente immédiatement, libérer son ticket et terminer la génération sans publier un faux timed_out. Une nouvelle exécution du même document recrée une attente propre et reste elle aussi arrêtable. Le test attend l'inscription observable, pas un délai arbitraire.

Occurrence : arrêter une attente infinie

Mode: expected-abort. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:


[Open the actual FROG document](main.frog).
