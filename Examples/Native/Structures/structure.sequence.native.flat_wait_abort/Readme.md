# Flat Sequence : publier une frame terminée avant une attente

Flat Sequence : publier une frame terminée avant une attente

La première frame publie 7 avant l'attente infinie de la dernière frame. Stop retire l'attente et clôt l'instance ; il n'annule pas la valeur déjà publiée. Le test ciblé vérifie cette publication avant Abort, puis recommence après Restart.

Flat Sequence : publier une frame terminée avant une attente

Mode: expected-abort. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
early (i64) = 7


[Open the actual FROG document](main.frog).
