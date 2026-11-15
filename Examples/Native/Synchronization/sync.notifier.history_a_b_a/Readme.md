# Notifier History : revenir à A sans relire son ancien signal

Notifier History : revenir à A sans relire son ancien signal

B publie 20 puis A publie 10. Le même Wait History visite les références A, B, A à travers une entrée auto-indexée. Son historique par ressource conserve la réception de A : les valeurs sont [10,20,0], avec [False,False,True] pour les timeouts. Le retour à A ne rejoue pas son ancien signal, et les handles sont libérés après la boucle.

Notifier History : revenir à A sans relire son ancien signal

Mode: execution. Profile: studio-draft. Capability: windows-x64/structured-program.
Expected:
samples (array<f64>) = [10,20,0]
timeouts (array<bool>) = [false,false,true]
error_status (bool) = false
error_code (i32) = 0


[Open the actual FROG document](main.frog).
