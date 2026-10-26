# Stacked Sequence : diagnostiquer une dépendance circulaire

Stacked Sequence : diagnostiquer une dépendance circulaire

Ce montage reprend le trajet extérieur de l'exemple Flat. Stacked doit disposer de toutes ses entrées avant la première frame et ne publie qu'à sa sortie finale : le trajet constitue donc un vrai cycle. Le Readiness doit le refuser dès la validation avec diagram.combinational_cycle, avant le FIR et sans réécrire le document. Un test séparé contrôle également la garde indépendante du FIR sur le même graphe canonique.

Stacked Sequence : diagnostiquer une dépendance circulaire

Mode: expected-diagnostic. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
Diagnostic: diagram.combinational_cycle

[Open the actual FROG document](main.frog).
