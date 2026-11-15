# Stacked Sequence : diagnostiquer une dépendance circulaire

Stacked Sequence : diagnostiquer une dépendance circulaire

Ce montage reprend le trajet extérieur de l'exemple Flat. Stacked doit disposer de toutes ses entrées avant la première frame et ne publie qu'à sa sortie finale : le trajet constitue donc un vrai cycle. Le Frontend commun doit le refuser lors de la dérivation du FIR avec IR_SEQUENCE_DEPENDENCY_CYCLE, sans produire de FIR exécutable ni réécrire le document. Un test séparé contrôle également la garde indépendante du FIR sur le même graphe canonique.

Stacked Sequence : diagnostiquer une dépendance circulaire

Mode: expected-diagnostic. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
Diagnostic: IR_SEQUENCE_DEPENDENCY_CYCLE

[Open the actual FROG document](main.frog).
